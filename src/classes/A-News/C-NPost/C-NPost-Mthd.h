
// -- for testing purposes
// # define NPOST_NO_NETIO

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l), GetS(h)); }

/*****************************************************************************
 * Function: Create a popup group editor                                     *
 *****************************************************************************/

static void PopGrpList(SW_IDATA *idata, ULONG Attr, Object *AttrUI)
{
   // -- insert the popup post editor into our UI.  we don`t need to save -
   // -- these coz they insert themselves into the UI.  clever, eh? -------

   NewObject((struct IClass *)
             DoMethod(NN_PostList(idata), SWM_NPostList_NGClass), NULL,
             AppBaseDefTop(idata),
             SWA_NGList_Attr,   Attr,
             SWA_NGList_AttrUI, AttrUI,
             SWA_NGList_HelpL,  GetS(SWS_NG_H_LeftNG),
             SWA_NGList_HelpR,  GetS(SWS_NG_H_Right),
             TAG_DONE);
}

   
/*****************************************************************************
 * Method: Edit a post                                                       *
 *****************************************************************************/

METHOD(SWM_NPost_Edit, SWP_NPost_Edit *)
{
   Object *b_Queue = NULL, *b_Edit = NULL, *b_Canc, *b_Del = NULL, *EUI, *BGrp;
   static STRPTR Pages[5];
   static STRPTR ROPages[5];

   Object *b_Mode;
   
   // -- if window is open, move it to front ---------------------------------
   
   if (idata->UIWin) {
      DoMethod(idata->UIWin, MUIM_Window_ToFront);
      SWDB_RET(SUCCESS);
   }

   // -- otherwise, create a window ------------------------------------------

   Pages[0] = GetS(SWS_L_PEdit),
   Pages[1] = GetS(SWS_L_PView),
   Pages[2] = GetS(SWS_L_PViewAll),
   Pages[3] = GetS(SWS_L_PViewEnc),
   Pages[4] = NULL;
   
   ROPages[0] = GetS(SWS_L_PViewPost),
   ROPages[1] = GetS(SWS_L_PView),
   ROPages[2] = GetS(SWS_L_PViewAll),
   ROPages[3] = GetS(SWS_L_PViewEnc),
   ROPages[4] = NULL;
   
   idata->G_View = ListviewObject,
      MUIA_ShortHelp,            GetS(SWS_H_WList),
      MUIA_CycleChain,           1,
      MUIA_Listview_Input,       FALSE,
      MUIA_Listview_List, ObjListObject,
         ReadListFrame,
         SWA_ObjList_Object,      o,
         MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
         MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
      End,
   End;

   if (msg->ReadOnly) {
      EUI            = NULL;
      idata->UI_Post = NULL;
      BGrp = HGroup,
         MUIA_Group_SameSize, TRUE,
         Child, b_Canc         = MkBt(SWS_B_Canc,  SWS_H_Cancel),
         Child, b_Mode         = Cycle(ROPages),
      End;
   } else {
      EUI = idata->AttrUI  = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o);
      BGrp = ColGroup(3),
         MUIA_Group_SameSize, TRUE,
         Child, idata->UI_Post = MkBt(SWS_B_Post,  SWS_H_Post),
         Child, b_Edit         = MkBt(SWS_B_Edit,  SWS_H_Edit),
         Child, b_Queue        = MkBt(SWS_B_Queue, SWS_H_Queue),
         Child, b_Canc         = MkBt(SWS_B_Canc,  SWS_H_Cancel),
         Child, b_Del          = MkBt(SWS_B_Del,   SWS_H_Del),
         Child, b_Mode         = Cycle(Pages),
      End;

      // -- insert the popup post editor into our UI.  we don`t need to save -
      // -- these coz they insert themselves into the UI.  clever, eh? -------

      PopGrpList(idata, SWA_NPost_Grp,  EUI);
      PopGrpList(idata, SWA_NPost_FUps, EUI);
   }

   idata->UIWin = WindowObject,
      MUIA_Window_Title,       GetS(SWS_T_Edit),
      MUIA_Window_ID,          MAKE_ID('N','P','o','e'),
      MUIA_Window_IsSubWindow, TRUE,
      MUIA_Window_AppWindow,   TRUE,
      MUIA_Window_Menustrip,   MUI_MakeObject(MUIO_MenustripNM, EditMenu, 0),
      WindowContents, VGroup,
         MUIA_Background, MUII_GroupBack,
         msg->ReadOnly ? TAG_IGNORE : Child, EUI,
         Child, idata->G_Group = HGroup,
            MUIA_InnerTop,    0,
            MUIA_InnerBottom, 0,
            MUIA_InnerLeft,   0,
            MUIA_InnerRight,  0,
            Child, idata->G_Current = idata->TEditor = NN_EditorObject,
               AppBaseDefTop(idata),
               SWA_NNews_Editor_FSpec,
                  (STRPTR)DoMethod(o, SWM_NPost_FSpec,
                                   SWF_NPost_FSpec_Temp),
               SWA_NNews_Editor_CursorDisp,  TRUE,
               SWA_NNews_Editor_CmdObj,      o,
               SWA_NNews_Editor_AskClob,     TRUE,
               SWA_NNews_Editor_ReadOnly,    msg->ReadOnly,
               SWA_NNews_Editor_NoDelayInit, TRUE,
            End,
         End,
         Child, BGrp,
      End,
   End;

   DoMethod(idata->UIWin, MUIM_Notify, MUIA_Window_MenuAction,
            MUIV_EveryTime, o, 2, SWM_NPost_MenuCall, MUIV_TriggerValue);

   DoMethod(b_Mode, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            o, 2, SWM_NPost_EditMode, MUIV_TriggerValue);

   DoMethod(idata->UIWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            NN_App(idata), 7, MUIM_Application_PushMethod, o, 4,
            SWM_NPost_CloseEdit, FALSE, TRUE, idata->SWA_NPost_NewPost);

   if (b_Queue)
      DoMethod(b_Queue, NOTP, NN_App(idata), 7, MUIM_Application_PushMethod,
               o, 4, SWM_NPost_CloseEdit, FALSE, TRUE, FALSE);
   
   DoMethod(b_Canc,  NOTP, NN_App(idata), 7, MUIM_Application_PushMethod,
            o, 4, SWM_NPost_CloseEdit, msg->ReadOnly, FALSE,
            idata->SWA_NPost_NewPost);

   if (b_Edit)
      DoMethod(b_Edit,  NOTP, o, 2, SWM_NPost_Editor, FALSE);

   if (b_Del)
      DoMethod(b_Del,   NOTP, o, 2, SWM_NPost_IDel, msg->ReadOnly);

   if (idata->UI_Post)
      DoMethod(idata->UI_Post,  NOTP, o, 1, SWM_NPost_IPost);

   DoMethod(o, SWM_NPostList_OnOffLine,
            SWGetUL(NN_TCPList(idata), SWA_NTCPList_Online));

   DoMethod(NN_App(idata), OM_ADDMEMBER, idata->UIWin);
   set(idata->UIWin, MUIA_Window_Open, TRUE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Go on or offline                                                  *
 *****************************************************************************/

METHOD(SWM_NPostList_OnOffLine, SWP_NPostList_OnOffLine *)
{
#  ifndef NPOST_NO_NETIO
      if (idata->UI_Post)
         set(idata->UI_Post, MUIA_Disabled, !msg->State);
#  endif
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Save editor data to a file                                      *
 *****************************************************************************/

static ULONG SaveED(Object *o, SW_IDATA *idata, STRPTR Buff, STRPTR FSpec)
{
   ULONG Err = SWWriteFileString(FSpec, Buff, 0);

   if (Err)
      return SWERR_NM(o, FAILURE, SWDecodeErrStr(Err), FSpec, 0);

   return SUCCESS;
}

/*****************************************************************************
 * Method: save texteditor class data to file                                *
 *****************************************************************************/

METHOD(SWM_NPost_SaveEditData, SWP_NPost_SaveEditData *)
{
   STRPTR Buff, FSpec = msg->FSpec, WrappedFSpec;
   ULONG  rc, DefSave = FALSE;

   if (SW_STRISNULL(FSpec)) {
      FSpec   = (STRPTR)DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Temp);
      DefSave = TRUE;
   }

   if (Buff = (STRPTR)DoMethod(idata->TEditor, MUIM_TextEditor_ExportText))
      if ((rc = SaveED(o, idata, Buff, FSpec)) != SUCCESS)
         goto Done;

   FreeVec(Buff);
   Buff = NULL;

   WrappedFSpec = (STRPTR)DoMethod(o,SWM_NPost_FSpec,SWF_NPost_FSpec_Wrapped);
   
   if (msg->SaveWrapped)
      if (Buff = (STRPTR)DoMethod(o, SWM_NPost_Rewrap, 0))
         if ((rc = SaveED(o, idata, Buff, WrappedFSpec)) != SUCCESS)
            goto Done;

  Done:
   FreeVec(Buff);

   if (rc == SUCCESS && DefSave)
      DoMethod(o, SWM_NPost_UpdStats, FSpec);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Close edit window                                                 *
 *****************************************************************************/

METHOD(SWM_NPost_CloseEdit, SWP_NPost_CloseEdit *)
{
   ULONG rc, Force = msg->Force;

   if (!idata->UIWin) SWDB_RET(SUCCESS);

   // -- maybe ask about deleting --------------------------------------------

   if (msg->AskDel) {
      rc = MUI_Request(NN_App(idata), idata->UIWin, 0, NULL,
                       GetS(SWS_L_AskDel), GetS(SWS_L_WarnDel), NULL);

      if (rc == 1) {
         DoMethod(NN_App(idata), MUIM_Application_PushMethod, o, 2,
                  SWM_NPost_IDel, TRUE);
         SWDB_RET(SUCCESS);
      }

      Force = TRUE;
   }
   
   // -- maybe ask about aborting --------------------------------------------

   if (!Force && !msg->Save &&
       DoMethod(idata->TEditor, SWM_NNews_Editor_AskClob) != SUCCESS)
      SWDB_RET(FAILURE);

   DoMethod(idata->TEditor, SWM_NNews_Editor_HaltNotify);

   if (msg->Save) {
      DoMethod(o, SWM_NPost_SaveEditData, NULL, TRUE);
      DoMethod(o, SWM_NPost_CopyFile, SWT_NPost_CF_Del, TRUE, TAG_DONE);
   } else
      DoMethod(o, SWM_NPost_UpdDisp,
               DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Perm));

   DeleteFile((STRPTR)DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Temp));

   idata->SWA_NPost_Mod = FALSE;

   DoMethod(o, SWM_Root_CheckErr, idata->UIWin);

   if (idata->AttrUI) {
      SWKillAttrUI(SW_ATTRMAP, idata->AttrUI, o);
      idata->AttrUI = NULL;
   }

   DoMethod(o, SWM_NPost_EditMode, 256);
   
   set(idata->UIWin, MUIA_Window_Open, FALSE);

   DoMethod(NN_App(idata), OM_REMMEMBER, idata->UIWin);
   MUI_DisposeObject(idata->UIWin);
   MUI_DisposeObject(idata->TEditor);
   MUI_DisposeObject(idata->G_View);

   idata->G_Group   = 
   idata->G_View    = 
   idata->G_Current = 
   idata->UI_Post   =
   idata->TEditor   =
   idata->UIWin     = NULL;
   idata->SWA_NPost_NewPost = FALSE;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Find indent level                                               *
 *****************************************************************************/

static LONG IndentLevel(STRPTR s, LONG PrevIL)
{
   LONG  l = 0, found, x = 0, y, MaxPL = 6;
   STRPTR qq = ">|:";

   if (strspn(s, " \t") == strcspn(s, "\n")) return PrevIL;
   
   do {
      y = found = 0;
      for (; s[x] && s[x] != '\n' && y<=MaxPL && !found; x++, y++)
         if (strchr(qq, s[x]))
            l++, found=TRUE;
   } while (found);

   return l;
}


/*****************************************************************************
 * Function: Initialize post analysis data                                   *
 *****************************************************************************/

static void InitPostData(SW_IDATA *idata)
{
   idata->Post_HasNonASCII = FALSE;
   idata->Post_CharSet     = NULL;
   idata->Post_Encoding    = NULL;
   idata->Post_ContentType = NULL;
}


/*****************************************************************************
 * Function: Analyze post body, set Post_* instance data appropriately       *
 *****************************************************************************/

static ULONG AnalyzePostData(Object *o, SW_IDATA *idata, STRPTR Line)
{
   if (idata->Post_HasNonASCII || !Line) return SUCCESS;

   for (; *Line; Line++)
      if (QPNeedsEncoding(*Line) && !(*Line == '\n' || *Line == '=')) {
         ULONG EncBody = NN_EncBody(idata);

         if (EncBody == SWV_NNews_Enc_7bit)
            return SWERR(o, FAILURE, SWS_Err_Body8bit, 0, 0);
         
         idata->Post_HasNonASCII = TRUE;
         idata->Post_CharSet     = (STRPTR)DoMethod(idata->SWA_AppBase_TopObj,
                                                    SWM_NNews_CharSetName);
         idata->Post_ContentType = "text/plain";
         idata->Post_Encoding    = EncBody == SWV_NNews_Enc_QP ? "quoted-printable" :
                                   "8bit";

         return SUCCESS;
      }

   return SUCCESS;
}


/*****************************************************************************
 * Function: check out a file - measure longest line, # of quoted lines, etc *
 *****************************************************************************/

static ULONG CheckFile(Object *o, SW_IDATA *idata, STRPTR FSpec,
                       ULONG *Line, ULONG *BodyLines, ULONG *QuotePct)
{
   ULONG Len, Longest = 0, LPos = 0, QuotLines = 0, ILevel = 0;
   BPTR fp;
   STRPTR Buff  = AV(LBSize);

   if (Line)   *Line   = 0;

   if (!(fp = Open(FSpec, MODE_OLDFILE))) {
      FV(Buff);
      return 0;
   }

   while (FGets(fp, Buff, LBSize)) {
      if (Len = strlen(Buff))
         Buff[--Len] = '\0';  // nuke newline

      if (AnalyzePostData(o, idata, Buff) != SUCCESS) {
         Longest = 0;
         goto Done;
      }

      LPos++;

      if (ILevel = IndentLevel(Buff, ILevel))
         QuotLines++;
      
      if (Len > Longest) {
         Longest = Len;
         if (Line) *Line = LPos;
      }
   }

  Done:
   Close(fp);
   FV(Buff);

   if (QuotePct)  *QuotePct  = QuotLines * 100 / max(LPos, 1);
   if (BodyLines) *BodyLines = LPos;
   
   return Longest;
}


/*****************************************************************************
 * Function: See if a string contains the word test                          *
 *****************************************************************************/

LONG HasSubStr(STRPTR s, STRPTR Sub)
{
   ULONG slen = strlen(Sub);

   if (SW_STRISNULL(s))
      return FALSE;
   
   for (; *s; s++)
      if (Strnicmp(s, Sub, slen) == 0)
         return TRUE;

   return FALSE;
}



/*****************************************************************************
 * Function: check send command                                              *
 *****************************************************************************/

static Object *CheckSendCmd(Object *o, SW_IDATA *idata, ULONG *msg)
{
   Object *DSrv,
           *Srv = GetTagOB(SWT_NPost_SrvList, NN_ServList(idata), msg+1);
   STRPTR FSpec, host = NN_UserEmail(idata), htmp;
   ULONG Longest, Line, rc, BodyLines, QuotePct;
   Object *Win = idata->UIWin ? idata->UIWin : _win(NN_PostList(idata)),
          *App = NN_App(idata);

   InitPostData(idata);

   idata->SWA_NPost_PostedOK = FALSE;

   if (!Srv) return NULL;

   // -- find the default server ---------------------------------------------

   DSrv = (Object *)DoMethod(Srv, SWM_NServerList_GetDefault,
                             SWT_NServerList_ServerName,idata->SWA_NPost_Server,
                             TAG_DONE);

   if (!DSrv)
      return (Object *)DoMethod(o, SWM_Root_AdoptObjError, Srv, 0);

   // -- make sure we have some groups ---------------------------------------
   
   if (SW_STRISNULL(idata->SWA_NPost_Grp))
      return (Object *)SWERR(o, NULL, SWS_Err_NoGroups, 0, 0);

   // -- update stats & check file size --------------------------------------
   
   if (DoMethod(o, SWM_NPost_UpdStats, NULL) != SUCCESS ||
       idata->SWA_NPost_Bytes == 0)
      return (Object *)SWERR(o, NULL, SWS_Err_BadBody, 0, 0);

   // -- check user email addr -----------------------------------------------

   if (strchr(host, ' ') || strchr(host, '>') || strchr(host, '<') ||
       strchr(host, '(') || strchr(host, ')'))
      return (Object *)SWERR(o, NULL, SWS_Err_BadEmail, host, 0);

   if (!(htmp = strchr(host, '@')) || htmp == host)
      return (Object *)SWERR(o, NULL, SWS_Err_BadEmail, host, 0);

   if (strchr(htmp+1, '@'))
      return (Object *)SWERR(o, NULL, SWS_Err_BadEmail, host, 0);

   // -- make sure we have a subject -----------------------------------------
   
   if (SW_STRISNULL(idata->SWA_NPost_Subj) ||
       !strcmp(idata->SWA_NPost_Subj, SWV_NArticle_ReStr))
      return (Object *)SWERR(o, NULL, SWS_Err_NoSubj, 0, 0);

   // -- check for sig file --------------------------------------------------

   if (!SW_STRISNULL(idata->SWA_NPost_SigFile)) {
      if (!SWFileExists(idata->SWA_NPost_SigFile))
         return (Object *)SWERR(o,NULL,SWS_Err_NoSig,idata->SWA_NPost_SigFile,0);

      if (!CheckFile(o, idata, idata->SWA_NPost_SigFile, NULL, NULL, NULL))
         return NULL;
   }

   // -- check for body ------------------------------------------------------

   FSpec = (STRPTR)DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Wrapped);

   if (!SWFileExists(FSpec))
      return (Object *)SWERR(o, NULL, SWS_Err_BadBody, 0, 0);

   // -- check longest line --------------------------------------------------
   
   if (!(Longest = CheckFile(o, idata, FSpec, &Line, &BodyLines, &QuotePct)))
      return NULL;

   if (Longest > SWV_MaxPostLineLen)
      return (Object *)SWERR(o, NULL, SWS_Err_LineTooLong, Line, Longest);

   if (Longest > SWV_WarnPostLineLen) {
      rc = MUI_Request(App, Win, 0, NULL,
                       GetS(SWS_L_YN), GetS(SWS_Warn_LineTooLong), NULL);

      if (!rc)
         return (Object *)SWERR(o, NULL, SWS_Err_PostAborted, 0, 0);
   }

   // -- check for percentage quoted text ------------------------------------

   if (BodyLines > SWV_WarnQuoteMinLines &&
       QuotePct  > SWV_WarnQuotePct) {
      
      rc = MUI_Request(App, Win, 0, NULL,
                       GetS(SWS_L_YN), GetS(SWS_Warn_QuotePct), NULL);

      if (!rc)
         return (Object *)SWERR(o, NULL, SWS_Err_PostAborted, 0, 0);
   }

   // -- check for test posts ------------------------------------------------

   if (HasSubStr(idata->SWA_NPost_Subj, "test") &&
       !HasSubStr(idata->SWA_NPost_Grp, "test")) {

      rc = MUI_Request(App, Win, 0, NULL,
                       GetS(SWS_Warn_TestPost_Bt), GetS(SWS_Warn_TestPost), NULL);

      if (!rc)
         return (Object *)SWERR(o, NULL, SWS_Err_PostAborted, 0, 0);

      if (rc == 2)
         set(o, SWA_NPost_Grp, "alt.test");
   }


   // -- check for newsgroup count -------------------------------------------
   
   {
      LONG   GroupCount = 0;
      STRPTR g  = idata->SWA_NPost_Grp,
             NG = AV(strlen(g)+2),
             s  = NG;

      if (!NG) return (Object *)SWERR(o, NULL, SWS_Err_Malloc, 0, 0);

      while (g) {
         if (*g == ',' || *g == '\0')
            GroupCount++;

         if (*g == ',' && (g[1] == '\0' || g[1] == ','))
            return (Object *)SWERR(o, NULL, SWS_Err_BadGroups, 0, 0);

         if (*g != ' ' && *g != '\t')
            *s++ = *g;

         if (!*g++) break;
      }

      if (strcmp(NG, idata->SWA_NPost_Grp) != 0)
         set(o, SWA_NPost_Grp, NG);

      FV(NG);
      
      // -- make sure we have some groups ------------------------------------
   
      if (GroupCount <= 0)
         return (Object *)SWERR(o, NULL, SWS_Err_NoGroups, 0, 0);

      // -- See if we have a lot of groups -----------------------------------

      if (GroupCount >= SWV_WarnLotaGroups) {
         rc = MUI_Request(App, Win, 0, NULL,
                          GetS(SWS_L_YN), GetS(SWS_Warn_GroupCount), GroupCount);

         if (!rc)
            return (Object *)SWERR(o, NULL, SWS_Err_PostAborted, 0, 0);
      }
   }


   // -- abort any pending cmd -----------------------------------------------

   DoMethod(o, SWM_NPost_Abort);

   idata->CmdObj    = GetTagOB(SWT_NPost_CmdObj,    NULL, msg+1);
   idata->CmdMethod = GetTagUL(SWT_NPost_CmdMethod, NULL, msg+1);
   idata->CmdData   = GetTagUL(SWT_NPost_CmdData,   NULL, msg+1);
   
   return DSrv;
}


