# include "C-NGroupList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NTCP

// -- group menus ------------------------------------------------------------

static struct NewMenu GroupMenu[] =
{
   { NM_TITLE, (STRPTR)SWS_M_Groups,    0 ,0,0,(APTR)0                      },
   { NM_ITEM,  (STRPTR)SWS_M_ImportNRC, 0 ,0,0,(APTR)(SWS_M_ImportNRC+MOff) },
   { NM_ITEM,  (STRPTR)SWS_M_AllCache,  0 ,0,0,(APTR)(SWS_M_AllCache+MOff)  },
   
   { NM_END,   (STRPTR)NULL,            0, 0,0,(APTR)0                      },
};


// -- attribute map ----------------------------------------------------------

# define Min2T(x) ((x)*60)
# define Hr2T(x) (Min2T(x)*60)

# define UI (SWAM_UI|SWAM_Notify)

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_PT(SWA_NGroupList_MemPool,      SWAM_NoLab,    SWAM_SGIN, NULL),
   SWAM_BO(SWA_NGroupList_FullNames,    SWS_L_FNames,  UI,   FALSE),
   SWAM_SA(SWA_NGroupList_LastFetchPat, SWAM_NoLab,    SWAM_SGI, "*", 1024),

   SWAM_BO(SWA_NGroupList_UpdOpen,   SWS_L_UOpen,   UI, FALSE),
   SWAM_TX(SWS_L_UpdInt, NULL),
   SWAM_BO(SWA_NGroupList_Upd,       SWAM_NoLab,    UI|SWAM_NewHG,   FALSE),
   SWAM_IB(SWA_NGroupList_UpdInt,    SWAM_NoLab,
           UI | SWAM_TimeSld | SWAM_TimeSec | SWAM_EndG,
           Min2T(20), Min2T(1), Hr2T(2)),
   SWAM_IB(SWA_NGroupList_SGUpdateInt, SWS_L_SGUpdInt, UI, 100, 10, 50000),
   SWAM_SA(SWA_NGroupList_SGPatt,    SWS_L_SGPatt, UI, "",
           SWV_NGroupList_SG_MaxLineLen),

   SWAM_SRT(SWA_NGroupList_Sort1, Attrs, 0, SWAM_Int,  SWAM_SGI, 0),
   SWAM_SRT(SWA_NGroupList_Dir1,  Dirs,  0, SWAM_Bool, SWAM_SGI, 0),
   SWAM_SRT(SWA_NGroupList_Sort2, Attrs, 1, SWAM_Int,  SWAM_SGI, 0),
   SWAM_SRT(SWA_NGroupList_Dir2,  Dirs,  1, SWAM_Bool, SWAM_SGI, 0),
   SWAM_SRT(SWA_NGroupList_Sort3, Attrs, 2, SWAM_Int,  SWAM_SGI, 0),
   SWAM_SRT(SWA_NGroupList_Dir3,  Dirs,  2, SWAM_Bool, SWAM_SGI, 0),

   SWAM_OB(SWA_NGroupList_ListCol,    SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB(SWA_NGroupList_BatchDLFlt, SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_SA(SWA_NGroupList_LastSGDate, SWAM_NoLab, SWAM_SGI, InitSGDate, 64),

   SWAM_PT(SWA_NGroupList_SG_L,      SWAM_NoLab,    SWAM_SGIN, NULL),
   SWAM_PT(SWA_NGroupList_SG_LV,     SWAM_NoLab,    SWAM_SGIN, NULL),

   SWP_NNews_Attrs,
   SWP_NList_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

# undef UI

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_I        "Unnamed",
   MUIX_I        "Unset",
   MUIX_I        "Never",
   MUIX_I        "Default",
   MUIX_U MUIX_B "Group Name",
   MUIX_U MUIX_B "Server",
   MUIX_U MUIX_B "New",
   MUIX_U MUIX_B "Avail",
   MUIX_U MUIX_B "Group",
   MUIX_U MUIX_B "Unread",
   MUIX_U MUIX_B "Read",
   MUIX_U MUIX_B "Description",
   MUIX_U MUIX_B "Mod",
   MUIX_U MUIX_B MUIX_R "Last",
   MUIX_U MUIX_B MUIX_L "Read",
   MUIX_U MUIX_B "Status Icon",
   MUIX_U MUIX_B "Last Read",
   
   MUIX_C "Subscribed Groups (%ld)",
   "Online Mode",
   "Offline Mode",
   "Group Commands",
   "Group %%ld of %ld: %s",
   "Group Search",
   "Server Groups Upd Interval: ",
   "Server Groups Pattern: ",

   "Yes",
   "No",

   "_Cancel",
   "_Add Group",
   "_Delete",
   "Sort",
   "Se_lect All",
   "Show _Groups...",

   "_Read Group...",
   "Mark as Read",
   "Mark as Unread",
   "_Update Count",
   "Read Description",
   "Batch D/L Over_view...",
   "Batch D/L Ar_ticles...",
   "_Fetch",
   "_Clear",
   "_Search...",
   "Abort",
   "S_ort",
   "Sa_ve",
   "_Load",
   "S_ubscribe",
   "Select _All",
   "Show _New",
   "_Browse",
   "_On",
   "_Off",

   "This is a list of subscribed newsgroups.\n"
      "each newsgroup can potentially be obtained\n"
      "from a different server.",
   "This is a list of groups on the default server.\n"
      "Selected groups may be subscribed to by using\n"
      "the " SWX_I("Subscribe") " button or by dragging groups\n"
      "from this list to the subscribed group list.\n"
      "(To drag from a multiselect list, drag the list\n"
      "items sideways out of the list).",
   "Add a new group to the list.",
   "Remove selected groups from the list.",
   "Sort newsgroups by the selected sort criteria.",
   "Select all groups in the list.",
   "View groups available on server.",
   "Open the group reader window.",
   "Mark all current articles in the selected\n"
      "groups as read.  This will not download\n"
      "and mark any new articles on the server,\n"
      "only articles already fetched.",
   "Mark all current articles in the selected\n"
      "groups as unread.",
   "Search for and select groups\n"
      "matching a pattern.  Hotkey: '/'",
   "Update count of new and available messages\n"
      "on the server for the selected groups.  Due\n"
      "to the nature of netnews, these numbers are\n"
      "very approximate.",
   "Read the selected groups' descriptions",
   "Download article overviews from the selected\n"
      "groups.  Article overviews can also be read\n"
      "automatically from group reader windows, but\n"
      "here more than one can be done at once.\n"
      "Overview download is done asynchronously, so\n"
      "you can continue to read news while it happens.",
   "Download article " SWX_B("text") " from the selected\n"
      "groups.  This will allow articles to be read\n"
      "off line at a later date.  Note that this this\n"
      "may take a long time!  However, it is done\n"
      "asynchronously, so you can continue to read\n"
      "news while it happens.",
   "Fetch groups matching the pattern from\n"
      "the default server.  The pattern is parsed\n"
      "on the server and does " SWX_B("not") " support the\n"
      "full AmigaOS pattern syntax.  If there are\n"
      "many matching articles, this operation may\n"
      "take some time.",
   "Groups matching this pattern will be fetched.  The\n"
      "pattern is parsed on the server and does " SWX_B("not") "\n"
      "support the full AmigaOS pattern syntax.",
   "Clear the list of groups.",
   "Search for groups.",
   "Abort a running transfer.",
   "Sort the list of groups.",
   "Save the server group list to a file.",
   "Load the server group list from a file.\n"
      "This can also load " SWX_I(".newsrc") " files,\n"
      "for example from tin.",
   "Subscribe to the selected groups.",
   "Select all groups in the list.",
   "Show the groups that are new since the\n"
     "previous time this option was used.",
   "Browse a group on a temporary basis\n"
     "without subscribing to it.",
   "Select the sort criteria for\n"
      "the group list.  Hotkey: '='",
   "Change to online mode.",
   "Change to offline mode.",
   "Display the list of available FAQs\n"
      "for the selected group.  Hotkey: 'Q'",
   "The update interval for the server groups\n"
      "window.  For example, if this is set to\n"
      "200, then the window will update with each\n"
      "200 group names that are fetched.",
   "If non-empty, this is a pattern which server\n"
      "groups must match to be displayed.  You might\n"
      "use this, for example, to avoid seeing groups\n"
      "beginning with 'it.*' if you cannot speak italian.",

   "The demo version of this program supports\n"
      "only 2 groups.  Please support the commercial\n"
      "version of this software, which supports an\n"
      "unlimited number of groups.",

   "_Delete OV|_Keep OV|*_Cancel|_Ask for Each",
   SWX_B("Warning: ") "This action will delete the overview\n"
      "information for %ld %s.  All information associated\n"
      "with the %s (all cached articles, read/unread state,\n"
      "etc.) will be lost.",
   "group",
   "groups",
   "Warning!",

   "Groups Available on Server",
   "Downloading Server Groups...",
   "Search for Server Groups",
   MUIX_C "Server Groups (%ld)",
   "Save Server Group List",
   "Load Server Group List",
   "Cmds",
   "D/L Headers",
   "D/L Articles",

   "Save Cancelled",
   "Load Cancelled",
   "*_OK",

   "Subscribe request cancelled.",
   "Note!",
   SWX_B("Note: ") "You have selected quite a few groups (%ld) to\n"
      "subscribe to.  This is probably not a good\n"
      "idea; only groups you are interested in\n"
      "should be subscribed to.  Nonetheless, you\n"
      "may proceed if you wish.",
   "_Proceed|*_Cancel",
   "*_OK",

   "Full Names: ",
   "If set, full newsgroup names are displayed.\n"
      "Otherwise, abbreviations are used.",

   "Update Interval: ",
   "The count of new articles in each group will\n"
      "be updated this often (in online mode) if\n"
      "this option is turned on.",

   "Update on Start: ",
   "If selected, the count of new articles in each\n"
      "group will be updated when the group list\n"
      "window is first opened.",

   "Subscribed Groups",
   "Options",
   "Batch D/L Filters",

   "This filter list is used when batch downloading\n"
      "articles.  Only articles that match filters\n"
      "in this list will be selected for downloading.\n"
      "filters can be made to apply to only certain\n"
      "groups by using the group pattern.",

   "First Sort",
   "Second Sort",
   "Third Sort",
   "Reverse Sort:",
      
   "Groups",
      "Import .newsrc",
      "All in Cache Dir",

   "Select which columns should be displayed\n"
      "in the group list.  Hotkey: '-'",
   "Copy the selected entries to the\n"
      "clipboard as text.  Hotkey: 'c'",
      

   SWX_B("Error: ") "Unable to save server group list to:\n%s",
   SWX_B("Error: ") "Unable to load server group list from:\n%s",
   SWX_B("Error: ") "Not a valid group list file:\n%s",
   SWX_B("Error: ") "out of memory",
   SWX_B("Error: ") "No HOME environment variable.",
   SWX_B("Error: ") "Unable to scan cache directory.",
   SWX_B("Note: ")  "%ld groups were already subscribed to.",
   SWX_B("Note: ")  "No groups are selected.",
         
   NULL,
};

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NGroup_BadGroup,
   SWA_NGroup_Name,
   SWA_NGroup_Server1,
   SWA_NGroup_AvailArtNum,
   SWA_NGroup_NewArtNum,
   SWA_NGroup_ReadArtCnt,
   SWA_NGroup_TotArtCnt,
   SWA_NGroup_RdDays,
   SWA_NGroup_RdMin,
   SWA_NGroup_ReaderOpen,
   SWA_NGroup_Desc,
   SWA_NGroup_PermFlags,
   SWA_NGroup_Moderated,
   NULL
};

