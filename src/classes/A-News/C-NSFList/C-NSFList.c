# include "C-NSFList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- popup filter menu ------------------------------------------------------

static struct NewMenu FltMenu[] =
{
  { NM_TITLE, (STRPTR)SWS_M_Filter,       0 ,0,0,(APTR)0                },
  { NM_ITEM,  (STRPTR)SWS_M_SelAll,       0 ,0,0,(APTR)SWS_M_SelAll     },
  { NM_ITEM,  (STRPTR)SWS_M_Save,         0 ,0,0,(APTR)SWS_M_Save       },
  { NM_ITEM,  (STRPTR)SWS_M_Load,         0 ,0,0,(APTR)SWS_M_Load       },
  { NM_ITEM,  (STRPTR)SWS_M_Clear,        0 ,0,0,(APTR)SWS_M_Clear      },

  { NM_END,   (STRPTR)NULL,               0, 0,0, (APTR)0               },
};


// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[30];
SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_NSFList_Mode,       SWAM_NoLab, SWAM_SGI,  SWV_NSFList_Or),
   SWAM_BO(SWA_NSFList_UIMode,     SWAM_NoLab, SWAM_SGIN, SWV_NSFList_UI_Normal),
   SWAM_BO(SWA_NSFList_ShowGroups, SWAM_NoLab, SWAM_SGIN, FALSE),
   SWAM_FI(SWA_NSFList_ItemChange, SWAM_NoLab, SWAM_Get,  FALSE),
   SWAM_BO(SWA_NSFList_NoDispose,  SWAM_NoLab, SWAM_SGIN, FALSE),
   SWAM_PT(SWA_NSFList_Group,      SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_PT(SWA_NSFList_FltType,    SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_BO(SWA_NSFList_DefMatch,   SWAM_NoLab, SWAM_SGIN, TRUE),
   SWAM_BO(SWA_NSFList_UseCtxMenu, SWAM_NoLab, SWAM_SGIN, TRUE),
   SWAM_PT(SWA_NSFList_FFToolBar,  SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_IN(SWA_NSFList_Use,        SWAM_NoLab, SWAM_SGIN, SWV_NSFList_Use_Select),
   SWP_NNews_Attrs,
   SWP_NList_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Add...",
   "Delete",
   "Edit..",
   "Clone",
   "_Insert Selected",
   "_Close",

   "Description: ",
   "Group Patt: ",

   "Any",
   "All",
   MUIX_I "None",
   MUIX_I "Unset",
   MUIX_I "All Groups",

   MUIX_U MUIX_B "Use",
   MUIX_U MUIX_B "Inv",
   MUIX_U MUIX_B "Description",
   MUIX_U MUIX_B "Type",
   MUIX_U MUIX_B "Tests",
   MUIX_U MUIX_B "%",
   MUIX_U MUIX_B "Groups",

   "Add a new filter to the list.",
   "Delete selected filters from the list.",
   "Edit a filter.",
   "Make a copy of the filter.",
   "Inserts the selected filters to the list.",
   "Close the list of filter types.",

   "A description for the filter.  This may be\n"
      "any string.",

   "An AmigaOS pattern matching groups that\n"
      "this filter will be used in.  For example:\n"
      SWX_B("  comp.sys.amiga.*") " or " SWX_B("*.pets.*") " An emtpy\n"
      "string matches all groups.",

    "Filters",
    "Select All",
    "Save...",
    "Load...",
    "Clear",

    "*_Ok",
    "_Ok|*_Cancel",
    "Canceled.",
    "%ld filters were saved.",
    "%ld filters were loaded.",
    "OK to clear filter list?",

    "Select Filters to Load",
    "Author Note",

    "_Load Selected",
    "_Select All",
    "_Close",

    "This button will cause all of the\n"
       "selected filters to be loaded.",
    "This button will select\n"
       "all of the filters.",
    "This button will close the insert window\n"
       "without loading any more filters.",

    "There are two possible modes which\n"
    "control how filters operate if more\n"
    "than one is defined:\n\n"
    SWX_B("  Any") " - An article will be\n"
   "   matched if it is matched by any of\n"
   "   the defined filters.\n\n"
    SWX_B("  All") " - An article will be\n"
   "   matched only if it is matched by all\n"
   "   of the defined filters.",
   
    "Double click on a single filter to load\n"
       "it, or select the filters you wish to load,\n"
       "and use the load button below.  If a filter\n"
       "has an author note field, it will be displayed\n"
       "in the information box below when it is\n"
       "clicked on in the this list.",

    SWX_B("Note: ") "%ld filters from this file were\n"
       "of the wrong type for this list.",
    SWX_B("Error: ") "No filters were selected.",
    SWX_B("Error: ") "This file is not a\n"
       "NewsRog display filter object.",

   NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NSFilter_Inv,
   SWA_NSFilter_Enabled,
   SWA_NSFilter_Desc,
   SWA_NSFilter_TotNow,
   SWA_NSFilter_Groups,
   NULL
};

static Object *AddIfMatch(Object *o, SW_IDATA *idata)
{
   ULONG  PLen;
   STRPTR Groups = SWGetStr(o, SWA_NSFilter_Groups);
   Object *Group = idata->SWA_NSFList_Group;

   // -- set the appbase object ----------------------------------------------

   if (idata->SWA_AppBase_TopObj)
      SetAttrs(o, AppBaseDefTop(idata), TAG_DONE);

   // -- insert the object if master list or if grp patt is empty ------------

   if (FIsSet(idata->NLFlags,  SWF_NList_MasterFlag) ||
       SW_STRISNULL(Groups)                          ||
       !Group)
      goto Done;

   PLen = strlen(Groups)*2+2;

   if (PLen >= idata->GrpPattLen) {
      FreeVec(idata->GrpPatt);
      if (!(idata->GrpPatt = AllocVec(PLen, MEMF_ANY))) return NULL;
   }

   if (ParsePattern(Groups, idata->GrpPatt, PLen) < 0)  return NULL;

   o = MatchPattern(idata->GrpPatt, SWGetStr(Group, SWA_NGroup_Name))
      ? o : NULL;

 Done:

   if (o && idata->SWA_NSFList_FFToolBar)
      DoMethod(o, SWM_NSFilter_AddFFIcons, Group, idata->o);

   return o;
}

# include "C-NSFList-Hooks.h"
# include "S-Lists-Hooks.h"

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

static ULONG CreateListmgr(Object *o, SW_IDATA *idata)
{
   idata->o = o;
   
   idata->SWA_NList_L = ObjListObject,
      InputListFrame,
      SWA_ObjList_Object,      o,
      MUIA_List_Format,        "",
      MUIA_List_DragSortable,  TRUE,
      MUIA_List_Title,         TRUE,
      MUIA_List_DisplayHook,   &idata->DspHook,
      MUIA_List_DestructHook,  &idata->DstHook,
      MUIA_List_ConstructHook, &idata->CstHook,
   End;

   if (!idata->SWA_NList_L) return FAILURE;

   DoMethod(o, OM_ADDMEMBER,
            idata->SWA_NList_LV = ListviewObject,
               MUIA_CycleChain,           1,
               MUIA_Listview_Input,       TRUE,
               MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
               MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
               MUIA_Listview_DefClickColumn, 0,
               MUIA_Listview_List,        idata->SWA_NList_L,
            End);

   if (!idata->SWA_NList_LV) {
      MUI_DisposeObject(idata->SWA_NList_L);
      return FAILURE;
   }

   return SUCCESS;
}

static void SWNListUserSet(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data);

static void SetTop(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (Tag == SWA_AppBase_TopObj) {
      idata->SWA_AppBase_TopObj     = (Object *)Data,

      DoMethod(o, SWM_NList_ListSet, Tag, Data);
    }

   if (Tag == SWA_NList_Master) {
      set(o, SWA_NSFList_Mode, SWGetUL((Object *)Data, SWA_NSFList_Mode));
   }

   SWNListUserSet(o, idata, Tag, Data);
}

static void CloseInsertEditor(SW_IDATA *idata)
{
   if (!idata->InsertEditorWin) return;

   DoMethod(idata->InsertEditorList, SWM_NSFilter_HideUI);

   set(idata->InsertEditorWin, MUIA_Window_Open, FALSE);
   DoMethod(idata->App, OM_REMMEMBER, idata->InsertEditorWin);
   MUI_DisposeObject(idata->InsertEditorWin);
   MUI_DisposeObject(idata->InsertEditorList);
   idata->InsertEditorWin  = NULL;
   idata->InsertEditorList = NULL;
   idata->InsertEditorInfo = NULL;
}

Object *PopMenu = NULL;

# define SW_USERSETUP if (FirstTime) \
                         SWLocalizeNewMenu(SW_CATALOG, SW_CATSTR, FltMenu); \
                      if (!PopMenu) \
                         PopMenu = MUI_MakeObject(MUIO_MenustripNM, FltMenu, 0);

# define SW_USERCLEANUP MUI_DisposeObject(PopMenu); PopMenu = NULL;

# define SW_USERSET SetTop(o, idata, Tag, Data)

# define DISPOSECUST FreeVec(idata->GrpPatt);  \
                     CloseInsertEditor(idata);

# define DISPOSECUST_POST   \
     if (idata->MemPool) {  \
        FV(idata->FFIcons); \
        FV(idata->FFObjs);  \
     }

static ULONG SetupObj(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   setlent(o, SWA_ObjNoChildCopy, TRUE);
   return CreateListmgr(o, idata);
}

# define SETUPCUST if (SetupObj((Object *)retval, idata, ti) != SUCCESS) \
                      SWDB_RET(NULL);

# define SW_NList_SetupPre \
    idata->SWA_NSFList_Group     = GetTagObj(SWA_NSFList_Group, NULL, ti), \
    idata->SWA_NSFList_FFToolBar = GetTagObj(SWA_NSFList_FFToolBar, NULL, ti), \
    idata->SWA_AppBase_TopObj    = GetTagObj(SWA_AppBase_TopObj, NULL, ti)
   

# define SW_NList_ClickCol

# define SNLT_Attr idata->ClickAttrs
# define SNLT_Cols idata->ClickCols

static Object *FixInsObj(Object *io, ULONG Enabled)
{
   if (io)
      SetAttrs(io,
               SWA_NSFilter_Enabled, Enabled,
               SWA_NSFilter_Desc,    SWGetStr(io, SWA_ObjClassName) + 4,
               TAG_DONE);

   return io;
}

static Object *MkInsObj(Object *c)
{
   return FixInsObj((Object *)DoMethod(c, SWM_ClassList_MakeObj), FALSE);
}

# define SW_Methods                    \
  SW_DO_MF(SWM_NSFilter_Begin);        \
  SW_DO_MF(SWM_NSFilter_Match);        \
  SW_DO_MF(SWM_NSFilter_End);          \
  SW_DO_MF(SWM_NSFilter_UI);           \
  SW_DO_MF(SWM_NSFilter_HideUI);       \
  SW_DO_MF(SWM_NSFilter_TrackStats);   \
  SW_DO_MF(SWM_NSFilter_Expire);       \
  SW_DO_MF(SWM_NSFList_DoSelected);    \
  SW_DO_MF(SWM_NSFList_AddSel);        \
  SW_DO_MF(SWM_NSFList_Save);          \
  SW_DO_MF(SWM_NSFList_Load);          \
  SW_DO_MF(SWM_NSFList_CtxMenuChoice); \
  SW_DO_MF(SWM_NSFList_InsEdClose);    \
  SW_DO_MF(SWM_NSFList_InsEdLoad);     \
  SW_DO_MF(SWM_NSFList_InsEdDspInfo);  \
  SW_DO_MF(SWM_NSFList_InsEdMkAll);    \
  SW_DO_MF(SWM_NSFList_AddFFIcon);     \
  SW_DO_MF(SWM_NSFList_RemFFIcons);    \
  SW_DO_MF(SWM_ObjList_DragQuery);     \
  SW_DO_MF(SWM_ObjList_DragDrop);      \
  SW_DO_MF(MUIM_Setup);                \
  SW_DO_MF(MUIM_Cleanup);              \

# include "S-Lists-Mthd.h"
# include "C-NSFList-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
