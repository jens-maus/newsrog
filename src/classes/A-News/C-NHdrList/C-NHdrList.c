# include "C-NHdrList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_NHdrList

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWP_NList_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_B MUIX_U "Header",
   MUIX_B MUIX_U "Value",
   MUIX_B MUIX_U "Group Patt",
   MUIX_B MUIX_U "Mode",

   MUIX_I "Unset",
   MUIX_I "Unset",
   "*",

   "Header: ",
   "Value: ",
   "Group Patt: ",
   "Mode: ",
   "Any",
   "All",
   "_Add",
   "_Delete",

   "Header list.  These optional headers will\n"
      "attached to articles.  For example, you might\n"
      "define an " SWX_B("X-No-Archive") " header with a value\n"
      "of " SWX_I("yes") ".  A group pattern may be used\n"
      "to have certain headers apply to a restricted set\n"
      "of groups.",

   "Add a new header.",
   "Delete selected header(s).",

   "This string is used as a user defined header\n"
   "to be posted with articles.  User headers should\n"
   "almost always begin with " SWX_B("X-") ".  For example\n"
   "you might use " SWX_B("X-No-Archive") " here.",

   "This string is the value for the header.  If\n"
   "the header above is " SWX_B("X-No-Archive") ", then\n"
   "you might use " SWX_B("yes") " for the value here.",

   "This is a pattern matching a groups name.\n"
   "This header will be posted if this pattern\n"
   "matches any (all) of the groups to which the\n"
   "article is being posted.  For example, if the\n"
   "pattern is " SWX_B("comp.*") ", then the header will\n"
   "be posted to comp.sys.amiga.misc, but not to\n"
   "rec.boats.marketplace.",
         
   "Change the header pattern mode between:\n\n"
   SWX_B("Any") " - This header will be posted if the\n"
   "  given group pattern matches any group to\n"
   "  which the article is posted.\n\n"
   SWX_B("All") " - This header will be posted only if\n"
   "  the given group pattern matches all the\n"
   "  groups to which the article is posted.\n\n"
   "This gadget has no effect if the article is\n"
   "posted to only one group, or if no group\n"
   "pattern is used.",

   NULL,
};


static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NHdrList_Hdr,
   SWA_NHdrList_Val,
   SWA_NHdrList_Grp,
   SWA_NHdrList_Mode,
   NULL
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

# include "C-NHdrList-Hooks.h"
# include "S-Lists-Hooks.h"


/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

static Object *StrGad(ULONG h)
{
   return StringObject,
      StringFrame,
      MUIA_CycleChain,    1,
      MUIA_HorizWeight,   5000,
      MUIA_ShortHelp,     GetS(h),
      MUIA_String_MaxLen, 512,
   End;
}

static void InitStr(SW_IDATA *idata, Object *s)
{
   SetAttrs(s,
            MUIA_Disabled,              TRUE,
            MUIA_String_AttachedList,   idata->SWA_NList_LV,
            TAG_DONE);
}


static ULONG CreateObj(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   Object *g, *b_Add, *b_Del;
   static STRPTR ModeLabs[3];

   set(o, MUIA_Group_Horiz, FALSE);

   ModeLabs[0] = GetS(SWS_L_ModeAny);
   ModeLabs[1] = GetS(SWS_L_ModeAll);
   ModeLabs[2] = NULL;
   
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
            MUIA_List_Format,          "COL=0 BAR,COL=1 BAR,COL=3 BAR,COL=2",
            MUIA_List_DragSortable,  TRUE,
            MUIA_List_Title,         TRUE,
            MUIA_List_DisplayHook,   &idata->DspHook,
            MUIA_List_DestructHook,  &idata->DstHook,
            End,
          End);

   if (!idata->SWA_NList_LV) return FAILURE;

   DoMethod(o, OM_ADDMEMBER,
            g = VGroup,
               MUIA_Background, MUII_GroupBack,
               Child, ColGroup(2),
                  Child, Label2(GetS(SWS_L_HdrLab)),
                  Child, s_Hdr  = StrGad(SWS_H_Hdr),
                  Child, Label2(GetS(SWS_L_ValLab)),
                  Child, s_Val  = StrGad(SWS_H_Val),
                  Child, Label2(GetS(SWS_L_GrpLab)),
                  Child, s_Grp  = StrGad(SWS_H_Grp),
                  Child, Label2(GetS(SWS_L_ModeLab)),
                  Child, c_Mode = CycleObject,
                     MUIA_Cycle_Entries, ModeLabs,
                     MUIA_CycleChain,    1,
                     MUIA_ShortHelp,     GetS(SWS_H_Mode),
                  End,
               End,
               Child, HGroup,
                  MUIA_Group_SameSize, TRUE,
                  Child, b_Add  = MkBt(SWS_B_Add,   SWS_H_Add),
                  Child, b_Del  = MkBt(SWS_B_Del,   SWS_H_Del),
               End,
            End);

   s_HdrAttr   = SWA_NHdrList_Hdr;
   s_ValAttr   = SWA_NHdrList_Val;
   s_GrpAttr   = SWA_NHdrList_Grp;
   c_ModeAttr  = SWA_NHdrList_Mode;
   c_ModeGAttr = MUIA_Cycle_Active;
   c_ModeDef   = SWV_NHdrList_Any;

   if (!g) return FAILURE;

   InitStr(idata, s_Hdr);
   InitStr(idata, s_Val);
   InitStr(idata, s_Grp);

   DoMethod(b_Del,   NOTP, o, 1, SWM_NList_RemoveSel);
   DoMethod(b_Add,   NOTP, o, 2, SWM_NList_Add, NULL);

   DoMethod(b_Add, NOTP, MUIV_Notify_Window, 3,
            MUIM_Set, MUIA_Window_ActiveObject, s_Hdr);

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   return SUCCESS;
}


# define SETUPCUST                                                \
         if (CreateObj((Object *)retval, idata, ti) != SUCCESS)   \
            SWDB_RET(NULL);


#  define SW_Methods ;

# include "C-NHdrList-Mthd.h"
# include "S-Lists-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
