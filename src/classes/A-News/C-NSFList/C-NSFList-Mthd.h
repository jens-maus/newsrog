
# define SWM_USERREADPOST DoMethod(o, SWM_NSFilter_Expire)

/*****************************************************************************
 * Method: MUI Setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   if (!DoSuperMethodA(cl, o, msg)) SWDB_RET(FALSE);

   SWMakeImg(idata, SWV_Img_True,  CheckMark(TRUE));
   SWMakeImg(idata, SWV_Img_False, CheckMark(FALSE));

   SWCreateImages(idata);
   
   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: MUI Cleanup                                                       *
 *****************************************************************************/
   
METHOD(MUIM_Cleanup, Msg)
{
   SWDeleteImages(idata);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Begin match tests                                                 *
 *****************************************************************************/

METHOD(SWM_NSFilter_Begin, Msg)
{
   Object *lo;
   LONG Entries, x, y = 0;
   ULONG rc;

   rc = DoSuperMethodA(cl, o, msg);
   
   idata->Total = idata->Found = 0;
   idata->Inv   = SWGetUL(o, SWA_NSFilter_Inv);

   if (Entries = SWGetUL(idata->SWA_NList_L, MUIA_List_Entries))
      if (!(idata->Flt = AllocVec(Entries * sizeof(Object *),
                                  MEMF_ANY|MEMF_CLEAR)))
         rc = FAILURE;
   
   SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
      if (SWGetUL(lo, SWA_NSFilter_Enabled))
         if (DoMethodA(idata->Flt[y++] = lo, msg) != SUCCESS)
            rc = FAILURE;

   idata->Entries = y;

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Determine if article is a match                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_Match, SWP_NSFilter_Match *)
{
   ULONG x, m,
         Match = (idata->Entries == 0) ? idata->SWA_NSFList_DefMatch :
            (idata->SWA_NSFList_Mode == SWV_NSFList_Or ? FALSE : TRUE);

   idata->Total++;

   for (x=0; x<idata->Entries; x++) {
      m = DoMethodA(idata->Flt[x], msg);
   
      if (m && idata->SWA_NSFList_Mode == SWV_NSFList_Or) {
         Match = TRUE;
         break;
      }
   
      if (!m && idata->SWA_NSFList_Mode == SWV_NSFList_And) {
         Match = FALSE;
         break;
      }
   }

  Done:
   if (idata->Inv) Match = !Match;
   if (Match)      idata->Found++;
   
   SWDB_RET(Match);
} SDFT
   

/*****************************************************************************
 * Method: End match tests                                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_End, Msg)
{
   LONG x;

   for (x=0; x<idata->Entries && idata->Flt; x++)
      DoMethodA(idata->Flt[x], msg);
   
   FreeVec(idata->Flt);
   idata->Flt = NULL;
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Update statistics after xfer                                      *
 *****************************************************************************/

METHOD(SWM_NSFilter_TrackStats, SWP_NSFilter_TrackStats *)
{
   *msg->Total = idata->Total;
   *msg->Found = idata->Found;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

static Object *strobj(ULONG h)
{
   return StringObject,
      StringFrame,
      MUIA_CycleChain, 1,
      MUIA_Disabled,   TRUE,
      MUIA_ShortHelp,  GetS(h),
   End;
}

METHOD(SWM_NSFilter_UI, Msg)
{
   Object *clb, *cb, *b_Del, *b_Edit, *b_Mode, *b_Clone, *b_Add, *b_Close,
           *lv, *g;
   TEXT FmtStr[256];
   static STRPTR Modes[3];

   FmtStr[0] = '\0';

   if (SW_STRISNULL(idata->SWA_NSFList_FltType))
      set(o, SWA_NSFList_FltType, "#?NSFilter#?");

   idata->EnableNew =
      (strcmp(idata->SWA_NSFList_FltType, "#?NDFilter#?") == 0);
   
   if (idata->SWA_NSFList_UIMode == SWV_NSFList_UI_Tiny) {
      SNLT_Attr[0] = SWA_NSFilter_Enabled;
      SNLT_Attr[1] = SWA_NSFilter_Inv;
      SNLT_Attr[2] = 0;
      SNLT_Cols[0] = 0,
      SNLT_Cols[1] = 1;

      if (strcmp(idata->SWA_NSFList_FltType, "#?NSFilter#?") == 0)
         strcat(FmtStr, "COL=0,COL=1,COL=2");
      
      if (strcmp(idata->SWA_NSFList_FltType, "#?NDFilter#?") == 0)
         strcat(FmtStr, "COL=0,COL=2");

      if (strcmp(idata->SWA_NSFList_FltType, "#?NAFilter#?") == 0)
         strcat(FmtStr, "COL=0,COL=2");
   } else if (idata->SWA_NSFList_UIMode == SWV_NSFList_UI_Insert) {
      SNLT_Attr[0] = 0;
      strcat(FmtStr, "COL=2 BAR,COL=3");
   } else {
      SNLT_Attr[0] = SWA_NSFilter_Enabled;
      SNLT_Attr[1] = SWA_NSFilter_Inv;
      SNLT_Attr[2] = 0;
      SNLT_Cols[0] = 0,
      SNLT_Cols[1] = 1;
      
      if (strcmp(idata->SWA_NSFList_FltType, "#?NSFilter#?") == 0)
         strcat(FmtStr, "COL=0 W=5 BAR,"
                        "COL=1 W=5 BAR,"
                        "COL=2 W=60 BAR,"
                        "COL=3 W=30 BAR,"
                        "COL=4 P=" MUIX_R " W=5 MAW=5,"
                        "COL=5 P=" MUIX_R " W=5 MAW=5");

      if (strcmp(idata->SWA_NSFList_FltType, "#?NDFilter#?") == 0)
         strcat(FmtStr, "COL=0 W=5 BAR,"
                        "COL=2 W=60 BAR,"
                        "COL=3 W=30 BAR");

      if (strcmp(idata->SWA_NSFList_FltType, "#?NAFilter#?") == 0)
         strcat(FmtStr, "COL=0 W=5 BAR,"
                        "COL=2 W=60 BAR,"
                        "COL=3 W=30 BAR");
   }

   if (idata->SWA_NSFList_ShowGroups)
      strcat(FmtStr, " BAR,COL=6 W=50");

   set(idata->SWA_NList_L, MUIA_List_Format, FmtStr);

   Modes[0] = GetS(SWS_L_Any);
   Modes[1] = GetS(SWS_L_All);
   Modes[2] = NULL;

   if (!idata->SWA_NList_LV)       SWDB_RET(NULL);
   if (!(idata->UI = VGroup, End)) SWDB_RET(NULL);

   DoMethod(idata->UI, OM_ADDMEMBER, o);

   if (idata->SWA_NSFList_UIMode == SWV_NSFList_UI_Normal) {
      DoMethod(idata->UI, OM_ADDMEMBER,
               g = ColGroup(2),
                  Child, Label(GetS(SWS_L_Desc)),
                  Child, s_Desc = strobj(SWS_H_Desc),
               End);

      if (!g) SWDB_RET(NULL);

      if (idata->SWA_NSFList_ShowGroups) {
         DoMethod(g, OM_ADDMEMBER, Label(GetS(SWS_L_Grp)));
         DoMethod(g, OM_ADDMEMBER, s_Grp  = strobj(SWS_H_Grp));
         s_GrpAttr = SWA_NSFilter_Groups;
      }
      
      DoMethod(idata->UI, OM_ADDMEMBER, cb = PopobjectObject,
                  MUIA_Popstring_Button, MkBt(SWS_B_Add, SWS_H_Add),
                  MUIA_Popobject_Object, VGroup,
                     GroupFrame,
                     MUIA_InnerTop,      1,
                     MUIA_InnerBottom,   1,
                     MUIA_InnerLeft,     1,
                     MUIA_InnerRight,    1,
                     MUIA_Group_Spacing, 1,
                     Child, lv = ListviewObject,
                        MUIA_CycleChain,           TRUE,
                        MUIA_Listview_Input,       TRUE,
                        MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
                        MUIA_Listview_List,  idata->CList = clb = ClasslistObject,
                            InputListFrame,
                            SWA_ClassList_Type, idata->SWA_NSFList_FltType,
                        End,
                     End,
                     Child, HGroup,
                        MUIA_Group_SameSize, TRUE,
                        Child, b_Add   = MkBt(SWS_LB_Add,   SWS_LBH_Add),
                        Child, b_Close = MkBt(SWS_LB_Close, SWS_LBH_Close),
                     End,
                  End,
               End);

      if (!cb) SWDB_RET(NULL);

      DoMethod(idata->UI, OM_ADDMEMBER, g = HGroup,
                  MUIA_Group_SameSize, TRUE,
                  Child, b_Edit = MkBt(SWS_B_Edit,  SWS_H_Edit),
                  Child, b_Del  = MkBt(SWS_B_Del,   SWS_H_Del),
                  Child, b_Clone= MkBt(SWS_B_Clone, SWS_H_Clone),
               End);

      if (!g) SWDB_RET(NULL);

      if (strcmp(idata->SWA_NSFList_FltType, "#?NSFilter#?") == 0) {
         DoMethod(g, OM_ADDMEMBER, b_Mode = Cycle(Modes));

         SetAttrs(b_Mode,
                  MUIA_CycleChain,   1,
                  MUIA_Cycle_Active, idata->SWA_NSFList_Mode,
                  MUIA_ShortHelp,    GetS(SWS_H_AnyAll),
                  TAG_DONE);

         DoMethod(b_Mode,  MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
                  o, 3, MUIM_Set, SWA_NSFList_Mode, MUIV_TriggerValue);
      }

      s_DescAttr   = SWA_NSFilter_Desc;

      DoMethod(o, MUIM_MultiSet, MUIA_String_AttachedList, idata->SWA_NList_LV,
               s_Desc, s_Grp, NULL);

      DoMethod(lv, DCLK, MUIV_Notify_Window, 3,
               MUIM_Set, MUIA_Window_ActiveObject, s_Desc);

      DoMethod(lv,    DCLK, o,  3, SWM_NSFList_AddSel, clb, cb);
      DoMethod(b_Add, NOTP, o,  3, SWM_NSFList_AddSel, clb, cb);

      DoMethod(b_Close, NOTP, cb,  2, MUIM_Popstring_Close, TRUE);

      DoMethod(b_Del,   NOTP, o, 1, SWM_NList_RemoveSel);
      DoMethod(b_Clone, NOTP, o, 1, SWM_NList_Clone);

      DoMethod(b_Edit, MUIM_Notify, MUIA_Pressed, FALSE,
               o, 7, SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 2,
               SWM_NSFilter_ShowUI, SWV_ListDoMethod_App);
   
      DoMethod(idata->SWA_NList_LV, DCLK,
               o, 7, SWM_Root_ListDoMethod, idata->SWA_NList_L,
               SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 2,
               SWM_NSFilter_ShowUI, SWV_ListDoMethod_App);

      if (PopMenu && idata->SWA_NSFList_UseCtxMenu) {
         set(idata->UI, MUIA_ContextMenu, PopMenu);

         DoMethod(idata->UI, MUIM_Notify, MUIA_ContextMenuTrigger,
                  MUIV_EveryTime, o, 2, SWM_NSFList_CtxMenuChoice,
                  MUIV_TriggerValue);
      }
      
   }
   
   SWDB_RET(idata->UI);
} SDFT


/*****************************************************************************
 * Method: Hide all UIs for objects in the list                              *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
   if (!idata->UI) SWDB_RET(FAILURE);

   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);
   
   DoMethod(o, SWM_NList_ObjMethodA, msg);
   
   DoMethod(idata->SWA_NList_LV,MUIM_KillNotifyObj,MUIA_Listview_DoubleClick,o);

   DoMethod(idata->UI, OM_REMMEMBER, o);
   DoMethod(idata->UI, MUIM_KillNotifyObj, MUIA_ContextMenuTrigger, o);

   s_Desc       = NULL;
   s_Grp        = NULL;
   idata->UI    = NULL;
   idata->CList = NULL;

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Expire children if possible                                       *
 *****************************************************************************/

METHOD(SWM_NSFilter_Expire, Msg)
{
   DoMethod(o, SWM_NSFilter_ExpireList, o);
   
   // -- send method to such children as we have left ------------------------
   
   DoMethod(o, SWM_NList_ObjMethodA, msg);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Drag Query                                                        *
 *****************************************************************************/

METHOD(SWM_ObjList_DragQuery, SWP_ObjList_DragQuery *)
{
   STRPTR t;
   Object *obj = SWGetObj(msg->obj, SWA_ObjList_Object);
   
   // -- dragsorting handled by ObjListObject --------------------------------
   
   if (!SW_ISCLASS_SAFE(obj, SWC_NSFList, t))
      SWDB_RET(MUIV_DragQuery_Refuse);

   if (strcmp(SWGetStr(obj, SWA_NSFList_FltType),
              SWGetStr(o,   SWA_NSFList_FltType)))
      SWDB_RET(MUIV_DragQuery_Refuse);

   SWDB_RET(MUIV_DragQuery_Accept);
} SDFT


/*****************************************************************************
 * Method: Drag Drop action                                                  *
 *****************************************************************************/

METHOD(SWM_ObjList_DragDrop, SWP_ObjList_DragDrop *)
{
   ULONG x;
   Object *Clone, *lo;
   Object *obj = SWGetObj(msg->obj, SWA_ObjList_Object);
   
   AB_SetMod(idata);

   // -- reset notifies after a dragsort -------------------------------------
   
   if (obj == o)
      SWDB_RET(DoMethod(o, SWM_NList_SetNotify));

   // -- if different object, insert its selected objects --------------------

   set(o, MUIA_List_Quiet, TRUE);
   SW_MUIListLoopSel(obj, x, lo, TRUE)
      if (Clone = (Object *)DoMethod(lo, SWM_Clone))
         DoMethod(o, MUIM_List_InsertSingle, Clone, SWGetUL(o, MUIA_List_DropMark));
   set(o, MUIA_List_Quiet, FALSE);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Drag Drop action                                                  *
 *****************************************************************************/

METHOD(SWM_NSFList_DoSelected, ULONG *)
{
   Object *lo;
   LONG x;

   SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
      if (SWGetUL(lo, SWA_NSFilter_Enabled))
         DoMethodA(lo, (Msg)(msg+1));

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Add selected objects                                              *
 *****************************************************************************/

METHOD(SWM_NSFList_AddSel, SWP_NSFList_AddSel *)
{
   Object *lo;
   LONG x;

   if (!msg->CList) SWDB_RET(FAILURE);

   DoMethod(msg->CBox, MUIM_Popstring_Close, TRUE);
   
   SW_MUIListLoopSel(msg->CList, x, lo, TRUE)
      DoMethod(o, SWM_NList_Add,
               FixInsObj((Object *)DoMethod(msg->CList, SWM_ClassList_MakeObjNew,
                                            SWT_ClassList_MakeObj_Pos, x,
                                            TAG_DONE),
                         idata->EnableNew),
               NULL);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Save selected filters to disk                                     *
 *****************************************************************************/

METHOD(SWM_NSFList_Save, Msg)
{
   ULONG rc = FAILURE;
   LONG   x, GoodCount = 0;
   Object *lo;
   
   Object *TmpList = NULL;

   if (SWGetUL(idata->SWA_NList_L, MUIA_List_Entries) == 0) {
      SWERR(o, FAILURE, SWS_Err_NoFilters, 0, 0);
      goto Done;
   }
   
   TmpList = NSFListObject,
      SWA_NSFList_FltType,   idata->SWA_NSFList_FltType,
      SWA_NSFList_NoDispose, TRUE,
   End;

   if (!TmpList) goto Done;

   // -- build temporary list containing selected objects --------------------
   
   SW_MUIListLoopSel(idata->SWA_NList_L, x, lo, TRUE) {
      GoodCount++;
      DoMethod(TmpList, SWM_NList_Add, lo, NULL);
   }

   // -- save temporary object to disk ---------------------------------------

   if (DoMethod(TmpList, SWM_Root_M_SaveAs,
                SWT_Root_SL_Win,         _win(o),
                SWT_Root_SL_InitDrawer,   SWV_NSFList_SaveLoadDir,
                TAG_DONE) == SUCCESS) {
      MUI_Request(_app(o), _win(o), 0, NULL,
                  GetS(SWS_L_OK), GetS(SWS_L_SaveMsg), GoodCount);
      rc = SUCCESS;
   } else {
      MUI_Request(_app(o), _win(o), 0, NULL,
                  GetS(SWS_L_OK), GetS(SWS_L_Canc), GoodCount);
   }

  Done:
   if (TmpList) {
      DoMethod(TmpList, SWM_Root_CheckErr, _win(o));
      MUI_DisposeObject(TmpList);
   }

   DoMethod(o, SWM_Root_CheckErr, _win(o));

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: see if this filter is bad for this list                         *
 *****************************************************************************/

static LONG IsGoodFlt(SW_IDATA *idata, Object *lo)
{
   return (LONG)DoMethod(idata->CList, SWM_ClassList_GoodTypeObj, lo);
}


/*****************************************************************************
 * Method: Load selected filters from disk                                   *
 *****************************************************************************/

METHOD(SWM_NSFList_InsEdClose, Msg)
{
   CloseInsertEditor(idata);
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Display user info from active element                             *
 *****************************************************************************/

METHOD(SWM_NSFList_InsEdDspInfo, SWP_NSFList_InsEdDspInfo *)
{
   Object *Flt;
   STRPTR UInfo;

   if (!idata->InsertEditorInfo) SWDB_RET(FAILURE);

   DoMethod(idata->InsertEditorInfo, MUIM_TextEditor_ClearText);
   
   DoMethod(idata->InsertEditorList, MUIM_List_GetEntry,
            MUIV_List_GetEntry_Active, &Flt);

   if (!Flt) SWDB_RET(SUCCESS);

   UInfo = SWGetStr(Flt, SWA_NSFilter_AuthNote);

   if (!SW_STRISNULL(UInfo))
      set(idata->InsertEditorInfo, MUIA_TextEditor_Contents, UInfo);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Mark all filters for loading                                      *
 *****************************************************************************/

METHOD(SWM_NSFList_InsEdMkAll, Msg)
{
   if (!idata->InsertEditorList) SWDB_RET(FAILURE);

   DoMethod(idata->InsertEditorList, MUIM_List_Select, MUIV_List_Select_All,
            MUIV_List_Select_On, NULL);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Load selected filters from insert editor                          *
 *****************************************************************************/

METHOD(SWM_NSFList_InsEdLoad, Msg)
{
   LONG x, Cnt;
   Object *Flt, *EList = idata->InsertEditorList;
   
   if (!EList) SWDB_RET(FAILURE);

   set(EList, SWA_NSFList_NoDispose, TRUE);

   DoMethod(idata->InsertEditorList, MUIM_List_Select,
            MUIV_List_Select_Active, MUIV_List_Select_On, NULL);

   // -- insert the selected filters. ----------------------------------------
   
   SW_MUIListLoopSel(EList, x, Flt, TRUE)
      DoMethod(o, SWM_NList_Add, Flt, NULL);

   DoMethod(EList, MUIM_List_Remove, MUIV_List_Remove_Selected);
   set(EList, SWA_NSFList_NoDispose, FALSE);

   // -- close if no more filters in list ------------------------------------

   Cnt = 0;
   SW_MUIListLoop(EList, x, Flt, TRUE) Cnt++;

   if (!Cnt)
      DoMethod(idata->App, MUIM_Application_PushMethod, o, 1,
            SWM_NSFList_InsEdClose);
   
   SWDB_RET(SUCCESS);
} SDFT
   

/*****************************************************************************
 * Method: Load selected filters from disk                                   *
 *****************************************************************************/

METHOD(SWM_NSFList_Load, Msg)
{
   LONG   x, BadCount = 0, GoodCount = 0;
   Object *lo, *b_Load, *b_Close, *b_MkAll;
   ULONG  rc = FAILURE;
   Object *TmpList;
   STRPTR t1, t2;

   CloseInsertEditor(idata);

   TmpList = (Object *)
      DoMethod(o, SWM_Root_M_Open,
               SWT_Root_SL_Win,          _win(o),
               SWT_Root_SL_InitDrawer,   SWV_NSFList_SaveLoadDir,
               TAG_DONE);

   if (!TmpList) goto Done;

   if (!SW_SAMECLASS_SAFE(TmpList, o, t1, t2)) {
      SWERR(o, NULL, SWS_Err_WrongObjClass, 0, 0);
      MUI_DisposeObject(TmpList);
      goto Done;
   }

   set(TmpList, MUIA_List_Active, MUIV_List_Active_Off);
   DoMethod(TmpList, MUIM_List_Select, MUIV_List_Select_All,
            MUIV_List_Select_Off, NULL);

   SW_MUIListLoop(TmpList, x, lo, TRUE) {
      if (IsGoodFlt(idata, lo)) {
         GoodCount++;
      } else {
         BadCount++;
         DoMethod(TmpList, MUIM_List_Select, x, MUIV_List_Select_On, NULL);
      }
   }

   if (BadCount) {
      DoMethod(TmpList, MUIM_List_Remove, MUIV_List_Remove_Selected);

      MUI_Request(_app(o), _win(o), 0, NULL,
                  GetS(SWS_L_OK), GetS(SWS_Note_BadFltType), BadCount);
   }

   // -- make sure list is not empty -----------------------------------------

   if (!GoodCount) {
      MUI_DisposeObject(TmpList);
      goto Done;
   }
   
   SetAttrs(TmpList,
            MUIA_ShortHelp,         GetS(SWS_H_InsertHelp),
            SWA_NSFList_UseCtxMenu, FALSE,
            SWA_NSFList_UIMode,     SWV_NSFList_UI_Insert,
            TAG_DONE);
   
   idata->InsertEditorWin = WindowObject,
      MUIA_Window_Title,       GetS(SWS_T_InsEdit),
      MUIA_Window_ID,          MAKE_ID('I', 'f', 'L', 's'),
      MUIA_Window_IsSubWindow, TRUE,
      WindowContents, VGroup,
         Child, DoMethod(idata->InsertEditorList = TmpList, SWM_NSFilter_UI),
         Child, TextObject,
            TextFrame,
            MUIA_Background,    MUII_TextBack,
            MUIA_Text_PreParse, MUIX_C MUIX_B,
            MUIA_Text_Contents, GetS(SWS_T_AuthNote),
         End,
         Child, idata->InsertEditorInfo = TextEditorObject,
            ReadListFrame,
            MUIA_CycleChain,            TRUE,
            MUIA_TextEditor_ImportHook, MUIV_TextEditor_ImportHook_EMail,
            MUIA_TextEditor_ReadOnly,   TRUE,
         End,
         Child, HGroup,
            MUIA_Group_SameSize, TRUE,
            Child, b_Load  = MkBt(SWS_B_InsLoad,  SWS_H_InsLoad),
            Child, b_Close = MkBt(SWS_B_InsClose, SWS_H_InsClose),
            Child, b_MkAll = MkBt(SWS_B_InsMkAll, SWS_H_InsMkAll),
         End,
      End,
   End;

   if (!idata->InsertEditorWin) {
      MUI_DisposeObject(TmpList);
      goto Done;
   }

   idata->App = _app(o);
   
   DoMethod(b_Load,  NOTP, o, 1, SWM_NSFList_InsEdLoad);
   DoMethod(b_MkAll, NOTP, o, 1, SWM_NSFList_InsEdMkAll);

   DoMethod(SWGetObj(idata->InsertEditorList, SWA_NList_LV),
            DCLK, o, 1, SWM_NSFList_InsEdLoad);
   
   DoMethod(b_Close, NOTP, idata->App, 4, MUIM_Application_PushMethod, o, 1,
            SWM_NSFList_InsEdClose);

   DoMethod(idata->InsertEditorWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            idata->App, 4, MUIM_Application_PushMethod, o, 1,
            SWM_NSFList_InsEdClose);

   DoMethod(TmpList, MUIM_Notify, MUIA_List_Active, MUIV_EveryTime,
            o, 2, SWM_NSFList_InsEdDspInfo, MUIV_TriggerValue);

   DoMethod(idata->App,  OM_ADDMEMBER, idata->InsertEditorWin);
   set(idata->InsertEditorWin, MUIA_Window_Open, TRUE);

   set(TmpList, MUIA_List_Active, 0);

   rc = SUCCESS;
   
  Done:
   DoMethod(o, SWM_Root_CheckErr, _win(o));

   SWDB_RET(rc);
} SDFT



/*****************************************************************************
 * Method: Context menu choice                                               *
 *****************************************************************************/

METHOD(SWM_NSFList_CtxMenuChoice, SWP_NSFList_CtxMenuChoice *)
{
   ULONG Choice = SWGetUL(msg->Item, MUIA_UserData);

   switch (Choice) {
     case SWS_M_Save:
      DoMethod(o, SWM_NSFList_Save);
      break;
      
     case SWS_M_Load:
      DoMethod(o, SWM_NSFList_Load);
      break;

     case SWS_M_SelAll:
      DoMethod(o, MUIM_List_Select, MUIV_List_Select_All,
               MUIV_List_Select_On, NULL);
      break;

     case SWS_M_Clear:
      if (MUI_Request(_app(o), _win(o), 0, NULL,
                      GetS(SWS_L_OkCanc), GetS(SWS_L_ClearMsg), NULL))
         DoMethod(o, MUIM_List_Clear);
      break;
   }

   DoMethod(o, SWM_Root_CheckErr, _win(o));

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Find empty position in filter function list                     *
 *****************************************************************************/

static LONG FindEmptyFFPos(SW_IDATA *idata)
{
   LONG x;

   for (x=0; x<idata->FFArrayMax; x++)
      if (!idata->FFIcons[x])
	 return x;

   return -1;
}


/*****************************************************************************
 * Method: Add a filter function icon to our list                            *
 *****************************************************************************/

METHOD(SWM_NSFList_AddFFIcon, SWP_NSFList_AddFFIcon *)
{
   LONG AddPos;
   Object *AddObj, *txt;
   
   if (!idata->MemPool)
      idata->MemPool = NN_MemPool(idata);

   if (!idata->SWA_NSFList_FFToolBar || !msg->Icon || !idata->MemPool)
      SWDB_RET(FAILURE);

   if ((AddPos = FindEmptyFFPos(idata)) < 0) {
      idata->FFArrayMax += 32;
      RV(idata->FFArrayMax * sizeof(Object *), &idata->FFIcons);
      RV(idata->FFArrayMax * sizeof(Object *), &idata->FFObjs);

      if (!idata->FFIcons || !idata->FFObjs) {
         idata->FFArrayMax = 0;
         SWDB_RET(FAILURE);
      }
   }

   if ((AddPos = FindEmptyFFPos(idata)) < 0)
      SWDB_RET(FAILURE);

   idata->FFIcons[AddPos] = msg->Icon;
   idata->FFObjs[AddPos]  = msg->FilterObj;

   if (idata->FFLabs = NN_FFTBLab(idata)) {
      AddObj = VGroup,
         Child, msg->Icon,
         Child, txt = TextObject,
            MUIA_Text_PreParse, SWGetUL(msg->FilterObj, SWA_NSFilter_Enabled) ?
                                FL_PPEnabled : FL_PPDisabled,
            MUIA_Text_Contents, SWGetStr(msg->FilterObj, SWA_NSFilter_Desc),
         End,
      End;

      // -- set the label to bold/normal for en/disabled ---------------------
   
      set(msg->Icon, MUIA_UserData, txt);

      DoMethod(msg->FilterObj, MUIM_Notify, SWA_NSFilter_Desc, MUIV_EveryTime,
               txt, 3, MUIM_Set, MUIA_Text_Contents, MUIV_TriggerValue);
   
      DoMethod(msg->FilterObj, MUIM_Notify, SWA_NSFilter_Enabled, TRUE,
               txt, 3, MUIM_Set, MUIA_Text_PreParse, FL_PPEnabled);
   
      DoMethod(msg->FilterObj, MUIM_Notify, SWA_NSFilter_Enabled, FALSE,
               txt, 3, MUIM_Set, MUIA_Text_PreParse, FL_PPDisabled);
   } else
      AddObj = msg->Icon;

   // -- add it to the toolbar -----------------------------------------------
   
   DoMethod(idata->SWA_NSFList_FFToolBar, MUIM_Group_InitChange);
   DoMethod(idata->SWA_NSFList_FFToolBar, OM_ADDMEMBER, AddObj);
   DoMethod(idata->SWA_NSFList_FFToolBar, MUIM_Group_ExitChange);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Remove object's filter function icons from our list               *
 *****************************************************************************/

METHOD(SWM_NSFList_RemFFIcons, SWP_NSFList_RemFFIcons *)
{
   LONG x;
   Object *RemObj, *Parent, *Txt;

   if (!idata->SWA_NSFList_FFToolBar)
      SWDB_RET(FAILURE);

   DoMethod(idata->SWA_NSFList_FFToolBar, MUIM_Group_InitChange);

   for (x=0; x<idata->FFArrayMax; x++)
      if (idata->FFObjs[x] == msg->FilterObj ||
          idata->FFObjs[x] && !msg->FilterObj) {
         RemObj = idata->FFIcons[x];

         // -- if it`s in a group with a label, get the group pointer --------
         
         if ((Parent = SWGetObj(RemObj, MUIA_Parent)) &&
             Parent != idata->SWA_NSFList_FFToolBar) {
            RemObj = Parent;

            Txt = SWGetObj(idata->FFIcons[x], MUIA_UserData);

            // -- kill ALL notifies, since we set up several -----------------
            
            DoMethod(idata->FFObjs[x], MUIM_KillNotifyObj,
                     SWA_NSFilter_Enabled, Txt);

            DoMethod(idata->FFObjs[x], MUIM_KillNotifyObj,
                     SWA_NSFilter_Enabled, Txt);

            DoMethod(idata->FFObjs[x], MUIM_KillNotifyObj,
                     SWA_NSFilter_Desc, Txt);
         }
         
	 DoMethod(idata->SWA_NSFList_FFToolBar, OM_REMMEMBER, RemObj);
      }

   DoMethod(idata->SWA_NSFList_FFToolBar, MUIM_Group_ExitChange);

   for (x=0; x<idata->FFArrayMax; x++)
      if (idata->FFObjs[x] == msg->FilterObj ||
          idata->FFObjs[x] && !msg->FilterObj) {
         DoMethod(idata->FFObjs[x], SWM_NSFilter_RemFFIcon,
                  idata->SWA_NSFList_Group, o, idata->FFIcons[x]);
	 idata->FFObjs[x]  = NULL;
         idata->FFIcons[x] = NULL;
      }
   
   SWDB_RET(SUCCESS);
} SDFT
