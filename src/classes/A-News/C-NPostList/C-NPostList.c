# include "C-NPostList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NTCP

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_OB(SWA_NPostList_ListCol,    SWAM_NoLab,  SWAM_SGI | SWAM_ObjRead),
   SWAM_BO(SWA_NPostList_Saved,      SWAM_NoLab,  SWAM_SGIN, FALSE),
   SWAM_PT(SWA_NPostList_PostDir,    SWAM_NoLab,  SWAM_Get,  NULL),

   SWP_NNews_Attrs,
   SWP_NList_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_I        "Unset",
   MUIX_I        "Empty.",
   MUIX_C "Post Queue (%ld)",

   "Subject: ",
   "Group(s): ",

   "Posting %%ld of %ld",
   "Sending: %ld%%",
   "Posting Articles",
   MUIX_U MUIX_B "Lines",
   MUIX_U MUIX_B "Size",
   MUIX_U MUIX_B "Subject",
   MUIX_U MUIX_B "Group(s)",

   "_Cancel",
   "_Post",
   "_Edit",
   "_Delete",
   "_Add",
   "Select A_ll",
   "-->",
   "<--",      

   "The queue for articles to be posted to the net.\n"
      "Workbench icons, articles from a group window\n"
      "or groups from the group list window may be\n"
      "dropped on this list to create new posts.",

   "Post selected articles to the net.",
   "Edit an article.",
   "Delete selected articles.",
   "Add new post.",
   "Select all posts in the list.",
   "Move the selected posts to the\n"
      "sent posts archive.",
   "Move the selected posts to the\n"
      "post outgoing queue.",

   "Select which columns should be displayed\n"
      "in the server list.  Hotkey: '-'",
   "Copy the selected entries to the\n"
      "clipboard as text.  Hotkey: 'c'",

   "This is the list of groups this article\n"
      "will be posted to.  You may drag groups\n"
      "and drop them on this list to crosspost\n"
      "the article to those groups.",
   "You may drag these groups into the list on\n"
      "the left to add them to the crosspost\n"
      "list for this article.",
      
   "_Delete",
   "_Close",
   "_Restore",
   "Removes the selected groups from\n"
   "the list of groups to which this\n"
   "post will be sent.",
   "Close this popup group editor.",
   "Restore the group list to its\n"
   "state before recent changes.",

   NULL,
};

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NPost_Lines,
   SWA_NPost_Bytes,
   SWA_NPost_Subj,
   SWA_NPost_Grp,
   NULL
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


static STRPTR ColSpecs[] = {
   "COL=0 W=5 P=" MUIX_R " BAR",
   "COL=1 W=5 P=" MUIX_R " BAR",
   "COL=2 W=60 BAR",
   "COL=3 W=40 BAR",
   NULL,
};

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

# include "C-NGList.h"

/*****************************************************************************
 * Function: Generate an object to insert                                    *
 *****************************************************************************/

static Object *MkInsObj(SW_IDATA *idata)
{
   Object *no = MUI_NewObject(SWC_NPost,
                              AppBaseDefTop(idata),
                              SWA_NPost_PostDir, idata->SWA_NPostList_PostDir,
                              SWA_NPost_NewPost, TRUE,
                              TAG_DONE);
   
   return no;
}

/*****************************************************************************
 * Function: Remove list object, if asked to                                 *
 *****************************************************************************/

static void DelListObj(Object *o)
{
   if (!SWGetUL(o, SWA_NPost_NoDel))
      MUI_DisposeObject(o);
}


/*****************************************************************************
 * Function: Create a column list object                                     *
 *****************************************************************************/

static ULONG CreateListCol(Object *o, SW_IDATA *idata)
{
   ULONG x;
   
   static STRPTR ColNames[8];

   for (x=0; ColSpecs[x]; x++)
      ColNames[x] = GetS(SWS_T_Lines + x) + sizeof(MUIX_U MUIX_B) - 1;

   ColNames[x] = NULL;

   if (!(idata->SWA_NPostList_ListCol =
         MUI_NewObject(SWC_ListCol,
                       SWA_ListCol_ColSpecs, ColSpecs,
                       SWA_ListCol_ColNames, ColNames,
                       SWA_ListCol_Hook,     &idata->DspHook,
                       SWA_ListCol_List,     o,
                       TAG_DONE)))
      return FAILURE;

   return SUCCESS;
}


/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

# include "C-NPostList-Hooks.h"
# include "S-Lists-Hooks.h"


/*****************************************************************************
 * Some utility fns                                                          *
 *****************************************************************************/

static void LDoM(Object *o, SW_IDATA *idata, Object *b, ULONG m, ULONG m1)
{
   DoMethod(b, NOTP, o, 7, SWM_Root_ListDoMethod,
            idata->SWA_NList_LV,
            SWV_ListDoMethod_Selected, SWV_ListDoMethod_Entry, 2, m, m1);
}

