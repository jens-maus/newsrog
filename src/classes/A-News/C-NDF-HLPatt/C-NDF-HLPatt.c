# include "C-NDF-HLPatt-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

# define BEG SWAM_NewCG2|SWAM_EndG

static STRPTR MatchMode[4];
   
SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewVG,  0, 0, 0),

   SWAM_DSS(SWA_NDF_HLPatt_Pattern,    SWS_L_HLPattPat,  SWAM_UI|SWAM_NewCG2, DefPt, MaxPL),
   SWAM_BO (SWA_NDF_HLPatt_Case,       SWS_L_HLPattCase, SWAM_UI,   FALSE),
   SWAM_DSS(SWA_NDF_HLPatt_FastCheck,  SWS_L_HLPattFC,   SWAM_UI,    DefFC, 1024),
   SWAM_CY (SWA_NDF_HLPatt_MatchMode,  SWS_L_HLPattMode, SWAM_UI,
            SWV_NDF_HLPatt_MatchShortest, 2, MatchMode),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   
   SWAM_UICTL(SWAM_NewCG2 | SWAM_GrpLab, SWS_L_HLPattTxt, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_HLPatt, FALSE, FALSE, FALSE, MPEN_SHINE,
                      SWA_NDF_HLPatt_Pens, PEN_HLPatt, TRUE),

   SWAM_UICTL(SWAM_EndG, 0, 0, 0),

   SWP_NDFilter_Attrs(-800),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Text Style for highlighted pattern. ",
   "Text Style",

   "Pattern: ",
   "Case Sensitive: ",
   "Fast Check: ",
   "Match Mode: ",
   "Shortest Match",
   "Longest Match",
   
   "Pattern matching part of a text line\n"
      "to be colorized.  You might, for example,"
      "change the color or font of all references"
      "to your own name.",
   "If selected, the pattern match\n"
      "is case sensitive.",
   "Since checking for regular expressions on every\n"
      "line can be a slow operation on some systems,\n"
      "this string allows the pattern match to be avoided\n"
      "most of the time.  Unless a line contains one of\n"
      "these fixed strings, the regular expression match\n"
      "is bypassed.  This string should be a | separated\n"
      "set of strings, e.g.:\n"
      SWX_I("    " DefFC "\n")
      "If left empty, this feature is ignored.",
   "This controls whether the pattern will match\n"
      "the longest or the shortest possible subset\n"
      "of the line.  For example, if an article line\n"
      "contains:\n\n"
      "   foo test bloot blah\n"
      "and the pattern is " SWX_I("foo*t") ", then the shortest\n"
      "match is " SWX_I("foo t") ", while the longest is\n"
      SWX_I("foo test bloot") ".",

   SWP_NDF_StyleLabStr,

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWP_NDF_StyleHelp(SWA_NDF_HLPatt, SWS_H_HLPatt),
   SWA_NDF_HLPatt_Pattern,   SWS_H_HLPattPat,
   SWA_NDF_HLPatt_Case,      SWS_H_HLPattCase,
   SWA_NDF_HLPatt_FastCheck, SWS_H_HLPattFC,
   SWA_NDF_HLPatt_MatchMode, SWS_H_HLPattMode,
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods                  \
  SW_DO_MF(SWM_NDFilter_ModTxt);     \
  SW_DO_MF(SWM_NDFilter_StartMatch); \
  SW_DO_MF(SWM_NDFilter_EndMatch);   \
  SW_DO_MF(SWM_NDFilter_UI);         \
  SW_DO_MF(SWM_NDFilter_HideUI);     \

# include "C-NDF-HLPatt-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