static HelpMap SW_HELPMAP[] = {
   SWA_NGroupList_FullNames,   SWS_H_FNames,
   SWA_NGroupList_UpdInt,      SWS_H_UpdInt,
   SWA_NGroupList_UpdOpen,     SWS_H_UOpen,
   SWA_NGroupList_SGUpdateInt, SWS_H_SGUpdInt,
   SWA_NGroupList_SGPatt,      SWS_H_SGPatt,
   NULL, NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

# include "C-NGroupList-Hooks.h"
# include "S-Lists-Hooks.h"

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l), GetS(h)); }


/*****************************************************************************
 * Function: create search object                                            *
 *****************************************************************************/

static Object *CreateGroupSrch(Object *o, SW_IDATA *idata)
{
   return
      MUI_NewObject(SWC_ListSearch,
                    SWA_ListSearch_Title,        GetS(SWS_L_GroupSrch),
                    SWA_ListSearch_List,         idata->SWA_NList_L,
                    SWA_ListSearch_SearchObj,    o,
                    SWA_ListSearch_CmpMethod,    SWV_ListSearch_ObjAttr,
                    SWA_ListSearch_ObjAttr,      SWA_NGroup_Name,
                    SWA_ListSearch_Dynamic,      TRUE,
                    SWA_ListSearch_First,        TRUE,
                    SWA_ListSearch_WindowID,     MAKE_ID('G','l','S','r'),
                    SWA_ListSearch_ItemName,     GetS(SWS_L_Group),
                    SWA_ListSearch_ItemNamePl,   GetS(SWS_L_Groups),
                    TAG_DONE);
}


