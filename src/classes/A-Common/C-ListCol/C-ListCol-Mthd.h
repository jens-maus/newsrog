
# define SWM_NOIDATA2
# define SWM_NOASSIGNAPP
# define SWM_USERPOSTASSIGN DoMethod(o, SWM_ListCol_BuildFmt);
# define SWM_USERREADPOST   DoMethod(o, SWM_ListCol_BuildFmt);

/*****************************************************************************
 * Function: Find position of string                                         *
 *****************************************************************************/

static LONG FindCol(SW_IDATA *idata, STRPTR Item)
{
   LONG x;

   for (x=0; idata->SWA_ListCol_ColNames[x]; x++)
      if (idata->SWA_ListCol_ColNames[x] == Item)
         return x;

   return -1;         
}


/*****************************************************************************
 * Function: Add new columns                                                 *
 *****************************************************************************/

static void AddNewCols(SW_IDATA *idata)
{
   LONG found, x, y;
   
   // -- put any new columns into the hidden list ----------------------------

   for (x=0; idata->SWA_ListCol_ColNames[x]; x++) {
      found = FALSE;

      for (y=0; Shown(y) >= 0 && !found; y++)
         if (Shown(y) == x) found = TRUE;

      for (y=0; idata->SWA_ListCol_HiddenCols[y] >= 0 && !found; y++)
         if (idata->SWA_ListCol_HiddenCols[y] == x) found = TRUE;

      if (!found)
         idata->SWA_ListCol_HiddenCols[y++] = x,
         idata->SWA_ListCol_HiddenCols[y]   = -1;
   }
}


/*****************************************************************************
 * Function: fix list position arrays                                        *
 *****************************************************************************/

static void FixEntries(SW_IDATA *idata, Object *l, BYTE *Cols)
{
   STRPTR Item;
   LONG   x;

   SW_MUIListLoop(l, x, Item, TRUE)
      Cols[x] = FindCol(idata, Item);

   Cols[x] = -1;
}

static void FixCols(SW_IDATA *idata)
{
   // -- rebuild Shown/Hidden data from lists --------------------------------
   
   FixEntries(idata, idata->L1, idata->SWA_ListCol_ShownCols);
   FixEntries(idata, idata->L2, idata->SWA_ListCol_HiddenCols);
}


/*****************************************************************************
 * Function: find the col that's the biggest when displayed                  *
 *****************************************************************************/

static LONG FindBiggestCol(Object *o, SW_IDATA *idata)
{
   LONG x, LCol = 0, Width, MaxWidth = 0;

   for (x=0; idata->SWA_ListCol_ColNames[x]; x++)
      if ((Width = strlen(idata->SWA_ListCol_ColNames[x])) > MaxWidth)
         MaxWidth = Width, LCol = x;

   strcat(strcpy(idata->BCol, idata->SWA_ListCol_ColNames[LCol]), "WM");
   
   return LCol;
}


/*****************************************************************************
 * Function: Generate list from entries                                      *
 *****************************************************************************/

static void RegenList(SW_IDATA *idata, Object *l, BYTE *SrcCol)
{
   ULONG x;
   STRPTR ColData[SWV_ListCol_MaxCol];

   set(l, MUIA_List_Quiet, TRUE);
   DoMethod(l, MUIM_List_Clear);

   for (x=0; SrcCol[x] >= 0; x++)
      ColData[x] = idata->SWA_ListCol_ColNames[SrcCol[x]];

   ColData[x] = NULL;
   
   DoMethod(l, MUIM_List_Insert, ColData, x, MUIV_List_Insert_Bottom);

   set(l, MUIA_List_Quiet, FALSE);
}


/*****************************************************************************
 * Function: Generate list from entries                                      *
 *****************************************************************************/

static void RegenAll(SW_IDATA *idata)
{
   if (!idata->SWA_ListCol_ColSpecs ||
       !idata->SWA_ListCol_ColNames) return;
   
   AddNewCols(idata);

   RegenList(idata, idata->L1, idata->SWA_ListCol_ShownCols);
   RegenList(idata, idata->L2, idata->SWA_ListCol_HiddenCols);

   idata->Setup = TRUE;
}


