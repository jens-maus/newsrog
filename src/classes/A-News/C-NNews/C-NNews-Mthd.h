/*****************************************************************************
 * Method: Open one of our windows                                           *
 *****************************************************************************/

METHOD(SWM_NNews_OpenWin, SWP_NNews_OpenWin *)
{
   Object *m = NULL, *sm = NULL, *App    = SWGetObj(o, SWA_AppBase_App),
          *DefaultObj;
   struct NWin *w = &idata->Win[msg->Win];

   if (msg->Win < 0 || msg->Win >= SWV_WinMax) SWDB_RET(0);

   if (!App) SWDB_RET(0);

   // -- bring win to front if already open ----------------------------------

   if (w->Window) {
      set(w->Window, MUIA_Window_Open, TRUE);
      DoMethod(w->Window, MUIM_Window_ToFront);
      SWDB_RET(SUCCESS);
   }

   // -- Create menus --------------------------------------------------------

   if (msg->Win == SWV_PstListWin) {
      if (!idata->PLGrp) {
         idata->PLGrp = RegisterGroup(idata->PLTabs),
            MUIA_Background, MUII_RegisterBack,
            MUIA_CycleChain, 1,
         End;

         if (!idata->PLGrp) SWDB_RET(FAILURE);
      }

      DoMethod(*w->Contents, OM_ADDMEMBER, idata->SWA_NNews_PostList);
      DoMethod(*w->Contents, OM_ADDMEMBER, idata->SWA_NNews_SvPostList);
   }

   if (msg->Win == SWV_MMListWin) {
      if (!idata->MMGrp) {
         idata->MMGrp = RegisterGroup(idata->MMTabs),
            MUIA_Background, MUII_RegisterBack,
            MUIA_CycleChain, 1,
         End;

         if (!idata->MMGrp) SWDB_RET(FAILURE);
      }

      DoMethod(*w->Contents, OM_ADDMEMBER, idata->SWA_NNews_MMList);
      DoMethod(*w->Contents, OM_ADDMEMBER, idata->SWA_NNews_FAList);
   }

   if (msg->Win == SWV_CfgListWin) {
      if (!idata->Config) {
         Object *AttrUI, *ProjUI, *tgad, *tparent;

         DoMethod(o, MUIM_MultiSet,
                  SWA_NSFList_ShowGroups, TRUE,
                  idata->SWA_NNews_GlobSel,
                  idata->SWA_NNews_GlobHL,
                  idata->SWA_NNews_GlobKill,
                  idata->SWA_NNews_GlobDsp,
                  idata->SWA_NNews_GlobAct,
                  TAG_DONE);

         set(idata->SWA_NNews_GlobDsp, SWA_NSFList_FltType, "#?NDFilter#?");
         set(idata->SWA_NNews_GlobAct, SWA_NSFList_FltType, "#?NAFilter#?");

         SetColSpecs(idata);
         
         idata->Config = RegisterGroup(idata->CfgTabs),
            MUIA_Background, MUII_RegisterBack,
            MUIA_CycleChain, 1,
            Child, ProjUI = (Object *)DoMethod(o, SWM_AppBase_UI),
            Child, AttrUI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o),
            Child, DoMethod(idata->SWA_NNews_ArtListCol, SWM_ListCol_UI),
            Child, RegisterGroup(idata->FltTabs),
               Child, DoMethod(idata->SWA_NNews_GlobSel,    SWM_NSFilter_UI),
               Child, DoMethod(idata->SWA_NNews_GlobKill,   SWM_NSFilter_UI),
               Child, DoMethod(idata->SWA_NNews_GlobDsp,    SWM_NDFilter_UI),
               Child, DoMethod(idata->SWA_NNews_GlobAct,    SWM_NDFilter_UI),
               Child, DoMethod(idata->SWA_NNews_GlobHL,     SWM_NSFilter_UI),
            End,
            // last childs are: idata->SWA_NNews_VarList, added dynamically,
            // and              idata->SWA_NNews_HdrList, added dynamically.
         End;

         // -- move a few things to the project UI ---------------------------
         
         if ((tgad = (Object *)DoMethod(AttrUI, MUIM_FindUData,
                                        SWA_NNews_AutoSave)) &&
             (tparent = SWGetObj(tgad, MUIA_Parent))) {
            DoMethod(tparent, OM_REMMEMBER, tgad);
            DoMethod(ProjUI, OM_ADDMEMBER, Label2(GetS(SWS_L_AutoSave)));
            DoMethod(ProjUI, OM_ADDMEMBER, tgad);
         }

         PopSrvListObject,
            SWA_AppBase_TopObj,        o,
            SWA_PopSrvList_AttrUI,     AttrUI,
            SWA_PopSrvList_SrvAttr,    SWA_NNews_FAQServer,
            SWA_PopSrvList_SrvObj,     o,
            SWA_PopSrvList_CopyOnDisp, FALSE,  // don't update during dispose
            SWA_PopSrvList_DefBt,      FALSE,
         End;

         DoMethod(o, MUIM_Notify, SWA_NNews_ToolBarStyle, MUIV_EveryTime,
                  o, 2, SWM_NNews_MakeToolbar, MUIV_TriggerValue);
         
         if (!idata->Config) SWDB_RET(FAILURE);
      }
      
      DoMethod(*w->Contents, OM_ADDMEMBER, idata->SWA_NNews_VarList);
      DoMethod(*w->Contents, OM_ADDMEMBER, idata->SWA_NNews_HdrList);
   }

   // -- create menu strip ---------------------------------------------------
   
   if (msg->Win != SWV_CfgListWin && msg->Win != SWV_MMListWin) {
      m  = (Object *)DoMethod(o, SWM_NNews_MenuStrip);

      if (msg->Win != SWV_NNewsWin)
         sm = (Object *)DoMethod(*w->Contents, SWM_NNews_MenuStrip);

      if (sm) {
         DoMethod(sm, MUIM_Family_Transfer, m);
         MUI_DisposeObject(sm);
      }
   }

   // -- create toolbar if required ------------------------------------------

   if (msg->Win == SWV_NNewsWin)
      DoMethod(o, SWM_NNews_MakeToolbar, idata->SWA_NNews_ToolBarStyle);


   // -- create window -------------------------------------------------------

   DefaultObj = SWGetObj(*w->Contents, SWA_NList_LV);
      
   w->Window = WindowObject,
      MUIA_Window_Title,          w->Title,
      MUIA_Window_ID,             w->ID,
      MUIA_Window_IsSubWindow,    TRUE,
      MUIA_Window_AppWindow,      TRUE,
      DefaultObj ? MUIA_Window_DefaultObject : TAG_IGNORE, DefaultObj,
      m          ? MUIA_Window_Menustrip     : TAG_IGNORE, m,
      WindowContents,          w->RGroup = VGroup,
         Child, *w->Contents,
      End,
   End;

   if (!w->Window) SWDB_RET(FAILURE);

   DoMethod(App, OM_ADDMEMBER, w->Window);
   set(w->Window, MUIA_Window_Open, TRUE);

   if (msg->Win == SWV_NNewsWin) {
      DoMethod(w->Window,  MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
               App, 6, MUIM_Application_PushMethod, o, 3,
               SWM_AppBase_M_Quit, w->Window, SWV_AppBase_Mode_Menu);
   } else {
      DoMethod(w->Window, MUIM_Notify, MUIA_Window_MenuAction, MUIV_EveryTime,
               App, 6, MUIM_Application_PushMethod, *w->Contents, 3,
               SWM_AppBase_MenuCall, MUIV_TriggerValue, w->Window);

      DoMethod(w->Window,  MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
               App, 6, MUIM_Application_PushMethod, o, 3,
               SWM_NNews_CloseWin, msg->Win, FALSE);
   }

   if (w->LastVar)
      set(o, w->LastVar, TRUE);

   // -- let appbase know this is our main window ----------------------------

   if (msg->Win == SWV_NNewsWin) {
      set(o, SWA_AppBase_Win, w->Window);
      
      ((struct Process *)FindTask(0))->pr_WindowPtr =
         SWGetObj(w->Window, MUIA_Window_Window);
   }

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Close one of our windows                                          *
 *****************************************************************************/