static Object *MkTO(void)
{
   return TextObject,
      TextFrame,
      MUIA_Background, MUII_TextBack,
   End;
}


/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static ULONG CreateObj(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   Object *g, *to, *b_Add = NULL, *b_Del, *b_Edit, *b_SelAll, *b_Xfer;

   set(o, MUIA_Group_Horiz, FALSE);

   // -- set init-time attrs for saved/postdir -------------------------------
   
   idata->SWA_NPostList_PostDir =
      (idata->SWA_NPostList_Saved = GetTagUL(SWA_NPostList_Saved, FALSE, ti)) ?
         SWV_NPostList_PostSvDir : SWV_NPostList_DefPostDir;

   DoMethod(o, OM_ADDMEMBER,
            to = SWUI_FrameText(GetS(SWS_L_TopTitle), 0,
                                (HGroup,
                                 Child, (Object *)
                                    DoMethod(idata->SWA_NPostList_ListCol,
                                             SWM_ListCol_Popup,
                                             '-', GetS(SWS_H_ListCol)),
                                 Child, (Object *)
                                    DoMethod(idata->SWA_NPostList_ListCol,
                                             SWM_ListCol_ClipBt,
                                             'c', GetS(SWS_H_Clip)),
                                 End), NULL));

   if (!to) return FAILURE;

   DoMethod(o, OM_ADDMEMBER,
            idata->SWA_NList_LV = ListviewObject,
            MUIA_CycleChain,           1,
            MUIA_ShortHelp,            GetS(SWS_H_List),
            MUIA_Listview_Input,       TRUE,
            MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
            MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
            MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
            InputListFrame,
            SWA_ObjList_Object,       o,
            MUIA_List_Format,         DoMethod(idata->SWA_NPostList_ListCol,
                                               SWM_ListCol_BuildFmt),
            MUIA_List_DragSortable,  TRUE,
            MUIA_List_Title,         TRUE,
            MUIA_List_DisplayHook,   &idata->DspHook,
            MUIA_List_DestructHook,  &idata->DstHook,
            MUIA_List_CompareHook,   &idata->CmpHook,
            End,
          End);

   if (!idata->SWA_NList_LV) return FAILURE;

   SWM_SetListColNotify(idata->SWA_NPostList_ListCol, idata->SWA_NList_L);

   DoMethod(idata->SWA_NList_L, MUIM_Notify,
            MUIA_List_Entries, MUIV_EveryTime,
            to, 4, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_L_TopTitle), MUIV_TriggerValue);

   idata->SWA_AppBase_TopObj = GetTagObj(SWA_AppBase_TopObj, NULL, ti);

   if (idata->SWA_NPostList_Saved) {
      DoMethod(o, OM_ADDMEMBER,
               g = VGroup,
                  MUIA_Background, MUII_GroupBack,
                  Child, ColGroup(2),
                     Child, Label2(GetS(SWS_L_Subj)),
                     Child, s_Subj  = MkTO(),
                     Child, Label2(GetS(SWS_L_Grp)),
                     Child, s_Grp  = MkTO(),
                  End,
                  Child, HGroup,
                     MUIA_Group_SameSize, TRUE,
                     Child, b_Xfer   = MkBt(SWS_B_MoveToOut, SWS_H_MoveToOut),
                     Child, b_Del    = MkBt(SWS_B_Del,       SWS_H_Del),
                     Child, b_Edit   = MkBt(SWS_B_Edit,      SWS_H_Edit),
                     Child, b_SelAll = MkBt(SWS_B_SelAll,    SWS_H_SelAll),
                  End,
               End);

      s_SubjGAttr = MUIA_Text_Contents;
      s_GrpGAttr  = MUIA_Text_Contents;

      LDoM(o, idata, b_Edit, SWM_NPost_Edit, TRUE);

      DoMethod(idata->SWA_NList_LV, DCLK,
               o, 7, SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 2,
               SWM_NPost_Edit, TRUE);
   } else {
      DoMethod(o, OM_ADDMEMBER,
               g = VGroup,
                  MUIA_Background, MUII_GroupBack,
                  Child, ColGroup(2),
                     Child, Label2(GetS(SWS_L_Subj)),
                     Child, s_Subj  = StringObject,
                                        StringFrame,
                                        MUIA_CycleChain,        1,
                                        MUIA_HorizWeight,    5000,
                                        MUIA_String_MaxLen,  8192,
                                      End,
                     Child, Label2(GetS(SWS_L_Grp)),
                     Child, idata->GLPop = NewObject(NGListClass(), NULL,
                                                     SWA_AppBase_TopObj,
                                                       idata->SWA_AppBase_TopObj,
                                                     SWA_NGList_HelpL,
                                                       GetS(SWS_NG_H_Left),
                                                     SWA_NGList_HelpR,
                                                       GetS(SWS_NG_H_Right),
                                                     TAG_DONE),
                  End,
                  Child, HGroup,
                     MUIA_Group_SameSize, TRUE,
                     Child, b_Add         = MkBt(SWS_B_Add,       SWS_H_Add),
                     Child, b_Del         = MkBt(SWS_B_Del,       SWS_H_Del),
                     Child, b_Edit        = MkBt(SWS_B_Edit,      SWS_H_Edit),
                     Child, idata->b_Post = MkBt(SWS_B_Post,      SWS_H_Post),
                     Child, b_SelAll      = MkBt(SWS_B_SelAll,    SWS_H_SelAll),
                     Child, b_Xfer        = MkBt(SWS_B_MoveToSent,SWS_H_MoveToSent),
                  End,
               End);

                                      
      s_Grp  = SWGetObj(idata->GLPop, SWA_NGList_Str);

      LDoM(o, idata, b_Edit, SWM_NPost_Edit, FALSE);

      DoMethod(idata->SWA_NList_LV, DCLK,
               o, 7, SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 2,
               SWM_NPost_Edit, FALSE);
   }

   s_SubjAttr  = SWA_NPost_Subj;
   s_GrpAttr   = SWA_NPost_Grp;
   
   if (!g) return FAILURE;

   DoMethod(b_SelAll, NOTP, idata->SWA_NList_L, 4,
            MUIM_List_Select, MUIV_List_Select_All, MUIV_List_Select_On, NULL);

   DoMethod(b_Xfer, NOTP, o, 1, SWM_NPostList_XferSel);
   
   if (b_Add) {
      DoMethod(b_Add,   NOTP, o, 2, SWM_NList_Add, NULL);
      DoMethod(b_Add,   NOTP, MUIV_Notify_Window, 3,
               MUIM_Set, MUIA_Window_ActiveObject, s_Subj);
   }

   LDoM(o, idata, b_Del, SWM_NPost_Delete, 0);
   DoMethod(b_Del,   NOTP, o, 1, SWM_NList_RemoveSel);

   if (idata->b_Post) {
      DoMethod(idata->b_Post, NOTP, o, 1, SWM_NPostList_Post);
   }

   if (s_Subj) {
      SetAttrs(s_Subj,
               MUIA_Disabled,              TRUE,
               MUIA_String_AttachedList,   idata->SWA_NList_LV,
               TAG_DONE);
   }

   if (s_Grp) {
      SetAttrs(s_Grp,
               MUIA_Disabled,              TRUE,
               MUIA_String_AttachedList,   idata->SWA_NList_LV,
               TAG_DONE);
   }

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   DoMethod(o, SWM_NPostList_OnOffLine,
            SWGetUL(NN_TCPList(idata), SWA_NTCPList_Online));

   DoMethod(NN_TCPList(idata), MUIM_Notify, SWA_NTCPList_Online,
            MUIV_EveryTime, o,2,SWM_NPostList_OnOffLine, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
            o, 2, SWM_NPostList_AppMessage, MUIV_TriggerValue);

   return SUCCESS;
}

