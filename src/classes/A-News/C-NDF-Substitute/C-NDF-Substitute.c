# include "C-NDF-Substitute-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

# define BEG SWAM_NewCG2|SWAM_EndG

SWAttrMapT SW_ATTRMAP[] = {
   SWP_NDFilter_Attrs(-800),
   SWP_NList_Attrs,
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_U MUIX_B "Case",
   MUIX_U MUIX_B "Wild",
   MUIX_U MUIX_B "Longest",
   MUIX_U MUIX_B "Pattern",
   MUIX_U MUIX_B "Replacement",
   MUIX_U MUIX_B "Image",
   MUIX_U MUIX_B "Mask",

   MUIX_C "Replacement Patterns (%ld)",
   
   "_Add Pattern",
   "_Delete",
   "_Select All",

   SWX_I("Unset"),

   "Replacement: ",
   "Image: ",
   "Mask: ",
   
   "Add a new pattern.",
   "Delete selected patterns.",
   "Select all patterns in the list.",

   "This is a list of strings that may be replaced\n"
      "with other strings or with small graphics when\n"
      "an article is displayed.  If the " SWX_B("Case") " option is\n"
      "selected, the comparison is case sensitive.\n"
      "Otherwise, it is case insensitive.  The " SWX_B("Wild") " option\n"
      "searches use AmigaOS wildcards, and the\n"
       SWX_B("Longest") " option will replace the longest matching\n"
      "substring instead of the shortest.",

   "This pattern is searched for in the article\n"
      "text and replaced with the replacement text or\n"
      "graphics defined below.",
         
   "If non-empty, this string is used to replace the\n"
      "pattern above before displaying an article text.\n"
      "This string may use the variable " SWX_B("${Img}") " to imbed\n"
      "the image defined below in the replacement text.\n"
      "If this string is empty but an image is defined\n"
      "below, then the image is used without the need to\n"
      "use the " SWX_B("${Img}") " variable here.",

   "If non-empty, this is a filename containing an\n"
      "image that will be used to replace the pattern\n"
      "given above.  It may be used with the " SWX_B("${Img}") "\n"
      "variable in the replacement string above.  For\n"
      "example, you might replace an ASCII smiley " SWX_B(":-)") "\n"
      "with an actual graphic of a smiley.",

   "If non-empty, this filename is used as a mask\n"
      "when rendering the image defined above.  It must\n"
      "be a one-plane IFF file of the same dimensions\n"
      "as the image file.  For every bit which is set\n"
      "in the mask, the image above will be copied, and\n"
      "for every bit that is a zero, the background will\n"
      "show through.",

   NULL,
};

HelpMap SW_HELPMAP[] = {
   NULL, NULL,
};

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NDF_Substitute_Case,
   SWA_NDF_Substitute_Wild,
   SWA_NDF_Substitute_Longest,
   SWA_NDF_Substitute_Pattern,
   SWA_NDF_Substitute_Replace,
   SWA_NDF_Substitute_Image,
   SWA_NDF_Substitute_Mask,
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
      MUIA_ShortHelp,            GetS(SWS_H_List),
      MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
      MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
      MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
         InputListFrame,
         SWA_ObjList_Object,      o,
         MUIA_List_Format,        "COL=0,"
                                  "COL=1,"
                                  "COL=2 BAR,"
                                  "COL=3 W=10 BAR,"
                                  "COL=4 W=25 BAR,"
                                  "COL=5 W=25 BAR,"
                                  "COL=6 W=25",
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

# include "C-NDF-Substitute-Hooks.h"
# include "S-Lists-Hooks.h"

static ULONG SNLT_Attr[] = { SWA_NDF_Substitute_Case,
                             SWA_NDF_Substitute_Wild,
                             SWA_NDF_Substitute_Longest, 0 };
static ULONG SNLT_Cols[] = { 0, 1, 2 };

# define SETUPCUST \
            if (CreateListmgr((Object *)retval, idata) != SUCCESS) \
               SWDB_RET(NULL);

# define SW_Methods                  \
  SW_DO_MF(SWM_NDFilter_ModTxt);     \
  SW_DO_MF(SWM_NDFilter_StartMatch); \
  SW_DO_MF(SWM_NDFilter_EndMatch);   \
  SW_DO_MF(SWM_NDFilter_UI);         \
  SW_DO_MF(SWM_NDFilter_HideUI);     \
  SW_DO_MF(SWM_NDFilter_GenAddObj);  \
  SW_DO_MF(SWM_NDFilter_RemAddObj);  \
  SW_DO_MF(MUIM_Setup);              \
  SW_DO_MF(MUIM_Cleanup);            \

# include "S-Lists-Mthd.h"
# include "C-NDF-Substitute-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
