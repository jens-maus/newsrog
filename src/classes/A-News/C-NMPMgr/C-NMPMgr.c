# include "C-NMPMgr-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NTCP

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWP_NNews_Attrs,
   SWP_NList_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_I "None",

   MUIX_B MUIX_U "Subject",

   NULL,
};

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NMPMgr_Subj,
   NULL
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


/*****************************************************************************
 * Function: Generate an object to insert                                    *
 *****************************************************************************/

static Object *MkInsObj(SW_IDATA *idata, STRPTR Name)
{
   Object *no = MUI_NewObject(SWC_Attrval4,
                              TAG_DONE);

   return no;
}


/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

# include "C-NMPMgr-Hooks.h"
# include "S-Lists-Hooks.h"

//static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static ULONG CreateObj(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   set(o, MUIA_Group_Horiz, FALSE);

   DoMethod(o, OM_ADDMEMBER,
            idata->SWA_NList_LV = ListviewObject,
            MUIA_CycleChain,           1,
            MUIA_Listview_Input,       TRUE,
            MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
            MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
            MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
            InputListFrame,
            SWA_ObjList_Object,       o,
            MUIA_List_Format,         "COL=0",
            MUIA_List_DragSortable,  TRUE,
            MUIA_List_Title,         TRUE,
            MUIA_List_DisplayHook,   &idata->DspHook,
            MUIA_List_DestructHook,  &idata->DstHook,
            MUIA_List_CompareHook,   &idata->CmpHook,
            End,
          End);

   if (!idata->SWA_NList_LV) return FAILURE;

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   return SUCCESS;
}

static void SWNListUserSet(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data);

static void SetTop(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (Tag == SWA_AppBase_TopObj)
      idata->SWA_AppBase_TopObj     = (Object *)Data, 

   SWNListUserSet(o, idata, Tag, Data);
}

# define SW_USERSET SetTop(o, idata, Tag, Data)

# define SETUPCUST                                                \
         if (CreateObj((Object *)retval, idata, ti) != SUCCESS)   \
            SWDB_RET(NULL);

// -- info for list clicking to set attrs ------------------------------------

#  define SW_Methods       \
   SW_DO_MF(MUIM_Setup);   \
   SW_DO_MF(MUIM_Cleanup);

# include "S-Lists-Mthd.h"
# include "C-NMPMgr-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
