# include "C-NDF-TextStyles-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IB(SWA_NDF_TextStyles_MaxLen,    SWS_L_MaxLen,    SWAM_UI, 20, 1, 250),
   SWAM_BO(SWA_NDF_TextStyles_Bold,      SWS_L_Bold,      SWAM_UI, TRUE),
   SWAM_BO(SWA_NDF_TextStyles_Italic,    SWS_L_Italic,    SWAM_UI, TRUE),
   SWAM_BO(SWA_NDF_TextStyles_Underline, SWS_L_Underline, SWAM_UI, TRUE),
   SWAM_BO(SWA_NDF_TextStyles_WordBoundary, SWS_L_WordB,  SWAM_UI, TRUE),
   
   SWAM_FI(SWA_NDFilter_Pri,   SWAM_NoLab,    SWAM_SGIN, -200),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Max Len: ",
   "Bold: ",
   "Italic: ",
   "Underline: ",
   "Word Boundries: " ,

   "Strings of the form *text* are changed\n"
      "to bold " SWX_B("text") ", _text_ to underlined\n"
      SWX_U("text") ", and so on.\n\n"
      "This setting controls the maximum number\n"
      "of characters between the emphasis marks.",

   "If checked, text between asterisk (*)\n"
      "characters will be changed to bold.",

   "If checked, text between slash (/)\n"
      "characters will be changed to italic.",

   "If checked, text between underscore (_)\n"
      "characters will be changed to underlined.",

   "If checked, then emphasized regions of text\n"
      "are constrained to start and end on word\n"
      "boundaries.  This can prevent emphasizing\n"
      "text in places where it would be inappropriate,\n"
      "such as when forward slash characters are\n"
      "used in pathnames.",

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NDF_TextStyles_MaxLen,       SWS_H_MaxLen,
   SWA_NDF_TextStyles_Bold,         SWS_H_Bold,
   SWA_NDF_TextStyles_Italic,       SWS_H_Italic,
   SWA_NDF_TextStyles_Underline,    SWS_H_Underline,
   SWA_NDF_TextStyles_WordBoundary, SWS_H_WordB,
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# include "C-NDF-TextStyles-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
