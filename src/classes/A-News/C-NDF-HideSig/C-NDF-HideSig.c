# include "C-NDF-HideSig-I.h"
# include "S-NDSetupCust.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewVG,  0, 0, 0),

   SWAM_SA(SWA_NDF_HideSig_Separator,  SWS_L_Separator, SWAM_UI|SWAM_NewCG2, DefPt, MaxPL),
   SWAM_SA(SWA_NDF_HideSig_Marker,     SWS_L_Marker,    SWAM_UI,   "--",  256),
   SWAM_IB(SWA_NDF_HideSig_MaxLineLen, SWS_L_MaxLL,     SWAM_UI,     4, 1, 80),
   SWAM_IB(SWA_NDF_HideSig_MaxSigLines,SWS_L_MaxSL,     SWAM_UI,     8, 1, 50),
   SWAM_BO(SWA_NDF_HideSig_StartMode,  SWS_L_StartMode, SWAM_UI|SWAM_EndG, FALSE),

   SWAM_UICTL(SWAM_NewCG2 | SWAM_GrpLab, SWS_L_Hidden, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_HideSig_Hidden, FALSE, FALSE, FALSE, MPEN_TEXT,
                      SWA_NDF_HideSig_Pens, EHPEN_Hidden, FALSE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),

   SWAM_UICTL(SWAM_NewCG2 | SWAM_GrpLab, SWS_L_Shown, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_HideSig_Shown, FALSE, FALSE, FALSE, MPEN_TEXT,
                      SWA_NDF_HideSig_Pens, EHPEN_Shown, FALSE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),

   SWAM_FI(SWA_NDFilter_HasTxtMod,      SWAM_NoLab,    SWAM_Get, TRUE),

   SWP_NDFilter_Attrs(1000),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Separator: ",
   "Marker: ",
   "Show Initially: ",
   "Max Line Len: ",
   "Max Sig Lines: ",

   "Marker Text Style",
   "Signature Text Style",
   
   "Font: ",
   "Style: ",
   "Color: ",
   "Bold: ",
   "Underline: ",
   "Italic: ",

   "A pattern matching the signature separator line.\n"
      "Everything from this line to the end of the\n"
      "article is replaced by the marker text.",

   "Text to substitute for\nthe deleted signature.",

   "If set, the signature will be displayed initially.\n"
      "Otherwise, it is hidden and the gadget in the\n"
      "article view must be clicked to show it.",

   "The maximum line length that will be scanned for\n"
      "a signature marker.  By not scanning lines longer\n"
      "than this, the pattern matching overhead can be\n"
      "avoided much of the time.",

   "The font style for the marker.",
   "The font style for the signature text, when shown.",

   "If the text after a signature marker is longer than\n"
   "this many lines, it is assumed that it is not part\n"
   "of a signature block even if the marker looks like\n"
   "a signature marker.  This can be used to prevent\n"
   "hiding text when people use signature line markers\n"
   "to separate quoted text from new text.",
   
   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NDF_HideSig_Separator,   SWS_H_Separator,
   SWA_NDF_HideSig_Marker,      SWS_H_Marker,
   SWA_NDF_HideSig_StartMode,   SWS_H_StartMode,
   SWA_NDF_HideSig_MaxLineLen,  SWS_H_MaxLL,
   SWA_NDF_HideSig_MaxSigLines, SWS_H_MaxSL,
   SWP_NDF_StyleHelp(SWA_NDF_HideSig_Shown,  SWS_H_Shown),
   SWP_NDF_StyleHelp(SWA_NDF_HideSig_Hidden, SWS_H_Hidden),
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods                    \
  SW_DO_MF(SWM_NDFilter_ModTxt);       \
  SW_DO_MF(SWM_NDFilter_ModTxtNoObjs); \
  SW_DO_MF(SWM_NDFilter_StartMatch);   \
  SW_DO_MF(SWM_NDFilter_EndMatch);     \
  SW_DO_MF(SWM_NDFilter_UI);           \
  SW_DO_MF(SWM_NDFilter_HideUI);       \
  SW_DO_MF(SWM_NDFilter_GenAddObj);    \
  SW_DO_MF(SWM_NDFilter_RemAddObj)     \

# define   NDF_AddObj
# include "C-NDF-HideSig-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
