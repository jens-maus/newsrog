/*****************************************************************************
 * Default filter stuff                                                      *
 *****************************************************************************/

/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# ifndef SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
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

# ifndef SWF_FilterHIDEUIF
METHOD(SWM_NDFilter_HideUI, Msg)
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
# endif


/*****************************************************************************
 * Methods we accept                                                         *
 *****************************************************************************/

# ifndef SW_Methods
#    ifdef NDF_AddObj
#       define SW_Methods                  \
        SW_DO_MF(SWM_NDFilter_ModTxt);     \
        SW_DO_MF(SWM_NDFilter_StartMatch); \
        SW_DO_MF(SWM_NDFilter_EndMatch);   \
        SW_DO_MF(SWM_NDFilter_UI);         \
        SW_DO_MF(SWM_NDFilter_HideUI);     \
        SW_DO_MF(SWM_NDFilter_GenAddObj);  \
        SW_DO_MF(SWM_NDFilter_RemAddObj)
#    else
#       define SW_Methods                  \
        SW_DO_MF(SWM_NDFilter_ModTxt);     \
        SW_DO_MF(SWM_NDFilter_StartMatch); \
        SW_DO_MF(SWM_NDFilter_EndMatch);   \
        SW_DO_MF(SWM_NDFilter_UI);         \
        SW_DO_MF(SWM_NDFilter_HideUI)
#    endif
# endif
