# include "C-NArticleList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NTCP

// -- attribute map ----------------------------------------------------------

static STRPTR LongLabs[16];

static STRPTR HEntries[] = {
   SWF_NAHeader_Subject,   //  0
   SWF_NAHeader_From,      //  1
   SWF_NAHeader_Groups,    //  2
   SWF_NAHeader_Date,      //  3
   SWF_NAHeader_Ref,       //  4
   SWF_NAHeader_Keywords,  //  5
   SWF_NAHeader_MsgID,     //  6
   SWF_NAHeader_X_NewsRd,  //  7
   SWF_NAHeader_MimeVer,   //  8
   SWF_NAHeader_MimeType,  //  9
   SWF_NAHeader_Followup,  // 10
   SWF_NAHeader_Organiz,   // 11
   "------------------",   // 12
   "All Headers",          // 13
   "Cached Bodies",        // 14
   "Hdrs+Cached Bodies",   // 15
   NULL,
};

static STRPTR ColSpecs[] = {
   "COL=0 MIW=3 MAW=3 DELTA=0",
   "COL=1 P=" MUIX_R " MIW=5 MAW=5 BAR",
   "COL=2 P=" MUIX_R " MIW=4 MAW=4 BAR",
   "COL=3 MIW=10 MAW=15 BAR",
   "COL=4 MIW=10 MAW=10 BAR",
   "COL=5 MIW=6 MAW=6 BAR",
   "COL=6 P=" MUIX_R " MIW=4 MAW=4 BAR",
   "COL=7 MIW=100 MAW=100 BAR",
   "COL=8 P=" MUIX_R " MIW=8 MAW=8 BAR",
   "COL=9 MIW=10 MAW=10 BAR",
   NULL,
};

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NArticleList_Title,         SWAM_NoLab,   SWAM_SGIN, NULL, 128),
   SWAM_IN(SWA_NArticleList_QSH,           SWAM_NoLab,   SWAM_SGIN,    0),
   SWAM_BO(SWA_NArticleList_QSUpdDisp,     SWAM_NoLab,   SWAM_SGIN, TRUE),
   SWAM_BO(SWA_NArticleList_QSFromRam,     SWAM_NoLab,   SWAM_SGIN, FALSE),

   SWAM_SRT(SWA_NArticleList_Sort1, Attrs, 0, SWAM_Int,  SWAM_SGI,     0),
   SWAM_SRT(SWA_NArticleList_Dir1,  Dirs,  0, SWAM_Bool, SWAM_SGI,     0),
   SWAM_SRT(SWA_NArticleList_Sort2, Attrs, 1, SWAM_Int,  SWAM_SGI,     0),
   SWAM_SRT(SWA_NArticleList_Dir2,  Dirs,  1, SWAM_Bool, SWAM_SGI,     0),
   SWAM_SRT(SWA_NArticleList_Sort3, Attrs, 2, SWAM_Int,  SWAM_SGI,     0),
   SWAM_SRT(SWA_NArticleList_Dir3,  Dirs,  2, SWAM_Bool, SWAM_SGI,     0),

   SWAM_PT(SWA_NArticleList_ToolBar,       SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_IN(SWA_NArticleList_ClickAttr,     SWAM_NoLab,   SWAM_SGIN,    0),
   SWAM_PT(SWA_NArticleList_ClickItem,     SWAM_NoLab,   SWAM_SGIN,    0),
   SWAM_PT(SWA_NArticleList_MemPool,       SWAM_NoLab,   SWAM_SGIN,    0),
   SWAM_PT(SWA_NArticleList_ThreadMgr,     SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_BO(SWA_NArticleList_Threaded,      SWAM_NoLab,   SWAM_SGIN, FALSE),

   SWAM_FI(SWA_NArticleList_ColNames,      SWAM_NoLab,   SWAM_SGIN, LongLabs),
   SWAM_FI(SWA_NArticleList_ColSpecs,      SWAM_NoLab,   SWAM_SGIN, ColSpecs),
   SWAM_FI(SWA_NArticleList_HEntries,      SWAM_NoLab,   SWAM_SGIN, HEntries),

   SWAM_PT(SWA_NArticleList_ListCol,       SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_PT(SWA_NArticleList_Group,         SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_PT(SWA_NArticleList_SortBt,        SWAM_NoLab,   SWAM_SGIN, NULL),

   SWAM_BO(SWA_NArticleList_NoUI,          SWAM_NoLab,   SWAM_SGIN, FALSE),
   
   SWP_NNews_Attrs,
   SWP_NList_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_C "%ld Article(s)",
   MUIX_I "Unknown",
   MUIX_I "?",
   "",
   MUIX_U MUIX_B "Lines",
   MUIX_U MUIX_B "Grp",
   MUIX_U MUIX_B "Auth",
   MUIX_U MUIX_B "Date",
   MUIX_U MUIX_B "Time",
   MUIX_U MUIX_B "Repl",
   MUIX_U MUIX_B "Subject",
   MUIX_U MUIX_B "Size",
   MUIX_U MUIX_B "Age",

   "Status Icon",
   "# Lines",
   "# Groups",
   "Author",
   "Posted Date",
   "Posted Time",
   "# Replies",
   "Subject",
   "Size",
   "Age",
   
   "First Sort",
   "Second Sort",
   "Third Sort",
   "Reverse Sort:",
   
   "Copy the selected entries to the\n"
      "clipboard as text.  Hotkey: 'C'",

   "Change the list columns.  Hotkey: '^'",

   NULL,
};

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NArticle_NewState,
   NULL
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

# include "C-NArticleList-Hooks.h"
# include "S-Lists-Hooks.h"


/*****************************************************************************
 * Function: create a single sort radio group                                *
 *****************************************************************************/

static Object *MakeSortEntry(Object *o, ULONG title, Object *art,
                             ULONG rattr, ULONG dattr)
{
   ULONG sort;

   Object *r, *d, *g = VGroup,
      GroupFrameT(GetS(title)),
      Child, HGroup,
          Child, HSpace(0),
          Child, Label2(GetS(SWS_L_SortRev)),
          Child, d = CheckMark(SWGetUL(o, dattr)),
      End,
      Child, r = RadioObject,
         MUIA_CycleChain,    1,
         MUIA_Radio_Entries, DoMethod(art, SWM_NArticle_Cmp, NULL),
         MUIA_Radio_Active,  sort = SWGetUL(o, rattr),
      End,
   End;

   if (!g) return NULL;

   set(d, MUIA_CycleChain, 1);

   DoMethod(r, MUIM_Notify, MUIA_Radio_Active, MUIV_EveryTime,
            o, 3, MUIM_Set, rattr, MUIV_TriggerValue);

   DoMethod(r, MUIM_Notify, MUIA_Radio_Active, MUIV_EveryTime,
            d, 3, MUIM_Set, MUIA_Disabled, FALSE);

   DoMethod(d, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, dattr, MUIV_TriggerValue);

   DoMethod(r, MUIM_Notify, MUIA_Radio_Active, SWV_NArticle_SortRef,
            d, 3, MUIM_NoNotifySet, MUIA_Selected, FALSE);
   
   DoMethod(r, MUIM_Notify, MUIA_Radio_Active, SWV_NArticle_SortRef,
            d, 3, MUIM_NoNotifySet, MUIA_Disabled, TRUE);

   DoMethod(o, MUIM_Notify, rattr, MUIV_EveryTime, o, 1, MUIM_List_Sort);
   DoMethod(o, MUIM_Notify, dattr, MUIV_EveryTime, o, 1, MUIM_List_Sort);

   set(d, MUIA_Disabled, sort == SWV_NArticle_SortRef);

   return g;
}


static ULONG CreateListmgr(Object *o, SW_IDATA *idata)
{
   Object *g;
   Object *art;
   ULONG x, rc = FAILURE;
   STRPTR Fmt;

   if (!idata->SWA_NArticleList_NoUI) {
      art = MUI_NewObject(SWC_NArticle, TAG_DONE);
      if (!art) goto Done;
   }

   for (x=0; x<=(SWS_L_LongTitleLast - SWS_L_LongTitle0); x++) 
      LongLabs[x] = GetS(SWS_L_LongTitle0 + x);

   LongLabs[x] = NULL;
   
   if (!idata->SWA_NArticleList_NoUI) {
      SetAttrs(o, MUIA_Group_Horiz, FALSE,
               MUIA_Background,  MUII_GroupBack,
               TAG_DONE);

      DoMethod(o, OM_ADDMEMBER,
               g = HGroup,
                  MUIA_Group_HorizSpacing, 3,
                  TextFrame,
                  MUIA_Background, MUII_TextBack,
                  MUIA_VertWeight, 0,
                  Child, idata->Title = TextObject,
                     MUIA_Text_PreParse, MUIX_L,
                  End,
                  Child, HSpace(0),
                  Child, idata->SWA_NArticleList_ToolBar ?
                         idata->SWA_NArticleList_ToolBar : HSpace(0),
                  Child, PopobjectObject,
                     MUIA_Popstring_Button, idata->SWA_NArticleList_SortBt,
                     MUIA_Popobject_Object, HGroup,
                        GroupFrame,
                        MUIA_Background, MUII_GroupBack,
                        Child, MakeSortEntry(o, SWS_L_Sort1, art,
                                             SWA_NArticleList_Sort1,
                                             SWA_NArticleList_Dir1),
                        Child, MakeSortEntry(o, SWS_L_Sort2, art,
                                             SWA_NArticleList_Sort2,
                                             SWA_NArticleList_Dir2),
                        Child, MakeSortEntry(o, SWS_L_Sort3, art,
                                             SWA_NArticleList_Sort3,
                                             SWA_NArticleList_Dir3),
                     End,
                  End,
                  Child, idata->SWA_NArticleList_ListCol ? (Object *)
                          DoMethod(idata->SWA_NArticleList_ListCol,
                                   SWM_ListCol_Popup, '^', GetS(SWS_H_ListCol)) :
                         HSpace(0),
                  Child, idata->SWA_NArticleList_ListCol ? (Object *)
                          DoMethod(idata->SWA_NArticleList_ListCol,
                                   SWM_ListCol_ClipBt, 'C', GetS(SWS_H_Clip)) :
                         HSpace(0),
               End);

      if (!g) goto Done;

      Fmt = (STRPTR)DoMethod(idata->SWA_NArticleList_ListCol, SWM_ListCol_BuildFmt);

      DoMethod(o, OM_ADDMEMBER,
               idata->SWA_NList_LV = ListviewObject,
               MUIA_CycleChain,              1,
               MUIA_Listview_DefClickColumn, SWV_NArticleList_KeyCC,
               MUIA_Listview_Input,          TRUE,
               MUIA_Listview_MultiSelect,    MUIV_Listview_MultiSelect_Default,
               MUIA_Listview_DragType,       MUIV_Listview_DragType_Immediate,
               MUIA_Listview_List,           idata->SWA_NList_L = ObjListObject,
               InputListFrame,
               SWA_ObjList_Object,           o,
               MUIA_List_Format,             Fmt,
               MUIA_List_DragSortable,       FALSE,
               MUIA_List_Title,              TRUE,
               MUIA_List_DisplayHook,        &idata->DspHook,
               MUIA_List_CompareHook,        &idata->CmpHook,
               MUIA_List_DestructHook,       &idata->DstHook,
               End,
             End);

      if (!idata->SWA_NList_LV) goto Done;

      DoMethod(idata->SWA_NList_L, MUIM_Notify, MUIA_List_Entries, MUIV_EveryTime,
               o, 2, SWM_NArticleList_ChangeTitle, MUIV_TriggerValue);

      DoMethod(o, MUIM_Notify, SWA_NArticleList_Title, MUIV_EveryTime,
               o, 2, SWM_NArticleList_ChangeTitle, 0);

      DoMethod(idata->SWA_NList_LV, MUIM_Notify, MUIA_Listview_ClickColumn,
               MUIV_EveryTime, o, 2, SWM_NArticleList_Click, MUIV_TriggerValue);

      SetAttrs(idata->SWA_NArticleList_ListCol,
               SWA_ListCol_Hook, &idata->DspHook,
               SWA_ListCol_List, o,
               TAG_DONE);

      SWM_SetListColNotify(idata->SWA_NArticleList_ListCol, o);
   }
   
   rc = SUCCESS;
   
 Done:
   if (!idata->SWA_NArticleList_NoUI) {
      MUI_DisposeObject(art);
   }
   
   return rc;
}

#define SW_SET

static void SWNListUserSet(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data);

static __asm ULONG SWSet(SETFNPROTO)
   SDST(ULONG)
{
   ULONG rc;
   SW_IDATA *idata = INST_DATA(cl, o);
   
   rc = SWSetAttr(SW_ATTRMAP, (UBYTE *)idata, Tag, Data);

   if (Tag == SWA_NArticleList_Sort1 ||
       Tag == SWA_NArticleList_Sort2 ||
       Tag == SWA_NArticleList_Sort3) {
      idata->Threaded = (idata->CmpInfo.Attrs[0] == SWV_NArticle_SortRef ||
                         idata->CmpInfo.Attrs[1] == SWV_NArticle_SortRef ||
                         idata->CmpInfo.Attrs[2] == SWV_NArticle_SortRef);
   }

   SWNListUserSet(o, idata, Tag, Data);

   SWDB_RET(rc);
} SDFT

# define SW_SETUP_POSTSET                                            \
            if (CreateListmgr((Object *)retval, idata) != SUCCESS)   \
               SWDB_RET(NULL);                                       \
            InitCmpInfo(idata);

# define DISPOSECUST                            \
   if (!idata->SWA_NArticleList_NoUI) {         \
      DoMethod(idata->SWA_NArticleList_ListCol, \
               SWM_ListCol_RemNotify);          \
   }


# define SW_Methods                                       \
  SW_DO_MF(SWM_NArticleList_SrchCmp);                     \
  SW_DO_MF(SWM_NArticleList_Click);                       \
  SW_DO_MF(SWM_NArticleList_ChangeTitle);                 \
  SW_DO_MF(SWM_NArticleList_CtxCenter);                   \
  SW_DO_MF(SWM_NArticleMgr_DelArts);                      \
  SW_DO_MF(MUIM_Setup);                                   \
  SW_DO_MF(MUIM_Cleanup);                                 \

# include "C-NArticleList-Mthd.h"
# include "S-Lists-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
