
# ifndef _GENPROTO

/*****************************************************************************
 * Method: Generate abbreviation of group's name.  buflen>=32 if not told.   *
 *****************************************************************************/

METHOD(SWM_NGroup_Abbrev, SWP_NGroup_Abbrev *)
{
   UBYTE *buf = msg->Buff, *name, *t;
   ULONG blen = msg->BuffLen, slen;

   if (!(t=buf) || !blen) SWDB_RET(0);
   buf[0] = '\0';

   name=idata->SWA_NGroup_Name;
   if (SW_STRISNULL(name)) SWDB_RET(0);

   while (name[0]) {
      if (name[slen = strcspn(name, ".")]=='\0') {
         strncpy(t, name, blen-1);
         t[min(blen-1,slen)]='\0';
         name += slen;
      } else if (blen > 2) {
         *t++  = name[0], *t++ = '.', *t = '\0';
         blen -= 2;
         name += slen + 1;
      }
   }

   SWDB_RET(buf);
} SDFT


/*****************************************************************************
 * Method: Find overview name for this group                                 *
 *****************************************************************************/

METHOD(SWM_NGroup_OVName, ULONG *)
{
   STRPTR name;
   STRPTR OVBuff = GetTagUB(SWT_NGroup_OVBuff,    NULL, msg+1);
   ULONG  OVBLen = GetTagUL(SWT_NGroup_OVBuffLen, 0,    msg+1);

   if (!OVBuff || OVBLen < 36) SWDB_RET(NULL);

   if (SW_STRISNULL(idata->SWA_NGroup_Name)) SWDB_RET(NULL);

   name = (STRPTR)DoMethod(NN_ArtTpl(idata), SWM_NArticle_HashFName,
                           SWT_NArticle_HashStr,  idata->SWA_NGroup_Name,
                           SWT_NArticle_HashBuff, OVBuff,
                           TAG_DONE);

   SWDB_RET(name);
} SDFT

   
/*****************************************************************************
 * Method: Mark article as read                                              *
 *****************************************************************************/

METHOD(SWM_NGroup_MarkRead, SWP_NGroup_MarkRead *)
{
   if (idata->SWA_NGroup_NewArtMd == SWV_NGroup_NA_Read ||
       idata->SWA_NGroup_NewArtMd == SWV_NGroup_NA_Both)
      DoMethod(msg->Art, SWM_NArticle_FlagReset, SWF_NArticle_NewArt);
   
   if (DoMethod(msg->Art, SWM_NArticle_FlagTest, SWF_NArticle_Read))
      SWDB_RET(NULL);

   DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_ReadListAdd, msg->Art);

   set(o, SWA_NGroup_ReadArtCnt, idata->SWA_NGroup_ReadArtCnt+1);

   DoMethod(o, SWM_NGroup_UpdArtList, msg->Art);

   SWDB_RET(NULL);
} SDFT


/*****************************************************************************
 * Method: Process clicks in article list                                    *
 *****************************************************************************/