/*****************************************************************************
 * Function: create a single sort radio group                                *
 *****************************************************************************/

static Object *MakeSortEntry(Object *o, ULONG title, Object *grp,
                             ULONG rattr, ULONG dattr)
{
   Object *r, *d, *g = VGroup,
      GroupFrameT(GetS(title)),
      Child, HGroup,
          Child, HSpace(0),
          Child, Label2(GetS(SWS_L_SortRev)),
          Child, d = CheckMark(SWGetUL(o, dattr)),
      End,
      Child, r = RadioObject,
         MUIA_CycleChain,    1,
         MUIA_Radio_Entries, DoMethod(grp, SWM_NGroup_Cmp, NULL),
         MUIA_Radio_Active,  SWGetUL(o, rattr),
      End,
   End;

   if (!g) return NULL;

   set(d, MUIA_CycleChain, 1);

   DoMethod(r, MUIM_Notify, MUIA_Radio_Active, MUIV_EveryTime,
            o, 3, MUIM_Set, rattr, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, rattr, MUIV_EveryTime,
            r, 3, MUIM_Set, MUIA_Radio_Active, MUIV_TriggerValue);

   DoMethod(d, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, dattr, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, dattr, MUIV_EveryTime,
            d, 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, rattr, MUIV_EveryTime, o, 1, MUIM_List_Sort);
   DoMethod(o, MUIM_Notify, dattr, MUIV_EveryTime, o, 1, MUIM_List_Sort);

   return g;
}


