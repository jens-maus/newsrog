# include "C-NDF-Rewrap-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NDF_ReWrap

// -- attribute map ----------------------------------------------------------

# define BG SWAM_NewCG2
# define EG SWAM_EndG

STRPTR RegLab[4];
   
SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewRG,  SWS_L_L0, 2, RegLab),
   
   SWAM_SA(SWA_NDF_Rewrap_QuoteChars, SWS_L_QChars, SWAM_UI|BG, ">:|", 256),
   SWAM_IB(SWA_NDF_Rewrap_PrefixLen,  SWS_L_PLen,   SWAM_UI, 4, 1, 16),
   SWAM_IB(SWA_NDF_Rewrap_WidthPct,   SWS_L_WPct,   SWAM_UI, 75, MinWP, MaxWP),
   SWAM_SA(SWA_NDF_Rewrap_BreakChars, SWS_L_BChars, SWAM_UI, " ", 256),
   SWAM_IB(SWA_NDF_Rewrap_MinWrapLen, SWS_L_MRLen,  SWAM_UI, 32, 1, 200),
   SWAM_BO(SWA_NDF_Rewrap_BadWrap,    SWS_L_BadW,   SWAM_UI, TRUE),
   SWAM_IB(SWA_NDF_Rewrap_BadWrapMax, SWAM_NoLab,   SWAM_SGI, 20, 1, 200),
   SWAM_BO(SWA_NDF_Rewrap_PrefixHeur, SWS_L_PreH,   SWAM_UI, TRUE),
   SWAM_TX(SWS_L_NoFl, NULL),
   SWAM_BO(SWA_NDF_Rewrap_UseNoFlow,  SWAM_NoLab,   SWAM_UI|SWAM_NewHG, TRUE),
   SWAM_SA(SWA_NDF_Rewrap_NoFlow,     SWAM_NoLab,   SWAM_UI|SWAM_EndG, DefNoFl, 1024),
   SWAM_IB(SWA_NDF_Rewrap_Cols,       SWS_L_Cols,   SWAM_UI|EG, 70, 20, 256),
   SWAM_FI(SWA_NDFilter_HasTxtMod,    SWAM_NoLab,   SWAM_Get, TRUE),

   SWAM_BO(SWA_NDF_Rewrap_Tbl_Use,    SWS_L_UseTbl, SWAM_UI|BG, FALSE),
   SWAM_IB(SWA_NDF_Rewrap_Tbl_MinRows,SWS_L_MinTR,  SWAM_UI, 3, 2, 10),
   SWAM_IB(SWA_NDF_Rewrap_Tbl_MinCols,SWS_L_MinTC,  SWAM_UI, 3, 2, 10),
   SWAM_IB(SWA_NDF_Rewrap_Tbl_MinTblProb,SWS_L_MinTP,  SWAM_UI, 50, 10, 95),
   SWAM_IB(SWA_NDF_Rewrap_Tbl_MaxILines,SWS_L_MaxIL, SWAM_UI, 2, 0, 50),
   SWAM_PF(SWA_NDF_Rewrap_Tbl_Font,   SWS_L_TFont,  SWAM_UI|EG,
           DefTblFont, MAXFONTNAME),
   
   SWP_NDFilter_Attrs(-5000),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Quote Chars: ",
   "Prefix Len: ",
   "Wrap Width %: ",
   "BadWrap Fix: ",
   "Break Chars: ",
   "Min Wrap Len: ",
   "NoFlow Pattern: ",
   "Prefix Heuristics: ",
   "Wrap Column: ",
   "Table Heuristics: ",
   "Min Table Rows: ",
   "Min Table Columns: ",
   "Min Table Probability: ",
   "Max Intro Lines: ",
   "Table Font: ",

   "Text",
   "Tables",

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

   "The % width of the display area that the article\n"
      "text should be wrapped to.\n",

   "If set, an attempt is made to detect and fix\n"
      "bad wrapping by other software.  For example,\n"
      "some text is posted like this:\n\n"
         SWX_I("> > This is a line of quoted text, but\n"
               "this\n"
               "> > word is not wrapped correctly, and\n"
               "neither\n"
               "> > is this one.\n\n")
      "Setting this option will try to find and fix\n"
         "these cases.",

   "These are characters where it is OK to perform\n"
      "line wrapping.  The default is a single space,\n"
      "but other characters such as a hyphen may be\n"
      "included in this set.",

   "The minimum line length that is a candidate for\n"
      "rewrap.  Lines shorter than this will cause a\n"
      "line break.",

   "This pattern can be set to match lines which should\n"
      "not be wrapped.  For example, patterns to match\n"
      "various types of item lists are useful here.",

   "If enabled, the filter will use some heuristics\n"
      "to attempt to determine when a line prefix is\n"
      "intentional, and when it is accidental.  For\n"
      "example, the prefix " SWX_B("so:") " might occur as part of\n"
      "english text, and not be intended as a quote\n"
      "prefix.  Or it might occur as part of text\n"
      "quoting someone named Sam Ospry.  This heuristic\n"
      "attempts to make the correct determination.",

   "When used as a filter for saving, printing, or\n"
      "replying, the text will be wrapped at this\n"
      "column position instead of wrapping to a\n"
      "given percentage of the display width.",

   "If enabled, the filter will search for text lines\n"
   "that look like tabular data, and display them\n"
   "without rewrapping, using a fixed width font.",

   "The minimum number of rows that must be present\n"
   "for tabular data to be detected as a table.",
         
   "The minimum number of columns that must be present\n"
   "for tabular data to be detected as a table.",
         
   "The table heuristic generates a probability that any\n"
   "given line of text is a table.  This is the minimum\n"
   "probability that's needed before the text will be\n"
   "displayed as a table.  Values of 50-60 work well.",

   "If the table is not preceded immediately by a blank\n"
   "line, then there may be no more than this number of\n"
   "lines immediately before the table data.",
      
   "The font used to display tabular data.  Works best\n"
   "if set to a fixed width (non-proportional) font.",
         
   NULL,
};