/*****************************************************************************
 * Function: Register beginning of command                                   *
 *****************************************************************************/

static void MsgBegin(Object *o, SW_IDATA *idata, SWP_NServer_StatusMsg *msg)
{
   if (!NSR_SBegin(msg)) return;
   
   idata->CmdTCP = (Object *)DoMethod(idata->CmdSrv = NSR_Srv(msg),
                                      SWM_NServer_GetTCP,
                                      idata->CmdID  = NSR_ID(msg));
}


/*****************************************************************************
 * Function: Register end of command                                         *
 *****************************************************************************/

static void MsgEnd(Object *o, SW_IDATA *idata, SWP_NServer_StatusMsg *msg)
{
   if (!NSR_SAnyEnd(msg)) return;

   if (idata->CmdObj)
      DoMethod(idata->CmdObj, idata->CmdMethod,
               idata->SWA_NPost_PostedOK ? SWF_NPost_State_End
                                         : SWF_NPost_State_Abort);

   DoMethod(o, SWM_Root_AdoptObjError, idata->CmdSrv, 0);

   idata->CmdSrv = NULL;
   idata->CmdID  = -1;
   idata->CmdTCP = NULL;
}


/*****************************************************************************
 * Method: Post a post (Interactive)                                         *
 *****************************************************************************/

