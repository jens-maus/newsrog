# include "C-FileActs-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_FileActs

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
   MUIX_B MUIX_U "Action Name",
   MUIX_B MUIX_U "Command",
   MUIX_B MUIX_U "Args",
   MUIX_B MUIX_U "Input",
   MUIX_B MUIX_U "Output",
   MUIX_B MUIX_U "Dir",
   MUIX_B MUIX_U "Stack",
   MUIX_B MUIX_U "Pri",

   "Action Defs",
   "Other Opts",
   
   "Multimedia actions list.  Media of various types may have\n"
   "multiple defined actions.  For example, " SWX_B("Show") " might call\n"
   "one command on " SWX_B("image/*") " files and another on\n"
   SWX_B("video/quicktime") "  The first specification that\n"
   "matches will be used. You must read the documentation\n"
   "to fully understand this feature!",
   
   MUIX_I "Unset",

   "Media Type: ",
   "Action: ",
   "Cmd: ",
   "Args: ",
   "Input: ",
   "Output: ",
   "Stack: ",
   "Pri: ",
   "Dir: ",
   "ASync: ",
   "CLI",
   "ARexx",
   "WB",
   "Rexx Port: ",
   "Rexx Cmd: ",

   "_New",
   "_Delete",
   "_Clone",
   "_Save...",
   "_Insert...",
   "So_rt",

   "Add a new action to the list.",
   "Delete the selected actions.",
   "Clone the selected actions.",
   "Save the selected action.",
   "Insert saved actions from disk.",
   "Sort the action list by name.",

   "Select Action Command",

   SWX_B("Error: ") "No action `%s' found\nfor type '%s'.",
   SWX_B("Error: ") "Empty action",

   NULL,
};


static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_FAct_Type,
   SWA_FAct_Action,
   SWP_SysCmdAttrVals(SWA_FAct),
   NULL
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

static LONG CmpVars(Object *o1, Object *o2);

# include "C-FileActs-Hooks.h"
# include "S-Lists-Hooks.h"

/*****************************************************************************
 * Function: return string object                                            *
 *****************************************************************************/

static Object *StrObj(Object *l)
{
   return ObjStringObject,
      StringFrame,
      MUIA_Disabled,            TRUE,
      MUIA_CycleChain,          1,
      MUIA_HorizWeight,         5000,
      MUIA_String_AttachedList, l,
      MUIA_String_MaxLen,       512,
   End;
}

static Object *IntObj(Object *l, LONG Min, LONG Max)
{
   Object *o= Slider(Min, Max, 0);

   set(o, MUIA_CycleChain, 1);

   return o;
}


/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

