# include "C-NDF-ROT13-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_TX(SWS_H_ROT13, 0),
   SWAM_BO (SWA_NDF_ROT13_UNUSED2, SWAM_NoLab, SWAM_SGI|SWAM_NewHG|SWAM_NoFrame, TRUE),
   SWAM_DSS(SWA_NDF_ROT13_UNUSED,  SWAM_NoLab, SWAM_SGI|SWAM_EndG, "", 256),
   SWAM_IB (SWA_NDF_ROT13_UNUSED3, SWAM_NoLab, SWAM_SGI, 5, 1, 100),
   SWAM_FI(SWA_NDFilter_HasTxtMod, SWAM_NoLab, SWAM_Get, TRUE),

   SWAM_FI(SWA_NDFilter_Pri,   SWAM_NoLab,    SWAM_SGIN, -10000),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "This filter performs an ROT13 operation on the displayed "
      "article.  There are no options.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SWF_FilterHideUI
# define SWF_FilterUI                                \
  SWDB_RET((FloattextObject,                         \
            TextFrame,                               \
            MUIA_Background,     MUII_TextBack,      \
            MUIA_Floattext_Text, GetS(SWS_H_ROT13),  \
            End));

# define SW_Methods                    \
  SW_DO_MF(SWM_NDFilter_ModTxt);       \
  SW_DO_M(SWM_NDFilter_ModTxtNoObjs, SWM_NDFilter_ModTxt_F); \
  SW_DO_MF(SWM_NDFilter_StartMatch);   \
  SW_DO_MF(SWM_NDFilter_EndMatch);     \
  SW_DO_MF(SWM_NDFilter_UI);           \
  SW_DO_MF(SWM_NDFilter_HideUI);

# include "C-NDF-ROT13-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
