# include "C-NDF-HideHeader-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NDF_HideHeader_Pattern,  SWS_L_Pattern, SWAM_UI,  NULL, MaxPL),
   SWAM_IN(SWA_NDF_HideHeader_SaveVer,  SWAM_NoLab,    SWAM_SGI, 0),
   SWAM_FI(SWA_NDFilter_HasTxtMod,      SWAM_NoLab,    SWAM_Get, TRUE),

   SWP_NDFilter_Attrs(0),
   SWP_NList_Attrs,
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Pattern",

   "All headers " SWX_B("except") " those that match\n"
      "this pattern will be hidden.",

   MUIX_B MUIX_U "Pattern",
   "Header Patterns (%ld)",

   "_Add",
   "_Del",
   "_Select All",

   SWX_I("Unset"),

   "Add a new pattern",
   "Delete selected patterns",
   "Select all patterns in the list.",

   "This is a list of AmigaOS patterns matching\n"
   "article headers, including the colon (for\n"
   "example, 'Subject:').  Only headers matching\n"
   "one of these patterns will be displayed.  The\n"
   "headers will be displayed in the same order\n"
   "that they are matched in this list, but if\n"
   "a header is matched by more than one entry,\n"
   "it will only be displayed once.",

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NDF_HideHeader_Pattern, SWS_H_Pattern,
   NULL, NULL,
};

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NDF_HideHeader_Pat,
   NULL
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static ULONG CreateListmgr(Object *o, SW_IDATA *idata)
{
   setlent(o, SWA_ObjNoChildCopy, TRUE);

      idata->SWA_NList_LV = ListviewObject,
      MUIA_CycleChain,           1,
      MUIA_Listview_Input,       TRUE,
      MUIA_ShortHelp,            GetS(SWS_H_List),
      MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
      MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
      MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
         InputListFrame,
         SWA_ObjList_Object,      o,
         MUIA_List_Format,        "COL=0",
         MUIA_List_DragSortable,  TRUE,
         MUIA_List_Title,         TRUE,
         MUIA_List_DisplayHook,   &idata->DspHook,
         MUIA_List_DestructHook,  &idata->DstHook,
      End,
   End;

   if (!idata->SWA_NList_LV) return FAILURE;

   DoMethod(o, OM_ADDMEMBER, idata->SWA_NList_LV);

   DoMethod(o, SWM_NList_Add, NULL, DefPt);

   return SUCCESS;
}

# include "C-NDF-HideHeader-Hooks.h"
# include "S-Lists-Hooks.h"

static void FreePPats(SW_IDATA *idata)
{
   LONG x;

   if (idata->PPat) {
      for (x=0; x<idata->PatCount; x++) {
         NDF_FV(idata->PPat[x]);
         idata->PPat[x] = NULL;
      }

      NDF_FV(idata->PPat);
      idata->PPat = NULL;
   }

   idata->Parsed = FALSE;
}

# define DISPOSECUST FreePPats(idata)

# define SETUPCUST \
            if (CreateListmgr((Object *)retval, idata) != SUCCESS) \
               SWDB_RET(NULL);

# define SW_Methods                                          \
  SW_DO_MF(SWM_NDFilter_ModTxt);                             \
  SW_DO_M(SWM_NDFilter_ModTxtNoObjs, SWM_NDFilter_ModTxt_F); \
  SW_DO_MF(SWM_NDFilter_StartMatch);                         \
  SW_DO_MF(SWM_NDFilter_EndMatch);                           \
  SW_DO_MF(SWM_NDFilter_UI);                                 \
  SW_DO_MF(SWM_NDFilter_HideUI);

# include "S-Lists-Mthd.h"
# include "C-NDF-HideHeader-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
