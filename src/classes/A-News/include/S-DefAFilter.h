
/*****************************************************************************
 * Default filter stuff                                                      *
 *****************************************************************************/

/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# ifndef SWF_FilterUIF
METHOD(SWM_NAFilter_UI, Msg)
{
#  ifdef SWF_FilterUI
      SWF_FilterUI;
#  else
      SWDB_RET(idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o));
#  endif
} SDFT
# endif


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NAFilter_HideUI, Msg)
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

# ifndef SW_Methods
# define SW_Methods              \
  SW_DO_MF(SWM_NAFilter_Begin);  \
  SW_DO_MF(SWM_NAFilter_Action); \
  SW_DO_MF(SWM_NAFilter_End);    \
  SW_DO_MF(SWM_NAFilter_UI);     \
  SW_DO_MF(SWM_NAFilter_HideUI);
# endif
