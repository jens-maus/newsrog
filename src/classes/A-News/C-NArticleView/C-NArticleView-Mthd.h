# include "S-FastStrStr.h"

/*****************************************************************************
 * Function: return font for this article                                    *
 *****************************************************************************/

static STRPTR VFont(SW_IDATA *idata)
{
   if (idata->SWA_NArticleView_UseArtFont &&
       !SW_STRISNULL(idata->SWA_NArticleView_ArtFont))
      return idata->SWA_NArticleView_ArtFont;
   
   return NN_VFont(idata);
}


/*****************************************************************************
 * Function: return color for this article                                   *
 *****************************************************************************/

static void *VColor(SW_IDATA *idata)
{
   if (idata->SWA_NArticleView_UseArtCol)
      return &idata->SWA_NArticleView_Pens[PEN_ArtCol];
   
   return NN_VColor(idata);
}


/*****************************************************************************
 * Function: generate text                                                   *
 *****************************************************************************/

static Object *TxtObj(SW_IDATA *idata, STRPTR s, Object **Prev)
{
   *Prev = AV_CTextObject,
      MUIA_HorizWeight,      1,
      SWA_CText_MemPool,     idata->MemPool,
      SWA_CText_PrevCText,   *Prev,
      SWA_CText_LineSpacing, idata->SWA_NArticleView_LineSpacing,
      SWA_CText_Contents,    s,
      SWA_CText_DefFont,     VFont(idata),
      SWA_CText_DefPen,      VColor(idata),
      SWA_CText_AutoClip,    NN_AutoClip(idata),
   End;

   if (!idata->FirstTxt)
      DoMethod(idata->FirstTxt = *Prev, MUIM_Notify,
               SWA_CText_NumSelections, MUIV_EveryTime,
               idata->o, 2, SWM_NArticleView_SelChange, MUIV_TriggerValue);

   return *Prev;
}


/*****************************************************************************
 * Function: Find ordinal position of filter in flt list                     *
 *****************************************************************************/

static ULONG FltListData(SW_IDATA *idata, Object *DFlt)
{
   ULONG x;
   Object *flt;

   // -- find filter data for a given filter ---------------------------------

   SW_MUIListLoop(idata->FltList, x, flt, TRUE)
      if (flt == DFlt)
         return idata->FltData ? idata->FltData[x] : NULL;

   return 0;
}

/*****************************************************************************
 * Function: add space                                                       *
 *****************************************************************************/

static void AddSpace(SW_IDATA *idata)
{
   if (idata->Empty || !idata->VGroupObj) return;
   
   DoMethod(idata->VGroupObj, OM_ADDMEMBER, idata->Empty = HVSpace);
}


/*****************************************************************************
 * Function: remove space                                                    *
 *****************************************************************************/

static void RemSpace(SW_IDATA *idata)
{
   if (!idata->Empty || !idata->VGroupObj) return;

   DoMethod(idata->VGroupObj, OM_REMMEMBER, idata->Empty);
   MUI_DisposeObject(idata->Empty);
   idata->Empty = NULL;
}


/*****************************************************************************
 * Function: clear object list                                               *
 *****************************************************************************/

static void ClearObjList(Object *o, SW_IDATA *idata)
{
   LONG x;
   
   if (idata->ObjList)
      for (x=0; x<idata->ObjListCnt; x++)
         DoMethod(idata->ObjList[x].NDF, SWM_NDFilter_RemAddObj,
                  idata->ObjList[x].Data, o,
                  FltListData(idata, idata->ObjList[x].NDF));
   
   FV(idata->ObjList);
   
   idata->ObjList     = NULL;
   idata->ObjListMax  =
   idata->ObjListCnt  = 0;
}


/*****************************************************************************
 * Method: Clear the reader group objects                                    *
 *****************************************************************************/

