
# define USE_WINOPEN_BODY
# define USE_WINOPEN_COLORS
# define USE_WINCLOSE_BODY
# define USE_NEWGRP_BODY
# define USE_Broken_COLORS
# define USE_Broken_BODY

# include "BM_Broken.h"
# include "BM_WinOpen.h"
# include "BM_WinClose.h"
# include "BM_NewGrp.h"
# include "MBM_Broken.h"
# include "MBM_WinOpenClose.h"
# include "S-SetUIToggle.h"


/*****************************************************************************
 * Function: Free Server Group Info                                          *
 *****************************************************************************/

static void FreeSGInfo(SW_IDATA *idata)
{
   FV(idata->SGPatt);
   idata->SGPatt = NULL;

   FV(idata->GL_Lines);
   idata->GL_Lines   = NULL;
}


/*****************************************************************************
 * Function: en/disable group buttons                                        *
 *****************************************************************************/

static void EnDisAbleBt(SW_IDATA *idata, ULONG Cmd, ULONG State)
{
   set(idata->Cmds[Cmd].Button, MUIA_Disabled, State);

   if (Cmd == SWV_GC_GL_Hdrs)
      set(idata->Cmds[SWV_GC_GL_Arts].Button, MUIA_Disabled, State);

   if (Cmd == SWV_GC_GL_Arts)
      set(idata->Cmds[SWV_GC_GL_Hdrs].Button, MUIA_Disabled, State);
}


/*****************************************************************************
 * Method: Open search window                                                *
 *****************************************************************************/

