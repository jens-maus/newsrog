# include "C-NDFilter-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NDFilter

// -- attribute map ----------------------------------------------------------

// keep Desc as 1st attr

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NDFilter_Desc,       SWAM_NoLab, SWAM_SGI,  NULL, 256),
   SWAM_BO(SWA_NDFilter_Enabled,    SWAM_NoLab, SWAM_SGI,  TRUE),
   SWAM_SA(SWA_NDFilter_Groups,     SWAM_NoLab, SWAM_SGI,  NULL, 256),
   SWAM_FI(SWA_NDFilter_Pri,        SWAM_NoLab, SWAM_SGIN, 0),
   SWAM_SA(SWA_NDFilter_AuthNote,   SWAM_NoLab, SWAM_SGI, NULL, LONG_MAX),
   SWAM_BO(SWA_NDFilter_UseOnOffBt, SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_SA(SWA_NDFilter_OnOffBtImg, SWAM_NoLab, SWAM_SGI, NULL, FMSIZE),
   SWAM_BO(SWA_NDFilter_T_Print,    SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_BO(SWA_NDFilter_T_Save,     SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_BO(SWA_NDFilter_T_NetReply, SWAM_NoLab, SWAM_SGI, FALSE),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Display Filter",
   "Global Info",
   "Filter Settings",
   "Author Note",

   "Description:",
   "Enabled:",
   "Toolbar Button: ",
   "For Printing: ",
   "Saving: ",
   "Replies: ",

   "A description string displayed in\n"
   "filter lists.  Use any meaningful\n"
   "string.",

   "If set, this filter is enabled.  Otherwise,\n"
   "it will have no effect.  Provides an easy\n"
   "way to turn on or off the effects of filters.",

   "If enabled, this filename will be used to\n"
   "create a filter toolbar button which can be\n"
   "used to toggle this filter on or off and update\n"
   "the currently displayed article accordingly.",

   "Turn the " SWX_B("%s") " display filter on or off.",

   "If checked, this filter will be used to modify\n"
   "article text that is sent to the printer.",
      
   "If checked, this filter will be used to modify\n"
   "article text that is saved to disk files.",
      
   "If checked, this filter will be used to modify\n"
   "replies to this article.",
      
   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static void SetupObj(Object *o, SW_IDATA *idata)
{
   setlent(o, SWA_ObjNoChildCopy, TRUE);
}

# define SETUPCUST   SetupObj((Object *)retval, idata)

# define DISPOSECUST DoMethod(o, SWM_NDFilter_HideUI); \
                     if (idata->MemPool) \
                        FV(idata->TBDesc);

# define SW_Methods                     \
  SW_DO_MF(SWM_NDFilter_UI);            \
  SW_DO_MF(SWM_NDFilter_ShowUI);        \
  SW_DO_MF(SWM_NDFilter_HideUI);        \
  SW_DO_MF(SWM_NDFilter_TBAppend);      \
  SW_DO_MF(SWM_NDFilter_TBAppendTxt);   \
  SW_DO_MF(SWM_NDFilter_TBPrependTxt);  \
  SW_DO_MF(SWM_NDFilter_TBAppendSub);   \
  SW_DO_MF(SWM_NDFilter_TBSet);         \
  SW_DO_MF(SWM_NDFilter_TBClear);       \
  SW_DO_MF(SWM_NDFilter_TBDispose);     \
  SW_DO_MF(SWM_NDFilter_TBInit);        \
  SW_DO_MF(SWM_NDFilter_TBAssign);      \
  SW_DO_MF(SWM_NDFilter_TBAssignDel);   \
  SW_DO_MF(SWM_NDFilter_TBIsEncaps);    \
  SW_DO_MF(SWM_NDFilter_TBNoCtrl);      \
  SW_DO_MF(SWM_NDFilter_TBTruncate);    \
  SW_DO_MF(SWM_NDFilter_StartMatch);    \
  SW_DO_MF(SWM_NDFilter_EndMatch);      \
  SW_DO_MF(SWM_NDFilter_StyleOn);       \
  SW_DO_MF(SWM_NDFilter_StyleOff);      \
  SW_DO_MF(SWM_NDFilter_StyleTxt);      \
  SW_DO_MF(SWM_NDFilter_TBGetSpace);    \
  SW_DO_MF(SWM_NDFilter_TBAppendChar);  \
  SW_DO_MF(SWM_NDFilter_TBSkipHdrs);    \
  SW_DO_MF(SWM_NDFilter_LastParseLine); \
  SW_DO_MF(SWM_NDFilter_TBMultiInit);   \
  SW_DO_MF(SWM_NDFilter_TBMultiDispose);\
  SW_DO_MF(SWM_NDFilter_AddFFIcons);    \
  SW_DO_MF(SWM_NDFilter_RemFFIcon);     \
  SW_DO_MF(SWM_NDFilter_FF_OnOff);      \
  SW_DO_MF(SWM_NDFilter_TBUI);          \
  SW_DO_MF(SWM_NDFilter_TBDelSub);      \
  SW_DO_MF(SWM_NDFilter_TBCountChar);   \
  SW_DO_MF(SWM_NDFilter_TBAddPrefix);   \

# include "C-NDFilter-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