METHOD(SWM_NPost_IPost, Msg)
{
   ULONG rc;

   DoMethod(o, SWM_NPost_SaveEditData, NULL, TRUE);
   
   if ((rc = DoMethod(o, SWM_NPost_CopyFile, TAG_DONE)) != SUCCESS)
      goto Done;

   rc = DoMethod(o, SWM_NPost_Post,
                 SWT_NPost_CmdObj,    o,
                 SWT_NPost_CmdMethod, SWM_NPost_IPost_End,
                 TAG_DONE);

   if (rc == SUCCESS)
      set(idata->UIWin, MUIA_Window_Sleep, TRUE);

  Done:
   DoMethod(o, SWM_Root_CheckErr, idata->UIWin);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Post a post (Interactive) end process                             *
 *****************************************************************************/

METHOD(SWM_NPost_IPost_End, SWP_NPost_CmdMsg *)
{
   if (!DoMethod(o, SWM_Root_CheckErr, idata->UIWin)) {
      MUI_Request(NN_App(idata), idata->UIWin, 0, NULL,
                  GetS(SWS_L_OK), GetS(SWS_L_PostSucc), NULL);

      // -- either archive or delete the sent post ---------------------------
      
      if (NN_MaxSavedPosts(idata))
         DoMethod(o, SWM_NPost_Archive);
      else
         DoMethod(o, SWM_NPost_IDel, TRUE);
   }
   
   set(idata->UIWin, MUIA_Window_Sleep, FALSE);

   SWDB_RET(0);
} SDFT



/*****************************************************************************
 * Function: Return either original string, or qp'ed copy if required        *
 *****************************************************************************/

static STRPTR GenPostLine(SW_IDATA *idata, STRPTR s, STRPTR *Out)
{
   // -- no encoding, or 8-bit  ----------------------------------------------
   
   if (!idata->Post_Encoding || idata->Post_Encoding[0] == '8') {
      *Out = s;
      return "";  // return null string to signify we`re done
   }

   // -- only other choice right now is quoted-printable ---------------------

   if (!idata->QPLineBuff)
      if (!(idata->QPLineBuff = AV(SWV_QPEncStr_BuffLen + 4)))
         return NULL;
      
   return SWQPEncodeStr(s, *Out = idata->QPLineBuff);
}


/*****************************************************************************
 * Function: Send single line to server                                      *
 *****************************************************************************/

static ULONG SendLine(Object *o, SW_IDATA *idata, STRPTR Str,
                      ULONG *Line, ULONG Lines)
{
   // -- escape single dot (which would otherwise end post) ------------------

   if (Str[0] == '.' && (Str[1] == '\r' || Str[1] == '\n'))
      Str = "..\r\n";

   if (Line) {
      set(o, SWA_NPost_Pct, ++(*Line) * 100 / (Lines+1));

      if ((*Line & 0xf) == 0)
         DoMethod(NN_App(idata), MUIM_Application_CheckRefresh);
   }

#  ifdef NPOST_NO_NETIO
      PutStr(Str);
      return SUCCESS;
#  else
      return (ULONG)(DoMethod(idata->CmdTCP, SWM_NTCP_SendStr, Str) ?
                     FAILURE : SUCCESS);
#  endif // NPOST_NO_NETIO
}


/*****************************************************************************
 * Function: Send File to server                                             *
 *****************************************************************************/

static ULONG SendFile(Object *o, SW_IDATA *idata,
                      STRPTR FSpec, ULONG *Line, ULONG Lines)
{
   BPTR  fp;
   ULONG rc = SUCCESS, Len;
   STRPTR Buff  = AV(LBSize), Out, Next;

   if (!Buff) return FAILURE;
   
   if (!(fp = Open(FSpec, MODE_OLDFILE))) {
      rc = FAILURE;
      goto Done;
   }

   Buff[0] = '\0';

   // -- send the file`s data ------------------------------------------------
   
   while (FGets(fp, Buff, LBSize-2)) {
      Buff[LBSize-2] = '\0';
      if (Len = strlen(Buff)) {
         // -- tromp any newline and cr for now - we`ll put it back shortly --
         
         Buff[--Len] = '\0';
         if (Len && Buff[Len-1] == '\r') Buff[--Len] = '\0';

         Out = Buff;
         while (TRUE) {
            if (!(Next = GenPostLine(idata, Out, &Out)))
               break;

            strcat(Out, "\r\n");  // terminate line properly
            
            if ((rc = SendLine(o, idata, Out, Out == Buff ? Line : NULL, Lines))
                != SUCCESS)
               goto Done;
            
            if (!(*(Out = Next))) break;
         }
      }
   }

   // -- make sure we`re sending a trailing newline --------------------------

   if (Buff[0] && Buff[strlen(Buff)-1] != '\n')
      if ((rc = SendLine(o, idata, "\r\n", Line, Lines)) != SUCCESS)
         goto Done;

  Done:
   if (fp) Close(fp);
   FV(Buff);
   
   return rc;
}


/*****************************************************************************
 * Function: Send signature file                                             *
 *****************************************************************************/

static ULONG SendSig(Object *o, SW_IDATA *idata, ULONG *Line, ULONG Lines)
{
   if (!SW_STRISNULL(idata->SWA_NPost_SigFile))
      return SendFile(o, idata, idata->SWA_NPost_SigFile, Line, Lines);
   else
      return SUCCESS;
}


/*****************************************************************************
 * Function: Send tagline                                                    *
 *****************************************************************************/

static ULONG SendTagLine(Object *o, SW_IDATA *idata, ULONG *Line, ULONG Lines)
{
   ULONG rc = SUCCESS, x, y;
   
   if (SW_STRISNULL(idata->SWA_NPost_TagLine))
      return SUCCESS;

   for (x=y=0; idata->SWA_NPost_TagLine[x]; x++)
      if (idata->SWA_NPost_TagLine[x] == '\n') {
         idata->SWA_NPost_TagLine[x] = '\0';

         if ((rc = SendLine(o, idata, idata->SWA_NPost_TagLine+y, Line, Lines))
             != SUCCESS) {
            idata->SWA_NPost_TagLine[x] = '\n';          
            break;
         }
      
         if ((rc = SendLine(o, idata, "\r\n", NULL, Lines)) != SUCCESS) {
            idata->SWA_NPost_TagLine[x] = '\n';          
            break;
         }
      
         idata->SWA_NPost_TagLine[x] = '\n';
         y = x + 1;
      }

   return rc;
}


/*****************************************************************************
 * Function: Send body                                                       *
 *****************************************************************************/

static ULONG SendBody(Object *o, SW_IDATA *idata, ULONG *Line, ULONG Lines)
{
   STRPTR FSpec;

   FSpec = (STRPTR)DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Wrapped);

   return SendFile(o, idata, FSpec, Line, Lines);
}


/*****************************************************************************
 * Function: Determine if a pattern is matched by a group list               *
 *****************************************************************************/

static ULONG IsGrpMatch(Object *o, SW_IDATA *idata,
                        Object *OptHdr, STRPTR PatBuf)
{
   STRPTR p, Next,
          Hdr    = SWGetStr(OptHdr, SWA_NHdrList_Hdr),
          Val    = SWGetStr(OptHdr, SWA_NHdrList_Val),
          GrpPat = SWGetStr(OptHdr, SWA_NHdrList_Grp),
          Groups = SWGetStr(o, SWA_NPost_Grp);
   ULONG  Mode   = SWGetUL(OptHdr, SWA_NHdrList_Mode),
          Match  = Mode == SWV_NHdrList_Any ? FALSE : TRUE,
          Done   = FALSE;

   if (SW_STRISNULL(Groups) || SW_STRISNULL(Hdr) || SW_STRISNULL(Val) ||
       !idata->SWA_NPost_UseCustHdrs)
      return FALSE;

   if (ParsePattern(SW_STRDEF(GrpPat, "*"), PatBuf, PBSize) < 0)
      return FALSE;

   for (p=Groups; !Done && (Next = p ? strchr(p, ',') : NULL, p); p = Next) {
      if (Next) Next[0] = '\0';

      if (MatchPattern(PatBuf, p)) {
         if (Mode == SWV_NHdrList_Any)
            Match = TRUE,  Done = TRUE;
      } else {
         if (Mode == SWV_NHdrList_All)
            Match = FALSE, Done = TRUE;
      }
      
      if (Next) (Next++)[0] = ',';
   }

   return Match;   
}


# define NonAscii(x)   (((x) > 127 || (x) < 32) && (x)!='\r' && (x)!='\n')
# define MustEncode(x) (NonAscii(x) || (x)=='?' || (x)=='=')

/*****************************************************************************
 * Function: Search for non-ASCII chars                                      *
 *****************************************************************************/

static LONG HasNonAscii(STRPTR s, STRPTR term)
{
   LONG x, NeedsEncoding = FALSE;

   for (x=0; s[x] && !strchr(term, s[x]); x++)
      if (NonAscii(s[x]))
         NeedsEncoding = TRUE;

   return NeedsEncoding;
}


/*****************************************************************************
 * Function: Add charset begin to string                                     *
 *****************************************************************************/

static ULONG AddISOBegin(SW_IDATA *idata, STRPTR s)
{
   sprintf(s, "=?%s?Q?",
           (STRPTR)DoMethod(idata->SWA_AppBase_TopObj,
                            SWM_NNews_CharSetName));

   return strlen(s);
}


