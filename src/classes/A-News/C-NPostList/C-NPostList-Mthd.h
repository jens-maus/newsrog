/*****************************************************************************
 * Method: Post selected                                                     *
 *****************************************************************************/

METHOD(SWM_NPostList_Post, Msg)
{
   ULONG x;
   Object *Post, *cb, *App = NN_App(idata);
   
   idata->PostCount = 0;
   idata->PostObj   = NULL;

   SW_MUIListLoopSel(idata->SWA_NList_L, x, Post, TRUE)
      idata->PostCount++;

   if (!idata->PostCount) SWDB_RET(SUCCESS);

   set(_win(o), MUIA_Window_Sleep, TRUE);

   sprintf(idata->PostOf, GetS(SWS_T_PostOf), idata->PostCount);

   idata->PostWin = WindowObject,
      MUIA_Window_IsSubWindow, TRUE,
      MUIA_Window_Title,       GetS(SWS_T_PostWin),
      MUIA_Window_ID,          MAKE_ID('P','l','P','w'),
      WindowContents, VGroup,
         MUIA_Background, MUII_GroupBack,
         Child, idata->G1 = GaugeObject,
            GaugeFrame,
            MUIA_Gauge_InfoText, idata->PostOf,
            MUIA_Gauge_Max,      idata->PostCount,
            MUIA_Gauge_Horiz,    TRUE,
         End,
         Child, idata->G2 = GaugeObject,
            GaugeFrame,
            MUIA_Gauge_InfoText, GetS(SWS_T_Sending),
            MUIA_Gauge_Horiz,    TRUE,
         End,
         Child, ScaleObject, End,
         Child, cb = SimpleButton(GetS(SWS_B_Cancel)),
      End,
   End;

   if (!idata->PostWin) SWDB_RET(FAILURE);

   DoMethod(cb, NOTP, App, 4, MUIM_Application_PushMethod, o,
            1, SWM_NPostList_Abort);

   DoMethod(App, OM_ADDMEMBER, idata->PostWin);
   set(idata->PostWin, MUIA_Window_Open, TRUE);

   // -- trigger first post --------------------------------------------------

   idata->PostCurr = 0;
   idata->PostPos  = MUIV_List_NextSelected_Start;
   DoMethod(o, SWM_NPostList_Post_P1, SWF_NPost_State_End);

   SWDB_RET(SUCCESS);
} SDFT



/*****************************************************************************
 * Function: close window, remove notifications, etc                         *
 *****************************************************************************/

static void ClosePostWin(Object *o, SW_IDATA *idata)
{
   ULONG x;
   Object *Post;
   
   if (!idata->PostWin) return;

   if (idata->PostObj)
      DoMethod(idata->PostObj, MUIM_KillNotifyObj, SWA_NPost_Pct, idata->G2);
   
   // -- remove the ones posted successfully ---------------------------------

   DoMethod(idata->SWA_NList_L, MUIM_List_Select,
            MUIV_List_Select_All, MUIV_List_Select_Off, NULL);
   
   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);

   SW_MUIListLoop(idata->SWA_NList_L, x, Post, TRUE)
      if (SWGetUL(Post, SWA_NPost_PostedOK)) {
         DoMethod(idata->SWA_NList_L, MUIM_List_Select, x,
                  MUIV_List_Select_On, NULL);
         if (!NN_MaxSavedPosts(idata))
            DoMethod(Post, SWM_NPost_Delete);
      }

   if (NN_MaxSavedPosts(idata))
      DoMethod(o, SWM_NPostList_XferSel);
   else
      DoMethod(idata->SWA_NList_L, MUIM_List_Remove, MUIV_List_Remove_Selected);

   // -- close the status window ---------------------------------------------

   set(idata->PostWin, MUIA_Window_Open, FALSE);
   DoMethod(NN_App(idata), OM_REMMEMBER, idata->PostWin);

   idata->PostWin = NULL;
   idata->PostObj = NULL;
   idata->PostPos = 0;

   set(_win(o), MUIA_Window_Sleep, FALSE);

   DoMethod(o, SWM_Root_CheckErr, _win(o));
}


