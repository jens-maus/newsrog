# include "C-NGroup-I.h"

# ifndef _GENPROTO

/*****************************************************************************
 * Function: Start with correct article                                      *
 *****************************************************************************/

void MoveToStartArt(Object *o, SW_IDATA *idata)
{
   ULONG  SArt;
   
   switch (SArt = NN_StartArt(idata)) {
     case SWV_NNews_SA_PrevArt:
     case SWV_NNews_SA_PrevThread:
      DoMethod(o, SWM_NGroup_ViewMsgID,
               SWT_NGroup_VMID_MsgID,      idata->SWA_NGroup_LastMsgID,
               SWT_NGroup_VMID_UpdArtView, FALSE,
               SWT_NGroup_VMID_NoReq,      TRUE,
               SWT_NGroup_VMID_BaseArt,    SArt == SWV_NNews_SA_PrevThread,
               TAG_DONE);
      break;

     case SWV_NNews_SA_First:
     case SWV_NNews_SA_Last:
      set(idata->ArtList, MUIA_List_Active,
          SArt == SWV_NNews_SA_First ? MUIV_List_Active_Top :
                  MUIV_List_Active_Bottom);
      break;
   }
}


/*****************************************************************************
 * Function: Initialize debug stuff                                          *
 *****************************************************************************/

void OpenDBFiles(Object *o, SW_IDATA *idata)
{
   TEXT  FSpec[FMSIZE], DBVar[64];
   ULONG OpenDBFile = FALSE;

   idata->DBType = 0;

   if (GetVar("NewsRog/Debug", DBVar, sizeof(DBVar)-1, 0) <= 0)
      return;

   if (strstr(DBVar, "GROUPFETCH"))
      OpenDBFile = TRUE, FSet(idata->DBType, SWV_DB_Fetch);

   if (strstr(DBVar, "GROUPBATCHDL"))
      OpenDBFile = TRUE, FSet(idata->DBType, SWV_DB_BatchDL);

   if (!OpenDBFile) return;

   strcpy(FSpec, "RAM:NR.GROUP.");
   DoMethod(o, SWM_NGroup_Abbrev, FSpec + strlen(FSpec), FMSIZE - strlen(FSpec) - 1);

   idata->DBOut = SWOpenAppend(FSpec);
}


/*****************************************************************************
 * Function: Set window menus/controls                                       *
 *****************************************************************************/

void SetWinCtrls(Object *o, SW_IDATA *idata, Object *Win)
{
   if (!Win) return;
   
   DoMethod(Win, MUIM_Notify, MUIA_Window_MenuAction,
            MUIV_EveryTime, o, 2, SWM_NGroup_MenuCall, MUIV_TriggerValue);

   DoMethod(Win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            MUIV_Notify_Application, 5,
            MUIM_Application_PushMethod, o, 3, SWM_NGroup_StopRead, TRUE,FALSE);

   // -- add keyboard controls -----------------------------------------------

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad 3", o, 3, SWM_NGroup_Browse, 1, SWV_NGroup_Browse_Browse);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad 9", o, 3, SWM_NGroup_Browse, -1, SWV_NGroup_Browse_Browse);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad 1", idata->SWA_NGroup_ArtView, 1, SWM_NArticleView_Bottom);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad 7", idata->SWA_NGroup_ArtView, 1, SWM_NArticleView_Top);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat 2", idata->SWA_NGroup_ArtView, 3,
            SWM_NArticleView_Scroll, 1, 0);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat 8", idata->SWA_NGroup_ArtView, 3,
            SWM_NArticleView_Scroll, -1, 0);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat 4", idata->SWA_NGroup_ArtView, 3,
            SWM_NArticleView_Scroll, 0, -12);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat 6", idata->SWA_NGroup_ArtView, 3,
            SWM_NArticleView_Scroll, 0, 12);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat enter", o, 1, SWM_NGroup_ArtListDClick);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat --", o, 3, SWM_NGroup_Browse, -1,
            SWV_NGroup_Browse_ArtMov);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat +", o, 3, SWM_NGroup_Browse, 1,
            SWV_NGroup_Browse_ArtMov);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "backspace", o, 4, SWM_NGroup_ChgThreadFlt,
            SWF_NGroup_ThreadFlt_Up, TRUE, TRUE);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "ramiga c", idata->SWA_NGroup_ArtView, 1, SWM_NArticleView_Clip);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat )", o, 3, SWM_NGroup_Browse,
            1, SWV_NGroup_Browse_ThreadMov);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "numericpad -repeat (", o, 3, SWM_NGroup_Browse,
            -1, SWV_NGroup_Browse_ThreadMov);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "1", o, 5, SWM_NGroup_WinCtrl,
            SWV_NGroup_WC_AL, SWV_NGroup_WC_Front,
            SWV_NGroup_WC_Keep, SWV_NGroup_WC_Keep);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "2", o, 5, SWM_NGroup_WinCtrl,
            SWV_NGroup_WC_AL, SWV_NGroup_WC_Back,
            SWV_NGroup_WC_Keep, SWV_NGroup_WC_Keep);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "3", o, 5, SWM_NGroup_WinCtrl,
            SWV_NGroup_WC_AV, SWV_NGroup_WC_Front,
            SWV_NGroup_WC_Keep, SWV_NGroup_WC_Keep);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "4", o, 5, SWM_NGroup_WinCtrl,
            SWV_NGroup_WC_AV, SWV_NGroup_WC_Back,
            SWV_NGroup_WC_Keep, SWV_NGroup_WC_Keep);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "z", o, 2, SWM_NGroup_Remove, FALSE);

   DoMethod(Win, MUIM_Notify, MUIA_Window_InputEvent,
            "Z", o, 2, SWM_NGroup_Remove, TRUE);

   DoMethod(idata->App, OM_ADDMEMBER, Win);
   set(Win, MUIA_Window_Open,      TRUE);
}


/*****************************************************************************
 * Function: Change highlight filter settings                                *
 *****************************************************************************/

ULONG ChgHLFlt(Object *o, SW_IDATA *idata, Object **Articles, ULONG ArtCnt)
{
   ULONG rc = FAILURE;
   ULONG x;

   if (!Articles) return FAILURE;
   if (!ArtCnt)   return SUCCESS;

   if (DoMethod(idata->Filter_GlobHL, SWM_NSFilter_Begin, o) != SUCCESS)
      goto Done;

   for (x=0; x<ArtCnt; x++)
      DoMethod(Articles[x],
               DoMethod(idata->Filter_GlobHL, SWM_NSFilter_Match, Articles[x], o) ?
                  SWM_NArticle_FlagSet : 
                  SWM_NArticle_FlagReset,
               SWF_NArticle_Highlight);

   rc = SUCCESS;
   
  Done:
   DoMethod(idata->Filter_GlobHL, SWM_NSFilter_End, o);

   return rc;
}


/*****************************************************************************
 * Function: Make sort notifications                                         *
 *****************************************************************************/

void SetSortNotify(Object *o, SW_IDATA *idata)
{
   ULONG x;
   
   struct { ULONG s1, s2; } N[] =
   {
      SWA_NArticleList_Sort1, SWA_NGroup_Sort1,
      SWA_NArticleList_Sort2, SWA_NGroup_Sort2,
      SWA_NArticleList_Sort3, SWA_NGroup_Sort3,
      SWA_NArticleList_Dir1,  SWA_NGroup_Dir1,
      SWA_NArticleList_Dir2,  SWA_NGroup_Dir2,
      SWA_NArticleList_Dir3,  SWA_NGroup_Dir3,
      NULL,                   NULL,
   };

   for (x=0; N[x].s1; x++)
      DoMethod(idata->ArtList, MUIM_Notify, N[x].s1, MUIV_EveryTime,
               o, 3, MUIM_NoNotifySet, N[x].s2, MUIV_TriggerValue);
}


/*****************************************************************************
 * Function: get list of selected articles (be smart about threads)          *
 *    returns NULL, or Object * list terminated by NULL                      *
 *****************************************************************************/

ULONG IncArray(SW_IDATA *idata, ULONG Count, ULONG Size, Object ***a)
{
   if (Count >= Size) {
      Size += 50;
      if (RV(Size * sizeof(Object *), a) < 0) return 0;
   }

   return Size;
}


Object **GetSelArts(SW_IDATA *idata, ULONG Threads, ULONG *Count)
{
   Object **a = NULL, **b = NULL, *lo;
   ULONG  Size = 0, C2, y, x;

   if (!idata->ArtList) return NULL;
   
   *Count = 0;

   // -- get list of all articles --------------------------------------------

   SW_MUIListLoopSel(idata->ArtList, x, lo, TRUE) {
      Size = IncArray(idata, *Count, Size, &a);
      a[(*Count)++] = lo;
   }

   if (!Threads) return a;

   // -- in thread mode, get thread base for all selected --------------------

   for (x=0; x<*Count; x++)
      a[x]=(Object *)DoMethod(idata->SWA_NGroup_ThreadMgr,
                              SWM_NArticleMgr_ThreadBase, a[x]);

   // -- now remove the duplicates -------------------------------------------

   for (x=0; x<*Count; x++)
      if (a[x])
         for (y=x+1; y<*Count; y++)
            if (a[x] == a[y] && x != y)
               a[y] = NULL;

   C2     = *Count;
   *Count = 0;
   Size   = 0;

   // -- now get all articles in the same threads ----------------------------

   for (x=0; x<C2; x++) {
      Object **Repl;
      
      if (a[x]) {
         Size = IncArray(idata, *Count, Size, &b);
         b[(*Count)++] = a[x];

         if (Repl = (Object **)DoMethod(idata->SWA_NGroup_ThreadMgr,
                                        SWM_NArticleMgr_GetThreadArts, a[x],
                                        NULL)) {
            for (y=0; !ThreadArtEnd(Repl[y]); y++) {
               Size = IncArray(idata, *Count, Size, &b);
               b[(*Count)++] = Repl[y];
            }
         }
      }
   }

   FV(a);

   return b;
}


/*****************************************************************************
 * Function: Return group cache directory                                    *
 *****************************************************************************/

