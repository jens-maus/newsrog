# include "C-NSF-MatchThreadBase-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_OBO(SWA_NSF_MatchThreadBase_Filter,
            SWA_NSF_MatchThreadBase_FPtr, SWAM_NoLab, SWAM_SGI),
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Set a new condition which must be matched\n"
      "by the base article in the current thread.\n"
      "Under many curcumstances, matching base\n"
      "articles can be faster than using the same\n"
      "filter on every thread article.",
   
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

# include "C-NSF-MatchThreadBase-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
