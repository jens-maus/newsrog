# include "C-NGroup-I.h"
# include "S-SetUIToggle.h"

# ifndef _GENPROTO

/*****************************************************************************
 * Function: Set progress message                                            *
 *****************************************************************************/

static void SetProgMsg(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG title,
                       ULONG Pos, ULONG Max)
{
   Object *TCP;
   ULONG  Secs, PerSecI = 0, PerSecF = 0, M = 0, S = 0, Remain,
          RemainM = 0, RemainS = 0;

   if (Pos % 10) return;
   
   if (!idata->SrvInfo[Cmd].ProgMsg)
      idata->SrvInfo[Cmd].ProgMsg = AV(strlen(GetS(title))+64);

   if (!idata->SrvInfo[Cmd].ProgMsg)
      return;

   if (TCP = (Object *)DoMethod(idata->SrvInfo[Cmd].Srv, SWM_NServer_GetTCP,
                                idata->SrvInfo[Cmd].ID)) {
      Secs      = DoMethod(TCP, SWM_NTCP_RunningTime, NULL);
      M         = Secs / 60;
      S         = Secs % 60;
      Secs      = max(Secs, 1);
      PerSecI   = Pos / Secs;
      PerSecF   = (Pos - (PerSecI * Secs)) * 100 / Secs;

      Remain  = Secs * 100 / max(Pos * 100 / max(Max, 1), 1) - Secs;
      RemainM = Remain / 60;
      RemainS = Remain % 60;
   }
   
   sprintf(idata->SrvInfo[Cmd].ProgMsg, GetS(title),
           Pos, Max, PerSecI, PerSecF, M, S, RemainM, RemainS);

   SendCmdState(o, idata, Cmd, SWV_NGroup_NewProgMsg);
}

   
/*****************************************************************************
 * Function: figure % completion                                             *
 *****************************************************************************/

static void FetchPctComplete(Object *o, SW_IDATA *idata, ULONG Pos)
{
   ULONG Tot     = idata->FetchNumCnt - idata->FetchStart + 1,
         Fract   = Pos                - idata->FetchStart;

   if (Tot)
      set(o, SWA_NGroup_HdrPctDone, 100 * Fract / Tot);


   SetProgMsg(o, idata, SWV_GC_FetchHdr, SWS_T_PM_Hdrs, Fract, Tot - 1);
}


/*****************************************************************************
 * Method: Remove UI                                                         *
 *****************************************************************************/

static Object *RemUI(Object *o, SW_IDATA *idata)
{
   Object *b_arts, *b_thrds, *b_unrem, *UI = PopobjectObject,
      MUIA_Popstring_Button, MkIB(idata, SWS_H_Remove, BF_Remove, '\0'),
      MUIA_Popobject_Object, VGroup,
         GroupFrame,
         MUIA_Background, MUII_GroupBack,
         Child, b_arts  = MkBt(SWS_L_RemArts,  SWS_H_RemArts),
         Child, b_thrds = MkBt(SWS_L_RemThrds, SWS_H_RemThrds),
         Child, b_unrem = MkBt(SWS_L_UnRemove, SWS_H_UnRemove),
      End,
   End;

   if (!UI) return NULL;

   DoMethod(b_arts,  NOTP, UI, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_thrds, NOTP, UI, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_unrem, NOTP, UI, 2, MUIM_Popstring_Close, TRUE);

   DoMethod(b_arts,  NOTP, o, 2, SWM_NGroup_Remove, FALSE);
   DoMethod(b_thrds, NOTP, o, 2, SWM_NGroup_Remove, TRUE);
   DoMethod(b_unrem, NOTP, o, 1, SWM_NGroup_UnRemove);

   return UI;
}


/*****************************************************************************
 * Method: Kill UI                                                           *
 *****************************************************************************/