STRPTR OVDir(Object *o, SW_IDATA *idata, STRPTR Sub)
{
   STRPTR OVRoot = NN_CacheDir(idata);
   UBYTE fb[36], *ovn;

   if (idata->OVDir) return idata->OVDir;

   if (!idata->OVDir) idata->OVDir = AV(FMSIZE);

   if (!idata->OVDir) return NULL;

   OVRoot = SW_STRDEF(OVRoot, SWV_DefOVRoot);
   
   if (!(ovn = (STRPTR)DoMethod(o, SWM_NGroup_OVName,
                                SWT_NGroup_OVBuff,    fb,
                                SWT_NGroup_OVBuffLen, 36,
                                TAG_DONE)))
      return NULL;

   AddPart(strcpy(idata->OVDir, OVRoot), SWV_NGroup_GroupDir, FMSIZE);
   AddPart(idata->OVDir,                 ovn,                 FMSIZE);

   if (Sub)
      AddPart(idata->OVDir,              Sub,                 FMSIZE);

   return idata->OVDir;
}


/*****************************************************************************
 * Function: Warn/ask about uncaching articles                               *
 *****************************************************************************/

LONG AskUnCache(Object *o, SW_IDATA *idata, ULONG buttons, ULONG prompt,
                ULONG Save, LONG DefReply, LONG RemReply,
                LONG KeepReply)
{
   LONG  rc = DefReply;
   ULONG Count;
   Object *CFilter = MUI_NewObject(SWC_NSF_Cached, TAG_DONE);

   if (!idata->SWA_NGroup_ArtMgr || !idata->ReaderWin || !CFilter) goto Done;
   
   Count = DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_Match, NULL,
                    1, FALSE, o, idata->SWA_NGroup_GrpDefFlt, CFilter, NULL);

   if (Count || !Save) {
      if (Save) {
         if (idata->SWA_NGroup_CacheExit == SWV_NGroup_CE_Keep)
            rc = KeepReply;
         if (idata->SWA_NGroup_CacheExit == SWV_NGroup_CE_Del)
            rc = RemReply;
      }

      if (!Save || idata->SWA_NGroup_CacheExit == SWV_NGroup_CE_Ask)
         rc = MUI_Request(NN_App(idata), idata->ReaderWin, 0, NULL,
                          GetS(buttons), GetS(prompt), Count);
   }

   if (rc == RemReply)
      DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_UnCache,
               OVDir(o, idata, NULL),
               1, FALSE, o, idata->SWA_NGroup_GrpDefFlt, CFilter, NULL);

   MUI_DisposeObject(CFilter);

  Done:
   return rc;
}


/*****************************************************************************
 * Function: Start gauge for some operation                                  *
 *****************************************************************************/

void UpdGaugeStat(Object *o, SW_IDATA *idata, STRPTR Status, LONG Cmd)
{
   if (idata->ProgPos == Cmd)
      set(o, SWA_NGroup_Status, idata->ProgInfo[Cmd].Status = Status);
}


/*****************************************************************************
 * Function: Keep track of some object's percent attribute in the gauge      *
 *****************************************************************************/

void KillGaugeNfy(Object *o, SW_IDATA *idata, LONG Cmd)
{
   if (idata->ProgInfo[Cmd].Attr)
      DoMethod(idata->ProgInfo[Cmd].Obj, MUIM_KillNotifyObj,
               idata->ProgInfo[Cmd].Attr, idata->ProgGauge);

   if (idata->ProgInfo[Cmd].AbortButton)
      DoMethod(idata->B_ProgStop, MUIM_KillNotify, MUIA_Pressed);
   
   set(idata->ProgGauge, MUIA_Gauge_Current, 0);
   set(o, SWA_NGroup_Status, GetS(SWS_L_Status_Idle));
}


void InitGaugeNfy(Object *o, SW_IDATA *idata, LONG Cmd)
{
   if (Cmd < 0) {
      set(idata->B_ProgStop, MUIA_Disabled, TRUE);
      DoMethod(idata->B_ProgStop, MUIM_KillNotify, MUIA_Pressed);
      return;
   }

   if (idata->ProgInfo[Cmd].AbortButton) {
      set(idata->B_ProgStop, MUIA_Disabled, FALSE);
      DoMethod(NN_App(idata), MUIM_Application_PushMethod, 
               idata->B_ProgStop, 7, NOTP, o, 2, SWM_NGroup_Abort, Cmd);
   }

   if (idata->ProgInfo[Cmd].Attr) {
      DoMethod(idata->ProgInfo[Cmd].Obj, MUIM_Notify, idata->ProgInfo[Cmd].Attr,
               MUIV_EveryTime, idata->ProgGauge, 3,
               MUIM_Set, MUIA_Gauge_Current, MUIV_TriggerValue);

      set(idata->ProgGauge, MUIA_Gauge_Current,
          SWGetUL(idata->ProgInfo[Cmd].Obj, idata->ProgInfo[Cmd].Attr));
   }

   UpdGaugeStat(o, idata, idata->ProgInfo[Cmd].Status, Cmd);
}


/*****************************************************************************
 * Function: Start gauge for some operation                                  *
 *****************************************************************************/

void StartGauge(Object *o, SW_IDATA *idata, Object *Track, ULONG Attr,
                STRPTR Status, LONG Cmd, ULONG AbortButton)
{
   if (!Track || !idata->ProgGauge) return;

   if (idata->ProgPos >= 0)
      KillGaugeNfy(o, idata, idata->ProgPos);
   
   idata->ProgInfo[Cmd].Obj         = Track;
   idata->ProgInfo[Cmd].Attr        = Attr;
   idata->ProgInfo[Cmd].Status      = Status;
   idata->ProgInfo[Cmd].AbortButton = AbortButton;
   idata->ProgPos                   = Cmd;

   InitGaugeNfy(o, idata, Cmd);
}


/*****************************************************************************
 * Function: Stop gauge for some operation (must call for each StartGauge)   *
 *****************************************************************************/

void StopGauge(Object *o, SW_IDATA *idata, LONG Cmd)
{
   ULONG x;

   if (!idata->ProgGauge)         return;
   if (!idata->ProgInfo[Cmd].Obj) return;

   KillGaugeNfy(o, idata, Cmd);

   idata->ProgInfo[Cmd].Obj    = NULL;
   idata->ProgInfo[Cmd].Attr   = 0;
   idata->ProgInfo[Cmd].Status = NULL;

   idata->ProgPos              = -1;

   for (x=0; x<SWV_GC_MAX; x++)
      if (idata->ProgInfo[x].Obj)
         idata->ProgPos = x;

   InitGaugeNfy(o, idata, idata->ProgPos);
}


/*****************************************************************************
 * Function: Init fields that change when group name does                    *
 *****************************************************************************/

void InitNameFields(Object *o, SW_IDATA *idata, STRPTR NewName)
{
   if (NewName && strcmp(NewName, idata->SWA_NGroup_Name) == 0) return;

   if (!idata->SWA_NGroup_RdDays) return;
   
   SetAttrs(o,
            SWA_NGroup_Desc,         0,
            SWA_NGroup_RdDays,       0,
            SWA_NGroup_RdMin,        0,
            SWA_NGroup_RdTick,       0,
            SWA_NGroup_HistArtCount, 0,
            SWA_NGroup_HistArtKill,  0,
            SWA_NGroup_HistArtRead,  0,
            SWA_NGroup_PrevArtNum,   0,
            SWA_NGroup_FirstArtNum,  0,
            SWA_NGroup_LastArtNum,   0,
            SWA_NGroup_AvailArtNum,  0,
            SWA_NGroup_NewArtNum,    0,
            SWA_NGroup_TotArtCnt,    0,
            SWA_NGroup_ReadArtCnt,   0,
            SWA_NGroup_BadGroup,     FALSE,
            TAG_DONE);

   DoMethod(o, SWM_ClearObjError);
   
   if (FIsSet(idata->SWA_NGroup_Flags, SWF_NGroup_OVDir))
       DoMethod(o, SWM_NGroup_DeleteOVDir, TAG_DONE);

   FReset(idata->SWA_NGroup_Flags, SWF_NGroup_OVDir);
}


/*****************************************************************************
 * Function: make title for article selector window                          *
 *****************************************************************************/

void SetArtWinTitle(SW_IDATA *idata)
{
   UBYTE Title[120], Entries[64];

   strcpy(Title, MUIX_PH MUIX_B MUIX_C);

   sprintf(Entries, GetS(SWS_L_FT_Count), idata->SWA_NGroup_TotArtCnt);
   strcat(Title, Entries);

   set(idata->ArtList, SWA_NArticleList_Title, Title);
}


/*****************************************************************************
 * Function: Make entry active by object ptr                                 *
 *****************************************************************************/

LONG SetActiveObj(Object *l, Object *Obj)
{
   Object *lo;
   ULONG x;

   if (!Obj) return FALSE;

   SW_MUIListLoop(l, x, lo, TRUE) {
      if (lo == Obj) {
         set(l, MUIA_List_Active, x);
         break;
      }
   }

   return lo == Obj;
}


/*****************************************************************************
 * Function: Create open marker file name                                    *
 *****************************************************************************/

STRPTR MakeCacheMkName(Object *o, SW_IDATA *idata, STRPTR Buff)
{
   TEXT   fb[36];
   STRPTR ovn;

   if (!(ovn = (STRPTR)DoMethod(o, SWM_NGroup_OVName,
                                SWT_NGroup_OVBuff,    fb,
                                SWT_NGroup_OVBuffLen, 36,
                                TAG_DONE)))
      return NULL;

   AddPart(strcpy(Buff, NN_CacheDir(idata)), SWV_NGroup_ChMarkDir, FMSIZE);

   if (SWMakePath(Buff) == FAILURE) return NULL;

   AddPart(Buff, ovn, FMSIZE);

   return Buff;
}


/*****************************************************************************
 * Function: Artlist quiet                                                   *
 *****************************************************************************/