static ULONG CreateObj(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   Object *g, *b_New, *b_Del, *b_Sort, *b_Clone, *b_Save, *b_Ins;
   static STRPTR Labs[3];
   static STRPTR CmdTLab[4];

   SetAttrs(o,
            MUIA_Group_Horiz, FALSE,
            MUIA_Background,  MUII_GroupBack,
            TAG_DONE);

   Labs[0] = GetS(SWS_L_L1),
   Labs[1] = GetS(SWS_L_L2),
   Labs[2] = NULL;

   CmdTLab[0] = GetS(SWS_L_CLI);
   CmdTLab[1] = GetS(SWS_L_ARexx);
   CmdTLab[2] = NULL;
   
   DoMethod(o, OM_ADDMEMBER,
            HGroup,
               Child, idata->SWA_NList_LV = ListviewObject,
                  MUIA_CycleChain,           1,
                  MUIA_ShortHelp,            GetS(SWS_H_List),
                  MUIA_Listview_Input,       TRUE,
                  MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
                  MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
                  MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
                     InputListFrame,
                     SWA_ObjList_Object,        o,
                     MUIA_List_Format,          "COL=0 BAR,COL=1 BAR,COL=2 BAR,"
                                                "COL=3 BAR,COL=4 BAR,COL=5 BAR,"
                                                "COL=6 BAR,COL=7 BAR,COL=8",
                     MUIA_List_DragSortable,  TRUE,
                     MUIA_List_Title,         TRUE,
                     MUIA_List_DisplayHook,   &idata->DspHook,
                     MUIA_List_DestructHook,  &idata->DstHook,
                     MUIA_List_CompareHook,   &idata->CmpHook,
                  End,
               End,
               Child, VGroup,
                  MUIA_HorizWeight, 1,
                  Child, b_New   = MkBt(SWS_B_New,   SWS_H_New),
                  Child, b_Del   = MkBt(SWS_B_Del,   SWS_H_Del),
                  Child, b_Clone = MkBt(SWS_B_Clone, SWS_H_Clone),
                  Child, b_Save  = MkBt(SWS_B_Save,  SWS_H_Save),
                  Child, b_Ins   = MkBt(SWS_B_Ins,   SWS_H_Ins),
                  Child, b_Sort  = MkBt(SWS_B_Sort,  SWS_H_Sort),
                  Child, VSpace(0),
               End,
            End);

   if (!idata->SWA_NList_LV) return FAILURE;

   DoMethod(o, OM_ADDMEMBER,
            g = RegisterGroup(Labs),
                  MUIA_CycleChain, 1,
                  MUIA_VertWeight, 1,
                  MUIA_Background, MUII_RegisterBack,
                  Child, ColGroup(2),
                     Child, Label2(GetS(SWS_L_Type)),
                     Child, HGroup,
                        Child, s_Type   = StrObj(idata->SWA_NList_LV),
                        Child, Label2(GetS(SWS_L_Action)),
                        Child, s_Action = StrObj(idata->SWA_NList_LV),
                     End,
                     Child, idata->Lab1 = TextObject,
                        MUIA_Text_Contents, GetS(SWS_L_Cmd),
                     End,
                     Child, s_Cmd    = PopaslObject,
                        MUIA_Popstring_String, StrObj(idata->SWA_NList_LV),
                        MUIA_Popstring_Button, PopButton(MUII_PopFile),
                        ASLFR_TitleText,       GetS(SWS_T_CmdAsl),
                     End,
                     Child, idata->Lab2 = TextObject,
                        MUIA_Text_Contents, GetS(SWS_L_Args),
                     End,
                     Child, s_Args   = StrObj(idata->SWA_NList_LV),
                  End,
                  Child, ColGroup(4),
                     Child, Label2(GetS(SWS_L_Input)),
                     Child, s_In     = StrObj(idata->SWA_NList_LV),
                     Child, Label2(GetS(SWS_L_Output)),
                     Child, s_Out    = StrObj(idata->SWA_NList_LV),
                     Child, Label2(GetS(SWS_L_Stack)),
                     Child, s_Stack  = IntObj(idata->SWA_NList_LV, 1000, 100000),
                     Child, Label2(GetS(SWS_L_Pri)),
                     Child, s_Pri    = IntObj(idata->SWA_NList_LV, -127, 127),
                     Child, Label2(GetS(SWS_L_Dir)),
                     Child, s_Dir    = StrObj(idata->SWA_NList_LV),
                     Child, Label2(GetS(SWS_L_ASync)),
                     Child, HGroup,
                       Child, s_ASync  = CheckMark(TRUE),
                       Child, HSpace(0),
                       Child, s_CmdT   = CycleObject,
                          MUIA_Cycle_Entries, CmdTLab,
                          MUIA_CycleChain,    1,
                          MUIA_Disabled,      TRUE,
                       End,
                     End,
                  End,
               End);

   set(s_ASync, MUIA_CycleChain, 1);
   
   s_TypeAttr    = SWA_FAct_Type;
   s_ActionAttr  = SWA_FAct_Action;
   s_CmdAttr     = SWA_FAct_Command;
   s_ArgsAttr    = SWA_FAct_Args;
   s_InAttr      = SWA_FAct_Input;
   s_OutAttr     = SWA_FAct_Output;
   s_StackAttr   = SWA_FAct_Stack;
   s_StackGAttr  = MUIA_Slider_Level;
   s_StackDef    = 0;
   s_PriAttr     = SWA_FAct_Pri;
   s_PriGAttr    = MUIA_Slider_Level;
   s_PriDef      = 0;
   s_DirAttr     = SWA_FAct_Dir;
   s_ASyncAttr   = SWA_FAct_ASync;
   s_ASyncGAttr  = MUIA_Selected;
   s_ASyncDef    = TRUE;
   s_CmdTAttr    = SWA_FAct_CmdType;
   s_CmdTDef     = SWV_FAct_CLI;
   s_CmdTGAttr   = MUIA_Cycle_Active;

   DoMethod(s_CmdT, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            o, 2, SWM_FileActs_PostChange, 0);
   
   if (!g) return FAILURE;

   DoMethod(b_Sort,  NOTP, o, 1, MUIM_List_Sort);
   DoMethod(b_Del,   NOTP, o, 1, SWM_NList_RemoveSel);
   DoMethod(b_New,   NOTP, o, 2, SWM_NList_Add, NULL);
   DoMethod(b_Clone, NOTP, o, 1, SWM_NList_Clone);
   DoMethod(b_Ins,   NOTP, o, 1, SWM_FileActs_Ins);
   DoMethod(b_Save,  NOTP, o, 1, SWM_FileActs_Save);

   DoMethod(b_New, NOTP, MUIV_Notify_Window, 3,
            MUIM_Set, MUIA_Window_ActiveObject, s_Type);

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   return SUCCESS;
}

# define SETUPCUST                                                \
         if (CreateObj((Object *)retval, idata, ti) != SUCCESS)   \
            SWDB_RET(NULL);

# define DISPOSECUST FreeVec(idata->PatBuf);

#  define SW_Methods                   \
   SW_DO_MF(SWM_FileActs_Ins);         \
   SW_DO_MF(SWM_FileActs_Save);        \
   SW_DO_MF(SWM_FileActs_DoAction);    \
   SW_DO_MF(SWM_FileActs_GetActList);  \
   SW_DO_MF(SWM_FileActs_PostChange);  \


# define SW_NList_SetNum TRUE
   
# include "C-FileActs-Mthd.h"
# include "S-Lists-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