/*****************************************************************************
 * Function: create an image button object                                   *
 *****************************************************************************/

static Object *MkIB(SW_IDATA *idata, ULONG h, STRPTR Image, LONG Cmd)
{
   return DTImgObject,
      MUIA_InnerTop,        0,
      MUIA_InnerBottom,     0,
      MUIA_InnerLeft,       0,
      MUIA_InnerRight,      0,
      MUIA_Frame,           MUIV_Frame_ImageButton,
      MUIA_InputMode,       MUIV_InputMode_RelVerify,
      MUIA_ControlChar,     Cmd,
      MUIA_ShortHelp,       GetS(h),
      MUIA_CycleChain,      1,
      SWA_DTImg_FSpec,      Image,
      SWA_DTImg_ImgCache,   NN_DTImages(idata),
      SWA_DTImg_Path,       NN_ButtonPath(idata),
   End;
}


static ULONG CreateBatchFlt(Object *o, SW_IDATA *idata)
{
   if (!idata->SWA_NGroupList_BatchDLFlt)
      idata->SWA_NGroupList_BatchDLFlt =
         MUI_NewObject(SWC_NSFList,
                       SWA_NSFList_ShowGroups, TRUE,
                       AppBaseTopObj(o),
                       TAG_DONE);

   if (!idata->SWA_NGroupList_BatchDLFlt) return FAILURE;

   return SUCCESS;
}

/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static ULONG CreateListmgr(Object *o, SW_IDATA *idata)
{
   ULONG  rc = FAILURE;

   CreateBatchFlt(o, idata);

   idata->SWA_NList_LV = ListviewObject,
      MUIA_CycleChain,           1,
      MUIA_Dropable,             TRUE,
      MUIA_Listview_Input,       TRUE,
      MUIA_ShortHelp,            GetS(SWS_H_SubGrpList),
      MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
      MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
      MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
         InputListFrame,
         SWA_ObjList_Object,      o,
         MUIA_List_Format,        DoMethod(idata->SWA_NGroupList_ListCol,
                                           SWM_ListCol_BuildFmt),
         MUIA_List_DragSortable,  TRUE,
         MUIA_List_Title,         TRUE,
         MUIA_List_DisplayHook,   &idata->DspHook,
         MUIA_List_DestructHook,  &idata->DstHook,
         MUIA_List_CompareHook,   &idata->CmpHook,
      End,
   End;

   if (!idata->SWA_NList_LV) goto Done;

   SWM_SetListColNotify(idata->SWA_NGroupList_ListCol, idata->SWA_NList_L);

   set(o, MUIA_Background, MUII_GroupBack);

   DoMethod(NN_TCPList(idata), MUIM_Notify,SWA_NTCPList_Online, MUIV_EveryTime,
            o, 2, SWM_NGroupList_ChangeMode, MUIV_TriggerValue);

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   idata->CmpInfo.Count = 3;

   rc = SUCCESS;
   
 Done:
   return rc;
}