METHOD(SWM_NArticleView_Clear, Msg)
{
   APTR state; Object *obj;
   List *l;

   idata->PrevParseComplete = FALSE;
   idata->PrevParseLine     = 0;
   idata->PrevObjCnt        = 0;
   idata->FirstTxt          = NULL;

   DoMethod(idata->VGroupObj, MUIM_Group_InitChange);

   RemSpace(idata);
   
   SW_GETLH(idata->VGroupObj, l);
   SW_LISTLOOP(l, state, obj) {
      DoMethod(idata->VGroupObj, OM_REMMEMBER, obj);
      MUI_DisposeObject(obj);
   }

   AddSpace(idata);

   DoMethod(idata->VGroupObj, MUIM_Group_ExitChange);

   idata->SWA_NArticleView_DspArt =
   idata->SWA_NArticleView_DspGrp = NULL;

   // -- remove context menu -------------------------------------------------
  
#  ifdef DYNAMIC_MENUS 
   set(idata->VGroupObj, MUIA_ContextMenu, NULL);
#  endif

   set(idata->Title, MUIA_String_Contents, "");

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: Call start/endmatch on all filters                              *
 *****************************************************************************/

static ULONG StartEndMatch(Object *o, SW_IDATA *idata, ULONG Mth, Object *Art,
                           Object *Grp)
{
   ULONG x, rc = SUCCESS;
   Object *flt;

   // -- call start/end on each filter, tracking data if start ---------------
      
   SW_MUIListLoop(idata->FltList, x, flt, TRUE) {
      if (Mth == SWM_NDFilter_StartMatch)
         SetAttrs(flt,
                  AppBaseDefTop(idata),
                  SWA_NDFilter_MemPool, idata->MemPool,
                  TAG_DONE);
      
      if (DoMethod(flt, Mth, Art, Grp, o,
                   idata->FltData ? &idata->FltData[x] : NULL) == FAILURE)
         rc = FAILURE;
   }

   return rc;
}


/*****************************************************************************
 * Function: make text object from buffer lines, concating separated headers *
 * ala:                                                                      *
 *   References: <foo>                                                       *
 *     <bar>                                                                 *
 * ->                                                                        *
 *   References: <foo> <bar>                                                 *
 *****************************************************************************/

static TxtBuff *MkBuffTxt(SW_IDATA *idata, TxtBuff *Tx, STRPTR *Lines,
                          LONG StLine, LONG StCol, LONG EndLine, LONG EndCol)
{
   LONG    x, sc, ec, len, InHdr = (StLine == 0);
   UBYTE   c;

   if (EndCol < 0) EndCol = strlen(Lines[EndLine]);

   DoMethod(idata->NDF, SWM_NDFilter_TBGetSpace, Tx, (EndLine-StLine)*64);
   
   for (x=StLine; x<=EndLine; x++) {
      ec = len = strlen(Lines[x]), sc = 0;

      if (x==StLine)   sc = min(StCol, len);
      if (x==EndLine)  ec = min(EndCol, len), ec = max(ec, sc);

      c = Lines[x][ec], Lines[x][ec] = '\0';

      DoMethod(idata->NDF, SWM_NDFilter_TBAppendTxt, Tx, Lines[x]+sc);

      if (c == '\0')
         if (!(InHdr && IsWhite(Lines[x+1][0])))
            DoMethod(idata->NDF, SWM_NDFilter_TBAppendTxt, Tx, "\n");

      // -- keep track of whether we`re in the header ------------------------
      if (InHdr && c=='\0' && Lines[x+1][0] == '\0')
         InHdr = FALSE;

      Lines[x][ec] = c;
   }

   return Tx;
}


/*****************************************************************************
 * Function: get last line to try parsing to                                 *
 *****************************************************************************/

static ULONG GetParseLast(SW_IDATA *idata, STRPTR *Lines,
                          ULONG First, ULONG Last, ULONG EndOfData)
{
   Object *flt;
   ULONG  x, LPos, LPosMin = Last;

   SW_MUIListLoop(idata->FltList, x, flt, TRUE) {
      LPos = DoMethod(flt, SWM_NDFilter_LastParseLine, Lines, First, LPosMin,
                      idata->FltData ? idata->FltData[x] : NULL, EndOfData);
      if (LPos < LPosMin) LPosMin = LPos;
   }

   return LPosMin;
}


/*****************************************************************************
 * Method: Run text by all filters                                           *
 *****************************************************************************/

METHOD(SWM_NArticleView_GenTxt, SWP_NArticleView_GenTxt *)
{
   ULONG  x, HasHeaders = msg->HasHeaders, Complete, ScratchInit = FALSE;
   Object *flt;
   TxtBuff Scratch;

   // -- loop through filters, calling each on the buffer --------------------

   if (msg->ParseComplete) *msg->ParseComplete = TRUE;

   SW_MUIListLoop(idata->FltList, x, flt, TRUE) {

      // -- create a scratch buffer for filters to use -----------------------

      if (!ScratchInit) {
         DoMethod(idata->NDF, SWM_NDFilter_TBInit, &Scratch);
         DoMethod(idata->NDF, SWM_NDFilter_TBGetSpace, &Scratch, TxLen(msg->Tx));
         ScratchInit = TRUE;
      }

      DoMethod(flt, SWM_NDFilter_TBClear, &Scratch);

      // -- call the filter to modify the text -------------------------------

      Complete = DoMethod(flt, msg->Mthd, msg->Tx, &HasHeaders,
                          msg->Art, msg->Grp, o,
                          idata->FltData ? idata->FltData[x] : NULL,
                          msg->EndOfData, &Scratch);

      if (Complete == SWV_NDFilter_Incomplete)
         if (msg->ParseComplete)
            *msg->ParseComplete = FALSE;

      if (!TxLen(msg->Tx)) break;

      // -- make sure buffer is newline terminated ---------------------------
      
      if (TxBuff(msg->Tx)[TxLen(msg->Tx)-1] != '\n')
         DoMethod(idata->NDF, SWM_NDFilter_TBAppendTxt, msg->Tx, "\n");
   }

   // -- nuke the scratch buffer ---------------------------------------------

   if (ScratchInit)
      DoMethod(idata->NDF, SWM_NDFilter_TBDispose, &Scratch);

   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: Add objects for a particular set of lines                         *
 *****************************************************************************/

static ULONG AddObjs(Object *o, SW_IDATA *idata, Object *Art, Object *Grp,
                     STRPTR *Lines, ULONG First, ULONG Last, ULONG EndOfData)
{
   TxtBuff Tx;
   ULONG   Complete, x, Line, OldCount = 0;
   UBYTE   c, c1;
   APTR state; Object *SObj;
   List *l;

   _PROFINIT(0)

   _ProfBegin(__FUNC__);
   
   DoMethod(idata->NDF, SWM_NDFilter_TBInit, &Tx);

   _ProfCheck(__FUNC__ ": After Init");
   
   // -- Generate object to add ----------------------------------------------

   MkBuffTxt(idata, &Tx, Lines, First, 0, Last, -1);

   _ProfCheck(__FUNC__ ": After MkBuffTxt");

   DoMethod(o, SWM_NArticleView_GenTxt, &Tx, First == 0, Art, Grp, &Complete,
            EndOfData, SWM_NDFilter_ModTxt);

   _ProfCheck(__FUNC__ ": After GenTxt");

   // -- find first nonheader line after filter mods -------------------------

   for (x=Line=0, c='\0'; c1=TxBuff(&Tx)[x]; x++, c=c1)
      if (c == '\n') {
         if (++Line >= 30) break;
         
         if (c1 == '\n') {
            idata->SWA_NArticleView_FirstBodyLine = Line+2;
            break;
         }
      }
      
   // -- add it`s contents to our viewer group -------------------------------

   _ProfCheck(__FUNC__ ": Before GenObj");

   SW_GETLH(idata->VGroupObj, l);
   SW_LISTLOOP(l, state, SObj)
      OldCount++;

   if (TxLen(&Tx) &&
       DoMethod(o, SWM_NArticleView_GenObj, &Tx, idata->VGroupObj)) {
      if (!Complete) {
         x = 0;
         _ProfCheck(__FUNC__ ": Before ListLoop");
         SW_LISTLOOP(l, state, SObj) {
            if (++x > OldCount) {
               if (idata->PrevObjCnt >= idata->PrevObjMax) {
                  idata->PrevObjMax += 16;
                  if (RV(idata->PrevObjMax * sizeof(Object *),
                         &idata->PrevObjs) < 0)
                     return Complete;
               }

               idata->PrevObjs[idata->PrevObjCnt++] = SObj;
            }
         }
      }
   }

   _ProfCheck(__FUNC__ ": After ListLoop");

   DoMethod(idata->NDF, SWM_NDFilter_TBDispose, &Tx);

   _ProfCheck(__FUNC__ ": After Dispose");
   
   _PROFTERM;

   return Complete;
}

   
/*****************************************************************************
 * Method: parse region                                                      *
 *****************************************************************************/

static void TryParseReg(Object *o, SW_IDATA *idata, Object *Art, Object *Grp,
                        STRPTR *Lines, ULONG First, ULONG Last, ULONG EndOfData)
{
   ULONG Comp;
   
   Comp = AddObjs(o, idata, Art, Grp, Lines, First, Last, EndOfData);

   if (!Comp)
      idata->PrevParseComplete = FALSE;
   else
      idata->PrevParseLine     = Last + 1;
}


/*****************************************************************************
 * Method: Add lines from article                                            *
 *****************************************************************************/

METHOD(SWM_NArticleView_NewLines, SWP_NArticleView_NewLines *)
{
   _PROFINIT(1)
   ULONG x, ParseLastTry;

   if (!idata->FltData) SWDB_RET(FAILURE);
   
   _ProfBegin(__FUNC__);

   // -- Remove trailing space from vgroup -----------------------------------

   DoMethod(idata->VGroupObj, MUIM_Group_InitChange);
   RemSpace(idata);

   _ProfCheck(__FUNC__ ": Before Remove Objs");

   // -- remove previous objects that were incomplete ------------------------
   
   if (!idata->PrevParseComplete)
      for (x=0; x<idata->PrevObjCnt; x++) {
         DoMethod(idata->VGroupObj, OM_REMMEMBER, idata->PrevObjs[x]);
         MUI_DisposeObject(idata->PrevObjs[x]);
      }

   idata->PrevObjCnt              = 0;
   idata->PrevParseComplete       = TRUE;

   // -- find last line to try parsing to ------------------------------------

   _ProfCheck(__FUNC__ ": Before GetParseLast");

   ParseLastTry = GetParseLast(idata, msg->Lines, idata->PrevParseLine,
                               msg->LastNew, msg->EndOfData);
   
   // -- add the region, either in one chunk if we think we can parse --------
   // -- the whole shebang, or in two, breaking at the last point we ---------
   // -- think we can parse up to. -------------------------------------------

   _ProfCheck(__FUNC__ ": Before TryParseReg");

   if (ParseLastTry > idata->PrevParseLine && ParseLastTry < msg->LastNew && 
       !msg->EndOfData) {
      TryParseReg(o, idata, msg->Article, msg->Group, msg->Lines,
                  idata->PrevParseLine, ParseLastTry, FALSE);
      
      TryParseReg(o, idata, msg->Article, msg->Group, msg->Lines,
                  ParseLastTry + 1, msg->LastNew, msg->EndOfData);
   } else {
      TryParseReg(o, idata, msg->Article, msg->Group, msg->Lines,
                  idata->PrevParseLine, msg->LastNew, msg->EndOfData);
   }

   _ProfCheck(__FUNC__ ": After TryParseReg");

   AddSpace(idata);
   DoMethod(idata->VGroupObj, MUIM_Group_ExitChange);

   _ProfCheck(__FUNC__ ": After ExitChange");

   // -- clear object list ---------------------------------------------------

   ClearObjList(o, idata);

   _ProfCheck(__FUNC__ ": After ClearObjList");

   _PROFTERM;
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Sort hook for list                                                        *
 *****************************************************************************/

static __saveds __asm LONG FltCmpFn(a1 Object *o1, a2 Object *o2,
                                    a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);
   
   return SWGetLong(o1, SWA_NDFilter_Pri) - SWGetLong(o2, SWA_NDFilter_Pri);
}


/*****************************************************************************
 * Method: Insert filters into our list                                      *
 *****************************************************************************/

static ULONG InsertFilters(SW_IDATA *idata, ULONG Attr)
{
   ULONG x;
   Object *Flt;

   if (idata->FltList) {
      DoMethod(idata->FltList, MUIM_List_Clear);
   } else {
      idata->FltCmpHook.h_Entry = (ULONG (*)())FltCmpFn;
      idata->FltCmpHook.h_Data  = idata;

      idata->FltList = ListObject,
         MUIA_List_CompareHook, &idata->FltCmpHook,
      End;
   }

   if (!idata->RawFlag) {
      SW_MUIListLoop(idata->SWA_NArticleView_DspList, x, Flt, TRUE)
         if (SWGetUL(Flt, Attr))
            DoMethod(idata->FltList, MUIM_List_InsertSingle, Flt,
                     MUIV_List_Insert_Sorted);
      
      SW_MUIListLoop(idata->SWA_NArticleView_DspGlob, x, Flt, TRUE)
         if (SWGetUL(Flt, Attr))
            DoMethod(idata->FltList, MUIM_List_InsertSingle, Flt,
                     MUIV_List_Insert_Sorted);
   }
   
   return SWGetUL(idata->FltList, MUIA_List_Entries);
}


/*****************************************************************************
 * Function: Allocate filter data for some number of filters                 *
 *****************************************************************************/

static ULONG AllocFltData(SW_IDATA *idata, ULONG FltCnt)
{
   if (!(idata->FltData = (ULONG *)AV((FltCnt+1) * sizeof(ULONG))))
      return FAILURE;
      
   memset(idata->FltData, 0, (FltCnt+1) * sizeof(ULONG));

   return SUCCESS;
}


/*****************************************************************************
 * Function: Free filter data                                                *
 *****************************************************************************/

static void FreeFltData(SW_IDATA *idata)
{
   FV(idata->FltData);
   idata->FltData = NULL;
}



/*****************************************************************************
 * Method: Begin article                                                     *
 *****************************************************************************/

METHOD(SWM_NArticleView_Begin, SWP_NArticleView_Begin *)
{
   DoMethod(o, SWM_NArticleView_Clear);

   idata->SWA_NArticleView_DspArt      = msg->Article;
   idata->SWA_NArticleView_DspGrp      = msg->Group;
   idata->SWA_NArticleView_PendingDisp = TRUE;

#  ifdef DYNAMIC_MENUS 
   set(idata->VGroupObj, MUIA_ContextMenu, NULL);
#  endif

   // -- kludge to set artcol from group, since 2-dir notify was busted ------

   if (msg->Group && idata->SWA_NArticleView_UseArtCol)
      set(o, SWA_NArticleView_ArtCol, SWGetObj(msg->Group, SWA_NGroup_ArtCol));

   AllocFltData(idata,
                InsertFilters(idata, SWA_NDFilter_Enabled));

   DoMethod(o, MUIM_MultiSet, MUIA_Disabled, FALSE,
            idata->b_Up, idata->b_Dn, NULL);


   // -- call StartMatch on the filters --------------------------------------
      
   if (StartEndMatch(o, idata, SWM_NDFilter_StartMatch,
                     idata->SWA_NArticleView_DspArt,
                     idata->SWA_NArticleView_DspGrp) == FAILURE) {
      DoMethod(o, SWM_NArticleView_End);
      SWDB_RET(0);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: End article                                                       *
 *****************************************************************************/

METHOD(SWM_NArticleView_End, Msg)
{
   StartEndMatch(o, idata, SWM_NDFilter_EndMatch,
                 idata->SWA_NArticleView_DspArt,
                 idata->SWA_NArticleView_DspGrp);

   DoMethod(idata->FltList, MUIM_List_Clear);

   FreeFltData(idata);

   // -- set title --------------------------------------------------------

   if (!SW_STRISNULL(idata->SWA_NArticleView_TitleTxt)) {
      STRPTR t = VL_Alloc(o, idata, idata->SWA_NArticleView_TitleTxt);

      set(idata->Title, MUIA_Text_Contents, t);

      VL_Free(o, idata, t);
   }

   idata->SWA_NArticleView_PendingDisp = FALSE;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: add incomplete marker                                           *
 *****************************************************************************/

static void AddIncomplete(Object *Grp, SW_IDATA *idata)
{
   if (!Grp) return;

   DoMethod(Grp, MUIM_MultiSet, MUIA_Disabled, TRUE,
            idata->b_Up, idata->b_Dn, NULL);
   
   DoMethod(Grp, OM_ADDMEMBER,
            HGroup,
               Child, MUI_MakeObject(MUIO_HBar,4),
               Child, TextObject,
                  MUIA_HorizWeight,   1,
                  MUIA_Text_Contents, GetS(SWS_L_Incomplete),
               End,
               Child, MUI_MakeObject(MUIO_HBar,4),
            End);
}


/*****************************************************************************
 * Method: Abort article                                                     *
 *****************************************************************************/

METHOD(SWM_NArticleView_Abort, Msg)
{
   DoMethod(o, SWM_NArticleView_End);

   DoMethod(idata->VGroupObj, MUIM_Group_InitChange);
     AddIncomplete(idata->VGroupObj, idata);
   DoMethod(idata->VGroupObj, MUIM_Group_ExitChange);

   SWDB_RET(0);
} SDFT



/*****************************************************************************
 * Method: Clip text                                                         *
 *****************************************************************************/

METHOD(SWM_NArticleView_Clip, Msg)
{
   if (!idata->VGroupObj)          SWDB_RET(0);
   
   DoMethod(idata->FirstTxt,  SWM_CText_Clip, SWV_CText_Sel_Last);
   DoMethod(idata->VGroupObj, SWM_CText_Unsel, SWV_CText_Unsel_All);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: forward method to currently displayed article                     *
 *****************************************************************************/

METHOD(SWM_NArticleView_ArtFwd, ULONG *)
{
   if (!idata->SWA_NArticleView_DspArt ||
       !idata->SWA_NArticleView_DspGrp) SWDB_RET(NULL);

   SWDB_RET(DoMethodA(idata->SWA_NArticleView_DspArt, (Msg)(msg+1)));
} SDFT


/*****************************************************************************
 * Method: forward method to local display filter (for buffer fns etc)       *
 *****************************************************************************/

METHOD(SWM_NArticleView_DFwd, ULONG *)
{
   SWDB_RET(DoMethodA(idata->NDF, (Msg)(msg+1)));
} SDFT


/*****************************************************************************
 * Method: select all text                                                   *
 *****************************************************************************/

METHOD(SWM_NArticleView_SelectAll, Msg)
{
   if (!idata->VGroupObj) SWDB_RET(0);
   
   SWDB_RET(DoMethod(idata->VGroupObj, SWM_CText_SelAll));
} SDFT


/*****************************************************************************
 * Method: unselect all text                                                 *
 *****************************************************************************/

METHOD(SWM_NArticleView_UnselectAll, Msg)
{
   if (!idata->VGroupObj) SWDB_RET(0);
   
   SWDB_RET(DoMethod(idata->VGroupObj, SWM_CText_Unsel, SWV_CText_Unsel_All));
} SDFT


/*****************************************************************************
 * Method: Open search window                                                *
 *****************************************************************************/

METHOD(SWM_NArticleView_SWin, Msg)
{
   if (idata->QSearch)
      DoMethod(idata->QSearch, SWM_ListSearch_OpenWin, _app(o));

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: scroll by a given distance                                      *
 *****************************************************************************/

static ULONG ScrollVG(SW_IDATA *idata, LONG VDist, LONG HDist, LONG Soft)
{
   ULONG x;
   static UWORD SoftScroll[] = {
      1,     4,   9,  16,  24,  35,  48,  62,  78,  95,
      115, 136, 158, 181, 206, 232, 259, 287, 316, 345,
      376, 406, 437, 469, 500, 531, 563, 594, 624, 655,
      684, 713, 741, 768, 794, 819, 842, 864, 885, 905,
      922, 938, 952, 965, 976, 984, 991, 996, 999,   0,
        0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   };

   ULONG Top, Height, Left, Width, SS = NN_SoftScroll(idata);
   LONG  SoftH = Soft,
         SoftV = Soft;
   
   if (!idata->VGroupObj ||
       !idata->SWA_NArticleView_DspArt)
      SWDB_RET(SWV_NArticleView_Browse_NoArticle);

   if (VDist <= idata->LineHeight) SoftV = FALSE;
   if (HDist <= 20)                SoftH = FALSE;
   
   SS = 11 - min(SS, 10);

   if (!VDist && !HDist)
      SWDB_RET(SWV_NArticleView_Browse_NoOp);

   // -- handle vertical scroll ----------------------------------------------
   
   if (VDist) {
      Top    = SWGetUL(idata->VGroupObj, MUIA_Virtgroup_Top);
      Height = SWGetUL(idata->VGroupObj, MUIA_Virtgroup_Height);

      if (VDist > 0  && Top + _mheight(idata->VGroupObj) >= Height ||
          VDist < 0  && Top == 0)
         SWDB_RET(SWV_NArticleView_Browse_End);

      // -- limit scroll dist if bigger than we can do -----------------------
   
      if (VDist > 0 &&
          VDist > (Height - _mheight(idata->VGroupObj) - Top))
         VDist = Height - _mheight(idata->VGroupObj) - Top;
      else if (VDist < 0 && abs(VDist) > Top)
         VDist = -Top;

      // -- scroll softly ----------------------------------------------------
      
      if (SS <= 10 && abs(VDist) > 8 && SoftV)
         for (x=0; SoftScroll[x]; x+=SS) {
            WaitTOF();
            set(idata->VGroupObj, MUIA_Virtgroup_Top,
                Top + VDist * (LONG)SoftScroll[x] / 1000);
         }
   
      set(idata->VGroupObj, MUIA_Virtgroup_Top, Top + VDist);
   }

   // -- handle horizontal scroll --------------------------------------------
   
   if (HDist) {
      Left   = SWGetUL(idata->VGroupObj, MUIA_Virtgroup_Left);
      Width  = SWGetUL(idata->VGroupObj, MUIA_Virtgroup_Width);

      if (!(HDist > 0  && Left + _mleft(idata->VGroupObj) >= Width ||
            HDist < 0  && Left == 0)) {

         if (SS <= 10 && abs(HDist) > 8 && SoftH)
            for (x=0; SoftScroll[x]; x+=SS) {
               WaitTOF();
               set(idata->VGroupObj, MUIA_Virtgroup_Left,
                   Left + HDist * (LONG)SoftScroll[x] / 1000);
            }

         set(idata->VGroupObj, MUIA_Virtgroup_Left, Left + HDist);
      }
   }

   SWDB_RET(SWV_NArticleView_Browse_Move);
}


/*****************************************************************************
 * Method: Scroll the view                                                   *
 *****************************************************************************/

METHOD(SWM_NArticleView_Scroll, SWP_NArticleView_Scroll *)
{
   if (!idata->LineHeight) {
      STRPTR f = strchr(VFont(idata), '/');

      idata->LineHeight = f ? atol(f+1) : 8;

      if (!idata->LineHeight) idata->LineHeight = 8;

      idata->LineHeight += idata->SWA_NArticleView_LineSpacing;
   }

   SWDB_RET(ScrollVG(idata, msg->Lines * idata->LineHeight, msg->Cols, FALSE));
} SDFT


/*****************************************************************************
 * Method: Make a certain object visible, if possible                        *
 *****************************************************************************/

METHOD(SWM_NArticleView_MakeObjVis, SWP_NArticleView_MakeObjVis *)
{
   ULONG RelTop, RelBot;
   
   if (!idata->VGroupObj) SWDB_RET(0);

   RelTop = _mtop(msg->o) - _mtop(idata->VGroupObj);
   RelBot = RelTop + _mheight(msg->o);

   // -- if it won`t fit in scroll region, move to top -----------------------
   
   if (_mheight(msg->o) >= _mheight(idata->VGroupObj))
      SWDB_RET(ScrollVG(idata, RelTop, 0, TRUE));
   
   // -- else, if partially off bottom, scroll up ----------------------------

   if (RelBot > _mheight(idata->VGroupObj))
      SWDB_RET(ScrollVG(idata, RelBot - _mheight(idata->VGroupObj), 0, TRUE));
      
   SWDB_RET(NULL);
} SDFT



/*****************************************************************************
 * Method: Scroll group horizontally                                         *
 *****************************************************************************/

METHOD(SWM_NArticleView_Browse, SWP_NArticleView_Browse *)
{
   SWDB_RET(ScrollVG(idata, msg->Dir*_mheight(idata->VGroupObj)*9/10, 0, TRUE));
} SDFT


/*****************************************************************************
 * Method: Go to top of article                                              *
 *****************************************************************************/

METHOD(SWM_NArticleView_Top, Msg)
{
   if (!idata->VGroupObj) SWDB_RET(0);

   set(idata->VGroupObj, MUIA_Virtgroup_Top, 0);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Go to top of article                                              *
 *****************************************************************************/

METHOD(SWM_NArticleView_Bottom, Msg)
{
   if (!idata->VGroupObj) SWDB_RET(0);

   set(idata->VGroupObj, MUIA_Virtgroup_Top, LONG_MAX);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: get TextHide class pointer                                        *
 *****************************************************************************/

METHOD(SWM_NArticleView_TextHideClass, Msg)
{
   SWDB_RET(TextHideClass());
} SDFT


/*****************************************************************************
 * Method: get TextSel class pointer                                         *
 *****************************************************************************/

METHOD(SWM_NArticleView_TextSelClass, Msg)
{
   SWDB_RET(TextSelClass());
} SDFT


/*****************************************************************************
 * Method: get TextCmd class pointer                                         *
 *****************************************************************************/

METHOD(SWM_NArticleView_TextCmdClass, Msg)
{
   SWDB_RET(TextCmdClass());
} SDFT


/*****************************************************************************
 * Method: get TextScroll class pointer                                      *
 *****************************************************************************/

METHOD(SWM_NArticleView_TextScrollClass, Msg)
{
   SWDB_RET(TextScrollClass());
} SDFT


/*****************************************************************************
 * Method: get MimeBox class pointer                                         *
 *****************************************************************************/

METHOD(SWM_NArticleView_MimeBoxClass, Msg)
{
   SWDB_RET(MimeBoxClass());
} SDFT


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   ULONG rc;
   LONG f, l, x, HasSp;

   if (!idata->SWA_NArticleView_DspArt ||
       !idata->SWA_NArticleView_DspGrp)
      SWDB_RET(GetS(SWS_L_None));

   // -- search for variables we understand ----------------------------------

   if (idata->SelCnt) {
      if (!idata->VarBuf) idata->VarBuf = AV(SWV_VarBuffSize+1);
      
      VL_FindRetNum("SelTxtCnt", idata->SelCnt, idata->VarBuf, "%ld");

      if (!strncmp(msg->Name, "SelTxt ", 7)) {
         STRPTR Buff, s, tmp;
         Object *VarList = NN_VarList(idata);
         TEXT QuoteStr[8], SepStr[8];
         ULONG QuoteLen, SepLen;
         STRPTR Rest;
         
         (s = idata->VarBuf)[0] = '\0';

         DoMethod(VarList, SWM_VarList_FindParam, msg->Name, "QUOTE", QuoteStr,
                  sizeof(QuoteStr), "\"");

         DoMethod(VarList, SWM_VarList_FindParam, msg->Name, "SEP", SepStr,
                  sizeof(SepStr), " ");

         QuoteLen = strlen(QuoteStr);
         SepLen   = strlen(SepStr);

         Rest = (STRPTR)DoMethod(VarList, SWM_VarList_FindRest, msg->Name, NULL);

         if (!strcmp(Rest, "ALL"))
            f=0, l=idata->SelCnt-1;
         else
            f = l = atol(Rest);

         for (x=f; x<=l; x++) {
            if (tmp = Buff = (STRPTR)DoMethod(idata->FirstTxt,
                                              SWM_CText_GetGrpClip, x)) {

               if (HasSp = (strchr(Buff, ' ') ? TRUE : FALSE))
                  strcpy(s, QuoteStr), s += QuoteLen;

               for (; *tmp && (s - idata->VarBuf)
                       < (SWV_VarBuffSize - QuoteLen - SepLen - 1);)
                  *s++ = *tmp++;
               
               *s = '\0';
   
               FreeVec(Buff);

               if (HasSp)  strcpy(s, QuoteStr), s += QuoteLen;
               if (x != l) strcpy(s, SepStr),   s += SepLen;
            }
         }
         
         SWDB_RET(idata->VarBuf);
      }
   }
      
   // -- search article object for variables ---------------------------------

   if (rc = DoMethodA(idata->SWA_NArticleView_DspArt, (Msg)msg))
      SWDB_RET(rc);

   // -- search group object for variables -----------------------------------

   if (rc = DoMethodA(idata->SWA_NArticleView_DspGrp, (Msg)msg))
      SWDB_RET(rc);

   SWDB_RET(GetS(SWS_L_None));
} SDFT


/*****************************************************************************
 * Method: add a new imbedded object                                         *
 *****************************************************************************/

METHOD(SWM_NArticleView_AddObj, SWP_NArticleView_AddObj *)
{
   if (idata->ObjListCnt >= idata->ObjListMax) {
      idata->ObjListMax += 25;
      if (RV(idata->ObjListMax * sizeof(ObjInfo),
             &idata->ObjList) < 0)
         SWDB_RET("");
   }

   idata->ObjList[idata->ObjListCnt].NDF  = msg->NDF;
   idata->ObjList[idata->ObjListCnt].Data = msg->Data;

   sprintf(msg->TmpBuf, "\033AO{%04ld}", idata->ObjListCnt++);
   
   SWDB_RET(msg->TmpBuf);
} SDFT


/*****************************************************************************
 * Function: get object ptr from \033AO{xxxx} string                         *
 *****************************************************************************/
   
static Object *AObj(Object *o, SW_IDATA *idata, STRPTR s)
{
   LONG pos;
   
   if (!IsAO(s)) return NULL;

   pos = atol(s+4);

   if (pos < 0 || pos >= idata->ObjListCnt) return NULL;

   return (Object *)
      DoMethod(idata->ObjList[pos].NDF, SWM_NDFilter_GenAddObj,
               idata->ObjList[pos].Data,
               o,
               idata->SWA_NArticleView_DspGrp,
               idata->SWA_NArticleView_DspArt,
               FltListData(idata, idata->ObjList[pos].NDF));
}


/*****************************************************************************
 * Method: Add some lines to the group                                       *
 *****************************************************************************/

static void GO_InsLines(SW_IDATA *idata, STRPTR linepos, STRPTR lastpos,
                        Object *grp, Object **Prev)
{
   UBYTE c;

   if (linepos == lastpos) return;

   // -- only one char to insert - can`t \0 over it --------------------------
   
   if (lastpos+1 == linepos) {
      DoMethod(grp, OM_ADDMEMBER, TxtObj(idata, "", Prev));
      return;
   }

   // -- otherwise, insert text from lastpos to linepos ----------------------

   linepos--;
   c = *linepos, *linepos = '\0';

   if (*lastpos != '\0')
      DoMethod(grp, OM_ADDMEMBER, TxtObj(idata, lastpos, Prev));
         
   *linepos = c;
}


/*****************************************************************************
 * Method: generate object from text string                                  *
 *****************************************************************************/

static Object *GetG(ULONG Horiz)
{
   return GroupObject,
      MUIA_InnerTop,      0,
      MUIA_InnerBottom,   0,
      MUIA_InnerLeft,     0,
      MUIA_InnerRight,    0,
      MUIA_Group_Spacing, 0,
      MUIA_Group_Horiz, Horiz,
   End;
}

METHOD(SWM_NArticleView_GenObj, SWP_NArticleView_GenObj *)
{
   Object *grp, *hgrp, *Prev = NULL, *ao;
   STRPTR s, lastpos, linepos, epos;
   UBYTE  c;
   ULONG  MaxLines = 0;
      
   if (!msg->Tx) SWDB_RET(NULL);

   if (!(grp = msg->Grp ? msg->Grp : GetG(FALSE))) SWDB_RET(NULL);

   s = TxBuff(msg->Tx);
   idata->o = o;

   if (SW_STRISNULL(s))
      SWDB_RET(TxtObj(idata, "", &Prev));

   for (lastpos = linepos = s; *s; ) {
      if (*s == '\n') {
         linepos = ++s;

         // -- don`t insert too many lines at once, or we exceed max size
         if (++MaxLines < SWV_NArticleView_MaxTxtLines)
            continue;

         MaxLines = 0;
         GO_InsLines(idata, linepos, lastpos, grp, &Prev);
         lastpos = linepos;
         continue;
      }
      
      if (*s != CT_Ctrl || !IsAO(s)) { s++; continue; }

      epos = strchr(s, '}') + 1;

      // -- insert previous lines -----------------------------------------
      GO_InsLines(idata, linepos, lastpos, grp, &Prev);

      if (*s != CT_Ctrl || !IsAO(s)) { s++; continue; }

      // -- object on line by itself? -------------------------------------
      if (linepos == s && (epos[0] == '\n' || epos[0] == '\0')) {
         DoMethod(grp, OM_ADDMEMBER,
                  HGroup,
                     MUIA_InnerTop,     0,
                     MUIA_InnerBottom,  0,
                     MUIA_InnerLeft,    0,
                     MUIA_InnerRight,   0,
                     Child, AObj(o, idata, s),
                     Child, ao = HSpace(0),
                  End);

         set(ao, MUIA_HorizWeight, 1);
         
         lastpos = epos[0] == '\n' ? epos+1 : epos;
         s       = epos;

         continue;
      }

      // -- build horizontal group ----------------------------------------
      if (!(hgrp = GetG(TRUE)))
         SWDB_RET(NULL);

      for (s=lastpos=linepos; *s!='\0' && *s!='\n';) {
         if (!IsAO(s)) {s++; continue; }
         
         c = *s, *s = '\0';
         if (lastpos[0] != '\0')
            DoMethod(hgrp, OM_ADDMEMBER, TxtObj(idata, lastpos, &Prev));
         *s = c;
         DoMethod(hgrp, OM_ADDMEMBER, ao = AObj(o, idata, s));
         set(ao, MUIA_HorizWeight, 1);
         lastpos = s = strchr(s, '}') + 1;
      }

      if (lastpos != s) {
         c = *s, *s = '\0';
         if (*lastpos != '\0')
            DoMethod(hgrp, OM_ADDMEMBER, TxtObj(idata, lastpos, &Prev));
         *s = c;
         lastpos = s;
      }

      if (*lastpos == '\n') lastpos++;

      DoMethod(hgrp, OM_ADDMEMBER,
               RectangleObject, MUIA_HorizWeight, 10000, End);

      DoMethod(grp, OM_ADDMEMBER, hgrp);

   }

   if (*lastpos != '\0')
      DoMethod(grp, OM_ADDMEMBER, TxtObj(idata, lastpos, &Prev));

   SWDB_RET(grp);
} SDFT


/*****************************************************************************
 * Method: save data block to file                                           *
 *****************************************************************************/

METHOD(SWM_NArticleView_SaveData, SWP_NArticleView_SaveData *)
{
   ULONG Err;
   
   if (Err = SWWriteFileContents(msg->SaveFile, msg->Data, msg->DataSize))
      SWERR_NM(msg->ErrObj, NULL, SWDecodeErrStr(Err), msg->SaveFile, 0);

   SWDB_RET(Err ? FAILURE : SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Query for filename                                              *
 *****************************************************************************/

STRPTR SUID_FNameQuery(Object *o, Object *Win, STRPTR Title,
                       STRPTR InitDrawer, STRPTR InitFile, STRPTR SaveFile)
{
   return (STRPTR)
      DoMethod(o, SWM_Root_RequestFileName,
               SWT_Root_SL_Win,         Win,
               SWT_Root_SL_Title,       Title,
               SWT_Root_SL_InitDrawer,  InitDrawer,
               SWT_Root_SL_InitFile,    InitFile,
               SWT_Root_SL_Buffer,      SaveFile,
               SWT_Root_SL_SaveMode,    TRUE,
               TAG_DONE);
}


/*****************************************************************************
 * Method: save data block to file with UI                                   *
 *****************************************************************************/

METHOD(SWM_NArticleView_SaveDataUI, ULONG *)
{
   STRPTR Title      = GetTagUB(SWT_NArticleView_SD_Title,      NULL,   msg+1),
          InitDrawer = GetTagUB(SWT_NArticleView_SD_InitDrawer, "RAM:", msg+1),
          InitFile   = GetTagUB(SWT_NArticleView_SD_InitFile,   NULL,   msg+1),
          Data       = GetTagUB(SWT_NArticleView_SD_Data,       NULL,   msg+1);
   ULONG  MakeP      = GetTagUL(SWT_NArticleView_SD_MakePath,   TRUE,   msg+1),
          OWPrompt   = GetTagUL(SWT_NArticleView_SD_OWPrompt,   TRUE,   msg+1),
          SaveAs     = GetTagUL(SWT_NArticleView_SD_SaveAs,    FALSE,   msg+1),
          AskMSBase  = GetTagUL(SWT_NArticleView_SD_MSBase,    FALSE,   msg+1),
          CancPrompt = GetTagUL(SWT_NArticleView_SD_CancPrompt,FALSE,   msg+1),
          DataSize   = GetTagUL(SWT_NArticleView_SD_DataSize,  0,       msg+1);
   Object *App       = GetTagTyp(SWT_NArticleView_SD_App,      NULL,    msg+1, Object *),
          *Win       = GetTagTyp(SWT_NArticleView_SD_Win,      NULL,    msg+1, Object *),
          *ErrObj    = GetTagTyp(SWT_NArticleView_SD_ErrObj,   NULL,    msg+1, Object *);
   STRPTR MSBase, SF;
   TEXT   SaveFile[FMSIZE];
   ULONG  QRes;

   if (!Data) SWDB_RET(FAILURE);
   
   // -- prompt to nuke windows basename -------------------------------------
   
   if (AskMSBase)
      if ((MSBase = MSBaseName(InitFile)) != InitFile) {
         if (MUI_Request(App, Win, 0, NULL,
                         GetS(SWS_B_SvCanc), GetS(SWS_L_WindozeFile),
                         InitFile, MSBase) == 0) {
            if (CancPrompt)
               MUI_Request(App, Win, 0, NULL, GetS(SWS_L_OK), GetS(SWS_L_Canceled));
            SWDB_RET(FAILURE);
         } else InitFile = MSBase;
      }

   // -- prompt for filename if needed ---------------------------------------

   if (SaveAs) {
      SF = (STRPTR)SUID_FNameQuery(o, Win, Title, InitDrawer,
                                   InitFile, SaveFile);

      if (!SF) {
         if (CancPrompt)
            MUI_Request(App, Win, 0, NULL, GetS(SWS_L_OK), GetS(SWS_L_Canceled));
         SWDB_RET(FAILURE);
      }
   } else {
      if (MakeP) SWMakePath(InitDrawer);
      AddPart(strcpy(SF = SaveFile, InitDrawer), InitFile, FMSIZE);
   }

   if (SW_STRISNULL(SF)) SWDB_RET(FAILURE);

   // -- If requested, check for overwriting an old file ---------------------

   if (OWPrompt && SWFileExists(SF)) {
      QRes = MUI_Request(App, Win, 0, NULL, GetS(SWS_B_OWQuery),
                         GetS(SWS_L_OWrite), SF);

      if (QRes == 0) {
         MUI_Request(App, Win, 0, NULL, GetS(SWS_L_OK), GetS(SWS_L_Canceled));
         SWDB_RET(FAILURE);
      } else if (QRes == 2) {
         SF = (STRPTR)SUID_FNameQuery(o, Win, Title, InitDrawer,
                                      InitFile, SaveFile);

         if (!SF) {
            if (CancPrompt)
               MUI_Request(App, Win, 0, NULL, GetS(SWS_L_OK), GetS(SWS_L_Canceled));
            SWDB_RET(FAILURE);
         }
      }
   }
      
   // -- save the data -------------------------------------------------------         

   if (DoMethod(o, SWM_NArticleView_SaveData, Data, DataSize, ErrObj,
                SF) == FAILURE)
      DoMethod(ErrObj, SWM_Root_CheckErr, Win);
   else if (CancPrompt)
      MUI_Request(App, Win, 0, NULL,
                  GetS(SWS_L_OK), GetS(SWS_L_SavedOK), SF);
      
   SWDB_RET(SUCCESS);
} SDFT
   

/*****************************************************************************
 * Method: Search through displayed article text                             *
 *****************************************************************************/

METHOD(SWM_NArticleView_Search, SWP_NArticleView_Search *)
{
   ULONG  x, Match = FALSE, State = 0, Len = msg->Len, BlankCount = 0;
   STRPTR s;

   if (!idata->VGroupObj)
      SWDB_RET(0);

   if (!Len && msg->Wild) Len = strlen(msg->Pat);
   
   DoMethod(idata->VGroupObj, SWM_ListSearch_Setup, &State);

   for (x=msg->First; x<8192 && BlankCount < 100; x++) {
      DoMethod(idata->VGroupObj, MUIM_List_GetEntry, x, &s);
      if (!s) break;

      if (s[0] == '\0') {
         BlankCount++;
         continue;
      }

      BlankCount = 0;

      if (msg->Wild)
         Match = (msg->Case?MatchPattern:MatchPatternNoCase)(msg->Pat, s);
      else
         Match = findstr(s, msg->Pat, LONG_MAX, Len, msg->Case,
                         msg->Sub);

      if (Match) {
         DoMethod(idata->VGroupObj, MUIM_List_Jump, x);
         
         DoMethod(idata->VGroupObj, SWM_ListSearch_HLMatch,
                  x, msg->OrigStr, msg->Case, FALSE, msg->Wild);
         break;
      }
   }
   
   
   SWDB_RET(Match);
} SDFT


/*****************************************************************************
 * Method: Search through displayed article text                             *
 *****************************************************************************/

METHOD(SWM_NArticleView_CtxMenuChoice, SWP_NArticleView_CtxMenuChoice *)
{
   STRPTR Choice = SWGetStr(msg->item, MUIA_Menuitem_Title);

   // -- make sure we have selected text -------------------------------------
   
   if (!idata->SelCnt) {
      SWERR(o, FAILURE, SWS_Err_NoSel, 0, 0);
      DoMethod(o, SWM_Root_CheckErr, _win(o));
      SWDB_RET(FAILURE);
   }


   // -- clip? ---------------------------------------------------------------
   
   if (strcmp(Choice, GetS(SWS_M_CpClip)) == 0) {
      DoMethod(o, SWM_NArticleView_Clip);
      SWDB_RET(SUCCESS);
   }
   
   // -- otherwise, perform given action -----------------------------
   DoMethod(NN_MMActs(idata), SWM_FileActs_DoAction,
            SWT_FileActs_DoA_Type,    TXT_MIME_TYPE,
            SWT_FileActs_DoA_Action,  Choice,
            SWT_FileActs_DoA_VarList, NN_VarList(idata),
            SWT_FileActs_DoA_VarObj,  o,
            TAG_DONE);
   
   SWDB_RET(SUCCESS);
} SDFT

/*****************************************************************************
 * Method: Display raw article (no filters)                                  *
 *****************************************************************************/

METHOD(SWM_NArticleView_DispRaw, SWP_NArticleView_DispRaw *)
{
   ULONG rc, OldFlag = idata->RawFlag;

   idata->RawFlag = TRUE;
   rc = DoMethod(o, SWM_NArticleView_DispCooked, msg->Art);
   idata->RawFlag = OldFlag;

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Display cooked article (filters)                                  *
 *****************************************************************************/

METHOD(SWM_NArticleView_DispCooked, SWP_NArticleView_DispCooked *)
{
   Object *Art = msg->Art ? msg->Art : idata->SWA_NArticleView_DspArt,
          *Grp = idata->SWA_NArticleView_DspGrp;
   
   ULONG   Lines;
   STRPTR *Txt;

   // -- can`t redisplay if pending article ----------------------------------
   
   if (idata->SWA_NArticleView_PendingDisp || !Art || !Grp)
      SWDB_RET(FAILURE);

   if (!(Lines = SWGetUL(Art, SWA_NArticle_Lines)))
      SWDB_RET(FAILURE);
   
   if (!(Txt   = (STRPTR *)SWGetUL(Art, SWA_NArticle_Txt)))
      SWDB_RET(FAILURE);

   DoMethod(o, SWM_NArticleView_Begin, Art, Grp);

   DoMethod(o, SWM_NArticleView_NewLines, Art, Grp,
            Txt, 0, Lines - 1, 0, TRUE);

   DoMethod(o, SWM_NArticleView_End);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Get default font                                                  *
 *****************************************************************************/

METHOD(SWM_NArticleView_GetDefFont, Msg)
{
   SWDB_RET(VFont(idata));
}


/*****************************************************************************
 * Method: Get default Color                                                 *
 *****************************************************************************/

METHOD(SWM_NArticleView_GetDefColor, Msg)
{
   SWDB_RET(VColor(idata));
}


/*****************************************************************************
 * Method: Modify text buffer using filters                                  *
 *****************************************************************************/

METHOD(SWM_NArticleView_ModTxt, ULONG *)
{
   ULONG    Attr  = GetTagUL(SWT_NArticleView_MT_Attr,       0, msg+1),
            QMode = GetTagUL(SWT_NArticle_QMode, SWV_NNews_QM_Off,  msg+1),
            Parts = GetTagUL(SWT_NArticle_Parts, SWF_NArticle_HdBd, msg+1);
   TxtBuff *Tx    = GetTagTyp(SWT_NArticleView_MT_TxBuff, NULL, msg+1, TxtBuff *);
   Object *Grp    = GetTagObj(SWT_NArticleView_MT_Grp, NULL, msg+1),
          *Art    = GetTagObj(SWT_NArticleView_MT_Art, NULL, msg+1);
   ULONG  Complete = FALSE, HasHeaders, HPos;
   STRPTR Prefix   = GetTagUB(SWT_NArticle_Prefix,     NULL, msg+1);

   if (!Tx) SWDB_RET(NULL);

   if (idata->SWA_NArticleView_PendingDisp)
      SWDB_RET(NULL);

   if (AllocFltData(idata, InsertFilters(idata, Attr)) == FAILURE)
      SWDB_RET(NULL);

   DoMethod(idata->NDF, SWM_NDFilter_TBClear, Tx);

   TxBuff(Tx) = (STRPTR)DoMethod(Art, SWM_NArticle_GetTxt,
                                 SWT_NArticle_Parts, Parts | SWF_NArticle_Head,
                                 TAG_DONE);
   TxLen(Tx)  = 
   TxSize(Tx) = strlen(TxBuff(Tx));
   HasHeaders = TRUE;
   
   // -- call StartMatch on the filters --------------------------------------
      
   if (StartEndMatch(o, idata, SWM_NDFilter_StartMatch, Art, Grp)) {
      DoMethod(idata->NDF, SWM_NDFilter_TBDispose, Tx);
      Tx = NULL;
      goto Done;
   }

   DoMethod(o, SWM_NArticleView_GenTxt, Tx, HasHeaders, Art, Grp, &Complete,
            TRUE, SWM_NDFilter_ModTxtNoObjs);

   // -- Remove headers if not asked for -------------------------------------
   
   if (!(Parts & SWF_NArticle_Head)) {
      if (HPos = DoMethod(idata->NDF, SWM_NDFilter_TBSkipHdrs, Tx, HasHeaders))
         DoMethod(idata->NDF, SWM_NDFilter_TBDelSub, Tx, 0, HPos);
   }

   // -- add line prefix -----------------------------------------------------

   DoMethod(idata->NDF, SWM_NDFilter_TBAddPrefix, Tx, Prefix, QMode);

 Done:
   StartEndMatch(o, idata, SWM_NDFilter_EndMatch, Art, Grp);

   DoMethod(idata->FltList, MUIM_List_Clear);

   FreeFltData(idata);

   SWDB_RET(Tx);
}


/*****************************************************************************
 * Method: MUI Setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   if (!idata->CtxMenu)
      idata->CtxMenu = BuildCtxMenu(o, idata->SWA_AppBase_TopObj, TXT_MIME_TYPE);

#  ifndef DYNAMIC_MENUS
   set(idata->VGroupObj, MUIA_ContextMenu, idata->CtxMenu);
#  endif

   if (!DoSuperMethodA(cl, o, msg)) SWDB_RET(FALSE);

   MUI_RequestIDCMP(o, IDCMP_MOUSEBUTTONS);

   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: MUI Cleanup                                                       *
 *****************************************************************************/
   
METHOD(MUIM_Cleanup, Msg)
{
   FreeCtxMenu(o, TXT_MIME_TYPE);
   idata->CtxMenu = NULL;

   MUI_RejectIDCMP(o, IDCMP_MOUSEBUTTONS);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: number of selection regions has changed                           *
 *****************************************************************************/
   
METHOD(SWM_NArticleView_SelChange, SWP_NArticleView_SelChange *)
{
   idata->SelCnt = msg->Count;

#  ifdef DYNAMIC_MENUS 

   set(idata->VGroupObj, MUIA_ContextMenu, msg->Count ? idata->CtxMenu : NULL);
# endif

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: build context menu for this object                              *
 *****************************************************************************/

# define MenuCnt 32
static LONG MenuInit = FALSE;
static struct { Object *Win, *Menu, *AList; UBYTE *Type; ULONG RefCnt; }
   CtxMenu[MenuCnt] = { 0 };


/*****************************************************************************
 * Function: generate new context menu for this object                       *
 *****************************************************************************/

static Object *GenCtxMenu(STRPTR Type, Object *TopObj, Object **Acts)
{
   Object *AList, *act, *menustrip, *menu, *mi;
   ULONG  x;
   
   menustrip = MenustripObject,
      MUIA_Family_Child, menu = MenuObject,
         MUIA_Menu_Title, Type,
         MUIA_Family_Child, MenuitemObject,
            MUIA_Menuitem_Title, GetS(SWS_M_CpClip),
         End,
      End,
   End;

   if (!menustrip) goto Done;

   AList = (Object *)
      DoMethod(SWGetObj(TopObj, SWA_NNews_FAList), SWM_FileActs_GetActList,
               SWT_FileActs_GAL_Type, Type,
               TAG_DONE);

   if (Acts) *Acts = AList;
   
   if (AList)
      SW_MUIListLoop(AList, x, act, TRUE)
         if (!SW_STRISNULL(act))
            if (mi = MenuitemObject, MUIA_Menuitem_Title, act, End)
               DoMethod(menu, MUIM_Family_AddTail, mi);

  Done:
   return menustrip;
}


/*****************************************************************************
 * Function: Build a context menu                                            *
 *****************************************************************************/

static Object *BuildCtxMenu(Object *o, Object *TopObj, STRPTR Type)
{
   LONG x;
   
   if (!MenuInit) {
      memset(CtxMenu, 0, sizeof(CtxMenu));
      MenuInit = TRUE;
   }

   // -- return existing menu ------------------------------------------------
   
   for (x=0; x<MenuCnt; x++)
      if (CtxMenu[x].Win == _win(o) && !strcmp(CtxMenu[x].Type, Type)) {
         CtxMenu[x].RefCnt++;
         return CtxMenu[x].Menu;
      }

   // -- if no existing one, build a new one ---------------------------------
   
   for (x=0; x<MenuCnt; x++)
      if (!CtxMenu[x].RefCnt) {
         CtxMenu[x].RefCnt = 1;
         strcpy(CtxMenu[x].Type = AllocVec(strlen(Type)+1, MEMF_ANY), Type);
         CtxMenu[x].Win    = _win(o);
         CtxMenu[x].Menu   = GenCtxMenu(Type, TopObj, &CtxMenu[x].AList);

         return CtxMenu[x].Menu;
      }
         
   return NULL;
}


/*****************************************************************************
 * Free a particular context menu                                            *
 *****************************************************************************/

static void FreeCtxMenu(Object *o, STRPTR Type)
{
   LONG x;

   for (x=0; x<MenuCnt; x++)
      if (CtxMenu[x].Win == _win(o) && !strcmp(CtxMenu[x].Type, Type)) {
         if (!--CtxMenu[x].RefCnt) {
            FreeVec(CtxMenu[x].Type);
            MUI_DisposeObject(CtxMenu[x].Menu);
            MUI_DisposeObject(CtxMenu[x].AList);
            memset(&CtxMenu[x], 0, sizeof(CtxMenu[x]));
         }
         break;
      }
}
