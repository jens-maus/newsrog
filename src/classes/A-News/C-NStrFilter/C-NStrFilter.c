# include "C-NStrFilter-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NStrFilter



// -- attribute map ----------------------------------------------------------

STRPTR HLab[20];
# define UIA (SWAM_UI | SWAM_Notify)
   
SWAttrMapT SW_ATTRMAP[] = {
   SWAM_CY(SWA_NStrFilter_Header,  SWS_L_Header,  UIA,
           SWS_LH_Subject-SWS_LH_Approved, SWS_LH_X_NewsRd-SWS_LH_Approved+1,
           HLab),
   
   SWAM_SA(SWA_NStrFilter_Pattern, SWS_L_Pattern, UIA, NULL, 16384),
   SWAM_BO(SWA_NStrFilter_Case,    SWS_L_Case,    UIA, TRUE),
   SWAM_BO(SWA_NStrFilter_Wild,    SWS_L_Wild,    UIA, FALSE),
   SWAM_BO(SWA_NStrFilter_Anchor,  SWS_L_Anchor,  UIA, FALSE),
   SWAM_IN(SWA_NStrFilter_Total,   SWAM_NoLab,    SWAM_SGIN, 0),
   SWAM_IN(SWA_NStrFilter_Found,   SWAM_NoLab,    SWAM_SGIN, 0),
   
   SWP_Root_Attrs,
   SWAM_DONE
};


static HelpMap SW_HELPMAP[] = {
   SWA_NStrFilter_Header,  SWS_H_Header,
   SWA_NStrFilter_Pattern, SWS_H_Pattern,
   SWA_NStrFilter_Case,    SWS_H_Case,
   SWA_NStrFilter_Wild,    SWS_H_Wild,
   SWA_NStrFilter_Anchor,  SWS_H_Anchor,
   NULL, NULL,
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[SWS_LASTSTR+1] = {
   "Header: ",
   SWV_NStrFilter_HFields,

   "Pattern: ",
   "Case: ",
   "Wild: ",
   "Anchor: ",

   "The header field to match.  Some headers may not be\n"
      "locally cached, and cannot be matched against.",
   
   "The pattern to match in the header data.",

   "Whether the pattern is case sensitive.  Case\n"
      "sensitive searches are faster than non-case.",

   "Whether the pattern should be matched using\n"
      "wildcards.  Fixed string matches are much faster\n"
      "than wildcard matches.",

   "Whether the pattern should be anchored at the\n"
      "beginning and end of the header.  Otherwise,\n"
      "the pattern may be appear anywhere in the\n"
      "header line.",
   
   NULL,
} ;

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods                  \
  SW_DO_MF(SWM_NSFilter_UI);         \
  SW_DO_MF(SWM_NSFilter_HideUI);     \
  SW_DO_MF(SWM_NSFilter_TrackStats); \

# include "C-NStrFilter-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
