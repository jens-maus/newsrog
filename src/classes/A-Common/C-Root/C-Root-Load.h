/*****************************************************************************
 * load object from specified filename                                       *
 *****************************************************************************/

METHOD(SWM_Load, SWP_Load *)
{
   ULONG rc;
   BPTR  fp;
   UBYTE cookie[32];
   Object *App = SWGetObj(o, MUIA_ApplicationObject);

   if (SW_STRISNULL(msg->Name))
      SWDB_RET(SWERR(o, NULL, SWS_Err_NullFileName, 0, 0));

   SWERR_P(msg->Name, 0);
   
   if (!(fp = Open(msg->Name, MODE_OLDFILE)))
      SWDB_RET(SWERR(o, NULL, SWS_Err_FileOpenRead, msg->Name, 0));

   if (App) set(App, MUIA_Application_Sleep, TRUE);
   
   if (FGets(fp, cookie, 31) == NULL) {
      rc = SWERR(o, NULL, SWS_Err_FileRead, msg->Name, 0);
      goto Done;
   }
   
   cookie[30] = NULL;
   SW_REMOVENL(cookie);

   // -- verify cookie - make sure we`re what we think we are ----------------
   
   if (strcmp(cookie, SAVE_COOKIE)) {
      rc = SWERR(o, NULL, SWS_Err_FileFormat, msg->Name, 0);
      goto Done;
   }
   
   if (!(rc = DoMethod(o, SWM_Read, fp)))
      SWERR(o, NULL, SWS_Err_FileRead, msg->Name, 0);

  Done:
   if (App) set(App, MUIA_Application_Sleep, FALSE);
   Close(fp);
   SWDB_RET(rc);
} SDFT