void ArtListQuiet(SW_IDATA *idata, ULONG Quiet)
{
   if (Quiet) {
      if (idata->ArtListQuiet++ == 0)
         set(idata->ArtList, MUIA_List_Quiet, TRUE);
   } else   
      if (--idata->ArtListQuiet == 0)
         set(idata->ArtList, MUIA_List_Quiet, FALSE);
}


/*****************************************************************************
 * Function: abort all pending xfers                                         *
 *****************************************************************************/

void AbortAll(Object *o, SW_IDATA *idata)
{
   ULONG x;

   for (x=0; x<SWV_GC_MAX; x++)
      DoMethod(o, SWM_NGroup_Abort, x);
}


/*****************************************************************************
 * Function: Sleep/Wake app                                                  *
 *****************************************************************************/

void AppSleep(SW_IDATA *idata, ULONG sleep)
{
   set(NN_App(idata), MUIA_Application_Sleep, sleep);
}


/*****************************************************************************
 * Function: add articles to article list                                    *
 *****************************************************************************/

void ArtListAdd(SW_IDATA *idata, Object **ArtArray, ULONG ArtCount, ULONG Clear)
{
   LONG  Top;
   Object *l, *to = NULL, *Base = NULL;
   
   if (!idata->ArtList || (!Clear && !ArtCount)) return;

   AppSleep(idata, TRUE);

   ArtListQuiet(idata, TRUE);
   
   if (Clear) {
      l    = SWGetObj(idata->ArtList, SWA_NList_L);

      DoMethod(l, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &to);
      if (!to) {
         Top  = SWGetLong(l, MUIA_List_First);
         DoMethod(l, MUIM_List_GetEntry, Top, &to);
      }

      Base = (Object *)DoMethod(idata->SWA_NGroup_ThreadMgr,
                                SWM_NArticleMgr_ThreadBase, to);
      
      DoMethod(idata->ArtList, MUIM_List_Clear);
   }

   if (ArtCount && ArtArray)
      DoMethod(idata->ArtList, MUIM_List_Insert, ArtArray, ArtCount,
               MUIV_List_Insert_Sorted);

   ArtListQuiet(idata, FALSE);

   idata->BrowseNoBase = FALSE;

   !Clear || SetActiveObj(l, to) || SetActiveObj(l, Base) ||
      (set(l, MUIA_List_Active, idata->PrevThPos[idata->SWA_NGroup_ThreadFT]),
       idata->BrowseNoBase = TRUE);

   SetArtWinTitle(idata);

   AppSleep(idata, FALSE);
}


/*****************************************************************************
 * Function: Create open marker file                                         *
 *****************************************************************************/

LONG CacheMk(Object *o, SW_IDATA *idata, ULONG Mode)
{
   LONG  rc;
   BPTR   fp;
   UBYTE  Path[FMSIZE];

   if (!MakeCacheMkName(o, idata, Path)) return FAILURE;

   switch (Mode) {
     case SWV_NGroup_MakeChMk:
        if (FIsSet(idata->SWA_NGroup_Flags, SWF_NGroup_CacheChg))
           return SUCCESS;

        FSet(idata->SWA_NGroup_Flags, SWF_NGroup_CacheChg);

        if (!(fp = Open(Path, MODE_NEWFILE)))
           return (LONG)SWERR(o, FAILURE, SWS_Err_MakeChMk, 0, 0);
        Close(fp);
        return SUCCESS;
      
     case SWV_NGroup_RemChMk:
        DeleteFile(Path);

        rc = SWFileExists(Path) ? FAILURE : SUCCESS;
        if (rc == SUCCESS)
           FReset(idata->SWA_NGroup_Flags, SWF_NGroup_CacheChg);
        else
           SWERR(o, NULL, SWS_Err_RemChMk, 0, 0);
        return rc;

     case SWV_NGroup_TestChMk:
        if ((rc = SWFileExists(Path)))
           FSet(idata->SWA_NGroup_Flags, SWF_NGroup_CacheChg);
        else
           FReset(idata->SWA_NGroup_Flags, SWF_NGroup_CacheChg);
        return rc;
      
     default: return FAILURE;
   }
}


/*****************************************************************************
 * Function: reset new article flag                                          *
 *****************************************************************************/

void ResetNew(Object *o, SW_IDATA *idata)
{
   LONG x;

   if (idata->SWA_NGroup_NewArtMd == SWV_NGroup_NA_Exit ||
       idata->SWA_NGroup_NewArtMd == SWV_NGroup_NA_Both)
      for (x=0; x<idata->GrpArticleCnt; x++)
         DoMethod(idata->GrpArticles[x], SWM_NArticle_FlagReset,
                  SWF_NArticle_NewArt);
}


/*****************************************************************************
 * Function: mark articles as expired, keep at most MaxArticles              *
 *****************************************************************************/

void ExpireOld(Object *o, SW_IDATA *idata, ULONG Final, ULONG Oldest)
{
   Object *lo;
   LONG x, PermCount = 0, Excess, Expired = 0;
   ULONG PrevSort, PrevDir, OldestAge, DelCnt = 0, DelSize = 0;
   Object **Del = NULL;

   if (!idata->ArtList || !idata->GrpArticles || Oldest == 0)
      return;

   AppSleep(idata, TRUE);
   
   StartGauge(o, idata, o, SWA_NGroup_HdrPctDone,
              GetS(SWS_L_Status_Expiring), SWV_GC_Private1, FALSE);

   // -- put back any killed articles so we can expire them ------------------
   
   set(idata->SWA_NGroup_GrpDefFlt, SWA_NSF_InGroup_Killed,    TRUE);

   idata->GrpArticleCnt = DoMethod(idata->SWA_NGroup_ArtMgr,
                                   SWM_NArticleMgr_FilterList,
                                   SWV_NArticleMgr_Filter_UseTbl, 0,
                                   &idata->GrpArticles, &idata->GrpArticleMax,
                                   0, FALSE, 1, FALSE, o, idata->SWA_NGroup_GrpDefFlt,
                                   NULL);

   set(idata->SWA_NGroup_GrpDefFlt, SWA_NSF_InGroup_Killed, FALSE);

   // -- first, find # of permanent ones (since we don`t delete those) -------

   for (x=0; x<idata->GrpArticleCnt; x++) {
      if (DoMethod(idata->GrpArticles[x], SWM_NArticle_FlagTest,
                   SWF_NArticle_Permanent))
         PermCount++;

      if (!(x%25))
         set(o, SWA_NGroup_HdrPctDone, x * 10 / max(idata->GrpArticleCnt, 1));
   }

   // -- provide default val for MaxKeep -------------------------------------
   
   if (idata->SWA_NGroup_MaxKeep == 10001)
      idata->SWA_NGroup_MaxKeep = idata->SWA_NGroup_MaxArticles;

   // -- compute number of excess articles -----------------------------------

   Excess = (LONG)idata->GrpArticleCnt - PermCount -
      (LONG)idata->SWA_NGroup_MaxKeep;

   OldestAge = DoMethod(idata->OldestArt, SWM_NArticle_Age, NULL);

   if (Excess <= 0 && OldestAge < Oldest) goto Done;

   ArtListQuiet(idata, TRUE);
   
   DoMethod(idata->ArtList, MUIM_List_Clear);

   PrevSort = SWGetUL(idata->ArtList, SWA_NArticleList_Sort1);
   PrevDir  = SWGetUL(idata->ArtList, SWA_NArticleList_Dir1);

   SetAttrs(idata->ArtList,
            MUIA_NoNotify,          TRUE,  // prevent notify to group
            SWA_NArticleList_Sort1, SWV_NArticle_SortAge,
            SWA_NArticleList_Dir1,  SWV_NArticle_ForwardSort,
            TAG_DONE);

   DoMethod(idata->ArtList, MUIM_List_Insert, idata->GrpArticles,
            idata->GrpArticleCnt, MUIV_List_Insert_Sorted);

   // -- Now nuke the older ones until we have few enough --------------------

   SW_MUIListLoop(idata->ArtList, x, lo, TRUE) {
      if (!DoMethod(lo, SWM_NArticle_FlagTest, SWF_NArticle_Permanent) &&
          (Expired < Excess ||
           DoMethod(lo, SWM_NArticle_Age, NULL) > Oldest)) {

         DoMethod(lo, SWM_NArticle_FastUnCache,
                  SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
                  TAG_DONE);
         DoMethod(lo, SWM_NArticle_FlagSet, SWF_NArticle_Expired);
         Expired++;

         // -- remember articles so we can delete them -----------------------

         if (DelCnt >= DelSize) {
            DelSize += 50;
            RV(ASize(DelSize), &Del);
         }

         Del[DelCnt++] = lo;
      }

      if (!(x%25))
         set(o, SWA_NGroup_HdrPctDone, 10+x*90 / max(idata->GrpArticleCnt,1));
   }

   // -- put back old sort type/direction ------------------------------------

   if (!Final) {
      SetAttrs(idata->ArtList,
               SWA_NArticleList_Sort1, PrevSort,
               SWA_NArticleList_Dir1,  PrevDir,
               TAG_DONE);
      DoMethod(o, SWM_NGroup_ChgSelFlt, 0);
   }

   // -- potentially remove articles from other groups -----------------------

   DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_DelArts,
            SWT_NArticleMgr_ArticleArray, Del,
            SWT_NArticleMgr_ArticleCount, DelCnt,
            TAG_DONE);

   FV(Del);

   set(o, SWA_NGroup_TotArtCnt, idata->GrpArticleCnt);

   ArtListQuiet(idata, FALSE);

 Done:
   StopGauge(o, idata, SWV_GC_Private1);
   AppSleep(idata, FALSE);
}


/*****************************************************************************
 * Function: Add pop server button                                           *
 *****************************************************************************/

void AddPopSrv(Object *o, SW_IDATA *idata)
{
   if (!idata->AttrUI) return;

   // OM_NEW in the PopSrvList adds itself to the AttrUI we give it.
   
   PopSrvListObject,
      AppBaseDefTop(idata),
      SWA_PopSrvList_AttrUI,  idata->AttrUI,
      SWA_PopSrvList_SrvAttr, SWA_NGroup_Server1,
      SWA_PopSrvList_SrvObj,  o,
   End;
}