static Object *KillUI(Object *o, SW_IDATA *idata)
{
   static STRPTR Type[4], When[4], Where[3];
   Object *UI, *exp, *when, *where, *b_Canc, *b_Kill, *type, *expok;

   Type[0]  = GetS(SWS_L_KillArts);
   Type[1]  = GetS(SWS_L_KillThrds);
   Type[2]  = GetS(SWS_L_KillAuth);
   Type[3]  = NULL;
 
   When[0]  = GetS(SWS_L_KillCurrent);
   When[1]  = GetS(SWS_L_KillFuture);
   When[2]  = NULL;

   Where[0] = GetS(SWS_L_KillThisGr);
   Where[1] = GetS(SWS_L_KillAllGr);
   Where[2] = NULL;

   UI = PopobjectObject,
      MUIA_Popstring_Button, MkIB(idata, SWS_H_Kill, BF_Kill,  'k'),
      MUIA_Popobject_Object, VGroup,
         GroupFrame,
         MUIA_Background, MUII_GroupBack,
         Child, type = CycleObject,
            MUIA_Cycle_Entries, Type,
            MUIA_Cycle_Active,  1,
            MUIA_CycleChain,    1,
            MUIA_ControlChar,   't',
            MUIA_ShortHelp,     GetS(SWS_H_KillType),
         End,
         Child, when = CycleObject,
            MUIA_Cycle_Entries, When,
            MUIA_CycleChain,    1,
            MUIA_ControlChar,   'w',
            MUIA_ShortHelp,     GetS(SWS_H_KillWhen),
         End,
         Child, ColGroup(2),
            Child, Label2(GetS(SWS_L_KillExp)),
            Child, HGroup,
               Child, expok = CheckMark(TRUE),
               Child, exp = NewObject((struct IClass *)
                                      DoMethod(idata->SWA_NGroup_GrpDefFlt,
                                               SWM_NSFilter_DisuseSliderClass), NULL,
                                      MUIA_Numeric_Min,           1,
                                      MUIA_Numeric_Max,           365*2,
                                      MUIA_Numeric_Value,         14,
                                      MUIA_CycleChain,            1,
                                      MUIA_ShortHelp,             GetS(SWS_H_KillExp),
                                      MUIA_Disabled,              TRUE,
                                      MUIA_FixWidth,              250,
                                      TAG_DONE),
            End,
            Child, Label2(GetS(SWS_L_KillLoc)),
            Child, where = CycleObject,
               MUIA_Cycle_Entries, Where,
               MUIA_CycleChain,    1,
               MUIA_ControlChar,   'l',
               MUIA_ShortHelp,     GetS(SWS_H_KillWhere),
               MUIA_Disabled,     TRUE,
            End,
         End,
         Child, HGroup,
            Child, b_Kill = MkBt(SWS_L_KillKill, SWS_H_KillKill),
            Child, b_Canc = MkBt(SWS_L_KillCanc, SWS_H_KillCanc),
         End,
      End,
   End;

   SetAttrs(expok, MUIA_Disabled, TRUE, MUIA_CycleChain, 1, TAG_DONE);
   
   DoMethod(expok, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            exp, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

   DoMethod(when, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            exp, 9, MUIM_MultiSet, MUIA_Disabled, MUIV_NotTriggerValue,
            exp, expok, where, NULL);

   DoMethod(b_Canc, NOTP, UI, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_Kill, NOTP, UI, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_Kill, NOTP, o,  6, SWM_NGroup_KillSel,
            type, when, exp, expok, where);

   return UI;
}


/*****************************************************************************
 * Method: Filter UI                                                         *
 *   Mode == 0: Select filter                                                *
 *   Mode == 1: Display filter                                               *
 *   Mode == 2: Action filter                                                *
 *   Mode == 3: Highlight filter                                             *
 *****************************************************************************/

static Object *FltUI(Object *o, SW_IDATA *idata, LONG Mode)
{
   Object *b_Upd, *b_Close, *b_Edit, *BGrp, *L,
          *GlobFlt = SWGetObj(idata->SWA_AppBase_TopObj,
                              Mode == 0 ? SWA_NNews_GlobSel :
                              Mode == 1 ? SWA_NNews_GlobDsp :
                              Mode == 2 ? SWA_NNews_GlobAct :
                              Mode == 3 ? SWA_NNews_GlobHL : 0),
          *LocFlt  = Mode == 0 ? idata->SWA_NGroup_SelFlt :
                     Mode == 1 ? idata->SWA_NGroup_DispFlt :
                     Mode == 2 ? idata->SWA_NGroup_ActFlt : 0,
          *UI;
   ULONG  Use    = Mode == 0 ? SWV_NSFList_Use_Select  :
                   Mode == 1 ? SWV_NSFList_Use_Display :
                   Mode == 2 ? SWV_NSFList_Use_Action  :
                   Mode == 3 ? SWV_NSFList_Use_Hlight  : 0;

   Object **FObj = Mode == 0 ? &idata->Filter_GlobSel :
                   Mode == 1 ? &idata->Filter_GlobDsp :
                   Mode == 2 ? &idata->Filter_GlobAct :
                   Mode == 3 ? &idata->Filter_GlobHL  : 0;
   
   ULONG VHelp   = Mode == 0 ? SWS_H_SView :
                   Mode == 1 ? SWS_H_DView :
                   Mode == 2 ? SWS_H_AView :
                   Mode == 3 ? SWS_H_HView : 0,
   
         UIMth   = Mode == 0 ? SWM_NSFilter_UI :
                   Mode == 1 ? SWM_NDFilter_UI :
                   Mode == 2 ? SWM_NAFilter_UI :
                   Mode == 3 ? SWM_NSFilter_UI : 0,
            
         CgMth   = Mode == 0 ? SWM_NGroup_ChgSelFlt :
                   Mode == 1 ? SWM_NGroup_ChgDspFlt :
                   Mode == 2 ? SWM_NGroup_RunActFlt :
                   Mode == 3 ? SWM_NGroup_ChgHLFlt  : 0,

         BUpd    = Mode == 2 ? SWS_B_Run : SWS_B_Upd,
         HUpd    = Mode == 2 ? SWS_H_Run : SWS_H_Upd;

   LONG  VKey    = Mode == 0 ? 'v' :
                   Mode == 1 ? 'V' :
                   Mode == 2 ? '!' :
                   Mode == 3 ? 'H' : 0;

   STRPTR Type   = Mode == 0 ? "#?NSFilter#?" :
                   Mode == 1 ? "#?NDFilter#?" :
                   Mode == 2 ? "#?NAFilter#?" :
                   Mode == 3 ? "#?NSFilter#?" : 0;

   STRPTR Icon   = Mode == 0 ? BF_View :
                   Mode == 1 ? BF_View :
                   Mode == 2 ? BF_Action :
                   Mode == 3 ? BF_HLight : 0;

   BGrp = HGroup,
      Child, b_Upd   = MkBt(BUpd,        HUpd),
      Child, b_Edit  = MkBt(SWS_B_Edit,  SWS_H_Edit),
      Child, b_Close = MkBt(SWS_B_Close, SWS_H_Close),
   End;

   if (!BGrp) return NULL;

   if (Mode != 3) {
      UI = PopobjectObject,
         MUIA_Popstring_Button, MkIB(idata, VHelp, Icon, VKey),
         MUIA_Popobject_Object, VGroup,
            GroupFrame,
            MUIA_Background, MUII_GroupBack,
            Child, HGroup,
               MUIA_FixWidthTxt, "DEFGhijklMnopqrSTUSBARZblat"
                                 "DEFGhijklMnopqrSTUSBARZblat",
               Child, HGroup,
                  GroupFrameT(GetS(SWS_T_LocFlt)),
                  Child, DoMethod((NSFListObject,
                                   AppBaseDefTop(idata),
                                   SWA_NSFList_UIMode,  SWV_NSFList_UI_Tiny,
                                   SWA_NSFList_FltType, Type,
                                   SWA_NList_Master,    LocFlt,
                                   SWA_NSFList_Group,   o,
                                   SWA_NSFList_FFToolBar, idata->FFToolBar,
                                   SWA_NSFList_Use,     Use,
                                   End), UIMth),
               End,
               Child, BalanceObject, End,
               Child, HGroup,
                  GroupFrameT(GetS(SWS_T_GlobFlt)),
                  Child, DoMethod(*FObj =
                                  (NSFListObject,
                                   AppBaseDefTop(idata),
                                   SWA_NList_Master,    GlobFlt,
                                   SWA_NSFList_FltType, Type,
                                   SWA_NSFList_UIMode,  SWV_NSFList_UI_Tiny,
                                   SWA_NSFList_Group,   o,
                                   SWA_NSFList_FFToolBar, idata->FFToolBar,
                                   SWA_NSFList_Use,     Use,
                                   End), UIMth),
               End,
            End,
            Child, BGrp,
         End,
      End;
   } else {
      UI = PopobjectObject,
         MUIA_Popstring_Button, MkIB(idata, VHelp, Icon, VKey),
         MUIA_Popobject_Object, VGroup,
            GroupFrame,
            MUIA_Background, MUII_GroupBack,
            Child, HGroup,
               GroupFrameT(GetS(SWS_T_GlobFlt)),
               MUIA_FixWidthTxt, "DEFGhijklMnopqrSTUSBARZblat",
               Child, DoMethod(*FObj =
                               (NSFListObject,
				AppBaseDefTop(idata),
                                SWA_NList_Master,    GlobFlt,
                                SWA_NSFList_FltType, Type,
                                SWA_NSFList_UIMode,  SWV_NSFList_UI_Tiny,
                                SWA_NSFList_Group,   o,
                                SWA_NSFList_FFToolBar, idata->FFToolBar,
                                SWA_NSFList_Use,     Use,
                                End), UIMth),
            End,
            Child, BGrp,
         End,
      End;

      set(*FObj, SWA_NSFList_DefMatch, FALSE);
   }

   DoMethod(b_Close, NOTP, UI, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_Upd,   NOTP, UI, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_Upd,   NOTP, o,  2, CgMth,                0);

   L = SWGetObj(*FObj, SWA_NList_L);
   
   DoMethod(b_Edit, MUIM_Notify, MUIA_Pressed, FALSE,
            o, 7, SWM_Root_ListDoMethod, L,
            SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 2,
            SWM_NSFilter_ShowUI, idata->App);

   DoMethod(SWGetObj(*FObj, SWA_NList_LV), DCLK,
            o, 7, SWM_Root_ListDoMethod, L,
            SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 2,
            SWM_NSFilter_ShowUI, idata->App);

   return UI;
}


/*****************************************************************************
 * Function: fixed width space                                               *
 *****************************************************************************/

static Object *FWS(void)
{
   return RectangleObject, MUIA_FixWidth, 2, End;
}


/*****************************************************************************
 * Function: clear buf, increment pos                                        *
 *****************************************************************************/

static Object *InfoText(ULONG i)
{
   return  ListviewObject,
      TextFrame,
      MUIA_VertWeight,     20,
      MUIA_Listview_List, FloattextObject,
         MUIA_Background,     MUII_TextBack,
         MUIA_Floattext_Text, GetS(i),
      End,
   End;
}


/*****************************************************************************
 * Method: Start reading the group                                           *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_StartRead, Msg)
{
   ULONG WasOpen = idata->ReaderWin != NULL;
   struct DateStamp Now;

   // -- provide default val for MaxKeep -------------------------------------
   
   if (idata->SWA_NGroup_MaxKeep == 10001)
      idata->SWA_NGroup_MaxKeep = idata->SWA_NGroup_MaxArticles;

   if (GP_NoName(idata))
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_NoName, 0, 0));

   if (!idata->SWA_NGroup_ArtMgr)                        SWDB_RET(FAILURE);

   idata->SWA_NGroup_ThreadMgr =
      MUI_NewObject(SWC_NArticleMgr,
                    AppBaseDefTop(idata),
                    SWA_NArticleMgr_Threads,     TRUE,
                    SWA_NArticleMgr_Master,      FALSE,
                    TAG_DONE);

   if (!idata->SWA_NGroup_ThreadMgr)                     SWDB_RET(FAILURE);

   if (DoMethod(o, SWM_NGroup_OpenReaderWin, TAG_DONE)
       != SUCCESS)                                       SWDB_RET(FAILURE);
   if (WasOpen)                                          SWDB_RET(SUCCESS);

   if (DoMethod(idata->SWA_NGroup_ArtMgr,SWM_NArticleMgr_AddGroup,o)!=SUCCESS)
      SWDB_RET(FAILURE);

   set(o, SWA_NGroup_Status, GetS(SWS_L_Status_Idle));

   DoMethod(o, SWM_NGroup_LoadOV,       TAG_DONE);

   // -- position last article in the artlist --------------------------------

   MoveToStartArt(o, idata);

   // -- see if group was shut down properly last time -----------------------

   if (CacheMk(o, idata, SWV_NGroup_TestChMk))
      DoMethod(o, SWM_NGroup_CleanCache);

   // -- set datestamp so we know when we last read the group ----------------

   DateStamp(&Now);
   SetAttrs(o,
            SWA_NGroup_RdDays,     Now.ds_Days,
            SWA_NGroup_RdMin,      Now.ds_Minute,
            SWA_NGroup_RdTick,     Now.ds_Tick,
            SWA_NGroup_PrevArtNum, idata->SWA_NGroup_LastArtNum,
            SWA_NGroup_NewArtNum,  0,
            TAG_DONE);            

   // -- download new headers if online & in DLHdrStart is set ---------------

   if (idata->SWA_NGroup_SrvList    &&
       idata->SWA_NGroup_DLHdrStart &&
       IsOnline(idata))
      DoMethod(o, SWM_NGroup_Fetch, TAG_DONE);

   DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);

   AB_SetMod(idata);

   idata->BrowseThPos = -1;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Stop reading the group                                            *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_StopRead, SWP_NGroup_StopRead *)
{
   Object *LastArt;
   
   AbortAll(o, idata);
   
   if (!idata->ReaderWin) SWDB_RET(0);

   if (!FIsSet(idata->SWA_NGroup_Flags,SWF_NGroup_TempGroup)) {

      // -- save last article`s msg ID ---------------------------------------

      set(o, SWA_NGroup_LastMsgID, NULL);

      if (idata->ArtList) {
         DoMethod(idata->ArtList, MUIM_List_GetEntry, MUIV_List_GetEntry_Active,
                  &LastArt);
   
         if (LastArt)
            set(o, SWA_NGroup_LastMsgID,
                DoMethod(LastArt, SWM_NArticle_GetMsgID));
   
         SetAttrs(o,
                  SWA_NGroup_PrevArtNum, idata->SWA_NGroup_LastArtNum,
                  SWA_NGroup_NewArtNum,  0,
                  TAG_DONE);
      }

      // -- save headers/uncache/etc -----------------------------------------

      if (msg->Save) {
         if (!msg->Force)
            if (AskUnCache(o,idata,SWS_L_KeepDelete,
                           SWS_L_AskDelCache,TRUE,2,2,1)==0)
               SWDB_RET(0);
         
         if (idata->SWA_NGroup_ReadOnExit)
            DoMethod(o, SWM_NGroup_Mark, TRUE, TRUE, FALSE, TRUE);
   
         DoMethod(idata->SWA_NGroup_SelFlt,  SWM_NSFilter_Expire);
         DoMethod(idata->SWA_NGroup_KillFlt, SWM_NSFilter_Expire);
   
         ExpireOld(o, idata, TRUE, LONG_MAX);
         ResetNew(o, idata);
         
         DoMethod(o, SWM_NGroup_SaveOV, TAG_DONE);

         // -- autosave if needed --------------------------------------------
   
         if (NN_AutoSave(idata) == SWV_NNews_AS_PostGrpClose && !msg->Force)
            DoMethod(idata->SWA_AppBase_TopObj, SWM_AppBase_M_Save,
                     idata->ReaderWin);
      } else
         if (!msg->Force)
            if (AskUnCache(o, idata, SWS_L_OkCanc, SWS_L_NoSvWarn,
                           FALSE, 0, -2, -2)!=1)
               SWDB_RET(0);
   }

   // -- remove window, etc --------------------------------------------------

   DLQueueClear(o, idata);
   DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);
   DoMethod(o, SWM_NGroup_CloseReaderWin);

   NukeArtArrays(idata);

   // -- tell artmgr we`re no longer open ------------------------------------

   DoMethod(idata->SWA_NGroup_ArtMgr,  SWM_NArticleMgr_RemGroup, o);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: close reader window                                               *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_CloseReaderWin, Msg)
{
   if (!idata->ReaderWin) SWDB_RET(NULL);

   DoMethod(idata->FFTBGrp, MUIM_Group_InitChange);
   DoMethod(idata->FFTBGrp, OM_REMMEMBER, idata->FFToolBar);
   DoMethod(idata->FFTBGrp, MUIM_Group_ExitChange);
   
   DoMethod(o, MUIM_KillNotifyObj, SWA_NGroup_UseArtFont,
            idata->SWA_NGroup_ArtView);

   DoMethod(o, MUIM_KillNotifyObj, SWA_NGroup_UseArtCol,
            idata->SWA_NGroup_ArtView);
   
   DoMethod(o, MUIM_KillNotifyObj, SWA_NGroup_ArtFont,
            idata->SWA_NGroup_ArtView);
   
   SWKillAttrUI(SW_ATTRMAP, idata->AttrUI, o);

   DoMethod(idata->SWA_NGroup_SelFlt,  SWM_NSFilter_HideUI);
   DoMethod(idata->SWA_NGroup_KillFlt, SWM_NSFilter_HideUI);
   DoMethod(idata->SWA_NGroup_DispFlt, SWM_NDFilter_HideUI);
   DoMethod(idata->SWA_NGroup_ActFlt,  SWM_NAFilter_HideUI);
   DoMethod(idata->Filter_GlobSel,     SWM_NSFilter_HideUI);
   DoMethod(idata->Filter_GlobHL,      SWM_NSFilter_HideUI);
   DoMethod(idata->Filter_GlobKill,    SWM_NSFilter_HideUI);
   DoMethod(idata->Filter_GlobDsp,     SWM_NDFilter_HideUI);
   DoMethod(idata->Filter_GlobAct,     SWM_NAFilter_HideUI);

   MUI_DisposeObject(idata->Filter_GlobSel);
   MUI_DisposeObject(idata->Filter_GlobHL);
   MUI_DisposeObject(idata->Filter_GlobKill);
   MUI_DisposeObject(idata->Filter_GlobDsp);
   MUI_DisposeObject(idata->Filter_GlobAct);
   idata->Filter_GlobSel  = NULL;
   idata->Filter_GlobHL   = NULL;
   idata->Filter_GlobKill = NULL;
   idata->Filter_GlobDsp  = NULL;
   idata->Filter_GlobAct  = NULL;

   set(idata->ReaderWin, MUIA_Window_Open, FALSE);
   DoMethod(idata->App, OM_REMMEMBER, idata->ReaderWin);

   if (idata->ArtViewWin && idata->ArtViewWin != idata->ReaderWin) {
      set(idata->ArtViewWin, MUIA_Window_Open, FALSE);
      DoMethod(idata->App, OM_REMMEMBER, idata->ArtViewWin);
   }
   
   // -- remove the objects we don`t want deleted ----------------------------

   DoMethod(o, MUIM_KillNotifyObj, SWA_NGroup_Status, idata->StatMsg);

   DoMethod(NN_TCPList(idata), MUIM_KillNotifyObj, SWA_NTCPList_Online, o);

   if (idata->B_ThCnt)
      DoMethod(idata->G_Thrd, OM_REMMEMBER, idata->B_ThCnt);

   MUI_DisposeObject(idata->FFToolBar);
   MUI_DisposeObject(idata->ALQSearch);
   MUI_DisposeObject(idata->ReaderWin);
   MUI_DisposeObject(idata->B_Th1);
   MUI_DisposeObject(idata->B_Th2);
   MUI_DisposeObject(idata->B_Th3);

   if (idata->ArtViewWin && idata->ArtViewWin != idata->ReaderWin)
      MUI_DisposeObject(idata->ArtViewWin);

   idata->B_Th1                = NULL;
   idata->B_Th2                = NULL;
   idata->B_Th3                = NULL;
   idata->B_ThCnt              = NULL;
   idata->ReaderWin            = NULL;
   idata->ArtViewWin           = NULL;
   idata->ArtList              = NULL;
   idata->StatMsg              = NULL;
   idata->ProgGauge            = NULL;
   idata->SWA_NGroup_ArtView   = NULL;
   idata->ALQSearch            = NULL;
   idata->FFToolBar            = NULL;
   idata->FFTBGrp              = NULL;

   set(o, SWA_NGroup_ReaderOpen, FALSE);
   FReset(idata->SWA_NGroup_PermFlags, SWF_NGroup_WinOpen);

   DoMethod(o, SWM_NGroup_CloseStatWin);

   MUI_DisposeObject(idata->Filter_Read);
   MUI_DisposeObject(idata->Filter_Auth);
   MUI_DisposeObject(idata->Filter_ThBase);
   MUI_DisposeObject(idata->Filter_BaseMsgID);
   idata->Filter_Read      = NULL;
   idata->Filter_Auth      = NULL;
   idata->Filter_ThBase    = NULL;
   idata->Filter_BaseMsgID = NULL;
   idata->Filter_MsgID     = NULL;

   SWDB_RET(NULL);
} SDFT


/*****************************************************************************
 * Method: open reader window                                                *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_OpenReaderWin, ULONG *)
{
   ULONG rc = FAILURE;
   Object *vg, *b_bf, *b_br, *b_pn, *b_pf, *b_af, *b_ab, *b_tf, *b_tb, *qs,
          *b_faq, *ArtView = NULL, *ArtList = NULL, *bal = NULL;
   static STRPTR RegLab[5];
   ULONG  ArtListWght = NN_ArtListWght(idata),
          UseAVWin    = NN_GrpWinMode(idata) == SWV_NNews_GW_Separate,
          FFTBLoc     = NN_FFTBLoc(idata),
          FFTBHoriz   = (FFTBLoc == SWV_NNews_FFTB_Top    ||
                         FFTBLoc == SWV_NNews_FFTB_Bottom ||
                         FFTBLoc == SWV_NNews_FFTB_ArtView);
   struct TagItem *ti = (struct TagItem *)(msg+1);

   UseAVWin = GetTagUL(SWT_NGroup_GrpWinMode, UseAVWin, ti);
   
   if (!idata->SWA_AppBase_TopObj) SWDB_RET(FAILURE);

   // -- if window exists, just bring it to the front ------------------------
   if (idata->ReaderWin) {
      set(idata->ReaderWin, MUIA_Window_Open, TRUE);
      DoMethod(idata->ReaderWin, MUIM_Window_ToFront);
      SWDB_RET(SUCCESS);
   }

   SetAttrs(idata->SWA_NGroup_SelFlt,  AppBaseDefTop(idata), TAG_DONE);
   SetAttrs(idata->SWA_NGroup_KillFlt, AppBaseDefTop(idata), TAG_DONE);

   SetAttrs(idata->SWA_NGroup_DispFlt,
            AppBaseDefTop(idata),
            SWA_NSFList_FltType, "#?NDFilter#?",
            TAG_DONE);

   SetAttrs(idata->SWA_NGroup_ActFlt,
            AppBaseDefTop(idata),
            SWA_NSFList_FltType, "#?NAFilter#?",
            TAG_DONE);

   if (!(idata->App = NN_App(idata)))
      goto Done;

   idata->ProgPos = -1;

   RegLab[0]  = GetS(SWS_L_ReadNews);
   RegLab[1]  = GetS(SWS_L_SelFilters);
   RegLab[2]  = GetS(SWS_L_DspFilters);
   RegLab[3]  = GetS(SWS_L_Options);
   RegLab[4]  = NULL;

   idata->Filter_ThBase    = MUI_NewObject(SWC_NSF_ThreadBase, TAG_DONE);
   idata->Filter_Read      = MUI_NewObject(SWC_NSF_Read,
                                           SWA_NSFilter_Enabled, FALSE,
                                           SWA_NSFilter_ThreadMode,
                                             SWV_NSFilter_TM_Any,
                                           TAG_DONE);

   idata->StrF_Auth        = MUI_NewObject(SWC_NStrFilter,
                                   SWA_NStrFilter_Header, SWV_HM_From,
                                   SWA_NStrFilter_Case,   TRUE,
                                   SWA_NStrFilter_Wild,   FALSE,
                                   SWA_NStrFilter_Anchor, TRUE,
                                   TAG_DONE);
                                           
   idata->Filter_Auth      = MUI_NewObject(SWC_NSF_HeaderMatch,
                                           SWA_NSFilter_Enabled, FALSE,
                                           TAG_DONE);

   DoMethod(idata->Filter_Auth, MUIM_List_InsertSingle, idata->StrF_Auth,
            MUIV_List_Insert_Bottom);

   idata->Filter_MsgID     = MUI_NewObject(SWC_NSF_SubjMatch, TAG_DONE);

   idata->Filter_BaseMsgID = MUI_NewObject(SWC_NSF_MatchThreadBase,
                                           SWA_NSFilter_Enabled, FALSE,
                                           SWA_NSF_MatchThreadBase_FPtr,
                                               idata->Filter_MsgID,
                                           TAG_DONE);

   if (!idata->Filter_Read || !idata->Filter_ThBase || !idata->Filter_MsgID ||
       !idata->Filter_BaseMsgID || !idata->StrF_Auth)
      goto Done;

   if (!(idata->B_Th1 = MkIB(idata, SWS_H_Th1, BF_1Art, 't')) ||
       !(idata->B_Th2 = MkIB(idata, SWS_H_Th2, BF_2Art, 't')) ||
       !(idata->B_Th3 = MkIB(idata, SWS_H_Th3, BF_3Art, 't')))
      goto Done;

   ArtListWght = max(min(ArtListWght, 90), 10);

   if (!(idata->FFToolBar = GroupObject,
            InnerSpacing(0,0),
            MUIA_Group_Horiz,    FFTBHoriz,
            MUIA_HorizWeight,    1,
            MUIA_VertWeight,     1,
	    MUIA_Group_Spacing,  2,
            Child, RectangleObject,
               MUIA_VertWeight,   1,
               MUIA_HorizWeight,  1,
            End,
         End))
      goto Done;

   switch (FFTBLoc) {
     case SWV_NNews_FFTB_ArtView:
      idata->FFTBGrp = HGroup,
         InnerSpacing(0,0),
         MUIA_VertWeight,  0,
         Child, idata->FFToolBar,
      End;
      break;

     case SWV_NNews_FFTB_Top:
     case SWV_NNews_FFTB_Bottom:
      idata->FFTBGrp = HGroup,
         InnerSpacing(0,0),
         GroupFrame,
         MUIA_Background, MUII_GroupBack,
         MUIA_VertWeight,  0,
         Child, idata->FFToolBar,
         Child, RectangleObject,
            MUIA_HorizWeight,   10000,
         End,
      End;
      break;

     case SWV_NNews_FFTB_Left:
     case SWV_NNews_FFTB_Right:
      idata->FFTBGrp = VGroup,
         InnerSpacing(0,0),
         GroupFrame,
         MUIA_Background, MUII_GroupBack,
         MUIA_HorizWeight,  0,
         Child, RectangleObject,
            MUIA_FixHeight,    20,
            MUIA_HorizWeight,  0,
         End,
         Child, idata->FFToolBar,
         Child, RectangleObject,
            MUIA_VertWeight,    10000,
         End,
      End;
      break;
   }

   if (!idata->FFTBGrp) goto Done;

   ArtList = HGroup,
      GroupSpacing(6),
      GroupFrameT(GetS(SWS_L_ArticleList)),
      MUIA_VertWeight, ArtListWght,
      Child, idata->ArtList = NArticleListObject,
         AppBaseDefTop(idata),
         SWA_NList_NoNotifies,      TRUE,
         MUIA_ShortHelp,            GetS(SWS_H_ArtList),
         MUIA_HorizWeight,          90,
         SWA_NArticleList_MemPool,  idata->SWA_NGroup_MemPool,
         SWA_NArticleList_ThreadMgr,idata->SWA_NGroup_ThreadMgr,
         SWA_NArticleList_Sort1,    idata->SWA_NGroup_Sort1,
         SWA_NArticleList_Dir1,     idata->SWA_NGroup_Dir1,
         SWA_NArticleList_Sort2,    idata->SWA_NGroup_Sort2,
         SWA_NArticleList_Dir2,     idata->SWA_NGroup_Dir2,
         SWA_NArticleList_Sort3,    idata->SWA_NGroup_Sort3,
         SWA_NArticleList_Dir3,     idata->SWA_NGroup_Dir3,
         SWA_NArticleList_ListCol,  idata->SWA_NGroup_ArtListCol,
         SWA_NArticleList_Group,    o,
         SWA_NArticleList_SortBt,   MkIB(idata, SWS_H_ALSort, BF_Sort, '='),
         SWA_NArticleList_ToolBar,  HGroup,
            MUIA_Group_HorizSpacing, 3,
            Child, idata->B_Up   = ImageB(MUII_ArrowUp,   'u', SWS_H_TUp),
            Child, idata->B_Dn   = ImageB(MUII_ArrowDown, 'd', SWS_H_TDn),
            Child, b_faq         = MkIB(idata, SWS_H_FAQb, BF_FAQ, 'Q'),
            Child, FltUI (o, idata, 3),
            Child, idata->G_Thrd = HGroup,
               MUIA_Group_HorizSpacing, 0,
               Child, idata->B_ThCnt = idata->B_Th2,
            End,
            Child, idata->B_Read = MkTB(idata, SWS_H_ReadFlt, BF_Read,  'R'),
            Child, idata->B_Unrd = MkTB(idata, SWS_H_UnrdFlt, BF_Unread,'U'),
            Child, idata->B_Auth = MkTB(idata, SWS_H_AuthFlt, BF_1Auth, 'A'),
            Child, RemUI(o, idata),
            Child, KillUI(o, idata),
            Child, FltUI (o, idata, 0),
            Child, FltUI (o, idata, 2),
            Child, qs = MkIB(idata, SWS_H_ALQSearch, BF_Search, '/'),
         End,
      End,
   End;

   if (!ArtList) goto Done;

   ArtView = HGroup,
      MUIA_VertWeight, 100-ArtListWght,
      GroupFrameT(GetS(SWS_L_ArticleReader)),
      (FFTBLoc == SWV_NNews_FFTB_Left ? Child : TAG_IGNORE), idata->FFTBGrp,
      Child, idata->SWA_NGroup_ArtView = NArticleViewObject,
         AppBaseDefTop(idata),
         SWA_NArticleView_UseArtFont, idata->SWA_NGroup_UseArtFont,
         SWA_NArticleView_UseArtCol,  idata->SWA_NGroup_UseArtCol,
         SWA_NArticleView_ArtFont,    idata->SWA_NGroup_ArtFont,
         SWA_NArticleView_ArtCol,    &idata->SWA_NGroup_Pens[PEN_ArtCol],
         SWA_NArticleView_DspList,    idata->SWA_NGroup_DispFlt,
         SWA_NArticleView_DspGlob,    idata->Filter_GlobDsp,
         SWA_NArticleView_ToolBar,  HGroup,
            MUIA_Group_HorizSpacing, 3,
            (FFTBLoc == SWV_NNews_FFTB_ArtView ? Child : TAG_IGNORE), idata->FFTBGrp,
            Child, FWS(),
            Child, b_br = MkIB(idata, SWS_H_BrowseR, BF_BrowseR, ','),
            Child, b_bf = MkIB(idata, SWS_H_BrowseF, BF_BrowseF, '.'),
            Child, FWS(),
            Child, b_ab = MkIB(idata, SWS_H_ArtBck,  BF_ABack,   '['),
            Child, b_af = MkIB(idata, SWS_H_ArtFwd,  BF_AFwd,    ']'),
            Child, FWS(),
            Child, b_tb = MkIB(idata, SWS_H_ThreadBck, BF_TBack, '\0'),
            Child, b_tf = MkIB(idata, SWS_H_ThreadFwd, BF_TFwd,  '\0'),
            Child, FWS(),
            Child, b_pn = MkIB(idata, SWS_H_PostNew, BF_PNew,    'N'),
            Child, b_pf = MkIB(idata, SWS_H_PostFup, BF_PFup,    'F'),
            Child, FWS(),
            Child, FltUI (o, idata, 1),
         End,
      End,
      (FFTBLoc == SWV_NNews_FFTB_Right ? Child : TAG_IGNORE), idata->FFTBGrp,
   End;

   if (FFTBLoc == SWV_NNews_FFTB_Bottom ||
       FFTBLoc == SWV_NNews_FFTB_Top)
      ArtView = VGroup,
         InnerSpacing(0, 0),
         (FFTBLoc == SWV_NNews_FFTB_Top ? Child : TAG_IGNORE), idata->FFTBGrp,
         Child, ArtView,
         (FFTBLoc == SWV_NNews_FFTB_Bottom ? Child : TAG_IGNORE), idata->FFTBGrp,
      End;

   if (!ArtView) goto Done;

   if (!UseAVWin)
      bal = BalanceObject, End;
   
   idata->ReaderWin = WindowObject,
      MUIA_Window_Title,       idata->SWA_NGroup_Name,
      MUIA_Window_ID,          SWGetUL(o, SWA_ObjClassHash) + (UseAVWin ? 1 : 0),
      MUIA_Window_IsSubWindow, TRUE,
      MUIA_Window_Menustrip,   MUI_MakeObject(MUIO_MenustripNM, MainMenu, 0),
      WindowContents, vg = RegisterGroup(RegLab),
         MUIA_CycleChain, 1,
         MUIA_Background, MUII_RegisterBack,
         Child, VGroup,
            GroupSpacing(5),
            MUIA_Background, MUII_GroupBack,
            Child, HGroup,
               MUIA_Background, MUII_TextBack,
               MUIA_VertWeight, 1,
               TextFrame,
               Child, HGroup,
                  MUIA_HorizWeight, 70,
                  Child, idata->StatMsg = TextObject,
                     MUIA_Text_PreParse, MUIX_L,
                     MUIA_Text_Contents, idata->SWA_NGroup_Status,
                  End,
                  Child, HSpace(0),
               End,
               Child, idata->ProgGauge = GaugeObject,
                  GaugeFrame,
                  MUIA_HorizWeight, 30,
                  MUIA_Gauge_Horiz, TRUE,
               End,
               Child, RectangleObject, MUIA_HorizWeight, 2, End,
               Child, idata->B_ProgStop = ImageB(MUII_TapeStop, 'S', SWS_H_Stop),
            End,
            Child, ArtList,
            !UseAVWin ? Child : TAG_IGNORE, !UseAVWin ? bal     : NULL,
            !UseAVWin ? Child : TAG_IGNORE, !UseAVWin ? ArtView : NULL,
         End,
         Child, ColGroup(2),
            GroupSpacing(5),
            Child, VGroup,
               GroupFrameT(GetS(SWS_L_SelFltTitle)),
               Child, InfoText(SWS_L_SelFilterTxt),
               Child, DoMethod(idata->SWA_NGroup_SelFlt,  SWM_NSFilter_UI),
            End,
            Child, VGroup,
               GroupFrameT(GetS(SWS_L_KillFltTitle)),
               Child, InfoText(SWS_L_KillFilterTxt),
               Child, DoMethod(idata->SWA_NGroup_KillFlt, SWM_NSFilter_UI),
            End,
         End,
         Child, ColGroup(2),
            Child, VGroup,
               GroupFrameT(GetS(SWS_L_DispFltTitle)),
               Child, InfoText(SWS_L_DispFilterTxt),
               Child, DoMethod(idata->SWA_NGroup_DispFlt, SWM_NDFilter_UI),
            End,
            Child, VGroup,
               GroupFrameT(GetS(SWS_L_ActFltTitle)),
               Child, InfoText(SWS_L_ActFilterTxt),
               Child, DoMethod(idata->SWA_NGroup_ActFlt,  SWM_NAFilter_UI),
            End,
         End,
         Child, HGroup,
            Child, RectangleObject, MUIA_HorizWeight, 20, End,
            Child, idata->AttrUI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o),
            Child, RectangleObject, MUIA_HorizWeight, 20, End,
         End,
      End,
   End;

   if (UseAVWin) {
      idata->ArtViewWin = WindowObject,
         MUIA_Window_Title,       idata->SWA_NGroup_Name,
         MUIA_Window_ID,          SWGetUL(o, SWA_ObjClassHash) + 2,
         MUIA_Window_IsSubWindow, TRUE,
         MUIA_Window_Menustrip,   MUI_MakeObject(MUIO_MenustripNM, MainMenu, 0),
         WindowContents,          ArtView,
      End;
   }
   
   if (!idata->ReaderWin) goto Done;

   idata->ArtListQuiet = 0;

   set(idata->ReaderWin, MUIA_Window_DefaultObject,
       SWGetObj(idata->ArtList, SWA_NList_LV));

   idata->Filter_GlobKill = NSFListObject,
      SWA_NList_Master,    NN_GlobKill(idata),
      SWA_NSFList_FltType, "#?NSFilter#?",
      SWA_NSFList_UIMode,  SWV_NSFList_UI_Tiny,
      SWA_NSFList_Group,   o,
      SWA_NSFList_Use,     SWV_NSFList_Use_Kill,
   End;

   // -- set up sort notifications -------------------------------------------

   SetSortNotify(o, idata);

   // -- set up pen/font controls --------------------------------------------
   
   SetUIToggle(idata->AttrUI, SWA_NGroup_ArtFont, SWA_NGroup_UseArtFont,
               MUIV_NotTriggerValue, !idata->SWA_NGroup_UseArtFont);
   
   SetUIToggle(idata->AttrUI, SWA_NGroup_ArtCol, SWA_NGroup_UseArtCol,
               MUIV_NotTriggerValue, !idata->SWA_NGroup_UseArtCol);

   DoMethod(o, MUIM_Notify, SWA_NGroup_UseArtFont, MUIV_EveryTime,
            idata->SWA_NGroup_ArtView, 3, MUIM_Set,
            SWA_NArticleView_UseArtFont, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NGroup_UseArtCol, MUIV_EveryTime,
            idata->SWA_NGroup_ArtView, 3, MUIM_Set,
            SWA_NArticleView_UseArtCol, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NGroup_ArtFont, MUIV_EveryTime,
            idata->SWA_NGroup_ArtView, 3, MUIM_Set,
            SWA_NArticleView_ArtFont, MUIV_TriggerValue);

   // -- set up button notifications -----------------------------------------

   DoMethod(qs, NOTP, o, 1, SWM_NGroup_ALQSWin);

   DoMethod(o, SWM_NGroup_OnOffLine, IsOnline(idata));

   DoMethod(NN_TCPList(idata), MUIM_Notify, SWA_NTCPList_Online, MUIV_EveryTime,
            o, 2, SWM_NGroup_OnOffLine, MUIV_TriggerValue);
   
   DoMethod(idata->B_Th1, NOTP, o, 4, SWM_NGroup_ChgThreadFlt,
            SWF_NGroup_ThreadFlt_All, TRUE, TRUE);

   DoMethod(idata->B_Th2, NOTP, o, 4, SWM_NGroup_ChgThreadFlt,
            SWF_NGroup_ThreadFlt_1Thread, TRUE, TRUE);

   DoMethod(idata->B_Th3, NOTP, o, 4, SWM_NGroup_ChgThreadFlt,
            SWF_NGroup_ThreadFlt_Base, TRUE, TRUE);

   DoMethod(b_bf, NOTP, o, 3, SWM_NGroup_Browse,  1, SWV_NGroup_Browse_Browse);
   DoMethod(b_br, NOTP, o, 3, SWM_NGroup_Browse, -1, SWV_NGroup_Browse_Browse);

   AddPopSrv(o, idata);

   set(idata->B_ProgStop, MUIA_Disabled, TRUE);

   DoMethod(SWGetObj(idata->ArtList, SWA_NList_LV), MUIM_Notify,
            MUIA_Listview_DoubleClick, TRUE, o, 1, SWM_NGroup_ArtListDClick);

   DoMethod(idata->B_Up, NOTP, o, 4, SWM_NGroup_ChgThreadFlt,
            SWF_NGroup_ThreadFlt_Up, TRUE, TRUE);

   DoMethod(idata->B_Dn, NOTP, o, 4, SWM_NGroup_ChgThreadFlt,
            SWF_NGroup_ThreadFlt_Dn, TRUE, TRUE);

   DoMethod(idata->B_Read, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, o, 3,
            SWM_NGroup_ChgReadFlt, SWV_NGroup_ReadFlt_Buttons, TRUE);

   DoMethod(b_pn,  NOTP, o, 1, SWM_NGroup_PostNew);
   DoMethod(b_pf,  NOTP, o, 1, SWM_NGroup_PostFup);
 
   DoMethod(b_af,  NOTP, o, 3, SWM_NGroup_Browse,  1, SWV_NGroup_Browse_ArtMov);
   DoMethod(b_ab,  NOTP, o, 3, SWM_NGroup_Browse, -1, SWV_NGroup_Browse_ArtMov);
   DoMethod(b_tf,  NOTP, o, 3, SWM_NGroup_Browse,  1, SWV_NGroup_Browse_ThreadMov);
   DoMethod(b_tb,  NOTP, o, 3, SWM_NGroup_Browse, -1, SWV_NGroup_Browse_ThreadMov);
   DoMethod(b_faq, NOTP, o, 1, SWM_NGroup_OpenFAQList);

   DoMethod(idata->B_Unrd, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, o, 3,
            SWM_NGroup_ChgReadFlt, SWV_NGroup_ReadFlt_Buttons, TRUE);

   DoMethod(idata->B_Auth, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, o, 3,
            SWM_NGroup_ChgAuthFlt, MUIV_TriggerValue, TRUE);

   DoMethod(o, MUIM_Notify, SWA_NGroup_Status, MUIV_EveryTime,
            idata->StatMsg, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);

   SetWinCtrls(o, idata, idata->ReaderWin);
   SetWinCtrls(o, idata, idata->ArtViewWin);

   // -- add window to app ---------------------------------------------------

   SetAttrs(o,
            SWA_NGroup_ReaderOpen, TRUE,
            SWA_NGroup_PermFlags,  (idata->SWA_NGroup_PermFlags |
                                    SWF_NGroup_WinOpen) &
                                    ~SWF_NGroup_NewBatch,
            TAG_DONE);

   DoMethod(idata->ArtList, MUIM_Notify,
            SWA_NArticleList_ClickAttr, MUIV_EveryTime,
            o, 2, SWM_NGroup_ArtListClick, MUIV_TriggerValue);

   DoMethod(o, SWM_NGroup_ChgThreadFlt, idata->SWA_NGroup_ThreadFT, FALSE, FALSE);
   DoMethod(o, SWM_NGroup_ChgReadFlt,   idata->SWA_NGroup_ReadFT,   FALSE, FALSE);

   rc = SUCCESS;
   
  Done:
   if (rc != SUCCESS) {
      if (!idata->ReaderWin) {
         MUI_DisposeObject(ArtView);
         MUI_DisposeObject(ArtList);
      }
      
      MUI_DisposeObject(idata->Filter_Read);
      MUI_DisposeObject(idata->Filter_Auth);
      MUI_DisposeObject(idata->Filter_ThBase);
      MUI_DisposeObject(idata->Filter_BaseMsgID);
      MUI_DisposeObject(idata->B_Th1);
      MUI_DisposeObject(idata->B_Th2);
      MUI_DisposeObject(idata->B_Th3);
      idata->Filter_Read      = NULL;
      idata->Filter_Auth      = NULL;
      idata->Filter_ThBase    = NULL;
      idata->Filter_BaseMsgID = NULL;
      idata->Filter_MsgID     = NULL;
      idata->B_Th1            = NULL;
      idata->B_Th2            = NULL;
      idata->B_Th3            = NULL;
   }

   SWDB_RET(rc);
}


/*****************************************************************************
 * Method: Check msg nums: count, old, new                                   *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_CheckStats, ULONG *)
{
   ULONG rc;
   UBYTE CmdBuff[SWV_NGroup_MaxNameLen+8];
   Object *Srv;
   
   if (!(Srv = CheckSendCmd(o, idata, msg, SWV_GC_CheckStats)))
      SWDB_RET(FAILURE);

   sprintf(CmdBuff, "GROUP %s\r\n", idata->SWA_NGroup_Name);

   AB_SetMod(idata);

   rc = DoMethod(Srv,
                 SWM_NServer_SendCmd,
                 SWT_NServer_Cmd,       CmdBuff,
                 SWT_NServer_CmdObj,    o,
                 SWT_NServer_CmdMethod, SWM_NGroup_CheckStats_P,
                 SWT_NServer_Title,     MakeTitle(o, idata, SWV_GC_CheckStats,
                                                  SWS_L_TCheckCnt),
                 SWT_NServer_LineLimit, 1,
                 TAG_DONE);

   SWDB_RET(DoMethod(o, SWM_Root_AdoptObjError, Srv, rc));
} SDFT

   
/*****************************************************************************
 * Method: Parse group statistics output                                     *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_CheckStats_P, SWP_NServer_StatusMsg *)
{
   UBYTE *n;
   LONG rc, an, fn, ln;

   MsgBegin(o, idata, msg, SWS_L_Status_CheckCount, SWV_GC_CheckStats);
   
   if (NSR_SRescode(msg)) {
      if ((rc = CheckResCode(o, idata, msg, SWV_NSR_Group))
          != SWV_NServer_RescodeOK)
         SWDB_RET(rc);

      n  = NSR_Line(msg,0) + strcspn(NSR_Line(msg,0), " ")+1;
      an = atol(n); n += strcspn(n, " ")+1;
      fn = atol(n); n += strcspn(n, " ")+1;
      ln = atol(n);

      SetAttrs(o,
               SWA_NGroup_AvailArtNum, an,
               SWA_NGroup_FirstArtNum, fn,
               SWA_NGroup_LastArtNum,  ln,
               SWA_NGroup_NewArtNum,
                   max(min(ln - (LONG)idata->SWA_NGroup_PrevArtNum, an), 0),
               TAG_DONE);

      SWDB_RET(rc);
   }

   MsgEnd(o, idata, msg, SWV_GC_CheckStats);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Read group description                                            *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_ReadDesc, ULONG *)
{
   ULONG rc;
   Object *Srv;
   UBYTE CmdBuff[SWV_NGroup_MaxNameLen*2+32];

   if (!(Srv = CheckSendCmd(o, idata, msg, SWV_GC_ReadDesc)))
      SWDB_RET(FAILURE);

   sprintf(CmdBuff,
           "LIST NEWSGROUPS %s\r\n"
           "LIST ACTIVE %s\r\n",
           idata->SWA_NGroup_Name,
           idata->SWA_NGroup_Name);

   rc = DoMethod(Srv,
                 SWM_NServer_SendCmd,
                 SWT_NServer_Cmd,       CmdBuff,
                 SWT_NServer_CmdObj,    o,
                 SWT_NServer_Title,     MakeTitle(o, idata, SWV_GC_ReadDesc,
                                                  SWS_L_TReadDesc),
                 SWT_NServer_CmdMethod, SWM_NGroup_ReadDesc_P,
                 SWT_NServer_DataCount, 2,
                 TAG_DONE);

   SWDB_RET(DoMethod(o, SWM_Root_AdoptObjError, Srv, rc));
} SDFT

   
/*****************************************************************************
 * Method: Parse group description output                                    *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_ReadDesc_P, SWP_NServer_StatusMsg *)
{
   STRPTR s = NULL;
    
   MsgBegin(o, idata, msg, SWS_L_Status_ReadDesc, SWV_GC_ReadDesc);

   if (NSR_SRescode(msg))
      SWDB_RET(CheckResCode(o, idata, msg, SWV_NSR_List));
   
   if (NSR_SNext(msg)) {
      if (NSR_Last(msg) >= 0) {
         s = NSR_Line(msg, 0);
         if (s=strchr(s, '\t'))
            s++;
      }

      set(o, SWA_NGroup_Desc, SW_STRDEF(s, ""));
   }
      
   if (NSR_SDone(msg)) {
      if (NSR_Last(msg) >= 0)
         if ((s = NSR_Line(msg, 0)) && *s) {
            UBYTE c = s[strlen(s)-1];
            set(o, SWA_NGroup_Moderated, c != 'y' && c != 'Y');
         }
   }

   MsgEnd(o, idata, msg, SWV_GC_ReadDesc);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Fetch article headers from group                                  *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_Fetch, ULONG *)
{
   ULONG rc,
         HdrDL = NN_HdrDL(idata);
   Object *Srv, *am;
   struct TagItem *ti = (struct TagItem *)(msg+1);
   UBYTE CmdBuff[SWV_NGroup_MaxNameLen+12];

   if (idata->SrvInfo[SWV_GC_FetchHdr].Srv) SWDB_RET(-1); // we`ve already got one

   FreeFetchData(idata);

   am                = GetTagOB(SWT_NGroup_ArtMgr,        NULL, ti);
   if (am) idata->SWA_NGroup_ArtMgr = am;
   else am = idata->SWA_NGroup_ArtMgr;

   idata->FetchFirst    = GetTagUL(SWT_NGroup_FetchFirst,       1, ti);
   idata->FetchLast     = GetTagUL(SWT_NGroup_FetchLast, LONG_MAX, ti);
   idata->FetchObj      = GetTagOB(SWT_NGroup_FetchObj,         o, ti);
   idata->FetchMthd     = GetTagUL(SWT_NGroup_FetchMthd,
                                SWM_NGroup_FetchAdd,            ti);
   idata->FetchQObj     = GetTagOB(SWT_NGroup_FetchQObj,       am, ti);
   idata->FetchQMthd    = GetTagUL(SWT_NGroup_FetchQMthd,
                                SWM_NArticleMgr_Find,           ti);
   idata->FetchCnt      = GetTagUL(SWT_NGroup_FetchStCnt, NN_HdrInc(idata), ti);

   idata->FetchPrevCnt  = 
   idata->FetchCnt      = min(idata->FetchCnt, SWV_NGroup_FetchMax);
   idata->FetchCmdCnt   = 0;
   idata->SimulCmds     = NN_SimulCmds(idata);
   idata->SimulCmds     = max(idata->SimulCmds, 1);
   idata->FetchBytesCnt = 0;

   switch (HdrDL) {
     case SWV_NNews_HD_Bytes:
      idata->FetchCmdMax = 2,
      idata->FetchFlags  = SWF_NGroup_FetchBytes;
      break;
     case SWV_NNews_HD_Normal:
      idata->FetchCmdMax = 1,
      idata->FetchFlags  = 0;
      break;
     case SWV_NNews_HD_XOver:
      idata->FetchFlags  = SWF_NGroup_FetchXOver;
      break;
   }
   
   if (!idata->SWA_NGroup_ArtMgr) SWDB_RET(FAILURE);

   OpenDBFiles(o, idata);

   if (idata->DBOut && FIsSet(idata->DBType, SWV_DB_Fetch))
      FPrintf(idata->DBOut, "%s: [Start Fetch]\n", __FUNC__);

   if (!(Srv = CheckSendCmd(o, idata, msg, SWV_GC_FetchHdr)))
      rc = FAILURE;
   else {
      sprintf(CmdBuff, "LISTGROUP %s\r\n", idata->SWA_NGroup_Name);

      rc = DoMethod(Srv,
                    SWM_NServer_SendCmd,
                    SWT_NServer_Cmd,       CmdBuff,
                    SWT_NServer_CmdObj,    o,
                    SWT_NServer_Title,     MakeTitle(o, idata, SWV_GC_FetchHdr,
                                                     SWS_L_THeaders),
                    SWT_NServer_CmdMethod, SWM_NGroup_Fetch_P1,
                    SWT_NServer_Flags,     SWF_NServer_NumMode |
                                           SWF_NServer_ExplicitDone,
                    TAG_DONE);
      DoMethod(o, SWM_Root_AdoptObjError, Srv, 0);
   }
   
   if (idata->ReaderWin)
      DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);

   SWDB_RET(rc);
} SDFT

   
/*****************************************************************************
 * Method: Fetch Articles (internal)                                         *
 *****************************************************************************/
   
METHOD_NOINLINE(SWM_NGroup_Fetch_P1, SWP_NServer_StatusMsg *)
{
   Object *Srv;
   
   MsgBegin(o, idata, msg, SWS_L_Status_Headers, SWV_GC_FetchHdr);
   
   if (NSR_SRescode(msg))
      SWDB_RET(CheckResCode(o, idata, msg, SWV_NSR_Group));

   if (NSR_SBegin(msg)) {
      SetProgMsg(o, idata, SWV_GC_FetchHdr, SWS_T_PM_Hdrs, 0, 0);
   }
   
   if (NSR_SNext(msg)) {
      if (NSR_Last(msg) >= 0) {
         ULONG x, FPolicy = NN_FetchPolicy(idata), NumNew;

         if (!(idata->FetchNums = (ULONG *)AV(NSR_Size(msg))))
            SWDB_RET(0);

         idata->FetchNumCnt = NSR_Count(msg);
         memcpy(idata->FetchNums, NSR_Lines(msg), NSR_Size(msg));

         for (NumNew = 0;
              NumNew < idata->FetchNumCnt &&
                 idata->FetchNums[NumNew] < idata->SWA_NGroup_LastRdMsgE;
              NumNew++);

         NumNew = idata->FetchNumCnt - NumNew;
         
         idata->MaxArticles =
            FPolicy == SWV_NNews_FP_Limit ? idata->SWA_NGroup_MaxArticles :
            FPolicy == SWV_NNews_FP_All   ? LONG_MAX :
               idata->SWA_NGroup_MaxArticles;

         // -- prompt for what to do if needed -------------------------------
         
         if (FPolicy == SWV_NNews_FP_Prompt   &&
             NumNew > idata->MaxArticles &&
             !(idata->SrvInfo[SWV_GC_GL_Hdrs].Srv ||
               idata->SrvInfo[SWV_GC_GL_Arts].Srv)) {
            ULONG rc;
            TEXT  bt[80];

            sprintf(bt, GetS(SWS_L_AskFetchBt),
                    idata->FetchNumCnt, idata->MaxArticles);

            rc = MUI_Request(NN_App(idata), idata->ReaderWin, 0,
                             idata->SWA_NGroup_Name,
                             bt, GetS(SWS_L_AskFetch),
                             idata->FetchNumCnt, idata->MaxArticles,
                             idata->FetchNumCnt, idata->MaxArticles);

            if (rc == 1) idata->MaxArticles = LONG_MAX;

            if (rc == 0) {
               DoMethod(idata->SrvInfo[SWV_GC_FetchHdr].Srv,
                        SWM_NServer_Done,
                        idata->SrvInfo[SWV_GC_FetchHdr].ID);

               MUI_Request(NN_App(idata), idata->ReaderWin, 0,
                           idata->SWA_NGroup_Name,
                           GetS(SWS_L_FetchCancOk),
                           GetS(SWS_L_FetchCanc));

               SWDB_RET(0);
            }
         }


         // -- work out where to start, etc ----------------------------------
         
         idata->FetchStart  =
         idata->FetchNumPos =
            idata->FetchNumCnt > idata->MaxArticles ?
               idata->FetchNumCnt - idata->MaxArticles : 0;

         // -- skip ones we`ve read before. ----------------------------------
         
         if (!SW_STRISNULL(idata->SWA_NGroup_LastRdSrv) &&
             idata->FetchNumCnt > 1                     &&
             idata->SWA_NGroup_LastRdMsgE <=
                 idata->FetchNums[idata->FetchNumCnt-1] &&
             !strcmp(idata->SWA_NGroup_LastRdSrv,
                     SWGetStr(msg->NServer, SWA_NServer_Host))) {

            if (idata->MaxArticles <= idata->SWA_NGroup_LastRdMsgC)
               while (idata->FetchStart < idata->FetchNumCnt &&
                      idata->FetchNums[idata->FetchStart] <
                      idata->SWA_NGroup_LastRdMsgE)
                  idata->FetchStart++;

            idata->FetchNumPos = idata->FetchStart;
         }

         if (idata->FetchNumCnt)
            idata->SWA_NGroup_LastArtNum =
               idata->FetchNums[idata->FetchNumCnt-1];

         SetProgMsg(o, idata, SWV_GC_FetchHdr, SWS_T_PM_Hdrs, 0,
                    idata->FetchNumCnt - idata->FetchStart);

         // -- start the fetch operation -------------------------------------
         
         if (!FIsSet(idata->FetchFlags, SWF_NGroup_FetchXOver)) {
            for (x=idata->FetchStart;
                 x<idata->FetchStart+idata->SimulCmds; x++)
               FetchHeaders(o, idata, x);
         } else {
            if (DoMethod(Srv = idata->SrvInfo[SWV_GC_FetchHdr].Srv,
                         SWM_NServer_SendCmd,
                         SWT_NServer_ID,        idata->SrvInfo[SWV_GC_FetchHdr].ID,
                         SWT_NServer_Cmd,       "LIST OVERVIEW.FMT\r\n",
                         SWT_NServer_CmdObj,    o,
                         SWT_NServer_CmdMethod, SWM_NGroup_Fetch_XOV_P1,
                         SWT_NServer_Flags,     SWF_NServer_ExplicitDone,
                         TAG_DONE) != SUCCESS) {
               DoMethod(o, SWM_Root_AdoptObjError, Srv, 0);

               if (idata->ReaderWin)
                  DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);

               DoMethod(idata->SrvInfo[SWV_GC_FetchHdr].Srv,
                        SWM_NServer_Done,
                        idata->SrvInfo[SWV_GC_FetchHdr].ID);
            }
         }
      } else {
         DoMethod(idata->SrvInfo[SWV_GC_FetchHdr].Srv,
                  SWM_NServer_Done,
                  idata->SrvInfo[SWV_GC_FetchHdr].ID);
      }
   }

   MsgEnd(o, idata, msg, SWV_GC_FetchHdr);

   if (NSR_SAnyEnd(msg))
      FreeFetchData(idata);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Fetch Articles (internal)                                         *
 *****************************************************************************/
   
METHOD_NOINLINE(SWM_NGroup_Fetch_XOV_P1, SWP_NServer_StatusMsg *)
{
   if (NSR_SRescode(msg))
      SWDB_RET(CheckResCode(o, idata, msg, SWV_NSR_List));
   
   if (NSR_SNext(msg)) {
      idata->FetchOVCount = NSR_Count(msg);
      
      if (!(idata->FetchOverview = (STRPTR *)
            AV(sizeof(STRPTR) * (idata->FetchOVCount + 2)))) {
         DoMethod(idata->SrvInfo[SWV_GC_FetchHdr].Srv,
                  SWM_NServer_Done,
                  idata->SrvInfo[SWV_GC_FetchHdr].ID);
         SWDB_RET(0);
      }

      NSR_Adopt(msg, NSR_First(msg), NSR_Last(msg), idata->FetchOverview, FALSE);

      idata->FetchOverview[idata->FetchOVCount] = NULL;

      FetchHeaders(o, idata, idata->FetchStart);
   }

   MsgEnd(o, idata, msg, SWV_GC_FetchHdr);

   if (NSR_SAnyEnd(msg))
      FreeFetchData(idata);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: inc fetch pos                                                   *
 *****************************************************************************/

static void IncFetch(Object *o, SW_IDATA *idata)
{
   if (++idata->FetchArtCnt   >= idata->FetchCnt ||
       (idata->FetchNumPos+1) >= idata->FetchNumCnt) {
      idata->FetchArtCnt  = 0;
      
      DoMethod(idata->FetchObj, idata->FetchMthd,
               idata->FetchLines, idata->FetchLineCnt, o);
      
      ClearFetchLines(idata);

      DoMethod(o, SWM_NGroup_ChgSelFlt, 0);

      if (FIsSet(idata->FetchFlags, SWF_NGroup_FetchXOver))
         FetchPctComplete(o, idata, idata->FetchNumPos);
   }

   FetchHeaders(o, idata, idata->SimulCmds + idata->FetchNumPos++);

   if (idata->FetchNumPos >= idata->FetchNumCnt)
      DoMethod(idata->SrvInfo[SWV_GC_FetchHdr].Srv,
               SWM_NServer_Done,
               idata->SrvInfo[SWV_GC_FetchHdr].ID);

   if (!FIsSet(idata->FetchFlags, SWF_NGroup_FetchXOver))
      FetchPctComplete(o, idata, idata->FetchNumPos);
}


/*****************************************************************************
 * Function: allocate more space in line array                               *
 *****************************************************************************/

static void MakeFetchSpace(SW_IDATA *idata, ULONG NewLines)
{
   if (idata->FetchLineCnt + NewLines + 1 < idata->FetchLineMax)
      return;
   
   idata->FetchLineMax += NewLines + 100;
   RV(sizeof(STRPTR) * idata->FetchLineMax, &idata->FetchLines);
}


/*****************************************************************************
 * Function: Set done attrs                                                  *
 *****************************************************************************/

static void SetDone(Object *o, SW_IDATA *idata, SWP_NServer_StatusMsg *msg)
{
   SetAttrs(o,
            SWA_NGroup_LastRdMsgC, idata->MaxArticles,
            SWA_NGroup_LastRdMsgE, idata->FetchNums[idata->FetchNumCnt-1],
            SWA_NGroup_LastRdSrv,  SWGetStr(msg->NServer, SWA_NServer_Host),
            TAG_DONE);
}

/*****************************************************************************
 * Method: Parse incoming article lines                                      *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_Fetch_XOV_P2, SWP_NServer_StatusMsg *)
{
   LONG  x, f, full, hasgroups;
   STRPTR fp0, fp1, fp2, xref, line;
   UBYTE  c;
   
   if (NSR_SRescode(msg))
      SWDB_RET(CheckResCode(o, idata, msg, SWV_NSR_XOverData));

   if (NSR_SNext(msg) || NSR_SRun(msg)) {
      
      for (f=0, hasgroups=FALSE; f<idata->FetchOVCount; f++)
         if (SWP_HasHeader(idata->FetchOverview[f], SWF_NAHeader_Groups))
            hasgroups = TRUE;
            
      MakeFetchSpace(idata, NSR_Count(msg) * (idata->FetchOVCount + 2));

      for (x=NSR_First(msg); x<=NSR_Last(msg); x++) {
         if (!(fp0 = strchr(NSR_Line(msg, x), '\t')))
            SWDB_RET(0);

         for (f=0, fp0++, xref = NULL; f<idata->FetchOVCount; f++) {
            if (!(fp1 = strchr(fp0, '\t')))
               fp1 = fp0 + strlen(fp0);

            c = *fp1, *fp1 = '\0';

            line = idata->FetchLines[idata->FetchLineCnt] =
               AV((fp1 - fp0) + strlen(idata->FetchOverview[f]) + 4);

            fp2 = strchr(strcpy(line, idata->FetchOverview[f]), ':');

            if (fp2) {
               full = (fp2[1] == 'f' || fp2[1] == 'F');
               *(++fp2) = ' ',
               *(++fp2) = '\0';

               strcpy(full ? line : fp2, fp0);

               if (SWP_HasHeader(line, SWF_NAHeader_Xref))
                  xref = line + sizeof(SWF_NAHeader_Xref);
            }

            *fp1 = c;
            fp0  = fp1 + (c ? 1 : 0);

            idata->FetchLineCnt++;
         }
         
         // -- make imitation Newsgroups: header from Xref if none exists ----
            
         if (!hasgroups) {
            if (xref) {
               if (fp0 = strchr(xref, ' ')) {
                  fp0++;

                  if (!(fp1 = strchr(fp0, '\t')))
                     fp1 = fp0 + strlen(fp0);

                  line = idata->FetchLines[idata->FetchLineCnt++] =
                     AV(sizeof(SWF_NAHeader_Groups) + (fp1 - fp0) + 16);

                  strcat(strcpy(line, SWF_NAHeader_Groups), " ");
                  line += strlen(line);
                  
                  for (; *fp0 && *fp0 != '\t';)
                     if (*fp0 != ':')
                        *line++ = *fp0++;
                     else {
                        while (*fp0 && *fp0 != '\t' && *fp0 != ' ')
                           fp0++;

                        if (*fp0 == ' ') *line++ = ',', fp0++;
                     }

                  *line = '\0';
               }
            } else {
               line = idata->FetchLines[idata->FetchLineCnt++] =
                  AV(sizeof(SWF_NAHeader_Groups) + strlen(idata->SWA_NGroup_Name) + 4);

               strcat(strcat(strcpy(line, SWF_NAHeader_Groups), " "),
                      idata->SWA_NGroup_Name);
            }
         }

         idata->FetchLines[idata->FetchLineCnt++] = NULL;
         IncFetch(o, idata);
      }

      DoMethod(NSR_Srv(msg), SWM_NServer_DelLines,
               NSR_Lines(msg),
               NSR_First(msg),
               NSR_Last(msg));
   }

   MsgEnd(o, idata, msg, SWV_GC_FetchHdr);

   if (NSR_SDone(msg))
      SetDone(o, idata, msg);

   if (NSR_SAnyEnd(msg)) FreeFetchData(idata);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: track fetch command position                                    *
 *****************************************************************************/

void IncFetchCmdPos(SW_IDATA *idata)
{
   if (++idata->FetchCmdCnt >= idata->FetchCmdMax)
      idata->FetchCmdCnt = 0;
}


/*****************************************************************************
 * Method: Parse incoming article lines                                      *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_Fetch_P2, SWP_NServer_StatusMsg *)
{
   if (NSR_SRescode(msg)) {
      if (idata->FetchCmdCnt == (idata->FetchCmdMax-1))
         if (NSR_ResCode(msg) != SWV_NSR_Head) {
            IncFetch(o, idata);
            IncFetchCmdPos(idata);
            SWDB_RET(SWV_NServer_RescodeMore);
         }
         
      SWDB_RET(SWV_NServer_RescodeOK);
   }

   // -- we got another hunk of data -----------------------------------------

   if (NSR_SNext(msg)) {
      MakeFetchSpace(idata, NSR_Count(msg));

      // -- this was the XHDR Bytes output -----------------------------------
      
      if (idata->FetchCmdMax == 2 && idata->FetchCmdCnt == 0) {
         idata->FetchBytesCnt = 0;

         if (NSR_Count(msg) >= 1 && strchr(NSR_Line(msg, 0), ' '))
            idata->FetchBytesCnt = atol(strchr(NSR_Line(msg, 0), ' '));
         
         if (idata->DBOut && FIsSet(idata->DBType, SWV_DB_Fetch))
            FPrintf(idata->DBOut, "%s: XHDR Bytes: %ld\n", __FUNC__,
                    idata->FetchBytesCnt);
      }

      // -- this was the HEAD output -----------------------------------------

      if (idata->FetchCmdCnt == (idata->FetchCmdMax - 1)) {
         if (idata->DBOut && FIsSet(idata->DBType, SWV_DB_Fetch))
            FPrintf(idata->DBOut, "%s: Got HEAD results: %ld lines\n",
                    __FUNC__, NSR_Count(msg));

         idata->FetchLineCnt += 
            NSR_Adopt(msg, NSR_First(msg), NSR_Last(msg),
                      idata->FetchLines + idata->FetchLineCnt, TRUE);

         // -- Fake a Bytes: header from the XHDR Bytes result ---------------
         
         if (idata->FetchBytesCnt)
            if (idata->FetchLines[idata->FetchLineCnt] = AV(20))
               sprintf(idata->FetchLines[idata->FetchLineCnt++],
                       SWF_NAHeader_Bytes " %ld", idata->FetchBytesCnt);

         idata->FetchLines[idata->FetchLineCnt++] = NULL;

         IncFetch(o, idata);
      }

      IncFetchCmdPos(idata);
   }

   MsgEnd(o, idata, msg, SWV_GC_FetchHdr);

   if (NSR_SDone(msg))
      SetDone(o, idata, msg);
   
   if (NSR_SAnyEnd(msg)) FreeFetchData(idata);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Abort receive if pending                                          *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_Abort, SWP_NGroup_Abort *)
{
   ULONG Cmd = msg->Cmd;

   if (Cmd == SWV_GC_BatchDL) {
      if (idata->DLPending)
         DoMethod(idata->DLPending, SWM_NArticle_Abort);
      
      DLQueueClear(o, idata);
   }

   if (!idata->SrvInfo[Cmd].Srv) SWDB_RET(0);

   SWDB_RET(DoMethod(idata->SrvInfo[Cmd].Srv,
                     SWM_NServer_Abort,
                     idata->SrvInfo[Cmd].ID));
} SDFT

   
/*****************************************************************************
 * Function: Find message ID position in header array                        *
 *****************************************************************************/

# define _IsHdr(x,Hdr,l) (Strnicmp((x), (Hdr), (l)) == 0)

static STRPTR HdrPos(UBYTE **Array, STRPTR Hdr)
{
   LONG x, l = strlen(Hdr);
   
   for (x=0; Array[x]; x++)
      if (_IsHdr(Array[x], Hdr, l))
         return Array[x] + l + 1;

   return NULL;
}


/*****************************************************************************
 * Method: Add new articles from fetched data                                *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_FetchAdd, SWP_NGroup_Fetch *)
{
   Object   *art, *MA = NN_ArtTpl(idata);
   LONG      x, y, PrevPos, ArtCnt = 0;
   UBYTE    *ID;
   Object  **Articles;

   // -- count number of articles we got -------------------------------------
   
   for (x=y=0; x<GF_DataCount(msg); x++)
      if (!GF_Data(msg, x)) y++;

   if (!(Articles = (Object **)AV(ASize(y)))) goto Done;
   
   for (x=PrevPos=0; x<GF_DataCount(msg); x++) {
      if (!GF_Data(msg, x)) {
         ID = HdrPos(msg->Data + PrevPos, SWF_NAHeader_MsgID);

         if (!SW_STRISNULL(ID))
            if (!DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_Find, ID)) {
               art = NewObject(OCLASS(MA), NULL,
                               SWA_NArticle_MemPool, idata->SWA_NGroup_MemPool,
                               TAG_DONE);
   
               if (art) {
                  DoMethod(art, SWM_NArticle_AddLines,
                           SWT_NArticle_AddLines,      msg->Data + PrevPos,
                           SWT_NArticle_AddLineCount,  x - PrevPos,
                           SWT_NArticle_AddAlloc,      FALSE,
                           SWT_NArticle_AddFixISOHdrs, TRUE,
                           SWT_NArticle_AddFixHdrCase, TRUE,
                           TAG_DONE);
   
                  for (y=PrevPos; y<x; y++)
                     GF_Data(msg, y) = NULL;
     
                  DoMethod(art, SWM_NArticle_FlagSet,
                           SWF_NArticle_OView |
                           (idata->SWA_NGroup_NewArtMd == SWV_NGroup_NA_DontUse
                            ? 0 : SWF_NArticle_NewArt));
     
                  Articles[ArtCnt++] = art;
               }
            }

         PrevPos = x + 1;
      }
   }

   DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_Add,
            SWT_NArticleMgr_ArticleArray, Articles,
            SWT_NArticleMgr_ArticleCount, ArtCnt,
            TAG_DONE);

  Done:
   FV(Articles);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Add new articles to our list of displayed ones                    *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NArticleMgr_NewArticles, ULONG *)
{
   ULONG  First;
   ULONG  ArtCount   = GetTagUL(SWT_NArticleMgr_ArticleCount, 0, msg+1),
          LastSet    = GetTagUL(SWT_NArticleMgr_LastSet,  FALSE, msg+1);
   Object **ArtArray = GetTagTyp(SWT_NArticleMgr_ArticleArray, NULL, msg+1,
                                 Object **);

   if (ArtCount) {
      KillFltArray(o, idata, ArtArray, ArtCount);
      
      First = idata->GrpArticleCnt;

      // -- add to our list --------------------------------------------------
      idata->GrpArticleCnt = DoMethod(idata->SWA_NGroup_ArtMgr,
                                      SWM_NArticleMgr_FilterList,
                                      ArtArray, ArtCount,
                                      &idata->GrpArticles, &idata->GrpArticleMax,
                                      idata->GrpArticleCnt, FALSE, 1, FALSE, o,
                                      idata->SWA_NGroup_GrpDefFlt, NULL);

      if (idata->GrpArticleCnt != First) {
         ThrdMgrAdd(o, idata,
                    idata->GrpArticles   + First,
                    idata->GrpArticleCnt - First);

         ChgHLFlt(o, idata,
                  idata->GrpArticles   + (LastSet ? 0 : First),
                  idata->GrpArticleCnt - (LastSet ? 0 : First));

         DoMethod(o, SWM_NGroup_ChgSelFlt, LastSet ? 0 : First);
      }
   }

   SWDB_RET(SUCCESS);
} SDFT

   
/*****************************************************************************
 * Method: Save overview file for this group                                 *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_SaveOV, ULONG *)
{
   ULONG  rc, ok;
   UBYTE  Group[256];
   STRPTR Path   = OVDir(o, idata, NULL);

   if (FIsSet(idata->SWA_NGroup_Flags,SWF_NGroup_TempGroup)) SWDB_RET(SUCCESS);
   
   if (!idata->SWA_NGroup_ArtMgr || SW_STRISNULL(Path)) SWDB_RET(FAILURE);

   if (!SWDirExists(Path) && !idata->GrpArticleCnt)
      SWDB_RET(SUCCESS);

   strncat(strcpy(Group, "Group: "), idata->SWA_NGroup_Name, 250);
   Group[250] = '\0';
   strcat(Group, "\n");

   StartGauge(o, idata, idata->SWA_NGroup_ArtMgr, SWA_NArticleMgr_OVPct,
              GetS(SWS_L_Status_SaveOV), SWV_GC_Private1, FALSE);

   ok = SWGetUL(idata->SWA_NGroup_GrpDefFlt, SWA_NSF_InGroup_Killed);
   set(idata->SWA_NGroup_GrpDefFlt, SWA_NSF_InGroup_Killed, TRUE);

   rc = DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_SaveOverview,
                 Path, Group, 1, FALSE, o, idata->SWA_NGroup_GrpDefFlt, NULL);

   set(idata->SWA_NGroup_GrpDefFlt, SWA_NSF_InGroup_Killed, ok);

   if (CacheMk(o, idata, SWV_NGroup_RemChMk) == FAILURE)
      rc = FAILURE;

   FSet(idata->SWA_NGroup_Flags, SWF_NGroup_OVDir);
   
   StopGauge(o, idata, SWV_GC_Private1);

   SWDB_RET(DoMethod(o, SWM_Root_AdoptObjError, idata->SWA_NGroup_ArtMgr, rc));
} SDFT

   
/*****************************************************************************
 * Method: Load overview file for this group                                 *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_LoadOV, ULONG *)
{
   ULONG rc;

   if (!idata->SWA_NGroup_ArtMgr) SWDB_RET(FAILURE);

   idata->GrpArticleCnt = DoMethod(idata->SWA_NGroup_ArtMgr,
                                SWM_NArticleMgr_FilterList,
                                SWV_NArticleMgr_Filter_UseTbl, 0,
                                &idata->GrpArticles, &idata->GrpArticleMax,
                                idata->GrpArticleCnt, FALSE, 1, FALSE, o,
                                idata->SWA_NGroup_GrpDefFlt, NULL);

   ChgHLFlt(o, idata, idata->GrpArticles, idata->GrpArticleCnt);

   ThrdMgrAdd(o, idata, idata->GrpArticles, idata->GrpArticleCnt);
   DoMethod(o, SWM_NGroup_ChgSelFlt, 0);

   StartGauge(o, idata, idata->SWA_NGroup_ArtMgr, SWA_NArticleMgr_OVPct,
              GetS(SWS_L_Status_LoadOV), SWV_GC_Private1, FALSE);
   
   rc = DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_LoadOverview,
                 OVDir(o, idata, NULL),
                 idata->SWA_NGroup_ExpireDays*SWV_NArticle_AgeTicksPerMin*60*24,
                 1, FALSE, o, idata->SWA_NGroup_GrpDefFlt, NULL);

   if (rc == SUCCESS) FSet(idata->SWA_NGroup_Flags, SWF_NGroup_OVDir);

   CountRead(o, idata);

   StopGauge(o, idata, SWV_GC_Private1);

   SWDB_RET(DoMethod(o, SWM_Root_AdoptObjError, idata->SWA_NGroup_ArtMgr, rc));
} SDFT

   
/*****************************************************************************
 * Method: Go fetch next one from DLQueue                                    *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_DLFetchNext, Msg)
{
   Object *Art = NULL, *DSrv;
   ULONG rc;

   if (!idata->DLStatMsg)
      if (!(idata->DLStatMsg = AV(strlen(GetS(SWS_L_Status_DLArt)) + 16))) {
         DLQueueClear(o, idata);
         return FAILURE;
      }

   if (idata->DLPending) return SUCCESS;  // one going already;

   while (!Art && idata->DLCnt) {
      idata->DLPending = Art = idata->DLQueue[--idata->DLCnt];
      idata->DLPendCmd = (LONG)idata->DLQueueCmd[idata->DLCnt];

      // -- don`t download killed articles -----------------------------------
      if (DoMethod(Art, SWM_NArticle_FlagTest, SWF_NArticle_Killed))
         Art = NULL;
   }

   if (!idata->DLCnt && !Art) {
      DLQueueClear(o, idata);

      return SUCCESS;
   }

   sprintf(idata->DLStatMsg, GetS(SWS_L_Status_DLArt),
           idata->DLNum - idata->DLCnt, idata->DLNum,
           (idata->DLNum - idata->DLCnt) * 100 / max(idata->DLNum, 1));

   UpdGaugeStat(o, idata, idata->DLStatMsg, SWV_GC_BatchDL);

   set(o, SWA_NGroup_DLQPctDone,
       (idata->DLNum - idata->DLCnt) * 100 / max(idata->DLNum, 1));
   
   StartGauge(o, idata, Art, SWA_NArticle_PctDL, idata->DLStatMsg,
              SWV_GC_BatchDL, TRUE);

   DSrv = (Object *)DoMethod(idata->SWA_NGroup_SrvList,
                             SWM_NServerList_GetDefault,
                             SWT_NServerList_ServerName,
                                   idata->SWA_NGroup_Server1,
                             TAG_DONE);

   if (!DSrv)
      rc = DoMethod(o, SWM_Root_AdoptObjError, idata->SWA_NGroup_SrvList,
                    FAILURE);
   else {
      rc = DoMethod(Art, SWM_NArticle_Fetch,
                    SWT_NArticle_Server,        DSrv,
                    SWT_NArticle_Parts,         idata->SWA_NGroup_FullHeaders ?
                                                   SWF_NArticle_HdBd :
                                                   SWF_NArticle_Body,
                    SWT_NArticle_FetchTitle,    MakeTitle(o, idata, SWV_GC_Private1,
                                                          SWS_L_TArtFetch),
                    SWT_NArticle_FetchObj,      o,
                    SWT_NArticle_CacheDir,      OVDir(o, idata, NULL),
                    SWT_NArticle_IncLines,      idata->SWA_NGroup_IncrDisp ? 100 : 0,
                    TAG_DONE);
      DoMethod(o, SWM_Root_AdoptObjError, DSrv, 0);
   }

   if (rc == FAILURE) DLQueueClear(o, idata);

   return rc;
} SDFT

   
/*****************************************************************************
 * Method: Process downloaded articles                                       *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NArticle_StatMsg, SWP_NArticle_StatMsg *)
{
   ULONG Inst = idata->DLInstRead,
         Cmd =  Inst ? SWV_NGroup_DLQ_Read : idata->DLPendCmd;

   if (!msg->Article) SWDB_RET(0);

   if (msg->Status == SWV_NArticle_Stat_Begin) {
      if (idata->SWA_NGroup_ArtView && Cmd == SWV_NGroup_DLQ_Read) {
         set(idata->SWA_NGroup_ArtView, SWA_NArticleView_TitleTxt,
             SW_STRDEF(idata->SWA_NGroup_ArtViewTitle,
                       SWGetStr(idata->SWA_AppBase_TopObj,
                                SWA_NNews_ArtViewTitle)));
         
         DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_Begin,
                  msg->Article, o);
      }
      
      SWDB_RET(0);
   }
   
   if (SWV_NA_IsNewLines(msg) && Cmd == SWV_NGroup_DLQ_Read) {
      if (!idata->SWA_NGroup_ArtView) SWDB_RET(0);
      
      DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_NewLines,
               msg->Article, o, msg->Lines, msg->FirstNew, msg->LastNew,
               msg->LastHdr, SWV_NA_IsLast(msg));
   }
   
   if (msg->Status == SWV_NArticle_Stat_End) {
      if (msg->Loc == SWV_NArticle_Stat_LocNet && Cmd != SWV_NGroup_DLQ_Read)
         CacheMk(o, idata, SWV_NGroup_MakeChMk);

      switch (Cmd) {
        case SWV_NGroup_DLQ_MakePerm:
           DoMethod(msg->Article, SWM_NArticle_SetPerm,
                    SWT_NArticle_Perm,     SWF_NArticle_PermYes,
                    SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
                    TAG_DONE);
           break;

        case SWV_NGroup_DLQ_Cache:
           DoMethod(msg->Article, SWM_NArticle_Cache,
                    SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
                    TAG_DONE);
           break;

        case SWV_NGroup_DLQ_Read:
           if (idata->SWA_NGroup_ReadnCache)
              DoMethod(msg->Article, SWM_NArticle_Cache,
                       SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
                       TAG_DONE);

           DoMethod(o, SWM_NGroup_MarkRead, msg->Article);

           break;

        case SWV_NGroup_DLQ_Action:
           DoMethod(idata->SWA_NGroup_ActFlt, SWM_NSFList_DoSelected,
                    SWM_NAFilter_Action, o, msg->Article);
           
           DoMethod(idata->Filter_GlobAct, SWM_NSFList_DoSelected,
                    SWM_NAFilter_Action, o, msg->Article);

           break;

        case SWV_NGroup_DLQ_PreFetch:
           break;
      }

      MemQueueAdd(idata, msg->Article);
   }

   if (SWV_NA_IsEnd(msg)) {
      if (!Inst)
         StopGauge(o, idata, SWV_GC_BatchDL);

      idata->DLInstRead = FALSE;

      MemQueueDel(idata, FALSE,
                  SWGetObj(idata->SWA_NGroup_ArtView, SWA_NArticleView_DspArt));
   
      if (msg->Status == SWV_NArticle_Stat_End ||
          msg->Status == SWV_NArticle_Stat_NotFound) {

         if (!Inst) {
            idata->DLPending = NULL;
            DoMethod(NN_App(idata), MUIM_Application_PushMethod,
                     o, 1, SWM_NGroup_DLFetchNext);
         }
         
         if (idata->SWA_NGroup_ArtView && Cmd == SWV_NGroup_DLQ_Read) {
            DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_End);
            ArtPrefetch(o, idata);
         }
      } else {
         DLQueueClear(o, idata);

         if (idata->SWA_NGroup_ArtView && Cmd == SWV_NGroup_DLQ_Read)
            DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_Abort);
      }

      DoMethod(o, SWM_NGroup_UpdArtList, msg->Article);
   }

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Function: Menu save fns                                                   *
 *****************************************************************************/

static void MenuSave(Object *o, SW_IDATA *idata, ULONG Parts, STRPTR Dest, ULONG Attr)
{
   DoMethod(idata->App, MUIM_Application_PushMethod, o, 4,
            SWM_NGroup_SaveArt, Parts, Dest, Attr);
}

   
/*****************************************************************************
 * Method: Process menu events                                               *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_MenuCall, SWP_NGroup_MenuCall *)
{
   Object *App = _app(idata->ReaderWin);
   ULONG  Parts = 0;
   STRPTR Hdr   = NULL;
   Object *Art  = SWGetObj(idata->SWA_NGroup_ArtView, SWA_NArticleView_DspArt);
   
   DoMethod(o, SWM_ClearObjError);

   switch (msg->Call) {
     case SWS_M_P_Save:      DoMethod(o, SWM_NGroup_SaveOV, TAG_DONE);
                             break;
     
     case SWS_M_P_Close:     DoMethod(App, MUIM_Application_PushMethod, o, 3,
                                      SWM_NGroup_StopRead, TRUE, FALSE);
                             break;
 
     case SWS_M_P_CloseNoSv: DoMethod(App, MUIM_Application_PushMethod, o, 3,
                                      SWM_NGroup_StopRead, FALSE, FALSE);
                             break;

     case SWS_M_P_ToggleWins:DoMethod(App, MUIM_Application_PushMethod, o, 1,
                                      SWM_NGroup_ToggleWins);
                             break;

     case SWS_M_G_FetchNew:  DoMethod(o, SWM_NGroup_Fetch, TAG_DONE);
                             break;

     case SWS_M_G_CleanCache:DoMethod(o, SWM_NGroup_CleanCache);
                             break;

     case SWS_M_G_Expire:    DoMethod(o, SWM_NGroup_ExpireOld, TAG_DONE);
                             break;

     case SWS_M_G_PostNew:   DoMethod(o, SWM_NGroup_PostNew);
                             break;

     case SWS_M_G_FAQ:       DoMethod(o, SWM_NGroup_OpenFAQList);
                             break;

     case SWS_M_G_ResetArtPos: idata->SWA_NGroup_LastRdMsgE = 0;
                             break;

     case SWS_M_A_LimSel:    DoMethod(o, SWM_NGroup_LimitToSel);
                             break;

     case SWS_M_A_Unlimit:   DoMethod(o, SWM_NGroup_ChgSelFlt, 0);
                             break;
      
     case SWS_M_A_R_SelArts: DoMethod(o, SWM_NGroup_Remove, FALSE);
                             break;
      
     case SWS_M_A_R_SelThrds:DoMethod(o, SWM_NGroup_Remove, TRUE);
                             break;
      
     case SWS_M_A_R_UnRemove:DoMethod(o, SWM_NGroup_UnRemove);
                             break;
      
     case SWS_M_A_Search:    DoMethod(o, SWM_NGroup_ALQSWin);
                             break;
      
     case SWS_M_A_SelectAll: DoMethod(idata->ArtList, MUIM_List_Select,
                                      MUIV_List_Select_All,
                                      MUIV_List_Select_On, NULL);
                             break;

     case SWS_M_A_S_Read:
     case SWS_M_A_S_Unread:
     case SWS_M_A_S_Marked:
     case SWS_M_A_S_Cached:
     case SWS_M_A_S_Uncached:
     case SWS_M_A_S_Perm:
     case SWS_M_A_S_New:    DoMethod(o, SWM_NGroup_Select, msg->Call);
                            break;

     case SWS_M_A_ToggleSel: DoMethod(idata->ArtList, MUIM_List_Select,
                                      MUIV_List_Select_All,
                                      MUIV_List_Select_Toggle, NULL);
                             break;

     case SWS_M_AS_Read: case SWS_M_AS_Unread:
     case SWS_M_AT_Read: case SWS_M_AT_Unread:
     case SWS_M_AG_Read: case SWS_M_AG_Unread:
     case SWS_M_AS_Mk:   case SWS_M_AS_UnMk:
     case SWS_M_AT_Mk:   case SWS_M_AT_UnMk:
     case SWS_M_AG_Mk:   case SWS_M_AG_UnMk:
     case SWS_M_AS_ResetNew:
     case SWS_M_AT_ResetNew: 
     case SWS_M_AG_ResetNew: 
                             DoMethod(o, SWM_NGroup_Mark,
                                      msg->Call == SWS_M_AS_Read     ||
                                      msg->Call == SWS_M_AT_Read     ||
                                      msg->Call == SWS_M_AG_Read     ||
                                      msg->Call == SWS_M_AS_ResetNew ||
                                      msg->Call == SWS_M_AT_ResetNew ||
                                      msg->Call == SWS_M_AG_ResetNew ||
                                      msg->Call == SWS_M_AS_Mk       ||
                                      msg->Call == SWS_M_AT_Mk       ||
                                      msg->Call == SWS_M_AG_Mk,
                                      msg->Call == SWS_M_AS_Read     || 
                                      msg->Call == SWS_M_AT_Read     || 
                                      msg->Call == SWS_M_AG_Read     ||
                                      msg->Call == SWS_M_AS_Unread   || 
                                      msg->Call == SWS_M_AT_Unread   || 
                                      msg->Call == SWS_M_AG_Unread ? 1 :
                                      msg->Call == SWS_M_AS_ResetNew ||
                                      msg->Call == SWS_M_AT_ResetNew ||
                                      msg->Call == SWS_M_AG_ResetNew ? 2 : 0,
                                      msg->Call == SWS_M_AT_Read     ||
                                      msg->Call == SWS_M_AT_Unread   ||
                                      msg->Call == SWS_M_AT_ResetNew ||
                                      msg->Call == SWS_M_AT_Mk       ||
                                      msg->Call == SWS_M_AT_UnMk,
                                      msg->Call == SWS_M_AG_Read     ||
                                      msg->Call == SWS_M_AG_Unread   ||
                                      msg->Call == SWS_M_AG_Mk       ||
                                      msg->Call == SWS_M_AG_ResetNew ||
                                      msg->Call == SWS_M_AG_UnMk);
                             break;
      
      case SWS_M_AS_Cache: case SWS_M_AS_Perm: case SWS_M_AS_Uncache:
      case SWS_M_AT_Cache: case SWS_M_AT_Perm: case SWS_M_AT_Uncache:
      case SWS_M_AG_Cache: case SWS_M_AG_Perm: case SWS_M_AG_Uncache:
                             DoMethod(o, SWM_NGroup_CacheSel,
                                      msg->Call == SWS_M_AS_Cache ||
                                      msg->Call == SWS_M_AT_Cache ||
                                      msg->Call == SWS_M_AG_Cache ||
                                      msg->Call == SWS_M_AS_Perm  ||
                                      msg->Call == SWS_M_AT_Perm  ||
                                      msg->Call == SWS_M_AG_Perm,
                                      msg->Call == SWS_M_AS_Perm  ||
                                      msg->Call == SWS_M_AT_Perm  ||
                                      msg->Call == SWS_M_AG_Perm,
                                      msg->Call == SWS_M_AT_Cache ||
                                      msg->Call == SWS_M_AT_Perm  ||
                                      msg->Call == SWS_M_AT_Uncache,
                                      msg->Call == SWS_M_AG_Cache ||
                                      msg->Call == SWS_M_AG_Perm  ||
                                      msg->Call == SWS_M_AG_Uncache);

                            break;

     case SWS_M_E_SelectAll:DoMethod(idata->SWA_NGroup_ArtView,
                                     SWM_NArticleView_SelectAll);
                            break;

     case SWS_M_E_Search:   DoMethod(idata->SWA_NGroup_ArtView,
                                     SWM_NArticleView_SWin);
                            break;

     case SWS_M_E_PostFup:  DoMethod(o, SWM_NGroup_PostFup);
                            break;

     case SWS_M_E_SV_Head:
      if (!Parts) Parts = SWF_NArticle_Head; // fall through
     case SWS_M_E_SV_Body:
      if (!Parts) Parts = SWF_NArticle_Body; // fall through
     case SWS_M_E_SV_HdBd:
      if (!Parts) Parts = SWF_NArticle_HdBd;
      MenuSave(o, idata, Parts, NULL, SWA_NDFilter_T_Save);
      break;

     case SWS_M_E_SV_RHead:
      if (!Parts) Parts = SWF_NArticle_Head; // fall through
     case SWS_M_E_SV_RBody:
      if (!Parts) Parts = SWF_NArticle_Body; // fall through
     case SWS_M_E_SV_RHdBd:
      if (!Parts) Parts = SWF_NArticle_HdBd;
      MenuSave(o, idata, Parts, NULL, SWA_NDFilter_T_RAW);
      break;

     case SWS_M_E_PR_Head:
      if (!Parts) Parts = SWF_NArticle_Head; // fall through
     case SWS_M_E_PR_Body:
      if (!Parts) Parts = SWF_NArticle_Body; // fall through
     case SWS_M_E_PR_HdBd:
      if (!Parts) Parts = SWF_NArticle_HdBd;
      MenuSave(o, idata, Parts, NN_PrinterDev(idata), SWA_NDFilter_T_Print);
      break;

     case SWS_M_E_PR_RHead:
      if (!Parts) Parts = SWF_NArticle_Head; // fall through
     case SWS_M_E_PR_RBody:
      if (!Parts) Parts = SWF_NArticle_Body; // fall through
     case SWS_M_E_PR_RHdBd:
      if (!Parts) Parts = SWF_NArticle_HdBd;
      MenuSave(o, idata, Parts, NN_PrinterDev(idata), SWA_NDFilter_T_RAW);
      break;

     case SWS_M_E_AC_Subj:
      if (!Hdr) Hdr = SWF_NAHeader_Subject; // fall through;
     case SWS_M_E_AC_From:
      if (!Hdr) Hdr = SWF_NAHeader_From; // fall through;
     case SWS_M_E_AC_MsgID:
      if (!Hdr) Hdr = SWF_NAHeader_MsgID; // fall through;
     case SWS_M_E_AC_Date:
      if (!Hdr) Hdr = SWF_NAHeader_Date; // fall through;
     case SWS_M_E_AC_Groups:
      if (!Hdr) Hdr = SWF_NAHeader_Groups;
      DoMethod(Art, SWM_NArticle_ClipHeader, Hdr);
      break;

     case SWS_M_E_AC_Head:
      if (!Parts) Parts = SWF_NArticle_Head; // fall through
     case SWS_M_E_AC_Body:
      if (!Parts) Parts = SWF_NArticle_Body; // fall through
     case SWS_M_E_AC_HdBd:
      if (!Parts) Parts = SWF_NArticle_HdBd;
      DoMethod(Art, SWM_NArticle_Clipboard, Parts);
      break;

     case SWS_M_E_RD_Raw:
      DoMethod(o, SWM_NGroup_Redisp, SWV_NGroup_Redisp_Raw);
      break;

     case SWS_M_E_RD_Cooked:
      DoMethod(o, SWM_NGroup_Redisp, SWV_NGroup_Redisp_Cooked);
      break;

     case SWS_M_G_Stats:
      DoMethod(o, SWM_NGroup_GroupStats, TAG_DONE);
      break;

     case SWS_M_P_SelectAll:
      DoMethod(o, SWM_NGroup_MPSelect, TAG_DONE);
      break;

     case SWS_M_P_Check:
      DoMethod(o, SWM_NGroup_MPCheck, TAG_DONE);
      break;

     case SWS_M_P_DispOne:
      DoMethod(o, SWM_NGroup_MPOne, TAG_DONE);
      break;

     case SWS_M_P_DispAll:
      DoMethod(o, SWM_NGroup_MPAll, TAG_DONE);
      break;
   }

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Delete our overview (Cache) directory                             *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_DeleteOVDir, ULONG *)
{
   STRPTR  Path   = OVDir(o, idata, NULL);
   ULONG   rc;
   ULONG   NoPr   = GetTagUL(SWT_NGroup_DelOV_NoPrompt, FALSE, msg+1),
           Force  = GetTagUL(SWT_NGroup_DelOV_Force,    FALSE, msg+1);

   if (!FIsSet(idata->SWA_NGroup_Flags, SWF_NGroup_OVDir) && !Force)
      SWDB_RET(SWV_NGroup_DelOV_OK);

   if (!idata->SWA_NGroup_ArtMgr || SW_STRISNULL(Path))
      SWDB_RET(SWV_NGroup_DelOV_Error);

   if (!SWDirExists(Path)) SWDB_RET(SWV_NGroup_DelOV_OK);
      
   if (!NoPr) {
      Object *Win = GetTagOB(SWT_NGroup_DelOV_Win, idata->ReaderWin, msg+1);
      
      rc = MUI_Request(NN_App(idata), Win, 0, NULL,
                       GetS(SWS_L_AskDelBt), GetS(SWS_L_AskDelDir),
                       idata->SWA_NGroup_Name);

      if (rc == 0) SWDB_RET(SWV_NGroup_DelOV_OK);
   }

   AppSleep(idata, TRUE);

   rc = DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_RemoveOV, Path);

   CacheMk(o, idata, SWV_NGroup_RemChMk);

   AppSleep(idata, FALSE);

   if (rc == SUCCESS) FReset(idata->SWA_NGroup_Flags, SWF_NGroup_OVDir);

   SWDB_RET(DoMethod(o, SWM_Root_AdoptObjError, idata->SWA_NGroup_ArtMgr,
                     rc == SUCCESS ? SWV_NGroup_DelOV_OK :
                                     SWV_NGroup_DelOV_Error));
} SDFT

   
/*****************************************************************************
 * Method: Mark selected articles as read or unread                          *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_CacheSel, SWP_NGroup_CacheSel *)
{
   ULONG x, Count = idata->GrpArticleCnt;
   ULONG Threads = msg->Threads || ThrdMode(idata);
   Object **a = idata->GrpArticles;

   AppSleep(idata, TRUE);

   if (!msg->All)
      if (!(a = GetSelArts(idata, Threads, &Count))) goto Done;

   if (!msg->Cache)
      CacheMk(o, idata, SWV_NGroup_MakeChMk);

   for (x=0; x<Count; x++)
      if (msg->Cache) {
         if (DLQueueAdd(o, idata, a[x],
                        msg->Perm ? SWV_NGroup_DLQ_MakePerm:SWV_NGroup_DLQ_Cache,
                        x==(Count-1)) == FAILURE)
            break;
   
      } else {
         DoMethod(a[x], SWM_NArticle_FastUnCache,
                  SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
                  TAG_DONE);
      }

   if (!msg->All)
      FV(a);

  Done:
   DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);

   DoMethod(o, SWM_NGroup_UpdArtList, Count == 1 ? a[0] : NULL);

   AppSleep(idata, FALSE);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Check for OV dir                                                  *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_TestOVDir, Msg)
{
   if (SWDirExists(OVDir(o, idata, NULL)))
      FSet(idata->SWA_NGroup_Flags,   SWF_NGroup_OVDir);
   else
      FReset(idata->SWA_NGroup_Flags, SWF_NGroup_OVDir);

   SWDB_RET(FIsSet(idata->SWA_NGroup_Flags, SWF_NGroup_OVDir) ? TRUE : FALSE);
} SDFT

   
/*****************************************************************************
 * Method: Clean cache directory                                             *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_CleanCache, Msg)
{
   ULONG rc;
   
   AppSleep(idata, TRUE);

   StartGauge(o, idata, idata->SWA_NGroup_ArtMgr, SWA_NArticleMgr_OVPct,
              GetS(SWS_L_Status_CleanCache), SWV_GC_Private1, FALSE);

   rc = DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_CleanCache,
                 idata->SWA_NGroup_Name, OVDir(o, idata, NULL),
                 1, FALSE, o, idata->SWA_NGroup_GrpDefFlt, NULL);

   StopGauge(o, idata, SWV_GC_Private1);

   AppSleep(idata, FALSE);

   SWDB_RET(rc);
} SDFT

   
/*****************************************************************************
 * Method: Batch DL articles post                                            *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_GL_Arts_P1, ULONG *)
{
   ULONG x, y;
   LONG State  = GetTagUL(SWT_NGroup_CmdState, 0,  msg+1);
   Object *Srv = GetTagObj(SWT_NGroup_Server,  0,  msg+1);
   ULONG    ID = GetTagUL(SWT_NGroup_ID,       0,  msg+1);

   if (State == SWV_NGroup_Begin) {
      idata->SrvInfo[SWV_GC_GL_Arts].Srv = Srv;
      idata->SrvInfo[SWV_GC_GL_Arts].ID  = ID;

      SendCmdState(o, idata, SWV_GC_GL_Arts, SWV_NGroup_Begin);
   }

   if (State == SWV_NGroup_NewProgMsg) {
      idata->SrvInfo[SWV_GC_GL_Arts].ProgMsg =
         idata->SrvInfo[SWV_GC_FetchHdr].ProgMsg;

      SendCmdState(o, idata, SWV_GC_GL_Arts, SWV_NGroup_NewProgMsg);
   }
   
   if (State == SWV_NGroup_Abort) {
      idata->SrvInfo[SWV_GC_GL_Arts].Srv     = NULL;
      idata->SrvInfo[SWV_GC_GL_Arts].ID      = -1;
      idata->SrvInfo[SWV_GC_GL_Arts].ProgMsg = NULL;
      return EndLFCmd(o, idata, SWV_GC_GL_Arts, State);
   }

   if (State == SWV_NGroup_End) {
      Object *DLFlt;

      idata->SrvInfo[SWV_GC_GL_Arts].Srv     = NULL;
      idata->SrvInfo[SWV_GC_GL_Arts].ID      = -1;

      DLFlt = NSFListObject,
         SWA_NList_Master,  SWGetObj(NN_GroupList(idata),
                                     SWA_NGroupList_BatchDLFlt),
         SWA_NSFList_UIMode,  SWV_NSFList_UI_Tiny,
         SWA_NSFList_Group, o,
      End;

      if (!DLFlt) return EndLFCmd(o, idata, SWV_GC_GL_Arts, SWV_NGroup_Abort);
      
      // -- just ended batch headers - send begin for article D/L ------------

      idata->SrvInfo[SWV_GC_GL_Arts].ProgMsg = NULL;
      if (!(idata->BatchArticles = (Object **)AV(ASize(idata->GrpArticleCnt))) ||
          DoMethod(DLFlt, SWM_NSFilter_Begin, o) == FAILURE) {
         MUI_DisposeObject(DLFlt);
         EndLFCmd(o, idata, SWV_GC_GL_Arts, SWV_NGroup_Abort);
         SWDB_RET(0);
      }

      // -- add matching articles to list ------------------------------------

      for (x=y=0; x<idata->GrpArticleCnt; x++)
         if (!DoMethod(idata->GrpArticles[x], SWM_NArticle_FlagTest,
                       CFlags | SWF_NArticle_Killed))
            if (DoMethod(DLFlt, SWM_NSFilter_Match, idata->GrpArticles[x], o))
               idata->BatchArticles[y++] = idata->GrpArticles[x];

      DoMethod(DLFlt, SWM_NSFilter_End);
      MUI_DisposeObject(DLFlt);

      if ((idata->BatchArticleCnt = y) > 0)
         set(o, SWA_NGroup_PermFlags,
             idata->SWA_NGroup_PermFlags | SWF_NGroup_NewBatch);
      
      if (!idata->BatchArticleCnt) {
         ClearBatchDL(idata);
         EndLFCmd(o, idata, SWV_GC_GL_Arts, SWV_NGroup_End);
         SWDB_RET(0);
      }

      idata->SrvInfo[SWV_GC_GL_Arts].Srv = Srv;
      idata->SrvInfo[SWV_GC_GL_Arts].ID  = -1;

      for (x=0; x<(idata->SimulCmds+1); x++)
         FetchBatchArt(o, idata, x);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: check for batchDL done                                          *
 *****************************************************************************/

void BatchDLInc(Object *o, SW_IDATA *idata)
{
   if (++idata->BatchArticlePos >= idata->BatchArticleCnt)
      DoMethod(idata->SrvInfo[SWV_GC_GL_Arts].Srv,
               SWM_NServer_Done,
               idata->SrvInfo[SWV_GC_GL_Arts].ID);
   else {
      SetProgMsg(o, idata, SWV_GC_GL_Arts, SWS_T_PM_Arts,
                 idata->BatchArticlePos, idata->BatchArticleCnt);

      FetchBatchArt(o, idata, idata->BatchArticlePos+idata->SimulCmds);
   }
}

   
/*****************************************************************************
 * Method: Batch DL articles post                                            *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_GL_Arts_P2, SWP_NServer_StatusMsg *)
{
   LONG x;
   STRPTR MsgID;

   if (NSR_SBegin(msg)) {
      idata->SrvInfo[SWV_GC_GL_Arts].Srv = NSR_Srv(msg);
      idata->SrvInfo[SWV_GC_GL_Arts].ID  = NSR_ID(msg);
      set(o, SWA_NGroup_HdrPctDone, 0);

      SetProgMsg(o, idata, SWV_GC_GL_Arts, SWS_T_PM_Arts, 0,
                 idata->BatchArticleCnt);
   }

   if (NSR_SRescode(msg)) {
      Object *Art = idata->BatchArticles[idata->BatchArticlePos];

      if (idata->DBOut && FIsSet(idata->DBType, SWV_DB_BatchDL)) {
         MsgID = (STRPTR)DoMethod(Art, SWM_NArticle_GetMsgID);
         FPrintf(idata->DBOut, "%s: Got BODY rescode: %ld %s\n",
                 __FUNC__, NSR_ResCode(msg), MsgID);
      }

      if (NSR_ResCode(msg) == SWV_NSR_Body) {
         DoMethod(Art, SWM_NArticle_FlagReset, SWF_NArticle_NotFound);

         SWDB_RET(SWV_NServer_RescodeOK);
      } else {
         DoMethod(Art, SWM_NArticle_FlagSet, SWF_NArticle_NotFound);
         
         BatchDLInc(o, idata);
         SWDB_RET(SWV_NServer_RescodeMore);
      }
   }
   
   if (NSR_SNext(msg)) {
      Object *Art = idata->BatchArticles[idata->BatchArticlePos];

      if (idata->DBOut && FIsSet(idata->DBType, SWV_DB_BatchDL)) {
         MsgID = (STRPTR)DoMethod(Art, SWM_NArticle_GetMsgID);
         FPrintf(idata->DBOut, "%s: Got BODY lines: %ld %s\n",
                 __FUNC__, NSR_Count(msg), MsgID);
      }

      DoMethod(Art, SWM_NArticle_FreeText, SWF_NArticle_Body);
      DoMethod(Art, SWM_NArticle_AddSeparator);

      DoMethod(Art, SWM_NArticle_AddLines,
               SWT_NArticle_AddLines,      NSR_Lines(msg),
               SWT_NArticle_AddLineCount,  NSR_Count(msg),
               SWT_NArticle_AddAlloc,      FALSE,
               TAG_DONE);
      
      for (x=0; x<NSR_Count(msg); x++)
         NSR_Line(msg, x) = NULL;

      DoMethod(Art, SWM_NArticle_Cache,
               SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
               TAG_DONE);

      DoMethod(Art, SWM_NArticle_FreeText, SWF_NArticle_Body);
      
      BatchDLInc(o, idata);

      set(o, SWA_NGroup_HdrPctDone,
          idata->BatchArticlePos * 100 / max(idata->BatchArticleCnt, 1));
   }

   if (NSR_SDone(msg)) {
      EndLFCmd(o, idata, SWV_GC_GL_Arts, SWV_NGroup_End);
   }

   if (NSR_SAbort(msg)) {
      EndLFCmd(o, idata, SWV_GC_GL_Arts, SWV_NGroup_Abort);
   }
   
   if (NSR_SAnyEnd(msg)) {
      idata->SrvInfo[SWV_GC_GL_Arts].Srv = NULL;
      idata->SrvInfo[SWV_GC_GL_Arts].ID  = -1;

      DoMethod(o, SWM_NGroup_UpdArtList, NULL);
      ClearBatchDL(idata);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: set flag so we don`t save the overview on dispose.                *
 *   (i.e, if we're trying to delete it and then dispose this obj).          *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_NoSaveOV, Msg)
{
   FSet(idata->SWA_NGroup_Flags, SWF_NGroup_NoSaveOV);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Batch mark articles read/unread                                   *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_GL_Mark, SWP_NGroup_GL_Mark *)
{
   ULONG x, rc = FAILURE;

   if (DoMethod(idata->SWA_NGroup_ArtMgr,SWM_NArticleMgr_AddGroup,o)!=SUCCESS)
      SWDB_RET(FAILURE);

   AppSleep(idata, TRUE);
   
   if (!idata->SWA_NGroup_ReaderOpen)
      if (DoMethod(o, SWM_NGroup_LoadOV, TAG_DONE) != SUCCESS)
         goto Done;

   for (x=0; x<idata->GrpArticleCnt; x++)
      DoMethod(idata->SWA_NGroup_ArtMgr,
               msg->Read ? SWM_NArticleMgr_ReadListAdd :
                           SWM_NArticleMgr_ReadListRem,
               idata->GrpArticles[x]);

   if (!msg->Read)
      DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_ReadListRemBl);

   CountRead(o, idata);
   
   if (idata->SWA_NGroup_ReaderOpen)
      DoMethod(o, SWM_NGroup_UpdArtList, NULL);
   else
      DoMethod(o, SWM_NGroup_SaveOV, TAG_DONE);

   rc = SUCCESS;

  Done:
   AppSleep(idata, FALSE);
   DoMethod(idata->SWA_NGroup_ArtMgr,SWM_NArticleMgr_RemGroup,o);

   if (!idata->SWA_NGroup_ReaderOpen)
      NukeArtArrays(idata);

   SWDB_RET(rc);
} SDFT

   
/*****************************************************************************
 * Method: Enable/Disable menu stuff                                         *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_OnOffLine, SWP_NGroup_OnOffLine *)
{
   ULONG x;
   Object *Menu, *Item;
   static ULONG NoNWMenus[] = {
      SWS_M_G_FetchNew,
      SWS_M_AS_Cache,
      SWS_M_AS_Perm,
      SWS_M_AT_Cache,
      SWS_M_AT_Perm,
      SWS_M_AG_Cache,
      SWS_M_AG_Perm,
      NULL,
   };
   
   if (!idata->ReaderWin)
      SWDB_RET(0);

   if (!(Menu = SWGetObj(idata->ReaderWin, MUIA_Window_Menustrip)))
      SWDB_RET(0);

   for (x=0; NoNWMenus[x]; x++)
      if (Item = (Object *)DoMethod(Menu, MUIM_FindUData, NoNWMenus[x]))
         set(Item, MUIA_Menuitem_Enabled, msg->State);

   if (idata->FAQListWin)
      DoMethod(idata->FAQListObj, SWM_FAQList_OnOffLine, msg->State);
   
   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Kill a bunch of articles                                          *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_KillSel, SWP_NGroup_KillSel *)
{
   ULONG Type    = SWGetUL(msg->Type,  MUIA_Cycle_Active),
         Threads = Type==1 || ThrdMode(idata);

   if (!idata->ArtList) return FAILURE;
   
   if (SWGetUL(msg->When,  MUIA_Cycle_Active) == 1)
      MkFlt(idata, Type,
            SWGetUL(msg->Exp,   MUIA_Numeric_Value),
            SWGetUL(msg->ExpOk, MUIA_Selected),
            SWGetUL(msg->Where, MUIA_Cycle_Active));
   
   SWDB_RET(RemArts(o, idata, Threads, Type == 2,
                    idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_KillArts, FALSE));
} SDFT

   
/*****************************************************************************
 * Method: Delete articles from our list of displayed ones                   *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NArticleMgr_DelArticles, ULONG *)
{
   ULONG x, y;
   Object *lo, *VArt;
   ULONG  ArtCnt = GetTagUL(SWT_NArticleMgr_ArticleCount, 0, msg+1);
   Object **ArtA = GetTagTyp(SWT_NArticleMgr_ArticleArray, NULL, msg+1,
                             Object **);

   if (!ArtCnt) SWDB_RET(0);

   // -- remove articles from displayed list ---------------------------------

   if (idata->ArtList) {
      if (idata->KList != ArtA) {
         set(idata->ArtList, MUIA_List_Quiet, TRUE);
   
         DoMethod(idata->ArtList, MUIM_List_Select, MUIV_List_Select_All,
                  MUIV_List_Select_Off, NULL);
         set(idata->ArtList, MUIA_List_Active, MUIV_List_Active_Off);
   
         SW_MUIListLoop(idata->ArtList, x, lo, TRUE)
            for (y=0; y<ArtCnt; y++)
               if (lo == ArtA[y]) {
                  DoMethod(idata->ArtList, MUIM_List_Select, x,
                           MUIV_List_Select_On, NULL);
                  break;
               }
      }
   
      DoMethod(idata->ArtList, MUIM_List_Remove, MUIV_List_Remove_Selected);
   
      if (idata->KList != ArtA)
         set(idata->ArtList, MUIA_List_Quiet, FALSE);
   }

   RemFromList(idata->CacheArticles,  &idata->CacheArticleCnt,  ArtA, ArtCnt);

   if (RemFromList(idata->GrpArticles, &idata->GrpArticleCnt, ArtA, ArtCnt)) {
      if (RemFromList(idata->SelArticles, &idata->SelArticleCnt, ArtA, ArtCnt))
         RemFromList(idata->UIArticles,  &idata->UIArticleCnt,  ArtA, ArtCnt);

      if (idata->SWA_NGroup_ThreadMgr)
         DoMethod(idata->SWA_NGroup_ThreadMgr,
                  SWM_NArticleMgr_DelArts,
                  SWT_NArticleMgr_ArticleArray, ArtA,
                  SWT_NArticleMgr_ArticleCount, ArtCnt,
                  TAG_DONE);
   }

   if (idata->ArtList) {
      DoMethod(idata->ArtList, SWM_NArticleList_ChangeTitle, 0);
   
      DoMethod(idata->ArtList,
               SWM_NArticleMgr_DelArts,
               SWT_NArticleMgr_ArticleArray, ArtA,
               SWT_NArticleMgr_ArticleCount, ArtCnt,
               TAG_DONE);
   }


   // -- tell articleview we don`t have this one any more --------------------

   if (idata->SWA_NGroup_ArtView) {
      VArt = SWGetObj(idata->SWA_NGroup_ArtView, SWA_NArticleView_DspArt);

      for (y=0; VArt && y<ArtCnt; y++)
         if (ArtA[y] == VArt) {
            DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_Clear);
            break;
         }
   }

   // -- recount read articles & find new oldest -----------------------------

   CountRead(o, idata);
      
   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Re-display based on new display filters                           *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_ChgDspFlt, Msg)
{
   Object *Active;

   DoMethod(idata->ArtList, MUIM_List_GetEntry, MUIV_List_GetEntry_Active,
            &Active);
   
   if (Active)
      DLQueueAdd(o, idata, Active, SWV_NGroup_DLQ_Read, TRUE);
   
   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Run action filters against articles                               *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_RunActFlt, Msg)
{
   ULONG x, Count = 0;

   Object **a;

   AppSleep(idata, TRUE);

   if (!(a = GetSelArts(idata, ThrdMode(idata), &Count))) goto Done;

   if (Count) {
      if (!FIsSet(idata->SWA_NGroup_Flags, SWF_NGroup_Action)) {
         DoMethod(idata->SWA_NGroup_ActFlt, SWM_NSFList_DoSelected, SWM_NAFilter_Begin, o);
         DoMethod(idata->Filter_GlobAct,    SWM_NSFList_DoSelected, SWM_NAFilter_Begin, o);
      
         FSet(idata->SWA_NGroup_Flags, SWF_NGroup_Action);
      }

      for (x=0; x<Count; x++)
         if (DLQueueAdd(o, idata, a[x], SWV_NGroup_DLQ_Action,
                        x==(Count-1)) == FAILURE)
            break;
   }

   FV(a);

  Done:
   DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);
   AppSleep(idata, FALSE);

   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Browse articles: page up/dn, or get next/prev art/thread          *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_Browse, SWP_NGroup_Browse *)
{
   Object *a, *ab;
   ULONG  BPos, BRet;

   if (!idata->SWA_NGroup_ArtView || !idata->ArtList) SWDB_RET(0);

   BPos = SWGetUL(idata->ArtList, MUIA_List_Active);

   // -- if we have no article, need to get one ------------------------------
   
   if (ThrdMode(idata)) {
      idata->BrowseThPos = SWGetUL(idata->ArtList, MUIA_List_Active);
      
      SWDB_RET(BrowseBase(o, idata, TRUE, 0));
   }

   // -- try paging - if we can move, then just return -----------------------

   if (msg->Mode == SWV_NGroup_Browse_Browse)
      if ((BRet = DoMethod(idata->SWA_NGroup_ArtView,
                           SWM_NArticleView_Browse, msg->Dir))
          == SWV_NArticleView_Browse_Move)
         SWDB_RET(0);

   // -- display 1st available/1st unread article if asked to ----------------
   
   if (msg->Mode == SWV_NGroup_Browse_1stAvail ||
       msg->Mode == SWV_NGroup_Browse_1stUnread)
      SWDB_RET(BrowseBase(o, idata, FALSE, msg->Mode));
   
   // -- otherwise, find article to display in this thread -------------------

   if (msg->Mode != SWV_NGroup_Browse_ThreadMov) {
      BPos = (BPos == MUIV_List_Active_Off) ? 0 :
         BPos + (BRet == SWV_NArticleView_Browse_NoArticle ? 0 : msg->Dir);
   
      if (BPos < 0)
         a = NULL;
      else {
         ULONG BA    = NN_BrowseArts(idata),
               Flags = BA == SWV_NNews_BA_All       ? 0 :
                       BA == SWV_NNews_BA_Unread    ? SWF_NArticle_Read   :
                       BA == SWV_NNews_BA_New       ? SWF_NArticle_NewArt :
                       (SWF_NArticle_Read|SWF_NArticle_NewArt);
         ULONG FlagT = BA == SWV_NNews_BA_All       ? 0 :
                       BA == SWV_NNews_BA_Unread    ? 0 :
                       BA == SWV_NNews_BA_New       ? SWF_NArticle_NewArt :
                       SWF_NArticle_NewArt;
         
         a = NextArt(idata, (LONG *)&BPos, msg->Dir,
                     Flags | SWF_NArticle_NotFound, FlagT);
      }
      
      if (a) {
         set(idata->ArtList, MUIA_List_Active, BPos);
         DLQueueAdd(o, idata, a, SWV_NGroup_DLQ_Read, TRUE);
         SWDB_RET(0);
      }
   }

   // -- no articles in this thread?  gotta get next thread ------------------

   if (idata->SWA_NGroup_ThreadFT == SWF_NGroup_ThreadFlt_All) {
      BPos = SWGetUL(idata->ArtList, MUIA_List_Active);
      BPos = (BPos == MUIV_List_Active_Off) ? 0 : BPos;

         DoMethod(idata->ArtList, MUIM_List_GetEntry, BPos, &ab);      

      do {
         BPos += msg->Dir;
         DoMethod(idata->ArtList, MUIM_List_GetEntry, BPos, &a);

         if (a)
            if (!DoMethod(idata->SWA_NGroup_ThreadMgr, SWM_NArticleMgr_SameThread,
                          ab, a))
               break;
      } while (a);

      set(idata->ArtList, MUIA_List_Active, BPos);
      DLQueueAdd(o, idata, a, SWV_NGroup_DLQ_Read, TRUE);
   } else {
      ArtListQuiet(idata, TRUE);
      DoMethod(o, SWM_NGroup_ChgThreadFlt, SWF_NGroup_ThreadFlt_Base, TRUE, FALSE);

      if ((BPos = SWGetUL(idata->ArtList, MUIA_List_Active)) != MUIV_List_Active_Off)
         idata->BrowseThPos = BPos;

      set(idata->ArtList, MUIA_List_Active,
          idata->BrowseThPos += (idata->BrowseNoBase ? msg->Dir-1 : msg->Dir));

      BrowseBase(o, idata, TRUE, 0);
      ArtListQuiet(idata, FALSE);
   }
   
   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: Save/Load: get file/dir/pattern                                   *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_SaveArt, SWP_NGroup_SaveArt *)
{
   STRPTR Dest = msg->FSpec;
   TEXT   FSpec[FMSIZE];
   ULONG  Parts  = msg->Parts ? msg->Parts : SWF_NArticle_HdBd,
          Attr   = msg->Attr  ? msg->Attr  : SWA_NDFilter_T_RAW;

   if (SW_STRISNULL(Dest))
      Dest = (STRPTR)DoMethod(o, SWM_Root_RequestFileName,
                       SWT_Root_SL_Title,       GetS(SWS_L_SaveArt),
                       SWT_Root_SL_InitDrawer,  idata->SWA_NGroup_LastDir,
                       SWT_Root_SL_InitPattern, idata->SWA_NGroup_LastPatt,
                       SWT_Root_SL_SaveMode,    TRUE,
                       SWT_Root_SL_Buffer,      FSpec,
                       SWT_Root_SL_Win,         idata->ReaderWin,
                       TAG_DONE);

   SWDB_RET(DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_ArtFwd,
                     SWM_NArticle_SaveText,
                     SWT_NArticle_FSpec,   Dest,
                     SWT_NArticle_Parts,   Parts,
                     SWT_NArticle_Append,  FALSE,
                     SWT_NArticle_Grp,     o,
                     SWT_NArticle_ArtView, idata->SWA_NGroup_ArtView,
                     SWT_NArticle_SvAttr,  Attr,
                     TAG_DONE));
} SDFT

   
/*****************************************************************************
 * Method: Expire old articles                                               *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_ExpireOld, ULONG *)
{
   UBYTE CmdBuff[SWV_NServer_MaxCmdLen];
   ULONG rc;
   Object *Srv;
   
   // -- if offline, just expire what we have --------------------------------

   if (!IsOnline(idata)) {
      if (ClosedGetOVStart(o, idata) != SUCCESS)
         SWDB_RET(FAILURE);

      ExpireOld(o, idata, FALSE, LONG_MAX);

      ClosedGetOVEnd(o, idata);
      SWDB_RET(SUCCESS);
   }

   // -- otherwise, need to get group stats ----------------------------------

   if (!(Srv = CheckSendCmd(o, idata, msg, SWV_GC_ExpireArts)))
      SWDB_RET(FAILURE);

   sprintf(CmdBuff,"GROUP %s\r\nSTAT\r\nXHDR Date\r\n",idata->SWA_NGroup_Name);

   AB_SetMod(idata);

   rc = DoMethod(Srv,
                 SWM_NServer_SendCmd,
                 SWT_NServer_Cmd,       CmdBuff,
                 SWT_NServer_CmdObj,    o,
                 SWT_NServer_CmdMethod, SWM_NGroup_ExpireOld_P1,
                 SWT_NServer_Title,     MakeTitle(o, idata, SWV_GC_ExpireArts,
                                                  SWS_L_TExpire),
                 TAG_DONE);

   SWDB_RET(DoMethod(o, SWM_Root_AdoptObjError, Srv, rc));
} SDFT

   
/*****************************************************************************
 * Method: Expire old articles                                               *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_ExpireOld_P1, SWP_NServer_StatusMsg *)
{
   MsgBegin(o, idata, msg, SWS_L_Status_Expiring, SWV_GC_ExpireArts);
   
   if (NSR_SDone(msg) && NSR_Count(msg) == 3) {
      ULONG  Oldest;
      STRPTR Date = NSR_Line(msg, 2);

      Oldest = DoMethod(NN_ArtTpl(idata), SWM_NArticle_ParseDate,
                        Date + strcspn(Date, " ") +1, NULL, TRUE);

      ExpireOld(o, idata, FALSE, Oldest);
   }

   MsgEnd(o, idata, msg, SWV_GC_ExpireArts);
   
   SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Method: new project                                                       *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NNews_NewProj, Msg)
{
   if (FIsSet(idata->SWA_NGroup_PermFlags, SWF_NGroup_WinOpen) &&
       NN_GrpWinOpen(idata) == SWV_NNews_WinLastSaved)
      DoMethod(o, SWM_NGroup_StartRead);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Open FAQ list window                                              *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_OpenFAQList, Msg)
{
   TEXT LocalDir[FMSIZE];
   
   // -- if window exists, move to front -------------------------------------
   
   if (idata->FAQListWin) {
      DoMethod(idata->FAQListWin, MUIM_Window_ToFront);
      SWDB_RET(SUCCESS);
   }

   // -- allocate space for FAQ title and build it ---------------------------

   idata->FAQWinTitle = AV(strlen(SW_STRDEF(idata->SWA_NGroup_Name, "")) +
                           strlen(GetS(SWS_L_FAQWinT)) + 4);

   if (!idata->FAQWinTitle) SWDB_RET(FAILURE);

   sprintf(idata->FAQWinTitle, "%s %s",
           idata->SWA_NGroup_Name, GetS(SWS_L_FAQWinT));

   // -- make the FAQ list window --------------------------------------------

   strcpy(LocalDir, OVDir(o, idata, NULL));
   AddPart(LocalDir, SWV_NGroup_FAQDir, FMSIZE);
   
   idata->FAQListWin = WindowObject,
      MUIA_Window_Title,       idata->FAQWinTitle,
      MUIA_Window_ID,          SWGetUL(o, SWA_ObjClassHash) ^
                               MAKE_ID('F','A','Q','s'),
      MUIA_Window_IsSubWindow, TRUE,
      WindowContents,          idata->FAQListObj =
         NewObject(FAQListClass(), NULL,
                   AppBaseDefTop(idata),
                   SWA_FAQList_LocalDir, LocalDir,
                   SWA_FAQList_Group,    o,
                   TAG_DONE),
   End;

   if (!idata->FAQListWin) goto Error;

   // -- set up notifications ------------------------------------------------
   
   DoMethod(NN_App(idata), OM_ADDMEMBER, idata->FAQListWin);
   set(idata->FAQListWin, MUIA_Window_Open, TRUE);

   DoMethod(idata->FAQListWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            NN_App(idata), 4, MUIM_Application_PushMethod, o, 1,
            SWM_NGroup_CloseFAQList);

   SWDB_RET(SUCCESS);

  Error:
   DoMethod(o, SWM_NGroup_CloseFAQList);
   SWDB_RET(FAILURE);
} SDFT

   
/*****************************************************************************
 * Method: Close FAQ list window                                             *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_CloseFAQList, Msg)
{
   if (!idata->FAQListWin) SWDB_RET(SUCCESS);

   set(idata->FAQListWin, MUIA_Window_Open, FALSE);
   DoMethod(NN_App(idata), OM_REMMEMBER, idata->FAQListWin);

   MUI_DisposeObject(idata->FAQListWin);
   idata->FAQListWin = NULL;

   FV(idata->FAQWinTitle);
   idata->FAQWinTitle = NULL;
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Change highlight filter                                           *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_ChgHLFlt, Msg)
{
   ULONG rc;
   
   if ((rc = ChgHLFlt(o, idata, idata->GrpArticles, idata->GrpArticleCnt))
       == SUCCESS)
      DoMethod(o, SWM_NGroup_UpdArtList, NULL);
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Update article list                                               *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_UpdArtList, SWP_NGroup_UpdArtList *)
{
   LONG x;
   Object *lo;
   
   if (!idata->ArtList) SWDB_RET(0);

   // -- only one article?  try to update just that line ---------------------
   
   if (msg->Art) {
      SW_MUIListLoop(idata->ArtList, x, lo, TRUE)
         if (lo == msg->Art) {
            DoMethod(idata->ArtList, MUIM_List_Redraw, x);
            break;
         }

      SWDB_RET(0);
   }

   // -- otherwise, update the whole shebang ---------------------------------

   DoMethod(idata->ArtList, MUIM_List_Redraw, MUIV_List_Redraw_All);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: process arexx command                                             *
 *****************************************************************************/

METHOD_NOINLINE(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *)
{
   STRPTR cmd = ARG0(msg->Rx);
   Object *App = msg->App, *Art;
   UBYTE TmpBuff[32];
   ULONG x;

   if (!strcmp(cmd, "SAVEOV")) {
      if (!idata->ReaderWin) goto Err;
      DoMethod(o, SWM_NGroup_SaveOV, TAG_DONE);
      goto OK;
   }

   if (!strcmp(cmd, "FETCHNEW")) {
      if (!idata->ReaderWin) goto Err;
      DoMethod(o, SWM_NGroup_Fetch, TAG_DONE);
      goto OK;
   }

   if (!strcmp(cmd, "FETCHPENDING")) {
      set(App, MUIA_Application_RexxString,
          idata->SrvInfo[SWV_GC_FetchHdr].Srv ? "1" : "0");
      goto OK;
   }
      
   if (!strcmp(cmd, "EXPIRE")) {
      if (!idata->ReaderWin) goto Err;
      DoMethod(o, SWM_NGroup_ExpireOld, TAG_DONE);
      goto OK;
   }

   if (!strcmp(cmd, "OPENFAQLIST")) {
      DoMethod(o, SWM_NGroup_OpenFAQList);
      goto OK;
   }

   if (!strcmp(cmd, "ISFAQLISTOPEN")) {
      set(App, MUIA_Application_RexxString, idata->FAQListWin ? "1" : "0");
      goto OK;
   }

   if (!strcmp(cmd, "CLOSEFAQLIST")) {
      DoMethod(o, SWM_NGroup_CloseFAQList);
      goto OK;
   }

   if (!strcmp(cmd, "OPENARTSEARCHWIN")) {
      if (!idata->ReaderWin) goto Err;
      DoMethod(o, SWM_NGroup_ALQSWin);
      goto OK;
   }

   if (!strcmp(cmd, "OPENBODYSEARCHWIN")) {
      if (!idata->ReaderWin || !idata->SWA_NGroup_ArtView) goto Err;
      DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_SWin);
      goto OK;
   }

   if (!strcmp(cmd, "BATCHDLHDRS")) {
      DoMethod(o, SWM_NGroup_GL_Hdrs, TAG_DONE);
      goto OK;
   }

   if (!strcmp(cmd, "BATCHDLHDRSPENDING")) {
      set(App, MUIA_Application_RexxString,
          idata->SrvInfo[SWV_GC_GL_Hdrs].Srv ? "1" : "0");
      goto OK;
   }
      
   if (!strcmp(cmd, "BATCHDLARTS")) {
      DoMethod(o, SWM_NGroup_GL_Arts, TAG_DONE);
      goto OK;
   }

   if (!strcmp(cmd, "BATCHDLARTSPENDING")) {
      set(App, MUIA_Application_RexxString,
          idata->SrvInfo[SWV_GC_GL_Arts].Srv ? "1" : "0");
      goto OK;
   }
      
   if (!strcmp(cmd, "DISPARTCOUNT")) {
      if (!idata->ReaderWin) goto Err;
      sprintf(TmpBuff, "%ld", idata->UIArticleCnt);
      set(App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }

   if (!strcmp(cmd, "GROUPARTCOUNT")) {
      if (!idata->ReaderWin) goto Err;
      sprintf(TmpBuff, "%ld", idata->GrpArticleCnt);
      set(App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }

   if (!strncmp(cmd, "SETTHREADMODE ", 14)) {
      if (!idata->ReaderWin) goto Err;
      x    = 256;
      cmd += 14;
      if (!strcmp(cmd, "ALL"))     x = SWF_NGroup_ThreadFlt_All;
      if (!strcmp(cmd, "1THREAD")) x = SWF_NGroup_ThreadFlt_1Thread;
      if (!strcmp(cmd, "BASE"))    x = SWF_NGroup_ThreadFlt_Base;
      DoMethod(o, SWM_NGroup_ChgThreadFlt, x, TRUE, TRUE);
      goto OK;
   }

   if (!strcmp(cmd, "GETTHREADMODE")) {
      if (!idata->ReaderWin) goto Err;

      set(App, MUIA_Application_RexxString,
          idata->SWA_NGroup_ThreadFT==SWF_NGroup_ThreadFlt_All     ? "ALL"     :
          idata->SWA_NGroup_ThreadFT==SWF_NGroup_ThreadFlt_1Thread ? "1THREAD" :
          idata->SWA_NGroup_ThreadFT==SWF_NGroup_ThreadFlt_Base    ? "BASE"    :
          "UNKNOWN");
      goto OK;
   }

   if (!strncmp(cmd, "ARTBROWSE", 9)) {
      if (!idata->ReaderWin) goto Err;

      DoMethod(o, SWM_NGroup_Browse,
               strstr(cmd+9, "BACK")   ? -1 : 1,  
               strstr(cmd+9, "THREAD") ? SWV_NGroup_Browse_ThreadMov    :
               strstr(cmd+9, "ART")    ? SWV_NGroup_Browse_ArtMov :
                                         SWV_NGroup_Browse_Browse);
      goto OK;
   }

   if (!strncmp(cmd, "ARTSCROLL", 9)) {
      if (!idata->ReaderWin) goto Err;

      DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_Scroll,
               strstr(cmd+9, "BACK") ? -1 : 1, 0);
      goto OK;
   }

   if (!strcmp(cmd, "ARTICLETOP")) {
      if (!idata->ReaderWin) goto Err;
      DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_Top);
      goto OK;
   }

   if (!strcmp(cmd, "ARTICLEBOTTOM")) {
      if (!idata->ReaderWin) goto Err;
      DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_Bottom);
      goto OK;
   }

   if (!strncmp(cmd, "DISPART ", 8)) {
      STRPTR MsgID = NULL;
      
      if (!idata->ReaderWin) goto Err;

      cmd += 8;
      if (cmd[0] == '<') {
         MsgID = cmd;
      } else {
         DoMethod(idata->ArtList, MUIM_List_GetEntry, atol(cmd), &Art);

         if (Art)
            MsgID = (STRPTR)DoMethod(Art, SWM_NArticle_GetMsgID);   
      }

      if (SW_STRISNULL(MsgID)) goto Err;

      DoMethod(o, SWM_NGroup_ViewMsgID,
               SWT_NGroup_VMID_MsgID, MsgID,
               TAG_DONE);
      goto OK;
   }

   if (!strcmp(cmd, "ARTFETCHPENDING")) {
      if (!idata->ReaderWin) goto Err;

      set(App, MUIA_Application_RexxString, idata->DLQueue ? "1" : "0");
      goto OK;
   }

   if (!strncmp(cmd, "SETACTIVE ", 10)) {
      if (!idata->ReaderWin) goto Err;
      x = atol(cmd+10);
      set(idata->ArtList, MUIA_List_Active, x);
      goto OK;
   }

   if (!strncmp(cmd, "WINCTRL ", 8)) {
      LONG w = -1, wcmd = -1;
      
      cmd += 8;
      if (!strncmp(cmd, "ARTLIST", 7)) w = SWV_NGroup_WC_AL;
      if (!strncmp(cmd, "ARTVIEW", 7)) w = SWV_NGroup_WC_AV;
      cmd = strchr(cmd, ' ');

      if (SW_STRISNULL(cmd) || cmd[1] == '\0') goto Err;
      cmd++;
      if (!strncmp(cmd, "BACK", 4))  wcmd = SWV_NGroup_WC_Back;
      if (!strncmp(cmd, "FRONT", 5)) wcmd = SWV_NGroup_WC_Front;

      DoMethod(o, SWM_NGroup_WinCtrl, w, wcmd,
               SWV_NGroup_WC_Keep, SWV_NGroup_WC_Keep);
      goto OK;
   }
   
   if (!strncmp(cmd, "ARTCMD ", 7)) {
      STRPTR ArtCmd, OrigCmd;
      if (!idata->ReaderWin) goto Err;
      cmd += 7;
      ArtCmd = strchr(cmd, ' ');

      if (SW_STRISNULL(ArtCmd) || ArtCmd[1] == '\0') goto Err;
      
      x = atol(cmd);
      DoMethod(idata->ArtList, MUIM_List_GetEntry, x, &Art);
      if (!Art) goto Err;

      ArtCmd++;
      OrigCmd       = ARG0(msg->Rx);
      ARG0(msg->Rx) = ArtCmd;
      x             = DoMethodA(Art, msg);
      ARG0(msg->Rx) = OrigCmd;
      SWDB_RET(x);
   }

   SWDB_RET(SWV_AppBase_RexxNotFound);

  OK:
   *msg->rc = RC_OK;
   SWDB_RET(SWV_AppBase_RexxFound);

  Err:
   *msg->rc = RC_ERROR;
   SWDB_RET(SWV_AppBase_RexxFound);
} SDFT


