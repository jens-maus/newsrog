
/*****************************************************************************
 * Function: ask about overwriting                                           *
 *****************************************************************************/

static LONG AskClob(Object *o, SW_IDATA *idata, Object *Win, ULONG Mode)
{
   ULONG rc;

   if (!idata->SWA_AppBase_ProjMod) return SUCCESS;

   if      (Mode == SWV_AppBase_Mode_Force)  rc = 1;
   else if (Mode == SWV_AppBase_Mode_Script) rc = 2;
   else
      rc = MUI_Request(idata->SWA_AppBase_App, Win, 0,
                       "WARNING!", GetS(SWS_AB_AskOverBt),
                       GetS(SWS_AB_AskOver), NULL);

   if (rc == 0)
      return (LONG)DoMethod(o, SWM_AppBase_M_Save, Win, Mode);

   return rc == 2 ? FAILURE : SUCCESS;
}


/*****************************************************************************
 * Function: set values                                                      *
 *****************************************************************************/

static void SW_AppBase_Set(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (Tag == SWA_AppBase_PopKey)
      idata->AB_PopKey =
         DoMethod(o, SWM_AppBase_AttachHotKey,
                  (UBYTE *)Data, idata->AB_PopKey, o, SWM_AppBase_UnIconify);

   if (Tag == SWA_AppBase_App && Data) {
      muiUserData((Object *)Data) = (ULONG)o;
      set((Object *)Data, MUIA_Application_BrokerHook, &idata->AB_CxHook);
      set((Object *)Data, MUIA_Application_RexxHook, &idata->AB_RxHook);
   }

   if (Tag == SWA_AppBase_Win) {
      DoMethod((Object *)Data, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
               o, 2, SWM_AppBase_AppMessage, MUIV_TriggerValue);

      DoMethod((Object *)Data, MUIM_Notify, MUIA_Window_MenuAction, MUIV_EveryTime,
               idata->SWA_AppBase_App, 6, MUIM_Application_PushMethod, o, 3,
               SWM_AppBase_MenuCall, MUIV_TriggerValue, (Object *)Data);
   }
}


/*****************************************************************************
 * Hook: commodities message                                                 *
 *****************************************************************************/

static LONG FindCxKey(SW_IDATA *idata, ULONG ID)
{
   LONG x;

   if (idata->AB_CxKeys)
      for (x=0; x<idata->AB_CxKeyCount; x++)
         if (idata->AB_CxKeys[x].ID == ID)
            return x;

   return -1;   
}

static __saveds __asm LONG CxFn(a2 Object *App, a1 CxMsg *cxmsg,
                                a0 struct Hook *hook)
{
   LONG  pos;
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   if ((pos = FindCxKey(idata, (ULONG)CxMsgID(cxmsg))) >= 0)
      DoMethod(idata->AB_CxKeys[pos].o, idata->AB_CxKeys[pos].Method);

   return 0;
}


/*****************************************************************************
 * Function: init data                                                       *
 *****************************************************************************/

__saveds __asm ULONG RexxFn(a1 struct RexxMsg *msg,
                            a0 struct Hook *hook)

{
   ULONG rc = RC_FATAL;
   
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   DoMethod(idata->o, SWM_AppBase_RexxCmd, msg, &rc, idata->SWA_AppBase_App);

   return rc;
}


/*****************************************************************************
 * Function: setup object                                                    *
 *****************************************************************************/

static void SW_AppBase_Setup(Object *o, SW_IDATA *idata)
{
   setlent(o, SWA_ObjNoChildCopy, TRUE);

   idata->AB_CxHook.h_Entry  = (ULONG (*)())CxFn;
   idata->AB_CxHook.h_Data   = (void *)idata;
   idata->AB_RxHook.h_Entry  = (ULONG (*)())RexxFn;
   idata->AB_RxHook.h_Data   = (void *)idata;
   idata->A4                 = getreg(REG_A4);
   idata->AB_CxNextID        = TAG_USER;
   idata->o                  = o;
   
   // -- turn on wildstar ----------------------------------------------------

   ((struct DosLibrary *)DOSBase)->dl_Root->rn_Flags |= RNF_WILDSTAR;
}


