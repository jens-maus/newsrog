
# include "S-GetEOL.h"
# include "S-CTSupport.h"
# include "S-SetUIToggle.h"
# include "S-IsEmptyLine.h"

/*****************************************************************************
 * Function: See if line has no alphabetic characters                        *
 *****************************************************************************/

static __inline IsAlphabetic(UBYTE c)
{
   return (c >= 'A' && c <= 'Z') ||
          (c >= 'a' && c <= 'z') ||
          c > 192;
}


static LONG NoASCII(STRPTR s, LONG IsEmpty)
{
   if (IsEmpty) return FALSE;
   
   if (s)
      for (; *s && *s != '\n'; s++)
         if (IsAlphabetic(*s))
            return FALSE;

   return TRUE;
}


/*****************************************************************************
 * Function: open topaz 8                                                    *
 *****************************************************************************/

static struct TextFont *GetFont(struct TextAttr *fa)
{
   struct TextAttr ta = { "topaz.font", 8, FS_NORMAL, FPF_ROMFONT };
   struct TextFont *tf;

   if (tf = OpenDiskFont(fa))
      return tf;

   return OpenFont(&ta);
}


/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   FData **FD = (FData **)msg->FltData;
   ULONG  x, rc = SUCCESS;
   ULONG  PL = strlen(idata->SWA_NDF_Rewrap_NoFlow);
   STRPTR tpat;

   if (!(*FD = (FData *)NDF_AV(sizeof(FData))))
      SWDB_RET(FAILURE);

   if (!((*FD)->PPat = NDF_AV(PL * 2 + 2)))
      SWDB_RET(FAILURE);

   if (!(tpat = NDF_AV(strlen(idata->SWA_NDF_Rewrap_NoFlow) + 4)))
      SWDB_RET(FAILURE);

   if (SW_STRISNULL(idata->SWA_NDF_Rewrap_NoFlow))
      tpat[0] = '\0';
   else
      strcat(strcpy(tpat, idata->SWA_NDF_Rewrap_NoFlow), "*");
   
   if (ParsePattern(tpat, (*FD)->PPat, PL*2+2)<0) {
      NDF_FV(tpat);
      SWDB_RET(FAILURE);
   }

   NDF_FV(tpat);

   memset(idata->QChars, 0, sizeof(idata->QChars));
   memset(idata->BChars, 0, sizeof(idata->BChars));

   for (x=0; idata->SWA_NDF_Rewrap_QuoteChars[x]; x++)
      idata->QChars[idata->SWA_NDF_Rewrap_QuoteChars[x]] = 1;

   for (x=0; idata->SWA_NDF_Rewrap_BreakChars[x]; x++)
      idata->BChars[idata->SWA_NDF_Rewrap_BreakChars[x]] = 1;

   if (msg->ArtView) {
      UBYTE  c;
      ULONG x, YSize = 8;
      struct RastPort rp;
      struct TextAttr ta;
      struct TextFont *tf = NULL;
      TEXT   Txt[2], Font[64];

      STRPTR sp,
             VFont = (STRPTR)DoMethod(msg->ArtView, SWM_NArticleView_GetDefFont);

      if (SW_STRISNULL(VFont)) {
         rc = FAILURE;
         goto Done;
      }
   
      // -- parse font size, etc ---------------------------------------------

      if (sp = strchr(VFont, '/')) {
         c = sp[0], sp[0] = '\0';
         YSize = atol(sp+1);
         YSize = max(YSize, 5);
      }

      strcat(strcpy(Font, VFont), ".font");
      if (sp) sp[0] = c;

      ta.ta_Name  = Font;
      ta.ta_Flags = 0;
      ta.ta_Style = FS_NORMAL;
      ta.ta_YSize = YSize;
   
      // -- open the font ----------------------------------------------------

      if (!(tf = GetFont(&ta))) {
         rc = FAILURE;
         goto Done;
      }

      // -- get table of character sizes -------------------------------------
   
      InitRastPort(&rp);
      SetFont(&rp, tf);
   
      for (x=0; x<CNUM; x++)
         (*FD)->CharWidths[x] = tf->tf_XSize;
   
      for (x=tf->tf_LoChar, Txt[1] = '\0'; x<=tf->tf_HiChar; x++) {
         Txt[0] = x;
         (*FD)->CharWidths[x] = TextLength(&rp, Txt, 1);
      }
   
      // -- get maximum wrap width in pixels ------------------------------------
   
      (*FD)->MaxWidth = SWGetUL(msg->ArtView, MUIA_Width) * 
         idata->SWA_NDF_Rewrap_WidthPct / 100;
   
      (*FD)->MaxWidth = max((*FD)->MaxWidth, SWV_NDF_Rewrap_MinWidth);

     Done:
      if (tf) CloseFont(tf);
   }
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   FData **FD = (FData **)msg->FltData;

   if (*FD) {
      NDF_FV((*FD)->PPat);
      NDF_FV((*FD)->Lines);
   }

   NDF_FV(*FD);
   *FD = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: See if this is truely a break location in the string            *
 *****************************************************************************/