/*****************************************************************************
 * Method: pushmethod Post_P2                                                *
 *****************************************************************************/

METHOD(SWM_NPostList_Post_P1, SWP_NPost_CmdMsg *)
{
   DoMethod(NN_App(idata), MUIM_Application_PushMethod, o, 2,
            SWM_NPostList_Post_P2, msg->State);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Post selected                                                     *
 *****************************************************************************/

METHOD(SWM_NPostList_Post_P2, SWP_NPost_CmdMsg *)
{
   if (msg->State == SWF_NPost_State_End) {
      set(idata->G1, MUIA_Gauge_Current, ++idata->PostCurr);

      DoMethod(idata->SWA_NList_L, MUIM_List_NextSelected, &idata->PostPos);

      if (idata->PostObj)
         DoMethod(idata->PostObj, MUIM_KillNotifyObj, SWA_NPost_Pct, idata->G2);

      if (idata->PostPos == MUIV_List_NextSelected_End) {
         ClosePostWin(o, idata);
         SWDB_RET(SUCCESS);
      }

      DoMethod(idata->SWA_NList_L, MUIM_List_GetEntry, idata->PostPos,
               &idata->PostObj);

      DoMethod(idata->PostObj, MUIM_Notify, SWA_NPost_Pct, MUIV_EveryTime,
               idata->G2, 3, MUIM_Set, MUIA_Gauge_Current, MUIV_TriggerValue);

      if (DoMethod(idata->PostObj, SWM_NPost_Post,
                   SWT_NPost_CmdObj,    o,
                   SWT_NPost_CmdMethod, SWM_NPostList_Post_P1,
                   TAG_DONE) != SUCCESS) {
         DoMethod(o, SWM_Root_AdoptObjError, idata->PostObj, 0);
         ClosePostWin(o, idata);
      }
   }

   if (msg->State == SWF_NPost_State_Abort) {
      if (idata->PostObj)
         DoMethod(idata->PostObj, MUIM_KillNotifyObj, SWA_NPost_Pct, idata->G2);

      DoMethod(o, SWM_Root_AdoptObjError, idata->PostObj, 0);
      ClosePostWin(o, idata);
   }

   SWDB_RET(SUCCESS);      
} SDFT


/*****************************************************************************
 * Method: Abort postings                                                    *
 *****************************************************************************/

METHOD(SWM_NPostList_Abort, Msg)
{
   DoMethod(idata->PostObj, SWM_NPost_Abort);
   SWDB_RET(SUCCESS);
}

/*****************************************************************************
 * Method: Go on or offline                                                  *
 *****************************************************************************/

METHOD(SWM_NPostList_OnOffLine, SWP_NPostList_OnOffLine *)
{
   set(idata->b_Post, MUIA_Disabled, !msg->State);

   DoMethod(o, SWM_NList_ObjMethodA, msg);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Workbench appmsg                                                  *
 *****************************************************************************/

METHOD(SWM_NPostList_AppMessage, SWP_NPostList_AppMessage *)
{
   UBYTE FSpec[FMSIZE];
   Object *temp;
   ULONG x;
   
   for (x=0; x<msg->AMsg->am_NumArgs; x++) {
      NameFromLock(msg->AMsg->am_ArgList[x].wa_Lock, FSpec, FMSIZE);
      AddPart(FSpec, msg->AMsg->am_ArgList[x].wa_Name,      FMSIZE);

      if (!(temp = MkInsObj(idata)))
         SWDB_RET(FAILURE);

      SetAttrs(temp, SWA_NPost_Subj, FSpec, TAG_DONE);

      if (DoMethod(temp, SWM_NPost_CopyFile,
                   SWT_NPost_CF_Src,    FSpec,
                   SWT_NPost_CF_Stats,  TRUE,
                   SWT_NPost_CF_ErrWin, _win(o),
                   TAG_DONE) == SUCCESS)
         DoMethod(o, MUIM_List_InsertSingle, temp, MUIV_List_Insert_Bottom);
      else {
         DoMethod(temp, SWM_NPost_Delete);
         MUI_DisposeObject(temp);
      }
   }
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Process menu event                                                *
 *****************************************************************************/

METHOD(SWM_AppBase_MenuCall, SWP_AppBase_MenuCall *)
{
   SWDB_RET(DoMethodA(idata->SWA_AppBase_TopObj, msg));
} SDFT


/*****************************************************************************
 * Method: Generate reply                                                    *
 *****************************************************************************/

METHOD(SWM_NPostList_GenReply, SWP_NPostList_GenReply *)
{
   Object   *Obj;
   ULONG     rc = SUCCESS;

   if (!msg->Grp && !msg->Art) SWDB_RET(FAILURE);

   // -- make a new post -----------------------------------------------------

   if (!(Obj=(Object *)DoMethod(o, SWM_NList_Add, NULL, NULL)))
      goto Done;

   if ((rc = DoMethod(Obj, SWM_NPost_GenReply,
                      msg->Grp, msg->Art, msg->Win)) != SUCCESS ||
       (rc = DoMethod(Obj, SWM_NPost_Edit, FALSE)) != SUCCESS)
      goto Done;

  Done:
   if (rc != SUCCESS && Obj)
      DoMethod(o, SWM_NList_RemoveObj, DoMethod(Obj, SWM_NPost_Delete));

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

# define SW_NList_NoDragDrop
METHOD(SWM_ObjList_DragDrop, SWP_ObjList_DragDrop *)
{
   Object *obj = SWGetObj(msg->obj, SWA_ObjList_Object);
   Object *drop;

   if (!obj) SWDB_RET(0);

   if (SW_ISCLASS(obj, SWC_NArticleList)) {
      DoMethod(obj, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &drop);
      DoMethod(o, SWM_NPostList_GenReply,
               SWGetObj(obj, SWA_NArticleList_Group), drop, _win(o));
   } else if (SW_ISCLASS(obj, SWC_NGroupList)) {
      DoMethod(obj, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &drop);
      DoMethod(o, SWM_NPostList_GenReply, drop, NULL, _win(o));
   }

   DoMethod(o, SWM_NList_SetNotify);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: process arexx command                                             *
 *****************************************************************************/

METHOD(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *)
{
   STRPTR cmd = ARG0(msg->Rx), PostCmd, OrigCmd;
   Object *Post;
   LONG x;
   UBYTE TmpBuff[32];

   // -- POSTCOUNT -----------------------------------------------------------

   if (!strcmp(cmd, "POSTCOUNT")) {
      SW_MUIListLoop(idata->SWA_NList_L, x, Post, TRUE);
      sprintf(TmpBuff, "%ld", x);
      set(msg->App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }

   // -- post cmd ------------------------------------------------------------

   if (!strncmp(cmd, "POSTCMD ", 8)) {
      cmd += 8;
      PostCmd = strchr(cmd, ' ');

      if (SW_STRISNULL(PostCmd) || PostCmd[1] == '\0') goto Err;
      PostCmd++;

      x = atol(cmd);
      DoMethod(o, MUIM_List_GetEntry, x, &Post);
      if (!Post) goto Err;

      OrigCmd       = ARG0(msg->Rx);
      ARG0(msg->Rx) = PostCmd;
      x             = DoMethodA(Post, msg);
      ARG0(msg->Rx) = OrigCmd;
      SWDB_RET(x);
   }

   // -- ADDPOST -------------------------------------------------------------

   if (!strcmp(cmd, "ADDPOST")) {
      DoMethod(o, SWM_NList_Add, NULL, NULL);
      sprintf(TmpBuff, "%ld",
              SWGetLong(idata->SWA_NList_L, MUIA_List_InsertPosition));
      set(msg->App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }

   // -- SENDPOST ------------------------------------------------------------

   if (!strncmp(cmd, "SENDPOST ", 9)) {
      cmd += 9;

      set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);

      DoMethod(idata->SWA_NList_L, MUIM_List_Select,
               MUIV_List_Select_All,
               !strcmp(cmd, "ALL") ? MUIV_List_Select_On :
                                     MUIV_List_Select_Off,
               NULL);

      if (strcmp(cmd, "ALL")) {
         while (*cmd) {
            while (*cmd == ' ') cmd++;
            if (!*cmd) break;
            
            x = atol(cmd);

            DoMethod(idata->SWA_NList_L, MUIM_List_Select,
                     x, MUIV_List_Select_On, NULL);
            
            while (*cmd != ' ' && *cmd) cmd++;
         }
      }

      DoMethod(o, SWM_NPostList_Post);

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
 * Method: Transfer post to other postqueue                                  *
 *****************************************************************************/

METHOD(SWM_NPostList_XferPost, SWP_NPostList_XferPost *)
{
   STRPTR NewDir;
   Object *Post = msg->Post;

   if (!msg->Dest) SWDB_RET(FAILURE);
   
   if (!Post)
      DoMethod(idata->SWA_NList_L, MUIM_List_GetEntry,
               MUIV_List_GetEntry_Active, &Post);

   DoMethod(Post, SWM_NPost_CloseEdit, TRUE, TRUE, FALSE);

   if (DoMethod(Post, SWM_NPost_MoveFiles,
                NewDir = SWGetStr(msg->Dest, SWA_NPostList_PostDir)) != SUCCESS)
      SWDB_RET(FAILURE);

   set(Post, SWA_NPost_NoDel, TRUE);
   DoMethod(o, SWM_NList_RemoveObj, Post);
   set(Post, SWA_NPost_NoDel, FALSE);
   
   set(Post, SWA_NPost_PostDir, NewDir);

   DoMethod(msg->Dest, SWM_NList_Add, Post, NULL);
   DoMethod(msg->Dest, SWM_NPostList_RemArch);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Remove old archived posts                                         *
 *****************************************************************************/

METHOD(SWM_NPostList_RemArch, Msg)
{
   LONG x, Count = 0, Max = NN_MaxSavedPosts(idata);
   Object *Post;
   
   if (!idata->SWA_NPostList_Saved)
      SWDB_RET(FAILURE);

   DoMethod(idata->SWA_NList_L, MUIM_List_Select,
            MUIV_List_Select_All, MUIV_List_Select_Off, NULL);
   
   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);

   SW_MUIListLoop(idata->SWA_NList_L, x, Post, TRUE)
      Count++;

   SW_MUIListLoop(idata->SWA_NList_L, x, Post, TRUE) 
      if (x < (Count - Max)) {
         DoMethod(idata->SWA_NList_L, MUIM_List_Select, x,
                  MUIV_List_Select_On, NULL);
         DoMethod(Post, SWM_NPost_Delete);
      } else break;

   DoMethod(idata->SWA_NList_L, MUIM_List_Remove, MUIV_List_Remove_Selected);
   
   SWDB_RET(SUCCESS);
}

   
/*****************************************************************************
 * Method: Transfer selected posts to other queue                            *
 *****************************************************************************/

METHOD(SWM_NPostList_XferSel, Msg)
{
   Object *Post, **Posts, *Dest;
   ULONG x, PostCount = 0, rc = FAILURE;

   SW_MUIListLoopSel(idata->SWA_NList_L, x, Post, TRUE)
      PostCount++;

   if (!(Posts = AllocVec(sizeof(Object *) * (PostCount+1), MEMF_ANY)))
      SWDB_RET(FAILURE);

   PostCount = 0;
   SW_MUIListLoopSel(idata->SWA_NList_L, x, Post, TRUE)
      Posts[PostCount++] = Post;

   if ((Dest = NN_PostList(idata)) == o)
      Dest = NN_SvPostList(idata);

   set(idata->SWA_NList_L, MUIA_List_Quiet, TRUE);
   
   for (x=0; x<PostCount; x++)
      DoMethod(o, SWM_NPostList_XferPost, Posts[x], Dest);

   set(idata->SWA_NList_L, MUIA_List_Quiet, FALSE);

   rc = SUCCESS;
   
  Done:
   FreeVec(Posts);
   
   DoMethod(o, SWM_Root_CheckErr, _win(o));

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: return NGList class pointer                                       *
 *****************************************************************************/

METHOD(SWM_NPostList_NGClass, Msg)
{
   SWDB_RET(NGListClass());
}