/*****************************************************************************
 * Function: free data                                                       *
 *****************************************************************************/

static void SW_AppBase_Dispose(SW_IDATA *idata)
{
   FreeVec(idata->AB_CxKeys);
   idata->AB_CxKeys = NULL;
}


/*****************************************************************************
 * Method: Attach hotkey                                                     *
 *****************************************************************************/

METHOD(SWM_AppBase_AttachHotKey, SWP_AppBase_AttachHotKey *)
{
   MsgPort *BrokerPort = NULL;
   CxObj   *Broker, *cx;
   LONG    x;
   UBYTE   *Key;
   Object *App = idata->SWA_AppBase_App;

   if (!App) SWDB_RET(AH_None);

   BrokerPort = SWGetType(App, MUIA_Application_BrokerPort, MsgPort *);
   Broker     = SWGetType(App, MUIA_Application_Broker,     CxObj *);

   if (!BrokerPort || !BrokerPort) SWDB_RET(AH_None);

   DoMethod(o, SWM_ClearObjError);
   
   // -- look for this method already ----------------------------------------

   if (!msg->OldID || (x = FindCxKey(idata, msg->OldID)) < 0)
      if ((x = FindCxKey(idata, 0)) < 0)        // any empty spots?
         x = idata->AB_CxKeyCount;              // else, need more space

   // -- if not found, allocate more space -----------------------------------

   if (x >= idata->AB_CxKeyCount) {
      AbCxCtrl *tmp = AllocVec(sizeof(AbCxCtrl) * (idata->AB_CxKeyCount+4),
                               MEMF_CLEAR|MEMF_ANY);

      if (!tmp) SWDB_RET(AH_None);

      memcpy(tmp, idata->AB_CxKeys, sizeof(AbCxCtrl)*idata->AB_CxKeyCount);
      idata->AB_CxKeyCount += 4;

      FreeVec(idata->AB_CxKeys);
      idata->AB_CxKeys = tmp;
   }

   // -- remove old cx object, if any ----------------------------------------

   DeleteCxObjAll(idata->AB_CxKeys[x].cxobj);
   memset(idata->AB_CxKeys+x, 0, sizeof(AbCxCtrl));

   if (SW_STRISNULL(msg->Key) || getlent(msg->o, SWA_ObjError)) {
      DoMethod(msg->o, SWM_ClearObjError);
      SWDB_RET(AH_None);
   }
   
   // -- attach new object ---------------------------------------------------
   
   AttachCxObj(cx = CxFilter(msg->Key), CxSender(BrokerPort,
                                                 (LONG)idata->AB_CxNextID));
   AttachCxObj(cx, CxTranslate(NULL));
   AttachCxObj(Broker, cx);

   switch (CxObjError(cx)) {
     case 0:               break;
     case COERR_BADFILTER: SWERR(o, NULL, SWS_ABErr_BadHotKey, msg->Key, 0);
                           break;
     default:              SWERR(o, NULL, SWS_ABErr_CxErr, 0, 0);
                           break;
   }

   idata->AB_CxKeys[x].cxobj  = cx;
   idata->AB_CxKeys[x].Method = msg->Method;
   idata->AB_CxKeys[x].o      = msg->o;
   idata->AB_CxKeys[x].ID     = idata->AB_CxNextID++;

   DoMethod(o, SWM_Root_CheckErr, idata->SWA_AppBase_Win);

   SWDB_RET(idata->AB_CxKeys[x].ID);
} SDFT


/*****************************************************************************
 * Method: Remove hotkey                                                     *
 *****************************************************************************/

