# include "C-NSF-PostRate-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

static STRPTR MLab[3];

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IB (SWA_NSF_PostRate_MinArts, SWS_L_MinArts,  SWAM_UI|SWAM_IntStr, 0, 0, 100000),
   SWAM_IB (SWA_NSF_PostRate_MaxArts, SWS_L_MaxArts,  SWAM_UI|SWAM_IntStr, 100000, 0, 100000),
   SWAM_IB (SWA_NSF_PostRate_MinPerDay, SWS_L_MinPerDay,  SWAM_UI|SWAM_IntStr, 0, 0, 100000),
   SWAM_IB (SWA_NSF_PostRate_MaxPerDay, SWS_L_MaxPerDay,  SWAM_UI|SWAM_IntStr, 100000, 0, 100000),

   SWP_Root_Attrs,
   SWAM_DONE
};


static HelpMap SW_HELPMAP[] = {
   SWA_NSF_PostRate_MinArts,   SWS_H_MinArts,
   SWA_NSF_PostRate_MaxArts,   SWS_H_MaxArts,
   SWA_NSF_PostRate_MinPerDay, SWS_H_MinPerDay,
   SWA_NSF_PostRate_MaxPerDay, SWS_H_MaxPerDay,
   NULL, NULL,
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Min Articles: ",
   "Max Articles",
   "Min Per Day",
   "Max Per Day",
   
   "",
   "",
   "",
   "",
   
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

# include "C-NSF-PostRate-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
