
/*****************************************************************************
 * Check & display error for object                                          *
 *****************************************************************************/

METHOD(SWM_Root_CheckErr, SWP_CheckErr *)
{
   if (getlent(o, SWA_ObjError)) {
      Object *App = SWGetObj(o, MUIA_ApplicationObject);
      Object *Win = msg->Win;
      
      if (!Win) Win = SWGetObj(o, MUIA_WindowObject);

      if (!App && Win) App = SWGetObj(Win, MUIA_ApplicationObject);

      if (App) {
         UBYTE *text = (UBYTE *)getlent(o, SWA_ObjErrorText);
         ULONG p1    = (ULONG)  getlent(o, SWA_ObjErrorParam1);
         ULONG p2    = (ULONG)  getlent(o, SWA_ObjErrorParam2);
   
         if (Win) MUI_Request(App, Win, 0, NULL, GetS(SWS_B_Ok), text, p1, p2);
      }
   
      DoMethod(o, SWM_ClearObjError);

      SWDB_RET(1);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Check error for any children as well                                      *
 *****************************************************************************/

METHOD(SWM_Root_CheckErrAll, SWP_CheckErr *)
{
   DoMethod(o, SWM_Root_CheckErr, msg->Win);
   DoMethod(o, SWM_SendMsg, msg, NULL);

   SWDB_RET(0);
} SDFT
