# include "C-FileTypes-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_FileTypes

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWP_NList_Attrs,
   SWP_AppBase_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_B MUIX_U "Media Type",
   MUIX_B MUIX_U "Method",
   MUIX_B MUIX_U "Pattern",

   "Multimedia types list.  Files or data can be classified\n"
   "based on a pattern match of the filename, or on the \n"
   "contents of the file or data.  The first specification\n"
   "that matches the file will be used, so expensive\n"
   "operations (such as \33bMatch File \33bContents (Pattern)\33n\n"
   "should be used last).  Some data may describe its own\n"
   "type (e.g. MIME).\n\n"
   "You must read the documentation fully understand this\n"
   "feature!",
   
   MUIX_I "Unset",
   MUIX_I "Unset",

   "Media Type:",
   "Pattern:",
   "Method:",

   "Match File Name (Ignore Case)",
   "Match File Name (Match Case)",
   "Match Data Contents (Binary)",
   "Match Data Contents (Pattern)",
   "Match Directory Name (Ignore Case)",
   "Match Directory Name (Match Case)",
   "Ask External",

   "FName",
   "FName(case)",
   "Contents(bin)",
   "Contents(patt)",
   "DirName",
   "DirName(case)",
   "External",

   "_New",
   "_Delete",
   "_Clone",
   "_Save...",
   "_Insert...",
   "So_rt",

   "Add a new filetype to the list.",
   "Delete the selected filetypes.",
   "Clone the selected filetype.",
   "Save the selected filetype.",
   "Insert saved filetypes from disk.",
   "Sort the filetypes list by name.",

   NULL,
};


static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_FileTypes_FT,
   SWA_FileTypes_Patt,
   SWA_FileTypes_Method,
   NULL
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

static LONG CmpVars(Object *o1, Object *o2);

# include "C-FileTypes-Hooks.h"
# include "S-Lists-Hooks.h"

/*****************************************************************************
 * Function: return string object                                            *
 *****************************************************************************/

static Object *StrObj(void)
{
   return StringObject,
      StringFrame,
      MUIA_CycleChain,     1,
      MUIA_HorizWeight, 5000,
   End;
}


/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

static ULONG CreateObj(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   Object *g, *b_New, *b_Del, *b_Sort, *b_Clone, *b_Save, *b_Ins;

   set(o, MUIA_Group_Horiz, FALSE);

   DoMethod(o, OM_ADDMEMBER,
            idata->SWA_NList_LV = ListviewObject,
            MUIA_CycleChain,           1,
            MUIA_ShortHelp,            GetS(SWS_H_List),
            MUIA_Listview_Input,       TRUE,
            MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
            MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
            MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
            InputListFrame,
            SWA_ObjList_Object,        o,
            MUIA_List_Format,          "COL=0 BAR,COL=1 BAR,COL=2",
            MUIA_List_DragSortable,  TRUE,
            MUIA_List_Title,         TRUE,
            MUIA_List_DisplayHook,   &idata->DspHook,
            MUIA_List_DestructHook,  &idata->DstHook,
            MUIA_List_CompareHook,   &idata->CmpHook,
            End,
          End);

   if (!idata->SWA_NList_LV) return FAILURE;

  idata->CycGads[FT_MatchFNameNoCase] = GetS(SWS_L_MatchFNameNoCase);
  idata->CycGads[FT_MatchFNameCase]   = GetS(SWS_L_MatchFNameCase);
  idata->CycGads[FT_MatchFCBin]       = GetS(SWS_L_MatchFCBin);
  idata->CycGads[FT_MatchFCPattern]   = GetS(SWS_L_MatchFCPattern);
  idata->CycGads[FT_MatchDNameNoCase] = GetS(SWS_L_MatchDNameNoCase);
  idata->CycGads[FT_MatchDNameCase]   = GetS(SWS_L_MatchDNameCase);
  idata->CycGads[FT_AskExternal]      = GetS(SWS_L_AskExternal);
  idata->CycGads[FT_FTMAX]            = NULL;

   DoMethod(o, OM_ADDMEMBER,
            g = VGroup,
               MUIA_Background, MUII_GroupBack,
               Child, ColGroup(2),
                  Child, Label2(GetS(SWS_L_EType)),
                  Child, s_FT   = StrObj(),
                  Child, Label2(GetS(SWS_L_EPatt)),
                  Child, s_Pat  = StrObj(),
                  Child, Label2(GetS(SWS_L_EMethod)),
                  Child, s_Meth = CycleObject,
                                     MUIA_CycleChain,    1,
                                     MUIA_Cycle_Entries, idata->CycGads,
                                  End,
               End,
               Child, RowGroup(2),
                  Child, b_New   = MkBt(SWS_B_New,   SWS_H_New),
                  Child, b_Del   = MkBt(SWS_B_Del,   SWS_H_Del),
                  Child, b_Clone = MkBt(SWS_B_Clone, SWS_H_Clone),
                  Child, b_Save  = MkBt(SWS_B_Save,  SWS_H_Save),
                  Child, b_Ins   = MkBt(SWS_B_Ins,   SWS_H_Ins),
                  Child, b_Sort  = MkBt(SWS_B_Sort,  SWS_H_Sort),
               End,
            End);

   s_FTAttr    = SWA_FileTypes_FT;
   s_PatAttr   = SWA_FileTypes_Patt;
   s_MethAttr  = SWA_FileTypes_Method;
   s_MethDef   = FT_MatchFNameNoCase;
   s_MethGAttr = MUIA_Cycle_Active;


   if (!g) return FAILURE;

   SetAttrs(s_FT,
            MUIA_Disabled,              TRUE,
            MUIA_String_AttachedList,   idata->SWA_NList_LV,
            TAG_DONE);

   SetAttrs(s_Pat,
            MUIA_Disabled,              TRUE,
            MUIA_String_AttachedList,   idata->SWA_NList_LV,
            TAG_DONE);

   SetAttrs(s_Meth,
            MUIA_Disabled,              TRUE,
            TAG_DONE);

   DoMethod(b_Sort,  NOTP, o, 1, MUIM_List_Sort);
   DoMethod(b_Del,   NOTP, o, 1, SWM_NList_RemoveSel);
   DoMethod(b_New,   NOTP, o, 2, SWM_NList_Add, NULL);
   DoMethod(b_Clone, NOTP, o, 1, SWM_NList_Clone);
   DoMethod(b_Ins,   NOTP, o, 1, SWM_FileTypes_Ins);
   DoMethod(b_Save,  NOTP, o, 1, SWM_FileTypes_Save);

   DoMethod(b_New, NOTP, MUIV_Notify_Window, 3,
            MUIM_Set, MUIA_Window_ActiveObject, s_FT);

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   return SUCCESS;
}


# define SETUPCUST                                                \
         if (CreateObj((Object *)retval, idata, ti) != SUCCESS)   \
            SWDB_RET(NULL);

#  define SW_Methods                    \
   SW_DO_MF(SWM_FileTypes_FindType);    \
   SW_DO_MF(SWM_FileTypes_Ins);         \
   SW_DO_MF(SWM_FileTypes_Save);        \


# include "C-FileTypes-Mthd.h"
# include "S-Lists-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