static void SWNListUserSet(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data);

static void SetTop(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (Tag == SWA_AppBase_TopObj) {
      idata->SWA_AppBase_TopObj     = (Object *)Data, 

      DoMethod(o, SWM_NList_ListSet, Tag, Data);
   }

   SWNListUserSet(o, idata, Tag, Data);
}

# define SW_USERSET SetTop(o, idata, Tag, Data)

# define SETUPCUST                                                \
         if (CreateListCol((Object *)retval, idata) != SUCCESS || \
             CreateObj((Object *)retval, idata, ti) != SUCCESS)   \
            SWDB_RET(NULL);

# define DISPOSECUST \
   DoMethod(NN_TCPList(idata), MUIM_KillNotifyObj, SWA_NTCPList_Online, o);

# define SW_USERSETUP NGListInit(libbase)

# define W_USERCLEANUP NGListExit();

// -- info for list clicking to set attrs ------------------------------------

#  define SW_Methods                       \
   SW_DO_MF(SWM_NPostList_Post);           \
   SW_DO_MF(SWM_NPostList_Post_P1);        \
   SW_DO_MF(SWM_NPostList_Post_P2);        \
   SW_DO_MF(SWM_NPostList_Abort);          \
   SW_DO_MF(SWM_NPostList_OnOffLine);      \
   SW_DO_MF(SWM_NPostList_AppMessage);     \
   SW_DO_MF(SWM_NPostList_GenReply);       \
   SW_DO_MF(SWM_NPostList_XferPost);       \
   SW_DO_MF(SWM_NPostList_XferSel);        \
   SW_DO_MF(SWM_NPostList_RemArch);        \
   SW_DO_MF(SWM_NPostList_NGClass);        \
   SW_DO_MF(SWM_ObjList_DragQuery);        \
   SW_DO_MF(SWM_ObjList_DragDrop);         \
   SW_DO_MF(SWM_AppBase_MenuCall);         \
   SW_DO_MF(SWM_AppBase_RexxCmd);          \


# include "S-Lists-Mthd.h"
# include "C-NPostList-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
