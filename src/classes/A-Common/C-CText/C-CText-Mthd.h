/*****************************************************************************
 * Find a particular select region from the stack of them.                   *
 *****************************************************************************/

static SelReg *SelRegNum(SW_IDATA *idata, LONG x)
{
   if (x < 0 || x > idata->SelRegCnt) return NULL;

   return idata->SelReg + x;
}

static SelReg *TopSel(SW_IDATA *idata)
{
   return SelRegNum(idata, idata->SelRegCnt);
}


/*****************************************************************************
 * Function: Remove highlight box                                            *
 *****************************************************************************/

static void RemHL(Object *o, SW_IDATA *idata, SelReg *sr)
{
   sr->Lp = sr->L0;
   sr->Xp = sr->X0;

   idata->SR_Draw = sr;
   MUI_Redraw(o, MADF_DRAWUPDATE);
   
   sr->L1 = -1;
}


/*****************************************************************************
 * Function: compare two drag positions                                      *
 *****************************************************************************/

static __inline LONG CmpDragPos(LONG L1, LONG x1, LONG L0, LONG x0)
{
   return (L1 - L0) ? (L1 - L0) : (x1 - x0);
}


/*****************************************************************************
 * Function: Compare select region drag positions                            *
 *****************************************************************************/

static LONG CmpSelRegDragPos(SelReg *sr)
{
   return CmpDragPos(sr->L1, sr->X1, sr->L0, sr->X0);
}


/*****************************************************************************
 * Function: Push a select region onto the stack                             *
 *****************************************************************************/

static SelReg *PushSelReg(Object *o, SW_IDATA *idata)
{
   SelReg *sr;
   
   if ((idata->SelRegCnt+1) >= idata->SelRegMax) {
      idata->SelRegMax += 2;

      if (RV(idata->SelRegMax * sizeof(*sr), &idata->SelReg) < 0)
         return NULL;
   }

   set(o, SWA_CText_NumSelections, ++idata->SelRegCnt + 1);

   if (!(sr = TopSel(idata)))
      return NULL;
   
   memset(sr, 0, sizeof(*sr));
   
   sr->L0 = sr->L1 = -1;

   return sr;
}


/*****************************************************************************
 * Function: Remove a given select region                                    *
 *****************************************************************************/

static void RemSelReg(Object *o, SW_IDATA *idata, LONG Num)
{
   SelReg *sr;

   if (!(sr = SelRegNum(idata, Num)))
      return;

   // -- remove it`s hilight -------------------------------------------------
   
   sr->Lp = sr->L0,
   sr->Xp = sr->X0;

   idata->SR_Draw = sr;
   MUI_Redraw(o, MADF_DRAWUPDATE);

   // -- remove it from the region list --------------------------------------

   if (Num != idata->SelRegCnt)
      memcpy(idata->SelReg+Num, idata->SelReg+(Num+1),
             sizeof(*sr) * (idata->SelRegCnt - Num));

   set(o, SWA_CText_NumSelections, --idata->SelRegCnt + 1);
}


/*****************************************************************************
 * Function: Pop a select region from the stack                              *
 *****************************************************************************/

static void PopSelReg(Object *o, SW_IDATA *idata)
{
   RemSelReg(o, idata, idata->SelRegCnt);
}


/*****************************************************************************
 * Function: get begin/end of region, handling drag directions               *
 *****************************************************************************/

static void GetSelRegBounds(SelReg *sr,
                            LONG *L0, LONG *L1,
                            LONG *C0, LONG *C1)
{
   if (CmpSelRegDragPos(sr) < 0) {
      *L0 = sr->L1,
      *L1 = sr->L0,
      *C0 = sr->C1,
      *C1 = sr->C0-1;
   } else {
      *L0 = sr->L0,
      *L1 = sr->L1,
      *C0 = sr->C0,
      *C1 = sr->C1;
   }
}


/*****************************************************************************
 * Function: Put it in forwards order, if it was select backwards            *
 *****************************************************************************/

static void NormalizeSelReg(SelReg *sr)
{
   LONG t;

   if (CmpSelRegDragPos(sr) < 0) {
      t = sr->L0, sr->L0=sr->L1, sr->L1 = t;
      t = sr->C0, sr->C0=sr->C1, sr->C1 = t;
      t = sr->X0, sr->X0=sr->X1, sr->X1 = t;
   }
}


/*****************************************************************************
 * normalize all select regions                                              *
 *****************************************************************************/

static void NormalizeSelRegAll(SW_IDATA *idata)
{
   LONG x;

   for (x=0; x<=idata->SelRegCnt; x++)
      NormalizeSelReg(SelRegNum(idata, x));
}


/*****************************************************************************
 * Function: sort select regions (must be normalized!)                       *
 *****************************************************************************/

static void SortSelReg(SW_IDATA *idata)
{
   LONG x, Change = TRUE;
   SelReg tmp;

   // -- simple, dumb, bubble sort, coz we probably only have a few regions --
   
   while (Change) {
      for (x=0, Change=FALSE; x<idata->SelRegCnt; x++) {
         
         if (CmpDragPos(idata->SelReg[x].L0,   idata->SelReg[x].C0,
                        idata->SelReg[x+1].L0, idata->SelReg[x+1].C0) > 0) {
            Change = TRUE;

            memcpy(&tmp, idata->SelReg+x, sizeof(tmp));
            memcpy(idata->SelReg+x, idata->SelReg+(x+1), sizeof(tmp));
            memcpy(idata->SelReg+(x+1), &tmp, sizeof(tmp));
         }
      }
   }
}


/*****************************************************************************
 * Function: See if select region overlaps with another one                  *
 *****************************************************************************/

static LONG SelRegOverlap(SelReg *sr0, SelReg *sr1)
{
   LONG L00, L01, L10, L11, C00, C01, C10, C11;
   
   if (!sr0 || !sr1 || sr0 == sr1 ||
       sr0->L0 < 0 || sr0->L1 < 0 ||
       sr1->L0 < 0 || sr1->L1 < 0) return FALSE;

   GetSelRegBounds(sr0, &L00, &L01, &C00, &C01);
   GetSelRegBounds(sr1, &L10, &L11, &C10, &C11);

   return !(CmpDragPos(L00, C00, L11, C11) > 0 ||
            CmpDragPos(L01, C01, L10, C10) < 0);
}


/*****************************************************************************
 * Function: Remove overlapping select regions                               *
 *****************************************************************************/

static void RemSelOverlap(Object *o, SW_IDATA *idata, LONG Num)
{
   LONG x;

   for (x=0; x<=idata->SelRegCnt; )
      if (SelRegOverlap(SelRegNum(idata, Num), SelRegNum(idata, x))) {
         DoMethod(idata->DragGrp, SWM_CText_Unsel, x);
         if (Num > x) Num--;
      } else x++;
}


/*****************************************************************************
 * Function: Find next control chr in string                                 *
 *****************************************************************************/

static STRPTR NextCtrl(STRPTR s)
{
   for (; *s; s++)
      if (CT_IsCtrl(s))
         return s;

   return NULL;
}


/*****************************************************************************
 * Function: Find strlen, ignoring control crap                              *
 *****************************************************************************/

