# include "C-NDF-TrimSpace-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IB(SWA_NDF_TrimSpace_MaxLines,   SWS_L_MaxLines,  SWAM_UI, 2, 1, 100),
   SWAM_FI(SWA_NDFilter_HasTxtMod, SWAM_NoLab, SWAM_Get, TRUE),

   SWAM_FI(SWA_NDFilter_Pri,   SWAM_NoLab,    SWAM_SGIN, -300),
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Max Lines: ",

   "The maximum number of contiguous blank lines\n"
      "allowed.  If more are present, they will be\n"
      "extra blank lines will be removed until only\n"
      "this many remain.",
   
   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NDF_TrimSpace_MaxLines,   SWS_H_MaxLines,
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods                    \
  SW_DO_MF(SWM_NDFilter_ModTxt);       \
  SW_DO_M(SWM_NDFilter_ModTxtNoObjs, SWM_NDFilter_ModTxt_F); \
  SW_DO_MF(SWM_NDFilter_StartMatch);   \
  SW_DO_MF(SWM_NDFilter_EndMatch);     \
  SW_DO_MF(SWM_NDFilter_UI);           \
  SW_DO_MF(SWM_NDFilter_HideUI);

# include "C-NDF-TrimSpace-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
