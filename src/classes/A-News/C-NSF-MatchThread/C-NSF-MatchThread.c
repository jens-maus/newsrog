# include "C-NSF-MatchThread-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

static STRPTR MLab[3];

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_OBO(SWA_NSF_MatchThread_Filter,
            SWA_NSF_MatchThread_FPtr, SWAM_NoLab, SWAM_SGI),
   SWAM_CY(SWA_NSF_MatchThread_Mode, SWS_L_Mode, SWAM_UI,
           SWV_NSF_MatchThread_Or, 2, MLab),
   SWP_Root_Attrs,
   SWAM_DONE
};


static HelpMap SW_HELPMAP[] = {
   SWA_NSF_MatchThread_Mode, SWS_H_Mode,
   NULL, NULL,
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Mode: ",
   "Any",
   "All",
   
   "Set a new condition which must be matched\n"
      "by any (or all) of the other articles in\n"
      "the same thread as this article.",

   "Controls whether the match must succeed on\n"
      "any article in the thread, or all articles.",
   
   NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods                        \
  SW_DO_MF(SWM_NSFilter_Begin);            \
  SW_DO_MF(SWM_NSFilter_Match);            \
  SW_DO_MF(SWM_NSFilter_End);              \
  SW_DO_MF(SWM_NSFilter_UI);               \
  SW_DO_MF(SWM_NSFilter_HideUI);           \
  SW_DO_MF(SWM_NSFilter_TrackStats);       \
  SW_DO_MF(SWM_NSFilter_Expire);           \

# include "C-NSF-MatchThread-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
