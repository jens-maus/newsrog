/*****************************************************************************
 * Default filter stuff                                                      *
 *****************************************************************************/

/*****************************************************************************
 * Method: Begin match tests                                                 *
 *****************************************************************************/

METHOD(SWM_NSFilter_Begin, SWP_NSFilter_Begin *)
{
   idata->Total = idata->Found = 0;
   idata->Inv   = SWGetUL(o, SWA_NSFilter_Inv);

#  ifdef SWF_FilterBegin
     SWF_FilterBegin;
#  endif
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Determine if article is a match                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_Match, SWP_NSFilter_Match *)
{
   ULONG Match;
   
   if (!msg->Art) SWDB_RET(FALSE);

   idata->Total++;

   SWF_FilterMatch;
   
   if (idata->Inv) Match = !Match;
   if (Match)      idata->Found++;

   SWDB_RET(Match);
} SDFT
   

/*****************************************************************************
 * Method: End match tests                                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_End, Msg)
{
#  ifdef SWF_FilterEnd
     SWF_FilterEnd;
#  endif

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
#  ifdef SWF_FilterUI
      SWF_FilterUI;
#  else
      SWDB_RET(idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o));
#  endif
} SDFT


/*****************************************************************************
 * Method: Hide UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
#  ifdef SWF_FilterHideUI
      SWF_FilterHideUI;
#  else
      if (idata->UI)
         SWKillAttrUI(SW_ATTRMAP, idata->UI, o);
      idata->UI = NULL;
#  endif
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Methods we accept                                                         *
 *****************************************************************************/

# define SW_Methods                  \
  SW_DO_MF(SWM_NSFilter_Begin);      \
  SW_DO_MF(SWM_NSFilter_Match);      \
  SW_DO_MF(SWM_NSFilter_End);        \
  SW_DO_MF(SWM_NSFilter_UI);         \
  SW_DO_MF(SWM_NSFilter_HideUI);     \
  SW_DO_MF(SWM_NSFilter_TrackStats);