/*****************************************************************************
 * Method: toggle single/dual windows                                        *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_ToggleWins, Msg)
{
   ULONG Active,
         NewMode = idata->ArtViewWin ? SWV_NNews_GW_Single :
                                       SWV_NNews_GW_Separate;

   if (!idata->ReaderWin) SWDB_RET(FAILURE);

   Active = SWGetUL(idata->ArtList, MUIA_List_Active);
   
   // -- toggle the window mode ----------------------------------------------
   
   DoMethod(o, SWM_NGroup_CloseReaderWin);
   DoMethod(o, SWM_NGroup_OpenReaderWin,
            SWT_NGroup_GrpWinMode, NewMode,
            TAG_DONE);

   // -- add our articles back into the thread manager -----------------------

   DoMethod(o, SWM_NGroup_ChgUIFlt, 0);

   set(idata->ArtList, MUIA_List_Active, Active);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Close stat window                                                 *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_CloseStatWin, Msg)
{
   if (idata->StatWin) {
      set(idata->StatWin, MUIA_Window_Open, FALSE);
      DoMethod(idata->App, OM_REMMEMBER, idata->StatWin);
      idata->StatWin = NULL;
   }

   FV(idata->StatWinTitle);
   idata->StatWinTitle = NULL;
   
   SWDB_RET(SUCCESS);
}


/*****************************************************************************
 * Method: Display group stats window                                        *
 *****************************************************************************/