/*****************************************************************************
 * Function: Clear batch D/L arts                                            *
 *****************************************************************************/

void ClearBatchDL(SW_IDATA *idata)
{
   FV(idata->BatchArticles);
   idata->BatchArticles   = NULL;
   idata->BatchArticleCnt =
   idata->BatchArticlePos = 0;
}


/*****************************************************************************
 * Function: free fetch data lines                                           *
 *****************************************************************************/

void ClearFetchLines(SW_IDATA *idata)
{
   ULONG x;

   if (idata->FetchLines)
      for (x=0; x<idata->FetchLineCnt; x++) {
         FV(idata->FetchLines[x]);
         idata->FetchLines[x] = NULL;
      }

   idata->FetchLineCnt = 0;
}


/*****************************************************************************
 * Function: free fetch data lines                                           *
 *****************************************************************************/

void FreeFetchData(SW_IDATA *idata)
{
   ULONG x;
   
   ClearFetchLines(idata);
   
   if (idata->FetchOverview)
      for (x=0; idata->FetchOverview[x]; x++)
         FV(idata->FetchOverview[x]);

   FV(idata->FetchNums);
   FV(idata->FetchLines);
   FV(idata->FetchOverview);

   idata->FetchLines    = NULL;
   idata->FetchNums     = NULL;
   idata->FetchObj      = NULL;
   idata->FetchQObj     = NULL;
   idata->FetchOverview = NULL;
   idata->FetchFirst    =
   idata->FetchLast     =
   idata->FetchLineMax  =
   idata->FetchArtCnt   =
   idata->FetchNumPos   =
   idata->FetchMthd     =
   idata->FetchQMthd    =
   idata->FetchNumPos   = 0;
}


/*****************************************************************************
 * Function: set up title buffer and return ptr to it                        *
 *****************************************************************************/

STRPTR MakeTitle(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG Str)
{
   STRPTR Buff;
   
   if (!idata->SrvInfo[Cmd].CmdTitle)
      idata->SrvInfo[Cmd].CmdTitle = AV(FMSIZE);

   if (!(Buff = idata->SrvInfo[Cmd].CmdTitle)) return NULL;

   strcpy(Buff, GetS(Str));

   DoMethod(o, SWM_NGroup_Abbrev, Buff + strlen(Buff),
            FMSIZE - strlen(Buff) - 1);

   return Buff;
}


/*****************************************************************************
 * Function: Clear articles from batch D/L queue                             *
 *****************************************************************************/

void DLQueueClear(Object *o, SW_IDATA *idata)
{
   ULONG x;
   
   StopGauge(o, idata, SWV_GC_BatchDL);

   if (idata->ReaderWin)
      DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);

   if (idata->DLPending)
      idata->DLQueue[idata->DLCnt++] = idata->DLPending; // so we can cancel it;

   for (x=0; x<idata->DLCnt; x++)
      DoMethod(idata->DLQueue[x],SWM_NArticle_FlagReset,SWF_NArticle_QueuedDL);
   
   if (FIsSet(idata->SWA_NGroup_Flags, SWF_NGroup_Action)) {
      DoMethod(idata->SWA_NGroup_ActFlt, SWM_NSFList_DoSelected, SWM_NAFilter_End, o);
      DoMethod(idata->Filter_GlobAct,    SWM_NSFList_DoSelected, SWM_NAFilter_End, o);
      
      FReset(idata->SWA_NGroup_Flags, SWF_NGroup_Action);
   }

   if (idata->DLCnt || idata->DLPending)
      DoMethod(o, SWM_NGroup_UpdArtList, NULL);

   FV(idata->DLQueue);
   FV(idata->DLQueueCmd);
   idata->DLCnt      = 0;
   idata->DLMax      = 0;
   idata->DLNum      = 0;
   idata->DLPending  = NULL;
   idata->DLQueue    = NULL;
   idata->DLQueueCmd = NULL;
}


/*****************************************************************************
 * Function: Fix DL Command                                                  *
 *****************************************************************************/

static LONG QueueRd(ULONG DLCmd, ULONG Cmd)
{
   return DLCmd == SWV_NGroup_DLQ_PreFetch && Cmd == SWV_NGroup_DLQ_Read;
}


/*****************************************************************************
 * Function: Compare download command                                        *
 *****************************************************************************/

static LONG CmpDLCmd(Object *DLArt, ULONG DLCmd, Object *Art, ULONG Cmd)
{
   return DLArt == Art && (DLCmd == Cmd || QueueRd(DLCmd, Cmd));
}


/*****************************************************************************
 * Function: Add article(s) to batch D/L queue                               *
 *****************************************************************************/

static ULONG DLQueueAddInt(Object *o, SW_IDATA *idata, Object *Art,
                           UBYTE Cmd, LONG Last)
{
   ULONG x, rc, Found = FALSE;

   // -- already have it? ----------------------------------------------------

   if (Cmd == SWV_NGroup_DLQ_PreFetch &&
       DoMethod(Art, SWM_NArticle_HasParts,
                SWT_NArticle_Parts, SWF_NArticle_HdBd,
                TAG_DONE))
      return SUCCESS;

   // -- search for entry in DLQueue -----------------------------------------

   for (x=0; !Found && x<idata->DLCnt; x++)
      if (CmpDLCmd(idata->DLQueue[x], idata->DLQueueCmd[x], Art, Cmd)) {
         if (QueueRd(idata->DLQueueCmd[x], Cmd))
            idata->DLQueueCmd[x] = Cmd;
         Found = TRUE;
      }

   if (!Found) {
      // -- see if we can get this article from cache ------------------------
   
      if (Cmd == SWV_NGroup_DLQ_Read) {
         idata->DLInstRead = TRUE;
         rc = DoMethod(Art, SWM_NArticle_Fetch,
                       SWT_NArticle_Parts,         idata->SWA_NGroup_FullHeaders ?
                                                      SWF_NArticle_HdBd :
                                                      SWF_NArticle_Body,
                       SWT_NArticle_FetchObj,      o,
                       SWT_NArticle_CacheDir,      OVDir(o, idata, NULL),
                       SWT_NArticle_NoNetwork,     TRUE,
                       TAG_DONE);
   
         idata->DLInstRead = FALSE;
         if (rc == SUCCESS)
            return rc;
      }

      // -- resize download queue array if necessary -------------------------

      if (idata->DLCnt >= idata->DLMax) {
         idata->DLMax += 50;
         if (RV(ASize(idata->DLMax), &idata->DLQueue)    < 0 ||
             RV(idata->DLMax,        &idata->DLQueueCmd) < 0)
         return SWERR(o, FAILURE, SWS_Err_NoSuchGroup, idata->SWA_NGroup_Name, 0);
      }

      DoMethod(Art, SWM_NArticle_FlagSet, SWF_NArticle_QueuedDL);

      idata->DLQueue[idata->DLCnt]    = Art;
      idata->DLQueueCmd[idata->DLCnt] = Cmd;
      idata->DLCnt++;
      idata->DLNum++;
   }

   if ((rc = DoMethod(o, SWM_NGroup_DLFetchNext)) == SUCCESS && Last) {
      if (idata->DLNum != idata->DLCnt) {
         sprintf(idata->DLStatMsg, GetS(SWS_L_Status_DLArt),
                 idata->DLNum - idata->DLCnt, idata->DLNum,
                 (idata->DLNum - idata->DLCnt) * 100 / max(idata->DLNum, 1));

         UpdGaugeStat(o, idata, idata->DLStatMsg, SWV_GC_BatchDL);
      }
   }

   return rc;
}


ULONG DLQueueAdd(Object *o, SW_IDATA *idata, Object *Art, UBYTE Cmd, LONG Last)
{
   // -- see if this is a multipart article ----------------------------------

#  ifdef MPINFO

   if (0 && DoMethod(Art, SWM_NArticle_ParseMPInfo, NULL, NULL, NULL, NULL)) {
      MPInfo *MP = (MPInfo *)DoMethod(idata->SWA_NGroup_ThreadMgr,
                                      SWM_NArticleMgr_MultiPart_Find,
                                      SWT_NArticleMgr_Article, Art,
                                      TAG_DONE);

      if (MP) {
         LONG x;
         for (x=0; x<=MP[0].MaxPart; x++) {
            STRPTR Subj = MP[x].Art ?
               (STRPTR)DoMethod(MP[x].Art, SWM_NArticle_GetSubj) : "";

            Printf("MsgID %ld: %s\n   %s\n",
                   x, SW_STRDEF(MP[x].MsgID, "Unknown"), Subj);
         }
         
         DoMethod(idata->SWA_NGroup_ThreadMgr,
                  SWM_NArticleMgr_MultiPart_Free, MP);

         return SUCCESS;
      }
   }

#  endif // MPINFO

   return DLQueueAddInt(o, idata, Art, Cmd, Last);
}


/*****************************************************************************
 * Function: If reader closed, load Overview                                 *
 *****************************************************************************/

ULONG ClosedGetOVStart(Object *o, SW_IDATA *idata)
{
   if (DoMethod(idata->SWA_NGroup_ArtMgr,SWM_NArticleMgr_AddGroup,o)!=SUCCESS)
      return FAILURE;

   if (!idata->SWA_NGroup_ReaderOpen) {
      if (DoMethod(o, SWM_NGroup_LoadOV, TAG_DONE) != SUCCESS) {
         DoMethod(idata->SWA_NGroup_ArtMgr,  SWM_NArticleMgr_RemGroup, o);
         return FAILURE;
      }

      if (CacheMk(o, idata, SWV_NGroup_TestChMk))
         DoMethod(o, SWM_NGroup_CleanCache);
   }

   return SUCCESS;
}


/*****************************************************************************
 * Function: If reader closed, load Overview                                 *
 *****************************************************************************/

void ClosedGetOVEnd(Object *o, SW_IDATA *idata)
{
   if (!idata->SWA_NGroup_ReaderOpen)
      DoMethod(o, SWM_NGroup_SaveOV, TAG_DONE);

   DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_RemGroup, o);

   if (!idata->SWA_NGroup_ReaderOpen)
      NukeArtArrays(idata);
}


