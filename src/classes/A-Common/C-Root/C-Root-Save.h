/*****************************************************************************
 * write object to specified filename                                        *
 *****************************************************************************/

METHOD(SWM_Save, SWP_Save *)
{
   ULONG rc = SUCCESS;
   BPTR  fp;
   Object *App = SWGetObj(o, MUIA_ApplicationObject);
   
   if (SW_STRISNULL(msg->Name))
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_NullFileName, 0, 0));

   SWERR_P(msg->Name, 0);

   if (!(fp = Open(msg->Name, MODE_NEWFILE)))
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_FileOpenWrite, msg->Name, 0));

   if (App) set(App, MUIA_Application_Sleep, TRUE);
   
   if (FPuts(fp, SAVE_COOKIE)  ||
       FPutC(fp, '\n') != '\n' ||
       DoMethod(o, SWM_Write, fp))
      rc = SWERR(o, FAILURE, SWS_Err_FileWrite, msg->Name, 0);
      
   Close(fp);

   if (App) set(App, MUIA_Application_Sleep, FALSE);
   
   SWDB_RET(rc);
} SDFT