METHOD(SWM_NNews_CloseWin, SWP_NNews_CloseWin *)
{
   Object *App = SWGetObj(o, SWA_AppBase_App);
   struct NWin *w;
   ULONG  x, OpenCnt;

   if (msg->Win < 0 || msg->Win >= SWV_WinMax) SWDB_RET(0);

   // -- otherwise, close the window normally --------------------------------
   
   w = &idata->Win[msg->Win];
   
   if (!w->Window) SWDB_RET(0);

   if (!msg->Shutdown) {
      
      // -- see how many windows are currently open --------------------------
   
      for (x=OpenCnt=0; x<SWV_WinMax; x++)
         if (idata->Win[x].Window)
            OpenCnt++;
      
      // -- if this is the last window, iconify the app instead --------------
   
      if (OpenCnt==1) {
         set(App, MUIA_Application_Iconified, TRUE);
         SWDB_RET(0);
      }
   }

   set(w->Window, MUIA_Window_Open, FALSE);

   if (w->LastVar)
      set(o, w->LastVar, FALSE);

   if (msg->Win == SWV_CfgListWin) {
      DoMethod(*w->Contents, OM_REMMEMBER, idata->SWA_NNews_VarList);
      DoMethod(*w->Contents, OM_REMMEMBER, idata->SWA_NNews_HdrList);
   }

   if (msg->Win == SWV_MMListWin) {
      DoMethod(*w->Contents, OM_REMMEMBER, idata->SWA_NNews_MMList);
      DoMethod(*w->Contents, OM_REMMEMBER, idata->SWA_NNews_FAList);
   }

   if (msg->Win == SWV_PstListWin) {
      DoMethod(*w->Contents, OM_REMMEMBER, idata->SWA_NNews_PostList);
      DoMethod(*w->Contents, OM_REMMEMBER, idata->SWA_NNews_SvPostList);
   }

   if (msg->Win == SWV_NNewsWin) {
      if (msg->Shutdown)
         ((struct Process *)FindTask(0))->pr_WindowPtr = NULL;

      if (idata->ToolBarGrp) {
         DoMethod(o, OM_REMMEMBER, idata->ToolBarGrp);
         MUI_DisposeObject(idata->ToolBarGrp);
         idata->ToolBarGrp = NULL;
      }
   }
   
   DoMethod(App, OM_REMMEMBER, w->Window);

   DoMethod(w->RGroup, OM_REMMEMBER, *w->Contents);
   MUI_DisposeObject(w->Window);

   w->Window = NULL;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Open one of our windows                                           *
 *****************************************************************************/

METHOD(SWM_AppBase_ShowAppUI, Msg)
{
   DoMethod(o, SWM_NNews_OpenWin, SWV_NNewsWin);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: About Window                                                      *
 *****************************************************************************/

METHOD(SWM_AppBase_M_About, SWP_AppBase_MenuFn *)
{
   static const char AboutText[] = "\33b\33c %s - "
      "Amiga News Reader\33n\n\n"
      "Version: %s\n"
      "%s\n\n"
      "Registration: \33i%s\33n\n\n"
      "This program uses the \33bMUI\33n object library.\n"
      "MUI is © by Stefan Stuntz\n\n"
      "Program Author: Steve Koren\n\n"
      "\33cSupport: http://www.frii.com/~srk/ShadowWorks/";

   MUI_Request(SWGetObj(o, SWA_AppBase_App),
               msg->Win, 0, NULL, GetS(SWS_B_OK), (char *)AboutText,
               APPNAME,
               SWGetStr(_app(o), MUIA_Application_Version)+6,
               SWGetStr(_app(o), MUIA_Application_Copyright),
               SWGetStr(SWGetObj(o, SWA_AppBase_Reg), SWA_RegStatus));

   SWDB_RET(SUCCESS);
}


/*****************************************************************************
 * Method: Find by window ptr                                                *
 *****************************************************************************/

static LONG FindWnPt(SW_IDATA *idata, Object *Win)
{
   LONG x;
   
   for (x=0; x < SWV_WinMax; x++)
      if (idata->Win[x].Window == Win)
         return x;

   return -1;
}


/*****************************************************************************
 * Method: Process menu event                                                *
 *****************************************************************************/

METHOD(SWM_AppBase_MenuCall, SWP_AppBase_MenuCall *)
{
   switch (msg->Call) {
     case SWS_M_Launcher:  DoMethod(o, SWM_NNews_OpenWin, SWV_NNewsWin);
                           break;

     case SWS_M_GroupWin:  DoMethod(o, SWM_NNews_OpenWin, SWV_GrpListWin);
                           break;
      
     case SWS_M_ServWin:   DoMethod(o, SWM_NNews_OpenWin, SWV_SrvListWin);
                           break;

     case SWS_M_TCPWin:    DoMethod(o, SWM_NNews_OpenWin, SWV_TcpListWin);
                           break;

     case SWS_M_PostWin:   DoMethod(o, SWM_NNews_OpenWin, SWV_PstListWin);
                           break;

     case SWS_M_MMWin:     DoMethod(o, SWM_NNews_OpenWin, SWV_MMListWin);
                           break;

     case SWS_M_ConfigWin: DoMethod(o, SWM_NNews_OpenWin, SWV_CfgListWin);
                           break;

     case SWS_M_Offline:
     case SWS_M_Online:    DoMethod(idata->SWA_NNews_TCPList,
                                    SWM_NTCPList_Online,
                                    msg->Call == SWS_M_Online ?
                                       SWV_NTCPList_Online_On    :
                                       SWV_NTCPList_Online_Off);
                           break;

     case SWV_ABM_Hide:    DoMethod(o, SWM_NNews_CloseWin,
                                    FindWnPt(idata, msg->Win), FALSE);
                           break;

     case SWS_M_QuitNoSv:  {
        Object *App = SWGetObj(o, SWA_AppBase_App);
        ULONG  rc;
                                  
        rc = MUI_Request(App, msg->Win, 0, NULL, GetS(SWS_B_OKCanc),
                         GetS(SWS_L_ExitQuery));

        if (rc == 1)
           DoMethod(SWGetObj(o, SWA_AppBase_App),
                    MUIM_Application_ReturnID,
                    MUIV_Application_ReturnID_Quit);
        break;
     }
      
     default: DoSuperMethodA(cl, o, msg);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: New project                                                       *
 *****************************************************************************/
   
METHOD(SWM_AppBase_NewProj, Msg)
{
   if (DoSuperMethodA(cl, o, msg) != SUCCESS)
      SWDB_RET(FAILURE);
   
   ClrLists(idata);

   DoMethod(o, SWM_NNews_NewProj);
   DoMethodA(idata->SWA_NNews_GroupList, msg);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Create menu strip                                                 *
 *****************************************************************************/
   
METHOD(SWM_NNews_MenuStrip, Msg)
{
   SWDB_RET(MUI_MakeObject(MUIO_MenustripNM, MainMenu, 0));
} SDFT


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   VL_FindRet("CacheDir",  idata->SWA_NNews_CacheDir);
   VL_FindRet("UserName",  idata->SWA_NNews_UserName);
   VL_FindRet("UserEmail", idata->SWA_NNews_UserEmail);
   VL_FindRet("UserOrg",   idata->SWA_NNews_UserOrg);
   VL_FindRet("SigFile",   idata->SWA_NNews_SigFile);
   VL_FindRet("Editor",    idata->SWA_NNews_Editor);
   
   SWDB_RET(DoMethodA(idata->SWA_NNews_VarList, (Msg)msg));
} SDFT


/*****************************************************************************
 * Function: restore window state                                            *
 *****************************************************************************/

static void RestoreWinState(Object *o, SW_IDATA *idata)
{
   ULONG lv, ov, x;
   
   for (x=0; x<SWV_WinMax; x++)
      if ((lv = idata->Win[x].LastVar) && (ov = idata->Win[x].OpenVar))
         if (SWGetUL(o, ov) == SWV_NNews_WinOpen ||
             SWGetUL(o, ov) == SWV_NNews_WinLastSaved && SWGetUL(o, lv))
            DoMethod(o, SWM_NNews_OpenWin, x);
         else
            DoMethod(o, SWM_NNews_CloseWin, x, FALSE);
}


/*****************************************************************************
 * Function: Set top object                                                  *
 *****************************************************************************/

static void SetTopObj(SW_IDATA *idata, Object *NewsObj)
{
   idata->SWA_AppBase_TopObj = NewsObj;
   
   DoMethod(NewsObj, MUIM_MultiSet,
            SWA_AppBase_TopObj, NewsObj,
            idata->SWA_NNews_ArtMgr,
            idata->SWA_NNews_ServList,
            idata->SWA_NNews_GroupList,
            idata->SWA_NNews_PostList,
            idata->SWA_NNews_SvPostList,
            idata->SWA_NNews_GlobSel,
            idata->SWA_NNews_GlobHL,
            idata->SWA_NNews_GlobKill,
            idata->SWA_NNews_GlobDsp,
            idata->SWA_NNews_GlobAct,
            idata->SWA_NNews_MMList,
            idata->SWA_NNews_HdrList,
            idata->SWA_NNews_FAList,
            NULL);
}


/*****************************************************************************
 * Method: post new project hook                                             *
 *****************************************************************************/
   
METHOD(SWM_AppBase_PostNewProj, Msg)
{
   ULONG x;

   if (DoSuperMethodA(cl, o, msg) != SUCCESS)
      SWDB_RET(FAILURE);

   SetTopObj(idata, o);
   CreateGlobFlt(o, idata);

   DoMethod(idata->SWA_NNews_TCPList, SWM_NTCPList_Online,
            idata->SWA_NNews_StartNWMode == SWV_NNews_StartNW_On ?
            SWV_NTCPList_Online_On : SWV_NTCPList_Online_Off);

   DoMethod(idata->SWA_NNews_GlobSel,  SWM_NSFilter_Expire);
   DoMethod(idata->SWA_NNews_GlobHL,   SWM_NSFilter_Expire);
   DoMethod(idata->SWA_NNews_GlobKill, SWM_NSFilter_Expire);

   SetColSpecs(idata);
   GetUserJunk(o, idata);

   RestoreWinState(o, idata);

   for (x=0; x<=SWV_LastProjWin; x++)
      if (idata->Win[x].Contents)
         DoMethod(*idata->Win[x].Contents, SWM_NNews_NewProj);

   // -- load read cache -----------------------------------------------------

   DoMethod(idata->SWA_NNews_ArtMgr, SWM_NArticleMgr_ReadListLoad,
            idata->SWA_NNews_CacheDir);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: post save project hook                                            *
 *****************************************************************************/
   
METHOD(SWM_AppBase_PostSaveProj, SWP_AppBase_PostSaveProj *)
{
   if (DoSuperMethodA(cl, o, msg) != SUCCESS)
      SWDB_RET(FAILURE);

   // -- Save read cache -----------------------------------------------------

   DoMethod(idata->SWA_NNews_ArtMgr, SWM_NArticleMgr_ReadListSave,
            idata->SWA_NNews_CacheDir);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: make toolbar                                                      *
 *****************************************************************************/
   
METHOD(SWM_NNews_MakeToolbar, SWP_NNews_MakeToolbar *)
{
   Object *b_Grp, *b_Srv, *b_Cfg, *b_TCP, *b_Pst, *b_MM;

   if (idata->LastTBStyle == msg->Style && idata->ToolBarGrp)
      SWDB_RET(SUCCESS);
   
   DoMethod(o, MUIM_Group_InitChange);

   if (idata->ToolBarGrp) {
      DoMethod(o, OM_REMMEMBER, idata->ToolBarGrp);
      MUI_DisposeObject(idata->ToolBarGrp);
      idata->ToolBarGrp = NULL;
   }

   idata->LastTBStyle = msg->Style;
   
   DoMethod(o, OM_ADDMEMBER,
         idata->ToolBarGrp = TBGrp(msg->Style),
            Child, b_Grp = MkIB(idata, SWS_L_Groups,  SWS_H_Groups,  BF_Groups,
                                GetS(SWS_CC_Groups)[0], msg->Style),

            Child, b_Srv = MkIB(idata, SWS_L_Servers, SWS_H_Servers, BF_Servers,
                                GetS(SWS_CC_Servers)[0], msg->Style),

            Child, b_TCP = MkIB(idata, SWS_L_TCP,     SWS_H_TCP,     BF_TCP,
                                GetS(SWS_CC_TCP)[0], msg->Style),

            Child, b_Pst = MkIB(idata, SWS_L_Posts,   SWS_H_Posts,   BF_Posts,
                                GetS(SWS_CC_Posts)[0], msg->Style),

            Child, b_MM  = MkIB(idata, SWS_L_MM,   SWS_H_MM,   BF_MMType,
                                GetS(SWS_CC_MM)[0], msg->Style),
   
            Child, b_Cfg = MkIB(idata, SWS_L_Config,  SWS_H_Config,  BF_Config,
                                GetS(SWS_CC_Config)[0], msg->Style),
         End);
   
   DoMethod(o, MUIM_Group_ExitChange);

   DoMethod(b_Grp, NOTP, o, 2, SWM_NNews_OpenWin, SWV_GrpListWin);
   DoMethod(b_Srv, NOTP, o, 2, SWM_NNews_OpenWin, SWV_SrvListWin);
   DoMethod(b_TCP, NOTP, o, 2, SWM_NNews_OpenWin, SWV_TcpListWin);
   DoMethod(b_MM,  NOTP, o, 2, SWM_NNews_OpenWin, SWV_MMListWin);
   DoMethod(b_Pst, NOTP, o, 2, SWM_NNews_OpenWin, SWV_PstListWin);
   DoMethod(b_Cfg, NOTP, o, 2, SWM_NNews_OpenWin, SWV_CfgListWin);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Get window # from string name                                   *
 *****************************************************************************/

LONG WinNumFromRexxStr(STRPTR WinName)
{
   LONG x;
   STRPTR WinNames[] = {
      "GROUPS",
      "NETWORK",
      "SERVERS",
      "POSTS",
      "MULTIMEDIA",
      "CONFIG",
      NULL,
   };

   if (SW_STRISNULL(WinName)) return -1;
   
   for (x=0; WinNames[x]; x++)
      if (strcmp(WinName, WinNames[x]) == 0)
         return x;

   return -1;
}


/*****************************************************************************
 * Method: process arexx command                                             *
 *****************************************************************************/

METHOD(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *)
{
   STRPTR cmd = ARG0(msg->Rx);
   LONG  WinNum;
   ULONG x, RexxObj[] = {
      SWA_NNews_GroupList,
      SWA_NNews_ServList,
      SWA_NNews_PostList,
      SWA_NNews_TCPList,
      SWA_NNews_MMList,
      SWA_NNews_FAList,
      SWA_NNews_ArtMgr,
      NULL,
   };
   
   // -- open windows --------------------------------------------------------

   if (!strncmp(cmd, "OPENWINDOW ", 11)) {
      if ((WinNum = WinNumFromRexxStr(cmd + 11)) >= 0) {
         DoMethod(o, SWM_NNews_OpenWin, WinNum);
         goto OK;
      }

      goto Err;
   }

   // -- close windows -------------------------------------------------------

   if (!strncmp(cmd, "CLOSEWINDOW ", 12)) {
      if ((WinNum = WinNumFromRexxStr(cmd + 12)) >= 0) {
         DoMethod(o, SWM_NNews_CloseWin, WinNum, FALSE);
         goto OK;
      } 

      goto Err;
   }

   // -- close windows -------------------------------------------------------

   if (!strncmp(cmd, "ISWINDOWOPEN ", 13)) {
      if ((WinNum = WinNumFromRexxStr(cmd + 13)) >= 0) {
         set(msg->App, MUIA_Application_RexxString,
             idata->Win[WinNum].Window ? "1" : "0");
         goto OK;
      }

      goto Err;
   }

   // -- let the other objects have a go at it -------------------------------
   
   for (x=0; RexxObj[x]; x++)
      if (DoMethodA(SWGetObj(o, RexxObj[x]), msg) == SWV_AppBase_RexxFound)
         SWDB_RET(SWV_AppBase_RexxFound);

   // -- nothing worked - pass it to our superclass --------------------------
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));

  OK:
   *msg->rc = RC_OK;
   SWDB_RET(SWV_AppBase_RexxFound);

  Err:
   *msg->rc = RC_ERROR;
   SWDB_RET(SWV_AppBase_RexxFound);

} SDFT


/*****************************************************************************
 * Method: get TextHide class pointer                                        *
 *****************************************************************************/

METHOD(SWM_NNews_EditorClass, Msg)
{
   SWDB_RET(EditorClass());
} SDFT


/*****************************************************************************
 * Method: Get default characterset name                                     *
 *****************************************************************************/

METHOD(SWM_NNews_CharSetName, Msg)
{
   static STRPTR CSetName[] = {
      "ISO-8859-1",
      "ISO-8859-2",
      "ISO-8859-3",
      "ISO-8859-4",
      "ISO-8859-5",
      "ISO-8859-6",
      "ISO-8859-7",
      "ISO-8859-8",
      "ISO-8859-9",
      "ISO-8859-10"
   };

   SWDB_RET(CSetName[idata->SWA_NNews_Charset]);
} SDFT


