# include "C-VarList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_VarList

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWP_NList_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- Date variables ---------------------------------------------------------

struct DateVars { UBYTE *VarName; UBYTE *Template; ULONG Desc; } DateVar[] =
{
   "AmPm",           "%p", SWS_L_AmPm,
   "DayNum",         "%d", SWS_L_DayNum,
   "DayNumSp",       "%e", SWS_L_DayNumSp,
   "Hour12",         "%I", SWS_L_Hour12,
   "Hour12Sp",       "%Q", SWS_L_Hour12Sp,
   "Hour24",         "%H", SWS_L_Hour24,
   "Hour24Sp",       "%q", SWS_L_Hour24Sp,
   "Min",            "%M", SWS_L_Min,
   "Month",          "%B", SWS_L_Month,
   "MonthNum",       "%m", SWS_L_MonthNum,
   "MonthShort",     "%b", SWS_L_MonthShort,
   "Sec",            "%S", SWS_L_Sec,
   "Weekday",        "%A", SWS_L_Weekday,
   "WeekdayShort",   "%a", SWS_L_WeekdayShort,
   "Year2",          "%y", SWS_L_Year2,
   "Year4",          "%Y", SWS_L_Year4,
   NULL,             NULL,
};

struct MemVars { UBYTE *VarName; ULONG Tag; ULONG Desc; ULONG Divisor; }
MemVar[] =
{
   "ChipKb",         MEMF_CHIP,             SWS_L_ChipKb,         1024,
   "FastKb",         MEMF_FAST,             SWS_L_FastKb,         1024,
   "AllKb",          MEMF_ANY,              SWS_L_AllKb,          1024,
   "LargestKb",      MEMF_ANY|MEMF_LARGEST, SWS_L_LargestKb,      1024,
   "ChipMb",         MEMF_CHIP,             SWS_L_ChipMb,    1024*1024,
   "FastMb",         MEMF_FAST,             SWS_L_FastMb,    1024*1024,
   "AllMb",          MEMF_ANY,              SWS_L_AllMb,     1024*1024,
   "LargestMb",      MEMF_ANY|MEMF_LARGEST, SWS_L_LargestMb, 1024*1024,
   NULL,             NULL,                  NULL,                    1,
};



// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_B MUIX_U "Variable",
   MUIX_B MUIX_U "Value",

   "Value: ",
   "_Add",
   "_Delete",
   "_Sort",

   "Variable list.  Almost any string value in an object\n"
   "can contain variable replacements of the form:\n"
   MUIX_I "${varname}" MUIX_N ".  Variables are replaced from the most\n"
   "most local scope possible, starting at the object, its\n"
   "parent, and so on up to this global list.  Replacement is\n"
   "performed until no more is possible: i.e, given variables\n"
   "foo=${bar}, and bar=not, then:\n"
   "   AA${foo}BB  = AAnotBB\n"
   "Variables can be used inside variables:\n"
   "   foo = bar, bar1 = A, then ${${foo}1} = A\n"
   "It's powerful, but read the documentation!",

   "Add a new variable.",
   "Delete selected variables.",
   "Sort variables by name.",
   
   "ERROR",

   MUIX_I "Unset",
   MUIX_I "Unset",

   "Variable:",
   "Value:",

   "Day of week (short)",
   "Day of week",
   "Month name (short)",
   "Month name",
   "Day number (leading zeros)",
   "Day number (leading spaces)",
   "Hour in 24 hour format (leading zeros)",
   "Hour in 12 hour format (leading zeros)",
   "Month number",
   "Minute value",
   "AM/PM string",
   "Hour in 24 hour format (leading spaces)",
   "Hour in 12 hour format (leading spaces)",
   "Seconds",
   "2 digit year",
   "4 digit year",

   "Chip memory in Kb",
   "Fast memory in Kb",
   "All memory in Kb",
   "Largest block in Kb",
   "Chip memory in Mb",
   "Fast memory in Mb",
   "All memory in Mb",
   "Largest block in Mb",

   "Unterminated variable name in object '%s'",
   "Too many variable replacements in object '%s'\n(Last = '%s')",
   "Variable name length too log ('%s...') in object '%s'",
   "No expansion found for variable '%s' in object '%s'",
   "Memory allocation error.",

   NULL,
};


static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_VarList_Var,
   SWA_VarList_Val,
   NULL
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

static __saveds __asm void DateFmtFn(a0 struct Hook *hook, a1 ULONG c);
static LONG CmpVars(Object *o1, Object *o2);

# include "C-VarList-Hooks.h"
# include "S-Lists-Hooks.h"

