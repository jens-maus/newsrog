/*****************************************************************************
 * MUI setup method                                                          *
 *****************************************************************************/

METHOD(SWM_Timer_Install, SWP_Timer_Install *)
{
   Object *App = msg->App ? msg->App : _app(msg->Target);
   if (idata->Installed) SWDB_RET(TRUE);

   if (msg->Target) idata->SWA_Timer_Target = msg->Target;

   idata->SWA_Timer_IHNode.ihn_Millis = idata->TMillis;

   idata->App = App;
   DoMethod(App, MUIM_Application_AddInputHandler, &idata->SWA_Timer_IHNode);

   idata->Installed = TRUE;

   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * MUI cleanup method                                                        *
 *****************************************************************************/

METHOD(SWM_Timer_Remove, Msg)
{
   if (!idata->Installed) SWDB_RET(TRUE);

   DoMethod(idata->App, MUIM_Application_RemInputHandler,
            &idata->SWA_Timer_IHNode);

   idata->Installed = FALSE;
   
   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Trigger method - reset timer                                              *
 *****************************************************************************/

METHOD(SWM_Timer_Trigger, Msg)
{
   if (++idata->TNow >= idata->TCount) {
      idata->TNow = 0;
      if (idata->SWA_Timer_Target)
         DoMethod(idata->SWA_Timer_Target, idata->SWA_Timer_Method,
                  idata->SWA_Timer_UData);
   }
      
   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Create our edit UI                                                        *
 *****************************************************************************/

extern ClassT *SW_CLASS;

METHOD(SWM_Invocation_MakeEditUI, Msg)
{
   Object *ui = NewObject(SWTimesliderClass(), NULL,
         MUIA_Numeric_Min,        idata->SWA_Timer_Min,
         MUIA_Numeric_Max,        idata->SWA_Timer_Max,
         MUIA_Numeric_Value,      idata->SWA_Timer_Millis,
         MUIA_CycleChain,         1,
         MUIA_UserData,           SWA_Timer_Millis,
         TAG_DONE);

   SW_NOTIFYSAME(o, SWA_Timer_Millis, ui, MUIA_Numeric_Value);
   SW_NOTIFYSAME(ui, MUIA_Numeric_Value, o, SWA_Timer_Millis);
   
   SWDB_RET((ULONG)ui);
} SDFT