/*****************************************************************************
 * Function: begin Load/Fetch cmd                                            *
 *****************************************************************************/

ULONG BeginLFCmd(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG Method,
                 ULONG Action, ULONG *msg)
{
   Object *Srv;

   AB_SetMod(idata);

   if (!(Srv = CheckSendCmd(o, idata, msg, Cmd)))
      return FAILURE;

   if (ClosedGetOVStart(o, idata))
      return FAILURE;

   if (DoMethod(o, Action,
                SWT_NGroup_CmdObj,     o,
                SWT_NGroup_CmdMethod,  Method,
                TAG_DONE) == SUCCESS)
      return SUCCESS;

   ClosedGetOVEnd(o, idata);
   return FAILURE;
}


/*****************************************************************************
 * Function: End Load/Fetch cmd                                              *
 *****************************************************************************/

ULONG EndLFCmd(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG State)
{
   if (State == SWV_NGroup_End || State == SWV_NGroup_Abort)
      ClosedGetOVEnd(o, idata);

   SendCmdState(o, idata, Cmd, State);

   return 0;
} SDFT


/*****************************************************************************
 * Function: remove matching items from list                                 *
 *****************************************************************************/

LONG RemFromList(Object **InList, ULONG *Size, Object **KillList, ULONG KLSize)
{
   ULONG x, y, pos, found;

   if (!InList || !Size || !KillList) return FALSE;

   for (x=pos=0; x<*Size; x++) {
      for (y=0, found=FALSE; y<KLSize; y++)
         if (InList[x] == KillList[y]) {
            found = TRUE; break;
         }

      if (!found)
         InList[pos++] = InList[x];
   }

   *Size = pos;

   return pos != x;
}


/*****************************************************************************
 * Method: Make filter entry                                                 *
 *****************************************************************************/

void MkFlt(SW_IDATA *idata, ULONG Type, ULONG Exp, ULONG ExpOK, ULONG Where)
{
   ULONG x;
   STRPTR Patt;
   Object *lo, *StrFlt,
          *KillGrp = (Where == 0 ? idata->SWA_NGroup_KillFlt :
                                   NN_GlobKill(idata)),
                      
          *ArtKill = (Object *)DoMethod(KillGrp,
                                        SWM_NList_FindObj,
                                        SWA_NSFilter_Desc, SWV_ArtKillFlt);

   if (!ArtKill) {
      ArtKill = MUI_NewObject(SWC_NSF_HeaderMatch,
                              SWA_NSFilter_Desc,    SWV_ArtKillFlt,
                              TAG_DONE);

      if (!ArtKill) return;
      
      DoMethod(KillGrp, MUIM_List_InsertSingle, ArtKill,
               MUIV_List_Insert_Bottom);
   }

   set(ArtKill, MUIA_List_Quiet, TRUE);

   SW_MUIListLoopSel(idata->ArtList, x, lo, TRUE) {
      if (Type == 2)
         Patt = (STRPTR)DoMethod(lo, SWM_NArticle_FindHeader,
                                  SWF_NAHeader_From);
      else
         Patt = (STRPTR)DoMethod(lo, SWM_NArticle_GetSubj);

      if (DoMethod(ArtKill, SWM_NList_FindObj, SWA_NStrFilter_Pattern, Patt))
         continue;

      StrFlt = MUI_NewObject(SWC_NStrFilter,
                             SWA_NStrFilter_Pattern,   Patt,
                             SWA_NStrFilter_Header,
                                Type == 2 ? SWV_HM_From : SWV_HM_Subject,
                             SWA_NStrFilter_Case,      TRUE,
                             SWA_NStrFilter_Wild,      FALSE,
                             SWA_NStrFilter_Anchor,    TRUE,
                             SWA_NSFilter_ExpDisuse,   Exp,
                             SWA_NSFilter_ExpDisuseOK, ExpOK,
                             TAG_DONE);

      if (!StrFlt) goto Done;

      DoMethod(ArtKill,MUIM_List_InsertSingle,StrFlt,MUIV_List_Insert_Bottom);
   }

  Done:
   set(ArtKill, MUIA_List_Quiet, FALSE);
}


/*****************************************************************************
 * Method: Select all articles by same author(s)                             *
 *****************************************************************************/

void SelectAuth(SW_IDATA *idata)
{
   STRPTR *Auths;
   Object *lo;
   ULONG x, y, Count = 0;
   
   SW_MUIListLoopSel(idata->ArtList, x, lo, TRUE)
      Count++;

   if (!(Auths = (STRPTR *)AV(Count * sizeof(STRPTR))))
      return;

   Count = 0;
   SW_MUIListLoopSel(idata->ArtList, x, lo, TRUE)
      Auths[Count++] = (STRPTR)DoMethod(lo, SWM_NArticle_FindHeader,
                                        SWF_NAHeader_From);

   SW_MUIListLoop(idata->ArtList, x, lo, TRUE) {
      STRPTR f = (STRPTR)DoMethod(lo, SWM_NArticle_FindHeader,
                                  SWF_NAHeader_From);

      for (y=0; y<Count; y++)
         if (!strcmp(f, Auths[y])) {
            DoMethod(idata->ArtList, MUIM_List_Select, x,
                     MUIV_List_Select_On, NULL);
            break;
         }
   }

   FV(Auths);
}


/*****************************************************************************
 * Fetch an article if possible - from NW in online mode, or cache           *
 *****************************************************************************/

void FetchIfPossible(Object *o, SW_IDATA *idata, Object *Art)
{
   if (IsOnline(idata) ||
       DoMethod(Art, SWM_NArticle_FlagTest, SWF_NArticle_CachedBody))
      DLQueueAdd(o, idata, Art, SWV_NGroup_DLQ_Read, TRUE);
}


/*****************************************************************************
 * Function: Find next article matching criteria                             *
 *****************************************************************************/

Object *NextArt(SW_IDATA *idata, LONG *ArtNum, LONG Dir,
                ULONG Flags, ULONG TestRes)
{
   Object *a;
   
   // -- find first unread article in this thread ----------------------------
   
   for (; ; (*ArtNum) += Dir) {
      DoMethod(idata->ArtList, MUIM_List_GetEntry, *ArtNum, &a);

      if (!a || !Flags ||
          (DoMethod(a, SWM_NArticle_FlagTest, Flags) == TestRes))
         break;
   }

   return a;
}


/*****************************************************************************
 * Function: move to base article in thread                                  *
 *****************************************************************************/

ULONG BrowseBase(Object *o, SW_IDATA *idata, ULONG NewMode, ULONG Action)
{
   Object *a;
   LONG ArtNum = 0;

   // -- go to 1Thread mode if asked to --------------------------------------
   
   if (NewMode)
      DoMethod(o, SWM_NGroup_ChgThreadFlt, SWF_NGroup_ThreadFlt_1Thread,
               TRUE, FALSE);

   // -- find first unread article in this thread ----------------------------

   a = NextArt(idata, &ArtNum, 1,
               (Action != SWV_NGroup_Browse_1stUnread) ? 0 : SWF_NArticle_Read,
               0);

   if (!a) return 0;

   set(idata->ArtList, MUIA_List_Active, ArtNum);

   // -- only fetch it if (A) in online mode, or (B) it`s cached -------------

   FetchIfPossible(o, idata, a);
      
   return 0;
}


/*****************************************************************************
 * Function: Send command state message                                      *
 *****************************************************************************/

void SendCmdState(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG State)
{
   if (idata->SrvInfo[Cmd].CmdObj)
      DoMethod(idata->SrvInfo[Cmd].CmdObj, idata->SrvInfo[Cmd].CmdMethod,
               SWT_NGroup_CmdState, State,
               SWT_NGroup_CmdData,  idata->SrvInfo[Cmd].CmdData,
               SWT_NGroup_Server,   idata->SrvInfo[Cmd].Srv,
               SWT_NGroup_ID,       idata->SrvInfo[Cmd].ID,
               SWT_NGroup_CmdGroup, o,
               SWT_NGroup_ProgMsg,  idata->SrvInfo[Cmd].ProgMsg,
               TAG_DONE);

   if (State == SWV_NGroup_End || State == SWV_NGroup_Abort) {
      FV(idata->SrvInfo[Cmd].CmdTitle);
      idata->SrvInfo[Cmd].CmdTitle = NULL;

      FV(idata->SrvInfo[Cmd].ProgMsg);
      idata->SrvInfo[Cmd].ProgMsg = NULL;
   }
}


/*****************************************************************************
 * Function: Register beginning of command                                   *
 *****************************************************************************/

void MsgBegin(Object *o, SW_IDATA *idata, SWP_NServer_StatusMsg *msg,
              ULONG st, ULONG Cmd)
{
   if (!NSR_SBegin(msg)) return;

   idata->SrvInfo[Cmd].Srv = NSR_Srv(msg); 
   idata->SrvInfo[Cmd].ID  = NSR_ID(msg);

   set(o, SWA_NGroup_HdrPctDone, 0);

   StartGauge(o, idata, o, SWA_NGroup_HdrPctDone, GetS(st), Cmd, TRUE);

   SendCmdState(o, idata, Cmd, SWV_NGroup_Begin);
}


/*****************************************************************************
 * Function: Register end of command                                         *
 *****************************************************************************/

void MsgEnd(Object *o, SW_IDATA *idata, SWP_NServer_StatusMsg *msg, ULONG Cmd)
{
   if (!NSR_SAnyEnd(msg)) return;

   set(o, SWA_NGroup_HdrPctDone, 100);
   StopGauge(o, idata, Cmd);

   if (NSR_SAbort(msg)) {
      DoMethod(o, SWM_Root_AdoptObjError, idata->SrvInfo[Cmd].Srv, 0);
      SendCmdState(o, idata, Cmd, SWV_NGroup_Abort);
      set(o, SWA_NGroup_Status, GetS(SWS_L_Status_Aborted));

      if (idata->ReaderWin)
         DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);
   } else {
      SendCmdState(o, idata, Cmd, SWV_NGroup_End);
   }

   idata->SrvInfo[Cmd].Srv = NULL;
   idata->SrvInfo[Cmd].ID  = -1;
}


