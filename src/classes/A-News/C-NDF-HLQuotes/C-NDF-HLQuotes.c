# include "C-NDF-HLQuotes-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_HLQuotes

// -- attribute map ----------------------------------------------------------

# define BG SWAM_NewCG2
# define EG SWAM_EndG

STRPTR RegLab[8];
STRPTR UIMLab[4];
   
SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewRG,  SWS_L_L0, 6, RegLab),

   SWAM_SA(SWA_NDF_HLQuotes_Chars,     SWS_L_Chars, SWAM_UI|BG, ">|:", 256),
   SWAM_IB(SWA_NDF_HLQuotes_PrefixLen, SWS_L_PLen,  SWAM_UI, 8, 1, 40),
   SWAM_IB(SWA_NDF_HLQuotes_MaxLines,  SWS_L_Lines, SWAM_UI, 5, 0, 50),
   SWAM_IB(SWA_NDF_HLQuotes_MaxLevel,  SWS_L_Level, SWAM_UI, 3, 1, 20),
   SWAM_CY(SWA_NDF_HLQuotes_UIMode, SWS_L_UIM,  SWAM_UI,
           SWV_NDF_HLQuotes_UIM_Scroll, 3, UIMLab),
   SWAM_BO(SWA_NDF_HLQuotes_PrefixHeur,SWS_L_PreH,  SWAM_UI|EG, TRUE),
   
   SWAM_UICTL(SWAM_NewCG2, 0, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_HLQuotes_1, FALSE, FALSE, FALSE, MPEN_SHINE,
                      SWA_NDF_HLQuotes_Pens, PEN_SQ1, TRUE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   SWAM_UICTL(SWAM_NewCG2, 0, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_HLQuotes_2, FALSE, FALSE, FALSE, MPEN_MARK,
                      SWA_NDF_HLQuotes_Pens, PEN_SQ2, TRUE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   SWAM_UICTL(SWAM_NewCG2, 0, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_HLQuotes_3, FALSE, FALSE, FALSE, MPEN_FILL,
                      SWA_NDF_HLQuotes_Pens, PEN_SQ3, TRUE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   SWAM_UICTL(SWAM_NewCG2, 0, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_HLQuotes_4, FALSE, FALSE, FALSE, MPEN_HALFSHADOW,
                      SWA_NDF_HLQuotes_Pens, PEN_SQ4, TRUE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   SWAM_UICTL(SWAM_NewCG2, 0, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_HLQuotes_5, FALSE, FALSE, FALSE, MPEN_TEXT,
                      SWA_NDF_HLQuotes_Pens, PEN_SQ5, FALSE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),

   SWP_NDFilter_Attrs(0),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Separator Chars: ",
   "Prefix Len: ",
   "Max Lines: ",
   "Max Level: ",
   "UI Mode: ",
      "Show Only Trimed Quote",
      "Toggle Full/Partial",
      "Scroll Quoted Region",

   "Options",
   "1",
   "2",
   "3",
   "4",
   "5",

   "Prefix Heuristics:" ,

   SWP_NDF_StyleLabStr,

   "Characters which separate quotation levels.\n"
      "For example, the line:\n"
      SWX_I("  Bob>Joe> This is a test\n")
      "Has a quote level of two if the '" SWX_B(">") "' character\n"
      "is in this set.",

   "The maximum number of characters in a single\n"
      "quote prefix.  For example, if this value is\n"
      "set to four (4), then the line:\n"
      SWX_I("  Bob>Joeseph> This is a test\n")
      "has only one quote level.",

   "The maximum number of lines to display from a\n"
      "single quote block.  Depending on the other\n"
      "settings, the full quote can be toggled on or\n"
       "scrolled through.",

   "The maximum quote level to display.  Lines\n"
      "quoted more deeply than this are hidden.  For\n"
      "example, if this value is 2, the 2nd and 3rd\n"
      "lines below would be hidden:\n"
      SWX_I(">> Hello\n")
      SWX_I(">>> These two lines\n")
      SWX_I(">>> are hidden\n")
      SWX_I("> But not this one."),

   "There are several available quoting modes:\n\n"
      SWX_B("Toggle Full/Partial") "- Allows toggling between\n"
         "   full and trimmed quotation blocks.\n\n"
      SWX_B("Show Only Trimmed Quote") "- Always shows\n"
         "   trimmed quotation blocks.\n\n"
      SWX_B("Scroll Quoted Region") "- Shows a small scrolling\n"
         "   view of the larger quotation block.  The\n"
         "   size of the scroller is determined from the\n"
         "   settings for " SWX_B("Max Lines") ".",

   "The text style for this particular level of\n"
      "quotation.  Unquoted text is displayed in\n"
      "the default text style.",

   "If enabled, the filter will use some heuristics\n"
      "to attempt to determine when a line prefix is\n"
      "intentional, and when it is accidental.  For\n"
      "example, the prefix " SWX_B("so:") " might occur as part of\n"
      "english text, and not be intended as a quote\n"
      "prefix.  Or it might occur as part of text\n"
      "quoting someone named Sam Ospry.  This heuristic\n"
      "attempts to make the correct determination.",

   NULL,
};


HelpMap SW_HELPMAP[] = {
   SWA_NDF_HLQuotes_Chars,      SWS_H_Chars,
   SWA_NDF_HLQuotes_PrefixLen,  SWS_H_PLen,
   SWA_NDF_HLQuotes_MaxLines,   SWS_H_Lines,
   SWA_NDF_HLQuotes_MaxLevel,   SWS_H_Level,
   SWA_NDF_HLQuotes_UIMode,     SWS_H_UIMode,
   SWA_NDF_HLQuotes_PrefixHeur, SWS_H_PreH,

   SWP_NDF_StyleHelp(SWA_NDF_HLQuotes_1,  SWS_H_Font),
   SWP_NDF_StyleHelp(SWA_NDF_HLQuotes_2,  SWS_H_Font),
   SWP_NDF_StyleHelp(SWA_NDF_HLQuotes_3,  SWS_H_Font),
   SWP_NDF_StyleHelp(SWA_NDF_HLQuotes_4,  SWS_H_Font),
   SWP_NDF_StyleHelp(SWA_NDF_HLQuotes_5,  SWS_H_Font),

   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define  NDF_AddObj
# include "C-NDF-HLQuotes-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