static __inline IsBrk(STRPTR qq, STRPTR s, ULONG x)
{
   if (!qq[s[x]]) return FALSE;

   if (s[x]==':' &&
       ((s[x+1]=='-' && (s[x+2]==')'||s[x+2]=='(')) ||
        s[x+1]==')'))
      return FALSE;

   return TRUE;
}


/*****************************************************************************
 * Function: Find indent level                                               *
 *****************************************************************************/

static LONG IndentLevel(SW_IDATA *idata, STRPTR *Lines, ULONG Start,
                        STRPTR IndStr, ULONG IndStrLen, ULONG *IndChars)
{
   LONG  l = 0, found, x = 0, y, LPos = 0,
         MaxPL = idata->SWA_NDF_Rewrap_PrefixLen;
   STRPTR qq = idata->QChars, s = Lines[Start];

   // -- find indent level ---------------------------------------------------
   
   do {
      y = found = 0;
      CT_Loop(s, x, x, s[x] != '\n' && y<=MaxPL && !found, y++)
         if (IsBrk(qq, s, x))
            l++, LPos = x+1, found=TRUE;
   } while (found);

   // -- heuristic to nuke some non-indent prefixes --------------------------
   
   if (l && idata->SWA_NDF_Rewrap_PrefixHeur) {
      x = LPos - 1;

      // -- permit some types of prefixes iff next/prev line has 1 too -------
         
      if (x >= 0 && s[x] != '>') {
         if (// -- we need the next or previous line to have this prefix too -
             !((Start > 0 && !strncmp(s, Lines[Start-1], x+1)) ||
               (*Lines[Start+1] && !strncmp(s, Lines[Start+1], x+1))) &&

             // -- but let it pass if it`s between two blank lines or --------
             // -- the first char on the line --------------------------------
             !(Start>0 && *Lines[Start+1] &&
               IsEmptyLine(Lines[Start-1]) && IsEmptyLine(Lines[Start+1]))) {
               for (l--, x--; x>=0 && !IsBrk(qq, s, x); x--);
               LPos = x + 1;
            }
      }
   }

   // -- skip white space before text ----------------------------------------
      
   while (s[LPos] == ' ' || s[LPos] == '\t')
      LPos++;

   // -- remember indent prefix ----------------------------------------------
      
   if (IndStr) {
      IndStr[0] = '\0';

      x = min(LPos, IndStrLen-1);
      strncpy(IndStr, s, x);
      IndStr[x] = '\0';
   }

   if (IndChars) *IndChars = LPos;

   return l;
}


/*****************************************************************************
 * Function: check for a break line                                          *
 *****************************************************************************/

