/*****************************************************************************
 * Method: Begin match tests                                                 *
 *****************************************************************************/

METHOD(SWM_NSFilter_Begin, SWP_NSFilter_Begin *)
{
   ULONG rc = DoSuperMethodA(cl, o, msg);

   idata->Total = idata->Found = 0;
   idata->Inv   = SWGetUL(o, SWA_NSFilter_Inv);

   idata->TM = msg->Grp ? SWGetObj(msg->Grp, SWA_NGroup_ThreadMgr) : NULL;

   idata->PrevBase   = AllocVec(SWV_NSF_MatchThreadBase_CSize * sizeof(Object **),
                                MEMF_ANY | MEMF_CLEAR);
   idata->PrevMatch  = AllocVec(SWV_NSF_MatchThreadBase_CSize * sizeof(UBYTE),
                                MEMF_ANY | MEMF_CLEAR);

   if (!idata->PrevBase || !idata->PrevMatch)
      rc = FAILURE;
   else
      memset(idata->PrevMatch, 2, SWV_NSF_MatchThreadBase_CSize * sizeof(UBYTE));

   if (idata->SWA_NSF_MatchThreadBase_Filter)
      if (DoMethodA(idata->SWA_NSF_MatchThreadBase_Filter, msg) != SUCCESS)
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

   if (idata->TM && idata->SWA_NSF_MatchThreadBase_Filter) {
      Object *Base = (Object *)
         DoMethod(idata->TM, SWM_NArticleMgr_ThreadBase, msg->Art);
      ULONG HPos = ((ULONG)(Base)>>2) % SWV_NSF_MatchThreadBase_CSize;

      // -- look for cached answer from previous threads ---------------------
      
      if (Base == idata->PrevBase[HPos])
         if ((Match = idata->PrevMatch[HPos]) != 2) goto Done;

      // -- didn`t find one, so check the base article -----------------------

      if (!Base) goto Done;
         
      Match = DoMethod(idata->SWA_NSF_MatchThreadBase_Filter,
                       SWM_NSFilter_Match, Base, msg->Grp);

      // -- cache the answer in case we find another art in this thread later- 

      idata->PrevBase[HPos]  = Base;
      idata->PrevMatch[HPos] = (Match ? 1 : 0);
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
   if (idata->SWA_NSF_MatchThreadBase_Filter)
      DoMethodA(idata->SWA_NSF_MatchThreadBase_Filter, msg);

   FreeVec(idata->PrevBase);
   FreeVec(idata->PrevMatch);
   idata->PrevBase   = NULL;
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
   Object *HSP = HVSpace,
           *ui = (Object *)
              DoMethod(o, SWM_NSFilter_ImbUI,
                       &idata->SWA_NSF_MatchThreadBase_Filter,
                       HSP,
                       GetS(SWS_H_New));

   if (!ui)
      MUI_DisposeObject(HSP);
   
   SWDB_RET(ui);
} SDFT


/*****************************************************************************
 * Method: Insert a new object                                               *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
   if (idata->SWA_NSF_MatchThreadBase_Filter)
      DoMethodA(idata->SWA_NSF_MatchThreadBase_Filter, msg);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Expire children if possible                                       *
 *****************************************************************************/

METHOD(SWM_NSFilter_Expire, Msg)
{
   ULONG rc = SUCCESS;
   
   if (idata->SWA_NSF_MatchThreadBase_Filter)
      DoMethodA(idata->SWA_NSF_MatchThreadBase_Filter, msg);

   SWDB_RET(rc);
} SDFT
