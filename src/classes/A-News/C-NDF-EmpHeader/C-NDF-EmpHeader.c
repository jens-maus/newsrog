# include "C-NDF-EmpHeader-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewVG,  0, 0, 0),

   SWAM_UICTL(SWAM_NewCG2, SWAM_NoLab, 0, 0),
   SWAM_BO(SWA_NDF_EmpHeader_FoldLines, SWS_L_FoldLines, SWAM_UI, FALSE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),

   SWAM_UICTL(SWAM_NewCG2 | SWAM_GrpLab, SWS_L_HField, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_EmpHeader_Field, TRUE, FALSE, FALSE, MPEN_TEXT,
                      SWA_NDF_EmpHeader_Pens, EHPEN_Field, FALSE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   
   SWAM_UICTL(SWAM_NewCG2 | SWAM_GrpLab, SWS_L_HData, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_EmpHeader_Data, FALSE, FALSE, FALSE, MPEN_TEXT,
                      SWA_NDF_EmpHeader_Pens, EHPEN_Data, FALSE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),

   SWAM_FI(SWA_NDFilter_Pri, SWAM_NoLab,    SWAM_SGIN, 10),
   
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Header Field Type Style",
   "Header Data  Type Style",

   "Font: ",
   "Style: ",
   "Color: ",
   "Bold",
   "Underline",
   "Italic",

   "Fold Lines: ",
   
   "The style of type for the field part\n"
      "of the header line (before the :\n"
      "character).  If the font is left\n"
      "blank, the font currently in effect\n"
      "will remain in effect.",

   "The style of type for the data part\n"
      "of the header line (after the :\n"
      "character).  If the font is left\n"
      "blank, the font currently in effect\n"
      "will remain in effect.",

   "If selected, certain header lines such\n"
      "as Newsgroups: and References: will\n"
      "be displayed on multiple lines rather\n"
      "than on a single line.",

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWP_NDF_StyleHelp(SWA_NDF_EmpHeader_Field,  SWS_H_Field),
   SWP_NDF_StyleHelp(SWA_NDF_EmpHeader_Data,   SWS_H_Data),
   SWA_NDF_EmpHeader_FoldLines,  SWS_H_FoldLines,
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# include "C-NDF-EmpHeader-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