static LONG IsShortLine(SW_IDATA *idata, STRPTR *Lines, ULONG QChars)
{
   return (LLen(Lines, 0) - QChars) < idata->SWA_NDF_Rewrap_MinWrapLen;
}

 
/*****************************************************************************
 * Function: check for break character                                       *
 *****************************************************************************/

static __inline IsBr(UBYTE c, STRPTR bc)
{
   return c == ' '|| c == '\n' || bc[c];
}


/*****************************************************************************
 * Function: increment line width (return 1=toolong, else 0)                 *
 *****************************************************************************/

static __inline LONG IncWidth(SW_IDATA *idata, ULONG *LineW,
                              FData *FD, UBYTE c)
{
   if (idata->TextOnly && idata->SWA_NDF_Rewrap_Cols)
      return ((*LineW) += 1) >= idata->SWA_NDF_Rewrap_Cols;

   return ((*LineW) += FD->CharWidths[c]) >= FD->MaxWidth;
}
   

/*****************************************************************************
 * Function: copy with no reflow, and in a fixed font                        *
 *****************************************************************************/

static void CopyNoFlow(Object *o, SW_IDATA *idata, TxtBuff *Tx,
                       STRPTR *Lines, ULONG l1, ULONG l2, STRPTR Font,
                       ULONG PrevEnd)
{
   ULONG x;
   
   Font = SW_STRDEF(Font, DefTblFont);

   if (!idata->TextOnly) {
      if (l1 == PrevEnd + 1) {
         for (x=TxLen(Tx); x>0; x--)
            if (CT_IsFont(TxBuff(Tx)+x)) {
               DoMethod(o, SWM_NDFilter_TBTruncate, Tx, x);
               break;
            }

         DoMethod(o, SWM_NDFilter_TBAppendTxt, Tx, "\n");
      } else {
         DoMethod(o, SWM_NDFilter_StyleOn, Tx, Font, FALSE, FALSE, FALSE, FALSE, NULL);
      }
   }

   DoMethod(o, SWM_NDFilter_TBAppendSub, Tx, Lines[0],
            Lines[l1]-Lines[0], Lines[l2+1]-1-Lines[0]);

   if (!idata->TextOnly) {
      DoMethod(o, SWM_NDFilter_TBTruncate, Tx, TxLen(Tx)-1);
      DoMethod(o, SWM_NDFilter_StyleOff, Tx, Font, FALSE, FALSE, FALSE,
               FALSE, NULL);
      DoMethod(o, SWM_NDFilter_TBAppendTxt, Tx, "\n");
   }
}


/*****************************************************************************
 * Function: Find line position given buffer pointer                         *
 *****************************************************************************/

static ULONG FindLinePos(STRPTR *Lines, STRPTR s, ULONG LastLPos)
{
   ULONG x;

   if (s >= Lines[LastLPos]   && s < Lines[LastLPos+1]) return LastLPos;
   if (s >= Lines[LastLPos+1] && s < Lines[LastLPos+2]) return LastLPos+1;

   for (x=0; *Lines[x+1]; x++)
      if (s < Lines[x+1])
         return x;

   return 0;
}


/*****************************************************************************
 * Function: append region with reflow                                       *
 *****************************************************************************/

