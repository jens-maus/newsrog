/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NAFilter_UI, Msg)
{
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# include "S-TrackWinHash.h"


METHOD(SWM_NAFilter_ShowUI, SWP_NSFilter_ShowUI *)
{
   Object *ui, *ui_b2;
   ULONG Hash = SWGetUL(o, SWA_ObjClassHash);
   static STRPTR Pages[4];

   Pages[0] = GetS(SWS_L_Edit),
   Pages[1] = GetS(SWS_L_Global),
   Pages[2] = GetS(SWS_L_AuthNote),
   Pages[3] = NULL;

   // just bring existing window to front --------

   if (!msg->App) SWDB_RET(0);

   idata->App                = msg->App;
   idata->SWA_AppBase_TopObj = (Object *)muiUserData(msg->App);
   
   if (idata->UIWin) {
      DoMethod(idata->UIWin, MUIM_Window_ToFront);
      SWDB_RET(SUCCESS);
   }

   if (!(ui = (Object *)DoMethod(o, SWM_NAFilter_UI)))
      SWDB_RET(FAILURE);

   idata->UIWin = WindowObject,
      MUIA_Window_Title,       GetS(SWS_L_WinTitle),
      MUIA_Window_ID,          Hash + TrackWinHash(Hash, TWC_Open),
      MUIA_Window_IsSubWindow, TRUE,
      MUIA_Window_AppWindow,   TRUE,
      WindowContents, RegisterGroup(Pages),
         MUIA_Background, MUII_RegisterBack,
         MUIA_CycleChain, 1,
         Child, ui,
         Child, ColGroup(2),
            Child, Label2(GetS(SWS_L_Desc)),
            Child, idata->ui_s1 = NewObject(SWObjStringClass(), NULL,
                                  StringFrame,
                                  MUIA_ShortHelp,     GetS(SWS_H_Desc),
                                  MUIA_CycleChain,    1,
                                  MUIA_String_MaxLen, SW_ATTRMAP[0].Max-1,
                                  TAG_DONE),
            
            Child, Label2(GetS(SWS_L_Enabled)),
            Child, HGroup, MUIA_Weight, 1,
               MUIA_ShortHelp,     GetS(SWS_H_Enabled),
               Child, idata->ui_b1 = CheckMark(idata->SWA_NAFilter_Enabled),
               Child, HSpace(0),
            End,
            Child, Label2(GetS(SWS_L_MarkRead)),
            Child, HGroup, MUIA_Weight, 1,
               MUIA_ShortHelp,     GetS(SWS_H_MarkRead),
               Child, ui_b2 = CheckMark(idata->SWA_NAFilter_MarkRead),
               Child, HSpace(0),
            End,
         End,
         Child, idata->Ed = NN_EditorObject,
            AppBaseDefTop(idata),
            SWA_NNews_Editor_InitStrAddr, &idata->SWA_NAFilter_AuthNote,
         End,
      End,
   End;

   // - set up notifications on various attributes ---------------------------

   if (!idata->UIWin) SWDB_RET(FAILURE);

   SetAttrs(idata->ui_b1, MUIA_HorizWeight, 1, MUIA_CycleChain, 1, TAG_DONE);
   SetAttrs(ui_b2,        MUIA_HorizWeight, 1, MUIA_CycleChain, 1, TAG_DONE);

   set(idata->ui_s1, MUIA_String_Contents, idata->SWA_NAFilter_Desc);

   DoMethod(idata->ui_s1, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NAFilter_Desc, MUIV_TriggerValue);

   DoMethod(idata->ui_b1, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NAFilter_Enabled, MUIV_TriggerValue);

   DoMethod(ui_b2, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NAFilter_MarkRead, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NAFilter_Desc, MUIV_EveryTime,
            idata->ui_s1, 3, MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NAFilter_Enabled, MUIV_EveryTime,
            idata->ui_b1, 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);

   DoMethod(msg->App, OM_ADDMEMBER, idata->UIWin);

   DoMethod(idata->UIWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            msg->App, 4, MUIM_Application_PushMethod, o, 1,
            SWM_NAFilter_HideUI);
   
   set(idata->UIWin, MUIA_Window_Open, TRUE);
   
   SWDB_RET(SUCCESS);
}


/*****************************************************************************
 * Method: Hide UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NAFilter_HideUI, Msg)
{
   STRPTR Buff;

   if (!idata->UIWin) SWDB_RET(0);

   // -- save contents of editor ---------------------------------------------
   
   if (Buff = (STRPTR)DoMethod(idata->Ed, MUIM_TextEditor_ExportText)) {
      set(o, SWA_NAFilter_AuthNote, Buff);
      FreeVec(Buff);
   }

   // -- be sure to nuke ALL the notifications we set up! --------------------

   DoMethod(o, MUIM_KillNotifyObj, SWA_NAFilter_Desc,      idata->ui_s1);
   DoMethod(o, MUIM_KillNotifyObj, SWA_NAFilter_Enabled,   idata->ui_b1);

   TrackWinHash(SWGetUL(o, SWA_ObjClassHash), TWC_Close);

   set(idata->UIWin, MUIA_Window_Open, FALSE);
   DoMethod(idata->App, OM_REMMEMBER, idata->UIWin);
   MUI_DisposeObject(idata->UIWin);
   idata->UIWin = NULL;
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Begin actions on messages                                         *
 *****************************************************************************/

METHOD(SWM_NAFilter_Begin, SWP_NAFilter_Begin *)
{
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Perform action on 1 message                                       *
 *****************************************************************************/

METHOD(SWM_NAFilter_Action, SWP_NAFilter_Action *)
{
   if (idata->SWA_NAFilter_MarkRead && msg->Art)
      DoMethod(msg->Art, SWM_NArticle_FlagSet, SWF_NArticle_Read);
   
   SWDB_RET(SWV_NAFilter_OK);
} SDFT
   

/*****************************************************************************
 * Method: End actions                                                       *
 *****************************************************************************/

METHOD(SWM_NAFilter_End, SWP_NAFilter_End *)
{
   SWDB_RET(0);
} SDFT
