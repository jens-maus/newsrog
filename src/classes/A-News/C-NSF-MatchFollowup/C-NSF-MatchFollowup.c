# include "C-NSF-MatchFollowup-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_OBO(SWA_NSF_MatchFollowup_Filter,
            SWA_NSF_MatchFollowup_FPtr, SWAM_NoLab, SWAM_SGI),

   SWP_Root_Attrs,
   SWAM_DONE
};


static HelpMap SW_HELPMAP[] = {
   NULL, NULL,
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Set a new condition which must be matched\n"
      "by an ancestor to this article.  That is,\n"
      "this article must be a followup to an article\n"
      "that is matched by this condition.",

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

# include "C-NSF-MatchFollowup-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