static void ReflowRegion(Object *o, SW_IDATA *idata, FData *FD, TxtBuff *Tx,
                         STRPTR *Lines, ULONG l1, ULONG l2, STRPTR bc, STRPTR qp)
{
   UBYTE   c;
   ULONG   LineW = 0, OutPos, OutSize;
   ULONG   LPLen, y, LastLPos = l1;
   STRPTR  OutBuf, bp, bpt, p2, PrevBreakPos = Lines[l1];

   DoMethod(o, SWM_NDFilter_TBGetSpace, Tx,
            TxLen(Tx) + (Lines[l2+1]-Lines[l1]) * 77 / 72);

   OutBuf  = TxBuff(Tx);
   OutPos  = TxLen(Tx);
   OutSize = TxSize(Tx);
   p2      = Lines[l2]-1;
   
   for (bp = Lines[l1]; bp<=p2;) {
      if ((c = *bp) == '\n' && bp != p2) {
         IndentLevel(idata, Lines,
                     (LastLPos = FindLinePos(Lines, bp, LastLPos))+1,
                     NULL, 0, &LPLen);
         bp += LPLen;
         c   = ' ';
      }

      // -- make sure we have enough space -----------------------------------
      
      if (OutPos + 250 >= OutSize) {
         OutBuf[TxLen(Tx) = OutPos] = '\0';
         DoMethod(o, SWM_NDFilter_TBGetSpace, Tx, OutPos + 250);

         OutBuf  = TxBuff(Tx);
         OutPos  = TxLen(Tx);
         OutSize = TxSize(Tx);
      }
      
      if (IncWidth(idata, &LineW, FD, c)) {
         // -- find previous word break --------------------------------------
         for (bpt=bp; bpt > PrevBreakPos && !IsBr(*bpt, bc); bpt--);

         // -- if none, find next word break ---------------------------------
         if (bpt == PrevBreakPos)
            for (bpt=bp; bpt<p2 && !IsBr(*bpt, bc); bpt++);

         // -- either remove part of last word or insert rest of this one ----

         if (bpt < bp) {
            if (OutPos) OutPos--;
            for(; !IsBr(OutBuf[OutPos], bc); OutPos--);
            while (OutBuf[OutPos] == ' ' || OutBuf[OutPos] == '\n') OutPos--;
            OutPos++;
         } else if (bpt > bp) {
            for (; bp <= p2 && bp < (bpt+200) && !IsBr(*bp, bc); bp++, OutPos++)
               OutBuf[OutPos] = *bp;
         }

         // -- insert newline ------------------------------------------------

         LineW   = 0;
         bp      = bpt;

         // -- collapse whitespace -------------------------------------------
         
         while (bp < p2 && (*bp == ' ' || *bp == '\n'))
            bp++;

         if (bp != Lines[l1] && bp[-1] == '\n') {
            IndentLevel(idata, Lines,
                        (LastLPos = FindLinePos(Lines, bp, LastLPos)),
                        NULL, 0, &LPLen);
            bp += LPLen;
         }

         // -- add quote prefix for next line --------------------------------

         if (bp < p2) {
            OutBuf[OutPos++] = '\n';

            for (y=0; qp[y]; y++)
               IncWidth(idata, &LineW, FD, OutBuf[OutPos++] = qp[y]);
         }

         OutBuf[OutPos]   = '\0';
         PrevBreakPos     = bp;

      } else OutBuf[OutPos++] = c, bp++;
   }

   OutBuf[TxLen(Tx) = OutPos] = '\0';
}


/*****************************************************************************
 * Function: see if two quote prefixes are the same, ignoring trailing space *
 *****************************************************************************/

static int SameQP(STRPTR QP1, STRPTR QP2, LONG PLen1, LONG PLen2)
{
   if (PLen1 == PLen2) return strcmp(QP1, QP2) == 0;

   if (PLen1) while (QP1[--PLen1] == ' ') QP1[PLen1] = '\0';
   if (PLen2) while (QP2[--PLen2] == ' ') QP2[PLen2] = '\0';

   return strcmp(QP1, QP2) == 0;
}


/*****************************************************************************
 * Function: detect bad wrapping:                                            *
 *     > A bunch of quoted text lines here and then                          *
 *     this                                                                  *
 *     > word and then some more quoted text and then                        *
 *     another.                                                              *
 *****************************************************************************/