/*****************************************************************************
 * Function: initialize command array data                                   *
 *****************************************************************************/

static void InitCmds(SW_IDATA *idata)
{
   idata->Cmds[SWV_GC_CheckStats].Method = SWM_NGroup_CheckStats;
   idata->Cmds[SWV_GC_ReadDesc].Method   = SWM_NGroup_ReadDesc;
   idata->Cmds[SWV_GC_GL_Hdrs].Method    = SWM_NGroup_GL_Hdrs;
   idata->Cmds[SWV_GC_GL_Arts].Method    = SWM_NGroup_GL_Arts;

   idata->Cmds[SWV_GC_GL_Hdrs].ShowProgWin = TRUE;
   idata->Cmds[SWV_GC_GL_Hdrs].PcAttr    = SWA_NGroup_HdrPctDone;
   idata->Cmds[SWV_GC_GL_Hdrs].WinTitle  = GetS(SWS_T_GL_Hdrs);

   idata->Cmds[SWV_GC_GL_Arts].ShowProgWin = TRUE;
   idata->Cmds[SWV_GC_GL_Arts].PcAttr    = SWA_NGroup_HdrPctDone;
   idata->Cmds[SWV_GC_GL_Arts].WinTitle  = GetS(SWS_T_GL_Arts);
}


/*****************************************************************************
 * Function: Create a new listcol object                                     *
 *****************************************************************************/

static ULONG CreateListCol(Object *o, SW_IDATA *idata)
{
   ULONG x;
   
   static STRPTR ColSpecs[] = {
      "COL=0 W=3",
      "COL=1 W=40 BAR",
      "COL=7 W=10,C=8 W=10 BAR",
      "COL=2 W=20 BAR",
      "COL=3 P=" MUIX_R " W=10 BAR",
      "COL=4 P=" MUIX_R " W=10 BAR",
      "COL=5 P=" MUIX_R " W=10 BAR",
      "COL=6 P=" MUIX_R " W=10 BAR",
      "COL=9 P=" MUIX_R " W=10 BAR",
      "COL=10 W=50 BAR",
      "COL=11 W=5 BAR",
      NULL,
   };
   
   static STRPTR ColNames[13];
   
   ColNames[0]  = GetS(SWS_L_Icon);
   ColNames[1]  = GetS(SWS_L_Name);
   ColNames[2]  = GetS(SWS_L_LastRead);
   ColNames[3]  = GetS(SWS_L_Server);
   ColNames[4]  = GetS(SWS_L_NewCount);
   ColNames[5]  = GetS(SWS_L_AvailCount);
   ColNames[6]  = GetS(SWS_L_TotArtCnt);
   ColNames[7]  = GetS(SWS_L_UnReadArtCnt);
   ColNames[8]  = GetS(SWS_L_ReadArtCnt);
   ColNames[9]  = GetS(SWS_L_Description);
   ColNames[10] = GetS(SWS_L_Moderated);
   ColNames[11] = NULL;

   for (x=0; ColNames[x]; x++)
      ColNames[x] += sizeof(MUIX_U MUIX_B) - 1;

   if (!(idata->SWA_NGroupList_ListCol =
         MUI_NewObject(SWC_ListCol,
                       SWA_ListCol_ColSpecs, ColSpecs,
                       SWA_ListCol_ColNames, ColNames,
                       SWA_ListCol_Hook,     &idata->DspHook,
                       SWA_ListCol_List,     o,
                       TAG_DONE)))
      return FAILURE;

   DoMethod(idata->SWA_NGroupList_ListCol, SWM_ListCol_HideCols, 3, 5, 8, 9, 10, -1);

   return SUCCESS;
}

static void SWNListUserSet(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data);

static void SetTop(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (Tag == SWA_AppBase_TopObj) {
      idata->SWA_AppBase_TopObj     = (Object *)Data, 
      idata->SWA_NGroupList_MemPool = NN_MemPool(idata);

      set(idata->SWA_NGroupList_BatchDLFlt,      Tag, Data);
      DoMethod(o, SWM_NList_ListSet,             Tag, Data);
      DoMethod(idata->SWA_NGroupList_BatchDLFlt, SWM_NList_ListSet, Tag, Data);
    }

   SWNListUserSet(o, idata, Tag, Data);
}