/*****************************************************************************
 * Function: send command                                                    *
 *****************************************************************************/

Object *CheckSendCmd(Object *o, SW_IDATA *idata, ULONG *msg, ULONG Cmd)
{
   Object *DSrv,
           *Srv = GetTagOB(SWT_NGroup_Server, idata->SWA_NGroup_SrvList, msg+1);

   if (GP_NoName(idata))
      return (Object *)SWERR(o, NULL, SWS_Err_NoName, 0, 0);

   if (!Srv) return NULL;

   DSrv = (Object *)DoMethod(Srv, SWM_NServerList_GetDefault,
                             SWT_NServerList_ServerName,
                                   idata->SWA_NGroup_Server1,
                             TAG_DONE);

   if (!DSrv)
      return (Object *)DoMethod(o, SWM_Root_AdoptObjError, Srv, 0);
   
   DoMethod(o, SWM_NGroup_Abort, Cmd);  // stop any pending command;

   idata->SrvInfo[Cmd].CmdObj    = GetTagOB(SWT_NGroup_CmdObj,    NULL, msg+1);
   idata->SrvInfo[Cmd].CmdMethod = GetTagUL(SWT_NGroup_CmdMethod, NULL, msg+1);
   idata->SrvInfo[Cmd].CmdData   = GetTagUL(SWT_NGroup_CmdData,   NULL, msg+1);
   
   return DSrv;
}


/*****************************************************************************
 * Function: reset article # stats                                           *
 *****************************************************************************/

static ULONG ResetArtNums(Object *o, SW_IDATA *idata, ULONG rc)
{
   SetAttrs(o, 
            SWA_NGroup_FirstArtNum, 0,
            SWA_NGroup_LastArtNum,  0,
            SWA_NGroup_AvailArtNum, 0,
            SWA_NGroup_NewArtNum,   0,
            TAG_DONE);

   return rc;
}


/*****************************************************************************
 * Function: check rescode                                                   *
 *****************************************************************************/

ULONG CheckResCode(Object *o, SW_IDATA *idata,
                   SWP_NServer_StatusMsg *msg, ULONG OKCode)
{
   ULONG rc = NSR_ResCode(msg);

   if (rc != OKCode) {
      set(o, SWA_NGroup_BadGroup, TRUE);

      switch (rc) {
        case SWV_NSR_NoSuchGroup:
         SWERR(o, NULL, SWS_Err_NoSuchGroup, idata->SWA_NGroup_Name, 0);
         break;

        default:
         SWERR(o, NULL, SWS_Err_Unknown, rc, 0);
      }

      if (idata->ReaderWin)
         DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);

      return ResetArtNums(o, idata, SWV_NServer_RescodeBad);
   }

   if (idata->SWA_NGroup_BadGroup)
      set(o, SWA_NGroup_BadGroup, FALSE);

   return SWV_NServer_RescodeOK;
}


/*****************************************************************************
 * Function: Send fetch line to server for indicated columns/data            *
 *****************************************************************************/

static LONG SendFetchLine(Object *o, SW_IDATA *idata, STRPTR CmdBuff,
                          ULONG CmdM, ULONG Cmd, STRPTR Title, ULONG Intv)
{
   Object *Srv;

   if (DoMethod(Srv = idata->SrvInfo[Cmd].Srv,
                SWM_NServer_SendCmd,
                SWT_NServer_ID,        idata->SrvInfo[Cmd].ID,
                SWT_NServer_Cmd,       CmdBuff,
                SWT_NServer_CmdObj,    o,
                SWT_NServer_CmdMethod, CmdM,
                SWT_NServer_Flags,     SWF_NServer_ExplicitDone,
                SWT_NServer_Title,     Title,
                SWT_NServer_Interval,  Intv,
                TAG_DONE) != SUCCESS) {

      DoMethod(o, SWM_Root_AdoptObjError, Srv, 0);

      if (idata->ReaderWin)
         DoMethod(o, SWM_Root_CheckErr, idata->ReaderWin);
      
      return 0;
   }

   return 1;
}
                          

/*****************************************************************************
 * Function: Fetch next batch of headers.  return 1=more, 0=done, -1=err     *
 *****************************************************************************/

void FetchHeaders(Object *o, SW_IDATA *idata, LONG Pos)
{
   UBYTE CmdBuff[SWV_NServer_MaxCmdLen];

   if (Pos >= idata->FetchNumCnt) return;

   // -- overview style fetch? -----------------------------------------------
   
   if (FIsSet(idata->FetchFlags, SWF_NGroup_FetchXOver)) {
      if (Pos != idata->FetchStart)
         return;
         
      sprintf(CmdBuff, "XOVER %ld-%ld\r\n",
              idata->FetchNums[idata->FetchStart],
              idata->FetchNums[idata->FetchNumCnt-1]);

      SendFetchLine(o, idata, CmdBuff, SWM_NGroup_Fetch_XOV_P2,
                    SWV_GC_FetchHdr, NULL, idata->FetchCnt);
   } else {
      // -- otherwise, normal fetch, with or without XHDR Bytes --------------
      sprintf(CmdBuff,
              FIsSet(idata->FetchFlags, SWF_NGroup_FetchBytes) ?
                "XHDR Bytes %ld\r\nHEAD %ld\r\n" :
                "HEAD %ld\r\n",
              idata->FetchNums[Pos], idata->FetchNums[Pos]);

      SendFetchLine(o, idata, CmdBuff, SWM_NGroup_Fetch_P2,
                    SWV_GC_FetchHdr, NULL, SWF_NServer_ItvNone);
   }
}


/*****************************************************************************
 * Function: Fetch next batch of headers.  return 1=more, 0=done, -1=err     *
 *****************************************************************************/

LONG FetchBatchArt(Object *o, SW_IDATA *idata, LONG Pos)
{
   UBYTE CmdBuff[SWV_NServer_MaxCmdLen];
   STRPTR MsgID;

   if (Pos >= idata->BatchArticleCnt) return 0;

   sprintf(CmdBuff, "BODY %s\r\n",
           MsgID = (STRPTR)
               DoMethod(idata->BatchArticles[Pos], SWM_NArticle_GetMsgID));

   if (idata->DBOut && FIsSet(idata->DBType, SWV_DB_BatchDL))
      FPrintf(idata->DBOut, "%s: Fetching: %s\n", __FUNC__, MsgID);

   if (!SendFetchLine(o, idata, CmdBuff, SWM_NGroup_GL_Arts_P2,
                      SWV_GC_GL_Arts,
                      MakeTitle(o, idata, SWV_GC_GL_Arts, SWS_L_TBatchArts),
                      SWF_NServer_ItvNone))
      return -1;
   
   return 1;
}


/*****************************************************************************
 * Function: add articles to thread manager                                  *
 *****************************************************************************/

void ThrdMgrAdd(Object *o, SW_IDATA *idata, Object **Articles, ULONG ArtCount)
{
   set(o, SWA_NGroup_TotArtCnt, idata->GrpArticleCnt);

   if (!idata->SWA_NGroup_ThreadMgr || !ArtCount) return;

   DoMethod(idata->SWA_NGroup_ThreadMgr, SWM_NArticleMgr_Add,
            SWT_NArticleMgr_ArticleArray, Articles,
            SWT_NArticleMgr_ArticleCount, ArtCount,
            TAG_DONE);
}


/*****************************************************************************
 * Function: Count read articles, track oldest article                       *
 *****************************************************************************/

void CountRead(Object *o, SW_IDATA *idata)
{
   ULONG  x, Count, Age, MaxAge = 0;

   idata->OldestArt = NULL;

   for (x=Count=0; x<idata->GrpArticleCnt; x++) {
      Age = DoMethod(idata->GrpArticles[x], SWM_NArticle_Age, NULL);

      if (Age > MaxAge)
         MaxAge = Age,
         idata->OldestArt = idata->GrpArticles[x];
      
      if (DoMethod(idata->GrpArticles[x], SWM_NArticle_FlagTest,
                   SWF_NArticle_Read))
         Count++;
   }
   
   SetAttrs(o,
            SWA_NGroup_ReadArtCnt, Count,
            SWA_NGroup_TotArtCnt,  idata->GrpArticleCnt,
            TAG_DONE);
}


/*****************************************************************************
 * Function: Nuke article arrays                                             *
 *****************************************************************************/

void NukeArtArrays(SW_IDATA *idata)
{
   MemQueueDel(idata, TRUE, NULL);

   FV(idata->GrpArticles);
   FV(idata->SelArticles);
   FV(idata->UIArticles);
   FV(idata->CacheArticles);

   idata->GrpArticles     = NULL;
   idata->SelArticles     = NULL;
   idata->UIArticles      = NULL;
   idata->CacheArticles   = NULL;

   idata->GrpArticleCnt   = 0;
   idata->SelArticleCnt   = 0;
   idata->UIArticleCnt    = 0;
   idata->CacheArticleCnt = 0;

   idata->GrpArticleMax   = 0;
   idata->SelArticleMax   = 0;
   idata->UIArticleMax    = 0;
   idata->CacheArticleMax = 0;

   FV(idata->DLStatMsg);
   idata->DLStatMsg     = NULL;
   
   FV(idata->OVDir);
   idata->OVDir         = NULL;

   MUI_DisposeObject(idata->SWA_NGroup_ThreadMgr);
   idata->SWA_NGroup_ThreadMgr = NULL;
}


/*****************************************************************************
 * Function: Add to memory queue                                             *
 *****************************************************************************/

ULONG MemQueueAdd(SW_IDATA *idata, Object *Art)
{
   ULONG x;

   // -- see if it`s already in the list -------------------------------------
   
   for (x=0; x<idata->CacheArticleCnt; x++)
      if (idata->CacheArticles[x] == Art)
         return SUCCESS;

   // -- grow list as needed -------------------------------------------------
   
   if ((idata->CacheArticleCnt+2) >= idata->CacheArticleMax) {
      idata->CacheArticleMax += 16;

      if (RV(sizeof(Object *) * idata->CacheArticleMax,
             &idata->CacheArticles) < 0)
         return FAILURE;
   }
   
   // -- add new article to end of list --------------------------------------

   idata->CacheArticles[idata->CacheArticleCnt++] = Art;

   return SUCCESS;
}