static void RemoveRO(Object *o)
{
   Object *obj;
   LONG x;

   DoMethod(o, MUIM_List_Select, MUIV_List_Select_All, MUIV_List_Select_Off,
            NULL);

   set(o, MUIA_List_Active, MUIV_List_Active_Off);

   SW_MUIListLoop(o, x, obj, TRUE)
      if (SWGetUL(obj, SWA_Attrval_Num))
         DoMethod(o, MUIM_List_Select, x, MUIV_List_Select_On, NULL);
         
   DoMethod(o, MUIM_List_Remove, MUIV_List_Remove_Selected);
}


static void InsVars(Object *o, UBYTE *Var, ULONG Desc, ULONG ro)
{
   DoMethod(o, MUIM_List_InsertSingle, 
            MUI_NewObject(SWC_Attrval,
                          SWA_VarList_Var,  Var,
                          SWA_VarList_Val,  GetS(Desc),
                          SWA_VarList_RO,   ro,
                          TAG_DONE),
            MUIV_List_Insert_Bottom);
}


static void SetupRO(Object *o)
{
   LONG x, y = 3;

   // -- first remove old vars -----------------------------------------------

   set(o, MUIA_List_Quiet, TRUE);
   RemoveRO(o);

   // -- Now insert new ones -------------------------------------------------

   for (x=0; DateVar[x].VarName; x++, y++)
      InsVars(o, DateVar[x].VarName, DateVar[x].Desc, y);

   for (x=0; MemVar[x].VarName; x++, y++)
      InsVars(o, MemVar[x].VarName,  MemVar[x].Desc, y);

   set(o, MUIA_List_Quiet, FALSE);
}


/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

static ULONG CreateObj(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   Object *g, *b_Add, *b_Del, *b_Sort;

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
            MUIA_List_Format,          "COL=0,COL=1",
            MUIA_List_DragSortable,  TRUE,
            MUIA_List_Title,         TRUE,
            MUIA_List_DisplayHook,   &idata->DspHook,
            MUIA_List_DestructHook,  &idata->DstHook,
            MUIA_List_CompareHook,   &idata->CmpHook,
            End,
          End);

   if (!idata->SWA_NList_LV) return FAILURE;

   DoMethod(o, OM_ADDMEMBER,
            g = VGroup,
               MUIA_Background, MUII_GroupBack,
               Child, ColGroup(2),
                  Child, b_Add  = MkBt(SWS_B_Add,    SWS_H_Add),
                  Child, s_Var  = StringObject,
                                     StringFrame,
                                     MUIA_CycleChain,     1,
                                     MUIA_HorizWeight, 5000,
                                  End,
                  Child, Label2(GetS(SWS_L_ValLab)),
                  Child, s_Val  = StringObject,
                                     StringFrame,
                                     MUIA_CycleChain,     1,
                                     MUIA_HorizWeight, 5000,
                                  End,
               End,
               Child, HGroup,
                  Child, b_Del   = MkBt(SWS_B_Del,   SWS_H_Del),
                  Child, b_Sort  = MkBt(SWS_B_Sort,  SWS_H_Sort),
               End,
            End);

   s_VarAttr   = SWA_VarList_Var;
   s_ValAttr   = SWA_VarList_Val;

   if (!g) return FAILURE;

   SetAttrs(s_Var,
            MUIA_Disabled,              TRUE,
            MUIA_String_AttachedList,   idata->SWA_NList_LV,
            TAG_DONE);

   DoMethod(b_Sort,  NOTP, o, 1, MUIM_List_Sort);
   DoMethod(b_Del,   NOTP, o, 1, SWM_NList_RemoveSel);
   DoMethod(b_Add,   NOTP, o, 2, SWM_NList_Add, NULL);

   DoMethod(b_Add, NOTP, MUIV_Notify_Window, 3,
            MUIM_Set, MUIA_Window_ActiveObject, s_Var);

   idata->DateHook.h_Entry= (ULONG (*)())DateFmtFn;
   idata->DateHook.h_Data = (void *)idata;

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   idata->loc = OpenLocale(NULL);

   return SUCCESS;
}


# define SETUPCUST                                                \
         if (CreateObj((Object *)retval, idata, ti) != SUCCESS)   \
            SWDB_RET(NULL);


# define DISPOSECUST \
    FreeVec(idata->ReplaceBuff); \
    FreeVec(idata->FnBuff);      \
    CloseLocale(idata->loc);


#  define SW_Methods                    \
   SW_DO_MF(SWM_VarList_Search);        \
   SW_DO_MF(SWM_VarList_ReplaceValues); \
   SW_DO_MF(SWM_VarList_FreeString);    \
   SW_DO_MF(SWM_VarList_CpParam);       \
   SW_DO_MF(SWM_VarList_FindParam);     \
   SW_DO_MF(SWM_VarList_FindParamNum);  \
   SW_DO_MF(SWM_VarList_FindParamChr);  \
   SW_DO_MF(SWM_VarList_FindRest);      \
   SW_DO_MF(MUIM_Setup);                \
   SW_DO_MF(MUIM_Cleanup);              \

# include "C-VarList-Mthd.h"
# include "S-Lists-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
