
static Object *TagWin(Object *o, ULONG *msg)
{
   return  GetTagObj(SWT_Root_SL_Win, SWGetObj(o, MUIA_WindowObject), msg);
}


/*****************************************************************************
 * Function: request filename & save or load to or from it                   *
 *****************************************************************************/

static ULONG SaveLoadFn(Object *o, ULONG SaveMode, ULONG *msg)
{
   UBYTE *fspec, *buff;
   ULONG rc;
   Object *Win   = TagWin(o, msg);

   DoMethod(o, SWM_ClearObjError);

   if (!(buff = AllocVec(FMSIZE+1, MEMF_ANY|MEMF_CLEAR))) {
      SWERR(o, NULL, SWS_Err_Malloc, 0, 0);
      goto Done;
   }
   
   fspec = (UBYTE *)DoMethod(o, SWM_Root_RequestFileName,
                             SWT_Root_SL_SaveMode, SaveMode,
                             SWT_Root_SL_Buffer,   buff,
                             SWT_Root_SL_Win,      Win,
                             SWT_Root_SL_Title, 
                                 GetS(SaveMode ? SWS_L_SaveAs : SWS_L_Open),
                             SWT_Root_SL_InitDrawer, 
                                GetTagStr(SWT_Root_SL_InitDrawer,  "PROGDIR:", msg),
                             SWT_Root_SL_InitPattern,
                                GetTagStr(SWT_Root_SL_InitPattern, "#?", msg),
                             TAG_DONE);

   if (fspec)
      rc = SaveMode ? DoMethod(o, SWM_Root_M_Save,
                               SWT_Root_SL_Name, fspec,
                               SWT_Root_SL_Win,  Win,
                               SWT_Root_SL_AskOverwrite,
                                   GetTagUL (SWT_Root_SL_AskOverwrite, TRUE, msg),
                               SWT_Root_SL_BackSuffix,
                                   GetTagStr(SWT_Root_SL_BackSuffix,  NULL,  msg),
                               TAG_DONE)
                    : DoMethod(o, SWM_Load, fspec);
   else
      rc = SaveMode ? FAILURE : NULL;

  Done:
   DoMethod(o, SWM_Root_CheckErr, Win);
   FreeVec(buff);
   
   return rc;
}


/*****************************************************************************
 * Method: request filename & load object from it                            *
 *****************************************************************************/

METHOD(SWM_Root_M_Open, ULONG *)
{
   SWDB_RET(SaveLoadFn(o, FALSE, msg+1));
} SDFT


/*****************************************************************************
 * Method: request filename & save object as it.                             *
 *****************************************************************************/

METHOD(SWM_Root_M_SaveAs, ULONG *)
{
   SWDB_RET(SaveLoadFn(o, TRUE, msg+1));
} SDFT


/*****************************************************************************
 * Open new object & assign to this one                                      *
 *****************************************************************************/

METHOD(SWM_Root_M_OpenAssign, ULONG *)
{
   ULONG mthd = msg[0];
   Object *temp;

   msg[0] = SWM_Root_M_Open;
   temp   = (Object *)DoMethodA(o, (Msg)msg);
   msg[0] = mthd;

   if (temp) {
      DoMethod(o, SWM_Root_AssignSafe, temp, TRUE);
      SWDB_RET(SUCCESS);
   }

   SWDB_RET(FAILURE);
} SDFT


/*****************************************************************************
 * Method: save, but ask before overwriting                                  *
 *****************************************************************************/

METHOD(SWM_Root_M_Save, ULONG *)
{
   Object *Win   = TagWin(o, msg+1);
   ULONG  AskOV  = GetTagUL (SWT_Root_SL_AskOverwrite, TRUE, msg+1);
   STRPTR Name   = GetTagStr(SWT_Root_SL_Name,         NULL, msg+1);
   STRPTR BackSfx= GetTagStr(SWT_Root_SL_BackSuffix,   NULL, msg+1);
   ULONG lock;
   TEXT   BackFName[FMSIZE];

   if (SW_STRISNULL(Name)) SWDB_RET(FAILURE);

   if (lock = Lock(Name, ACCESS_READ)) UnLock(lock);

   // -- prompt before saving, and don`t overwrite if asked not to -----------
   
   if (lock && AskOV)
      if (MUI_Request(_app(Win), Win, 0, NULL, GetS(SWS_B_Overwrite),
                      GetS(SWS_L_FileExists), Name) != 1) {
         MUI_Request(_app(Win), Win, 0, NULL, GetS(SWS_B_Ok),
                     GetS(SWS_L_WriteCanceled));
         SWDB_RET(FAILURE);
      }

   // -- make backup file -------------------------------------------------

   if (lock && !SW_STRISNULL(BackSfx)) {
      DeleteFile(strcat(strcpy(BackFName, Name), BackSfx));
      Rename(Name, BackFName);
   }
   
   SWDB_RET(DoMethod(o, SWM_Save, Name));
} SDFT