static LONG IsBadWrap(SW_IDATA *idata, FData *FD, STRPTR *Lines, ULONG Start)
{
   ULONG   PLen0, PLen1, PLen2, IndLev1, IndLev3;
   TEXT    QP1[QPrefixLen], QP3[QPrefixLen];

   if (!idata->SWA_NDF_Rewrap_BadWrap ||
       !Lines[Start+1][0] || !Lines[Start+2][0])
      return FALSE;
   
   // -- need a prefix on 1st line -------------------------------------------

   IndLev1 = IndentLevel(idata, Lines, Start, QP1,  QPrefixLen, &PLen0);

   // -- first line has to be longer than BadWrapMax -------------------------
   
   if ((LLen(Lines, Start) - PLen0) <= idata->SWA_NDF_Rewrap_BadWrapMax)
      return FALSE;

   // -- line 2 prefix has to be <= line 1 prefix ----------------------------

   IndentLevel(idata, Lines, Start+1, NULL, NULL, &PLen1);

   if (PLen1 > PLen0) return FALSE;

   IndLev3 = IndentLevel(idata, Lines, Start+2, QP3,  QPrefixLen, &PLen2);

   // -- the middle line can`t be longer than BadWrapMax ---------------------

   if ((LLen(Lines, Start+1) - PLen1) > idata->SWA_NDF_Rewrap_BadWrapMax)
      return FALSE;

   // -- the middle line cannot be empty. ------------------------------------
   
   Lines += Start;

   if (IsEmptyLine(Lines[1] + PLen1))
      return FALSE;

   // -- line 3 has to be like line 1, or empty  -----------------------------

   if (IsEmptyLine(Lines[2] + PLen2))
      return FALSE;

   if (!SameQP(QP1, QP3, PLen0, PLen2) && !IsEmptyLine(Lines[2]))
      if (!(IndLev1 == IndLev3 && PLen0 >= PLen2)) 
          return FALSE;

   // -- if line 3 matches the break pattern, it`s not a bad wrap ------------

   if (idata->SWA_NDF_Rewrap_UseNoFlow &&
       MatchPattern(FD->PPat, Lines[2] + PLen2))
      return FALSE;

   // -- need alpha characters on the lines ----------------------------------
   
   if (NoASCII(Lines[0] + PLen0, FALSE) ||
       NoASCII(Lines[1] + PLen1, FALSE) ||
       NoASCII(Lines[2] + PLen2, FALSE))
      return FALSE;
   
   // -- we gotta have some alpha char on the line ---------------------------
   
   return TRUE;
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

# include "C-NDF-Rewrap-Tbl.h"

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   FData  *FD = (FData *)msg->FltData;
   STRPTR  Buff;
   ULONG   bol, QLevel, QLevelNext, LastPos, QChars, QCharsNext,
           SigLine, Indented, Outdented, PrevOutdented = TRUE, LineCnt,
           SkipLine = FALSE, ThisShort, NextShort, IsEP, IsE1, IsE2, IsBR,
           Hanging, PrevTblEnd = 0;
   TEXT    QP1[QPrefixLen], QP2[QPrefixLen];
   STRPTR  QP, QPNext, ThisLine, NextLine, *Lines;
   LONG    Cols[MaxColArray];

   if (TxBuff(msg->Tx)[TxLen(msg->Tx)-1] != '\n')
      DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx, "\n");
   
   Buff = TxBuff(msg->Tx);
   DoMethod(o, SWM_NDFilter_TBGetSpace, msg->Scratch, TxSize(msg->Tx));

   // -- skip headers --------------------------------------------------------

   LastPos = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   if (LastPos > 0)
      DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff, 0, LastPos-1);

   // -- collect line pointers -----------------------------------------------

   for (LineCnt=0, bol=LastPos;; bol = GetEOL(Buff, bol) + 1) {
      if (LineCnt >= FD->LineMax) {
         FD->LineMax += 250;
         if (NDF_RV((FD->LineMax+2) * sizeof(STRPTR), &FD->Lines) < 0)
            SWDB_RET(SWV_NDFilter_Incomplete);
      }

      if ((FD->Lines[LineCnt++] = Buff + bol)[0] == '\0') {
         FD->Lines[LineCnt] = FD->Lines[LineCnt-1];
         break;
      }
   }

   // -- loop through buffer, checking indent level of lines -----------------

   Lines      = FD->Lines;
   QP         = QP1;
   QP[0]      = '\0';
   QPNext     = QP2;
   QPNext[0]  = '\0';
   IsEP       = TRUE;
   LastPos    = 0;

   QLevel   = IndentLevel(idata, Lines, 0, QP, QPrefixLen, &QChars);

   for (LineCnt=0; *(ThisLine = Lines[LineCnt]); LineCnt++) {

      NextLine = Lines[LineCnt+1];

      // -- get indent level of next line ------------------------------------

      if (NextLine)
         QLevelNext = IndentLevel(idata, Lines, LineCnt+1, QPNext, QPrefixLen,
                                  &QCharsNext);
      else
         QLevelNext = 0, QPNext[0] = '\0', QCharsNext = 0;

      Outdented = (QLevelNext == QLevel && QCharsNext > QChars);
      Indented  = (QLevelNext == QLevel && QCharsNext < QChars);
      ThisLine += QChars;
      NextLine += QCharsNext;
      ThisShort = IsShortLine(idata, Lines+LineCnt,   QChars);
      NextShort = IsShortLine(idata, Lines+LineCnt+1, QCharsNext);
      IsBR      = 2;
      Hanging   = (Indented || Outdented) && IsEP &&
                  (IsEmptyLine(QP) && IsEmptyLine(QPNext));

      // -- see if we have a table -------------------------------------------

      if (!(IsE1 = IsEmptyLine(ThisLine)) &&
          IsTblBegin(idata, Lines, LineCnt, PrevTblEnd, Cols, QChars)) {
         ULONG TblLast = FindTblEnd(Lines, LineCnt, Cols);

         ReflowRegion(o, idata, FD, msg->Scratch, Lines, LastPos,
                      LineCnt, idata->BChars, QP);
               
         CopyNoFlow(o, idata, msg->Scratch, Lines, LineCnt, TblLast,
                    idata->SWA_NDF_Rewrap_Tbl_Font, PrevTblEnd);

         LineCnt    = TblLast;
         LastPos    = TblLast+1;
         PrevTblEnd = TblLast;
         SkipLine   = FALSE;

         QLevelNext = IndentLevel(idata, Lines, LineCnt+1, QPNext,
                                  QPrefixLen, &QCharsNext);
         Outdented  = FALSE;

         goto Next;
      }

      // -- don`t reflow in signature block ----------------------------------

      if (NextLine[0] == '-' && NextLine[1] == '-' &&
          ((NextLine[2]==' ' && NextLine[3]=='\n') || NextLine[2]=='\n')) {
         ULONG SigLines;

         // -- make sure we don`t have too many sig lines --------------------
         
         for (SigLines=0; *Lines[LineCnt+SigLines+1] && SigLines <= 9;
              SigLines++);
         
         SigLine = (SigLines <= 10);
      } else
         SigLine = FALSE;

      // -- see if we should break here --------------------------------------

      if (!SkipLine &&
          (QLevelNext != QLevel                      ||
           IsE1                                      ||
           (IsE2 = IsEmptyLine(NextLine))            ||
           ThisShort                                 ||
           NoASCII(ThisLine, IsE1)                   ||
           NoASCII(NextLine, IsE2)                   ||
           (strcmp(QP, QPNext) && !PrevOutdented)    ||
           Lines[LineCnt+1][0] == '\0'               ||
           SigLine                                   ||
           (Outdented && !Hanging)                   ||
           (NextShort &&
            (IsBR = IsBadWrap(idata, FD, Lines, LineCnt))) ||
           (idata->SWA_NDF_Rewrap_UseNoFlow &&
            MatchPattern(FD->PPat, NextLine)))) {

         if (IsBR >= 2)
            IsBR = IsBadWrap(idata, FD, Lines, LineCnt);
         
         if (IsBR && Lines[LineCnt+1][0] != '\0') {
            // -- skip the badwrap line --------------------------------------
            SkipLine = TRUE;
         } else {
            ReflowRegion(o, idata, FD, msg->Scratch, Lines, LastPos, LineCnt+1,
                         idata->BChars, QP);

            LastPos  = LineCnt+1;
            SkipLine = FALSE;
         }
      } else SkipLine = FALSE;

      if (SigLine) {
         DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
                  Lines[LastPos]-Buff, TxLen(msg->Tx));
         break;
      }

      // -- copy next quote info to this -------------------------------------

     Next:
      QLevel        = QLevelNext;
      QChars        = QCharsNext;

      PrevOutdented = Outdented;
      IsEP          = IsE1;
      
      if (QP == QP1) QP = QP2, QPNext = QP1;
      else           QP = QP1, QPNext = QP2;
   }

   // -- terminate the output buffer -----------------------------------------
   
   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   SWDB_RET(QLevel ? SWV_NDFilter_Incomplete : SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Modify buffer, no objects (text only)                             *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxtNoObjs, ULONG *)
{
   ULONG rc;

   // -- fake call to SWM_NDFilter_ModTxt, and set TextOnly flag -------------
   
   idata->TextOnly = TRUE;
   msg[0]          = SWM_NDFilter_ModTxt;
   
   rc = DoMethodA(o, (Msg)msg);

   msg[0]          = SWM_NDFilter_ModTxtNoObjs;
   idata->TextOnly = FALSE;
   
   SWDB_RET(rc);
} SDFT