static Object *strii(STRPTR b)
{
   return TextObject,
      TextFrame,
      MUIA_Background,     MUII_TextBack,
      MUIA_Text_Contents,  b,
      MUIA_Text_SetMin,    TRUE,
   End;
}

static void stri(Object *cgrp, ULONG lab, SW_IDATA *idata, ULONG n, LONG Size)
{
   static TEXT b1[16], b2[16], b3[20];

   sprintf(b1, "%ld", n);
   sprintf(b2, "(%ld%%)",
           n * 100 / (idata->GrpArticleCnt ? idata->GrpArticleCnt : 1));

   strcpy(b3, "n/a");

   if (Size >= 0)
      SWFmtSize(Size, b3, 1);
   
   DoMethod(cgrp, OM_ADDMEMBER, Label2(GetS(lab)));
   DoMethod(cgrp, OM_ADDMEMBER, strii(b1));
   DoMethod(cgrp, OM_ADDMEMBER, strii(b2));
   DoMethod(cgrp, OM_ADDMEMBER, strii(b3));
}

static Object *gstitle(SW_IDATA *idata, ULONG l)
{
   return TextObject,
      MUIA_Text_Contents, GetS(l),
      MUIA_Text_PreParse, MUIX_C MUIX_B MUIX_U,
      MUIA_Text_SetMin,   TRUE,
   End;
}