METHOD(SWM_NGroupList_SWin, Msg)
{
   if (idata->GroupSearch)
      DoMethod(idata->GroupSearch, SWM_ListSearch_OpenWin, _app(o));

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: Free a list of groups for multi-commands                        *
 *****************************************************************************/

static void FreeGroupList(Object *o, SW_IDATA *idata, ULONG pos)
{
   ULONG x;

   if (idata->Cmds[pos].Button)
      EnDisAbleBt(idata, pos, FALSE);
   
   if (idata->Cmds[pos].List) {
      for (x=0; x<idata->Cmds[pos].Count; x++) {
         if (idata->Cmds[pos].ProgWin)
            DoMethod(FindGroup(idata, idata->Cmds[pos].List[x]),
                     MUIM_KillNotifyObj, idata->Cmds[pos].PcAttr,
                     idata->Cmds[pos].G2);

         FV(idata->Cmds[pos].List[x]);
      }

      FV(idata->Cmds[pos].List);
      idata->Cmds[pos].List    = NULL;
   }

   if (idata->Cmds[pos].ProgWin) {
      set(idata->Cmds[pos].ProgWin, MUIA_Window_Open, FALSE);
      DoMethod(idata->App, OM_REMMEMBER, idata->Cmds[pos].ProgWin);
      MUI_DisposeObject(idata->Cmds[pos].ProgWin);
      idata->Cmds[pos].ProgWin = NULL;
   }

   FV(idata->Cmds[pos].GrpMsg);

   idata->Cmds[pos].GrpMsg  = NULL;
   idata->Cmds[pos].Count   = 0;
   idata->Cmds[pos].Pos     = 0;
   idata->Cmds[pos].PrevPos = -1;
}


/*****************************************************************************
 * Function: make a list of groups for multi-commands                        *
 *****************************************************************************/

static STRPTR *MakeGroupList(Object *o, SW_IDATA *idata, LONG *Size, ULONG All)
{
   LONG  Count, x;
   STRPTR *gl;
   Object *g;

   *Size = 0;

   if (All) {
      Count = SWGetUL(idata->SWA_NList_L, MUIA_List_Entries);
   } else {
      for (Count=0, x = MUIV_List_NextSelected_Start; ;Count++) {
         DoMethod(idata->SWA_NList_L, MUIM_List_NextSelected, &x);
         if (x == MUIV_List_NextSelected_End)
            break;
      }
   }

   if (!Count) {
      SWERR(o, NULL, SWS_Note_NoneSelected, 0, 0);
      return NULL;
   }

   if (!(gl = (STRPTR *)AV(ASize(Count)))) {
      SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
      return NULL;
   }

   // -- if some entries are selected, use them.  else use all. --------------

   Count = 0;

   if (All) {
      SW_MUIListLoop(idata->SWA_NList_L, x, g, TRUE)
         if (!(gl[Count++] = SDup(SWGetStr(g, SWA_NGroup_Name))))
            goto Err;
   } else
      SW_MUIListLoopSel(idata->SWA_NList_L, x, g, TRUE)
         if (!(gl[Count++] = SDup(SWGetStr(g, SWA_NGroup_Name))))
            goto Err;

   *Size = Count;
   return gl;

  Err:
   SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
   while (--Count >= 0)
      FV(gl[Count]);
   return NULL;
}


/*****************************************************************************
 * Method: Run methods on multiple groups                                    *
 *****************************************************************************/

METHOD(SWM_NGroupList_DoGroup, SWP_NGroupList_DoGroup *)
{
   FreeGroupList(o, idata, msg->Cmd);

   if (!(idata->Cmds[msg->Cmd].List =
         MakeGroupList(o, idata, &idata->Cmds[msg->Cmd].Count, msg->All)))
      goto Done;

   if (idata->Cmds[msg->Cmd].ShowProgWin) {
      Object *Win, *cb;
      idata->Cmds[msg->Cmd].GrpMsg = AV(strlen(GetS(SWS_L_PM_GroupOf)) + 256);

      if (!idata->Cmds[msg->Cmd].GrpMsg) {
         FreeGroupList(o, idata, msg->Cmd);
         SWDB_RET(0);
      }

      Win = idata->Cmds[msg->Cmd].ProgWin = WindowObject,
         MUIA_Window_IsSubWindow, TRUE,
         MUIA_Window_Title,       idata->Cmds[msg->Cmd].WinTitle,
         MUIA_Window_ID,          MAKE_ID('P','M','s','g') +
                                  idata->Cmds[msg->Cmd].Method,
         WindowContents, VGroup,
            MUIA_Background, MUII_GroupBack,
            Child, idata->Cmds[msg->Cmd].G1 = GaugeObject,
               GaugeFrame,
               MUIA_Gauge_InfoText, "",
               MUIA_Gauge_Max,      idata->Cmds[msg->Cmd].Count,
               MUIA_Gauge_Horiz,    TRUE,
            End,
            Child, idata->Cmds[msg->Cmd].G2 = GaugeObject,
               GaugeFrame,
               MUIA_Gauge_InfoText, "\n",
               MUIA_Gauge_Horiz,    TRUE,
            End,
            Child, ScaleObject, End,
            Child, cb = SimpleButton(GetS(SWS_B_Cancel)),
         End,
      End;

      if (!Win) {
         FreeGroupList(o, idata, msg->Cmd);
         SWDB_RET(0);
      }
      
      DoMethod(Win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
               _app(o), 5, MUIM_Application_PushMethod, o,
               2, SWM_NGroupList_DoGroup_Abort, msg->Cmd);

      DoMethod(cb, NOTP, _app(o), 5, MUIM_Application_PushMethod, o,
               2, SWM_NGroupList_DoGroup_Abort, msg->Cmd);
      
      DoMethod(idata->App=_app(o), OM_ADDMEMBER, Win);
      set(Win, MUIA_Window_Open, TRUE);
   }

   if (idata->Cmds[msg->Cmd].Button)
      EnDisAbleBt(idata, msg->Cmd, TRUE);
   
   DoMethod(o, SWM_NGroupList_DoGroup_I,
            SWT_NGroup_CmdState, SWV_NGroup_End,
            SWT_NGroup_CmdData, msg->Cmd,
            TAG_DONE);

  Done:
   DoMethod(o, SWM_Root_CheckErr, _win(o));
   SWDB_RET(0);
} SDFT



/*****************************************************************************
 * Function: Set Group %ld of %ld message                                    *
 *****************************************************************************/

static void SetGrpOfMsg(SW_IDATA *idata, ULONG Cmd, Object *CurrGrp)
{
   if (!idata->Cmds[Cmd].GrpMsg || !idata->Cmds[Cmd].G1)
      return;
   
   sprintf(idata->Cmds[Cmd].GrpMsg,
           GetS(SWS_L_PM_GroupOf), idata->Cmds[Cmd].Count,
           SWGetStr(CurrGrp, SWA_NGroup_Name));
      
   set(idata->Cmds[Cmd].G1, MUIA_Gauge_InfoText, idata->Cmds[Cmd].GrpMsg);
}


/*****************************************************************************
 * Function: Set Progress message                                            *
 *****************************************************************************/

static void SetProgMsg(SW_IDATA *idata, ULONG Cmd, STRPTR ProgMsg)
{
   if (!idata->Cmds[Cmd].G2 || !ProgMsg)
      return;
   
   set(idata->Cmds[Cmd].G2, MUIA_Gauge_InfoText, ProgMsg);
}


/*****************************************************************************
 * Method: Run methods on multiple groups                                    *
 *****************************************************************************/

METHOD(SWM_NGroupList_DoGroup_I, ULONG *)
{
   LONG State      = GetTagUL(SWT_NGroup_CmdState, 0,  msg+1),
        Cmd        = GetTagUL(SWT_NGroup_CmdData,  -1, msg+1),
        Pos, Prev;

   STRPTR ProgMsg  = GetTagStr(SWT_NGroup_ProgMsg, "", msg+1);

   Object *CurrGrp = GetTagObj(SWT_NGroup_CmdGroup, NULL, msg+1),
          *g       = NULL;

   if (Cmd == -1) SWDB_RET(0);
   
   if (State == SWV_NGroup_NewProgMsg)
      SetProgMsg(idata, Cmd, ProgMsg);

   if (State == SWV_NGroup_Begin)
      SetGrpOfMsg(idata, Cmd, CurrGrp);

   if (State == SWV_NGroup_Abort) {
      FreeGroupList(o, idata, Cmd);
      DoMethod(CurrGrp,   SWM_Root_CheckErr, _win(o));
   }
   
   if (State == SWV_NGroup_End) {
      while (!g && idata->Cmds[Cmd].Pos < idata->Cmds[Cmd].Count)
         g=FindGroup(idata,idata->Cmds[Cmd].List[Pos=idata->Cmds[Cmd].Pos++]);

      if (idata->Cmds[Cmd].ProgWin) {
         ULONG Attr = idata->Cmds[Cmd].PcAttr;
         if ((Prev = idata->Cmds[Cmd].PrevPos) >= 0)
            DoMethod(FindGroup(idata, idata->Cmds[Cmd].List[Prev]),
                     MUIM_KillNotifyObj, Attr, idata->Cmds[Cmd].G2);

         if (g)
            DoMethod(g, MUIM_Notify, Attr, MUIV_EveryTime,
                     idata->Cmds[Cmd].G2, 3, MUIM_Set,
                  MUIA_Gauge_Current, MUIV_TriggerValue);

         set(idata->Cmds[Cmd].G1, MUIA_Gauge_Current, Pos+1);
         set(idata->Cmds[Cmd].G2, MUIA_Gauge_Current, 0);
      }

      if (!g) {
         FreeGroupList(o, idata, Cmd);
         SWDB_RET(0);
      }

      idata->Cmds[Cmd].PrevPos = Pos;
      
      if ((LONG)DoMethod(g, idata->Cmds[Cmd].Method,
                         SWT_NGroup_CmdObj,     o,
                         SWT_NGroup_CmdMethod,  SWM_NGroupList_DoGroup_I,
                         SWT_NGroup_CmdData,    Cmd,
                         TAG_DONE) != SUCCESS) {

         DoMethod(g,   SWM_Root_CheckErr, _win(o));
         FreeGroupList(o, idata, Cmd);
         SWDB_RET(-1);
      }
   }
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Abort multi-group commands                                        *
 *****************************************************************************/

METHOD(SWM_NGroupList_DoGroup_Abort, SWP_NGroupList_DoGroup *)
{
   Object *g;
   ULONG x;

   if (idata->Cmds[msg->Cmd].List)
      for (x=0; x<idata->Cmds[msg->Cmd].Count; x++)
         if (g = FindGroup(idata, idata->Cmds[msg->Cmd].List[x]))
            DoMethod(g, SWM_NGroup_Abort, msg->Cmd);

   SWDB_RET(0);
}   


/*****************************************************************************
 * Function: abort all xfers                                                 *
 *****************************************************************************/

static void AbortGroupListAll(Object *o, SW_IDATA *idata)
{
   ULONG x;

   for (x=0; x<SWV_GC_MAX; x++)
      DoMethod(o, SWM_NGroupList_DoGroup_Abort, x);
}


/*****************************************************************************
 * Method: Change on/offline mode                                            *
 *****************************************************************************/

METHOD(SWM_NGroupList_ChangeMode, SWP_NGroupList_ChangeMode *)
{
   if (idata->Mode)
      set(idata->Mode, MUIA_Text_Contents,
          GetS(msg->Online ? SWS_L_OnLine : SWS_L_OffLine));

   if (idata->WinOpen && idata->ModeGroup) {
      DoMethod(idata->ModeGroup, MUIM_Group_InitChange);
            
      if (idata->b_ModeBt) {
         DoMethod(idata->ModeGroup, OM_REMMEMBER, idata->b_ModeBt);
         idata->b_ModeBt = NULL;
      }

      DoMethod(idata->ModeGroup, OM_ADDMEMBER,
               idata->b_ModeBt = (msg->Online ? idata->b_GoOffLine
                                              : idata->b_GoOnLine));

      set(idata->b_ModeBt, MUIA_HorizWeight, 0);
      
      DoMethod(idata->ModeGroup, MUIM_Group_ExitChange);
   }

   DoMethod(o, MUIM_MultiSet, MUIA_Disabled, !msg->Online,
            b_UpdCnt, b_RdDesc, b_BatchO, b_BatchA, b_Fetch,
            NULL);

   idata->Online = msg->Online;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Delete selected groups                                            *
 *****************************************************************************/

METHOD(SWM_NGroupList_Delete, Msg)
{
   ULONG  rc = 1, found = 0, x;
   Object *g;

   SW_MUIListLoopSel(idata->SWA_NList_L, x, g, TRUE)
      found += DoMethod(g, SWM_NGroup_TestOVDir) ? 1 : 0;

   if (found) {
      STRPTR gn = GetS(found > 1 ? SWS_L_Groups : SWS_L_Group);
         
      rc = MUI_Request(_app(o), _win(o), 0, NULL,
                       GetS(SWS_B_DelOVBt), GetS(SWS_L_DelOVMsg),
                       found, gn, gn);

      if (rc == 3) SWDB_RET(FAILURE);
   }

   if (rc != 2) {
      set(_app(o), MUIA_Application_Sleep, TRUE);

      DoMethod(o, SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Selected, SWV_ListDoMethod_Entry, 6,
               SWM_NGroup_DeleteOVDir,
               SWT_NGroup_DelOV_NoPrompt, rc == 0 ? FALSE : TRUE,
               SWT_NGroup_DelOV_Win,     _win(o),
               TAG_DONE);

      set(_app(o), MUIA_Application_Sleep, FALSE);
   }

   // -- set flag to avoid writing of cache dir on dispose -------------------

   DoMethod(o, SWM_Root_ListDoMethod, idata->SWA_NList_L,
            SWV_ListDoMethod_Selected, SWV_ListDoMethod_Entry, 1,
            SWM_NGroup_NoSaveOV);

   DoMethod(o, MUIM_List_Remove, MUIV_List_Remove_Selected);

   AB_SetMod(idata);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: MUI Setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   static STRPTR RegTab[4];
   Object *b_Add, *b_Del, *b_Sort, *b_SelAll, *b_VUnsub,
           *b_Read, *b_Search, *b_MkRd, *b_MkUnrd, *b_FAQ, *title, *AUI, *BFUI;

   idata->WinOpen = TRUE;
   
   if (!idata->UIAdded) {
      Object *grp = MUI_NewObject(SWC_NGroup, TAG_DONE);

      RegTab[0] = GetS(SWS_L_SubGrps);
      RegTab[1] = GetS(SWS_L_GrpOpts);
      RegTab[2] = GetS(SWS_L_BatchFlt);
      RegTab[3] = NULL;

      if (!grp) SWDB_RET(FALSE);

      if (!(idata->GroupSearch = CreateGroupSrch(o, idata))) {
         MUI_DisposeObject(grp);
         SWDB_RET(FALSE);
      }

      if (!(idata->b_GoOnLine  = MkBt(SWS_B_GoOnLine,  SWS_H_GoOnLine)) ||
          !(idata->b_GoOffLine = MkBt(SWS_B_GoOffLine, SWS_H_GoOffLine))) {
         SWDB_RET(FALSE);
      }
      
      DoMethod(o, OM_ADDMEMBER,
         idata->RegGrp = RegisterGroup(RegTab),
            MUIA_CycleChain, 1,
            Child, HGroup,
               Child, VGroup,
                  Child, SWUI_FrameText(GetS(SWS_L_TopTitle),
                                        SWGetUL(idata->SWA_NList_L,
                                                MUIA_List_Entries),
                     (HGroup,
                        MUIA_Group_HorizSpacing, 3,
                      Child, b_FAQ = MkIB(idata, SWS_H_FAQb, BF_FAQ, 'Q'),
                        Child, b_Search =
                             MkIB(idata, SWS_H_Search, BF_Search, '/'),
                        Child, PopobjectObject,
                           MUIA_Popstring_Button,
                             MkIB(idata, SWS_H_SortP,  BF_Sort,   '='),
                           MUIA_Popobject_Object, HGroup,
                              GroupFrame,
                              MUIA_Background, MUII_GroupBack,
                              Child, MakeSortEntry(o, SWS_L_Sort1, grp,
                                                   SWA_NGroupList_Sort1,
                                                   SWA_NGroupList_Dir1),
                              Child, MakeSortEntry(o, SWS_L_Sort2, grp,
                                                   SWA_NGroupList_Sort2,
                                                   SWA_NGroupList_Dir2),
                              Child, MakeSortEntry(o, SWS_L_Sort3, grp,
                                                   SWA_NGroupList_Sort3,
                                                   SWA_NGroupList_Dir3),
                           End,
                        End,
                        Child, DoMethod(idata->SWA_NGroupList_ListCol,
                                        SWM_ListCol_Popup, '-',
                                        GetS(SWS_H_ListCol)),
                        Child, DoMethod(idata->SWA_NGroupList_ListCol,
                                             SWM_ListCol_ClipBt,
                                             'c', GetS(SWS_H_Clip)),
                     End), &title),
                  Child, idata->SWA_NList_LV,
                  Child, HGroup,
                     Child, b_Add  = MkBt(SWS_B_Add, SWS_H_Add),
                     Child, s_Name = StringObject,
                                        StringFrame,
                                        MUIA_CycleChain, 1,
                                        MUIA_HorizWeight, 5000,
                                     End,
                  End,
                  Child, HGroup,
                     MUIA_Group_SameSize, TRUE,
                     Child, b_Del    = MkBt(SWS_B_Del,       SWS_H_Del),
                     Child, b_Sort   = MkBt(SWS_B_Sort,      SWS_H_Sort),
                     Child, b_SelAll = MkBt(SWS_B_SelAll,    SWS_H_SelAll),
                     Child, b_VUnsub = MkBt(SWS_B_ViewUnsub, SWS_H_ViewUnsub),
                  End,
               End,
               Child, VGroup,
                  MUIA_HorizWeight, 1,
                  Child, idata->ModeGroup = HGroup,
                     MUIA_InnerBottom, 0,
                     MUIA_InnerTop,    0,
                     MUIA_InnerLeft,   0,
                     MUIA_InnerRight,  0,
                     Child, idata->Mode     = SWUI_FrameText(NULL, 0, NULL, NULL),
                  End,
                  Child, VGroup,
                     GroupFrameT(GetS(SWS_L_GroupCmds)),
                     Child, b_Read   = MkBt(SWS_B_Read,       SWS_H_Read),
                     Child, b_MkRd   = MkBt(SWS_B_MarkRead,   SWS_H_MarkRead),
                     Child, b_MkUnrd = MkBt(SWS_B_MarkUnread, SWS_H_MarkUnread),
                     Child, b_UpdCnt = MkBt(SWS_B_UpdateCnt,  SWS_H_UpdateCnt),
                     Child, b_RdDesc = MkBt(SWS_B_ReadDesc,   SWS_H_ReadDesc),
                     Child, b_BatchO = MkBt(SWS_B_BatchOView, SWS_H_BatchOView),
                     Child, b_BatchA = MkBt(SWS_B_BatchArts,  SWS_H_BatchArts),
                     Child, VSpace(0),
                  End,
               End,
            End,
            Child, AUI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o),
            Child, BFUI = (Object *)DoMethod(idata->SWA_NGroupList_BatchDLFlt,
                                             SWM_NSFilter_UI),
         End);

      MUI_DisposeObject(grp);

      if (!idata->RegGrp) SWDB_RET(FALSE);

      SetAttrs(s_Name,
               MUIA_Disabled,              TRUE,
               MUIA_String_AttachedList,   idata->SWA_NList_LV,
               TAG_DONE);
   
      s_NameAttr = SWA_NGroup_Name;
   
      DoMethod(b_Sort,   NOTP, o, 1, MUIM_List_Sort);
      DoMethod(b_Add,    NOTP, o, 2, SWM_NList_Add, NULL);
      DoMethod(b_Search, NOTP, o, 1, SWM_NGroupList_SWin);
      DoMethod(b_SelAll, NOTP, o, 4, SELALLON);
      DoMethod(b_UpdCnt, NOTP, o, 3, SWM_NGroupList_DoGroup, SWV_GC_CheckStats, FALSE);
      DoMethod(b_RdDesc, NOTP, o, 3, SWM_NGroupList_DoGroup, SWV_GC_ReadDesc, FALSE);
      DoMethod(b_BatchO, NOTP, o, 3, SWM_NGroupList_DoGroup, SWV_GC_GL_Hdrs, FALSE);
      DoMethod(b_BatchA, NOTP, o, 3, SWM_NGroupList_DoGroup, SWV_GC_GL_Arts, FALSE);
      DoMethod(b_Del,    NOTP, o, 1, SWM_NGroupList_Delete);
      DoMethod(b_VUnsub, NOTP, o, 1, SWM_NGroupList_OpenSGWin);

      DoMethod(idata->b_GoOffLine, NOTP, NN_TCPList(idata), 2,
               SWM_NTCPList_Online, SWV_NTCPList_Online_Off);

      DoMethod(idata->b_GoOnLine,  NOTP, NN_TCPList(idata), 2,
               SWM_NTCPList_Online, SWV_NTCPList_Online_On);

      DoMethod(b_FAQ,   NOTP, o, 6,
               SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 1,
               SWM_NGroup_OpenFAQList);
   
      DoMethod(b_MkRd,  NOTP, o, 7, 
               SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Selected, SWV_ListDoMethod_Entry, 2,
               SWM_NGroup_GL_Mark, TRUE);
   
      DoMethod(b_MkUnrd,  NOTP, o, 7, 
               SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Selected, SWV_ListDoMethod_Entry, 2,
               SWM_NGroup_GL_Mark, FALSE);
   
      DoMethod(b_Add, NOTP, MUIV_Notify_Window, 3,
               MUIM_Set, MUIA_Window_ActiveObject, s_Name);
      
      DoMethod(idata->SWA_NList_LV, DCLK,
               o, 6, SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 1,
               SWM_NGroup_StartRead);
   
      DoMethod(o, MUIM_Notify, SWA_NGroupList_FullNames, MUIV_EveryTime, o, 2,
               MUIM_List_Redraw, MUIV_List_Redraw_All);
      
      DoMethod(b_Read, NOTP,
               o, 6, SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 1,
               SWM_NGroup_StartRead);
   
      DoMethod(idata->SWA_NList_L, MUIM_Notify,
               MUIA_List_Active, MUIV_EveryTime,
               o, 2, SWM_NList_Change, MUIV_TriggerValue);
      
      DoMethod(idata->SWA_NList_L, MUIM_Notify, MUIA_List_Entries, MUIV_EveryTime,
               title, 4, MUIM_SetAsString, MUIA_Text_Contents,
               GetS(SWS_L_TopTitle), MUIV_TriggerValue);

      idata->UIAdded = TRUE;

      SetUIToggle(AUI, SWA_NGroupList_UpdInt, SWA_NGroupList_Upd,
                  MUIV_NotTriggerValue, !idata->SWA_NGroupList_Upd);

      set(BFUI, MUIA_ShortHelp, GetS(SWS_H_BatchFlt));
   }

   DoMethod(o, SWM_NGroupList_ChangeMode, idata->Online);

   if (!DoSuperMethodA(cl, o, msg)) SWDB_RET(FALSE);

   if (!(idata->UpdTimer =
         MUI_NewObject(SWC_Timer,
                       SWA_Timer_Secs,    idata->SWA_NGroupList_UpdInt,
                       SWA_Timer_Method,  SWM_NGroupList_UpdCheck,
                       TAG_DONE)))
      SWDB_RET(FALSE);

   if (idata->SWA_NGroupList_UpdOpen)
      DoMethod(NN_App(idata), MUIM_Application_PushMethod, o, 1,
               SWM_NGroupList_UpdCheck);

   SWMakeImg(idata, SWV_Img_Broken,   MakeBrokenBitm((UBYTE *)Broken_body));
   SWMakeImg(idata, SWV_Img_WinOpen,  MakeWinBitm((UBYTE *)WinOpen_body));
   SWMakeImg(idata, SWV_Img_WinClose, MakeWinBitm((UBYTE *)WinClose_body));
   SWMakeImg(idata, SWV_Img_NewBatch, MakeWinBitm((UBYTE *)NEWGRP_body));

   SWCreateImages(idata);

   DoMethod(idata->UpdTimer, SWM_Timer_Install, o, NN_App(idata));

   DoMethod(o, MUIM_Notify, SWA_NGroupList_UpdInt, MUIV_EveryTime,
            idata->UpdTimer, 3, MUIM_Set, SWA_Timer_Secs, MUIV_TriggerValue);
   
   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: MUI Cleanup                                                       *
 *****************************************************************************/
   
METHOD(MUIM_Cleanup, Msg)
{
   if (idata->b_ModeBt && idata->ModeGroup) {
      DoMethod(idata->ModeGroup, OM_REMMEMBER, idata->b_ModeBt);
      idata->b_ModeBt   = NULL;
   }
   
   SWDeleteImages(idata);

   DoMethod(o, MUIM_KillNotifyObj, SWA_NGroupList_UpdInt, idata->UpdTimer);

   DoMethod(idata->UpdTimer, SWM_Timer_Remove);
   MUI_DisposeObject(idata->UpdTimer);
   idata->UpdTimer = NULL;
   idata->WinOpen  = FALSE;

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Process menu event                                                *
 *****************************************************************************/

METHOD(SWM_AppBase_MenuCall, SWP_AppBase_MenuCall *)
{
   ULONG rc;
   
   switch (msg->Call) {
     case SWS_M_ImportNRC+MOff:
      rc = DoMethod(o, SWM_NGroupList_ImportNRC);
      break;
      
     case SWS_M_AllCache+MOff:
      rc = DoMethod(o, SWM_NGroupList_ImportCD);
      break;
      
     default: rc = DoMethodA(idata->SWA_AppBase_TopObj, msg);
   }

   DoMethod(o, SWM_Root_CheckErr, _win(o));
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Open Server Group window                                          *
 *****************************************************************************/

METHOD(SWM_NGroupList_OpenSGWin, Msg)
{
   Object *b_Clr, *b_Srch, *b_Sort, *b_Save, *b_Load,
          *b_SelAll, *b_Sub, *b_ListSrch, *b_ListSort, *b_ListClip, *b_New,
          *b_Browse, *title;
   
   if (idata->SGWin) {
      set(idata->SGWin, MUIA_Window_Open, TRUE);
      DoMethod(idata->SGWin, MUIM_Window_ToFront);
      SWDB_RET(SUCCESS);
   }

   idata->App = NN_App(idata);

   if (!idata->App) SWDB_RET(FAILURE);

   idata->SGWin = WindowObject,
      MUIA_Window_Title,       GetS(SWS_T_UnSub),
      MUIA_Window_ID,          MAKE_ID('v','s','v','g'),
      MUIA_Window_IsSubWindow, TRUE,
      WindowContents, HGroup,
         MUIA_Background, MUII_GroupBack,
         Child, VGroup,
            Child, SWUI_FrameText(GetS(SWS_T_GSTitle), NULL,
                      (HGroup,
                       MUIA_Group_HorizSpacing, 3,
                       Child, b_ListSrch =
                          MkIB(idata, SWS_H_Search, BF_Search, '/'),
                       Child, b_ListSort =
                          MkIB(idata, SWS_H_GLSort, BF_Sort,   '='),
                       Child, b_ListClip =
                          MkIB(idata, SWS_H_Clip,   BF_Clip,   'c'),
                       End), &title),
            Child, idata->SWA_NGroupList_SG_LV = ListviewObject,
               MUIA_CycleChain,           1,
               MUIA_Listview_Input,       TRUE,
               MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
               MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
               MUIA_Listview_List, idata->SWA_NGroupList_SG_L = ObjListObject,
                  InputListFrame,
                  SWA_ObjList_Object,      o,
                  MUIA_ShortHelp,          GetS(SWS_H_SrvGrpList),
                  MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
                  MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
               End,
            End,
            Child, HGroup,
               Child, b_Fetch           = MkBt(SWS_B_Fetch, SWS_H_Fetch),
               Child, idata->s_FetchPat = StringObject,
                  StringFrame,
                  MUIA_String_Contents, idata->SWA_NGroupList_LastFetchPat,
                  MUIA_String_MaxLen,   80,
                  MUIA_ShortHelp,       GetS(SWS_H_StrFetch),
                  MUIA_CycleChain,      1,
                  MUIA_HorizWeight,     5000,
               End,
            End,
         End,
         Child, VGroup,
            GroupFrameT(GetS(SWS_T_GSCmds)),
            MUIA_HorizWeight,    1,
            MUIA_Group_SameWidth, TRUE,
            Child, b_Clr          = MkBt(SWS_B_GLClr,     SWS_H_GLClr),
            Child, b_Srch         = MkBt(SWS_B_GLSrch,    SWS_H_GLSrch),
            Child, b_Sort         = MkBt(SWS_B_GLSort,    SWS_H_GLSort),
            Child, b_Sub          = MkBt(SWS_B_GLSub,     SWS_H_GLSub),
            Child, b_SelAll       = MkBt(SWS_B_GLSelAll,  SWS_H_GLSelAll),
            Child, b_Load         = MkBt(SWS_B_GLLoad,    SWS_H_GLLoad),
            Child, b_Save         = MkBt(SWS_B_GLSave,    SWS_H_GLSave),
            Child, b_New          = MkBt(SWS_B_GLNew,     SWS_H_GLNew),
            Child, b_Browse       = MkBt(SWS_B_GLBrowse,  SWS_H_GLBrowse),
            Child, idata->b_GLAbt = MkBt(SWS_B_GLAbt,     SWS_H_GLAbt),
            Child, VSpace(0),
         End,
      End,
   End;

   if (!idata->SGWin) SWDB_RET(FAILURE);

   DoMethod(idata->SWA_NGroupList_SG_L, MUIM_Notify, MUIA_List_Entries,
            MUIV_EveryTime, title, 4, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_T_GSTitle), MUIV_TriggerValue);

   idata->GL_Search =
      MUI_NewObject(SWC_ListSearch,
                    SWA_ListSearch_Title,        GetS(SWS_T_GL_Srch),
                    SWA_ListSearch_List,         idata->SWA_NGroupList_SG_L,
                    SWA_ListSearch_CmpMethod,    SWV_ListSearch_SimpleStr,
                    SWA_ListSearch_Dynamic,      TRUE,
                    SWA_ListSearch_First,        TRUE,
                    SWA_ListSearch_Case,         TRUE,
                    SWA_ListSearch_WindowID,     MAKE_ID('s','g','S','r'),
                    SWA_ListSearch_ItemName,     GetS(SWS_L_Group),
                    SWA_ListSearch_ItemNamePl,   GetS(SWS_L_Groups),
                    TAG_DONE);

   if (!idata->GL_Search) {
      MUI_DisposeObject(idata->SGWin);
      idata->SGWin = idata->b_GLAbt = idata->s_FetchPat = NULL;
      SWDB_RET(FAILURE);
   }

   set(idata->b_GLAbt, MUIA_Disabled, TRUE);

   DoMethod(idata->s_FetchPat, MUIM_Notify, MUIA_String_Acknowledge,
            MUIV_EveryTime, o, 1, SWM_NGroupList_FetchSG);

   DoMethod(b_Fetch,        NOTP, o,           1, SWM_NGroupList_FetchSG);
   DoMethod(b_Clr,          NOTP, idata->SWA_NGroupList_SG_L, 1, MUIM_List_Clear);
   DoMethod(b_Sort,         NOTP, o,           1, SWM_NGroupList_SortSG);
   DoMethod(b_ListSort,     NOTP, o,           1, SWM_NGroupList_SortSG);
   DoMethod(b_ListClip,     NOTP, o,           1, SWM_NGroupList_ClipSG);
   DoMethod(b_Sub,          NOTP, o,           2, SWM_NGroupList_SubSG,
                                                  MUIV_List_Insert_Bottom);
   DoMethod(b_Save,         NOTP, o,           2, SWM_NGroupList_SaveSG, NULL);
   DoMethod(b_Load,         NOTP, o,           2, SWM_NGroupList_LoadSG, NULL);
   DoMethod(b_New,          NOTP, o,           1, SWM_NGroupList_NewSG);
   DoMethod(idata->b_GLAbt, NOTP, o,           1, SWM_NGroupList_AbortSG);

   DoMethod(b_Browse, NOTP,
            o, 7, SWM_Root_ListDoMethod, idata->SWA_NGroupList_SG_L,
            SWV_ListDoMethod_Active, o, 2, SWM_NGroupList_BrowseSG,
            SWV_ListDoMethod_Entry);

   DoMethod(b_SelAll,       NOTP, idata->SWA_NGroupList_SG_L, 4, 
            MUIM_List_Select, MUIV_List_Select_All, MUIV_List_Select_On,
            NULL);

   DoMethod(b_Srch,     NOTP,idata->GL_Search,2,SWM_ListSearch_OpenWin,idata->App);
   DoMethod(b_ListSrch, NOTP,idata->GL_Search,2,SWM_ListSearch_OpenWin,idata->App);

   DoMethod(idata->SGWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            MUIV_Notify_Application, 4,
            MUIM_Application_PushMethod, o, 1, SWM_NGroupList_CloseSGWin);

   DoMethod(idata->SWA_NGroupList_SG_LV, DCLK, o, 2,
            SWM_NGroupList_SubSG, MUIV_List_Insert_Bottom);

   DoMethod(idata->App, OM_ADDMEMBER, idata->SGWin);
   set(idata->SGWin, MUIA_Window_Open,      TRUE);

   set(idata->SGWin, MUIA_Window_ActiveObject,  idata->s_FetchPat);
   set(idata->SGWin, MUIA_Window_DefaultObject, idata->SWA_NGroupList_SG_LV);

   DoMethod(idata->s_FetchPat, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
            idata->SGWin, 3, MUIM_Set, MUIA_Window_ActiveObject, idata->s_FetchPat);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Close Server Group window                                         *
 *****************************************************************************/

METHOD(SWM_NGroupList_CloseSGWin, Msg)
{
   if (!idata->SGWin) SWDB_RET(SUCCESS);

   if (idata->s_FetchPat)
      set(o, SWA_NGroupList_LastFetchPat,
          SWGetStr(idata->s_FetchPat, MUIA_String_Contents));

   DoMethod(o, SWM_NGroupList_AbortSG);

   set(idata->SGWin, MUIA_Window_Open, FALSE);
   DoMethod(idata->App, OM_REMMEMBER, idata->SGWin);

   MUI_DisposeObject(idata->GL_Search);
   MUI_DisposeObject(idata->SGWin);

   FreeSGInfo(idata);

   idata->SGWin                = NULL;
   idata->s_FetchPat           = NULL;
   idata->GL_Search            = NULL;
   b_Fetch                     = NULL;
   idata->SWA_NGroupList_SG_L  = NULL;
   idata->SWA_NGroupList_SG_LV = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: send SG fetch line                                              *
 *****************************************************************************/

static ULONG SendSGFetch(Object *o, SW_IDATA *idata, STRPTR Cmd)
{
   ULONG rc = FAILURE;
   Object *Srv, *SL = NN_ServList(idata);
   
   if (!SL) return FAILURE;

   idata->SGInt = max(idata->SWA_NGroupList_SGUpdateInt, 10);
   
   if (!(idata->GL_Lines = (STRPTR *)AV(idata->SGInt * sizeof(STRPTR))))
      return FAILURE;

   if (Srv = (Object *)DoMethod(SL, SWM_NServerList_GetDefault, TAG_DONE))
      if ((rc = DoMethod(Srv,
                         SWM_NServer_SendCmd,
                         SWT_NServer_Cmd,       Cmd,
                         SWT_NServer_Interval,  idata->SGInt,
                         SWT_NServer_CmdObj,    o,
                         SWT_NServer_CmdMethod, SWM_NGroupList_FetchSG_P,
                         SWT_NServer_Title,     GetS(SWS_T_DLSG),
                         TAG_DONE)) == SUCCESS) {
         set(idata->b_GLAbt, MUIA_Disabled, FALSE);
         set(b_Fetch, MUIA_Disabled, TRUE);
         set(idata->s_FetchPat, MUIA_Disabled, TRUE);
      }

   DoMethod(Srv ? Srv : SL, SWM_Root_CheckErr, idata->SGWin);

   return rc;
}


/*****************************************************************************
 * Method: Fetch Server Groups                                               *
 *****************************************************************************/

METHOD(SWM_NGroupList_FetchSG, Msg)
{
   TEXT CmdBuff[80+sizeof("LIST ACTIVE %s\r\n")];
   STRPTR Patt;
   ULONG  SGPattLen;

   if (!SW_STRISNULL(idata->SWA_NGroupList_SGPatt)) {
      SGPattLen     = strlen(idata->SWA_NGroupList_SGPatt) * 2 + 2;
      if (!(idata->SGPatt = AV(SGPattLen)))
         SWDB_RET(FAILURE);

      if (ParsePatternNoCase(idata->SWA_NGroupList_SGPatt,
                             idata->SGPatt, SGPattLen) < 0) {
         FV(idata->SGPatt);
         idata->SGPatt = NULL;
      }
   }

   if (!idata->s_FetchPat) SWDB_RET(FAILURE);

   Patt = SWGetStr(idata->s_FetchPat, MUIA_String_Contents);
   Patt = SW_STRDEF(Patt, "*");

   sprintf(CmdBuff, "LIST ACTIVE %s\r\n", Patt);

   SWDB_RET(SendSGFetch(o, idata, CmdBuff));
} SDFT


/*****************************************************************************
 * Method: Parse Fetch Server Groups output                                  *
 *****************************************************************************/

METHOD(SWM_NGroupList_FetchSG_P, SWP_NServer_StatusMsg *)
{
   STRPTR c, GrpName;
   LONG   x, GrpCnt;
   
   if (NSR_SBegin(msg)) {
      idata->GL_Srv = NSR_Srv(msg);
      idata->GL_Id  = NSR_ID(msg);
      DoMethod(idata->SWA_NGroupList_SG_L, MUIM_List_Clear);
   }

   if (NSR_SRescode(msg))
      switch (NSR_ResCode(msg)) {
        case SWV_NSR_List:      // fall through
        case SWV_NSR_NewGroups: SWDB_RET(SWV_NServer_RescodeOK);
        default:                SWDB_RET(SWV_NServer_RescodeBad);
      }

   if (NSR_SDone(msg) || NSR_SRun(msg)) {
      for (x=NSR_First(msg), GrpCnt=0; x<=NSR_Last(msg); x++) {
         if (c = strchr(GrpName = NSR_Line(msg, x), ' ')) *c = '\0';
         
         if (!idata->SGPatt || MatchPatternNoCase(idata->SGPatt, GrpName))
            idata->GL_Lines[GrpCnt++] = GrpName;
      }

      if (GrpCnt) {
         set(idata->SWA_NGroupList_SG_L, MUIA_List_Quiet, TRUE);
         DoMethod(idata->SWA_NGroupList_SG_L, MUIM_List_Insert,
                  idata->GL_Lines, GrpCnt, MUIV_List_Insert_Bottom);
         set(idata->SWA_NGroupList_SG_L, MUIA_List_Quiet, FALSE);

         set(idata->GL_Srv, SWA_NServer_NGroups,
             max(SWGetUL(idata->GL_Srv, SWA_NServer_NGroups),
                 SWGetUL(idata->SWA_NGroupList_SG_L, MUIA_List_Entries)));
      }
   }

   if (NSR_SAnyEnd(msg)) {
      if (NSR_SAbort(msg))
         DoMethod(idata->GL_Srv, SWM_Root_CheckErr, idata->SGWin);

      set(idata->b_GLAbt,    MUIA_Disabled, TRUE);
      set(b_Fetch,           MUIA_Disabled, FALSE);
      set(idata->s_FetchPat, MUIA_Disabled, FALSE);
      idata->GL_Srv = NULL, idata->GL_Id  = 0;

      FreeSGInfo(idata);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Abort server group transfer                                       *
 *****************************************************************************/

METHOD(SWM_NGroupList_AbortSG, Msg)
{
   if (!idata->GL_Srv) SWDB_RET(0);
   
   SWDB_RET(DoMethod(idata->GL_Srv, SWM_NServer_Abort, idata->GL_Id));
} SDFT


/*****************************************************************************
 * Method: Sort the list of server groups                                    *
 *****************************************************************************/

METHOD(SWM_NGroupList_SortSG, Msg)
{
   if (!idata->SGWin) SWDB_RET(FAILURE);

   set(idata->App, MUIA_Application_Sleep, TRUE);
   DoMethod(idata->SWA_NGroupList_SG_L, MUIM_List_Sort);
   set(idata->App, MUIA_Application_Sleep, FALSE);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: request SG file                                                 *
 *****************************************************************************/

static STRPTR RequestSGFile(Object *o, ULONG SaveMode, STRPTR FSpec,
                            Object *Win)
{
   return (STRPTR)DoMethod(o, SWM_Root_RequestFileName,
                           SWT_Root_SL_SaveMode,     SaveMode,
                           SWT_Root_SL_Buffer,       FSpec,
                           SWT_Root_SL_Win,          Win,
                           SWT_Root_SL_Title,
                                GetS(SaveMode ?  SWS_T_GSSave : SWS_T_GSLoad),
                           SWT_Root_SL_InitDrawer,   GSDir,
                           SWT_Root_SL_InitPattern,  GSPat,
                           TAG_DONE);
}
                            

/*****************************************************************************
 * Method: Clip selected groups from servergroup list to clipboard           *
 *****************************************************************************/

METHOD(SWM_NGroupList_ClipSG, Msg)
{
   ULONG      x, First = TRUE;
   STRPTR     g;
   IFFHandle *IFF;

   if (!idata->SWA_NGroupList_SG_L) SWDB_RET(FAILURE);
   
   if (!(IFF = SWOpenClipWrite(0))) SWDB_RET(FAILURE);

   set(idata->App, MUIA_Application_Sleep, TRUE);

   SW_MUIListLoopSel(idata->SWA_NGroupList_SG_L, x, g, TRUE) {
      if (!First)
         SWTextToClip(IFF, "\n");

      SWTextToClip(IFF, g);
      First = FALSE;
   }

   SWCloseClipWrite(IFF);

   DoMethod(idata->SWA_NGroupList_SG_L, MUIM_List_Select, MUIV_List_Select_All,
            MUIV_List_Select_Off);

   set(idata->App, MUIA_Application_Sleep, FALSE);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: save the Server Groups list                                       *
 *****************************************************************************/

METHOD(SWM_NGroupList_SaveSG, SWP_NGroupList_SaveSG *)
{
   BPTR fp;
   STRPTR fs;
   TEXT FSpec[FMSIZE];
   STRPTR g;
   ULONG  x;

   if (!idata->SGWin) SWDB_RET(FAILURE);

   fs = SW_STRDEF(msg->Path, RequestSGFile(o, TRUE, FSpec, idata->SGWin));

   if (SW_STRISNULL(fs)) {
      MUI_Request(idata->App, idata->SGWin, 0, NULL,
                  GetS(SWS_B_GSOK), GetS(SWS_B_GSSaveCanc), NULL);
      SWDB_RET(FAILURE);
   }

   set(idata->App, MUIA_Application_Sleep, TRUE);
   
   if (!(fp = Open(fs, MODE_NEWFILE))) {
      SWERR(o, NULL, SWS_Err_GSSaveOpen, fs, 0);
      goto Done;
   }

   SW_MUIListLoop(idata->SWA_NGroupList_SG_L, x, g, TRUE) {
      FPrintf(fp, "%s: \n", g);

      if (!(x % 200))
         DoMethod(NN_App(idata), MUIM_Application_CheckRefresh);
   }

  Done:
   if (fp) Close(fp);
   set(idata->App, MUIA_Application_Sleep, FALSE);

   DoMethod(o, SWM_Root_CheckErr, idata->SGWin);

   SWDB_RET(fp ? SUCCESS : FAILURE);
} SDFT


/*****************************************************************************
 * Function: free a group list file                                          *
 *****************************************************************************/

static void FreeGLInt(SW_IDATA *idata, STRPTR *Groups)
{
   ULONG x;

   if (!Groups) return;

   for (x=0; Groups[x]; x++)
      FV(Groups[x]);

   FV(Groups);
}


/*****************************************************************************
 * Function: Add to group buffer                                             *
 *****************************************************************************/

static ULONG GLAdd(Object *o, SW_IDATA *idata, STRPTR **Groups, ULONG *Size,
                   ULONG *Count, STRPTR GName)
{
   if (!Groups) return FAILURE;
   
   if ((*Count + 1) >= *Size) {
      *Size += SWV_NGroupList_SG_Inc;
      if (RV(*Size * sizeof(STRPTR), Groups) < 0)
         goto Error;
   }

   if (!((*Groups)[(*Count)++] = SDup(GName)))
      goto Error;

   (*Groups)[(*Count)] = NULL;

   return SUCCESS;

  Error:
   if (*Groups) (*Groups)[*Count] = NULL;
   FreeGLInt(idata, *Groups);
   *Groups = NULL;
   return SWERR(o, FAILURE, SWS_Err_Alloc, 0, 0);
}


/*****************************************************************************
 * Function: load a group list file, return ptr to string names              *
 *****************************************************************************/

static STRPTR *LoadGLInt(Object *o, SW_IDATA *idata, Object *Win, STRPTR FSpec,
                         ULONG *Count, ULONG LoadUnSub)
{
   ULONG  Size = 0;
   BPTR   fp;
   STRPTR c, in, *Groups = NULL;

   if (!Count) return NULL;
   *Count = 0;

   if (SW_STRISNULL(FSpec)) {
      MUI_Request(idata->App, Win, 0, NULL,
                  GetS(SWS_B_GSOK), GetS(SWS_B_GSLoadCanc), NULL);
      return NULL;
   }

   if (!(fp = Open(FSpec, MODE_OLDFILE)))
      return (STRPTR *)SWERR(o, NULL, SWS_Err_GSLoadOpen, FSpec, 0);

   if (!(in = AV(SWV_NGroupList_SG_MaxLineLen))) {
      SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
      goto Done;
   }

   while (FGets(fp, in, SWV_NGroupList_SG_MaxLineLen)) {
      if (in[0] == '\n' || in[0] == ',' || in[0] == '-' ||
          (in[0] >= '0' && in[0] <= '9'))
         continue;
      
      if (!((c = strchr(in, ':')) || (c = strchr(in, '!')))) {
         if (Groups) Groups[*Count] = NULL;
         FreeGLInt(idata, Groups);
         Groups = (STRPTR *)SWERR(o, NULL, SWS_Err_GSLoadFmt, FSpec, 0);
         goto Done;
      }

      if (*c == '!' && !LoadUnSub) continue;
      *c = NULL;

      if (GLAdd(o, idata, &Groups, &Size, Count, in) != SUCCESS) goto Done;

      if (!(*Count % 200))
         DoMethod(NN_App(idata), MUIM_Application_CheckRefresh);
   }

   if (Groups)
      Groups[*Count] = NULL;   // null terminate array;

  Done:
   if (in) FV(in);
   if (fp) Close(fp);

   return Groups;
}


/*****************************************************************************
 * Method: Load the Server Groups list                                       *
 *****************************************************************************/

METHOD(SWM_NGroupList_LoadSG, SWP_NGroupList_LoadSG *)
{
   STRPTR fs, *Groups;
   TEXT FSpec[FMSIZE];
   ULONG Count;

   if (!idata->SGWin) SWDB_RET(FAILURE);

   fs = SW_STRDEF(msg->Path, RequestSGFile(o, FALSE, FSpec, idata->SGWin));

   set(NN_App(idata), MUIA_Application_Sleep, TRUE);

   if (Groups = LoadGLInt(o, idata, idata->SGWin, fs, &Count, FALSE)) {
      set(idata->SWA_NGroupList_SG_L, MUIA_List_Quiet, TRUE);
      DoMethod(idata->SWA_NGroupList_SG_L, MUIM_List_Clear);
      DoMethod(idata->SWA_NGroupList_SG_L, MUIM_List_Insert, Groups, Count,
               MUIV_List_Insert_Bottom);
      set(idata->SWA_NGroupList_SG_L, MUIA_List_Quiet, FALSE);

      FreeGLInt(idata, Groups);
   }

   set(NN_App(idata), MUIA_Application_Sleep, FALSE);

   DoMethod(o, SWM_Root_CheckErr, idata->SGWin);

   SWDB_RET(Groups ? SUCCESS : FAILURE);
} SDFT


/*****************************************************************************
 * Method: subscribe from list of groups                                     *
 *****************************************************************************/

METHOD(SWM_NGroupList_Subscribe, SWP_NGroupList_Subscribe *)
{
   ULONG x, gcnt, rc = SUCCESS,
         CurrCnt = SWGetUL(idata->SWA_NList_L, MUIA_List_Entries);
   Object **Groups;

   if (!msg->Groups) SWDB_RET(FAILURE);

   if (msg->Count > SWV_NGroupList_SG_SubWarn)
      if (MUI_Request(NN_App(idata), msg->Win, 0, NULL,
                      GetS(SWS_B_GSSubWarn), GetS(SWS_L_GSSubWarn),
                      msg->Count) != 1) {
         MUI_Request(NN_App(idata), msg->Win, 0, NULL,
                     GetS(SWS_B_OK), GetS(SWS_L_Cancel));
         SWDB_RET(FAILURE);
      }

   if (msg->Dups) *msg->Dups = 0;

   if (!(Groups = (Object **)AV(msg->Count * sizeof(Object *)))) {
      rc = SWERR(o, FAILURE, SWS_Err_Alloc, 0, 0);
      goto Done;
   }

   AB_SetMod(idata);

   set(NN_App(idata), MUIA_Application_Sleep, TRUE);

   set(o, MUIA_List_Quiet, TRUE);

   for (x=gcnt=0; x<msg->Count; x++) {
      if (!msg->Groups[x]) break;
      
      if (DoMethod(o, SWM_NList_FindObj, SWA_NGroup_Name, msg->Groups[x])) {
         if (msg->Dups) (*msg->Dups)++;
         continue;
      }

      if (((CurrCnt + gcnt) >= SWV_NGroupList_MaxUnreg) &&
          NN_RegMsg(idata, GetS(SWS_L_UnregMsg), msg->Win) != SUCCESS)
         break;

      if (!(Groups[gcnt] = MkNewGrp(o, idata, msg->Groups[x])))
         break;

      if (!(gcnt++ % 5))
         DoMethod(NN_App(idata), MUIM_Application_CheckRefresh);
   }

   if (gcnt)
      DoMethod(o, MUIM_List_Insert, Groups, gcnt, msg->InsertPos);
   
   set(o, MUIA_List_Quiet, FALSE);

   set(NN_App(idata), MUIA_Application_Sleep, FALSE);

   if (msg->Dups && *msg->Dups)
      MUI_Request(NN_App(idata), msg->Win, 0, NULL,
                  GetS(SWS_B_OK), GetS(SWS_Note_DupGroups), *msg->Dups);

  Done:
   FV(Groups);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: subscribe to selected                                             *
 *****************************************************************************/

METHOD(SWM_NGroupList_SubSG, SWP_NGroupList_SubSG *)
{
   STRPTR g, *Groups;
   ULONG  x, Dups, count = 0, pos = 0;
   
   if (!idata->SGWin) SWDB_RET(FAILURE);

   SW_MUIListLoopSel(idata->SWA_NGroupList_SG_L, x, g, TRUE)
      count++;

   if (!(Groups = (STRPTR *)AV(count * sizeof(STRPTR)))) {
      SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
      goto Done;
   }

   SW_MUIListLoopSel(idata->SWA_NGroupList_SG_L, x, g, TRUE)
      Groups[pos++] = g;

   DoMethod(o, SWM_NGroupList_Subscribe, Groups, count, &Dups,
            msg->InsertPos, idata->SGWin);

  Done:
   FV(Groups);
   DoMethod(o, SWM_Root_CheckErr, idata->SGWin);

   SWDB_RET(0);   
} SDFT


/*****************************************************************************
 * Method: Drag Query                                                        *
 *****************************************************************************/

METHOD(SWM_ObjList_DragQuery, SWP_ObjList_DragQuery *)
{
   // -- dragsorting handled by ObjListObject --------------------------------
      
   if (msg->obj  == idata->SWA_NGroupList_SG_L && msg->target == idata->SWA_NList_L)
      SWDB_RET(MUIV_DragQuery_Accept);

   SWDB_RET(MUIV_DragQuery_Refuse);
} SDFT


/*****************************************************************************
 * Method: Drag Drop action                                                  *
 *****************************************************************************/

METHOD(SWM_ObjList_DragDrop, SWP_ObjList_DragDrop *)
{
   AB_SetMod(idata);

   // -- reset notifies after a dragsort -------------------------------------
   
   if (msg->obj == idata->SWA_NList_L && msg->target == idata->SWA_NList_L)
      SWDB_RET(DoMethod(o, SWM_NList_SetNotify));

   // -- dragging from server group list to subscribed list ------------------

   if (msg->obj == idata->SWA_NGroupList_SG_L && msg->target == idata->SWA_NList_L) {
      DoMethod(o, SWM_NGroupList_SubSG,
               SWGetUL(msg->target, MUIA_List_DropMark));
      SWDB_RET(0);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: subscribe from newsrc                                             *
 *****************************************************************************/

METHOD(SWM_NGroupList_SubNewsRC, SWP_NGroupList_SubNewsRC *)
{
   STRPTR *Groups;
   ULONG  rc = FAILURE, Count, Dups;
   
   set(idata->App, MUIA_Application_Sleep, TRUE);

   if (Groups = LoadGLInt(o, idata, _win(o), msg->Path, &Count, FALSE))
      rc = DoMethod(o, SWM_NGroupList_Subscribe, Groups, Count, &Dups,
                    MUIV_List_Insert_Bottom, _win(o));

   set(idata->App, MUIA_Application_Sleep, FALSE);

  Done:
   FreeGLInt(idata, Groups);
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Create menu strip                                                 *
 *****************************************************************************/
   
METHOD(SWM_NNews_MenuStrip, Msg)
{
   SWDB_RET(MUI_MakeObject(MUIO_MenustripNM, GroupMenu, 0));
} SDFT


/*****************************************************************************
 * Method: import .newsrc                                                    *
 *****************************************************************************/

METHOD(SWM_NGroupList_ImportNRC, Msg)
{
   ULONG rc;
   TEXT   Path[FMSIZE];

   if (GetVar("HOME", Path, FMSIZE, 0) < 0)
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_NoHome, 0, 0));

   AddPart(Path, ".newsrc", FMSIZE);

   rc = DoMethod(o, SWM_NGroupList_SubNewsRC, Path, FALSE);

   DoMethod(o, SWM_Root_CheckErr, _win(o));

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: import from cache dir                                             *
 *****************************************************************************/

METHOD(SWM_NGroupList_ImportCD, Msg)
{
#  define CCBuff 8192

   STRPTR *Groups = NULL;
   UBYTE  FNBuff[FMSIZE], GName[FMSIZE], *gn;
   BPTR   l = NULL;
   ULONG  Count = 0, Size=0,  more, cnt = 0, rc = FAILURE, Dups;
   struct ExAllData    *buff  = (void *)AV(CCBuff);
   struct ExAllControl *eac = AllocDosObject(DOS_EXALLCONTROL, TAG_DONE);
   struct ExAllData    *ead;
   Object *artmgr = NN_ArtMgr(idata);

   set(NN_App(idata), MUIA_Application_Sleep, TRUE);

   if (!buff || !eac || !artmgr) {
      SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
      goto Done;
   }

   AddPart(strcpy(FNBuff, NN_CacheDir(idata)), SWV_NGroup_GroupDir, FMSIZE);

   if (!(l = Lock(FNBuff, ACCESS_READ)))
      goto Done;

   for (more=1, eac->eac_LastKey=0; more; ) {
      more = ExAll(l, buff, CCBuff, ED_NAME, eac);

      if (!more && (IoErr() != ERROR_NO_MORE_ENTRIES)) {
         SWERR(o, NULL, SWS_Err_ImportCD_Scan, 0, 0);
         goto Done;
      }

      if (eac->eac_Entries == 0) continue;

      for (ead = buff; ead; ead = ead->ed_Next) {
         if (!(cnt++ % 5))
            DoMethod(NN_App(idata), MUIM_Application_CheckRefresh);

         AddPart(strcpy(FNBuff,NN_CacheDir(idata)),SWV_NGroup_GroupDir,FMSIZE);
         AddPart(FNBuff, ead->ed_Name, FMSIZE);
         
         if (gn = (STRPTR)DoMethod(artmgr, SWM_NArticleMgr_CDGroupName,
                                   FNBuff, GName))
            if (GLAdd(o, idata, &Groups, &Size, &Count, GName) != SUCCESS)
               goto Done;
      }
   }

   rc = DoMethod(o, SWM_NGroupList_Subscribe, Groups, Count, &Dups,
                 MUIV_List_Insert_Bottom, _win(o));

  Done:
   if (l) UnLock(l);
   FV(buff);
   if (eac)   FreeDosObject(DOS_EXALLCONTROL, eac);
   set(NN_App(idata), MUIA_Application_Sleep, FALSE);
   FreeGLInt(idata, Groups);

   DoMethod(o, SWM_Root_CheckErr, _win(o));
   
   SWDB_RET(rc);
#  undef CCBuff
} SDFT


/*****************************************************************************
 * Method: import from cache dir                                             *
 *****************************************************************************/

METHOD(SWM_NGroupList_UpdCheck, Msg)
{
   if (idata->Online && idata->SWA_NGroupList_Upd)
      DoMethod(o, SWM_NGroupList_DoGroup, SWV_GC_CheckStats, TRUE);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: new project                                                       *
 *****************************************************************************/

METHOD(SWM_NNews_NewProj, Msg)
{
   DoMethod(idata->SWA_NGroupList_BatchDLFlt, SWM_NSFilter_Expire);
   SWDB_RET(DoMethod(o, SWM_NList_ObjMethodA, msg));
} SDFT


/*****************************************************************************
 * Method: post new project hook                                             *
 *****************************************************************************/
   
METHOD(SWM_AppBase_NewProj, Msg)
{
   DoMethod(idata->SWA_NGroupList_BatchDLFlt, MUIM_List_Clear);
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: update last servergroup date                                    *
 *****************************************************************************/

static void UpdateLastSGDate(Object *o, SW_IDATA *idata)
{
   STRPTR  DateStr =
      VL_Alloc(idata->SWA_AppBase_TopObj, idata,
               "${Year4}${MonthNum}${DayNum} ${Hour24}${Min}${Sec}");

   if (!DateStr) return;

   set(o, SWA_NGroupList_LastSGDate, DateStr);

   VL_Free(o, idata, DateStr);
}


/*****************************************************************************
 * Method: Show new server groups since last time                            *
 *****************************************************************************/

METHOD(SWM_NGroupList_NewSG, Msg)
{
   TEXT    CmdBuff[64];
   
   sprintf(CmdBuff, "NEWGROUPS %s\r\n",
           SW_STRDEF(idata->SWA_NGroupList_LastSGDate, InitSGDate));

   UpdateLastSGDate(o, idata);
   
   SWDB_RET(SendSGFetch(o, idata, CmdBuff));
} SDFT


/*****************************************************************************
 * Method: Browse a group without subscribing to it                          *
 *****************************************************************************/

METHOD(SWM_NGroupList_BrowseSG, SWP_NGroupList_BrowseSG *)
{
   Object *g;
   
   if (SW_STRISNULL(msg->Group)) SWDB_RET(FAILURE);

   // -- if the group is subscribed to, just open it -------------------------
   
   if (g = (Object *)DoMethod(o, SWM_NList_FindObj,
                              SWA_NGroup_Name, msg->Group)) {
      DoMethod(g, SWM_NGroup_StartRead);
      SWDB_RET(SUCCESS);
   }

   // -- otherwise, dispose of any old BrowseGroup laying around... ----------

   DeleteBrowseGrp(o, idata);

   // -- ... and make a new one to read. -------------------------------------

   if (!(idata->BrowseGroup = MkNewGrp(o, idata, msg->Group)))
      SWDB_RET(FAILURE);

   // -- mark the group as a temporary group ---------------------------------
   
   set(idata->BrowseGroup, SWA_NGroup_Flags,
       SWGetUL(idata->BrowseGroup, SWA_NGroup_Flags) |
       SWF_NGroup_TempGroup);
   
   DoMethod(idata->BrowseGroup, SWM_NGroup_StartRead);
      
   SWDB_RET(SUCCESS);
} SDFT