METHOD(SWM_NGroup_ArtListClick, SWP_NGroup_ArtListClick *)
{
   Object *Active = SWGetObj(idata->ArtList, SWA_NArticleList_ClickItem);

   if (!Active) SWDB_RET(0);

   switch (idata->LastClickCol = msg->Col) {
     case SWV_NArticleList_A_Permanent: {
        ULONG PermFlags = DoMethod(Active, SWM_NArticle_FlagTest, CFlags);

        if ((PermFlags & CFlags) == 0) {
           // -- if not cached or permanent, set cached ----------------------
           DLQueueAdd(o, idata, Active, SWV_NGroup_DLQ_Cache, TRUE);
        } else if ((PermFlags & CFlags) == SWF_NArticle_CachedBody) {
           // -- if just cached, not perm, make perm -------------------------
           DLQueueAdd(o, idata, Active, SWV_NGroup_DLQ_MakePerm, TRUE);
        } else {
           DoMethod(Active, SWM_NArticle_SetPerm,
                    SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
                    SWT_NArticle_Perm, SWF_NArticle_PermNo,
                    TAG_DONE);
           CacheMk(o, idata, SWV_NGroup_MakeChMk);
        }

        DoMethod(o, SWM_NGroup_UpdArtList, Active);
        break;
     }
   }

   DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Process double clicks in article list                             *
 *****************************************************************************/

METHOD(SWM_NGroup_ArtListDClick, Msg)
{
   Object *Active;

   DoMethod(idata->ArtList, MUIM_List_GetEntry, MUIV_List_GetEntry_Active,
            &Active);
   
   if (!Active) SWDB_RET(0);
   
   switch (idata->LastClickCol) {
     case SWV_NArticleList_KeyCC:
     default:
        if (ThrdMode(idata)) {
           DoMethod(o, SWM_NGroup_ChgThreadFlt,
                    SWF_NGroup_ThreadFlt_1Thread, TRUE, TRUE);
        } else {
           if (Active) {
              DLQueueAdd(o, idata, Active, SWV_NGroup_DLQ_Read, TRUE);
              DoMethod(o, SWM_NGroup_UpdArtList, Active);
           }
        }
        break;
   }
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Toggle thread filtering between: all, thread entries, 1 thread    *
 *****************************************************************************/

METHOD(SWM_NGroup_ChgThreadFlt, SWP_NGroup_ChgThreadFlt *)
{
   Object *Active, *NewBt = NULL;
   ULONG Type        = msg->Type,
         OldType     = idata->SWA_NGroup_ThreadFT,
         EnterThread;

   if (!idata->ArtList) SWDB_RET(0);

   // -- inc/dec the thread display type -------------------------------------
   
   switch (msg->Type) {
     case SWF_NGroup_ThreadFlt_Up: Type=max(OldType-1, SWF_NGroup_ThreadFlt_All);
                                   break;

     case SWF_NGroup_ThreadFlt_Dn: Type=min(OldType+1, SWF_NGroup_ThreadFlt_1Thread);
                                   break;
   }

   DoMethod(idata->ArtList,MUIM_List_GetEntry,MUIV_List_GetEntry_Active,&Active);

   if (!Active)
      DoMethod(idata->ArtList, MUIM_List_GetEntry, 0, &Active);

   if (!Active && OldType == SWF_NGroup_ThreadFlt_1Thread)
      Type = SWF_NGroup_ThreadFlt_Base;
   
   if (Active && OldType == SWF_NGroup_ThreadFlt_Base)
      Active = (Object *)DoMethod(idata->SWA_NGroup_ThreadMgr,
                                  SWM_NArticleMgr_ThreadBase, Active);

   idata->PrevThPos[OldType] = 
      SWGetLong(SWGetObj(idata->ArtList, SWA_NList_L), MUIA_List_Active);

   set(o, SWA_NGroup_ThreadFT, Type);

   // -- perform needed filter changes ---------------------------------------
   
   switch (Type) {
     case SWF_NGroup_ThreadFlt_All:
      set(idata->Filter_ThBase,     SWA_NSFilter_Enabled, FALSE);
      set(idata->Filter_BaseMsgID,  SWA_NSFilter_Enabled, FALSE);
      set(idata->ArtList, SWA_NArticleList_Threaded, TRUE);
      set(idata->B_Up, MUIA_Disabled, TRUE);
      set(idata->B_Dn, MUIA_Disabled, FALSE);
      NewBt = idata->B_Th3;
      break;

     case SWF_NGroup_ThreadFlt_Base:
      set(idata->ArtList, SWA_NArticleList_Threaded, FALSE);
      set(idata->Filter_BaseMsgID,  SWA_NSFilter_Enabled, FALSE);
      set(idata->B_Up, MUIA_Disabled, FALSE);
      set(idata->B_Dn, MUIA_Disabled, FALSE);
      SetAttrs(idata->Filter_ThBase,
               SWA_NSFilter_Enabled, TRUE,
               SWA_NSFilter_Inv,    FALSE,
               TAG_DONE);
      NewBt = idata->B_Th2;
      break;

     case SWF_NGroup_ThreadFlt_1Thread: {
        if (Active) {
           STRPTR MsgID = (STRPTR)DoMethod(Active, SWM_NArticle_GetMsgID);

           set(idata->B_Up, MUIA_Disabled, FALSE);
           set(idata->B_Dn, MUIA_Disabled, TRUE);
           set(idata->Filter_ThBase, SWA_NSFilter_Enabled, FALSE);
           set(idata->ArtList, SWA_NArticleList_Threaded, TRUE);
           set(idata->Filter_BaseMsgID, SWA_NSFilter_Enabled, TRUE);
           SetAttrs(idata->Filter_MsgID, 
                    SWA_NSF_SubjMatch_MatchMsgID,  TRUE,
                    SWA_NSF_SubjMatch_Str,         MsgID,
                    TAG_DONE);
           NewBt = idata->B_Th1;
        }
        break;
     }
   }

   // -- fix the thread display button ---------------------------------------
   
   if (NewBt && idata->G_Thrd) {
      DoMethod(idata->G_Thrd, MUIM_Group_InitChange);
      if (idata->B_ThCnt)
         DoMethod(idata->G_Thrd, OM_REMMEMBER, idata->B_ThCnt);
      
      DoMethod(idata->G_Thrd, OM_ADDMEMBER, idata->B_ThCnt = NewBt);
      DoMethod(idata->G_Thrd, MUIM_Group_ExitChange);
   }

   if (msg->Immediate) DoMethod(o, SWM_NGroup_ChgSelFlt, 0);

   // -- if asked to, display an article -------------------------------------
   
   if (msg->DoAction &&
       (Type    == SWF_NGroup_ThreadFlt_1Thread &&
        (OldType == SWF_NGroup_ThreadFlt_Base ||
         OldType == SWF_NGroup_ThreadFlt_All)   &&
        (EnterThread = NN_EnterThread(idata)) != SWV_NNews_ET_NoAction)) {

      DoMethod(o, SWM_NGroup_Browse, 1, 
               EnterThread == SWV_NNews_ET_FirstAvail ?
                  SWV_NGroup_Browse_1stAvail : SWV_NGroup_Browse_1stUnread);
   }
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Toggle read filtering between: all, only read, only unread        *
 *****************************************************************************/

METHOD(SWM_NGroup_ChgReadFlt, SWP_NGroup_ChgReadFlt *)
{
   ULONG type = msg->Type;
   
   if (!idata->ArtList) SWDB_RET(0);

   if (type == SWV_NGroup_ReadFlt_Buttons) {
      ULONG sr = SWGetUL(idata->B_Read, MUIA_Selected),
            su = SWGetUL(idata->B_Unrd, MUIA_Selected);

      if ( sr &&  su) type = SWF_NGroup_ReadFlt_All;
      if (!sr && !su) type = SWF_NGroup_ReadFlt_All;
      if ( sr && !su) type = SWF_NGroup_ReadFlt_Read;
      if (!sr &&  su) type = SWF_NGroup_ReadFlt_Unread;
   } else {
      nnset(idata->B_Read, MUIA_Selected, type == SWF_NGroup_ReadFlt_Read);
      nnset(idata->B_Unrd, MUIA_Selected, type == SWF_NGroup_ReadFlt_Unread);
   }
   
   set(o, SWA_NGroup_ReadFT, type);

   switch (type) {
     case SWF_NGroup_ReadFlt_All:
      set(idata->Filter_Read,   SWA_NSFilter_Enabled, FALSE);
      break;

     case SWF_NGroup_ReadFlt_Read:
     case SWF_NGroup_ReadFlt_Unread:
      SetAttrs(idata->Filter_Read,
               SWA_NSFilter_Enabled, TRUE,
               SWA_NSFilter_Inv, type == SWF_NGroup_ReadFlt_Unread,
               TAG_DONE);
      break;
   }

   if (msg->Immediate) DoMethod(o, SWM_NGroup_ChgUIFlt, 0);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Toggle Author filtering                                           *
 *****************************************************************************/

METHOD(SWM_NGroup_ChgAuthFlt, SWP_NGroup_ChgAuthFlt *)
{
   Object *Active, Type = msg->Type;
   STRPTR Auth;

   if (!idata->ArtList) SWDB_RET(0);

   DoMethod(idata->ArtList, MUIM_List_GetEntry,
            MUIV_List_GetEntry_Active, &Active);

   if ((LONG)Active == MUIV_List_Active_Off) Type = SWF_NGroup_AuthFlt_All;

   nnset(idata->B_Auth, MUIA_Selected, Type);

   set(o, SWA_NGroup_AuthFT, Type);

   switch (Type) {
     case SWF_NGroup_AuthFlt_All:
      set(idata->Filter_Auth, SWA_NSFilter_Enabled, FALSE);
      set(o, SWA_NGroup_AuthFTName, "");
      break;

     case SWF_NGroup_AuthFlt_1Auth:
      Auth = (STRPTR)DoMethod(Active, SWM_NArticle_FindHeader, SWF_NAHeader_From);

      if (Auth) {
         set(o, SWA_NGroup_AuthFTName, Auth);
         set(idata->Filter_Auth,  SWA_NSFilter_Enabled, TRUE);
         set(idata->StrF_Auth,    SWA_NStrFilter_Pattern, Auth);
      }
      break;
   }

   if (msg->Immediate) DoMethod(o, SWM_NGroup_ChgUIFlt, 0);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Re-display based on new UI filters                                *
 *****************************************************************************/

METHOD(SWM_NGroup_ChgUIFlt, SWP_NGroup_ChgUIFlt *)
{
   ULONG AddMode = msg->First != 0,
         First   = AddMode ? idata->UIArticleCnt : 0;

   if (!idata->ArtList || !idata->GrpArticles) SWDB_RET(0);

   idata->UIArticleCnt =
      DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_FilterList,
               idata->SelArticles   + (AddMode ? msg->First : 0),
               idata->SelArticleCnt - (AddMode ? msg->First : 0),
               &idata->UIArticles, &idata->UIArticleMax,
               AddMode ? idata->UIArticleCnt : 0,
               ThrdMode(idata), 1, FALSE, o,
               idata->Filter_Read,
               idata->Filter_Auth,
               idata->Filter_ThBase,
               idata->Filter_BaseMsgID,
               NULL);

   ArtListAdd(idata, idata->UIArticles + First,
              idata->UIArticleCnt - First, AddMode ? FALSE : TRUE);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Re-display based on new select filters                            *
 *****************************************************************************/

METHOD(SWM_NGroup_ChgSelFlt, SWP_NGroup_ChgSelFlt *)
{
   ULONG AddMode = msg->First != 0;
   ULONG First   = AddMode ? idata->SelArticleCnt : 0;
   ULONG *FltMsg;
   Object *Flt;
   LONG  x, p = 0;
   
   if (!idata->ArtList) SWDB_RET(0);

   if (!(FltMsg = (ULONG *)AV((32 +
       SWGetUL(idata->SWA_NGroup_SelFlt, MUIA_List_Entries) +
       SWGetUL(idata->Filter_GlobSel,    MUIA_List_Entries)) * sizeof(ULONG))))
      SWDB_RET(0);

   FltMsg[p++] = (ULONG)(SWM_NArticleMgr_FilterList),
   FltMsg[p++] = (ULONG)(idata->GrpArticles+msg->First),
   FltMsg[p++] = (ULONG)(idata->GrpArticleCnt-msg->First),
   FltMsg[p++] = (ULONG)(&idata->SelArticles),
   FltMsg[p++] = (ULONG)(&idata->SelArticleMax),
   FltMsg[p++] = (ULONG)(AddMode ? idata->SelArticleCnt : 0),
   FltMsg[p++] = (ULONG)(ThrdMode(idata)),
   FltMsg[p++] = (ULONG)(SWGetUL(idata->Filter_GlobSel, SWA_NSFList_Mode) ==
                         SWV_NSFList_And) ? 1 : 2,
   FltMsg[p++] = (ULONG)(FALSE),
   FltMsg[p++] = (ULONG)(o);

   SW_MUIListLoop(idata->SWA_NGroup_SelFlt, x, Flt, TRUE)
      if (SWGetUL(Flt, SWA_NSFilter_Enabled))
         FltMsg[p++] = (ULONG)Flt;

   SW_MUIListLoop(idata->Filter_GlobSel, x, Flt, TRUE)
      if (SWGetUL(Flt, SWA_NSFilter_Enabled))
         FltMsg[p++] = (ULONG)Flt;

   FltMsg[p] = NULL;

   idata->SelArticleCnt = DoMethodA(idata->SWA_NGroup_ArtMgr, (Msg)FltMsg);

   FV(FltMsg);
   
   SWDB_RET(DoMethod(o, SWM_NGroup_ChgUIFlt, First));
} SDFT


/*****************************************************************************
 * Method: Mark selected articles as read or unread                          *
 *****************************************************************************/

METHOD(SWM_NGroup_Mark, SWP_NGroup_Mark *)
{
   ULONG x, Count = idata->GrpArticleCnt;
   ULONG Threads = msg->Threads || ThrdMode(idata);
   Object **a = idata->GrpArticles;
   
   AppSleep(idata, TRUE);

   if (!msg->All)
      if (!(a = GetSelArts(idata, Threads, &Count))) goto Done;

   for (x=0; a && x<Count; x++)
      if (msg->Mode == 1)
         DoMethod(idata->SWA_NGroup_ArtMgr,
                  msg->Val ? SWM_NArticleMgr_ReadListAdd :
                             SWM_NArticleMgr_ReadListRem,
                  a[x]);
      else if (msg->Mode == 0)
         DoMethod(a[x],
                  msg->Val  ? SWM_NArticle_FlagSet : SWM_NArticle_FlagReset,
                  SWF_NArticle_UserMk);
      else if (msg->Mode == 2)
         DoMethod(a[x], SWM_NArticle_FlagReset, SWF_NArticle_NewArt);

   if (msg->Mode == 1 && !msg->Val)
      DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_ReadListRemBl);

   DoMethod(o, SWM_NGroup_UpdArtList, Count == 1 ? a[0] : NULL);

   if (!msg->All)
      FV(a);

   CountRead(o, idata);

  Done:
   AppSleep(idata, FALSE);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Data: compare group attributes                                            *
 *****************************************************************************/

static STRPTR CmpFields[] = {
   "(Off)",
   "Group Name",
   "Last Read",
   "New Articles",
   "Avail Articles",
   "Article Count",
   "Unread Articles",
   "Server Name",
   NULL,
};


/*****************************************************************************
 * Function: Compare 2 groups                                                *
 *****************************************************************************/

static LONG GrpCmp(Object *o1, Object *o2, Class *cl,
                   struct SWS_CmpInfo *CmpInfo)
{
   LONG Pos, Cmp = 0, t1, t2;
   SW_IDATA *idata1 = INST_DATA(cl, o1);
   SW_IDATA *idata2 = INST_DATA(cl, o2);

   if (o1 == o2) return 0;

   for (Pos=0; Pos < CmpInfo->Count; Pos++) {
      switch (CmpInfo->Attrs[Pos]) {
        case SWV_NGroup_SortOff:  break;

        case SWV_NGroup_SortName:
         Cmp = strcmp(idata1->SWA_NGroup_Name, idata2->SWA_NGroup_Name);
         break;

        case SWV_NGroup_SortLastRead:
         Cmp = idata1->SWA_NGroup_RdDays - idata2->SWA_NGroup_RdDays;
         if (!Cmp) Cmp = idata1->SWA_NGroup_RdMin  - idata2->SWA_NGroup_RdMin;
         if (!Cmp) Cmp = idata1->SWA_NGroup_RdTick - idata2->SWA_NGroup_RdTick;
         break;

        case SWV_NGroup_SortNewCnt:
         t1 = min(idata1->SWA_NGroup_AvailArtNum, idata1->SWA_NGroup_NewArtNum);
         t2 = min(idata2->SWA_NGroup_AvailArtNum, idata2->SWA_NGroup_NewArtNum);
         Cmp = t1 - t2;
         break;
         
        case SWV_NGroup_SortAvailCnt:
         Cmp = idata1->SWA_NGroup_AvailArtNum - idata2->SWA_NGroup_AvailArtNum;
         break;

        case SWV_NGroup_SortArtCnt:
         Cmp = idata1->SWA_NGroup_TotArtCnt - idata2->SWA_NGroup_TotArtCnt;
         break;

        case SWV_NGroup_SortUnreadCnt:
         t1 = idata1->SWA_NGroup_TotArtCnt - idata1->SWA_NGroup_ReadArtCnt;
         t2 = idata2->SWA_NGroup_TotArtCnt - idata2->SWA_NGroup_ReadArtCnt;
         Cmp = t1 - t2;
         break;

        case SWV_NGroup_SortServer:
         Cmp = strcmp(idata1->SWA_NGroup_Server1, idata2->SWA_NGroup_Server1);
         break;

        default: break;
      }

      if (Cmp) break;
   }

   if (!Cmp) return Cmp;
   
   return (CmpInfo->Dirs[Pos] == SWV_CmpForwardSort) ? Cmp : -Cmp;
}


/*****************************************************************************
 * Method: Compare 2 groups                                                  *
 *****************************************************************************/

METHOD(SWM_NGroup_Cmp, SWP_Cmp *)
{
   if (!msg->Entry) SWDB_RET(CmpFields);

   SWDB_RET(GrpCmp(o, msg->Entry, cl, msg->CmpInfo));
} SDFT


/*****************************************************************************
 * Method: Batch DL headers                                                  *
 *****************************************************************************/

METHOD(SWM_NGroup_GL_Hdrs, ULONG *)
{
   if (idata->SrvInfo[SWV_GC_GL_Arts].Srv) SWDB_RET(FAILURE);
   
   SWDB_RET(BeginLFCmd(o, idata, SWV_GC_GL_Hdrs, SWM_NGroup_GL_Hdrs_P,
                       SWM_NGroup_Fetch, msg));
} SDFT


/*****************************************************************************
 * Method: Batch DL headers post                                             *
 *****************************************************************************/

METHOD(SWM_NGroup_GL_Hdrs_P, ULONG *)
{
   LONG State  = GetTagUL(SWT_NGroup_CmdState, 0,  msg+1);
   Object *Srv = GetTagObj(SWT_NGroup_Server,  0,  msg+1);
   ULONG    ID = GetTagUL(SWT_NGroup_ID,       0,  msg+1);

   if (State == SWV_NGroup_Begin) {
      idata->SrvInfo[SWV_GC_GL_Hdrs].Srv     = Srv;
      idata->SrvInfo[SWV_GC_GL_Hdrs].ID      = ID;
   }

   if (State == SWV_NGroup_NewProgMsg) {
      idata->SrvInfo[SWV_GC_GL_Hdrs].ProgMsg =
         idata->SrvInfo[SWV_GC_FetchHdr].ProgMsg;

      SendCmdState(o, idata, SWV_GC_GL_Hdrs, SWV_NGroup_NewProgMsg);
   }

   if (State == SWV_NGroup_Abort || State == SWV_NGroup_End) {
      KillFltArray(o, idata, idata->GrpArticles, idata->GrpArticleCnt);

      idata->SrvInfo[SWV_GC_GL_Hdrs].Srv     = NULL;
      idata->SrvInfo[SWV_GC_GL_Hdrs].ID      = -1;
      idata->SrvInfo[SWV_GC_GL_Hdrs].ProgMsg = NULL;
   }
   
   SWDB_RET(EndLFCmd(o, idata, SWV_GC_GL_Hdrs, State));
} SDFT


/*****************************************************************************
 * Method: Batch DL articles                                                 *
 *****************************************************************************/

METHOD(SWM_NGroup_GL_Arts, ULONG *)
{
   if (idata->SrvInfo[SWV_GC_GL_Hdrs].Srv) SWDB_RET(FAILURE);

   idata->SimulCmds  = NN_SimulCmds(idata);
   idata->SimulCmds  = max(idata->SimulCmds, 1);
   
   SWDB_RET(BeginLFCmd(o, idata, SWV_GC_GL_Arts, SWM_NGroup_GL_Arts_P1,
                       SWM_NGroup_Fetch, msg));
} SDFT


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   ULONG x;
   UBYTE c;

   if (!idata->VarBuf)
      idata->VarBuf = AllocVec(strlen(idata->SWA_NGroup_Name)+64, MEMF_ANY);

   if (!idata->VarBuf) SWDB_RET(NULL);
   
   VL_FindRet("GroupName", idata->SWA_NGroup_Name);

   VL_FindRet("GroupAbbrev",
              DoMethod(o, SWM_NGroup_Abbrev, idata->VarBuf, 63));
   
   VL_FindRetNum("GroupMaxArticles", idata->SWA_NGroup_MaxArticles,
                 idata->VarBuf, "%ld");

   VL_FindRetNum("GroupReadArticles", idata->SWA_NGroup_ReadArtCnt,
                 idata->VarBuf, "%ld");

   VL_FindRetNum("GroupTotArticles", idata->SWA_NGroup_TotArtCnt,
                 idata->VarBuf, "%ld");

   VL_FindRetNum("GroupUnreadArticles",
                 idata->SWA_NGroup_TotArtCnt - idata->SWA_NGroup_ReadArtCnt,
                 idata->VarBuf, "%ld");

   // -- handle ${GroupNameChr,/} - transform . to / -------------------------
   
   if (!strncmp(msg->Name, "GroupNameChr", 12)) {
      strcpy(idata->VarBuf, idata->SWA_NGroup_Name);

      c = '/';
      if (msg->Name[12] && msg->Name[13])
         c = msg->Name[13];

      for (x=0; idata->VarBuf[x]; x++)
         if (idata->VarBuf[x] == '.')
            idata->VarBuf[x] = c;

      SWDB_RET(idata->VarBuf);
   }

   SWDB_RET(DoMethodA(idata->SWA_AppBase_TopObj, (Msg)msg));
} SDFT


/*****************************************************************************
 * Method: Save/Load: get file/dir/pattern                                   *
 *****************************************************************************/

METHOD(SWM_Root_RequestFNData, SWP_Root_RequestFNData *)
{
   SetAttrs(o, MUIA_Group_Forward,        FALSE,
               SWA_NGroup_LastFile,       msg->FSpec,
               SWA_NGroup_LastDir,        msg->Dir,
               SWA_NGroup_LastPatt,       msg->Patt,
               TAG_DONE); 

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: post new article or followup                                      *
 *****************************************************************************/

METHOD(SWM_NGroup_Post, SWP_NGroup_Post *)
{
   SWDB_RET(DoMethod(NN_PostList(idata), SWM_NPostList_GenReply,
                     o, msg->Art, idata->ReaderWin));
} SDFT


/*****************************************************************************
 * Method: post new article                                                  *
 *****************************************************************************/

METHOD(SWM_NGroup_PostNew, Msg)
{
   SWDB_RET(DoMethod(o, SWM_NGroup_Post, NULL));
} SDFT


/*****************************************************************************
 * Method: post followup article                                             *
 *****************************************************************************/

METHOD(SWM_NGroup_PostFup, Msg)
{
   SWDB_RET(DoMethod(o, SWM_NGroup_Post,
                     SWGetObj(idata->SWA_NGroup_ArtView,
                              SWA_NArticleView_DspArt)));
} SDFT


/*****************************************************************************
 * Method: Open search window                                                *
 *****************************************************************************/

METHOD(SWM_NGroup_ALQSWin, Msg)
{
   SWDB_RET(ALQSWin(o, idata));
} SDFT


/*****************************************************************************
 * Method: Return group's overview directory                                 *
 *****************************************************************************/

METHOD(SWM_NGroup_OVDir, Msg)
{
   SWDB_RET(OVDir(o, idata, NULL));
} SDFT


/*****************************************************************************
 * Method: Add article to DL queue                                           *
 *****************************************************************************/

METHOD(SWM_NGroup_DLQAdd, SWP_NGroup_DLQAdd *)
{
   SWDB_RET(DLQueueAdd(o, idata, msg->Art, msg->Cmd, msg->Last));
} SDFT


/*****************************************************************************
 * Method: artlist UI search update                                          *
 *****************************************************************************/

METHOD(SWM_NGroup_ALSearchUIUpd, Msg)
{
   SWDB_RET(ALSearchUIUpd(o, idata));
} SDFT


/*****************************************************************************
 * Method: limit artlist to selected articles only                           *
 *****************************************************************************/

METHOD(SWM_NGroup_LimitToSel, Msg)
{
   SWDB_RET(LimitSel(o, idata));
} SDFT


/*****************************************************************************
 * Method: limit artlist to selected articles only                           *
 *****************************************************************************/

METHOD(SWM_NGroup_Redisp, SWP_NGroup_Redisp *)
{
   switch (msg->Type) {
     case SWV_NGroup_Redisp_Raw:
      DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_DispRaw, NULL);
      break;

     case SWV_NGroup_Redisp_Cooked:
      DoMethod(idata->SWA_NGroup_ArtView, SWM_NArticleView_DispCooked, NULL);
      break;
   }

   SWDB_RET(SUCCESS);   
} SDFT

# endif
