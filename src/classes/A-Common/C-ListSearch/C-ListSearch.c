# include "C-ListSearch-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_ListSearch

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_BO(SWA_ListSearch_Dynamic,      SWAM_NoLab, SWAM_SGIN, FALSE),
   SWAM_BO(SWA_ListSearch_Case,         SWAM_NoLab, SWAM_SGIN, FALSE),
   SWAM_BO(SWA_ListSearch_SubLine,      SWAM_NoLab, SWAM_SGIN,  TRUE),
   SWAM_BO(SWA_ListSearch_First,        SWAM_NoLab, SWAM_SGIN,  TRUE),
   SWAM_PT(SWA_ListSearch_StrLabel,     SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_PT(SWA_ListSearch_Title,        SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_PT(SWA_ListSearch_List,         SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_PT(SWA_ListSearch_SearchObj,    SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_IN(SWA_ListSearch_SearchMethod, SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_IN(SWA_ListSearch_CmpMethod,    SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_IN(SWA_ListSearch_WindowID,     SWAM_NoLab, SWAM_SGIN,     0),
   SWAM_PT(SWA_ListSearch_ItemName,     SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_PT(SWA_ListSearch_ItemNamePl,   SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_PT(SWA_ListSearch_UI,           SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_IN(SWA_ListSearch_PrevSrchPos,  SWAM_NoLab, SWAM_SGIN,     0),
   SWAM_IN(SWA_ListSearch_ObjAttr,      SWAM_NoLab, SWAM_SGIN,     0),
   SWAM_IN(SWA_ListSearch_Flags,        SWAM_NoLab, SWAM_SGIN,     0),
   SWAM_IN(SWA_ListSearch_InitSrchPos,  SWAM_NoLab, SWAM_SGIN,     0),
   SWAM_SA(SWA_ListSearch_PrevPat,      SWAM_NoLab, SWAM_SGIN,  NULL, 4096),
   SWAM_PT(SWA_ListSearch_StatusTxt,    SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_IN(SWA_ListSearch_UpdSrchCnt,   SWAM_NoLab, SWAM_SGIN,     0),
   
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Search",
   "Pattern:",
   "_Search",
   "SubString:",
   "Dynamic:",
   "Case:",
   "First:",
   "Se_lect All",
   "_Unselect All",
   "T_oggle All",
   "From _Top",
   
   "Item",
   "Items",
   "Incomplete Pattern",
   "Searching...",
   "Searching (%ld)...",

   "Search for a given pattern:\n"
      "  E.g.: " MUIX_I "*amiga*" MUIX_N "\n"
      "A faster search is performed if the pattern\n"
      "does not contain wildcard characters.",
   "Select all entries.",
   "Unselect all entries.",
   "Toggle selections.",
   "Restart the search from top.",

   "If set, the search will match a substring\n"
      "at any location in a line.  Otherwise, the\n"
      "match is anchored to the beginning and end\n"
      "of the line.",

   "Dynamic flag: If set, the search will update\n"
      "the display with every keystroke.  This is\n"
      "faster if the " MUIX_B " First" MUIX_N "option is turned\n"
       "on, but it works either way.",

   "If selected, the search is case sensitive.",

   "First match only: If set, the search will stop\n"
      "at the first match.  Otherwise, all matches\n"
      "in the list are selected.",

   NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define DISPOSECUST DoMethod(o, SWM_ListSearch_CloseWin);

# define SW_NOROOTMETHODS
# define SW_Methods                  \
  SW_DO_MF(SWM_ListSearch_Search);   \
  SW_DO_MF(SWM_ListSearch_FromTop);  \
  SW_DO_MF(SWM_ListSearch_OpenWin);  \
  SW_DO_MF(SWM_ListSearch_CloseWin); \
  SW_DO_MF(SWM_ListSearch_ListChg);  \

# include "C-ListSearch-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