/*****************************************************************************
 * Function: Remove from memory queue until small enough                     *
 *****************************************************************************/

void MemQueueDel(SW_IDATA *idata, ULONG All, Object *Art)
{
   LONG x, y, Size = 0, ToNuke = 0, ac = idata->CacheArticleCnt;

   if (!idata->CacheArticles || !ac) return;

   // -- find size of cached articles ----------------------------------------

   for (x=0; x<idata->CacheArticleCnt; x++)
      Size += DoMethod(idata->CacheArticles[x], SWM_NArticle_ByteSize,
                       SWT_NArticle_Parts, SWF_NArticle_Body,
                       TAG_DONE);

   // -- nuke articles until cache is small enough ---------------------------

   if (All)
      ToNuke = ac;
   else
      while ((Size/1024 > idata->SWA_NGroup_MemCacheKb) ||
              ac-- > max(idata->SWA_NGroup_MemCacheNum, 1))
         if (idata->CacheArticles[ToNuke++] != Art) {
            if (ToNuke >= idata->CacheArticleCnt) break;
            Size -= DoMethod(idata->CacheArticles[ToNuke-1], SWM_NArticle_ByteSize,
                             SWT_NArticle_Parts, SWF_NArticle_Body,
                             TAG_DONE);
         }

   ToNuke = min(ToNuke, idata->CacheArticleCnt);

   // -- remove the articles -------------------------------------------------

   for (x=0; x<ToNuke; x++)
      if (idata->CacheArticles[x] != Art) {
         DoMethod(idata->CacheArticles[x],SWM_NArticle_FreeText,SWF_NArticle_Body);
         idata->CacheArticles[x] = (Object *)-1;
      }

   if (ToNuke) {
      for (x=0, y=0; y<idata->CacheArticleCnt; y++)
         if ((idata->CacheArticles[x] = idata->CacheArticles[y]) != (Object *)-1)
            x++;

      idata->CacheArticleCnt  = x;
   }
}


/*****************************************************************************
 * Function: Prefetch articles from thread(s)                                *
 *****************************************************************************/

static void DoPF(Object *o, SW_IDATA *idata, LONG Pos)
{
   Object *a;

   DoMethod(idata->ArtList, MUIM_List_GetEntry, Pos, &a);

   if (a) {
      DLQueueAdd(o, idata, a, SWV_NGroup_DLQ_PreFetch, TRUE);
      DoMethod(idata->ArtList, MUIM_List_Redraw, Pos);
   }
}

void ArtPrefetch(Object *o, SW_IDATA *idata)
{
   LONG PreFetch, MaxFetch, x, Stop = 0;
   
   // -- handle prefetch -----------------------------------------------------

   if (ThrdMode(idata) || !IsOnline(idata))
      return;
      
   PreFetch = SWGetUL(idata->ArtList, MUIA_List_Active);
         
   MaxFetch = min(idata->SWA_NGroup_PrefetchNum,
                  idata->SWA_NGroup_MemCacheNum-1);

   if (!MaxFetch) return;

   if (!idata->DLPending) {
      DoPF(o, idata, PreFetch + 1);
      Stop = 1;
   }
   
   for (x=MaxFetch; x>Stop; x--)
      DoPF(o, idata, PreFetch+x);
}


/*****************************************************************************
 * Function: Make article list QSearch                                       *
 *****************************************************************************/

Object *CreateALQSrch(Object *o, SW_IDATA *idata)
{
   Object *qs = 
      MUI_NewObject(SWC_ListSearch,
                    SWA_ListSearch_Title,        GetS(SWS_L_ALQSTitle),
                    SWA_ListSearch_List,         SWGetObj(idata->ArtList, SWA_NList_L),
                    SWA_ListSearch_SearchObj,    idata->ArtList,
                    SWA_ListSearch_CmpMethod,    SWM_NArticleList_SrchCmp,
                    SWA_ListSearch_Dynamic,      TRUE,
                    SWA_ListSearch_First,        TRUE,
                    SWA_ListSearch_WindowID,     MAKE_ID('A','l','S','r'),
                    SWA_ListSearch_ItemName,     GetS(SWS_L_ALItem),
                    SWA_ListSearch_ItemNamePl,   GetS(SWS_L_ALItems),
                    SWA_ListSearch_UI, ColGroup(2),
                       Child, Label2(GetS(SWS_L_ALQS_Hdr)),
                       Child, idata->ALQS_cg = Cycle(SWGetObj(idata->ArtList,
                                                  SWA_NArticleList_HEntries)),
                       Child, Label2(GetS(SWS_L_ALQS_Upd)),
                       Child, HGroup,
                          Child, idata->ALQS_upd  = CheckMark(TRUE),
                          Child, Label2(GetS(SWS_L_ALQS_SRam)),
                          Child, idata->ALQS_sram = CheckMark(FALSE),
                          Child, HSpace(0),
                       End,
                    End,
                    TAG_DONE);

   if (!qs) return NULL;

   SetAttrs(idata->ALQS_cg,
            MUIA_CycleChain,  TRUE,
            MUIA_ShortHelp,   GetS(SWS_H_ALMatchHdr),
            MUIA_ControlChar, 'h',
            TAG_DONE);

   SetAttrs(idata->ALQS_upd,
            MUIA_CycleChain, TRUE,
            MUIA_ShortHelp,  GetS(SWS_H_ALUpdDisp),
            MUIA_Disabled,   TRUE,
            MUIA_ControlChar, 'u',
            TAG_DONE);

   SetAttrs(idata->ALQS_sram,
            MUIA_CycleChain, TRUE,
            MUIA_ShortHelp,  GetS(SWS_H_ALFromRam),
            MUIA_Disabled,   TRUE,
            TAG_DONE);

   DoMethod(idata->ALQS_cg, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            o, 1, SWM_NGroup_ALSearchUIUpd);

   DoMethod(idata->ALQS_upd, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 1, SWM_NGroup_ALSearchUIUpd);

   DoMethod(idata->ALQS_sram, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 1, SWM_NGroup_ALSearchUIUpd);
   
   return qs;
}

/*****************************************************************************
 * Function: Open search window                                              *
 *****************************************************************************/

ULONG ALQSWin(Object *o, SW_IDATA *idata)
{
   if (!idata->ALQSearch)
      if (!(idata->ALQSearch = CreateALQSrch(o, idata)))
         return FAILURE;
   
   DoMethod(idata->ALQSearch, SWM_ListSearch_OpenWin, NN_App(idata));

   return SUCCESS;
}


/*****************************************************************************
 * Function: Limit to selected                                               *
 *****************************************************************************/

ULONG LimitSel(Object *o, SW_IDATA *idata)
{
   ULONG   x, Count = 0;
   Object *art, **arts;
   
   if (!idata->ArtList) return FAILURE;

   // -- select the active article -------------------------------------------
   
   SW_MUIListLoopSel(idata->ArtList, x, art, TRUE)
      Count++;

   if (!Count) return SUCCESS;
   
   if (!(arts = (Object **)AV(sizeof(art) * (Count+2))))
      return FAILURE;

   Count = 0;
   SW_MUIListLoopSel(idata->ArtList, x, art, TRUE)
      arts[Count++] = art;

   arts[Count] = NULL;

   idata->SelArticleCnt =
      DoMethod(idata->SWA_NGroup_ArtMgr, SWM_NArticleMgr_FilterList,
               arts, Count,
               &idata->SelArticles, &idata->SelArticleMax,
               0, FALSE, 1, FALSE, o, NULL);

   FV(arts);

   return DoMethod(o, SWM_NGroup_ChgUIFlt, 0);
}


/*****************************************************************************
 * Function: Load all articles into RAM, if possible                         *
 *****************************************************************************/

static void ALSearchLoad(Object *o, SW_IDATA *idata)
{
   Object *StatusTxt = SWGetObj(idata->ALQSearch, SWA_ListSearch_StatusTxt);
   TEXT   Txt[80];
   ULONG  TotSize = 0;
   LONG   x;
   Object *Art;

   // -- scope out the size -----------------------------------------------

   SW_MUIListLoop(idata->ArtList, x, Art, TRUE)
      if (DoMethod(Art, SWM_NArticle_FlagTest, SWF_NArticle_CachedHdBd))
         TotSize += DoMethod(Art, SWM_NArticle_GetBytes);

   if (TotSize < (AvailMem(MEMF_FAST) - SWV_NGroup_QS_MinFree)) {
      AppSleep(idata, TRUE);

      SW_MUIListLoop(idata->ArtList, x, Art,
                     AvailMem(MEMF_FAST) > SWV_NGroup_QS_MinFree) {
         if (DoMethod(Art, SWM_NArticle_FlagTest, SWF_NArticle_CachedHdBd)) {
            DoMethod(idata->App, MUIM_Application_CheckRefresh);
   
            if (!(x % 10)) {
               sprintf(Txt, GetS(SWS_L_ALQS_Loading), x);
               set(StatusTxt, MUIA_Text_Contents, Txt);
            }
   
            DoMethod(Art, SWM_NArticle_GetCache,
                     SWT_NArticle_MsgID,    DoMethod(Art, SWM_NArticle_GetMsgID),
                     SWT_NArticle_Parts,    SWF_NArticle_HdBd,
                     SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
                     TAG_DONE);
         }
      }

      set(StatusTxt, MUIA_Text_Contents, "");
      AppSleep(idata, FALSE);
   } else {
      MUI_Request(idata->App, SWGetObj(idata->ALQS_sram, MUIA_WindowObject),
                  0, NULL,
                  GetS(SWS_L_ALQS_OK), GetS(SWS_L_ALQS_TooBig),
                  SWFmtSize(TotSize, Txt+0, 2),
                  SWFmtSize(AvailMem(MEMF_FAST), Txt+20, 2));

      nnset(idata->ALQS_sram, MUIA_Selected, FALSE);
   }
}


