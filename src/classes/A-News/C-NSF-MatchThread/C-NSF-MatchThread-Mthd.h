/*****************************************************************************
 * Method: Begin match tests                                                 *
 *****************************************************************************/

METHOD(SWM_NSFilter_Begin, SWP_NSFilter_Begin *)
{
   ULONG rc = DoSuperMethodA(cl, o, msg);
      
   idata->Total = idata->Found = 0;
   idata->Inv   = SWGetUL(o, SWA_NSFilter_Inv);

   idata->TM = msg->Grp ? SWGetObj(msg->Grp, SWA_NGroup_ThreadMgr) : NULL;

   idata->PrevThread = AllocVec(SWV_NSF_MatchThread_CSize * sizeof(Object **),
                                MEMF_ANY | MEMF_CLEAR);
   idata->PrevMatch  = AllocVec(SWV_NSF_MatchThread_CSize * sizeof(UBYTE),
                                MEMF_ANY | MEMF_CLEAR);

   if (!idata->PrevThread || !idata->PrevMatch)
      rc = FAILURE;
   else
      memset(idata->PrevMatch, 2, SWV_NSF_MatchThread_CSize * sizeof(UBYTE));

   if (idata->SWA_NSF_MatchThread_Filter)
      if (DoMethodA(idata->SWA_NSF_MatchThread_Filter, msg) != SUCCESS)
         rc = FAILURE;

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Determine if article is a match                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_Match, SWP_NSFilter_Match *)
{
   ULONG Match = 2;
   
   idata->Total++;

   if (idata->TM && idata->SWA_NSF_MatchThread_Filter) {
      Object *Base,
             **Thread = (Object **)
                  DoMethod(idata->TM, SWM_NArticleMgr_GetThreadArts,
                           msg->Art, &Base),
             **TPos = Thread;
      ULONG HPos = ((ULONG)(Thread)>>2) % SWV_NSF_MatchThread_CSize;

      // -- look for cached answer from previous threads ---------------------
      
      if (Thread && Thread == idata->PrevThread[HPos])
         if ((Match = idata->PrevMatch[HPos]) != 2) goto Done;

      // -- Check base article for a match -----------------------------------
         
      Match = DoMethod(idata->SWA_NSF_MatchThread_Filter,
                       SWM_NSFilter_Match, Base, msg->Grp);

      if (Match &&idata->SWA_NSF_MatchThread_Mode==SWV_NSF_MatchThread_Or ||
          !Match&&idata->SWA_NSF_MatchThread_Mode==SWV_NSF_MatchThread_And)
         goto Found;

      if (!Thread) goto Done;

      // -- didn`t find one - gotta search all the thread articles -----------

      for (; !ThreadArtEnd(*TPos); TPos++) {
         Match = DoMethod(idata->SWA_NSF_MatchThread_Filter,
                          SWM_NSFilter_Match, *TPos, msg->Grp);

         if (Match &&idata->SWA_NSF_MatchThread_Mode==SWV_NSF_MatchThread_Or ||
             !Match&&idata->SWA_NSF_MatchThread_Mode==SWV_NSF_MatchThread_And)
            break;
      }

      // -- cache the answer in case we find another art in this thread later- 

    Found:
      idata->PrevThread[HPos] = Thread;
      idata->PrevMatch[HPos]  = (Match ? 1 : 0);
   }

  Done:
   if (idata->Inv) Match = !Match;

   if (Match) idata->Found++;
   
   SWDB_RET(Match);
} SDFT
   

/*****************************************************************************
 * Method: End match tests                                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_End, Msg)
{
   if (idata->SWA_NSF_MatchThread_Filter)
      DoMethodA(idata->SWA_NSF_MatchThread_Filter, msg);

   FreeVec(idata->PrevThread);
   FreeVec(idata->PrevMatch);
   idata->PrevThread = NULL;
   idata->PrevMatch  = NULL;
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Update statistics after xfer                                      *
 *****************************************************************************/

METHOD(SWM_NSFilter_TrackStats, SWP_NSFilter_TrackStats *)
{
   *msg->Total = idata->Total;
   *msg->Found = idata->Found;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NSFilter_UI, Msg)
{
   Object *ui = (Object *)
      DoMethod(o, SWM_NSFilter_ImbUI,
               &idata->SWA_NSF_MatchThread_Filter,
               idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o),
               GetS(SWS_H_New));

   if (!ui) {
      MUI_DisposeObject(idata->UI);
      idata->UI = NULL;
   }
   
   SWDB_RET(ui);
} SDFT


/*****************************************************************************
 * Method: Insert a new object                                               *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
   if (idata->SWA_NSF_MatchThread_Filter)
      DoMethodA(idata->SWA_NSF_MatchThread_Filter, msg);

   if (idata->UI)
      SWKillAttrUI(SW_ATTRMAP, idata->UI, o);
   idata->UI = NULL;

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Expire children if possible                                       *
 *****************************************************************************/

METHOD(SWM_NSFilter_Expire, Msg)
{
   ULONG rc = SUCCESS;
   
   if (idata->SWA_NSF_MatchThread_Filter)
      rc = DoMethodA(idata->SWA_NSF_MatchThread_Filter, msg);

   SWDB_RET(rc);
} SDFT
