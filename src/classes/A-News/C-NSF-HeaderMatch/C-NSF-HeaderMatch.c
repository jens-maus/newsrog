# include "C-NSF-HeaderMatch-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_HeaderMatch

// -- attribute map ----------------------------------------------------------

static STRPTR ModeLab[3];

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_CY(SWA_NSF_HeaderMatch_Mode, SWS_L_MLab, SWAM_UI, SWV_HM_Mode_Or,
           SWV_HM_Mode_And+1, ModeLab),
   
   SWP_NList_Attrs,
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

static HelpMap SW_HELPMAP[] = {
   SWA_NSF_HeaderMatch_Mode, SWS_H_Mode,
   NULL, NULL,
};


static STRPTR Headers[] = {
   SWV_NStrFilter_HFields,
   NULL,
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   SWX_B("Case"),
   SWX_B("Wild"),
   SWX_B("Anchor"),
   SWX_B("Header"),
   SWX_B("Pattern"),

   "Mode: ",
   "",
   "Any",
   "All",

   MUIX_C "Patterns (%ld)",

   "_Add Pattern",
   "_Delete",
   "_Select All",
   "_Edit...",
   "Header: ",

   SWX_I("Unset"),

   "Whether to match if any pattern matches,\n"
      "or only match if all patterns match.",
   "Add a new pattern.",
   "Delete selected patterns.",
   "Select all patterns in the list.",
   "Edit a pattern.",

   "This is a list of patterns to be matched against various\n"
      "header fields of articles.  Only locally cached header\n"
      "fields may be matched against.  " SWX_B("Non") " wild matches are\n"
      "much faster than wild matches (the latter may use any valid\n"
      "AmigaOS pattern).  Non-case sensitive matches are faster\n"
      "if grouped together by header.\n\n"
      "Non-wild patterns use a custom, extremely high performance\n"
      "string searching algorithm that is between 2 and 10 times\n"
      "faster than the standard string searching library.",

   "The contents of this header from the article\n"
   "is matched against the pattern.",

   "An pattern to match against the header data, not\n"
      "including the header keyword, colon, and single trailing\n"
      "space.  Matches against the subject field ignore leading\n"
      SWX_I("Re: ") "strings.  This field is used as either a fixed string\n"
      "or an AmigaOS pattern depending on the " SWX_I("Wild") " setting.\n"
      "Anchored matches must match the whole line exactly, while\n"
      "non-anchored matches may match any substring or sub-\n"
      "pattern in the line.",
   NULL,
};


static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NStrFilter_Header,
   SWA_NStrFilter_Pattern,
   SWA_NStrFilter_Case,
   SWA_NStrFilter_Wild,
   SWA_NStrFilter_Anchor,
   NULL
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static ULONG CreateListmgr(Object *o, SW_IDATA *idata)
{
   setlent(o, SWA_ObjNoChildCopy, TRUE);

   idata->SWA_NList_LV = ListviewObject,
      MUIA_CycleChain,           1,
      MUIA_Listview_Input,       TRUE,
      MUIA_ShortHelp,            GetS(SWS_H_HMList),
      MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
      MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
      MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
         InputListFrame,
         SWA_ObjList_Object,      o,
         MUIA_List_Format,        "COL=0,"
                                  "COL=1,"
                                  "COL=2 BAR,"
                                  "COL=3 W=20 BAR,"
                                  "COL=4 W=50",
         MUIA_List_DragSortable,  TRUE,
         MUIA_List_Title,         TRUE,
         MUIA_List_DisplayHook,   &idata->DspHook,
         MUIA_List_DestructHook,  &idata->DstHook,
      End,
   End;

   if (!idata->SWA_NList_LV) return FAILURE;

   DoMethod(o, OM_ADDMEMBER, idata->SWA_NList_LV);

   return SUCCESS;
}


# include "C-NSF-HeaderMatch-Hooks.h"
# include "S-Lists-Hooks.h"

static ULONG SNLT_Attr[] = { SWA_NStrFilter_Case, SWA_NStrFilter_Wild,
                             SWA_NStrFilter_Anchor, 0 };
static ULONG SNLT_Cols[] = { 0, 1, 2 };

# define SETUPCUST \
            if (CreateListmgr((Object *)retval, idata) != SUCCESS) \
               SWDB_RET(NULL);

# define SW_Methods                  \
  SW_DO_MF(SWM_NSFilter_Begin);      \
  SW_DO_MF(SWM_NSFilter_Match);      \
  SW_DO_MF(SWM_NSFilter_End);        \
  SW_DO_MF(SWM_NSFilter_UI);         \
  SW_DO_MF(SWM_NSFilter_HideUI);     \
  SW_DO_MF(SWM_NSFilter_TrackStats); \
  SW_DO_MF(SWM_NSFilter_Expire);     \
  SW_DO_MF(MUIM_Setup);              \
  SW_DO_MF(MUIM_Cleanup);

# include "S-Lists-Mthd.h"
# include "C-NSF-HeaderMatch-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
