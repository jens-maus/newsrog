# include "C-NDF-Spoilers-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NDF_Spoilers_FmtStr,  SWS_L_FmtTxt, SWAM_UI, DefFmt, 256),

   SWP_NDF_StyleAttrs(SWA_NDF_Spoilers, FALSE, FALSE, FALSE, MPEN_TEXT,
                      SWA_NDF_Spoilers, EHPEN_SPText, FALSE),

   SWP_NDFilter_Attrs(990),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Format String: ",
   
   "Font: ",
   "Style: ",
   "Color: ",
   "Bold: ",
   "Underline: ",
   "Italic: ",

   "The text to be displayed inside the button\n"
   "which toggles the spoiler region.  In addition\n"
   "to the normal variables, these may be used:\n\n"
      SWX_B("  ${MaxPage}") " - The maximum number of\n"
      "  spoiler sections in this document.\n\n"
      SWX_B("  ${Page}") " - The ordinal number of the current\n"
      "  spoiler section.\n\n"
      SWX_B("  ${Size}") " - The size in bytes of the current\n"
      "  spoiler section.\n\n"
      SWX_B("  ${Lines}") " - The number of lines in the current\n"
      "  spoiler section.",

   "The text style for text inside a spoiler region.",
   
   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NDF_Spoilers_FmtStr, SWS_H_FmtTxt,
   SWP_NDF_StyleHelp(SWA_NDF_Spoilers,  SWS_H_Style),
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
  SW_DO_MF(SWM_VarList_Search);      \

# define   NDF_AddObj
# include "C-NDF-Spoilers-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