static ULONG CT_StrLen(STRPTR s)
{
   ULONG len = 0;
   
   for (; *s; s++)
      if (CT_IsCtrl(s)) s = strchr(s, CT_End);
      else len++;

   return len;
}


/*****************************************************************************
 * Function: draw highlight box                                              *
 *****************************************************************************/

static void DrawHL(Object *o, SW_IDATA *idata, LONG L0, LONG L1,
                   LONG x0, LONG x1)
{
   LONG Dir, t;
   ULONG x, ypos, xpos, xnext, ynext, OldMode;

   if (L0 < 0 || L1 < 0) return;
   if (!(Dir = CmpDragPos(L1, x1, L0, x0))) return;

   if (Dir < 0)
      t=L1,L1=L0,L0=t,
      t=x1,x1=x0,x0=t;
      
   OldMode = GetDrMd(_rp(o));

   SetDrMd(_rp(o), COMPLEMENT);

   for (ypos = _mtop(o), x=0; CTxt(x); x++, ypos = ynext) {
      ynext = ypos + CT_LineSp(idata, x);
      if (x < L0) continue;
      if (x > L1) break;

      xpos  = (x==L0 ? x0  : _mleft(o));
      xnext = (x==L1 ? x1-1: _mleft(o) + idata->LineInfo[x].LineLens);
   
      RectFill(_rp(o), xpos, ypos, xnext,
               ynext-idata->SWA_CText_LineSpacing-1);
   }

   SetDrMd(_rp(o), OldMode);
}


/*****************************************************************************
 * Method: Draw object                                                       *
 *****************************************************************************/

