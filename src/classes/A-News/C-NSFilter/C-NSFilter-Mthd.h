/*****************************************************************************
 * Method: Default begin method for new select filters                       *
 *****************************************************************************/

METHOD(SWM_NSFilter_Begin, Msg)
{
   SetAttrs(o,
            SWA_NSFilter_TotNow,   0,
            SWA_NSFilter_FoundNow, 0,
            TAG_DONE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Determine if filter matches article                               *
 *****************************************************************************/

METHOD(SWM_NSFilter_Match, Msg)
{
   SWDB_RET(TRUE);
} SDFT
   

/*****************************************************************************
 * Method: Default end Method for new select filters                         *
 *****************************************************************************/

METHOD(SWM_NSFilter_End, Msg)
{
   ULONG Total = 0, Found = 0;

   DoMethod(o, SWM_NSFilter_TrackStats, &Total, &Found);
   
   if (Found)
      DoMethod(o, SWM_NSFilter_UpdLastMatch);

   SetAttrs(o,
            SWA_NSFilter_TotNow,    idata->SWA_NSFilter_TotNow    + Total,
            SWA_NSFilter_FoundNow,  idata->SWA_NSFilter_FoundNow  + Found,
            SWA_NSFilter_TotHist,   idata->SWA_NSFilter_TotHist   + Total,
            SWA_NSFilter_FoundHist, idata->SWA_NSFilter_FoundHist + Found,
            TAG_DONE);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Update statistics after xfer                                      *
 *****************************************************************************/

METHOD(SWM_NSFilter_TrackStats, SWP_NSFilter_TrackStats *)
{
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Make UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NSFilter_UI, Msg)
{
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Make Toolbar UI for this object                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_TBUI, SWP_NSFilter_TBUI *)
{
   Object *cm, *str;

   if (!msg->Grp) SWDB_RET(FAILURE);

   DoMethod(msg->Grp, OM_ADDMEMBER, Label(GetS(SWS_L_UseOnOffBt)));

   DoMethod(msg->Grp, OM_ADDMEMBER,
     (HGroup,
          MUIA_ShortHelp, GetS(SWS_H_UseOnOffBt),
          Child, cm = CheckMark(idata->SWA_NSFilter_UseOnOffBt),
          Child, PopaslObject,
             MUIA_Popstring_String, str = NewObject(SWObjStringClass(), NULL,
                StringFrame,
                MUIA_String_MaxLen,   FMSIZE,
                MUIA_String_Contents, idata->SWA_NSFilter_OnOffBtImg,
             TAG_DONE),
             MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
             MUIA_Popasl_Type,      ASL_FileRequest,
          End,
       End));

   set(str, MUIA_Disabled, !idata->SWA_NSFilter_UseOnOffBt);

   DoMethod(cm, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_UseOnOffBt, MUIV_TriggerValue);
  
   DoMethod(cm, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            str, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
  
   DoMethod(str, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_OnOffBtImg, MUIV_TriggerValue);
  
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# include "S-TrackWinHash.h"

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

static Object *tobj(STRPTR str)
{
   return  TextObject,
      MUIA_FixWidthTxt, str,
   End;
}

static Object *StatInfo(Object *o, SW_IDATA *idata)
{
   Object *b_upd, *b_res, *g =
      VGroup,
         MUIA_HorizWeight,     300,
         MUIA_Group_SameWidth, TRUE,
         Child, HGroup,
            GroupFrameT(GetS(SWS_L_Hist)),
            Child, Label(GetS(SWS_L_Matched)),
            Child, idata->hm1 = tobj("00000000"),
            Child, Label(GetS(SWS_L_Of)),
            Child, idata->hm2 = tobj("00000000"),
            Child, HSpace(0),
         End,
         Child, VGroup,
            GroupFrameT(GetS(SWS_L_LastCount)),
            Child, HGroup,
               Child, Label(GetS(SWS_L_Matched)),
               Child, idata->lm1 = tobj("00000000"),
               Child, Label(GetS(SWS_L_Of)),
               Child, idata->lm2 = tobj("00000000"),
               Child, HSpace(0),
            End,
            Child, idata->lmdate = TextObject, End,
            Child, idata->eodate = TextObject, End,
         End,
         Child, HSpace(0),
         Child, HGroup,
            Child, b_upd = MkBt(SWS_L_Upd,   SWS_H_Upd),
            Child, b_res = MkBt(SWS_L_Reset, SWS_H_Reset),
         End,
      End;

   if (!g) return NULL;

   DoMethod(b_upd, NOTP, o, 1, SWM_NSFilter_UpdLastMatch);
   DoMethod(b_res, NOTP, o, 1, SWM_NSFilter_ResetStats);

   return g;
}

static void InitSV(Object *o, Object *no, ULONG Attr)
{
   DoMethod(no, MUIM_SetAsString, MUIA_Text_Contents, "%ld", SWGetUL(o, Attr));
}


METHOD(SWM_NSFilter_ShowUI, SWP_NSFilter_ShowUI *)
{
   Object *globui, *ui, *ui_b3, *ui_b4, *tm, *rgr;

   ULONG Hash = SWGetUL(o, SWA_ObjClassHash);
   static STRPTR Pages[5];
   static STRPTR TModes[4];
   struct DateStamp ds;

   DateStamp(&ds);

   Pages[0] = GetS(SWS_L_Edit),
   Pages[1] = GetS(SWS_L_Global),
   Pages[2] = GetS(SWS_L_Stats),
   Pages[3] = GetS(SWS_L_AuthNote),
   Pages[4] = NULL;

   TModes[0] = GetS(SWS_L_TM_Base),
   TModes[1] = GetS(SWS_L_TM_Any),
   TModes[2] = GetS(SWS_L_TM_All),
   TModes[3] = NULL;

   // just bring existing window to front --------

   if (!msg->App) SWDB_RET(0);

   idata->App                = msg->App;
   idata->SWA_AppBase_TopObj = (Object *)muiUserData(msg->App);
   
   if (idata->UIWin) {
      DoMethod(idata->UIWin, MUIM_Window_ToFront);
      SWDB_RET(SUCCESS);
   }

   if (!(ui = (Object *)DoMethod(o, SWM_NSFilter_UI)))
      SWDB_RET(FAILURE);

   if (!(globui = ColGroup(2),
      Child, Label2(GetS(SWS_L_Desc)),
      Child, idata->ui_s1 = NewObject(SWObjStringClass(), NULL,
                            StringFrame,
                            MUIA_ShortHelp,     GetS(SWS_H_Desc),
                            MUIA_CycleChain,    1,
                            MUIA_String_MaxLen, SW_ATTRMAP[0].Max-1,
                            TAG_DONE),
      Child, Label2(GetS(SWS_L_ThreadMode)),
      Child, tm = CycleObject,
         MUIA_Cycle_Entries, TModes,
         MUIA_CycleChain,    TRUE,
         MUIA_ShortHelp,     GetS(SWS_H_ThreadMode),
      End,
      Child, Label2(GetS(SWS_L_ExpDate)),
      Child, HGroup,
          Child, ui_b3 = CheckMark(idata->SWA_NSFilter_ExpireDateOK),
          Child, RectangleObject, MUIA_HorizWeight, 5, End,
          Child, idata->es = NewObject(ExpireSliderClass(), NULL,
             MUIA_Numeric_Min,           ds.ds_Days,
             MUIA_Numeric_Max,           ds.ds_Days+SWV_NSF_ExpMax,
             MUIA_Numeric_Value,         idata->SWA_NSFilter_ExpireDate,
             MUIA_CycleChain,            1,
             MUIA_ShortHelp,             GetS(SWS_H_ExpDate),
             TAG_DONE),
      End,
      Child, Label2(GetS(SWS_L_ExpDisuse)),
      Child, HGroup,
         Child, ui_b4 = CheckMark(idata->SWA_NSFilter_ExpDisuseOK),
         Child, RectangleObject, MUIA_HorizWeight, 5, End,
         Child, idata->du = NewObject(DisuseSliderClass(), NULL,
             MUIA_Numeric_Min,           1,
             MUIA_Numeric_Max,           SWV_NSF_ExpMax,
             MUIA_Numeric_Value,         idata->SWA_NSFilter_ExpDisuse,
             MUIA_CycleChain,            1,
             MUIA_ShortHelp,             GetS(SWS_H_ExpDisuse),
             TAG_DONE),
       End,
      Child, Label2(GetS(SWS_L_Enabled)),
      Child, HGroup,
         Child, idata->ui_b1 = CheckMark(idata->SWA_NSFilter_Enabled),
         Child, RectangleObject, MUIA_HorizWeight, 5, End,
         Child, Label2(GetS(SWS_L_Invert)),
         Child, idata->ui_b2 = CheckMark(idata->SWA_NSFilter_Inv),
         Child, HSpace(0),
      End,
   End))
      SWDB_RET(FAILURE);

   if (DoMethod(o, SWM_NSFilter_TBUI, globui) != SUCCESS)
      SWDB_RET(FAILURE);

   idata->UIWin = WindowObject,
      MUIA_Window_Title,       GetS(SWS_L_WinTitle),
      MUIA_Window_ID,          Hash + TrackWinHash(Hash, TWC_Open),
      MUIA_Window_IsSubWindow, TRUE,
      MUIA_Window_AppWindow,   TRUE,
      WindowContents, rgr = RegisterGroup(Pages),
         MUIA_Background, MUII_RegisterBack,
         MUIA_CycleChain, 1,
         Child, ui,
         Child, globui,
         Child, HCenter(VCenter(StatInfo(o, idata))),
         Child, idata->Ed = NN_EditorObject,
            AppBaseDefTop(idata),
            SWA_NNews_Editor_InitStrAddr, &idata->SWA_NSFilter_AuthNote,
         End,
      End,
   End;

   if (!idata->UIWin) SWDB_RET(FAILURE);

   set(idata->ui_b1, MUIA_ShortHelp, GetS(SWS_H_Enabled));
   set(idata->ui_b2, MUIA_ShortHelp, GetS(SWS_H_Invert));
   
   DoMethod(o, MUIM_MultiSet, MUIA_CycleChain, 1,
            idata->ui_b1, idata->ui_b2, ui_b3, ui_b4, NULL);

   DoMethod(o, SWM_NSFilter_UpdStatWin);

   DoMethod(o, MUIM_Notify, SWA_NSFilter_FoundNow, MUIV_EveryTime, o, 1,
            SWM_NSFilter_UpdStatWin);

   // -- update stats when we go to that page --------------------------------
   
   DoMethod(rgr, MUIM_Notify, MUIA_Group_ActivePage, 2,
            o, 1, SWM_NSFilter_UpdStatWin);

   // - set up notifications on various attributes ---------------------------

   set(idata->ui_s1, MUIA_String_Contents, idata->SWA_NSFilter_Desc);

   DoMethod(idata->ui_s1, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_Desc, MUIV_TriggerValue);

   DoMethod(idata->ui_b1, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_Enabled, MUIV_TriggerValue);

   DoMethod(idata->ui_b2, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_Inv, MUIV_TriggerValue);

   DoMethod(ui_b3, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_ExpireDateOK, MUIV_TriggerValue);

   DoMethod(ui_b4, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_ExpDisuseOK, MUIV_TriggerValue);

   DoMethod(idata->es, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_ExpireDate, MUIV_TriggerValue);

   DoMethod(idata->du, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_ExpDisuse, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NSFilter_Desc, MUIV_EveryTime,
            idata->ui_s1, 3, MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NSFilter_ExpireDateOK, MUIV_EveryTime,
            idata->es, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NSFilter_ExpDisuseOK,  MUIV_EveryTime,
            idata->du, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NSFilter_Enabled, MUIV_EveryTime,
            idata->ui_b1, 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NSFilter_Inv, MUIV_EveryTime,
            idata->ui_b2, 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);

   DoMethod(tm, MUIM_Notify, MUIA_Cycle_Active, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NSFilter_ThreadMode, MUIV_TriggerValue);

   DoMethod(msg->App, OM_ADDMEMBER, idata->UIWin);

   DoMethod(idata->UIWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            msg->App, 4, MUIM_Application_PushMethod, o, 1,
            SWM_NSFilter_HideUI);

   set(idata->es, MUIA_Disabled, !idata->SWA_NSFilter_ExpireDateOK);
   set(idata->du, MUIA_Disabled, !idata->SWA_NSFilter_ExpDisuseOK);
   set(tm,        MUIA_Cycle_Active, idata->SWA_NSFilter_ThreadMode);
   
   set(idata->UIWin, MUIA_Window_Open, TRUE);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Hide UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
   STRPTR Buff;
   
   if (!idata->UIWin) SWDB_RET(0);

   // -- save contents of editor ---------------------------------------------

   if (Buff = (STRPTR)DoMethod(idata->Ed, MUIM_TextEditor_ExportText)) {
      set(o, SWA_NSFilter_AuthNote, Buff);
      FreeVec(Buff);
   }

   // -- be sure to nuke ALL the notifications we set up! --------------------

   DoMethod(o, MUIM_KillNotifyObj, SWA_NSFilter_Desc,         idata->ui_s1);
   DoMethod(o, MUIM_KillNotifyObj, SWA_NSFilter_Enabled,      idata->ui_b1);
   DoMethod(o, MUIM_KillNotifyObj, SWA_NSFilter_Inv,          idata->ui_b2);
   DoMethod(o, MUIM_KillNotifyObj, SWA_NSFilter_FoundNow,     o);
   DoMethod(o, MUIM_KillNotifyObj, SWA_NSFilter_ExpireDateOK, idata->es);
   DoMethod(o, MUIM_KillNotifyObj, SWA_NSFilter_ExpDisuseOK,  idata->du);

   TrackWinHash(SWGetUL(o, SWA_ObjClassHash), TWC_Close);

   set(idata->UIWin, MUIA_Window_Open, FALSE);
   DoMethod(idata->App, OM_REMMEMBER, idata->UIWin);
   MUI_DisposeObject(idata->UIWin);
   idata->UIWin = NULL;
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Expire children if possible                                       *
 *****************************************************************************/

METHOD(SWM_NSFilter_Expire, Msg)
{
   SWDB_RET(FALSE);
} SDFT


/*****************************************************************************
 * Function: find expiration date of the filter                              *
 *****************************************************************************/

static ULONG GetExpDay(Object *o)
{
   ULONG ExpDispuse, ExpDay     = LONG_MAX;

   if (SWGetUL(o, SWA_NSFilter_ExpireDateOK))
      ExpDay = SWGetUL(o, SWA_NSFilter_ExpireDate);

   if (SWGetUL(o, SWA_NSFilter_ExpDisuseOK)) {
      ExpDispuse = SWGetUL(o, SWA_NSFilter_ExpDisuse) + 
                   SWGetUL(o, SWA_NSFilter_LastMatchDay);

      ExpDay = min(ExpDay, ExpDispuse);
   }

   return ExpDay;      
}


/*****************************************************************************
 * Method: Expire filters in a list                                          *
 *****************************************************************************/

METHOD(SWM_NSFilter_ExpireList, SWP_NSFilter_ExpireList *)
{
   LONG    x;
   Object *lo;
   struct DateStamp ds;

   set(msg->L, MUIA_List_Quiet, TRUE);
   
   DateStamp(&ds);

   DoMethod(msg->L, MUIM_List_Select, MUIV_List_Select_All,
            MUIV_List_Select_Off, NULL);

   set(msg->L, MUIA_List_Active, MUIV_List_Active_Off);

   // -- loop through the list looking for articles to expire ----------------
   // -- expire if it`s past the fixed expiration date --------------------
   
   SW_MUIListLoop(o, x, lo, TRUE)
      if (ds.ds_Days >= GetExpDay(lo))
          DoMethod(msg->L, MUIM_List_Select, x, MUIV_List_Select_On, NULL);

   // -- remove all the ones we found ----------------------------------------
   
   DoMethod(msg->L, MUIM_List_Remove, MUIV_List_Remove_Selected);

   set(msg->L, MUIA_List_Quiet, FALSE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Update statistics window                                          *
 *****************************************************************************/

METHOD(SWM_NSFilter_UpdStatWin, Msg)
{
   struct DateTime dt;
   TEXT buff[80], Day[24], Date[20];
   
   if (!idata->UIWin) SWDB_RET(0);

   InitSV(o, idata->hm1, SWA_NSFilter_FoundHist);
   InitSV(o, idata->hm2, SWA_NSFilter_TotHist);
   InitSV(o, idata->lm1, SWA_NSFilter_FoundNow);
   InitSV(o, idata->lm2, SWA_NSFilter_TotNow);

   dt.dat_Stamp.ds_Minute = 0;
   dt.dat_Stamp.ds_Tick   = 0;
   dt.dat_Flags           = 0;
   dt.dat_Format          = FORMAT_DOS;
   dt.dat_StrDate         = Date;
   dt.dat_StrTime         = NULL;
   dt.dat_StrDay          = Day;

   strcpy(buff, GetS(SWS_L_LastMtch));

   if (idata->SWA_NSFilter_LastMatchDayDsp == LONG_MAX) {
      strcat(buff, GetS(SWS_L_Never));
   } else {
      dt.dat_Stamp.ds_Days   = idata->SWA_NSFilter_LastMatchDayDsp;
      DateToStr(&dt);
      strcat(strcat(strcat(buff, Day), ", "), Date);
   }

   set(idata->lmdate, MUIA_Text_Contents, buff);

   strcpy(buff, GetS(SWS_L_ExpOn));

   if ((dt.dat_Stamp.ds_Days = GetExpDay(o)) == LONG_MAX) {
      strcat(buff, GetS(SWS_L_Never));
   } else {
      DateToStr(&dt);
      strcat(strcat(strcat(buff, Day), ", "), Date);
   }

   set(idata->eodate, MUIA_Text_Contents, buff);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Update last match date                                            *
 *****************************************************************************/

METHOD(SWM_NSFilter_UpdLastMatch, Msg)
{
   struct DateStamp ds;

   DateStamp(&ds);
  
   SetAttrs(o,
            SWA_NSFilter_LastMatchDay,    ds.ds_Days,
            SWA_NSFilter_LastMatchDayDsp, ds.ds_Days,
            TAG_DONE);

   SWDB_RET(DoMethod(o, SWM_NSFilter_UpdStatWin));
} SDFT


/*****************************************************************************
 * Method: Update last match date                                            *
 *****************************************************************************/

METHOD(SWM_NSFilter_ResetStats, Msg)
{
   struct DateStamp ds;

   DateStamp(&ds);

   SetAttrs(o,
            SWA_NSFilter_TotNow,          0,
            SWA_NSFilter_FoundNow,        0,
            SWA_NSFilter_TotHist,         0,
            SWA_NSFilter_FoundHist,       0,
            SWA_NSFilter_LastMatchDayDsp, LONG_MAX,
            SWA_NSFilter_LastMatchDay,    ds.ds_Days,
            TAG_DONE);
   
   SWDB_RET(DoMethod(o, SWM_NSFilter_UpdStatWin));
} SDFT


/*****************************************************************************
 * Method: return expire slider class                                        *
 *****************************************************************************/

METHOD(SWM_NSFilter_ExpireSliderClass, Msg)
{
   SWDB_RET(ExpireSliderClass());
} SDFT


/*****************************************************************************
 * Method: return disuse slider class                                        *
 *****************************************************************************/

METHOD(SWM_NSFilter_DisuseSliderClass, Msg)
{
   SWDB_RET(DisuseSliderClass());
} SDFT


/*****************************************************************************
 * Method: return child UI                                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_ChildUI, SWP_NSFilter_ChildUI *)
{
   SWDB_RET(msg->Obj ? (Object *)DoMethod(msg->Obj,SWM_NSFilter_UI) : HVSpace);
} SDFT


/*****************************************************************************
 * Method: substitute imbedded UI                                            *
 *****************************************************************************/

METHOD(SWM_NSFilter_NewImbUI, SWP_NSFilter_NewImbUI *)
{
   Object *NewO = (Object *)DoMethod(idata->ClassLB, SWM_ClassList_MakeObj),
          *OldO =  *msg->Filter;

   DoMethod(idata->PopB, MUIM_Popstring_Close, TRUE);
      
   if (!NewO) SWDB_RET(NULL);

   if (OldO) DoMethod(OldO, SWM_NSFilter_HideUI);

   *msg->Filter = NewO;
   
   DoMethod(idata->UIGroup, MUIM_Group_InitChange);

   if (idata->UIChild) {
      DoMethod(idata->UIGroup, OM_REMMEMBER, idata->UIChild);
      MUI_DisposeObject(idata->UIChild);
   }

   DoMethod(idata->UIGroup, OM_ADDMEMBER, idata->UIChild =
            (Object *)DoMethod(o, SWM_NSFilter_ChildUI, NewO));

   DoMethod(idata->UIGroup, MUIM_Group_ExitChange);

   MUI_DisposeObject(OldO);

   SWDB_RET(NULL);
} SDFT


/*****************************************************************************
 * Method: make imbedded UI                                                  *
 *****************************************************************************/

METHOD(SWM_NSFilter_ImbUI, SWP_NSFilter_ImbUI *)
{
   Object *ui, *lv, *v1, *v2;

   ui = VGroup,
      Child, v1 = VSpace(0),
      Child, idata->UIGroup = HGroup,
         Child, idata->UIChild = (Object *)
            DoMethod(o, SWM_NSFilter_ChildUI, *msg->Filter),
      End,
      Child, v2 = VSpace(0),
      Child, msg->UI,
      Child, idata->PopB = PopobjectObject,
         MUIA_Popstring_Button, SWUI_Button(GetS(SWS_B_New), msg->Help),
         MUIA_Popobject_Object, lv = ListviewObject,
            MUIA_CycleChain,     TRUE,
            MUIA_Listview_Input, TRUE,
            MUIA_Listview_List, idata->ClassLB =
               MUI_NewObject(SWC_Classlist,
                             InputListFrame,
                             SWA_ClassList_Type,"NSFilter",
                             TAG_DONE),
         End,
      End,
   End;

   if (!ui) SWDB_RET(NULL);

   set(v1, MUIA_VertWeight, 1);
   set(v2, MUIA_VertWeight, 1);

   DoMethod(lv, DCLK, o, 6, SWM_NSFilter_NewImbUI, msg->Filter);

   SWDB_RET(ui);
} SDFT


/*****************************************************************************
 * Method: Add filter function icons                                         *
 *****************************************************************************/

METHOD(SWM_NSFilter_AddFFIcons, SWP_NSFilter_AddFFIcons *)
{
   Object *b;
   ULONG NewLen;

   if (!idata->SWA_NSFilter_UseOnOffBt ||
       SW_STRISNULL(idata->SWA_NSFilter_OnOffBtImg))
      SWDB_RET(SUCCESS);

   if (!idata->MemPool) {
      idata->SWA_AppBase_TopObj = SWGetObj(msg->Grp, SWA_AppBase_TopObj);
      idata->MemPool            = NN_MemPool(idata);
   }

   NewLen = strlen(idata->SWA_NSFilter_Desc) + strlen(GetS(SWS_H_TBOnOff)) + 8;

   if (!idata->TBDesc || NewLen >= idata->TBDescLen)
      RV(NewLen, &idata->TBDesc);

   if (!idata->TBDesc) SWDB_RET(FAILURE);

   idata->TBDescLen = NewLen;

   sprintf(idata->TBDesc, GetS(SWS_H_TBOnOff), idata->SWA_NSFilter_Desc);

   b = DTImgObject,
      MUIA_ShortHelp,       idata->TBDesc,
      MUIA_Selected,        idata->SWA_NSFilter_Enabled,
      MUIA_InnerTop,        0,
      MUIA_InnerBottom,     0,
      MUIA_InnerLeft,       0,
      MUIA_InnerRight,      0,
      MUIA_Frame,           MUIV_Frame_ImageButton,
      MUIA_InputMode,       MUIV_InputMode_Toggle,
      MUIA_CycleChain,      1,
      SWA_DTImg_FSpec,      idata->SWA_NSFilter_OnOffBtImg,
      SWA_DTImg_ImgCache,   NN_DTImages(idata),
   End;

   if (!b) SWDB_RET(FAILURE);

   DoMethod(b, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 4, SWM_NSFilter_FF_OnOff, msg->Grp, MUIV_TriggerValue,
            msg->NSFList);

   DoMethod(o, MUIM_Notify, SWA_NSFilter_Enabled, MUIV_EveryTime,
            b, 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);
   
   SWDB_RET(DoMethod(msg->NSFList, SWM_NSFList_AddFFIcon, o, b));
} SDFT


/*****************************************************************************
 * Method: Remove filter function icons                                      *
 *****************************************************************************/

METHOD(SWM_NSFilter_RemFFIcon, SWP_NSFilter_RemFFIcon *)
{
   DoMethod(o, MUIM_KillNotifyObj, SWA_NSFilter_Enabled, msg->Icon);
   
   MUI_DisposeObject(msg->Icon);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Turn filter on/off (for filter function button)                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_FF_OnOff, SWP_NSFilter_FF_OnOff *)
{
   switch (msg->State) {
      case SWV_NSFilter_Off:    set(o, SWA_NSFilter_Enabled, FALSE); break;
      case SWV_NSFilter_On:     set(o, SWA_NSFilter_Enabled, TRUE);  break;
      case SWV_NSFilter_Toggle: set(o, SWA_NSFilter_Enabled,
                                    !idata->SWA_NSFilter_Enabled);   break;
      default: SWDB_RET(FAILURE);
   }

   switch (SWGetUL(msg->NSFList, SWA_NSFList_Use)) {
     case SWV_NSFList_Use_Select:
      DoMethod(msg->Grp, SWM_NGroup_ChgSelFlt, 0);
      break;

     case SWV_NSFList_Use_Hlight:
      DoMethod(msg->Grp, SWM_NGroup_ChgHLFlt);
      break;
   }
  
   SWDB_RET(SUCCESS);
} SDFT


