
/*****************************************************************************
 * Method: Run action                                                        *
 *****************************************************************************/

METHOD(SWM_FAct_Execute, SWP_FAct_Execute *)
{
   ULONG rc;
   STRPTR  Cmd, Args, In, Out, Dir;

   Cmd  = idata->SWA_FAct_Command;
   Args = idata->SWA_FAct_Args;
   Dir  = idata->SWA_FAct_Dir;
   In   = idata->SWA_FAct_Input;
   Out  = idata->SWA_FAct_Output;
   
   if (msg->VarList && msg->VarObj) {
      Cmd  = VL_Alloc(msg->VarList, msg->VarObj, Cmd);
      Args = VL_Alloc(msg->VarList, msg->VarObj, Args);
      Dir  = VL_Alloc(msg->VarList, msg->VarObj, Dir);
      In   = VL_Alloc(msg->VarList, msg->VarObj, In);
      Out  = VL_Alloc(msg->VarList, msg->VarObj, Out);
   }

   if (SW_STRISNULL(Cmd))
      rc = SWERR(o, FAILURE, SWS_Err_NoCmd,
                 idata->SWA_FAct_Action, idata->SWA_FAct_Type);
   else if (idata->SWA_FAct_CmdType == SWV_FAct_CLI)
      rc = DoMethod(o, SWM_Root_SysCmd,
                    SWT_Root_InFile,  SW_STRDEF(In,  "NIL:"),
                    SWT_Root_OutFile, SW_STRDEF(Out, "NIL:"),
                    SWT_Root_Cmd,     Cmd,
                    SWT_Root_Arg,     Args,
                    SWT_Root_Dir,     Dir,
                    SWT_Root_Stack,   idata->SWA_FAct_Stack,
                    SWT_Root_Pri,     idata->SWA_FAct_Pri,
                    SWT_Root_ASync,   idata->SWA_FAct_ASync,
                    TAG_DONE);
   else if (idata->SWA_FAct_CmdType == SWV_FAct_ARexx)
      if ((rc = SWSendRexxCmd(Cmd, Args, NULL, NULL, 0)) == RC_FATAL)
         rc = SWERR(o, FAILURE, SWS_Err_RexxErr,
                    idata->SWA_FAct_Action, idata->SWA_FAct_Type);
         

   if (msg->VarList && msg->VarObj) {
      VL_Free(msg->VarList, msg->VarObj, Cmd);
      VL_Free(msg->VarList, msg->VarObj, Args);
      VL_Free(msg->VarList, msg->VarObj, Dir);
      VL_Free(msg->VarList, msg->VarObj, In);
      VL_Free(msg->VarList, msg->VarObj, Out);
   }
   
   SWDB_RET(rc);
} SDFT