/*****************************************************************************
 * Function: return a single list                                            *
 *****************************************************************************/

static Object *GenList(Object *o, SW_IDATA *idata, ULONG lab,
                       Object **l, ULONG Help)
{
   if (!l) return HVSpace;

   return HGroup,
      GroupFrameT(GetS(lab)),
      Child, ListviewObject,
         MUIA_CycleChain,            1,
         MUIA_Dropable,              TRUE,
         MUIA_ShortHelp,             GetS(Help),
         MUIA_Listview_Input,        TRUE,
         MUIA_Listview_DragType,     MUIV_Listview_DragType_Immediate,
         MUIA_Listview_MultiSelect,  MUIV_Listview_MultiSelect_Default,
         MUIA_Listview_List, *l = ObjListObject,
            InputListFrame,
            SWA_ObjList_Object,      o,
            MUIA_List_DragSortable,  TRUE,
            idata->SWA_ListCol_FixWidth ? MUIA_FixWidthTxt : TAG_IGNORE,
                                     idata->BCol,
         End,
      End,
   End;
}


/*****************************************************************************
 * Method: Return UI for the object                                          *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l), GetS(h)); }

METHOD(SWM_ListCol_UI, Msg)
{
   Object *ui, *b_Res;

   if (!idata->Setup)
      InitColData(o, idata);

   FindBiggestCol(o, idata);

   ui = VGroup,
      MUIA_Background, MUII_GroupBack,
      GroupFrame,
         Child, HGroup,
         Child, idata->LV1 = GenList(o, idata, SWS_L_Disp, &idata->L1,
                                     SWS_H_Disp),
         Child, idata->LV2 = GenList(o, idata, SWS_L_NonDisp,
                                     &idata->L2, SWS_H_Hidden),
      End,
      Child, HGroup,
         MUIA_UserData,   1,
         Child, b_Res = MkBt(SWS_L_Reset, SWS_H_Reset),
      End,
   End;

   DoMethod(b_Res, NOTP, o, 1, SWM_ListCol_Reset);

   RegenAll(idata);
   
   SWDB_RET(ui);
} SDFT


/*****************************************************************************
 * Method: Build format string                                               *
 *****************************************************************************/

METHOD(SWM_ListCol_BuildFmt, Msg)
{
   STRPTR Fmt;
   LONG Len = 1, x;

   if (!idata->SWA_ListCol_ColSpecs ||
       !idata->SWA_ListCol_ColNames) SWDB_RET(NULL);

   // -- figure out how long it`s gonna be -----------------------------------
   
   for (x=0; Shown(x) >= 0; x++)
      Len += strlen(idata->SWA_ListCol_ColSpecs[Shown(x)])
         + 2;

   // -- allocate the space --------------------------------------------------
   
   if (!(Fmt = AllocVec(Len, MEMF_ANY))) SWDB_RET(NULL);

   FreeVec(idata->SWA_ListCol_Format);
   idata->SWA_ListCol_Format = "";
   
   // -- build the format string ---------------------------------------------
   
   Fmt[0] = '\0';

   for (x=0; Shown(x) >= 0; x++) {
      strcat(Fmt,
             idata->SWA_ListCol_ColSpecs[Shown(x)]);

      if (Shown(x+1) >= 0)
         strcat(Fmt, ",");
   }

   // -- nuke any trailing BAR -----------------------------------------------
   
   if ((Len = strlen(Fmt)) > 4)
      if (strcmp(Fmt + Len - 4, " BAR") == 0)
         Fmt[Len - 4] = '\0';

   set(o, SWA_ListCol_Format, Fmt);

   RegenAll(idata);
   
   SWDB_RET(Fmt);
} SDFT


/*****************************************************************************
 * Method: reset columns string                                              *
 *****************************************************************************/