METHOD_NOINLINE(SWM_NGroup_GroupStats, ULONG *)
{
   Object *Art, *cgrp;
   ULONG Flags;
   LONG Size, x, TotSize = 0, UISize = 0,
     Killed = 0, 
     Read   = 0, ReadS   = 0,
     Cached = 0, CachedS = 0,
     Perm   = 0, PermS   = 0,
     Hi     = 0, HiS     = 0,
     New    = 0, NewS    = 0;
   ULONG KilledCnt, KilledMax = 0;
   Object **KilledArts = NULL;

   DoMethod(o, SWM_NGroup_CloseStatWin);
   
   AppSleep(idata, TRUE);
   
   for (x=0; x<idata->GrpArticleCnt; x++)
      if (Art = idata->GrpArticles[x]) {
         Flags = DoMethod(Art, SWM_NArticle_FlagTest, 0xffffffff);
         Size  = DoMethod(Art, SWM_NArticle_GetBytes);
         TotSize += Size;
         
         if (FIsSet(Flags, SWF_NArticle_Read))       Read++, ReadS += Size;
         if (FIsSet(Flags, SWF_NArticle_Permanent))  Perm++, PermS += Size;
         else
            if (FIsSet(Flags, SWF_NArticle_CachedBody)) Cached++, CachedS += Size;
         if (FIsSet(Flags, SWF_NArticle_Highlight))  Hi++, HiS += Size;
         if (FIsSet(Flags, SWF_NArticle_NewArt))     New++, NewS += Size;
      }

   // -- Killed article stats ------------------------------------------------
   
   set(idata->SWA_NGroup_GrpDefFlt, SWA_NSF_InGroup_Killed,    TRUE);

   KilledCnt = DoMethod(idata->SWA_NGroup_ArtMgr,
                        SWM_NArticleMgr_FilterList,
                        SWV_NArticleMgr_Filter_UseTbl, 0,
                        &KilledArts, &KilledMax, 0, FALSE, 1, FALSE, o,
                        idata->SWA_NGroup_GrpDefFlt, NULL);

   set(idata->SWA_NGroup_GrpDefFlt, SWA_NSF_InGroup_Killed,    FALSE);

   for (x=0; x<KilledCnt; x++)
      if (Art = KilledArts[x]) {
         Flags = DoMethod(Art, SWM_NArticle_FlagTest, 0xffffffff);
         if (FIsSet(Flags, SWF_NArticle_Killed))
            Killed++;
      }

   FV(KilledArts);

   // -- Count UI articles ---------------------------------------------------
   
   for (x=0; x<idata->UIArticleCnt; x++)
      UISize += DoMethod(idata->UIArticles[x], SWM_NArticle_GetBytes);

   AppSleep(idata, FALSE);

   // -- allocate space for FAQ title and build it ---------------------------

   idata->StatWinTitle = AV(strlen(SW_STRDEF(idata->SWA_NGroup_Name, "")) +
                            strlen(GetS(SWS_GS_Title)) + 4);

   if (!idata->StatWinTitle) SWDB_RET(FAILURE);

   sprintf(idata->StatWinTitle, "%s %s",
           idata->SWA_NGroup_Name, GetS(SWS_GS_Title));

   idata->StatWin = WindowObject,
      MUIA_Window_Title,       idata->StatWinTitle,
      MUIA_Window_ID,          SWGetUL(o, SWA_ObjClassHash) ^
                               MAKE_ID('S','t','w','d'),
      MUIA_Window_IsSubWindow, TRUE,
      WindowContents,          cgrp = ColGroup(4),
         Child, HSpace(0),
         Child, gstitle(idata, SWS_GST_Count),
         Child, gstitle(idata, SWS_GST_Percent),
         Child, gstitle(idata, SWS_GST_Size),
       End,
   End;

   stri(cgrp, SWS_GS_Total,     idata, idata->GrpArticleCnt, TotSize);
   stri(cgrp, SWS_GS_Read,      idata, Read, ReadS);
   stri(cgrp, SWS_GS_Unread,    idata, idata->GrpArticleCnt-Read, TotSize - ReadS);
   stri(cgrp, SWS_GS_Killed,    idata, Killed, -1);
   stri(cgrp, SWS_GS_Cached,    idata, Cached, CachedS);
   stri(cgrp, SWS_GS_Permanent, idata, Perm, PermS);
   stri(cgrp, SWS_GS_Hilighted, idata, Hi, HiS);
   stri(cgrp, SWS_GS_New,       idata, New, NewS);
   stri(cgrp, SWS_GS_Displayed, idata, idata->UIArticleCnt, UISize);

   if (!idata->StatWin) goto Error;

   DoMethod(NN_App(idata), OM_ADDMEMBER, idata->StatWin);
   set(idata->StatWin, MUIA_Window_Open, TRUE);

   DoMethod(idata->StatWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            NN_App(idata), 4, MUIM_Application_PushMethod, o, 1,
            SWM_NGroup_CloseStatWin);

  Error:
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Select related multiparts                                         *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_MPSelect, ULONG *)
{
   ULONG x;
   Object *Active, *Art, **ArtArray = NULL;
   MPInfo *MP;
   ULONG rc = FAILURE;
   ULONG Inter = GetTagUL(SWT_NGroup_Interactive, TRUE, msg+1);
   
   if (!idata->ArtList) SWDB_RET(FAILURE);

   AppSleep(idata, TRUE);

   DoMethod(idata->ArtList,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,&Active);

   if (!Active) goto Done;

   if (!(ArtArray = (Object **)AV(sizeof(Object *) * (idata->UIArticleCnt+1))))
      goto Done;
   
   SW_MUIListLoop(idata->ArtList, x, Art, TRUE)
      ArtArray[x] = Art;

   DoMethod(idata->ArtList, MUIM_List_Select,
            MUIV_List_Select_All, MUIV_List_Select_Off, NULL);

   // -- go find a list of the parts we have ---------------------------------

   MP = (MPInfo *)
   DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_MultiPart_Find,
            SWT_NArticleMgr_Article,      Active,
            SWT_NArticleMgr_ArticleCount, idata->UIArticleCnt,
            SWT_NArticleMgr_ArticleArray, ArtArray,
            TAG_DONE);

   if (!MP) goto Done;

   // -- select all parts ----------------------------------------------------
   
   for (x=0; x<=MP[0].MaxPart; x++)
      if (MP[x].Art)
         DoMethod(idata->ArtList, MUIM_List_Select, MP[x].ArrayPos,
                  MUIV_List_Select_On, NULL);

   // -- report on ones we couldn`t find -------------------------------------
   
   if (Inter) MPReportMissing(idata, MP);

   // -- free up the list ----------------------------------------------------
   
   DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_MultiPart_Free, MP);

   rc = SUCCESS;
   
  Done:
   FV(ArtArray);
   AppSleep(idata, FALSE);
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Check to see if we're missing parts etc                           *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_MPCheck, ULONG *)
{
   MPInfo *MP;
   Object *Active;

   DoMethod(idata->ArtList,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,&Active);

   if (!Active) SWDB_RET(0);

   // -- go find a list of the parts we have ---------------------------------

   AppSleep(idata, TRUE);
   
   MP = (MPInfo *)
   DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_MultiPart_Find,
            SWT_NArticleMgr_Article,      Active,
            SWT_NArticleMgr_ArticleCount, idata->UIArticleCnt,
            SWT_NArticleMgr_ArticleArray, idata->UIArticles,
            TAG_DONE);

   AppSleep(idata, FALSE);

   if (!MP) SWDB_RET(0);

   // -- report on ones we couldn`t find -------------------------------------
   
   MPReportMissing(idata, MP);

   // -- free up the list ----------------------------------------------------
   
   DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_MultiPart_Free, MP);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Display single part of multipart file                             *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_MPOne, ULONG *)
{
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Display all parts of multipart file                               *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_MPAll, ULONG *)
{
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Window control                                                    *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_WinCtrl, SWP_NGroup_WinCtrl *)
{
   Object *Win;
   
   switch (msg->Win) {
     case SWV_NGroup_WC_AL: Win = idata->ReaderWin;  break;
     case SWV_NGroup_WC_AV: Win = idata->ArtViewWin; break;
     default: SWDB_RET(FAILURE);
   }   

   switch (msg->BackFront) {
     case SWV_NGroup_WC_Back:  DoMethod(Win, MUIM_Window_ToBack);  break;
     case SWV_NGroup_WC_Front: DoMethod(Win, MUIM_Window_ToFront); break;
   }
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Select some articles                                              *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_Select, SWP_NGroup_Select *)
{
   ULONG x, Flags;
   Object *Art;
   
   if (!idata->ArtList)
      SWDB_RET(FAILURE);

   DoMethod(idata->ArtList, MUIM_List_Select,
            MUIV_List_Select_All,
            MUIV_List_Select_Off, NULL);

   for (x=0;; x++) {
      DoMethod(idata->ArtList, MUIM_List_GetEntry, x, &Art);
      if (!Art) break;

      Flags = DoMethod(Art, SWM_NArticle_FlagTest, 0xffffffff);

      if (msg->Type == SWS_M_A_S_Read   &&  FIsSet(Flags, SWF_NArticle_Read)   ||
          msg->Type == SWS_M_A_S_Unread && !FIsSet(Flags, SWF_NArticle_Read)   ||
          msg->Type == SWS_M_A_S_Marked &&  FIsSet(Flags, SWF_NArticle_UserMk) ||
          msg->Type == SWS_M_A_S_Cached &&
             (FIsSet(Flags, SWF_NArticle_CachedBody) &&
              !FIsSet(Flags, SWF_NArticle_Permanent)) ||
          msg->Type == SWS_M_A_S_Perm && FIsSet(Flags, SWF_NArticle_Permanent) ||
          msg->Type == SWS_M_A_S_New  && FIsSet(Flags, SWF_NArticle_NewArt))
         DoMethod(idata->ArtList, MUIM_List_Select,
                  x, MUIV_List_Select_On, NULL);
   }
   
   SWDB_RET(SUCCESS);
} SDFT

   
/*****************************************************************************
 * Method: Remove selected arts/threads                                      *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_Remove, SWP_NGroup_Remove *)
{
   SWDB_RET(RemArts(o, idata, msg->Threads, FALSE,
                    o, SWM_NArticleMgr_DelArticles, TRUE));
} SDFT


/*****************************************************************************
 * Method: Unremove - restore arts removed with SWM_NGroup_Remove            *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_UnRemove, Msg)
{
   ULONG  First, x, y, Found, OldGrpCnt;
   Object **Arts = NULL, ArtsCnt, ArtsMax = 0;

   First = idata->GrpArticleCnt;

   // -- get all articles for this group -------------------------------------
   ArtsCnt = DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_FilterList,
                      SWV_NArticleMgr_Filter_UseTbl, 0,
                      &Arts, &ArtsMax,
                      idata->GrpArticleCnt, FALSE, 1, FALSE, o,
                      idata->SWA_NGroup_GrpDefFlt, NULL);

   if (ArtsCnt == First) goto Done;

   if (ArtsCnt >= idata->GrpArticleMax) {
      idata->GrpArticleMax = ArtsCnt;
      RV(ArtsCnt * sizeof(Object *), &idata->GrpArticles);
   }

   // -- put removed ones back -----------------------------------------------
   // -- to be restored, it has to be marked with SWF_NArticle_Removed AND ---
   // -- not in the current group article list -------------------------------

   for (x=0, OldGrpCnt = idata->GrpArticleCnt; x<ArtsCnt; x++)
      if (DoMethod(Arts[x], SWM_NArticle_FlagTest, SWF_NArticle_Removed)) {
         for (y=0, Found = FALSE; y<OldGrpCnt; y++)
            if (idata->GrpArticles[y] == Arts[x]) {
               Found = TRUE;
               break;
            }

         if (!Found)
            idata->GrpArticles[idata->GrpArticleCnt++] = Arts[x];
      }
   
   // -- update display, thread manager, etc ---------------------------------
   
   if (idata->GrpArticleCnt != First) {
      ThrdMgrAdd(o, idata,
                 idata->GrpArticles + First,
                 idata->GrpArticleCnt - First);
      
      DoMethod(o, SWM_NGroup_ChgSelFlt, 0);

      CountRead(o, idata);
   }

  Done:
   FV(Arts);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * find and select article in artlist                                        *
 *****************************************************************************/
   