/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   if (idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o)) {
      ULONG x, TblAttrs[] = {
         SWA_NDF_Rewrap_Tbl_MinRows,
         SWA_NDF_Rewrap_Tbl_MinCols,
         SWA_NDF_Rewrap_Tbl_MinTblProb,
         SWA_NDF_Rewrap_Tbl_MaxILines,
         SWP_NDF_StyleAttrVals(SWA_NDF_Rewrap_Tbl_),
         NULL,
      };
      
      SetUIToggle(idata->UI, SWA_NDF_Rewrap_NoFlow, SWA_NDF_Rewrap_UseNoFlow,
                  MUIV_NotTriggerValue, !idata->SWA_NDF_Rewrap_UseNoFlow);

      for (x=0; TblAttrs[x]; x++)
         SetUIToggle(idata->UI, TblAttrs[x], SWA_NDF_Rewrap_Tbl_Use,
                     MUIV_NotTriggerValue, !idata->SWA_NDF_Rewrap_Tbl_Use);
   }

   SWDB_RET(idata->UI);
} SDFT


/*****************************************************************************
 * Method: rewrap string using this filter                                   *
 *****************************************************************************/

METHOD(SWM_NDF_Rewrap_RewrapTx, SWP_NDF_Rewrap_RewrapTx *)
{
   TxtBuff Scratch;
   ULONG   FD, HasHeaders = FALSE;
   ULONG  *HH = msg->HasHeaders ? msg->HasHeaders : &HasHeaders;

   if (!msg->Tx) SWDB_RET(NULL);

   DoMethod(o, SWM_NDFilter_TBInit, &Scratch);

   if (DoMethod(o, SWM_NDFilter_StartMatch, NULL, NULL, NULL, &FD) == SUCCESS) {

      DoMethod(o, SWM_NDFilter_ModTxtNoObjs, msg->Tx, HH,
               NULL, NULL, NULL, FD, TRUE, &Scratch);
      
      DoMethod(o, SWM_NDFilter_EndMatch,   NULL, NULL, NULL, &FD);
   }

   DoMethod(o, SWM_NDFilter_TBDispose, &Scratch);
   
   SWDB_RET(msg->Tx);
} SDFT


