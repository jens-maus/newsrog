# include "C-ListCol-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_ListCol

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_PT(SWA_ListCol_ColSpecs,   SWAM_NoLab, SWAM_SGIN | SWAM_Asgn, NULL),
   SWAM_PT(SWA_ListCol_ColNames,   SWAM_NoLab, SWAM_SGIN | SWAM_Asgn, NULL),
   SWAM_PT(SWA_ListCol_Format,     SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_BO(SWA_ListCol_FixWidth,   SWAM_NoLab, SWAM_SGIN, TRUE),
   SWAM_PT(SWA_ListCol_Hook,       SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_PT(SWA_ListCol_List,       SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_SA(SWA_ListCol_ClipColSep, SWAM_NoLab, SWAM_SGI,  "  ", 32),
   SWAM_BO(SWA_ListCol_ClipTitles, SWAM_NoLab, SWAM_SGI,  TRUE),

   SWAM(SWA_ListCol_ShownCols, offsetof(SW_IDATA, SWA_ListCol_ShownCols),
        SWV_ListCol_MaxCol, SWAM_Struct, SWAM_SGI, NULL, NULL, NULL, NULL),

   SWAM(SWA_ListCol_HiddenCols, offsetof(SW_IDATA, SWA_ListCol_HiddenCols),
        SWV_ListCol_MaxCol, SWAM_Struct, SWAM_SGI, NULL, NULL, NULL, NULL),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Displayed",
   "Hidden",
   "_Reset",
   "_Close",
   "Cli_p",
   "Use Titles: ",
   "Separator: ",

   "This is the list of columns displayed\n"
      "by the list.  Items can be reordered\n"
      "or dragged over into the hidden list.",

   "This is the list of columns not displayed\n"
      "by the list.  Items can be dragged over\n"
      "into the displayed list.  Order is not\n"
      "significant.",

   "Resets the columns to\n"
      "their default order.",

   "Close the popup window.",

   "Copy the selected entries\nto the clipboard.",

   "If set, the title row is also copied.",
   "The column separator string.",

   NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

/*****************************************************************************
 * Method: Init col shown/hidden data                                        *
 *****************************************************************************/

static void InitColData(Object *o, SW_IDATA *idata)
{
   LONG x;

   if (!idata->SWA_ListCol_ColNames) return;
   
   for (x=0; idata->SWA_ListCol_ColNames[x]; x++)
      idata->SWA_ListCol_ShownCols[x] = (BYTE)x;

   idata->SWA_ListCol_ShownCols[x]  =
   idata->SWA_ListCol_HiddenCols[0] = -1;

   idata->Setup = TRUE;
}

# define SETUPCUST \
   idata->SWA_ListCol_ShownCols[0]  = \
   idata->SWA_ListCol_HiddenCols[0] = -1;

# define SW_SETUP_POSTSET InitColData((Object *)retval, idata);

static void RegenAll(SW_IDATA *idata);

# define DISPOSECUST FreeVec(idata->SWA_ListCol_Format);
   
# define SW_Methods                  \
   SW_DO_MF(SWM_ListCol_UI);         \
   SW_DO_MF(SWM_ListCol_BuildFmt);   \
   SW_DO_MF(SWM_ListCol_TrueCol);    \
   SW_DO_MF(SWM_ListCol_Reset);      \
   SW_DO_MF(SWM_ListCol_Popup);      \
   SW_DO_MF(SWM_ListCol_HideCols);   \
   SW_DO_MF(SWM_ListCol_Clip);       \
   SW_DO_MF(SWM_ListCol_ClipBt);     \
   SW_DO_MF(SWM_ListCol_RemNotify);  \
   SW_DO_MF(SWM_ObjList_DragQuery);  \
   SW_DO_MF(SWM_ObjList_DragDrop);   \

# include "C-ListCol-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