static void AbortGroupListAll(Object *o, SW_IDATA *idata);

/*****************************************************************************
 * Function: Delete browse group                                             *
 *****************************************************************************/

static void DeleteBrowseGrp(Object *o, SW_IDATA *idata)
{
   if (!idata->BrowseGroup) return;
      
   DoMethod(idata->BrowseGroup,
            SWM_NGroup_DeleteOVDir,
            SWT_NGroup_DelOV_NoPrompt, TRUE,
            SWT_NGroup_DelOV_Force,    TRUE,
            TAG_DONE);

   MUI_DisposeObject(idata->BrowseGroup);
   idata->BrowseGroup = NULL;
}


/*****************************************************************************
 * Function: Dispose of this object                                          *
 *****************************************************************************/

static void GLDispose(Object *o, SW_IDATA *idata)
{
   DeleteBrowseGrp(o, idata);
   MUI_DisposeObject(idata->GroupSearch);
   DoMethod(o, SWM_NGroupList_CloseSGWin);
   DoMethod(NN_TCPList(idata), MUIM_KillNotifyObj, SWA_NTCPList_Online, o);
   AbortGroupListAll(o, idata);
   MUI_DisposeObject(idata->b_GoOnLine);
   MUI_DisposeObject(idata->b_GoOffLine);
}

/*****************************************************************************
 * Function: Create a new listcol object                                     *
 *****************************************************************************/

static void CreateObj(Object *o, SW_IDATA *idata)
{
   CreateListCol(o, idata);
}

# define DISPOSECUST GLDispose(o, idata);

# define SETUPCUST         CreateObj((Object *)retval, idata);

# define SW_SETUP_POSTSET                                          \
            if (CreateListmgr((Object *)retval, idata) != SUCCESS) \
               SWDB_RET(NULL);                                     \
            InitCmds(idata);

# define SW_USERSETUP if (FirstTime) \
                         SWLocalizeNewMenu(SW_CATALOG, SW_CATSTR, GroupMenu);

# define SW_USERSET SetTop(o, idata, Tag, Data)

# define SW_Methods                              \
         SW_DO_MF(SWM_AppBase_MenuCall);         \
         SW_DO_MF(SWM_AppBase_RexxCmd);          \
         SW_DO_MF(SWM_AppBase_NewProj);          \
         SW_DO_MF(SWM_NNews_MenuStrip);          \
         SW_DO_MF(SWM_NGroupList_SWin);          \
         SW_DO_MF(SWM_NGroupList_DoGroup);       \
         SW_DO_MF(SWM_NGroupList_DoGroup_I);     \
         SW_DO_MF(SWM_NGroupList_DoGroup_Abort); \
         SW_DO_MF(SWM_NGroupList_ChangeMode);    \
         SW_DO_MF(SWM_NGroupList_Delete);        \
         SW_DO_MF(SWM_NGroupList_OpenSGWin);     \
         SW_DO_MF(SWM_NGroupList_CloseSGWin);    \
         SW_DO_MF(SWM_NGroupList_FetchSG);       \
         SW_DO_MF(SWM_NGroupList_FetchSG_P);     \
         SW_DO_MF(SWM_NGroupList_AbortSG);       \
         SW_DO_MF(SWM_NGroupList_SortSG);        \
         SW_DO_MF(SWM_NGroupList_SaveSG);        \
         SW_DO_MF(SWM_NGroupList_LoadSG);        \
         SW_DO_MF(SWM_NGroupList_SubSG);         \
         SW_DO_MF(SWM_NGroupList_BrowseSG);      \
         SW_DO_MF(SWM_NGroupList_SubNewsRC);     \
         SW_DO_MF(SWM_NGroupList_Subscribe);     \
         SW_DO_MF(SWM_NGroupList_ImportNRC);     \
         SW_DO_MF(SWM_NGroupList_ImportCD);      \
         SW_DO_MF(SWM_NGroupList_UpdCheck);      \
         SW_DO_MF(SWM_NGroupList_ClipSG);        \
         SW_DO_MF(SWM_NGroupList_NewSG);         \
         SW_DO_MF(SWM_ObjList_DragQuery);        \
         SW_DO_MF(SWM_ObjList_DragDrop);         \
         SW_DO_MF(SWM_NNews_NewProj);            \
         SW_DO_MF(MUIM_Setup);                   \
         SW_DO_MF(MUIM_Cleanup);                 \

# include "S-Lists-Mthd.h"
# include "C-NGroupList-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
