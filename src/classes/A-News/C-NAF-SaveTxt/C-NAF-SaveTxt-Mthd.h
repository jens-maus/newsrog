/*****************************************************************************
 * Method: Start action                                                      *
 *****************************************************************************/

METHOD(SWM_NAFilter_Begin, SWP_NAFilter_Begin *)
{
   idata->Count = 0;
   
   SWDB_RET(SWV_NAFilter_NeedBody);
} SDFT


/*****************************************************************************
 * Method: perform action                                                    *
 *****************************************************************************/

METHOD(SWM_NAFilter_Action, SWP_NAFilter_Action *)
{
   STRPTR FSpec, SaveFile;
   ULONG  Exists;
   ULONG  Parts = idata->SWA_NAF_SaveTxt_HdrMode == SWV_NAF_SaveTxt_HM_All ?
                     SWF_NArticle_HdBd :
                  idata->SWA_NAF_SaveTxt_HdrMode == SWV_NAF_SaveTxt_HM_Head ?
                     SWF_NArticle_Head : SWF_NArticle_Body;
   
   if (!msg->Grp || !msg->Art) SWDB_RET(SWV_NAFilter_Error);

   idata->Count++;

   // -- generate the filename -----------------------------------------------

   idata->Grp = msg->Grp;
   idata->Art = msg->Art;

   SaveFile = FSpec  = VL_AllocGrp(msg->Grp, o, idata->SWA_NAF_SaveTxt_FSpec);
   if (SW_STRISNULL(FSpec)) goto Done;
   
   Exists = SWFileExists(FSpec);

   // -- create directory if asked to ----------------------------------------

   if (idata->SWA_NAF_SaveTxt_MkDir) {
      UBYTE *pp = PathPart(SaveFile), c = *pp;

      *pp = '\0';

      if (!SWDirExists(SaveFile))
         SWMakePath(SaveFile);

      *pp = c;
   }

   // -- silently skip, if in skip mode --------------------------------------

   if (Exists && idata->SWA_NAF_SaveTxt_Mode == SWV_NAF_SaveTxt_Mode_Skip)
      goto Done;

   // -- append suffix, if file exists ---------------------------------------

   if (Exists && idata->SWA_NAF_SaveTxt_Mode == SWV_NAF_SaveTxt_Mode_Suffix) {
      ULONG num;

      for (num=1; num<(1<<16); num++) {
         sprintf(SaveFile = idata->File, "%s.%04ld", FSpec, num);
         if (!SWFileExists(SaveFile)) break;
      }
   }

   // -- save the actual text ------------------------------------------------

   if (SaveFile != idata->File)
      strncpy(idata->File, SaveFile, FMSIZE-1);

   idata->File[FMSIZE-1] = '\0';

   DoMethod(o, SWM_NAF_SaveTxt_Save,
            msg->Grp, msg->Art, SaveFile, Parts,
            idata->SWA_NAF_SaveTxt_Mode == SWV_NAF_SaveTxt_Mode_Append);

   // -- run the command -----------------------------------------------------

   DoMethod(o, SWM_NAF_SaveTxt_Exec, msg->Grp, msg->Art);

   if (idata->SWA_NAF_SaveTxt_DelFile)
      DeleteFile(idata->File);

  Done:
      VL_FreeGrp(msg->Grp, o, FSpec);
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: End action                                                        *
 *****************************************************************************/

METHOD(SWM_NAFilter_End, SWP_NAFilter_End *)
{
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: save the article text                                             *
 *****************************************************************************/

METHOD(SWM_NAF_SaveTxt_Save, SWP_NAF_SaveTxt_Save *)
{
   SWDB_RET(DoMethod(msg->Art, SWM_NArticle_SaveText,
                     SWT_NArticle_FSpec,  msg->FSpec,
                     SWT_NArticle_Parts,  msg->Parts,
                     SWT_NArticle_Append, msg->Append,
                     TAG_DONE));
} SDFT
   

/*****************************************************************************
 * Method: save the article text                                             *
 *****************************************************************************/

METHOD(SWM_NAF_SaveTxt_Exec, SWP_NAF_SaveTxt_Exec *)
{
   STRPTR Cmd  = VL_AllocGrp(msg->Grp, o, idata->SWA_NAF_SaveTxt_Cmd_Command),
          Args = VL_AllocGrp(msg->Grp, o, idata->SWA_NAF_SaveTxt_Cmd_Args),
          Dir  = VL_AllocGrp(msg->Grp, o, idata->SWA_NAF_SaveTxt_Cmd_Dir),
          In   = VL_AllocGrp(msg->Grp, o, idata->SWA_NAF_SaveTxt_Cmd_Input),
          Out  = VL_AllocGrp(msg->Grp, o, idata->SWA_NAF_SaveTxt_Cmd_Output);

   if (!SW_STRISNULL(Cmd))
      DoMethod(o, SWM_Root_SysCmd,
               SWT_Root_InFile,  In,
               SWT_Root_OutFile, Out,
               SWT_Root_Cmd,     Cmd,
               SWT_Root_Arg,     Args,
               SWT_Root_Dir,     Dir,
               SWT_Root_Stack,   idata->SWA_NAF_SaveTxt_Cmd_Stack,
               SWT_Root_Pri,     idata->SWA_NAF_SaveTxt_Cmd_Pri,
               SWT_Root_ASync,   idata->SWA_NAF_SaveTxt_Cmd_ASync,
               TAG_DONE);

   VL_FreeGrp(msg->Grp, o, Cmd);
   VL_FreeGrp(msg->Grp, o, Args);
   VL_FreeGrp(msg->Grp, o, Dir);
   VL_FreeGrp(msg->Grp, o, In);
   VL_FreeGrp(msg->Grp, o, Out);

   SWDB_RET(0);
} SDFT
   

/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   ULONG rc;
   
   sprintf(idata->NumBuff, "%04ld", idata->Count);

   VL_FindRet("Num",  idata->NumBuff);
   VL_FindRet("File", idata->File);
   
   // -- search article object for variables ---------------------------------

   if (rc = DoMethodA(idata->Art, (Msg)msg))
      SWDB_RET(rc);

   // -- search group object for variables -----------------------------------

   SWDB_RET(DoMethodA(idata->Grp, (Msg)msg));
} SDFT

