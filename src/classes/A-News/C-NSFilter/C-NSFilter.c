# include "C-NSFilter-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSFilter

// -- attribute map ----------------------------------------------------------

// keep Desc as 1st attr

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NSFilter_Desc,         SWAM_NoLab, SWAM_SGI, NULL, MaxFltDescLen),
   SWAM_IN(SWA_NSFilter_TotHist,      SWAM_NoLab, SWAM_SGI, 0),
   SWAM_IN(SWA_NSFilter_FoundHist,    SWAM_NoLab, SWAM_SGI, 0),
   SWAM_IN(SWA_NSFilter_TotNow,       SWAM_NoLab, SWAM_SGI, 0),
   SWAM_IN(SWA_NSFilter_FoundNow,     SWAM_NoLab, SWAM_SGI, 0),
   SWAM_BO(SWA_NSFilter_Inv,          SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_BO(SWA_NSFilter_Enabled,      SWAM_NoLab, SWAM_SGI, TRUE),
   SWAM_IN(SWA_NSFilter_LastMatchDay, SWAM_NoLab, SWAM_SGI, LONG_MAX),
   SWAM_IN(SWA_NSFilter_LastMatchDayDsp, SWAM_NoLab, SWAM_SGI, LONG_MAX),
   SWAM_IN(SWA_NSFilter_ExpireDate,   SWAM_NoLab, SWAM_SGI, LONG_MAX),
   SWAM_BO(SWA_NSFilter_ExpireDateOK, SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_IN(SWA_NSFilter_ExpDisuse,    SWAM_NoLab, SWAM_SGI, 10),
   SWAM_BO(SWA_NSFilter_ExpDisuseOK,  SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_SA(SWA_NSFilter_Groups,       SWAM_NoLab, SWAM_SGI, NULL, 4096),
   SWAM_SA(SWA_NSFilter_AuthNote,     SWAM_NoLab, SWAM_SGI, NULL, LONG_MAX),
   SWAM_IN(SWA_NSFilter_ThreadMode,   SWAM_NoLab, SWAM_SGI, SWV_NSFilter_TM_Base),
   SWAM_BO(SWA_NSFilter_UseOnOffBt,   SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_SA(SWA_NSFilter_OnOffBtImg,   SWAM_NoLab, SWAM_SGI, NULL, FMSIZE),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Article Filter",
   "Global Info",
   "Filter Settings",
   "Stats",
   "Author Note",

   "Description: ",
   "Enabled: ",
   "Invert: ",
   "Expire Date: ",
   "Expire Disuse: ",
   "Thread Mode: ",
      "Base",
      "Any",
      "All",

   "Total History",
   "Last Use",
   "Matched ",
   " of ",
   MUIX_L "Last Match: ",
   MUIX_L "Expires On: ",
   "_Update",
   "_Reset",
   "Toolbar Button: ",
   SWX_I("Never"),

   "A description string displayed in\n"
   "filter lists.  Use any meaningful\n"
   "string.",

   "If set, this filter is enabled.  Otherwise,\n"
   "it will have no effect.  Provides an easy\n"
   "way to turn on or off the effects of filters.",

   "If set, this filter will match all the\n"
   "articles it normally would not match, and\n"
   "vice versa.",

   "The date upon which this filter will expire.",
     
   "The number of days after the last successful\n"
      "match that this filter will expire.  This\n"
      "allows the creation of filters that match\n"
      "active threads and eventually expire when the\n"
      "thread they matched stops being replied to.",

   "Controls the operation of the filter when used\n"
      "as a selection filter.  If the article list in\n"
      "the group being read is set to display only\n"
      "thread entry points, then this setting controls\n"
      "how the filter is applied to the thread:\n\n"
      SWX_B("  Base ") " - In this mode, a thread is shown only if\n"
      "  the thread basenote matches.\n\n"
      SWX_B("  Any ") " - In this mode, a thread is shown if any\n"
      "  article in the thread matches.\n\n"
      SWX_B("  All ") " - In this mode, a thread is shown only if\n"
      "  all articles in the thread match.",

   "Force the last match date to be updated\n"
      "to the current date.",

   "Resets the filter statistics.",

   "If enabled, this filename will be used to\n"
   "create a filter toolbar button which can be\n"
   "used to toggle this filter on or off and\n"
   "update the article list accordingly.",

   "Turn the " SWX_B("%s") " selection filter on or off.",

   "New...",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


static void SetupObj(Object *o, SW_IDATA *idata)
{
   struct DateStamp ds;

   DateStamp(&ds);

   idata->SWA_NSFilter_ExpireDate      = ds.ds_Days + SWV_NSF_ExpDef;
   idata->SWA_NSFilter_LastMatchDay    = ds.ds_Days;

   setlent(o, SWA_ObjNoChildCopy, TRUE);
}


# define SW_USERSETUP    ExpireSliderInit(libbase); \
                         DisuseSliderInit(libbase)

# define SW_USERCLEANUP  ExpireSliderExit(); \
                         DisuseSliderExit()
   
# define SW_SETUP_POSTSET SetupObj((Object *)retval, idata);

# define DISPOSECUST      DoMethod(o, SWM_NSFilter_HideUI); \
                          if (idata->MemPool) \
                             FV(idata->TBDesc);

# define SW_Methods                         \
  SW_DO_MF(SWM_NSFilter_Begin);             \
  SW_DO_MF(SWM_NSFilter_Match);             \
  SW_DO_MF(SWM_NSFilter_End);               \
  SW_DO_MF(SWM_NSFilter_UI);                \
  SW_DO_MF(SWM_NSFilter_ShowUI);            \
  SW_DO_MF(SWM_NSFilter_HideUI);            \
  SW_DO_MF(SWM_NSFilter_Expire);            \
  SW_DO_MF(SWM_NSFilter_ExpireList);        \
  SW_DO_MF(SWM_NSFilter_UpdStatWin);        \
  SW_DO_MF(SWM_NSFilter_UpdLastMatch);      \
  SW_DO_MF(SWM_NSFilter_ResetStats);        \
  SW_DO_MF(SWM_NSFilter_ExpireSliderClass); \
  SW_DO_MF(SWM_NSFilter_DisuseSliderClass); \
  SW_DO_MF(SWM_NSFilter_NewImbUI);          \
  SW_DO_MF(SWM_NSFilter_ImbUI);             \
  SW_DO_MF(SWM_NSFilter_ChildUI);           \
  SW_DO_MF(SWM_NSFilter_AddFFIcons);        \
  SW_DO_MF(SWM_NSFilter_RemFFIcon);         \
  SW_DO_MF(SWM_NSFilter_FF_OnOff);          \
  SW_DO_MF(SWM_NSFilter_TBUI);              \

# include "C-ExpireSlider.h"
# include "C-NSFilter-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