METHOD(SWM_ListCol_Reset, Msg)
{
   InitColData(o, idata);
   RegenAll(idata);
   
   SWDB_RET(DoMethod(o, SWM_ListCol_BuildFmt));
} SDFT


/*****************************************************************************
 * Method: Drag Query                                                        *
 *****************************************************************************/

METHOD(SWM_ObjList_DragQuery, SWP_ObjList_DragQuery *)
{
   SWDB_RET(msg->obj == idata->L1 || msg->obj == idata->L2 ?
            MUIV_DragQuery_Accept : MUIV_DragQuery_Refuse);
} SDFT


/*****************************************************************************
 * Method: Drag Drop                                                         *
 *****************************************************************************/

METHOD(SWM_ObjList_DragDrop, SWP_ObjList_DragDrop *)
{
   LONG  x, DropMark = SWGetUL(msg->target, MUIA_List_DropMark);
   STRPTR Item;

   if (msg->obj != msg->target) {
      DoMethod(o, MUIM_MultiSet, MUIA_List_Quiet, TRUE, msg->obj,
               msg->target, NULL);
      
      SW_MUIListLoopSel(msg->obj, x, Item, TRUE)
         DoMethod(msg->target, MUIM_List_InsertSingle, Item, DropMark);
   
      DoMethod(msg->obj, MUIM_List_Remove, MUIV_List_Remove_Selected);
      
      DoMethod(o, MUIM_MultiSet, MUIA_List_Quiet, FALSE, msg->obj,
               msg->target, NULL);
   }

   FixCols(idata);
   DoMethod(o, SWM_ListCol_BuildFmt);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Find true click column (since MUI doesn't grok hidden cols)       *
 *****************************************************************************/

METHOD(SWM_ListCol_TrueCol, SWP_ListCol_TrueCol *)
{
   STRPTR ColSpec, p;

   if (msg->Col < 0 || Shown(msg->Col) < 0)
      SWDB_RET(-1);
   
   if (!(ColSpec = idata->SWA_ListCol_ColSpecs[Shown(msg->Col)]))
      SWDB_RET(-1);

   if (!(p = strstr(ColSpec, "COL"))) SWDB_RET(-1);

   p += sizeof("COL");

   SWDB_RET(atol(p));
} SDFT


/*****************************************************************************
 * Method: Popup for listcol                                                 *
 *****************************************************************************/

METHOD(SWM_ListCol_Popup, SWP_ListCol_Popup *)
{
   Object *g, *u1, *b_Close, *ui =PopobjectObject, 
      MUIA_Popstring_Button, SWUI_ImageBt(0,msg->Ctrl,BF_ListCol,msg->Help,0),
      MUIA_Popobject_Object, u1 = (Object *)DoMethod(o, SWM_ListCol_UI),
   End;

   if (g = (Object *)DoMethod(u1, MUIM_FindUData, 1)) {
      DoMethod(g, OM_ADDMEMBER, b_Close = MkBt(SWS_L_Close, SWS_H_Close));

      DoMethod(b_Close, NOTP, ui, 2, MUIM_Popstring_Close, TRUE);
   }
   
   SWDB_RET(ui);
} SDFT


/*****************************************************************************
 * Function: remove from column                                              *
 *****************************************************************************/

static void RemCol(BYTE *Cols, BYTE c)
{
   LONG y;
   
   for (y=0; Cols[y] >= 0; y++)
      if (Cols[y] == c) break;

   if (Cols[y] != c) return;
   
   for (; Cols[y] >= 0; y++)
      Cols[y] = Cols[y+1];
}


static void AddCol(BYTE *Cols, BYTE c)
{
   LONG y;
   
   for (y=0; Cols[y] >= 0; y++)
      if (Cols[y] == c) return;

   Cols[y++] = c;
   Cols[y]   = -1;
}


/*****************************************************************************
 * Method: Hide Columns                                                      *
 *****************************************************************************/

METHOD(SWM_ListCol_HideCols, LONG *)
{
   for (msg++; *msg >= 0; msg++) {
      RemCol(idata->SWA_ListCol_ShownCols,  *msg);
      AddCol(idata->SWA_ListCol_HiddenCols, *msg);
   }

   SWDB_RET(DoMethod(o, SWM_ListCol_BuildFmt));
} SDFT



/*****************************************************************************
 * Function: skip MUIX formatting codes                                      *
 *****************************************************************************/

STRPTR SkipFmt(STRPTR s)
{
   STRPTR is;
   LONG x;
   static STRPTR Fmt[] = { MUIX_R, MUIX_C, MUIX_L, MUIX_N, MUIX_B, MUIX_I,
                           MUIX_U, MUIX_PT, MUIX_PH, NULL };

   do {
      is = s;

      for (x=0; Fmt[x]; x++)
         if (!strncmp(s, Fmt[x], strlen(Fmt[x])))
            s += strlen(Fmt[x]);
   } while (is != s);

   return s;
}
   

/*****************************************************************************
 * Function: build data line                                                 *
 *****************************************************************************/

static STRPTR BuildClipLine(SW_IDATA *idata, STRPTR *Array, STRPTR Line,
                            UWORD *ColWidths, UBYTE *TrueCol)
{
   ULONG  x;
   STRPTR lpos;
   TEXT   fmt[12];
   
   for (x=0, lpos = Line; Shown(x) >= 0; x++) {
      STRPTR Data = SkipFmt(Array[TrueCol[x]]), rj;

      if (ColWidths[x] == SHRT_MAX) continue;

      // -- skip icons and stuff --------------
      if (!strncmp(Data, "\33O[", 3)) continue;

      rj = strstr(idata->SWA_ListCol_ColSpecs[Shown(x)], "P=" MUIX_R);
         
      sprintf(fmt, "%%%s%lds%%s", rj ? "" : "-", ColWidths[x]);
      sprintf(lpos, fmt, Data, idata->SWA_ListCol_ClipColSep);

      lpos += strlen(lpos);
   }

   lpos[-strlen(idata->SWA_ListCol_ClipColSep)] = '\0';

   return Line;
}


/*****************************************************************************
 * Method: Copy to clipboard                                                 *
 *****************************************************************************/

METHOD(SWM_ListCol_Clip, SWP_ListCol_Clip *)
{
   struct IFFHandle *iff;
   Object *Item;
   LONG   x, y, TotSize, MaxCol = 0;
   UWORD  ColWidths[48];
   STRPTR Array[48], Line;
   UBYTE  TrueCol[48];
   Object *App = msg->ClipBt ? _app(msg->ClipBt) : NULL;

   if (!idata->SWA_ListCol_Hook || !idata->SWA_ListCol_List)
      SWDB_RET(FAILURE);

   if (App) set(App, MUIA_Application_Sleep, TRUE);

   memset(ColWidths, 0, sizeof(ColWidths));

   for (x=0; Shown(x) >= 0; x++, MaxCol = x)
      TrueCol[x] = DoMethod(o, SWM_ListCol_TrueCol, x);
         
   // -- figure out maximum widths for column --------------------------------
   
   if (idata->SWA_ListCol_ClipTitles) {
      CallHookPkt(idata->SWA_ListCol_Hook, Array, NULL);
      for (x=0; x < MaxCol; x++)
         ColWidths[x] = max(ColWidths[x],
                            strlen(SkipFmt(Array[TrueCol[x]])));
   }
         
   SW_MUIListLoopSel(idata->SWA_ListCol_List, y, Item, TRUE) {
      CallHookPkt(idata->SWA_ListCol_Hook, Array, Item);
      for (x=0; x < MaxCol; x++) {
         STRPTR Data = SkipFmt(Array[TrueCol[x]]);
         if (!strncmp(Data, "\33O[", 3))
            ColWidths[x] = SHRT_MAX;
         else
            ColWidths[x] = max(ColWidths[x], strlen(Data));
      }
   }

   for (x=0, TotSize=2; x < MaxCol; x++)
      TotSize += (ColWidths[x]==SHRT_MAX ? 0 : ColWidths[x])
         + strlen(idata->SWA_ListCol_ClipColSep);

   if (!(Line = AllocVec(TotSize, MEMF_ANY))) goto Done;

   if (!(iff = SWOpenClipWrite(0))) goto Done;
   
   // -- now copy items to clipboard -----------------------------------------

   if (idata->SWA_ListCol_ClipTitles) {
      CallHookPkt(idata->SWA_ListCol_Hook, Array, NULL);
      if (SWTextToClip(iff, BuildClipLine(idata, Array, Line,
                                          ColWidths, TrueCol)) ||
          SWTextToClip(iff, "\n") != SUCCESS)
         goto Done;
   }

   SW_MUIListLoopSel(idata->SWA_ListCol_List, y, Item, TRUE) {
      CallHookPkt(idata->SWA_ListCol_Hook, Array, Item);

      if (SWTextToClip(iff, BuildClipLine(idata, Array, Line, ColWidths,
                                          TrueCol))
          != SUCCESS ||
          SWTextToClip(iff, "\n") != SUCCESS)
         goto Done;
   }

  Done:
   FreeVec(Line);
   SWCloseClipWrite(iff);
   if (App) set(App, MUIA_Application_Sleep, FALSE);
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Popup for clip button                                             *
 *****************************************************************************/

METHOD(SWM_ListCol_ClipBt, SWP_ListCol_ClipBt *)
{
   Object *b_Close, *b_Clip, *ui =PopobjectObject, 
      MUIA_Popstring_Button, SWUI_ImageBt(0,msg->Ctrl,BF_Clip,msg->Help,0),
      MUIA_Popobject_Object, VGroup,
         GroupFrame,
         MUIA_Background, MUII_GroupBack,
         Child, ColGroup(2),
            Child, Label2(GetS(SWS_L_Titles)),
            Child, HGroup,
               Child, idata->b_Title = CheckMark(idata->SWA_ListCol_ClipTitles),
               Child, HSpace(0),
            End,
            Child, Label2(GetS(SWS_L_Sep)),
            Child, idata->b_ColSep = StringObject,
               StringFrame,
               MUIA_ShortHelp,       GetS(SWS_H_Sep),
               MUIA_String_Contents, idata->SWA_ListCol_ClipColSep,
               MUIA_FixWidthTxt,     "abcdefghijkl",
               MUIA_CycleChain,      1,
               MUIA_String_MaxLen,   31,
            End,
         End,
         Child, HGroup,
            Child, b_Clip  = MkBt(SWS_L_Clip,  SWS_H_Clip),
            Child, b_Close = MkBt(SWS_L_Close, SWS_H_Close),
         End,
      End,
   End;

   SetAttrs(idata->b_Title,
            MUIA_ShortHelp,  GetS(SWS_H_Titles),
            MUIA_CycleChain, 1,
            TAG_DONE);

   SW_NOTIFYUPD(idata->b_Title, MUIA_Selected,       o,SWA_ListCol_ClipTitles);
   SW_NOTIFYUPD(idata->b_ColSep,MUIA_String_Contents,o,SWA_ListCol_ClipColSep);
   
   DoMethod(b_Close, NOTP, ui, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_Clip,  NOTP, ui, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_Clip,  NOTP, o,  2, SWM_ListCol_Clip, ui);
   
   SWDB_RET(ui);
} SDFT


/*****************************************************************************
 * Method: Popup for clip button                                             *
 *****************************************************************************/

METHOD(SWM_ListCol_RemNotify, Msg)
{
   if (idata->b_Title) {
      SW_KILLNOTIFYUPD(idata->b_Title, MUIA_Selected,
                       o,SWA_ListCol_ClipTitles);
   }

   if (idata->b_ColSep) {
      SW_KILLNOTIFYUPD(idata->b_ColSep,MUIA_String_Contents,
                       o,SWA_ListCol_ClipColSep);
   }

   DoMethod(o, MUIM_KillNotifyObj,SWA_ListCol_Format,idata->SWA_ListCol_List);
   
   SWDB_RET(SUCCESS);
} SDFT
