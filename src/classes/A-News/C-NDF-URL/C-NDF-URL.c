# include "C-NDF-URL-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

# define BEG SWAM_NewCG2|SWAM_EndG
   
SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewVG,  0, 0, 0),

   SWAM_DSS(SWA_NDF_URL_Pattern,    SWS_L_URLPat,  SWAM_UI|SWAM_NewCG2, DefPt, MaxPL),
   SWAM_BO (SWA_NDF_URL_Case,       SWS_L_URLCase, SWAM_UI,   FALSE),
   SWAM_DSS(SWA_NDF_URL_FastCheck,  SWS_L_URLFC,   SWAM_UI,    DefFC, 1024),
   SWAM_DSS(SWA_NDF_URL_Action,     SWS_L_URLAct,  SWAM_UI, "Send-WWW", 256),
   SWAM_BO (SWA_NDF_URL_DClick,     SWS_L_URLDClk, SWAM_UI,    FALSE),
   SWAM_BO (SWA_NDF_URL_PopMenus,   SWS_L_URLPopM, SWAM_UI|SWAM_EndG, TRUE),
   
   SWAM_UICTL(SWAM_NewCG2 | SWAM_GrpLab, SWS_L_URLTxt, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_URL, FALSE, TRUE, FALSE, MPEN_SHINE,
                      SWA_NDF_URL_Pens, PEN_URL, TRUE),

   SWAM_PA(SWA_NDF_URL_SelColor, SWS_L_SelCol, SWA_NDF_URL_Pens,
           PEN_SelURL, MPEN_TEXT, SWAM_UI),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),

   SWP_NDFilter_Attrs(-1001),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Text Style for URLs. ",
   "URL Text Style",

   "URL Pattern: ",
   "Case Sensitive: ",
   "Fast Check: ",
   "Multimedia Action: ",
   "Double Click: ",
   "Use PopMenus: ",
   
   "Pattern matching a URL, and nothing else.\n"
      "A useful pattern is: " SWX_B(DefPt) "  which\n"
      "matches HTTP: or FTP: style URL's, but any\n"
      "pattern may be used to match email addresses,\n"
      "filenames, or other types of data to be sent\n"
      "to other applications.",
   "If selected, the pattern match\n"
      "is case sensitive.",
   "Since checking for regular expressions on every\n"
      "line can be a slow operation on some systems,\n"
      "this string allows the pattern match to be avoided\n"
      "most of the time.  Unless a line contains one of\n"
      "these fixed strings, the regular expression match\n"
      "is bypassed This string should be a | separated\n"
      "list, e.g.:\n"
      SWX_I("    " DefFC "\n")
      "If left empty, this feature is ignored.",
   "An action in the Multimedia Action list.  The type\n"
     "is hardcoded at " SWX_B("application/X-URL") ", but any\n"
     "number of actions on this type may be defined to send\n"
     "URL's to various programs.",
   "If selected, then a doubleclick is needed to\n"
      "trigger the action for this URL.  Otherwise, a\n"
      "singleclick is sufficient.",
   "If selected, a popup menu is created for each\n"
      "URL.  The URL may be copied to the clipboard,\n"
      "or any Multimedia Action defined for the type\n"
      SWX_B("application/X-URL") " may be executed.",

   "Selected Col:",
   "The color for a selected URL.",
   
   SWP_NDF_StyleLabStr,

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWP_NDF_StyleHelp(SWA_NDF_URL, SWS_H_URL),
   SWA_NDF_URL_SelColor,  SWS_H_SelCol,
   SWA_NDF_URL_Pattern,   SWS_H_URLPat,
   SWA_NDF_URL_Case,      SWS_H_URLCase,
   SWA_NDF_URL_FastCheck, SWS_H_URLFC,
   SWA_NDF_URL_Action,    SWS_H_URLAct,
   SWA_NDF_URL_DClick,    SWS_H_URLDClk,
   SWA_NDF_URL_PopMenus,  SWS_H_URLPopM,
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
  SW_DO_MF(SWM_NDFilter_GenAddObj);  \
  SW_DO_MF(SWM_NDFilter_RemAddObj);  \

# include "C-NDF-URL-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