METHOD(MUIM_Draw, struct MUIP_Draw *)
{
   ULONG FPos  = 0, PPos = 0, SPos = 0, ypos = _mtop(o), xpos;
   LONG x;
   STRPTR s, prev;
   SelReg *sr;
   
   DoSuperMethodA(cl,o,msg);       // let parent draw self

   if (!(msg->flags & (MADF_DRAWOBJECT | MADF_DRAWUPDATE))) SWDB_RET(0);

   SetDrMd(_rp(o), JAM1);
   SetAPen(_rp(o), MUIPEN(idata->FontInfo[PPos].Pens));
   SetFont(_rp(o), idata->FontInfo[FPos].Fonts);
   SetSoftStyle(_rp(o), idata->FontInfo[FPos].FontStyle, FStyles);

   // -- draw the text itself ------------------------------------------------
   if ((msg->flags & MADF_DRAWOBJECT) || !CT_HasSelRegion(idata)) {
      for (x=0; CTxt(x); x++) {
         ypos      += idata->LineInfo[x].AboveBase;
         xpos       = _mleft(o);

         for (s = prev = CTxt(x); s && *s; prev = s) {
            for (; *s && !CT_IsCtrl(s); s++);
            
            if (s != prev) {
               Move(_rp(o), xpos, ypos);
               Text(_rp(o), prev, s - prev);
            }
   
            if (*s) {
               if (CT_IsFont(s)) {
                  SetFont(_rp(o), idata->FontInfo[++FPos].Fonts);
                  SetSoftStyle(_rp(o), idata->FontInfo[FPos].FontStyle, FStyles);
               } else if (CT_IsPen(s))
                  SetAPen(_rp(o), MUIPEN(idata->FontInfo[++PPos].Pens));
   
               xpos += idata->SegLens[SPos++];
               s = strchr(s, CT_End);
               if (s) s++;
            }
         }
   
         ypos += idata->LineInfo[x].BelowBase + idata->SWA_CText_LineSpacing;
      }

      for (x=0; x<=idata->SelRegCnt; x++)
         if (sr = SelRegNum(idata, x))
            DrawHL(o, idata, sr->L0, sr->L1, sr->X0, sr->X1);
   }

   // -- draw the highlighting for selected text -----------------------------

   if (msg->flags & MADF_DRAWUPDATE)
      if (sr = idata->SR_Draw)
         DrawHL(o, idata, sr->Lp, sr->L1, sr->Xp, sr->X1);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: Count ctrls in all lines                                        *
 *****************************************************************************/

static ULONG CountCtrl(SW_IDATA *idata, STRPTR Type)
{
   ULONG  x, v, len = strlen(Type);
   STRPTR s;
   UBYTE  c = Type[0];

   for (x=v=0; CTxt(x); x++)
      for (s=CTxt(x); s = NextCtrl(s); s+=CT_Skip)
         if (c == *s && strncmp(s, Type, len) == 0)
            v++;

   return v;
}


/*****************************************************************************
 * Function: allocate pen color                                              *
 *****************************************************************************/

static ULONG AllocPens(Object *o, SW_IDATA *idata)
{
   STRPTR s, es;
   ULONG  x, v, count=0, spos=0;

   idata->FontInfo[count].Pens =
      MUI_ObtainPen(muiRenderInfo(o), &idata->SWA_CText_PopPens[CTP_DefPen], 0);
   idata->FontInfo[count++].PenAlloc = TRUE;
   
   for (x=0; CTxt(x); x++) {
      for (s=CTxt(x); s = NextCtrl(s); s+=CT_Skip)
         if (CT_IsPen(s)) {
            if (CT_IsEmpty(s)) {
               if (spos) --spos;
               idata->FontInfo[count].Pens     = idata->FontInfo[spos].Pens;
               idata->FontInfo[count].PenAlloc = FALSE;
            } else {
               for (es=s+CT_Skip, v=0; *es && *es != CT_End; es++)
                  v = v*16 + avtod(*es);

               idata->FontInfo[count].Pens =
                  MUI_ObtainPen(muiRenderInfo(o), (struct MUI_PenSpec *)v, 0);
               idata->FontInfo[count].PenAlloc = TRUE;
               spos = count;
            }

            count++;
         }
   }

   return SUCCESS;
}


/*****************************************************************************
 * Function: Parse font name/size/style                                      *
 *****************************************************************************/

static void ParseFontSpec(STRPTR spec, STRPTR Name, USHORT *Size, UBYTE *Style)
{
   UBYTE  c, c0;
   STRPTR sp, p0;

   if (p0 = strchr(spec, CT_End))
      c0 = *p0, *p0 = '\0';

   if (Size)   *Size  = 8;
   if (Style)  *Style = FS_NORMAL;
   if (Name)   *Name  = '\0';
   
   if (sp = strchr(spec, '/'))      c = *sp, *sp = '\0';
   if (Name && !SW_STRISNULL(spec)) strcat(strcpy(Name, spec), ".font");
   if (sp)                          *sp = c;

   if (!sp) goto Done;

   if (sp = strchr(spec = sp+1, '/')) c = *sp, *sp = '\0';
   if (Size && !SW_STRISNULL(spec))   *Size = atol(spec);
   if (sp)                            *sp = c;

   if (!sp) goto Done;

   spec = sp + 1;
   if (Style) *Style = (strpbrk(spec,"bB") ? FSF_BOLD       : 0) |
                       (strpbrk(spec,"uU") ? FSF_UNDERLINED : 0) |
                       (strpbrk(spec,"iI") ? FSF_ITALIC     : 0);

  Done:
   if (p0) *p0 = c0;
}


/*****************************************************************************
 * Function: Find width of line                                              *
 *****************************************************************************/

static ULONG LineHW(SW_IDATA *idata, ULONG Line,
                    ULONG *Pos, ULONG *SPos,
                    struct RastPort *rp, ULONG *Height)
{
   ULONG  Length = 0, tl;
   USHORT ma = idata->FontInfo[*Pos].Fonts->tf_Baseline,
          mb = idata->FontInfo[*Pos].Fonts->tf_YSize - ma,
          a, b;
   STRPTR prev = CTxt(Line),
          s;

   if (!Pos || !SPos) return 50;

   do {
      if (!(s = NextCtrl(prev)))
         s = prev + strlen(prev);

      tl = 0;
      if (s != prev) 
         Length += (tl = TextLength(rp, prev, s - prev));

      if (CT_IsFont(s)) {
         SetFont(rp,      idata->FontInfo[++(*Pos)].Fonts);
         SetSoftStyle(rp, idata->FontInfo[*Pos].FontStyle, FStyles);

         a     = idata->FontInfo[*Pos].Fonts->tf_Baseline;
         b     = idata->FontInfo[*Pos].Fonts->tf_YSize - a,
         ma    = max(ma, a);
         mb    = max(mb, b);
      }

      if (*s) {
         idata->SegLens[(*SPos)++] = tl;
         s = strchr(s, CT_End);
         if (s) s++;
      }
      
      prev = s;
   } while (s && *s);

   *Height = (idata->LineInfo[Line].AboveBase = ma) +
             (idata->LineInfo[Line].BelowBase = mb);

   return Length;
}


/*****************************************************************************
 * Function: open a font, or topaz 8 if unable                               *
 *****************************************************************************/

LONG FontTabLast        = 0;
struct TextFont *Topaz8 = NULL;

static struct {
   TEXT Name[MAXFONTNAME+2];
   UWORD YSize;
   UBYTE Style;
   UBYTE Flags;
   struct TextFont *tf;
   ULONG  UseCount;
} FontTab[MAXFONTNAME];

static struct TextFont *GetFont(struct TextAttr *fa)
{
   LONG x;
   struct TextFont *tf;
   static struct TextAttr ta = { "topaz.font", 8, FS_NORMAL, FPF_ROMFONT };

   for (x=0; x<FontTabLast; x++)
      if (strcmp(FontTab[x].Name, fa->ta_Name) == 0 &&
          FontTab[x].YSize == fa->ta_YSize          &&
          FontTab[x].Style == fa->ta_Style          &&
          FontTab[x].Flags == fa->ta_Flags) {
         return FontTab[x].tf;
      }

   if (!(tf = OpenDiskFont(fa))) {
      if (!Topaz8) Topaz8 = OpenFont(&ta);
      return Topaz8;
   }

   strcpy(FontTab[FontTabLast].Name, fa->ta_Name);
   FontTab[FontTabLast].YSize = fa->ta_YSize;
   FontTab[FontTabLast].Style = fa->ta_Style;
   FontTab[FontTabLast].Flags = fa->ta_Flags;
   FontTab[FontTabLast++].tf  = tf;
   
   return tf;
}

void FreeFont(struct TextFont *f)
{
}

void InitFontTab(void)
{
   memset(FontTab, 0, sizeof(FontTab));
   FontTabLast = 0;
}

void CloseFontTab(void)
{
   LONG x;
   
   for (x=0; x<FontTabLast; x++)
      CloseFont(FontTab[x].tf);
}


/*****************************************************************************
 * Function: allocate fonts                                                  *
 *****************************************************************************/


static ULONG AllocFonts(Object *o, SW_IDATA *idata)
{
   STRPTR s;
   ULONG  x,  count=0, spos=0;
   TEXT   NameBuff[40];
   struct TextAttr ta;

   ta.ta_Name  = NameBuff;
   ta.ta_Flags = 0;
   ta.ta_Style = 0;
   idata->FontInfo[count].FontStyle = 0;
   
   ParseFontSpec(idata->SWA_CText_DefFont,NameBuff, &ta.ta_YSize,
                 &idata->FontInfo[count].FontStyle);

   idata->FontInfo[count].Fonts     = GetFont(&ta);
   idata->FontInfo[count].FontAlloc = TRUE;

   count++;
   
   for (x=0; CTxt(x); x++) {
      idata->LineInfo[x].LineFont = (count-1);
      
      for (s=CTxt(x); s = NextCtrl(s); s += CT_Skip)
         if (CT_IsFont(s)) {
            if (CT_IsEmpty(s)) {
               if (spos) --spos;
               idata->FontInfo[count].Fonts     = idata->FontInfo[spos].Fonts;
               idata->FontInfo[count].FontStyle = idata->FontInfo[spos].FontStyle;
               idata->FontInfo[count].FontAlloc = FALSE;
            } else {
               idata->FontInfo[count].FontStyle = 0;
               
               ParseFontSpec(s+CT_Skip, NameBuff, &ta.ta_YSize,
                             &idata->FontInfo[count].FontStyle);

               if (!SW_STRISNULL(NameBuff)) {
                  idata->FontInfo[count].Fonts     = GetFont(&ta);
                  idata->FontInfo[count].FontAlloc = TRUE;
               } else {
                  idata->FontInfo[count].Fonts     = idata->FontInfo[count-1].Fonts;
                  idata->FontInfo[count].FontAlloc = FALSE;
               }
                  
               spos = count;
            }

            count++;
         }
   }

   return SUCCESS;
}


/*****************************************************************************
 * Method: Show (when this object is displayed)                              *
 *****************************************************************************/

METHOD(MUIM_Show, Msg)
{
   if (!DoSuperMethodA(cl,o,msg)) SWDB_RET(FALSE);  // let parent setup self;

   SWDB_RET(TRUE);
}

/*****************************************************************************
 * Method: Hide (when this object is Hidden)                                 *
 *****************************************************************************/

METHOD(MUIM_Hide, Msg)
{
   SWDB_RET(DoSuperMethodA(cl, o, msg));
}


/*****************************************************************************
 * Method: Setup (when this object is displayed)                             *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   ULONG lc = idata->SWA_CText_LineCnt;

   idata->AMM_Height = idata->AMM_Width = -1;
   
   if (!DoSuperMethodA(cl,o,msg)) SWDB_RET(FALSE);  // let parent setup self;

   if (!(idata->LineInfo  = (LineInfo *)AV(lc * sizeof(LineInfo))))
      SWDB_RET(FALSE);

   idata->FontMax = CountCtrl(idata, CTxtFontSt) + 1;
   idata->PenMax  = CountCtrl(idata, CTxtPenSt)  + 1;

   if (!(idata->FontInfo =
         (FontInfo *)AV(max(idata->PenMax,idata->FontMax) * sizeof(FontInfo))))
      SWDB_RET(FALSE);

   if (AllocPens (o, idata) == FAILURE) SWDB_RET(FALSE);
   if (AllocFonts(o, idata) == FAILURE) SWDB_RET(FALSE);

   if (!(idata->SegLens =
         (UWORD *)AV((idata->FontMax + idata->PenMax) * sizeof(UWORD))))
      SWDB_RET(FALSE);

   MUI_RequestIDCMP(o, IDCMP_MOUSEBUTTONS);
  
   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: Cleanup (when this object is hidden)                              *
 *****************************************************************************/

METHOD(MUIM_Cleanup, Msg)
{
   ULONG x;

   MUI_RejectIDCMP(o, IDCMP_MOUSEBUTTONS);

   // -- free pens -----------------------------------------------------------
   
   if (idata->FontInfo)
      for (x=0; x<idata->PenMax; x++)
         if (idata->FontInfo[x].PenAlloc)
            MUI_ReleasePen(muiRenderInfo(o), idata->FontInfo[x].Pens);

   // -- free fonts ----------------------------------------------------------
   
   if (idata->FontInfo)
      for (x=0; x<idata->FontMax; x++)
         if (idata->FontInfo[x].Fonts && idata->FontInfo[x].FontAlloc)
            FreeFont(idata->FontInfo[x].Fonts);

   FV(idata->FontInfo);
   FV(idata->SegLens);
   FV(idata->LineInfo);

   idata->FontInfo  = NULL;
   idata->SegLens   = NULL;
   idata->DragGrp   = NULL;
   idata->LineInfo  = NULL;

   FV(idata->SelReg);
   idata->SelReg    = NULL;
   idata->SelRegCnt = -1;
   idata->SelRegMax = 0;
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: get display bounds                                                *
 *****************************************************************************/

METHOD(MUIM_AskMinMax, struct MUIP_AskMinMax *)
{
   ULONG x, lw, lh,
         rc     = DoSuperMethodA(cl, o, (Msg)msg),
         height = 0,
         width  = 0,
         p1     = 0,
         p2     = 0;

   if (idata->AMM_Width < 0) {
      struct RastPort rp;

      InitRastPort(&rp);
   
      SetFont(&rp,      idata->FontInfo[0].Fonts);
      SetSoftStyle(&rp, idata->FontInfo[0].FontStyle, FStyles);
   
      for (x=0; CTxt(x); x++)
         lw                          = LineHW(idata, x, &p1, &p2, &rp, &lh),
         height                     += (lh + idata->SWA_CText_LineSpacing),
         width                       = max(width, lw),
         idata->LineInfo[x].LineLens = lw;
   
      idata->AMM_Height = height;
      idata->AMM_Width  = width;
   }

   msg->MinMaxInfo->MinWidth  += idata->AMM_Width;
   msg->MinMaxInfo->DefWidth  += idata->AMM_Width;
   msg->MinMaxInfo->MaxWidth  += SHRT_MAX;
   
   msg->MinMaxInfo->MinHeight += idata->AMM_Height;
   msg->MinMaxInfo->DefHeight += idata->AMM_Height;
   msg->MinMaxInfo->MaxHeight += idata->AMM_Height;
   
   SWDB_RET(rc);
} SDFT


   
/*****************************************************************************
 * Function: Line, Column, & Bounds given either x,y pos or Line & Col       *
 *                                                                           *
 *   To find Bounds given Line & Col:                                        *
 *      In:  *L = line, *C0 = col, InMode = FCTP_LineCol                     *
 *      Out: Bounds                                                          *
 *                                                                           *
 *   To find Bounds given x & y pos:                                         *
 *      In: X = xpos, Y = ypos, InMode = FCTP_XYPos                          *
 *      Out: Bounds, *L = line pos at X,Y, *C = col pos at X,Y               *
 *                                                                           *
 *   SelMode is SWV_CText_SelMode_Char/Word/Line                             *
 *****************************************************************************/

static ULONG FindCTextPos(Object *o, SW_IDATA *idata,
                          ULONG InMode, ULONG SelMode,
                          LONG *L, LONG *C0, LONG *C1,
                          LONG X,  LONG Y,
                          struct Rectangle *Bounds)
{
   STRPTR s;
   LONG   FPos, ypos = SWGetL(o, MUIA_TopEdge),
          ynext, xpos = SWGetL(o, MUIA_LeftEdge), xnext = xpos+1, c, l,
          WordStart, WBreakLast = TRUE, XWordStart, Halt, xprev = xpos,
          XBegin, XEnd, ChrBegin, ChrEnd;
   RastPort rp;
   static UBYTE WBreaks[256];
   static ULONG WBInit = FALSE;

   // -- find word break characters ------------------------------------------

   if (!WBInit) {
      memset(WBreaks, 0, sizeof(WBreaks));
      for (s=SWV_CText_WordBrkChars; *s; s++) WBreaks[*s] = 1;
      WBInit = TRUE;
   }

   InitRastPort(&rp);

   // -- move down until we get to right line --------------------------------

   if (InMode == FCTP_XYPos) {
      for (l=0; CTxt(l); l++, ypos = ynext) {
         ynext = ypos + CT_LineSp(idata, l);
      
         if (Y >= ypos && Y < ynext)
            break;
      }
   } else l = *L;
   
   if (!CTxt(l)) return FAILURE;

   FPos = idata->LineInfo[l].LineFont;

   SetFont(&rp,      idata->FontInfo[FPos].Fonts);
   SetSoftStyle(&rp, idata->FontInfo[FPos].FontStyle, FStyles);

   // -- loop through until pixel or column position -------------------------

   if (SelMode != SWV_CText_SelMode_Line) {
      for (c=WordStart=0, Halt = FALSE, s=CTxt(l); s && *s; s++) {
         if (CT_IsCtrl(s)) {
            if (CT_IsFont(s)) {
               SetFont(&rp,      idata->FontInfo[++FPos].Fonts);
               SetSoftStyle(&rp, idata->FontInfo[FPos].FontStyle, FStyles);
            }
                
            s = strchr(s, CT_End);
         } else {
            xnext = xpos + TextLength(&rp, s, 1);
   
            if (!WBreaks[*s] && WBreakLast || WBreaks[*s]) {
               if (Halt) {
                  xnext = xpos, xpos = xprev, c--;
                  break;
               }
               
               WordStart      = c,
               XWordStart     = xpos;
               WBreakLast     = WBreaks[*s];
            }
   
            if (!Halt && (InMode == FCTP_LineCol && c == *C0 ||
                          InMode == FCTP_XYPos   && X >= xpos && X < xnext)) {
               XBegin = xpos, XEnd = xnext-1, ChrBegin = ChrEnd = c, Halt = TRUE;
   
               if (!s[1] || WBreakLast || !WBreakLast && WBreaks[s[1]] ||
                   SelMode == SWV_CText_SelMode_Char)
                  break;
            }
   
            c++, xprev = xpos, xpos = xnext;
         }
      }

      // -- we got to end of line --------------------------------------------
   
      if (!*s)
         XBegin = xpos, XEnd = xnext-1, ChrBegin = ChrEnd = c;
   }

   // -- if needed, find end of word  or line --------------------------------

   switch (SelMode) {
      case SWV_CText_SelMode_Word: {
         ChrBegin = WordStart,
         ChrEnd   = c;
         XBegin   = XWordStart,
         XEnd     = xnext - 1;
         break;
      }

      case SWV_CText_SelMode_Line: {
         ChrBegin = 0;
         ChrEnd   = CT_StrLen(CTxt(l));
         XBegin   = xpos;
         XEnd     = xpos + idata->LineInfo[l].LineLens;
         break;
      }
   }
   
   // -- set output data -----------------------------------------------------

   if (InMode == FCTP_XYPos) {
      if (L)  *L  = l;
      if (C0) *C0 = ChrBegin;
      if (C1) *C1 = ChrEnd;
   }

   if (Bounds)
      Bounds->MinY = ypos,
      Bounds->MaxY = ynext-1,
      Bounds->MinX = XBegin,
      Bounds->MaxX = XEnd;

   return SUCCESS;
}
   

/*****************************************************************************
 * Method: get clickgroup root                                               *
 *****************************************************************************/

static Object *GetClickGrp(Object *o, SW_IDATA *idata)
{
   Object *p;

   if (idata->DragGrp)
      return idata->DragGrp;
   
   for (p = o; p; p = SWGetObj(p, MUIA_Parent))
      if (muiUserData(p) == SWV_CText_ClickGrpRoot)
         return idata->DragGrp = p;

   return idata->DragGrp = o;
}


/*****************************************************************************
 * Function: see if click is before, in, or after these lines (-1/0/1)       *
 *****************************************************************************/

static LONG CmpClick(Object *o, LONG x, LONG y)
{
   if (_isinobject(o, x, y)) return 0;
   
   if (y < SWGetL(o, MUIA_TopEdge))    return -1;
   if (y > SWGetL(o, MUIA_BottomEdge)) return 1;
   if (x < SWGetL(o, MUIA_LeftEdge))   return -1;

   return 1;
}


/*****************************************************************************
 * Function: Get length of strin without escapes                             *
 *****************************************************************************/

static LONG CTLen(STRPTR s)
{
   STRPTR p = s;
   LONG len = 0;

   do {
      if (!(s = NextCtrl(p)))
         s = p + strlen(p);

      len += s - p;
      p    = s;

      if (*p)
         if (p = strchr(p, CT_End))
            p++;
   } while (p && *p);

   return len;
}


/*****************************************************************************
 * Method: Drag stop                                                         *
 *****************************************************************************/

METHOD(SWM_CText_DragStop, Msg)
{
   FReset(idata->Flags, SWF_CText_Selecting);
   MUI_RejectIDCMP(o, IDCMP_MOUSEMOVE);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: process mouse move                                              *
 *****************************************************************************/

static void MouseMove(Object *o, SW_IDATA *idata, LONG MouseX, LONG MouseY)
{
   struct Rectangle B;
   LONG MPos, CCmp, C0, C1;
   SelReg *sr = TopSel(idata);

   if (!idata->DragGrp || !sr ||
       !FIsSet(idata->Flags, SWF_CText_Selecting))
      return;
   
   if (!_isinobject(idata->DragGrp, MouseX, MouseY))
      return;

   CCmp = CmpClick(o, MouseX, MouseY);

   sr->Lp = sr->L1,
   sr->Xp = sr->X1;
      
   if (sr->Lp < 0)
      sr->Lp = sr->L0,
      sr->Xp = sr->X0;

   // -- see if mouse is above, below, or in this object ---------------------

   if (CCmp == 0) {
      MPos = FindCTextPos(o, idata, FCTP_XYPos, idata->DragMode,
                          &sr->L1, &C0, &C1, MouseX, MouseY, &B);

      // -- see if we`re dragging forwards or backwards ----------------------

      if (sr->L1 < sr->L0 ||
          sr->L1 == sr->L0 && MouseX < sr->X0)
         sr->X1 = B.MinX,
         sr->C1 = C0;
      else
         sr->X1 = B.MaxX,
         sr->C1 = C1;
   } else if (CCmp < 0) {
      if (idata->DragSt < 0) {
         RemHL(o, idata, sr);
         return;
      }
      MPos          = SUCCESS;
      sr->L1 = 0;
      sr->C1 = 0;
      sr->X1 = SWGetL(o, MUIA_LeftEdge);
   } else {
      if (idata->DragSt > 0) {
         RemHL(o, idata, sr);
         return;
      }
      MPos          = SUCCESS;
      sr->L1 = idata->SWA_CText_LineCnt-1;
      sr->C1 = CTLen(CTxt(idata->SWA_CText_LineCnt-1));
      sr->X1 = SWGetL(o, MUIA_LeftEdge) +
                   idata->LineInfo[idata->SWA_CText_LineCnt-1].LineLens;
   }

   // -- refresh region ------------------------------------------------------
   
   if (MPos == SUCCESS) {
      if (CmpSelRegDragPos(sr) < 0 && CCmp==0)
         sr->X1 = B.MinX;

      RemSelOverlap(o, idata, idata->SelRegCnt);

      idata->SR_Draw = TopSel(idata);
      MUI_Redraw(o, MADF_DRAWUPDATE);
   }
}


/*****************************************************************************
 * Method: Drag start                                                        *
 *****************************************************************************/

METHOD(SWM_CText_DragStart, SWP_CText_DragStart *)
{
   struct Rectangle B;
   SelReg *sr = TopSel(idata);

   if (FIsSet(idata->Flags, SWF_CText_Selecting))
      SWDB_RET(0);
   
   idata->DragMode = msg->Mode;
   FSet(idata->Flags, SWF_CText_Selecting);

   if (Shifted(msg) && sr && sr->L0 >= 0) {
      MouseMove(o, idata, msg->x, msg->y);
      SWDB_RET(0);
   }

   if (!Alted(msg))
      DoMethod(o, SWM_CText_Unsel, SWV_CText_Unsel_All);

   if (!(sr = PushSelReg(o, idata)))
      SWDB_RET(0);

   idata->DragSt   = CmpClick(o, msg->x, msg->y);

   if (idata->DragSt == 0) {
      FindCTextPos(o, idata, FCTP_XYPos, idata->DragMode,
                   &sr->L0, &sr->C0, NULL, msg->x, msg->y, &B);
      sr->X0 = B.MinX;
   } else if (idata->DragSt < 0) {
      sr->L0 = 0;
      sr->C0 = 0;
      sr->X0 = SWGetL(o, MUIA_LeftEdge);
   } else {
      sr->L0 = idata->SWA_CText_LineCnt-1;
      sr->C0 = CTLen(CTxt(idata->SWA_CText_LineCnt-1));
      sr->X0 = SWGetL(o, MUIA_LeftEdge) +
                   idata->LineInfo[idata->SWA_CText_LineCnt-1].LineLens;
   }

   MUI_RequestIDCMP(o, IDCMP_MOUSEMOVE);   

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: get display bounds                                                *
 *****************************************************************************/

METHOD(MUIM_HandleInput, struct MUIP_HandleInput *)
{
   ULONG Secs, Micros, DragStart = FALSE;
   
   if (idata->SWA_CText_Select == SWV_CText_Select_None) goto Done;

   if (msg->imsg) {
      switch (msg->imsg->Class) {
        case IDCMP_MOUSEBUTTONS: {
           if (msg->imsg->Code==SELECTDOWN) {
              if (GetClickGrp(o, idata) &&
                  _isinobject(o, msg->imsg->MouseX, msg->imsg->MouseY)) {

                 CurrentTime(&Secs, &Micros);

                 if (DoubleClick(idata->Secs, idata->Micros, Secs, Micros) &&
                     idata->SWA_CText_Select != SWV_CText_Select_NoMulti) {
                    if (++idata->DragMode > SWV_CText_SelMode_Line)
                       idata->DragMode = SWV_CText_SelMode_Char;

                    DragStart = TRUE;
                 } else {
                    if (CT_HasSelRegion(idata) &&
                        !Shifted(msg->imsg) && !Alted(msg->imsg)) {
                       DoMethod(idata->DragGrp, SWM_CText_Unsel, SWV_CText_Unsel_All);
                       break;
                    } else idata->DragMode = SWV_CText_SelMode_Char;
                 }

                 idata->Secs            = Secs,
                 idata->Micros          = Micros;
                 idata->Drag_MouseXInit = msg->imsg->MouseX;
                 idata->Drag_MouseYInit = msg->imsg->MouseY;

                 if (Shifted(msg->imsg)) DragStart = TRUE;
                    
                 if (DragStart) {
                    DoMethod(idata->DragGrp, SWM_CText_DragStart,
                             msg->imsg->MouseX, msg->imsg->MouseY,
                             msg->imsg->Qualifier, idata->DragMode);

                    MouseMove(o, idata, msg->imsg->MouseX, msg->imsg->MouseY);
                 } else
                    MUI_RequestIDCMP(o, IDCMP_MOUSEMOVE);
              }
           } else {
              if (idata->DragGrp) {
                 ULONG Selecting = FIsSet(idata->Flags, SWF_CText_Selecting);
                 DoMethod(idata->DragGrp, SWM_CText_DragStop);                 

                 if (Selecting) {
                    if (idata->SWA_CText_AutoClip && !Alted(msg->imsg)) {
                       DoMethod(o,              SWM_CText_Clip,  SWV_CText_Sel_Last);
                       DoMethod(idata->DragGrp, SWM_CText_Unsel, SWV_CText_Unsel_All);
                    }
                 }
              }
           }
        }
        break;
               
        case IDCMP_MOUSEMOVE: {
           if (!FIsSet(idata->Flags, SWF_CText_Selecting) &&
               (abs(msg->imsg->MouseX - idata->Drag_MouseXInit) > 0 ||
                abs(msg->imsg->MouseY - idata->Drag_MouseYInit) > 0))

              DoMethod(idata->DragGrp, SWM_CText_DragStart,
                       msg->imsg->MouseX, msg->imsg->MouseY,
                       msg->imsg->Qualifier, idata->DragMode);

           MouseMove(o, idata, msg->imsg->MouseX, msg->imsg->MouseY);
           break;
        }
      }
   }

  Done:
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: get relative line number                                          *
 *****************************************************************************/

static LONG RelLine(SW_IDATA *idata, LONG Pos)
{
   return (Pos <   idata->FirstLine) ?
               SWV_RelLine_BeforeFirst :
          (Pos >=  (idata->FirstLine + idata->SWA_CText_LineCnt)) ?
               SWV_RelLine_AfterLast :
          (Pos - idata->FirstLine);
}

   
/*****************************************************************************
 * Method: Select region                                                     *
 *****************************************************************************/

METHOD(SWM_CText_SelRegion, SWP_CText_SelRegion *)
{
   struct Rectangle Bounds;
   SelReg *sr;

   DoMethod(o, SWM_CText_Unsel, SWV_CText_Unsel_All);

   if (!(sr = PushSelReg(o, idata)))
      SWDB_RET(0);

   if (sr->L0 == msg->L0 &&
       sr->L1 == msg->L1 &&
       sr->C0 == msg->C0 &&
       sr->C1 == msg->C1)
      SWDB_RET(0);
   
   if (msg->L1 < 0 || msg->L1 < 0 || msg->C0 < 0 || msg->C1 < 0)
      SWDB_RET(0);
   
   sr->L0 = msg->L0;
   sr->C0 = msg->C0;
   sr->L1 = msg->L1;
   sr->C1 = msg->C1;

   FindCTextPos(o, idata, FCTP_LineCol, SWV_CText_SelMode_Char,
                &msg->L0, &msg->C0, NULL, 0, 0, &Bounds);

   sr->X0 = Bounds.MinX;

   FindCTextPos(o, idata, FCTP_LineCol, SWV_CText_SelMode_Char,
                &msg->L1, &msg->C1, NULL, 0, 0, &Bounds);
   
   sr->X1 = Bounds.MaxX;

   sr->Lp = sr->L0;
   sr->Xp = sr->X0;

   idata->SR_Draw = sr;
   MUI_Redraw(o, MADF_DRAWUPDATE);
   
   SWDB_RET(0);
}


/*****************************************************************************
 * Method: Select all text                                                   *
 *****************************************************************************/

METHOD(SWM_CText_SelAll, Msg)
{
   SWDB_RET(DoMethod(o, SWM_CText_SelRegion, 0, 0,
                     idata->SWA_CText_LineCnt-1,
                     CTLen(CTxt(idata->SWA_CText_LineCnt-1))));
} SDFT


/*****************************************************************************
 * Method: Unselect all text                                                 *
 *****************************************************************************/

METHOD(SWM_CText_Unsel, SWP_CText_Unsel *)
{
   LONG Num;
   
   switch (Num = msg->Pos) {
     case SWV_CText_Unsel_All:
      while (TopSel(idata))
         PopSelReg(o, idata);
      SWDB_RET(0);

     case SWV_CText_Unsel_First: Num = 0; break;
     case SWV_CText_Unsel_Last:  Num = idata->SelRegCnt; break;
   }

   RemSelReg(o, idata, Num);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Generate string for selected text                                 *
 *****************************************************************************/

METHOD(SWM_CText_GetClipStr, SWP_CText_GetClipStr *)
{
   LONG x, bsize=0, l, llen, R0, R1, R;
   LONG  off1, off2;
   STRPTR Txt, tpos, s, prev;
   SelReg *sr;

   if (FIsSet(idata->Flags, SWF_CText_Selecting))
      SWDB_RET(NULL);

   NormalizeSelRegAll(idata);


   // -- get loop bounds -----------------------------------------------------
   
   switch (msg->Num) {
     case SWV_CText_Sel_First: R0 = R1 = 0;                   break;
     case SWV_CText_Sel_Last:  R0 = R1 = idata->SelRegCnt;    break;
     case SWV_CText_Sel_All:   R0 = 0; R1 = idata->SelRegCnt;
                               SortSelReg(idata);
                               break;
     default:                  R0 = R1 = msg->Num;            break;
   }

   // -- determine maxmimum buffer size we`ll need ---------------------------

   for (R=R0; R<=R1; R++) {
      if (!(sr = SelRegNum(idata, R)) || !CT_SelRegActive(sr))
         continue;

      for (x=sr->L0; x<=sr->L1; x++)
         bsize += strlen(CTxt(x)) + 1;

      bsize += 2;
   }

   if (!bsize) SWDB_RET(NULL);

   // -- allocate a big enough buffer ----------------------------------------

   if (!(tpos = Txt = AllocVec(bsize + 2, MEMF_ANY))) SWDB_RET(NULL);
   

   // -- copy data to buffer -------------------------------------------------

   for (R=R0; R<=R1; R++) {
      if (!(sr = SelRegNum(idata, R)) || !CT_SelRegActive(sr))
         continue;

      for (x=sr->L0; x<=sr->L1; x++) {
         for (llen=0, s=prev=CTxt(x), tpos; s && *s; prev = s) {
            if (!(s = NextCtrl(prev)))
               s = prev + strlen(prev);
   
            if (s != prev) {
               l     = s-prev;
               off1  = (x == sr->L0 ? max(sr->C0 - llen, 0)   : 0);
               off2  = (x == sr->L1 ? min(sr->C1 - llen, l-1) : l-1);
               llen += l;
               
               if (off1 >= 0 && off1 < l && off2 >= 0 && off2 < l) {
                  strncpy(tpos, prev+off1, off2-off1+1);
                  tpos += off2-off1+1;
               }
            }
   
            if (*s) {
               s = strchr(s, CT_End);
               if (s) s++;
            }
         }
   
         if (x < sr->L1) *tpos++ = '\n';
      }

      if (R != R1) *tpos++ = '\n';
   }

   *tpos = '\0';
   
   SWDB_RET(Txt);
} SDFT


/*****************************************************************************
 * Method: Add clip string to array                                          *
 *****************************************************************************/

METHOD(SWM_CText_GCInt, SWP_CText_GCInt *)
{
   STRPTR s = (STRPTR)DoMethod(o, SWM_CText_GetClipStr, msg->Num);

   if (!s) SWDB_RET(0);

   if (*msg->Pos >= *msg->Size) {
      (*msg->Size)   += 8;

      if (RV(*msg->Size*sizeof(STRPTR),   msg->SArray) < 0 ||
          RV(*msg->Size*sizeof(Object *), msg->OArray) < 0) {
          FreeVec(s);
         (*msg->Size) = (*msg->Pos) = 0;
         SWDB_RET(0);
      }
   }

   (*msg->SArray)[(*msg->Pos)]   = s;
   (*msg->OArray)[(*msg->Pos)++] = o;
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Get string for all clips in group                                 *
 *****************************************************************************/

METHOD(SWM_CText_GetGrpClip, SWP_CText_GetGrpClip *)
{
   STRPTR s, sp, *SArray = NULL;
   Object **OArray = NULL;
   LONG   x, Pos = 0, Size = 0, TotSize = 0;

   if (FIsSet(idata->Flags, SWF_CText_Selecting))
      SWDB_RET(NULL);

   DoMethod(GetClickGrp(o, idata), SWM_CText_GCInt, &SArray, &OArray, &Pos,
            &Size, msg->Num);

   // -- count total size of strings -----------------------------------------
   
   for (x=0; x<Pos; x++)
      TotSize += strlen(SArray[x]) + 1;

   // -- allocate space ------------------------------------------------------

   if (!(s = sp = AllocVec(TotSize + 4, MEMF_ANY))) goto Done;

   // -- copy each object`s strings into one big-ass one ---------------------

   for (x=0; x<Pos; x++) {
      // -- handle newlines if this object is below previous one -------------

      if (x > 0 &&
          SWGetUL(OArray[x], MUIA_TopEdge) >
          SWGetUL(OArray[x-1], MUIA_BottomEdge))
         sp += strlen(strcpy(sp, "\n"));

      // -- copy little string into big one ----------------------------------
      
      sp += strlen(strcpy(sp, SArray[x]));
      FreeVec(SArray[x]);
      SArray[x] = NULL;
   }

  Done:
   for (x=0; x<Pos; x++)
      FreeVec(SArray[x]);
   
   FV(SArray);
   FV(OArray);

   SWDB_RET(s);
} SDFT


/*****************************************************************************
 * Method: Copy clip string to clipboard                                     *
 *****************************************************************************/

METHOD(SWM_CText_Clip, SWP_CText_Clip *)
{
   struct IFFHandle *iff;
   STRPTR s;

   if (FIsSet(idata->Flags, SWF_CText_Selecting))
      SWDB_RET(0);

   if (!(s = (STRPTR)DoMethod(o, SWM_CText_GetGrpClip, msg->Num)))
      SWDB_RET(0);

   if (iff = SWOpenClipWrite(0)) {
      SWTextToClip(iff, s);
      SWCloseClipWrite(iff);
   }

   FreeVec(s);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: setup in preparation for search                                   *
 *****************************************************************************/

METHOD(SWM_ListSearch_Setup, SWP_ListSearch_Setup *)
{
   idata->FirstLine = *msg->State;

   *msg->State += idata->SWA_CText_LineCnt;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: select entries                                                    *
 *****************************************************************************/

METHOD(MUIM_List_Select, struct MUIP_List_Select *)
{
   if (msg->pos == MUIV_List_Select_All) {
      switch (msg->seltype) {
        case MUIV_List_Select_On:  DoMethod(o, SWM_CText_SelAll);   break;
        case MUIV_List_Select_Off: DoMethod(o, SWM_CText_Unsel,
                                            SWV_CText_Unsel_All); break;
      }
   }
   
  SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: get a particular line                                             *
 *****************************************************************************/

METHOD(MUIM_List_GetEntry, struct MUIP_List_GetEntry *)
{
   LONG  Line, Len;
   STRPTR s, lb;

   if (msg->pos == MUIV_List_GetEntry_Active) SWDB_RET(0);

   // -- if outside of our line range, ignore the request --------------------

   if ((Line = RelLine(idata, msg->pos)) < 0)
      SWDB_RET(0);

   // -- if same as last, reuse it -------------------------------------------
   
   if (idata->LineBuffLine == Line)
      goto Done;
      
   // -- allocate space for line ---------------------------------------------

   if ((Len=strlen(s=CTxt(Line))) >= idata->LineBuffSize) {
      FV(idata->LineBuff);
      idata->LineBuffSize = Len + 12;
      
      if (!(idata->LineBuff = AV(idata->LineBuffSize))) SWDB_RET(0);
   }

   // -- copy line to buffer -------------------------------------------------

   for (lb=idata->LineBuff; *s; s++)
      if (*s == CT_Ctrl) {
         if (!(s = strchr(s, CT_End)))
            s = "}";
      } else
         *lb++ = *s;

   *lb = '\0';

   idata->LineBuffLine = Line;
   
  Done:
   *((STRPTR *)msg->entry) = idata->LineBuff;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: make a line visible in any parent scroll group                  *
 *****************************************************************************/

static void MakeLineVis(Object *o, SW_IDATA *idata, LONG Line)
{
#  define MLV_StackSize 4
   LONG   x, ypos, MTop, MHeight, Top;
   Object *Parent = o, *vg, *PStack[MLV_StackSize];

   // -- we have to traverse through scrollgroups in reverse order ----------
   // -- coz we can`t set VGroup_Top on non-visible ones ---------------------
   
   x=0;
   if (!(vg = VirtgroupObject, End)) return;

   while ((Parent = SWGetObj(Parent, MUIA_Parent)) && x < MLV_StackSize)
      if (OCLASS(vg) == OCLASS(Parent))
         PStack[x++] = Parent;

   MUI_DisposeObject(vg);

   if (!x) return; // no vgroups;

   // -- now loop through backwards, setting vgroup --------------------------
   
   for (--x; x > 0; x--) {
      MTop    = SWGetL(PStack[x-1], MUIA_TopEdge) -
                SWGetL(PStack[x], MUIA_TopEdge);
      MHeight = SWGetL(PStack[x], MUIA_Height);
      Top     = SWGetL(PStack[x], MUIA_Virtgroup_Top);
      if (MTop < 0 || MTop > MHeight)
         set(PStack[x], MUIA_Virtgroup_Top, Top + MTop - MHeight / 5);
   }

   for (x=0, ypos=SWGetL(o, MUIA_TopEdge); x<Line; x++)
      ypos += CT_LineSp(idata, x);

   MTop    = ypos - SWGetL(PStack[0], MUIA_TopEdge);
   MHeight = SWGetL(PStack[0], MUIA_Height);
   Top     = SWGetL(PStack[0], MUIA_Virtgroup_Top);
   
   if (MTop < 0 || MTop > MHeight)
      set(PStack[0], MUIA_Virtgroup_Top, Top + MTop - MHeight / 5);

#  undef MLV_StackSize
}


/*****************************************************************************
 * Method: jump to a particular line                                         *
 *****************************************************************************/

METHOD(MUIM_List_Jump, struct MUIP_List_Jump *)
{
   LONG  Line;

   if ((Line = RelLine(idata, msg->pos)) < 0)
      SWDB_RET(0);

   MakeLineVis(o, idata, Line);
         
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: jump to a particular line                                         *
 *****************************************************************************/

METHOD(SWM_ListSearch_HLMatch, struct SWP_ListSearch_HLMatch *)
{
   STRPTR s = NULL;
   LONG  Line;
   ULONG C0, C1;
   struct PatBounds pb;

   DoMethod(o, SWM_CText_Unsel, SWV_CText_Unsel_All);

   if (msg->Line == MUIV_List_Active_Off) SWDB_RET(0);
   
   if ((Line = RelLine(idata, msg->Line)) < 0)
      SWDB_RET(0);

   DoMethod(o, MUIM_List_GetEntry, msg->Line, &s);
   if (!s) SWDB_RET(0);

   if (SWPB_Start(&pb, msg->Str, msg->Case, TRUE, NULL) != SUCCESS)
      SWDB_RET(0);

   C0 = C1 = 0;

   if (SWPB_Find(s, &pb, &C0, &C1, TRUE))
      DoMethod(o, SWM_CText_SelRegion, Line, C0, Line, C1);

   SWPB_End(&pb);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: append stack ctrl codes to a string (must be big enough!)       *
 *****************************************************************************/

static void AppendCtrlStack(STRPTR s, STRPTR *Stack, LONG StPos)
{
   ULONG  x;
   STRPTR epos;
   UBYTE  c;

   for (x=0; x<StPos; x++) {
      if (epos = strchr(Stack[x], CT_End))
         c = epos[1], epos[1] = '\0';
      
      s += strlen(strcat(s, Stack[x]));

      if (epos) epos[1] = c;
   }
}


/*****************************************************************************
 * Method: Get unpopped ctrl junk                                            *
 *****************************************************************************/

# define IntStackSize 16
 
static void GS_Push(SW_IDATA *idata, STRPTR **Stack,
                    LONG *Pos, LONG *StackSize, STRPTR s)
{
   (*Stack)[(*Pos)++] = s;

   if (*Pos < *StackSize) return;

   if (*StackSize != IntStackSize) {
      FV(*Stack);
      *Stack = NULL;
   }

   (*StackSize) += 32;

   (*Stack) = (STRPTR *)AV(*StackSize * sizeof(STRPTR));
}


METHOD(SWM_CText_GetStack, Msg)
{
   LONG   x, FontPos = 0, PenPos = 0,
          FStackSize = IntStackSize,
          PStackSize = IntStackSize;
   STRPTR FIntStack[IntStackSize],
          PIntStack[IntStackSize];
   STRPTR s, PopBuff = NULL, *FStack = FIntStack, *PStack = PIntStack;

   // -- loop through, looking for unpopped ctrl junk ------------------------

   for (x=0; CTxt(x); x++)
      for (s=CTxt(x); s = NextCtrl(s); s+=CT_Skip) {
         if (CT_IsPen(s)) {
            if (CT_IsEmpty(s)) PenPos--;
            else
               GS_Push(idata, &PStack, &PenPos, &PStackSize, s);
         } else if (CT_IsFont(s)) {
            if (CT_IsEmpty(s)) FontPos--;
            else
               GS_Push(idata, &FStack, &FontPos, &FStackSize, s);
         }
      }

   // -- if no unpopped stuff, return OK -------------------------------------

   if (!PenPos && !FontPos) goto Done;

   // -- otherwise, build string to push later on ----------------------------
   
   if (!(PopBuff = AV((PenPos+FontPos)*max(MAXFONTNAME,32))))
      goto Done;

   PopBuff[0] = '\0';

   AppendCtrlStack(PopBuff, PStack, PenPos);
   AppendCtrlStack(PopBuff, FStack, FontPos);

  Done:
   if (FStack != FIntStack) FV(FStack);
   if (PStack != PIntStack) FV(PStack);

   SWDB_RET(PopBuff);
} SDFT


/*****************************************************************************
 * Method: Free unpopped ctrl junk                                           *
 *****************************************************************************/

METHOD(SWM_CText_FreeStack, SWP_CText_FreeStack *)
{
   FV(msg->s);
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: height given font name                                          *
 *****************************************************************************/

static LONG FontHeight(STRPTR f)
{
   STRPTR sp;

   if (!f || !(sp = strchr(f, '/')))
      return 13;

   return atol(sp+1);
}

   
/*****************************************************************************
 * Method: Find sum of line heights in a given range                         *
 *         (can be called before MUIM_Setup)                                 *
 *****************************************************************************/

METHOD(SWM_CText_LineRangeHeight, SWP_CText_LineRangeHeight *)
{
   LONG Line, *Height, Sum = 0, Pos = 0, Max, FontMax, L0, L1;
   STRPTR s;

   if (msg->Height) Sum = *msg->Height;
   
   FontMax = CountCtrl(idata, CTxtFontSt);

   // -- find relative line positions ----------------------------------------
   
   L0 = RelLine(idata, msg->L0);
   L1 = RelLine(idata, msg->L1);

   if (L1 == SWV_RelLine_BeforeFirst || L0 == SWV_RelLine_AfterLast)
      goto Done;

   if (L0 == SWV_RelLine_BeforeFirst) L0 = 0;
   if (L1 == SWV_RelLine_AfterLast)   L1 = L0 + idata->SWA_CText_LineCnt - 1;

   // -- allocate array for heights ------------------------------------------

   if (!(Height = (ULONG *)AV((FontMax+1) * sizeof(ULONG))))
      goto Done;

   Height[Pos] = FontHeight(idata->SWA_CText_DefFont);

   // -- loop through text, finding maximum line heights in pixels -----------

   for (Line=0; Line <= L1; Line++) {
      Max = Height[Pos];
      
      for (s=CTxt(Line); s = NextCtrl(s); s += CT_Skip) {
         if (CT_IsFont(s)) {
            if (CT_IsEmpty(s)) Pos--;
            else
               if ((Height[++Pos] = FontHeight(s+CT_Skip)) == 0)
                  Height[Pos] = Height[Pos-1];

            Max = max(Max, Height[Pos]);
         }
      }
      
      if (Line >= L0)
         Sum += Max + idata->SWA_CText_LineSpacing;
   }

   FV(Height);

  Done:
   if (msg->Height) *msg->Height = Sum;
   
   SWDB_RET(max(Sum, 5));
} SDFT