HelpMap SW_HELPMAP[] = {
   SWA_NDF_Rewrap_QuoteChars,     SWS_H_QChars,
   SWA_NDF_Rewrap_BreakChars,     SWS_H_BChars,
   SWA_NDF_Rewrap_PrefixLen,      SWS_H_PLen,
   SWA_NDF_Rewrap_WidthPct,       SWS_H_WPct,
   SWA_NDF_Rewrap_BadWrap,        SWS_H_BadW,
   SWA_NDF_Rewrap_NoFlow,         SWS_H_NoFl,
   SWA_NDF_Rewrap_UseNoFlow,      SWS_H_NoFl,
   SWA_NDF_Rewrap_PrefixHeur,     SWS_H_PreH,
   SWA_NDF_Rewrap_Cols,           SWS_H_Cols,
   SWA_NDF_Rewrap_Tbl_Use,        SWS_H_UseTbl,
   SWA_NDF_Rewrap_Tbl_MinRows,    SWS_H_MinTR,
   SWA_NDF_Rewrap_Tbl_MinCols,    SWS_H_MinTC,
   SWA_NDF_Rewrap_Tbl_MinTblProb, SWS_H_MinTP,
   SWA_NDF_Rewrap_Tbl_MaxILines,  SWS_H_MaxIL,
   SWA_NDF_Rewrap_Tbl_Font,       SWS_H_TFont,

   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

Library *DiskfontBase = NULL;

# define SW_USERSETUP if (!(DiskfontBase = OpenLibrary("diskfont.library", 37))) \
                         return -1;

# define SW_USERCLEANUP CloseLibrary(DiskfontBase);

# define SW_Methods                     \
   SW_DO_MF(SWM_NDFilter_ModTxt);       \
   SW_DO_MF(SWM_NDFilter_ModTxtNoObjs); \
   SW_DO_MF(SWM_NDFilter_StartMatch);   \
   SW_DO_MF(SWM_NDFilter_EndMatch);     \
   SW_DO_MF(SWM_NDFilter_UI);           \
   SW_DO_MF(SWM_NDFilter_HideUI);       \
   SW_DO_MF(SWM_NDF_Rewrap_RewrapTx);   \


# include "C-NDF-Rewrap-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
