/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NSFilter_UI, Msg)
{
   SWDB_RET(idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o));
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
   if (idata->UI)
      SWKillAttrUI(SW_ATTRMAP, idata->UI, o);
   idata->UI = NULL;

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Update statistics after xfer                                      *
 *****************************************************************************/

METHOD(SWM_NSFilter_TrackStats, SWP_NSFilter_TrackStats *)
{
   *msg->Total = idata->SWA_NStrFilter_Total;
   *msg->Found = idata->SWA_NStrFilter_Found;

   SWDB_RET(0);
} SDFT
