/*****************************************************************************
 * Method: Begin match tests                                                 *
 *****************************************************************************/

METHOD(SWM_NSFilter_Begin, SWP_NSFilter_Begin *)
{
   ULONG rc = DoSuperMethodA(cl, o, msg), x;
      
   idata->Total = idata->Found = 0;
   idata->Inv   = SWGetUL(o, SWA_NSFilter_Inv);

   idata->TM = msg->Grp ? SWGetObj(msg->Grp, SWA_NGroup_ThreadMgr) : NULL;

   idata->MD         = AllocVec(SWV_NSF_MatchFollowup_CSize * sizeof(MatchData),
                                MEMF_ANY | MEMF_CLEAR);

   idata->Followups  = AllocVec(SWV_NSF_MatchFollowup_MaxFDist * sizeof(Object *),
                                MEMF_ANY | MEMF_CLEAR);

   if (!idata->MD || !idata->Followups)
      rc = FAILURE;
   else
      for (x=0; x<SWV_NSF_MatchFollowup_CSize; x++)
         idata->MD[x].PrevMatch  = 2,
         idata->MD[x].PrevParent = ParentUnknown;

   if (idata->SWA_NSF_MatchFollowup_Filter)
      if (DoMethodA(idata->SWA_NSF_MatchFollowup_Filter, msg) != SUCCESS)
         rc = FAILURE;

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: get next parent                                                 *
 *****************************************************************************/

__inline static Object *NextP(SW_IDATA *idata, Object *Art, ULONG HPos)
{
   if (idata->MD[HPos].PrevArt    == Art &&
       idata->MD[HPos].PrevParent != ParentUnknown)
      return idata->MD[HPos].PrevParent;
   else
      return (Object *)DoMethod(idata->TM,SWM_NArticleMgr_ThreadParent,Art);
}


/*****************************************************************************
 * Method: Determine if article is a match                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_Match, SWP_NSFilter_Match *)
{
   ULONG Match = 0, HPos, FPos = 0, x;
   
   idata->Total++;

   if (idata->TM && idata->SWA_NSF_MatchFollowup_Filter) {
      Object *Art, *Parent;

      // -- first search all parents for a match -----------------------------
      
      for (Art = msg->Art; Art && !Match;) {
         HPos  = ((ULONG)(Art)>>2) % SWV_NSF_MatchFollowup_CSize;
         
         if (Art != msg->Art             &&
             idata->MD[HPos].PrevArt == Art && idata->MD[HPos].PrevMatch != 2)
            Match = idata->MD[HPos].PrevMatch;

         if (idata->MD[HPos].PrevArt != Art)
            idata->MD[HPos].PrevMatch = 2;

         idata->Followups[FPos++] = Art;
         
         Parent = NextP(idata, Art, HPos);
         idata->MD[HPos].PrevArt    = Art;
         idata->MD[HPos].PrevParent = Art = Parent;
      }

      // -- if not found in cache, do it the hard way ------------------------

      for (x=0; x<FPos && !Match; x++)
         Match = DoMethod(idata->SWA_NSF_MatchFollowup_Filter,
                          SWM_NSFilter_Match, idata->Followups[x], msg->Grp);
   }

   HPos  = ((ULONG)(msg->Art)>>2) % SWV_NSF_MatchFollowup_CSize;

   if (idata->MD[HPos].PrevArt == msg->Art)
      idata->MD[HPos].PrevMatch  = Match;
   else {
      idata->MD[HPos].PrevArt    = msg->Art;
      idata->MD[HPos].PrevParent = ParentUnknown;
      idata->MD[HPos].PrevMatch  = Match;
   }
   
   if (idata->Inv) Match = !Match;

   if (Match) idata->Found++;
   
   SWDB_RET(Match);
} SDFT
   

/*****************************************************************************
 * Method: End match tests                                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_End, Msg)
{
   if (idata->SWA_NSF_MatchFollowup_Filter)
      DoMethodA(idata->SWA_NSF_MatchFollowup_Filter, msg);

   FreeVec(idata->MD);
   FreeVec(idata->Followups);

   idata->MD        = NULL;
   idata->Followups = NULL;
   
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
   Object *HVS = HVSpace, *ui = (Object *)
      DoMethod(o, SWM_NSFilter_ImbUI,
               &idata->SWA_NSF_MatchFollowup_Filter,
               HVS,
               GetS(SWS_H_New));

   if (!ui)
      MUI_DisposeObject(HVS);
   
   SWDB_RET(ui);
} SDFT


/*****************************************************************************
 * Method: Insert a new object                                               *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
   if (idata->SWA_NSF_MatchFollowup_Filter)
      DoMethodA(idata->SWA_NSF_MatchFollowup_Filter, msg);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Expire children if possible                                       *
 *****************************************************************************/

METHOD(SWM_NSFilter_Expire, Msg)
{
   ULONG rc = SUCCESS;
   
   if (idata->SWA_NSF_MatchFollowup_Filter)
      rc = DoMethodA(idata->SWA_NSF_MatchFollowup_Filter, msg);

   SWDB_RET(rc);
} SDFT
