/*****************************************************************************
 * Method: Execute a command                                                 *
 *****************************************************************************/

METHOD(SWM_Root_SysCmd, ULONG *)
{
   LONG err;
   struct TagItem *ti = (struct TagItem *)(msg+1);
   BPTR infh = NULL, outfh = NULL, dirlock = NULL;
   UBYTE *in  = (UBYTE *)GetTagData(SWT_Root_InFile,  (ULONG)"NIL:", ti),
         *out = (UBYTE *)GetTagData(SWT_Root_OutFile, (ULONG)"NIL:", ti),
         *cmd = (UBYTE *)GetTagData(SWT_Root_Cmd,     NULL,          ti),
         *arg = (UBYTE *)GetTagData(SWT_Root_Arg,     NULL,          ti),
         *dir = (UBYTE *)GetTagData(SWT_Root_Dir,     NULL,          ti),
         *buf = cmd;
   ULONG stk  = GetTagData(SWT_Root_Stack, 4096,  ti),
         pri  = GetTagData(SWT_Root_Pri,      0,  ti),
         ASync= GetTagData(SWT_Root_ASync, TRUE,  ti);

   if (SW_STRISNULL(cmd)) goto Error;

   err = 5;

   // -- if we have arguments, build a string of cmd + " " + args ------------
   
   if (!SW_STRISNULL(arg)) {
      if (!(buf = AllocVec(strlen(cmd)+strlen(arg)+2, MEMF_ANY))) goto Error;
      strcat(strcat(strcpy(buf, cmd), " "), arg);
   }

   // -- open input & input filehandles --------------------------------------

   if (!(infh  = Open(in, MODE_OLDFILE))) {
      err = SWERR(o, 5, SWS_Err_OpenInput,  in,  0);
      goto Error;
   }
      
   if (!(outfh = Open(out, MODE_NEWFILE))) {
      err = SWERR(o, 5, SWS_Err_OpenOutput, out, 0);
      goto Error;
   }

   if (!SW_STRISNULL(dir))
      if (!(dirlock = Lock(dir, ACCESS_READ))) {
         err = SWERR(o, 5, SWS_Err_Lockdir, dir, 0);
         goto Error;
      }
   
   // -- finally run the command ---------------------------------------------
   
   err = SystemTags(buf, SYS_Input,    (APTR)infh,
                         SYS_Output,   (APTR)outfh,
                         SYS_Asynch,   ASync,
                         NP_StackSize, stk,
                         NP_Priority,  pri,
                         dirlock ? NP_CurrentDir : TAG_IGNORE, dirlock,
                         TAG_DONE);

   if (err) {
      SWERR(o, NULL, SWS_Err_RunCmd, 0, 0);
      goto Error;
   }

   goto Normal; // don`t close filehandles on normal return if ASync

  Error:
   if (infh)    { Close(infh);  infh = NULL;  }
   if (outfh)   { Close(outfh); outfh = NULL; }
   if (dirlock) UnLock(dirlock);
   // fall through to free, etc...

  Normal:
   if (!ASync)  {
      if (infh)    Close(infh);
      if (outfh)   Close(outfh);
   }
   
   if (buf != cmd) FreeVec(buf);

   SWDB_RET((ULONG)err);
} SDFT