static LONG FindArtInList(SW_IDATA *idata, Object *Find)
{
   LONG x;
   Object *Art;
   
   SW_MUIListLoop(idata->ArtList, x, Art, TRUE)
      if (Art == Find) {
         set(idata->ArtList, MUIA_List_Active, x);
         return x;
      }

   set(idata->ArtList, MUIA_List_Active, MUIV_List_Active_Off);

   return -1;
}


/*****************************************************************************
 * Method: Show a particular message-ID                                      *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NGroup_ViewMsgID, ULONG *)
{
   STRPTR MsgID      = GetTagUB(SWT_NGroup_VMID_MsgID,      NULL,  msg+1);
   LONG   UpdArtList = GetTagL (SWT_NGroup_VMID_UpdArtList, TRUE,  msg+1),
          UpdArtView = GetTagL (SWT_NGroup_VMID_UpdArtView, TRUE,  msg+1),
          NoReq      = GetTagL (SWT_NGroup_VMID_NoReq,      FALSE, msg+1),
          BaseArt    = GetTagL (SWT_NGroup_VMID_BaseArt,    FALSE, msg+1),
          Pos, OrigTMode;
   Object *Art, *ThreadBase;

   // -- look for the article in the group -----------------------------------

   if (!(Art = (Object *)DoMethod(idata->SWA_NGroup_ThreadMgr,
                                  SWM_NArticleMgr_Find, MsgID))) {

      if (!NoReq) {
         SWERR(o, FAILURE, SWS_Err_MsgIdNotFound, MsgID, 0);
         DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);
      }
      
      SWDB_RET(FAILURE);
   }


   // -- find it in the artlist ----------------------------------------------

   if (UpdArtList) {
      if (BaseArt || FindArtInList(idata, Art) < 0) {
         ThreadBase = (Object *)DoMethod(idata->SWA_NGroup_ThreadMgr,
                                         SWM_NArticleMgr_ThreadBase, Art);

         if (!ThreadBase) SWDB_RET(FAILURE);

         if (!BaseArt) {
            OrigTMode = idata->SWA_NGroup_ThreadFT;

            ArtListQuiet(idata, TRUE);
            DoMethod(o, SWM_NGroup_ChgThreadFlt, SWF_NGroup_ThreadFlt_Base, TRUE, FALSE);
         }

         if ((Pos = FindArtInList(idata, ThreadBase) < 0)) {
            if (!BaseArt)
               DoMethod(o, SWM_NGroup_ChgThreadFlt, OrigTMode, TRUE, FALSE);
         } else {
            idata->BrowseThPos = Pos;

            if (!BaseArt) {
               DoMethod(o, SWM_NGroup_ChgThreadFlt, SWF_NGroup_ThreadFlt_1Thread,
                        TRUE, FALSE);
               FindArtInList(idata, Art);
            }
         }

         if (!BaseArt)
            ArtListQuiet(idata, FALSE);
      }
   }

   
   // -- display it in the artview -------------------------------------------

   if (UpdArtView)
      FetchIfPossible(o, idata, Art);

   SWDB_RET(SUCCESS);
} SDFT


# endif