/*****************************************************************************
 * Function: Add charset end to string                                       *
 *****************************************************************************/

static ULONG AddISOEnd(STRPTR s) { return strlen(strcpy(s, "?=")); }


/*****************************************************************************
 * Function: Find end of ISO word                                            *
 *****************************************************************************/

LONG FindISOEnd(STRPTR s, LONG pos, STRPTR EncBrChrs)
{
   LONG CCount = 0, LastPos = -1;

   // -- find last non-ASCII char position -----------------------------------

   for (; s[pos] && CCount < 64 && !strchr(EncBrChrs, s[pos]); pos++)
      if (MustEncode(s[pos]))
         CCount += 3, LastPos = pos;
      else
         CCount++;

   if (LastPos < 0) return pos;
   
   // -- find place to end encoding ------------------------------------------
   
   for (pos=LastPos; s[pos] && !strchr(EncBrChrs,s[pos]) && s[pos]!=' '; pos++);

   return pos;
}


/*****************************************************************************
 * Function: Find start of ISO word                                          *
 *****************************************************************************/

LONG FindISOBegin(STRPTR s, LONG pos, STRPTR EncBrChrs)
{
   LONG First = pos;
   
   // -- find first non-ASCII char position ----------------------------------

   for (; s[pos]; pos++)
      if (NonAscii(s[pos]))
         break;

   // -- if none up to end of string, return last pos ------------------------
   
   if (!s[pos]) return pos;

   // -- else return beginning of word ---------------------------------------
   
   for (; pos >= First; pos--)
      if (s[pos] == ' ' || strchr(EncBrChrs, s[pos]))
         break;

   return pos + 1;
}


/*****************************************************************************
 * Function: Copy header data, perhaps using ISO encoding                    *
 *****************************************************************************/

static STRPTR FixISOHdr(SW_IDATA *idata, STRPTR s, STRPTR tmp, STRPTR EncBrChrs)
{
   LONG x, y, NeedSp, BeginPos, EndPos;
   TEXT  HexDigits[] = "0123456789ABCDEF";

   // -- if we need no encoding, just return it as-is ------------------------

   if (!HasNonAscii(s, "") || NN_EncHdrs(idata) == SWV_NNews_Enc_8bit)
      return s;

   // -- QP encode the headers -----------------------------------------------
   
   strcpy(tmp, s);
   
   // -- skip the field name -------------------------------------------------

   for (x=0; tmp[x] != ':'; x++);
   if (tmp[x++] != ':') return s;
   if (tmp[x++] != ' ') return s;

   // -- copy encoded words back into string ---------------------------------

   y        = x;
   NeedSp   = FALSE;
   
   while (TRUE) {
      BeginPos = FindISOBegin(tmp, x, EncBrChrs);
      
      for (; tmp[x] && x < BeginPos; )     // -- copy up to word beginning
         s[y++] = tmp[x++];

      if (!tmp[x]) break;

      y += AddISOBegin(idata, s+y);

      if (NeedSp) s[y++] = '_', NeedSp = FALSE;

      EndPos = FindISOEnd(tmp, x, EncBrChrs);

      while (tmp[x] && x<EndPos) {
         if (tmp[x] == ' ')
            s[y++] = '_', x++;
         else if (MustEncode(tmp[x]))
            s[y++] = '=',
            s[y++] = HexDigits[tmp[x] >> 4],
            s[y++] = HexDigits[tmp[x++] & 0x0f];
         else
            s[y++] = tmp[x++];
      }

      y += AddISOEnd(s+y);
   }

   s[y] = '\0';

   return s;
}


/*****************************************************************************
 * Function: Create header lines + blank separator line                      *
 *****************************************************************************/

static STRPTR *CreateHdrLines(Object *o, SW_IDATA *idata, ULONG *Count,
                              STRPTR LineTerm, ULONG Encode)
{
   ULONG  Lines = 0, x, LTLen = strlen(LineTerm)+1;
   STRPTR PatBuf  = AV(PBSize),
          LBuff   = AV(LBSize),
          TmpBuff = AV(LBSize),
          Val;
   Object *HdrList = NN_HdrList(idata), *OptHdr;
   STRPTR *Hdrs = NULL;

   if (NN_RegVer(idata) < 10)
      goto Done;
   
   if (!Count || !PatBuf || !LBuff) 
      goto Done;

   *Count = 0;

   // -- count fixed header lines --------------------------------------------

   for (x=0; PHeaders[x].Header; x++) {
      Val = SWGetStr(o, PHeaders[x].Attr);

      if (PHeaders[x].Header == SWF_NAHeader_MsgID && !NN_GenMsgID(idata))
         continue;

      if (!SW_STRISNULL(Val))
         Lines++;
   }

   // -- count optional header lines -----------------------------------------
   
   if (HdrList)
      SW_MUIListLoop(HdrList, x, OptHdr, TRUE)
         if (IsGrpMatch(o, idata, OptHdr, PatBuf))
            Lines++;

   *Count = Lines + 1;
   Lines  = 0;
   
   if (!(Hdrs = (STRPTR *)AV(*Count * sizeof(STRPTR))))
      goto Done;

   memset(Hdrs, 0, *Count * sizeof(STRPTR));

   // -- generate fixed headers ----------------------------------------------

   for (x=0; PHeaders[x].Header; x++) {
      if (PHeaders[x].Header == SWF_NAHeader_MsgID && !NN_GenMsgID(idata))
         continue;

      Val = SWGetStr(o, PHeaders[x].Attr);
      
      if (!SW_STRISNULL(Val)) {
         sprintf(LBuff, "%s %s", PHeaders[x].Header, Val);
         if (Encode)
            FixISOHdr(idata, LBuff, TmpBuff, "<>()\"");
         strcat(LBuff, LineTerm);
         Hdrs[Lines] = AV(strlen(LBuff)+LTLen);
         strcpy(Hdrs[Lines++], LBuff);
      }
   }

   // -- send optional headers -----------------------------------------------

   if (HdrList)
      SW_MUIListLoop(HdrList, x, OptHdr, TRUE) {
         STRPTR Hdr = SWGetStr(OptHdr, SWA_NHdrList_Hdr),
                Val = SWGetStr(OptHdr, SWA_NHdrList_Val);

         Hdr = VL_Alloc(o, idata, Hdr);
         Val = VL_Alloc(o, idata, Val);

         if (IsGrpMatch(o, idata, OptHdr, PatBuf)) {
            sprintf(LBuff,
                    Hdr[strlen(Hdr)-1] == ':' ? "%s %s" : "%s: %s",
                    Hdr, Val);

            if (Encode)
               FixISOHdr(idata, LBuff, TmpBuff, "<>()\"");
            
            strcat(LBuff, LineTerm);
            Hdrs[Lines] = AV(strlen(LBuff)+LTLen);
            strcpy(Hdrs[Lines++], LBuff);
         }
         
         VL_Free(o, idata, Hdr);
         VL_Free(o, idata, Val);
      }

   Hdrs[Lines] = AV(LTLen);
   strcpy(Hdrs[Lines], LineTerm);

  Done:
   FV(PatBuf);
   FV(LBuff);
   FV(TmpBuff);
   
   return Hdrs;
}


/*****************************************************************************
 * Function: Free header lines                                               *
 *****************************************************************************/

static void FreeHdrLines(SW_IDATA *idata, STRPTR *Hdrs, ULONG Count)
{
   ULONG x;

   if (!Hdrs) return;
   
   for (x=0; x<Count; x++)
      FV(Hdrs[x]);

   FV(Hdrs);
}


/*****************************************************************************
 * Function: Send post data to server                                        *
 *****************************************************************************/

static ULONG SendPost(Object *o, SW_IDATA *idata)
{
   ULONG  x, rc, Count;
   ULONG  Line = 0, Lines = idata->SWA_NPost_Lines + 1;
   STRPTR *Hdrs;

   // -- create header lines -------------------------------------------------

   if (!(Hdrs = CreateHdrLines(o, idata, &Count, "\r\n", TRUE))) {
      rc = FAILURE;
      goto Done;
   }

   Lines += Count;

   if (!SW_STRISNULL(idata->SWA_NPost_SigFile))
      Lines += 6; // just guess.

   // -- send header lines ---------------------------------------------------

   for (x=0; x<Count; x++)
      if ((rc = SendLine(o, idata, Hdrs[x], &Line, Lines)) != SUCCESS)
         goto Done;

   // -- free header crap ----------------------------------------------------

   FreeHdrLines(idata, Hdrs, Count);

#  ifdef POST_BETA_MSG
      if ((rc = SendLine(o, idata, SWV_NPost_BetaMsg, &Line, Lines)) != SUCCESS)
         goto Done;
#  endif
   
   // -- send the post body --------------------------------------------------

   if ((rc = SendBody(o, idata, &Line, Lines)) != SUCCESS)
      goto Done;

   // -- send the signature file & tagline -----------------------------------

   if (SW_STRISNULL(idata->SWA_NPost_TagLine))
      DoMethod(o, SWM_NPost_GetTagLine, SWV_NPost_GTL_Random);

   if (!SW_STRISNULL(idata->SWA_NPost_SigFile) ||
       !SW_STRISNULL(idata->SWA_NPost_TagLine)) {
      if (NN_UseSS(idata))
         if ((rc = SendLine(o, idata, "-- \r\n", &Line, Lines)) != SUCCESS)
            goto Done;

      if ((rc = SendSig(o, idata, &Line, Lines)) != SUCCESS)
         goto Done;

      if ((rc = SendTagLine(o, idata, &Line, Lines)) != SUCCESS)
         goto Done;
   }

  Done:
   return rc;
}


/*****************************************************************************
 * Method: Post a post                                                       *
 *****************************************************************************/

METHOD(SWM_NPost_Post, ULONG *)
{
   ULONG  rc;
   Object *Srv;

   if (!(Srv = CheckSendCmd(o, idata, msg)))
      SWDB_RET(FAILURE);

   AB_SetMod(idata);

#  ifdef NPOST_NO_NETIO
      rc = SUCCESS;
      PutStr(SWV_NServer_PostCmd);
      DoMethod(o, SWM_NPost_Post_P1, NULL, SWV_NServer_Begin, 0, 0, NULL,
               0, 0, NULL);

      DoMethod(o, SWM_NPost_Post_P1, NULL, SWV_NServer_Rescode, 0, 0, NULL,
               0, SWV_NSR_OKToPost, NULL);

      DoMethod(o, SWM_NPost_Post_P1, NULL, SWV_NServer_Done, 0, 0, NULL,
               0, 0, NULL);
#  else
      rc = DoMethod(Srv,
                  SWM_NServer_SendCmd,
                  SWT_NServer_Cmd,       SWV_NServer_PostCmd,
                  SWT_NServer_CmdObj,    o,
                  SWT_NServer_CmdMethod, SWM_NPost_Post_P1,
                  SWT_NServer_Title,     GetS(SWS_T_TCPTitle),
                  SWT_NServer_Flags,     SWF_NServer_ExplicitDone,
                  TAG_DONE);
#  endif // NPOST_NO_NETIO
   
   SWDB_RET(DoMethod(o, SWM_Root_AdoptObjError, Srv, rc));
} SDFT