METHOD(SWM_AppBase_RemoveHotKey, SWP_AppBase_RemoveHotKey *)
{
   LONG Pos;
   
   if ((Pos = FindCxKey(idata, msg->ID)) >= 0) {
      DeleteCxObjAll(idata->AB_CxKeys[Pos].cxobj);
      memset(idata->AB_CxKeys+Pos, 0, sizeof(AbCxCtrl));
   }
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: write an icon                                                   *
 *****************************************************************************/

static void PutIcon(Object *o, SW_IDATA *idata, UBYTE *Name)
{
   if (!idata->SWA_AppBase_SaveProjIcons) return;

   SWPutIcon(o, idata, Name, idata->SWA_AppBase_AppName, DEFSAVEICON);
}


/*****************************************************************************
 * Function: Check IO                                                        *
 *****************************************************************************/

static ULONG RegIOChk(Object *o, SW_IDATA *idata, Object *Win)
{
   if (idata->SWA_AppBase_UnregSave) return SUCCESS;

   return DoMethod(o, SWM_AppBase_UnregMsg, GetS(SWS_L_UnregMsg), Win);
}


/*****************************************************************************
 * Function: Menu call: Open a proj file                                     *
 *****************************************************************************/

METHOD(SWM_AppBase_M_Open, SWP_AppBase_MenuFn *)
{
   ULONG rc;
   
   if (AskClob(o, idata, msg->Win, msg->Mode)  == FAILURE) SWDB_RET(FAILURE);
   if (RegIOChk(o, idata, msg->Win) == FAILURE)            SWDB_RET(FAILURE);

#  ifndef DEMOVERSION
   if ((rc = DoMethod(o, SWM_Root_M_OpenAssign,
                      SWT_Root_SL_Win,         msg->Win,
                      SWT_Root_SL_InitDrawer,  idata->SWA_AppBase_LastDir,
                      SWT_Root_SL_InitPattern, idata->SWA_AppBase_LastPatt,
                      TAG_DONE)) == SUCCESS)
      set(o, SWA_AppBase_ProjMod, FALSE);
   
   SWDB_RET(rc);
#  else
      SWDB_RET(FAILURE);
#  endif
} SDFT


/*****************************************************************************
 * Function: Menu call: save project file as                                 *
 *****************************************************************************/

METHOD(SWM_AppBase_M_SaveAs, SWP_AppBase_MenuFn *)
{
   ULONG rc;

   if (RegIOChk(o, idata, msg->Win) == FAILURE) SWDB_RET(FAILURE);

#  ifndef DEMOVERSION
   if ((rc = DoMethod(o, SWM_Root_M_SaveAs,
                      SWT_Root_SL_Win,           msg->Win,
                      SWT_Root_SL_InitFile,      idata->SWA_AppBase_LastFile,
                      SWT_Root_SL_InitDrawer,    idata->SWA_AppBase_LastDir,
                      SWT_Root_SL_InitPattern,   idata->SWA_AppBase_LastPatt,
                      SWT_Root_SL_AskOverwrite, !idata->SWA_AppBase_Clobber,
                      SWT_Root_SL_BackSuffix,    idata->SWA_AppBase_BackSuffix,
                      TAG_DONE)) == SUCCESS)
      DoMethod(o, SWM_AppBase_PostSaveProj, idata->SWA_AppBase_LastSaveFile);

   SWDB_RET(rc);

#  else
      SWDB_RET(FAILURE);
#  endif
} SDFT


/*****************************************************************************
 * Function: Menu call: save proj file to last saved                         *
 *****************************************************************************/

METHOD(SWM_AppBase_M_Save, SWP_AppBase_MenuFn *)
{
   if (RegIOChk(o, idata, msg->Win) == FAILURE) SWDB_RET(FAILURE);

#  ifndef DEMOVERSION
   if (!SW_STRISNULL(idata->SWA_AppBase_LastSaveFile))
      SWDB_RET(DoMethod(o, SWM_AppBase_SaveProj,
                        idata->SWA_AppBase_LastSaveFile, msg->Win));

   SWDB_RET(DoMethod(o, SWM_AppBase_M_SaveAs, msg->Win, msg->Mode));
#  else
      SWDB_RET(FAILURE);
#  endif
} SDFT


/*****************************************************************************
 * Function: Menu call: save proj as default                                 *
 *****************************************************************************/

METHOD(SWM_AppBase_M_SaveAsDef, SWP_AppBase_MenuFn *)
{
   if (RegIOChk(o, idata, msg->Win) == FAILURE) SWDB_RET(FAILURE);

#  ifndef DEMOVERSION
      SWDB_RET(DoMethod(o, SWM_AppBase_SaveProj, DEFSAVEFILE, msg->Win));
#  else
      SWDB_RET(FAILURE);
#  endif
} SDFT



/*****************************************************************************
 * Function: Menu call: new project                                          *
 *****************************************************************************/

METHOD(SWM_AppBase_M_New, SWP_AppBase_MenuFn *)
{
   if (AskClob(o, idata, msg->Win, msg->Mode) == FAILURE)   SWDB_RET(FAILURE);

   DoMethod(o, SWM_AppBase_ShowAppUI);

   SWDB_RET(DoMethod(o, SWM_AppBase_NewProj));
} SDFT


/*****************************************************************************
 * Function: Menu call: register program                                     *
 *****************************************************************************/

METHOD(SWM_AppBase_M_Register, SWP_AppBase_MenuFn *)
{
   DoMethod(idata->SWA_AppBase_Reg, SWM_RegisterUI,
            idata->SWA_AppBase_App,
            idata->SWA_AppBase_AppName,
            idata->SWA_AppBase_Cost1,
            idata->SWA_AppBase_Cost2);
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: Menu call: about MUI                                            *
 *****************************************************************************/

METHOD(SWM_AppBase_M_AboutMUI, SWP_AppBase_MenuFn *)
{
   DoMethod(idata->SWA_AppBase_App, MUIM_Application_AboutMUI, msg->Win);
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Menu call: hide window                                          *
 *****************************************************************************/

METHOD(SWM_AppBase_M_Hide, SWP_AppBase_MenuFn *)
{
   SWDB_RET(DoMethod(o, SWM_AppBase_HideAppUI));
} SDFT


/*****************************************************************************
 * Function: Menu call: restore default project                              *
 *****************************************************************************/

METHOD(SWM_AppBase_M_RestoreDf, SWP_AppBase_MenuFn *)
{
   SWDB_RET(DoMethod(o, SWM_AppBase_LoadProj, DEFSAVEFILE, msg->Win));
} SDFT


/*****************************************************************************
 * Function: Menu call: load last saved file                                 *
 *****************************************************************************/

METHOD(SWM_AppBase_M_LastSv, SWP_AppBase_MenuFn *)
{
#  ifndef DEMOVERSION
   if (SW_STRISNULL(idata->SWA_AppBase_LastSaveFile))
      SWDB_RET(SWERR(o, FAILURE, SWS_ABErr_NoSaveFile, 0, 0));
   else
      SWDB_RET(DoMethod(o, SWM_AppBase_LoadProj, idata->SWA_AppBase_LastSaveFile,
                        msg->Win));
#  else
   SWDB_RET(FAILURE);
#  endif
} SDFT


/*****************************************************************************
 * Function: Menu call: MUI config                                           *
 *****************************************************************************/

METHOD(SWM_AppBase_M_MUI, SWP_AppBase_MenuFn *)
{
   DoMethod(idata->SWA_AppBase_App, MUIM_Application_OpenConfigWindow);
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Menu call: quit                                                 *
 *****************************************************************************/

METHOD(SWM_AppBase_M_Quit, SWP_AppBase_MenuFn *)
{
   switch (idata->SWA_AppBase_ExitMode) {
     case SWV_AppBase_EM_Ask:
      if (msg->Mode == SWV_AppBase_Mode_Menu)
         if (AskClob(o, idata, msg->Win, SWV_AppBase_Mode_Menu) == FAILURE)
            SWDB_RET(FAILURE);
      break;
      
     case SWV_AppBase_EM_Save:
      if (!SW_STRISNULL(idata->SWA_AppBase_LastSaveFile))
         if (DoMethod(o, SWM_AppBase_M_Save, msg->Win) != SUCCESS)
#           ifndef DEMOVERSION
               SWDB_RET(FAILURE);
#           endif
      break;
      
     case SWV_AppBase_EM_NoSave: break;
   }
   
   DoMethod(idata->SWA_AppBase_App, MUIM_Application_ReturnID,
            MUIV_Application_ReturnID_Quit);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Process AppMessage                                                *
 *****************************************************************************/
   
METHOD(SWM_AppBase_AppMessage, SWP_AppMsg *)
{
   UBYTE FSpec[FMSIZE];
   
   if (msg->AMsg->am_NumArgs == 0) SWDB_RET(0);
   
   NameFromLock(msg->AMsg->am_ArgList[0].wa_Lock, FSpec, FMSIZE);
   AddPart(FSpec, msg->AMsg->am_ArgList[0].wa_Name,      FMSIZE);

   SWDB_RET(DoMethod(o, SWM_AppBase_LoadProj, FSpec, idata->SWA_AppBase_Win));
} SDFT


/*****************************************************************************
 * Method: uniconify window                                                  *
 *****************************************************************************/

METHOD(SWM_AppBase_UnIconify, Msg)
{
   set(idata->SWA_AppBase_App, MUIA_Application_Iconified, FALSE);
   
   SWDB_RET(DoMethod(o, SWM_AppBase_ShowAppUI));
} SDFT


/*****************************************************************************
 * Method: Save/Load (remember last saved file)                              *
 *****************************************************************************/

METHOD(SWM_Save, SWP_Save *)
{
   ULONG rc = DoSuperMethodA(cl, o, msg);

   if (rc == SUCCESS) nfset(o, SWA_AppBase_LastSaveFile, msg->Name);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Save/Load (remember last saved file)                              *
 *****************************************************************************/

METHOD(SWM_Load, SWP_Load *)
{
   ULONG rc = DoSuperMethodA(cl, o, msg);

   if (rc) nfset(o, SWA_AppBase_LastSaveFile, msg->Name);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Save/Load: get file/dir/pattern                                   *
 *****************************************************************************/
   
METHOD(SWM_Root_RequestFNData, SWP_Root_RequestFNData *)
{
   SetAttrs(o, MUIA_Group_Forward,   FALSE,
               SWA_AppBase_LastFile, msg->FSpec,
               SWA_AppBase_LastDir,  msg->Dir,
               SWA_AppBase_LastPatt, msg->Patt,
               TAG_DONE); 

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Menu call                                                         *
 *****************************************************************************/

METHOD(SWM_AppBase_MenuCall, SWP_AppBase_MenuCall *)
{
   ULONG rc = FAILURE;

   DoMethod(o, SWM_ClearObjError);

   switch (msg->Call) {
      SW_AppBase_MC_Proj;
      SW_AppBase_MC_Edit;
      SW_AppBase_MC_Settings;
   }
   
   DoMethod(o, SWM_Root_CheckErr, msg->Win);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Show App UI                                                       *
 *****************************************************************************/
   
METHOD(SWM_AppBase_ShowAppUI, Msg)
{
   if (idata->SWA_AppBase_Win)
      set(idata->SWA_AppBase_Win, MUIA_Window_Open, TRUE);

   SWDB_RET(idata->SWA_AppBase_Win);
} SDFT


/*****************************************************************************
 * Method: Hide App UI                                                       *
 *****************************************************************************/
   
METHOD(SWM_AppBase_HideAppUI, Msg)
{
   if (idata->SWA_AppBase_Win)
      set(idata->SWA_AppBase_Win, MUIA_Window_Open, FALSE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Return Edit UI                                                    *
 *****************************************************************************/
   
METHOD(SWM_AppBase_UI, Msg)
{
   SWDB_RET(SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o));
} SDFT


/*****************************************************************************
 * Method: Display unregistered message if necessary                         *
 *****************************************************************************/

METHOD(SWM_AppBase_UnregMsg, SWP_AppBase_UnregMsg *)
{
   ULONG rc;

   if (SWGetUL(idata->SWA_AppBase_Reg, SWA_Registered) == REG_Registered)
      SWDB_RET(SUCCESS);

   rc = MUI_Request(idata->SWA_AppBase_App, msg->Win, 0,
                    "NOTE!", GetS(SWS_B_Ok), msg->Mesg, NULL);

   if (rc == 1) SWDB_RET(FAILURE);

#  ifndef DEMOVERSION
   DoMethod(o, SWM_AppBase_M_Register, msg->Win);
#  endif

   SWDB_RET(FAILURE);
} SDFT


/*****************************************************************************
 * Function: Load a project file                                             *
 *****************************************************************************/

METHOD(SWM_AppBase_LoadProj, SWP_AppBase_LoadProj *)
{
   ULONG rc;
   Object *nf;

   if (AskClob(o, idata, msg->Win, SWV_AppBase_Mode_Menu) == FAILURE)
      SWDB_RET(FAILURE);

   if (strcmp(msg->ProjFile, "DEFSAVEFILE") != 0)
      if (RegIOChk(o, idata, msg->Win) == FAILURE) SWDB_RET(FAILURE);

   rc = DoMethod(o, SWM_Root_AssignSafe,
                 nf = (Object *)DoMethod(o, SWM_Load, msg->ProjFile), TRUE);

   if (nf) set(o, SWA_AppBase_ProjMod, FALSE);

   DoMethod(o, SWM_Root_CheckErr, msg->Win);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: Load a project file                                             *
 *****************************************************************************/

METHOD(SWM_AppBase_SaveProj, SWP_AppBase_SaveProj *)
{
   ULONG rc, AskOver;

   if (RegIOChk(o, idata, msg->Win) == FAILURE) SWDB_RET(FAILURE);

#  ifndef DEMOVERSION

   AskOver = strcmp(msg->ProjFile, idata->SWA_AppBase_LastSaveFile) ?
      !idata->SWA_AppBase_Clobber : FALSE;
   
   if ((rc = DoMethod(o, SWM_Root_M_Save,
                      SWT_Root_SL_Name,         msg->ProjFile,
                      SWT_Root_SL_Win,          msg->Win,
                      SWT_Root_SL_AskOverwrite, AskOver,
                      SWT_Root_SL_BackSuffix,   idata->SWA_AppBase_BackSuffix,
                      TAG_DONE)) == SUCCESS)
      DoMethod(o, SWM_AppBase_PostSaveProj, idata->SWA_AppBase_LastSaveFile);

   SWDB_RET(rc);
#  else
     SWDB_RET(FAILURE);
#  endif
} SDFT


/*****************************************************************************
 * Method: Init new project                                                  *
 *****************************************************************************/

METHOD(SWM_AppBase_NewProj, Msg)
{
   SetAttrs(o,
            MUIA_Group_Forward,       FALSE,
            SWA_AppBase_ProjMod,      FALSE,
            SWA_AppBase_LastFile,     NULL,
            SWA_AppBase_LastDir,      NULL,
            SWA_AppBase_LastPatt,     NULL,
            SWA_AppBase_LastSaveFile, NULL,
            TAG_DONE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: assign to the AppBase object                                      *
 *****************************************************************************/

METHOD(SWM_Root_AssignSafe, Msg)
{
   ULONG rc;

   DoMethod(o, SWM_AppBase_PreNewProj);

   rc = DoSuperMethodA(cl, o, msg);

   DoMethod(o, SWM_AppBase_PostNewProj);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: new project hook                                                  *
 *****************************************************************************/

METHOD(SWM_AppBase_PreNewProj, Msg)
{
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: new project hook                                                  *
 *****************************************************************************/

METHOD(SWM_AppBase_PostNewProj, Msg)
{
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: post save project hook                                            *
 *****************************************************************************/

METHOD(SWM_AppBase_PostSaveProj, SWP_AppBase_PostSaveProj *)
{
   set(o, SWA_AppBase_ProjMod, FALSE);
   PutIcon(o, idata, idata->SWA_AppBase_LastSaveFile);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: post save project hook                                            *
 *****************************************************************************/

METHOD(SWM_AppBase_Rexx_ProjFileName, Msg)
{
   set(idata->SWA_AppBase_App, MUIA_Application_RexxString,
       SW_STRDEF(idata->SWA_AppBase_LastSaveFile, ""));
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: PROJISMODIFIED                                                    *
 *****************************************************************************/

METHOD(SWM_AppBase_Rexx_ProjIsModified, Msg)
{
   set(idata->SWA_AppBase_App, MUIA_Application_RexxString,
       idata->SWA_AppBase_ProjMod ? "1" : "0");
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: process arexx command                                             *
 *****************************************************************************/

METHOD(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *)
{
   ULONG x;
   
   struct {
      STRPTR Cmd;
      ULONG  Method;
   } Cmds[] = {
      "OPENPROJECT_I",   SWM_AppBase_M_Open,
      "SAVEPROJECTAS_I", SWM_AppBase_M_SaveAs,
      "SAVETOLAST",      SWM_AppBase_M_Save,
      "SAVETODEF",       SWM_AppBase_M_SaveAsDef,
      "NEWPROJECT",      SWM_AppBase_M_New,
      "RESTOREDEF",      SWM_AppBase_M_RestoreDf,
      "LASTSAVED",       SWM_AppBase_M_LastSv,
      "UNICONIFY",       SWM_AppBase_UnIconify,
      "PROJFILENAME",    SWM_AppBase_Rexx_ProjFileName,
      "PROJISMODIFIED",  SWM_AppBase_Rexx_ProjIsModified,
      NULL,              NULL,
   };
   
   STRPTR cmd = ARG0(msg->Rx);
   
   if (SW_STRISNULL(cmd) || !msg->rc) SWDB_RET(SWV_AppBase_RexxNotFound);

   // -- ARexx menu commands -------------------------------------------------

   for (x=0; Cmds[x].Cmd; x++) {
      ULONG Len  = strlen(Cmds[x].Cmd),
            Mode = SWV_AppBase_Mode_Script;
      
      if (strncmp(cmd, Cmds[x].Cmd, Len) == 0 &&
          (cmd[Len] == ' ' || cmd[Len] == '\0')) {

         if (strcmp(cmd+Len+1, "FORCE") == 0)
            Mode = SWV_AppBase_Mode_Force;
         
         DoMethod(o, Cmds[x].Method, idata->SWA_AppBase_Win, Mode);

         *msg->rc = RC_OK;
         SWDB_RET(SWV_AppBase_RexxFound);
      }
   }

   // -- load project command ------------------------------------------------

   if (!strncmp(cmd, "LOADPROJECT ", 12)) {
      if (cmd[12] == '\0') SWDB_RET(SWV_AppBase_RexxNotFound);

      DoMethod(o, SWM_AppBase_LoadProj, cmd+12, idata->SWA_AppBase_Win);
      *msg->rc = RC_OK;
      SWDB_RET(SWV_AppBase_RexxFound);
   }

   // -- save project command ------------------------------------------------

   if (!strncmp(cmd, "SAVEPROJECT", 11) && (cmd[11] == ' ' || cmd[11] == '\0')) {
      if (cmd[11] == '\0' || cmd[12] == '\0') {
         DoMethod(o, SWM_AppBase_M_Save,
                  idata->SWA_AppBase_Win,
                  SWV_AppBase_Mode_Script);
         *msg->rc = RC_OK;
         SWDB_RET(SWV_AppBase_RexxFound);
      }

      DoMethod(o, SWM_AppBase_SaveProj, cmd+12, idata->SWA_AppBase_Win);
      *msg->rc = RC_OK;
      SWDB_RET(SWV_AppBase_RexxFound);
   }

   if (!strncmp(cmd, "SHUTDOWN", 8) && (cmd[8] == ' ' || cmd[8] == '\0')) {
      
      if (cmd[8] != '\0' && cmd[9] != '\0') {
         if (!strcmp(cmd+9, "ASK"))
            idata->SWA_AppBase_ExitMode = SWV_AppBase_EM_Ask;
         else if (!strcmp(cmd+9, "SAVE"))
            idata->SWA_AppBase_ExitMode = SWV_AppBase_EM_Save;
         else if (!strcmp(cmd+9, "NOSAVE"))
            idata->SWA_AppBase_ExitMode = SWV_AppBase_EM_NoSave;
      }

      DoMethod(o, SWM_AppBase_M_Quit, idata->SWA_AppBase_Win,
               SWV_AppBase_Mode_Script);
      
      *msg->rc = RC_OK;
      SWDB_RET(SWV_AppBase_RexxFound);
   }

   SWDB_RET(SWV_AppBase_RexxNotFound);
} SDFT