/*****************************************************************************
 * Function: Load all articles into RAM, if possible                         *
 *****************************************************************************/

static void ALSearchFree(SW_IDATA *idata)
{
   Object *Art,
          *DspArt = SWGetObj(idata->SWA_NGroup_ArtView, SWA_NArticleView_DspArt),
          *StatusTxt = SWGetObj(idata->ALQSearch, SWA_ListSearch_StatusTxt);
   LONG x;
   TEXT Txt[80];

   AppSleep(idata, TRUE);

   SW_MUIListLoop(idata->ArtList, x, Art, TRUE) {
      if (!(x % 25)) {
         sprintf(Txt, GetS(SWS_L_ALQS_Freeing), x);
         set(StatusTxt, MUIA_Text_Contents, Txt);
      }
      
      if (Art != DspArt)
         DoMethod(Art, SWM_NArticle_FreeText, SWF_NArticle_Body);
   }

   set(StatusTxt, MUIA_Text_Contents, "");
   AppSleep(idata, FALSE);
}


/*****************************************************************************
 * Function: artlist UI search update                                        *
 *****************************************************************************/

ULONG ALSearchUIUpd(Object *o, SW_IDATA *idata)
{
   ULONG  QSH;
   ULONG  OldFR = SWGetUL(idata->ArtList, SWA_NArticleList_QSFromRam),
          NewFR = SWGetUL(idata->ALQS_sram, MUIA_Selected);

   if (!idata->ALQSearch) return FAILURE;

   // -- tell the artlist how to search --------------------------------------

   SetAttrs(idata->ArtList,
            SWA_NArticleList_QSH,       QSH=SWGetUL(idata->ALQS_cg,
                                                    MUIA_Cycle_Active),
            SWA_NArticleList_QSUpdDisp, SWGetUL(idata->ALQS_upd,  MUIA_Selected),
            SWA_NArticleList_QSFromRam, NewFR,
            TAG_DONE);

   SetAttrs(idata->ALQSearch,
            SWA_ListSearch_UpdSrchCnt, QSH < 14 ? 0 : 25,
            TAG_DONE);

   if (!OldFR && NewFR) ALSearchLoad(o, idata);
   if (OldFR && !NewFR) ALSearchFree(idata);

   set(idata->ALQS_upd,  MUIA_Disabled, QSH < 14);
   set(idata->ALQS_sram, MUIA_Disabled, QSH < 14);
   
   return SUCCESS;
}


/*****************************************************************************
 * Function: Run kill filters on array of articles                           *
 *****************************************************************************/

ULONG KillFltArray(Object *o, SW_IDATA *idata,
                          Object **ArtArray, ULONG ArtCount)
{
   ULONG x, AllocKill = FALSE, rc = FAILURE;
   
   if (ArtCount) {
      if (!idata->Filter_GlobKill) {
         idata->Filter_GlobKill = NSFListObject,
            SWA_NList_Master,    NN_GlobKill(idata),
            SWA_NSFList_FltType, "#?NSFilter#?",
            SWA_NSFList_UIMode,  SWV_NSFList_UI_Tiny,
            SWA_NSFList_Group,   o,
         End;
         AllocKill = TRUE;
      }
      
      set(idata->SWA_NGroup_KillFlt, SWA_NSFList_DefMatch, FALSE);
      set(idata->Filter_GlobKill,    SWA_NSFList_DefMatch, FALSE);

      if (DoMethod(idata->SWA_NGroup_KillFlt, SWM_NSFilter_Begin, o) == FAILURE)
         goto Done1;

      if (DoMethod(idata->Filter_GlobKill, SWM_NSFilter_Begin, o) == FAILURE)
         goto Done2;

      if (DoMethod(idata->SWA_NGroup_GrpDefFlt, SWM_NSFilter_Begin, o) == FAILURE)
         goto Done3;

      set(idata->SWA_NGroup_GrpDefFlt, SWA_NSF_InGroup_Killed, FALSE);
      
      // -- process kill filter on articles ----------------------------------

#     define KMatch(f, a, o) (DoMethod(f, SWM_NSFilter_Match, a, o))

      for (x=0; x<ArtCount; x++)
         if ((KMatch(idata->SWA_NGroup_KillFlt,   ArtArray[x], o) ||
              KMatch(idata->Filter_GlobKill,      ArtArray[x], o)) &&
             KMatch(idata->SWA_NGroup_GrpDefFlt, ArtArray[x], o))
            DoMethod(ArtArray[x], SWM_NArticle_Kill,
                     SWT_NArticle_CacheDir, OVDir(o, idata, NULL),
                     TAG_DONE);
   }

   if (AllocKill) {
      MUI_DisposeObject(idata->Filter_GlobKill);
      idata->Filter_GlobKill = NULL;
   }

   rc = SUCCESS;

  Done3:
   DoMethod(idata->SWA_NGroup_GrpDefFlt, SWM_NSFilter_End);
  Done2:
   DoMethod(idata->SWA_NGroup_KillFlt,   SWM_NSFilter_End);
  Done1:
   DoMethod(idata->Filter_GlobKill,      SWM_NSFilter_End);
   
   return rc;
}


/*****************************************************************************
 * Function: various fns for making UI parts                                 *
 *****************************************************************************/

Object *ImageB(ULONG i, LONG Cmd, LONG h)
{
   return ImageObject,
      ImageButtonFrame,
      MUIA_Image_Spec,  i,
      MUIA_CycleChain,  1,
      MUIA_InputMode,   MUIV_InputMode_RelVerify,
      MUIA_ControlChar, Cmd,
      MUIA_ShortHelp,   GetS(h),
   End;
}


/*****************************************************************************
 * Function: Create UI object (button/cycle)                                 *
 *****************************************************************************/

Object *ImageBt(SW_IDATA *idata, ULONG Mode, ULONG Ctrl, STRPTR Img,
                       ULONG Help)
{
   return DTImgObject,
      MUIA_InnerTop,        0,
      MUIA_InnerBottom,     0,
      MUIA_InnerLeft,       0,
      MUIA_InnerRight,      0,
      MUIA_Frame,           MUIV_Frame_ImageButton,
      MUIA_InputMode,       Mode,
      MUIA_ControlChar,     Ctrl,
      MUIA_ShortHelp,       GetS(Help),
      MUIA_CycleChain,      1,
      SWA_DTImg_FSpec,      Img,
      SWA_DTImg_ImgCache,   NN_DTImages(idata),
      SWA_DTImg_Path,       NN_ButtonPath(idata),
   End;
}

Object *MkTB(SW_IDATA *idata, ULONG h, STRPTR Image, LONG Cmd)
{
   return ImageBt(idata, MUIV_InputMode_Toggle, Cmd, Image, h);
}

Object *MkIB(SW_IDATA *idata, ULONG h, STRPTR Image, LONG Cmd)
{
   return ImageBt(idata, MUIV_InputMode_RelVerify, Cmd, Image, h);
}

Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }


/*****************************************************************************
 * Function: Report on missing parts of multipart post                       *
 *****************************************************************************/

ULONG MPReportMissing(SW_IDATA *idata, MPInfo *MP)
{
#  define MaxMPMc 16
   ULONG NotFound, x;
   WORD Missing[MaxMPMc];
   TEXT MissList[MaxMPMc * 6 + 2];
   STRPTR s;
      
   if ((NotFound = DoMethod(idata->SWA_NGroup_ArtMgr,
                           SWM_NArticleMgr_MultiPart_Check,
                            MP, Missing, MaxMPMc)) == 0)
      return NotFound;

   for (x=0, s = MissList; Missing[x] >= 0; x++) {
      sprintf(s, "%ld,", Missing[x]);
      s += strlen(s);
   }

   if (s != MissList) *(--s) = '\0';
   if (NotFound >= MaxMPMc)
      strcpy(s, "...");

    MUI_Request(NN_App(idata), idata->ReaderWin, 0, NULL,
                GetS(SWS_L_MissCont), GetS(SWS_L_MissMsg),
                NotFound, MP[0].MaxPart, MissList);

   return NotFound;
}


/*****************************************************************************
 * Function: remove article list                                             *
 *****************************************************************************/

ULONG RemArts(Object *o, SW_IDATA *idata, ULONG Threads, ULONG SameAuth, Object *DelObj,
              ULONG DelMeth, ULONG MarkAsRem)
{
   ULONG Count, x;
   
   if (!idata->ArtList) return FAILURE;

   AppSleep(idata, TRUE);
   set(idata->ArtList, MUIA_List_Quiet, TRUE);

   if (SameAuth) SelectAuth(idata);
   
   // -- kill present articles. ----------------------------------------------

   if (idata->KList = GetSelArts(idata, Threads, &Count)) {
      DoMethod(DelObj, DelMeth,
               SWT_NArticleMgr_ArticleArray, idata->KList,
               SWT_NArticleMgr_ArticleCount, Count,
               SWT_NArticleMgr_CacheDir,     OVDir(o, idata, NULL),
               TAG_DONE);

      if (MarkAsRem)
         for (x=0; x<Count; x++)
            DoMethod(idata->KList[x], SWM_NArticle_FlagSet, SWF_NArticle_Removed);
   }

   set(o, SWA_NGroup_ReadArtCnt,
       min(idata->SWA_NGroup_TotArtCnt, idata->SWA_NGroup_ReadArtCnt+Count));
   
   FV(idata->KList);
   idata->KList = NULL;

   set(idata->ArtList, MUIA_List_Quiet, FALSE);
   DoMethod(o, SWM_NGroup_ChgThreadFlt, idata->SWA_NGroup_ThreadFT, TRUE, FALSE);
   AppSleep(idata, FALSE);
   
   return SUCCESS;
}


/*****************************************************************************
 * Function: Test if we're in online mode                                    *
 *****************************************************************************/

ULONG IsOnline(SW_IDATA *idata)
{
   return SWGetUL(NN_TCPList(idata), SWA_NTCPList_Online);
}


# endif // _GENPROTO