/*****************************************************************************
 * Method: Post, after initial POST cmd                                      *
 *****************************************************************************/

METHOD(SWM_NPost_Post_P1, SWP_NServer_StatusMsg *)
{
   MsgBegin(o, idata, msg);

   // -- make sure server allows posting -------------------------------------

   if (NSR_SRescode(msg)) {
      if (NSR_ResCode(msg) != SWV_NSR_OKToPost) {
         SWERR(o,0,SWS_Err_NoPost,SWGetStr(idata->CmdSrv,SWA_NServer_Host),0);
         DoMethod(idata->CmdSrv, SWM_NServer_Done, idata->CmdID);
      } else {
         if (SendPost(o, idata) == SUCCESS) {
#           ifdef NPOST_NO_NETIO
               PutStr(SWV_NServer_EndCmd);
#           else
               DoMethod(idata->CmdSrv,
                      SWM_NServer_SendCmd,
                      SWT_NServer_ID,        idata->CmdID,
                      SWT_NServer_Cmd,       SWV_NServer_EndCmd,
                      SWT_NServer_CmdObj,    o,
                      SWT_NServer_CmdMethod, SWM_NPost_Post_P2,
                      SWT_NServer_Title,     GetS(SWS_T_TCPEnd),
                      SWT_NServer_Flags,     SWF_NServer_ExplicitDone,
                      TAG_DONE);
#           endif // NPOST_NO_NETIO
         } else
            SWERR(o, FAILURE, SWS_Err_PostSend, 0, 0);
      }
   }

   if (NSR_SAbort(msg))
      SWERR(o, FAILURE, SWS_Err_PostAborted, 0, 0);

   MsgEnd(o, idata, msg);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Post, after initial POST cmd                                      *
 *****************************************************************************/

METHOD(SWM_NPost_Post_P2, SWP_NServer_StatusMsg *)
{
   static TEXT ErrMsg[256];
   
   MsgBegin(o, idata, msg);

   if (NSR_SRescode(msg)) {
      if (NSR_ResCode(msg) == SWV_NSR_PostedOK)
         idata->SWA_NPost_PostedOK = TRUE;
      else
         SWERR(o, 0, SWS_Err_PostSend,
               strncpy(ErrMsg, NSR_Line(msg, 0), 255), 0);

      DoMethod(idata->CmdSrv, SWM_NServer_Done, idata->CmdID);
   }

   MsgEnd(o, idata, msg);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Abort pending post                                                *
 *****************************************************************************/

METHOD(SWM_NPost_Abort, Msg)
{
   if (!idata->CmdSrv) SWDB_RET(0);

   SWDB_RET(DoMethod(idata->CmdSrv, SWM_NServer_Abort, idata->CmdID));
} SDFT


/*****************************************************************************
 * Method: Send post to external editor                                      *
 *****************************************************************************/

METHOD(SWM_NPost_Editor, Msg)
{
   ULONG  rc = SUCCESS;
   STRPTR edit = VL_Alloc(o, idata, NN_Editor(idata)),
          args;

   if (SW_STRISNULL(edit)) {
      rc = SWERR(o, FAILURE, SWS_Err_EditErr, 0, 0);
      goto Done;
   }

   if (args = strchr(edit, ' ')) {
      *args++ = '\0';
      while (*args == ' ') args++;
   }

   DoMethod(o, SWM_NPost_SaveEditData, NULL, FALSE);

   DoMethod(idata->TEditor, SWM_NNews_Editor_InitNotify, NULL);

   DoMethod(o, SWM_Root_SysCmd,
            SWT_Root_Cmd,     edit,
            SWT_Root_Arg,     args,
            SWT_Root_Stack,   32000,
            TAG_DONE);

 Done:
   DoMethod(o, SWM_Root_CheckErr, idata->UIWin);
   VL_Free(o, idata, edit);
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Generate filespec for post                                        *
 *****************************************************************************/

METHOD(SWM_NPost_FSpec, SWP_NPost_FSpec *)
{
   STRPTR s, t;

   t = msg->FType == SWF_NPost_FSpec_Perm    ? idata->FS1 :
       msg->FType == SWF_NPost_FSpec_Temp    ? idata->FS2 :
       msg->FType == SWF_NPost_FSpec_Wrapped ? idata->FS3 : NULL;

   if (!t) SWDB_RET(NULL);
   
   if (SW_STRISNULL(idata->SWA_NPost_MsgID)) SWDB_RET(NULL);

   if (msg->FType == SWF_NPost_FSpec_Temp)
      strcpy(t, "T:");
   else
      AddPart(strcpy(t, NN_CacheDir(idata)), idata->SWA_NPost_PostDir, FMSIZE);

   AddPart(t, idata->SWA_NPost_MsgID + 1, FMSIZE);

   if (s = strchr(t, '@'))
      *s = '\0';

   if (msg->FType == SWF_NPost_FSpec_Wrapped)
      strcat(t, ".W");
   
   SWDB_RET(t);
} SDFT


/*****************************************************************************
 * Method: Delete the post and its files                                     *
 *****************************************************************************/

METHOD(SWM_NPost_Delete, Msg)
{
   SWDB_RET((SWDeleteFile((STRPTR)DoMethod(o, SWM_NPost_FSpec,
                                      SWF_NPost_FSpec_Perm))    == SUCCESS &&
             SWDeleteFile((STRPTR)DoMethod(o, SWM_NPost_FSpec,
                                      SWF_NPost_FSpec_Temp))    == SUCCESS &&
             SWDeleteFile((STRPTR)DoMethod(o, SWM_NPost_FSpec,
                                      SWF_NPost_FSpec_Wrapped)) == SUCCESS)
            ? o : NULL);
} SDFT


/*****************************************************************************
 * Method: Copy a file to our cached copy                                    *
 *****************************************************************************/

METHOD(SWM_NPost_CopyFile, ULONG *)
{
   ULONG  Del = GetTagUL(SWT_NPost_CF_Del,   FALSE, msg+1);
   ULONG  Dir = GetTagUL(SWT_NPost_CF_Dir,   0,     msg+1);
   STRPTR Loc = GetTagStr(SWT_NPost_CF_Trg,
                          DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Perm),
                          msg+1);
   STRPTR Tmp = GetTagStr(SWT_NPost_CF_Src,
                          DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Temp),
                          msg+1);
   Object *ErrWin = GetTagObj(SWT_NPost_CF_ErrWin, idata->UIWin, msg+1);
   ULONG  rc;

   if (Dir==1 && (!SWFileExists(Loc) || SWFileExists(Tmp)) ||
       Dir==0 && !SWFileExists(Tmp))
      SWDB_RET(SUCCESS);

   // -- copy file into (or out of) local cache ------------------------------
   
   rc = DoMethod(o, SWM_Root_CopyFile,
                 SWT_Root_CF_Src,      Dir ? Loc : Tmp,
                 SWT_Root_CF_Trg,      Dir ? Tmp : Loc,
                 SWT_Root_CF_Del,      Del,
                 SWT_Root_CF_MakePath, Dir==0,
                 TAG_DONE);

   // -- if asked to, update stats for file ----------------------------------

   if (rc == SUCCESS)
      if (GetTagUL(SWT_NPost_CF_Stats, FALSE, msg+1))
         rc = DoMethod(o, SWM_NPost_UpdStats, Tmp);

   DoMethod(o, SWM_Root_CheckErr, ErrWin);
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: update display from this fspec                                    *
 *****************************************************************************/

METHOD(SWM_NPost_UpdStats, SWP_NPost_UpdStats *)
{
   ULONG  Lines = 0, Bytes = 0, InBytes, rc = SUCCESS, x;
   BPTR   fp;
   STRPTR Buff = NULL;
   STRPTR File = SW_STRDEF(msg->File,
                           (STRPTR)DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Perm));
   
   SetAttrs(o,
            SWA_NPost_Lines, 0,
            SWA_NPost_Bytes, 0,
            TAG_DONE);

#  define US_Size 32768

   if (!(fp = Open(File, MODE_OLDFILE)))
      goto Done;

   if (!(Buff = AV(US_Size))) {
      rc = SWERR(o, FAILURE, SWS_Err_Malloc, 0, 0);
      goto Done;
   }

   while (InBytes = Read(fp, Buff, US_Size)) {
      Bytes += InBytes;
      for (x=0; x<InBytes; x++)
         if (Buff[x] == '\n')
            Lines++;
         else if (!IsAscii(Buff[x])) {
            rc = SWERR(o, FAILURE, SWS_Err_NotAscii, File, 0);
            goto Done;
         }
   }

   SetAttrs(o,
            SWA_NPost_Lines, Lines,
            SWA_NPost_Bytes, Bytes,
            TAG_DONE);

  Done:
   if (fp) Close(fp);
   FV(Buff);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: read file contents                                              *
 *****************************************************************************/

static STRPTR ReadFContents(Object *o, STRPTR FSpec, ULONG *Size)
{
   ULONG  Err;
   STRPTR FData;
   
   FData = SWReadFileContents(FSpec, Size, &Err);

   if (Err)
      SWERR_NM(o, NULL, SWDecodeErrStr(Err), FSpec, 0);
      
   return FData;
}


/*****************************************************************************
 * Function: insert lines into list                                          *
 *****************************************************************************/

static ULONG InsBuffLines(Object *o, SW_IDATA *idata,
                          ULONG Lines, STRPTR FData, Object *L, ULONG Mode)
{
   STRPTR s, ll, Out, Next;
   ULONG  rc = SUCCESS;

   set(L, MUIA_List_Quiet, TRUE);

   for (ll=s=FData; *s; s++)
      if (*s == '\n') {
         *s = '\0', Out = ll, ll = s+1;

         if (ModeSeesEncoding(Mode)) {
            // -- insert with display of quoted printable stuff --------------
            
            while (TRUE) {
               if (!(Next = GenPostLine(idata, Out, &Out)))
                  break;
               
               DoMethod(L, MUIM_List_InsertSingle, Out, MUIV_List_Insert_Bottom);
   
               if (!(*(Out = Next))) break;
            }
         } else {
            // -- insert raw characters, no encoding -------------------------

            DoMethod(L, MUIM_List_InsertSingle, Out, MUIV_List_Insert_Bottom);
         }
      }
   
   set(L, MUIA_List_Quiet, FALSE);

  Done:
   return rc;
}


/*****************************************************************************
 * Method: update display from this fspec                                    *
 *****************************************************************************/

METHOD(SWM_NPost_UpdDisp, SWP_NPost_UpdDisp *)
{
   ULONG Size = 0, rc = SUCCESS, Lines;
   STRPTR FData;

   if (!idata->TEditor) SWDB_RET(FAILURE);

   SetAttrs(o,
            SWA_NPost_Lines, 0,
            SWA_NPost_Bytes, 0,
            TAG_DONE);

   DoMethod(idata->TEditor, MUIM_TextEditor_ClearText);

   // -- read the file contents ----------------------------------------------

   if (!(FData = ReadFContents(o, msg->File, &Size))) {
      rc = FAILURE;
      goto Done;
   }

   // -- count lines & look for non-ascii chars ------------------------------

   Lines = SWCountLines(FData);

   // -- shove strings into text area ----------------------------------------

   set(idata->TEditor, MUIA_TextEditor_Quiet, TRUE);
      
   DoMethod(idata->TEditor, MUIM_TextEditor_InsertText, FData,
            MUIV_TextEditor_InsertText_Bottom);

   SetAttrs(idata->TEditor,
            MUIA_TextEditor_CursorX,    0,
            MUIA_TextEditor_CursorY,    0,
            MUIA_TextEditor_Quiet,      FALSE,
            TAG_DONE);
   
   FreeVec(FData);

   SetAttrs(o,
            SWA_NPost_Lines, Lines,
            SWA_NPost_Bytes, Size,
            TAG_DONE);

  Done:
   if (idata->UIWin)
      DoMethod(o, SWM_Root_CheckErr, idata->UIWin);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Add text to file                                                  *
 *****************************************************************************/

METHOD(SWM_NPost_AddText, SWP_NPost_AddText *)
{
   BPTR   fp;
   ULONG  rc = SUCCESS;
   STRPTR f;

   idata->SWA_NPost_Mod = TRUE;

   if (!(fp = Open(f=(STRPTR)DoMethod(o, SWM_NPost_FSpec,
                                      msg->Tmp ? SWF_NPost_FSpec_Temp
                                               : SWF_NPost_FSpec_Perm),
                   MODE_NEWFILE))) {
      rc = SWERR(o, FAILURE, SWS_Err_FileWrite, f, 0);
      goto Done;
   }

   FPuts(fp, msg->Text);
   FPuts(fp, "\n");

  Done:
   if (fp) Close(fp);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Delete article                                                    *
 *****************************************************************************/

METHOD(SWM_NPost_IDel, SWP_NPost_IDel *)
{
   if (!msg->Force &&
       DoMethod(idata->TEditor, SWM_NNews_Editor_AskClob) != SUCCESS)
      SWDB_RET(FAILURE);

   DoMethod(o, SWM_NPost_Delete);

   idata->IDel = TRUE;

   DoMethod(NN_App(idata), MUIM_Application_PushMethod, NN_PostList(idata), 2,
            SWM_NList_RemoveObj, o);
   
   SWDB_RET(SUCCESS);
}


/*****************************************************************************
 * Method: Archive post                                                      *
 *****************************************************************************/

METHOD(SWM_NPost_Archive, Msg)
{
   DoMethod(NN_App(idata), MUIM_Application_PushMethod, NN_PostList(idata), 3,
            SWM_NPostList_XferPost, o, NN_SvPostList(idata));

   SWDB_RET(SUCCESS);
}


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   STRPTR s;
   
   VL_FindRet("File", DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Temp));

   if (idata->VarSrchGrp && (s=(STRPTR)DoMethodA(idata->VarSrchGrp, msg)))
      SWDB_RET(s);

   if (idata->VarSrchArt && (s=(STRPTR)DoMethodA(idata->VarSrchArt, msg)))
      SWDB_RET(s);

   SWDB_RET(DoMethodA(idata->SWA_AppBase_TopObj, (Msg)msg));
} SDFT


/*****************************************************************************
 * Function: Make followup string                                            *
 *****************************************************************************/

static STRPTR GenFUpStr(SW_IDATA *idata, STRPTR Grps, ULONG MaxGrps)
{
   ULONG x, y, gc;
   STRPTR FUpStr = AV(strlen(Grps)+200);

   if (!FUpStr) return NULL;
         
   strcpy(FUpStr, "   ");
   for (x=0,gc=1,y=3; Grps[x] && gc < MaxGrps; x++, y++)
      if (Grps[x] == ',')
         FUpStr[y++] = '\n',
         FUpStr[y++] = ' ',
         FUpStr[y++] = ' ',
         FUpStr[y]   = ' ',
         gc++;
      else
         FUpStr[y] = Grps[x];

   FUpStr[y] = '\0';

   if (gc >= MaxGrps) {
      for (gc=1; Grps[x]; x++) if (Grps[x] == ',') gc++;
      sprintf(FUpStr + y, "[%ld more]", gc);
   }

   return FUpStr;
}


/*****************************************************************************
 * Function: Generate post data (subj/group strings/etc)                     *
 *****************************************************************************/

static ULONG GenPostData(Object *o, SW_IDATA *idata, PostData *PD,
                         Object *Grp, Object *Art, Object *Win)
{
   STRPTR t, t1;
   
   PD->Subj        = "", PD->Grps = "", PD->FUps = "";
   PD->SubjAlloc   = FALSE;
   PD->ReplLine    = NULL;
   PD->QuotePrefix = NULL;
   PD->Refs        = NULL;
   PD->SigFile     = SWGetStr(Grp, SWA_NGroup_SigFile);
   PD->SigFile     = SW_STRDEF(PD->SigFile, NN_SigFile(idata));

   // -- build subject/newsgroups lines --------------------------------------

   if (!Art) {
      PD->Grps = SWGetStr(Grp, SWA_NGroup_Name);
      return SUCCESS;
   }

   PD->Subj =(STRPTR)DoMethod(Art, SWM_NArticle_GetSubj);
   PD->Grps =(STRPTR)DoMethod(Art, SWM_NArticle_FindHeader, SWF_NAHeader_Groups);
   PD->FUps =(STRPTR)DoMethod(Art, SWM_NArticle_FindHeader, SWF_NAHeader_Followup);

   // -- build references string ---------------------------------------------

   t  = (STRPTR)DoMethod(Art, SWM_NArticle_FindHeader, SWF_NAHeader_Ref);
   t  = SW_STRDEF(t, "");
   t1 = (STRPTR)DoMethod(Art, SWM_NArticle_GetMsgID);

   if (!SW_STRISNULL(t1)) {
      if (!(PD->Refs = AV(strlen(t) + strlen(t1) + 2)))
         return FAILURE;

      strcpy(PD->Refs, t);
      if (!SW_STRISNULL(t)) strcat(PD->Refs, " ");

      strcat(PD->Refs, t1);
   }


   // -- followups redirected?  ask what to do. ------------------------------

   if (!SW_STRISNULL(PD->FUps) && strcmp(PD->FUps, PD->Grps)) {
      ULONG rv;
      STRPTR FUpStr = GenFUpStr(idata, PD->FUps, 5);

      rv = MUI_Request(NN_App(idata), Win, 0, NULL,
                       GetS(SWS_L_AskFUp), GetS(SWS_L_FUpWarn), FUpStr);

      FV(FUpStr);

      switch (rv) {
        case 0: return FAILURE;                       // cancel
        case 1: PD->Grps = PD->FUps; break;           // use followups
        case 2: break;                                // use groups
        case 3: PD->Grps = SWGetStr(Grp, SWA_NGroup_Name); // this group only
      }
   }


   // -- build proper subject line -------------------------------------------

   if (!SWV_NArticle_IsRe(PD->Subj)) {
      PD->SubjAlloc = TRUE;
      t             = PD->Subj;
      if (!(PD->Subj = AV(strlen(t)+SWV_NArticle_ReLen+1)))
         return FAILURE;

      strcat(strcpy(PD->Subj, SWV_NArticle_ReStr), t);
   }


   // -- build reply line ----------------------------------------------------

   idata->VarSrchArt = Art;
   idata->VarSrchGrp = Grp;
   PD->ReplLine      = VL_Alloc(o, idata, NN_ReplyLine(idata));
   PD->QuotePrefix   = VL_Alloc(o, idata, NN_QuoteStr(idata));
   PD->QuoteMode     = NN_QuoteMode(idata);
   idata->VarSrchArt = idata->VarSrchGrp = NULL;

   return SUCCESS;
}


/*****************************************************************************
 * Function: Free post data (from GenPostData)                               *
 *****************************************************************************/

static void FreePostData(Object *o, SW_IDATA *idata, PostData *PD)
{
   FV(PD->Refs);
   if (PD->SubjAlloc) FV(PD->Subj);
   VL_Free(o, idata, PD->ReplLine);
   VL_Free(o, idata, PD->QuotePrefix);
}


/*****************************************************************************
 * Method: Generate reply to article                                         *
 *****************************************************************************/

METHOD(SWM_NPost_GenReply, SWP_NPost_GenReply *)
{
   ULONG rc;
   PostData PD;
   TEXT CachePath[FMSIZE];
   
   // -- make sure we have a postqueue directory -----------------------------

   AddPart(strcpy(CachePath, NN_CacheDir(idata)), idata->SWA_NPost_PostDir,
           FMSIZE);

   if (SWMakePath(CachePath) != SUCCESS) {
      rc = SWERR(o, FAILURE, SWS_Err_MkPostDir,
                 idata->SWA_NPost_PostDir, NN_CacheDir(idata));
      goto Done;
   }
      
   // -- make reply strings, etc ---------------------------------------------

   if ((rc = GenPostData(o,idata,&PD,msg->Grp,msg->Art,msg->Win)) != SUCCESS)
      goto Done;

   // -- set article header fields -------------------------------------------

   SetAttrs(o,
            SWA_NPost_Subj,    PD.Subj,
            SWA_NPost_Grp,     PD.Grps,
            SWA_NPost_Refs,    PD.Refs,
            SWA_NPost_SigFile, PD.SigFile,
            SWA_NPost_Server,  msg->Grp ? SWGetStr(msg->Grp, SWA_NGroup_Server1) :
                               NULL,
            TAG_DONE);

   // -- Save article text, if in reply mode ---------------------------------

   idata->SWA_NPost_NewPost = TRUE;
   
   if (msg->Art) {
      STRPTR FSpec = (STRPTR)DoMethod(o, SWM_NPost_FSpec,
                                      idata->UIWin ? SWF_NPost_FSpec_Temp
                                                   : SWF_NPost_FSpec_Perm);

      if ((rc = DoMethod(o, SWM_NPost_AddText, PD.ReplLine,
                         idata->UIWin ? TRUE : FALSE)) != SUCCESS)
         goto Done;

      if ((rc = DoMethod(msg->Art, SWM_NArticle_SaveText,
                         SWT_NArticle_FSpec,    FSpec,
                         SWT_NArticle_Parts,    SWF_NArticle_Body,
                         SWT_NArticle_Append,   TRUE,
                         SWT_NArticle_Prefix,   PD.QuotePrefix,
                         SWT_NArticle_QMode,    PD.QuoteMode,
                         SWT_NArticle_Grp,      msg->Grp,
                         SWT_NArticle_ArtView,  SWGetObj(msg->Grp,
                                                         SWA_NGroup_ArtView),
                         SWT_NArticle_SvAttr,   SWA_NDFilter_T_NetReply,
                         TAG_DONE)) != SUCCESS)
         goto Done;

      if (idata->UIWin) {
         DoMethod(o, SWM_NPost_UpdDisp,
                  DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Temp));
         idata->SWA_NPost_Mod = TRUE;
      }
   }

  Done:
   FreePostData(o, idata, &PD);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Drag query                                                        *
 *****************************************************************************/

METHOD(SWM_ObjList_DragQuery, SWP_ObjList_DragQuery *)
{
   Object *obj = SWGetObj(msg->obj, SWA_ObjList_Object);
   STRPTR t;

   if (!obj) SWDB_RET(FALSE);

   SWDB_RET(SW_ISCLASS_SAFE(obj, SWC_NArticleList, t) ||
            SW_ISCLASS_SAFE(obj, SWC_NGroupList, t));
} SDFT


/*****************************************************************************
 * Method: Drag drop                                                         *
 *****************************************************************************/

METHOD(SWM_ObjList_DragDrop, SWP_ObjList_DragDrop *)
{
   Object *obj = SWGetObj(msg->obj, SWA_ObjList_Object);
   Object *drop;

   if (!obj) SWDB_RET(0);

   if (SW_ISCLASS(obj, SWC_NArticleList)) {
      DoMethod(obj, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &drop);
      DoMethod(o, SWM_NPost_GenReply,
               SWGetObj(obj, SWA_NArticleList_Group), drop, idata->UIWin);
   } else if (SW_ISCLASS(obj, SWC_NGroupList)) {
      DoMethod(obj, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &drop);
      DoMethod(o, SWM_NPost_GenReply, drop, NULL, idata->UIWin);
   }

   DoMethod(o, SWM_NList_SetNotify);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: menu call                                                         *
 *****************************************************************************/

METHOD(SWM_NPost_MenuCall, SWP_NPost_MenuCall *)
{
   if (!idata->TEditor) SWDB_RET(FAILURE);
   
   switch (msg->Event) {
     case SWS_M_T_Save:
      DoMethod(idata->TEditor, SWM_NNews_Editor_EdSave);
      break;
      
     case SWS_M_T_Ins:
      DoMethod(idata->TEditor, SWM_NNews_Editor_EdIns, NULL);
      break;

     case SWS_M_T_InsQuoted:
      DoMethod(idata->TEditor, SWM_NNews_Editor_EdIns, "> ");
      break;

     case SWS_M_T_Clear:
      if (DoMethod(idata->TEditor, SWM_NNews_Editor_AskClob) != SUCCESS)
         SWDB_RET(FAILURE);
      
      DoMethod(idata->TEditor, MUIM_TextEditor_ClearText);
      break;

     case SWS_M_E_Undo:
      DoMethod(idata->TEditor, MUIM_TextEditor_ARexxCmd, "UNDO");
      break;
      
     case SWS_M_E_Redo:
      DoMethod(idata->TEditor, MUIM_TextEditor_ARexxCmd, "REDO");
      break;

     case SWS_M_E_Cut: 
      DoMethod(idata->TEditor, MUIM_TextEditor_ARexxCmd, "CUT");
      break;

     case SWS_M_E_Copy: 
      DoMethod(idata->TEditor, MUIM_TextEditor_ARexxCmd, "COPY");
      break;

     case SWS_M_E_Paste:
      DoMethod(idata->TEditor, MUIM_TextEditor_ARexxCmd, "PASTE");
      break;

     case SWS_M_E_Rewrap:
      DoMethod(o, SWM_NPost_WrapEditTxt, 0);
      break;
   }

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Insert header lines                                             *
 *****************************************************************************/

static void InsertHdrLines(Object *o, SW_IDATA *idata, Object *L, ULONG Mode)
{
   ULONG x, Count;
   STRPTR *Hdrs;

   set(L, MUIA_List_Quiet, TRUE);

   if (!(Hdrs = CreateHdrLines(o, idata, &Count, "",
                               ModeSeesEncoding(Mode)))) return;

   for (x=0; x<Count; x++)
      DoMethod(L, MUIM_List_InsertSingle, Hdrs[x], MUIV_List_Insert_Bottom);

   set(L, MUIA_List_Quiet, FALSE);

   FreeHdrLines(idata, Hdrs, Count);
}


/*****************************************************************************
 * Function: Insert signature/tag file lines                                 *
 *****************************************************************************/

static void InsertSigFileLines(Object *o, SW_IDATA *idata, Object *L,
                               ULONG Mode)
{
   BPTR fp;
   TEXT Line[256];
   ULONG x, y, SigSep = FALSE;

   set(L, MUIA_List_Quiet, TRUE);

   // -- insert sig file -----------------------------------------------------
   
   if (!SW_STRISNULL(idata->SWA_NPost_SigFile)) {

      if (!(fp = Open(idata->SWA_NPost_SigFile, MODE_OLDFILE)))
         return;

      if (NN_UseSS(idata)) {
         DoMethod(L, MUIM_List_InsertSingle, "-- ", MUIV_List_Insert_Bottom);
         SigSep = TRUE;
      }

      while (FGets(fp, Line, 255)) {
         SW_REMOVENL(Line);

         DoMethod(L, MUIM_List_InsertSingle, Line, MUIV_List_Insert_Bottom);
      }

      Close(fp);
   }

   // -- insert tag lines ----------------------------------------------------

   if (SW_STRISNULL(idata->SWA_NPost_TagLine))
      DoMethod(o, SWM_NPost_GetTagLine, SWV_NPost_GTL_Random);
      
   if (!SW_STRISNULL(idata->SWA_NPost_TagLine)) {
      if (NN_UseSS(idata) && !SigSep)
         DoMethod(L, MUIM_List_InsertSingle, "-- ", MUIV_List_Insert_Bottom);
      
      for (x=y=0; idata->SWA_NPost_TagLine[x]; x++) {
         if (idata->SWA_NPost_TagLine[x] == '\n') {
            idata->SWA_NPost_TagLine[x] = '\0';
            DoMethod(L, MUIM_List_InsertSingle, idata->SWA_NPost_TagLine + y,
                     MUIV_List_Insert_Bottom);
            idata->SWA_NPost_TagLine[x] = '\n';
            y = x + 1;
         }
      }
   }

   // -- update the list -----------------------------------------------------
      
   set(L, MUIA_List_Quiet, FALSE);
}


/*****************************************************************************
 * Method: change edit mode                                                  *
 *****************************************************************************/

METHOD(SWM_NPost_EditMode, SWP_NPost_EditMode *)
{
   STRPTR Buff;
   ULONG  Lines;

   if (!idata->UIWin || !idata->G_Group)
      SWDB_RET(FAILURE);

   DoMethod(idata->G_Group, MUIM_Group_InitChange);

   if (idata->G_Current) {
      DoMethod(idata->G_Group, OM_REMMEMBER, idata->G_Current);
      idata->G_Current = NULL;
   }

   if (msg->Mode == SWV_NPost_ModeEdit)
      idata->G_Current = idata->TEditor;

   if (msg->Mode > SWV_NPost_ModeEdit && msg->Mode < SWV_NPost_ModeViewMAX) {
      InitPostData(idata);
      
      idata->G_Current = idata->G_View;
      DoMethod(idata->G_View, MUIM_List_Clear);

      if (Buff = (STRPTR)DoMethod(o, SWM_NPost_Rewrap, 0)) {
         Lines = SWCountLines(Buff);

         // look for QP chars, set up MIME crap, etc
         if (AnalyzePostData(o, idata, Buff) != SUCCESS)
            DoMethod(o, SWM_Root_CheckErr, idata->UIWin);

         if (msg->Mode == SWV_NPost_ModeViewAll ||
             msg->Mode == SWV_NPost_ModeViewEnc)
            InsertHdrLines(o, idata, idata->G_View, msg->Mode);

         InsBuffLines(o, idata, Lines, Buff, idata->G_View, msg->Mode);

         FreeVec(Buff);
      }

      if (msg->Mode == SWV_NPost_ModeViewAll ||
          msg->Mode == SWV_NPost_ModeViewEnc)
         InsertSigFileLines(o, idata, idata->G_View, msg->Mode);
   }

   if (!idata->G_Current)
     idata->G_Current = RectangleObject, End;

   if (idata->G_Current)
      DoMethod(idata->G_Group, OM_ADDMEMBER, idata->G_Current);

   DoMethod(idata->G_Group, MUIM_Group_ExitChange);

   if (msg->Mode == SWV_NPost_ModeEdit)
      DoMethod(idata->G_View, MUIM_List_Clear);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: process arexx command                                             *
 *****************************************************************************/

METHOD(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *)
{
   STRPTR cmd = ARG0(msg->Rx);
   LONG x, Len;
   UBYTE TmpBuff[32];

   struct {
      STRPTR Name;
      ULONG  Attr;
   } Attrs[] = {
      "SUBJ",     SWA_NPost_Subj,
      "GROUPS",   SWA_NPost_Grp,
      "FUPS",     SWA_NPost_FUps,
      "DISTRIB",  SWA_NPost_Distrib,
      "SIGFILE",  SWA_NPost_SigFile,
      NULL,       NULL,
   };

   // -- get attributes ------------------------------------------------------
   
   if (!strncmp(cmd, "GET", 3))
      for (x=0; Attrs[x].Name; x++)
         if (!strcmp(cmd+3, Attrs[x].Name)) {
            set(msg->App, MUIA_Application_RexxString, SWGetStr(o, Attrs[x].Attr));
            goto OK;
         }
   
   // -- set attributes ------------------------------------------------------
   
   if (!strncmp(cmd, "SET", 3))
      for (x=0; Attrs[x].Name; x++) {
         Len = strlen(Attrs[x].Name);
         if (!strncmp(cmd+3, Attrs[x].Name, Len) && cmd[Len+3] == ' ') {
            set(o, Attrs[x].Attr, cmd+Len+4);
            goto OK;
         }
      }

   // -- copy an external file into our edited one ---------------------------

   if (!strncmp(cmd, "INSFILE ", 8)) {
      if (!cmd[8]) goto Err;
      DoMethod(o, SWM_NPost_InsFile,
               SWT_NPost_IF_FSpec, cmd+8,
               TAG_DONE);
      goto OK;
   }
   
   // -- OPENPOSTEDITOR ------------------------------------------------------
   
   if (!strcmp(cmd, "OPENPOSTEDITOR")) {
      DoMethod(o, SWM_NPost_Edit, FALSE);
      goto OK;
   }
   
   // -- ISPOSTEDITOROPEN ----------------------------------------------------
   
   if (!strcmp(cmd, "ISPOSTEDITOROPEN")) {
      set(msg->App, MUIA_Application_RexxString, idata->UIWin ? "1" : "0");
      goto OK;
   }
   
   // -- ISPOSTMODIFIED ------------------------------------------------------
   
   if (!strcmp(cmd, "ISPOSTMODIFIED")) {
      set(msg->App, MUIA_Application_RexxString, idata->SWA_NPost_Mod ? "1" : "0");
      goto OK;
   }
   
   // -- GETPOSTLINES --------------------------------------------------------
   
   if (!strcmp(cmd, "GETPOSTLINES")) {
      sprintf(TmpBuff, "%ld", idata->SWA_NPost_Lines);
      set(msg->App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }
   
   // -- GETPOSTBYTES --------------------------------------------------------
   
   if (!strcmp(cmd, "GETPOSTBYTES")) {
      sprintf(TmpBuff, "%ld", idata->SWA_NPost_Bytes);
      set(msg->App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }
   
   // -- CLOSEPOSTEDITOR -----------------------------------------------------

   if (!strncmp(cmd, "CLOSEPOSTEDITOR", 15)) {
      ULONG Force = strstr(cmd+16, "FORCE")  ?  TRUE  : FALSE,
            Save  = strstr(cmd+16, "NOSAVE") ?  FALSE : TRUE;

      DoMethod(o, SWM_NPost_CloseEdit, Force, Save, FALSE);
      
      goto OK;
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
 * Method: rewrap text                                                       *
 *****************************************************************************/

METHOD(SWM_NPost_Rewrap, SWP_NPost_Rewrap *)
{
   STRPTR Buff, s = NULL;
   ULONG  Cols     = msg->Cols ? msg->Cols : NN_PostWrap(idata),
          WrapType = NN_PostWrapType(idata), Len;
   Object *Rewrap;

   if (WrapType == SWV_NNews_PR_Never ||
       WrapType == SWV_NNews_PR_Type) {
      Buff = (STRPTR)DoMethod(idata->TEditor, MUIM_TextEditor_ExportText);
      Len = strlen(Buff);

      if (Buff[Len-1] != '\n') {
         STRPTR b1 = AllocVec(Len+4, MEMF_ANY);
         if (!b1) SWDB_RET(Buff);
         strcpy(b1, Buff);
         strcat(b1+Len, "\n");
         FreeVec(Buff);

         SWDB_RET(b1);
      }
      
      SWDB_RET(Buff);
   }
   
   set(NN_App(idata), MUIA_Application_Sleep, TRUE);

   if (Rewrap = MUI_NewObject(SWC_NDF_Rewrap,
                              AppBaseDefTop(idata),
                              SWA_NDFilter_MemPool,     NN_MemPool(idata),
                              SWA_NDF_Rewrap_Cols,      Cols,
                              SWA_NDF_Rewrap_BadWrap,   TRUE,
                              SWA_NDF_Rewrap_PrefixLen, 8,
                              SWA_NDF_Rewrap_Tbl_Use,   TRUE,
                              TAG_DONE)) {
      if (Buff = (STRPTR)DoMethod(idata->TEditor, MUIM_TextEditor_ExportText)) {
         TxtBuff Tx;

         DoMethod(Rewrap, SWM_NDFilter_TBInit, &Tx);

         TxBuff(&Tx) = Buff;
         TxLen(&Tx)  = strlen(Buff);
         TxSize(&Tx) = TxLen(&Tx) + 1;

         DoMethod(Rewrap, SWM_NDF_Rewrap_RewrapTx, &Tx, NULL);

         s = TxBuff(&Tx);
      }

      MUI_DisposeObject(Rewrap);
   }

   set(NN_App(idata), MUIA_Application_Sleep, FALSE);

   SWDB_RET(s);
} SDFT


/*****************************************************************************
 * Method: rewrap text                                                       *
 *****************************************************************************/

METHOD(SWM_NPost_WrapEditTxt, SWP_NPost_Rewrap *)
{
   STRPTR s;
   
   if (!idata->UIWin) SWDB_RET(FAILURE);

   if (s = (STRPTR)DoMethod(o, SWM_NPost_Rewrap, msg->Cols)) {
      DoMethod(idata->TEditor, MUIM_TextEditor_ClearText);
      DoMethod(idata->TEditor, MUIM_TextEditor_InsertText, s,
               MUIV_TextEditor_InsertText_Bottom);
      
      FreeVec(s);
   }

   SWDB_RET(s);
} SDFT


/*****************************************************************************
 * Method: callback when editor is created                                   *
 *****************************************************************************/

METHOD(SWM_NNews_Editor_SetupDone, Msg)
{
   DoMethod(o, SWM_NPost_UpdDisp,
            DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Perm));

   // -- autolaunch editor if asked to ---------------------------------------
   
   if (NN_AutoStartEd(idata))
      DoMethod(o, SWM_NPost_Editor);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: callback for new editor contents                                  *
 *****************************************************************************/

METHOD(SWM_NNews_Editor_NewContents, SWP_NNews_Editor_NewContents *)
{
   SetAttrs(o,
            SWA_NPost_Lines, msg->Lines,
            SWA_NPost_Bytes, msg->Bytes,
            TAG_DONE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Move post files to new directory                                  *
 *****************************************************************************/

METHOD(SWM_NPost_MoveFiles, SWP_NPost_MoveFiles *)
{
   TEXT   Dest[FMSIZE];
   STRPTR Perm = (STRPTR)DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Perm),
          Wrap = (STRPTR)DoMethod(o, SWM_NPost_FSpec, SWF_NPost_FSpec_Wrapped),
          Tmp;

   if (!msg->Dest) SWDB_RET(FAILURE);
   
   AddPart(strcpy(Dest, NN_CacheDir(idata)), msg->Dest, FMSIZE);
   Tmp = Dest + strlen(Dest);

   if (SWMakePath(Dest) != SUCCESS)
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_MkPostDir, Dest, NN_CacheDir(idata)));

   AddPart(Dest, FilePart(Perm), FMSIZE);

   if (SWFileExists(Perm) && !Rename(Perm, Dest))
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_Rename, Perm, Dest));

   *Tmp = '\0';
   AddPart(Dest, FilePart(Wrap), FMSIZE);
   
   if (SWFileExists(Wrap))
      Rename(Wrap, Dest);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Get tagline                                                       *
 *****************************************************************************/

METHOD(SWM_NPost_GetTagLine, SWP_NPost_GetTagLine *)
{
   BPTR   fp = NULL;
   STRPTR Buff = NULL, bpos;
   ULONG  TagEntries = 0, LineCnt = 0;
   static ULONG  RangeVal;
   static ULONG  RangeInit = FALSE;

   STRPTR TagFile = NN_TagLineFile(idata),
          TagSep  = NN_TagLineSep(idata);

   if (!RangeInit)
      RangeInit = TRUE,
      RangeVal  = GetUniqueID();
   
   SetAttrs(o,
            SWA_NPost_TagLine,    NULL,
            SWA_NPost_TagLinePos, 0,
            TAG_DONE);
      
   // -- no tagfile or tag separator -----------------------------------------
   
   if (SW_STRISNULL(TagFile) || SW_STRISNULL(TagSep))
      SWDB_RET(SUCCESS);

   // -- otherwise, count how many entries are in the file -------------------
   
   if (!(Buff = AV(SWV_MaxTaglineLen * SWV_MaxTaglineLines))) goto Done;

   if (!(fp = Open(TagFile, MODE_OLDFILE)))
      goto Done;

   while (FGets(fp, Buff, SWV_MaxTaglineLen-2)) {
      SW_REMOVENL(Buff);
      if (strcmp(Buff, TagSep) == 0)
         TagEntries++;
   }

   // -- figure out which we`e supposed to read -----------------------------

   switch (msg->Pos) {
     case SWV_NPost_GTL_Random:
      RangeVal = (RangeVal + 12347) * 71;
      idata->SWA_NPost_TagLinePos = RangeVal % TagEntries;
      break;

     case SWV_NPost_GTL_Prev:
      idata->SWA_NPost_TagLinePos--;
      break;

     case SWV_NPost_GTL_Next:
      idata->SWA_NPost_TagLinePos++;
      break;
   }

   idata->SWA_NPost_TagLinePos = max(idata->SWA_NPost_TagLinePos, 0);
   idata->SWA_NPost_TagLinePos = min(idata->SWA_NPost_TagLinePos, TagEntries-1);
   
   // -- now find that particular entry --------------------------------------

   TagEntries = 0;
   Seek(fp, 0, OFFSET_BEGINNING);

   if (idata->SWA_NPost_TagLinePos)
      while (FGets(fp, Buff, SWV_MaxTaglineLen-2)) {
         SW_REMOVENL(Buff);
         if (strcmp(Buff, TagSep) == 0)
            if (TagEntries++ == idata->SWA_NPost_TagLinePos)
               break;
      }

   // -- read that entry into our buffer -------------------------------------

   Buff[0] = '\0';
   bpos    = Buff;
   for (LineCnt = 0; LineCnt < SWV_MaxTaglineLines; LineCnt++) {
      if (!FGets(fp, bpos, SWV_MaxTaglineLen-2))
         break;

      SW_REMOVENL(bpos);

      if (strcmp(bpos, TagSep) == 0) {
         *bpos = '\0';
         break;
      }

      bpos += strlen(strcat(bpos, "\n"));
   }

   set(o, SWA_NPost_TagLine, Buff);
      
  Done:
   if (fp)   Close(fp);
   if (Buff) FV(Buff);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Get tagline                                                       *
 *****************************************************************************/

METHOD(SWM_NPost_InsFile, ULONG *)
{
   STRPTR FSpec   = GetTagStr(SWT_NPost_IF_FSpec,   NULL, msg+1);
   STRPTR QPrefix = GetTagStr(SWT_NPost_IF_QPrefix, NULL, msg+1);

   if (SW_STRISNULL(FSpec)) SWDB_RET(FAILURE);

   // -- If editor is open, insert this text ---------------------------------
   
   if (idata->UIWin) {
      DoMethod(idata->TEditor, SWM_NNews_Editor_InsFile,
               FSpec, QPrefix, TRUE);
   } else {
      DoMethod(o, SWM_NPost_CopyFile,
               SWT_NPost_CF_Src,   FSpec,
               SWT_NPost_CF_Stats, TRUE,
               TAG_DONE);

      DoMethod(o, SWM_NPost_CopyFile,
               SWT_NPost_CF_Src,   FSpec,
               SWT_NPost_CF_Trg,
                  DoMethod(o,SWM_NPost_FSpec,SWF_NPost_FSpec_Wrapped),
               TAG_DONE);
   }

   SWDB_RET(SUCCESS);
} SDFT
