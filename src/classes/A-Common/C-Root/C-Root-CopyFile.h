/*****************************************************************************
 * Method: Copy one file to another                                          *
 *****************************************************************************/

METHOD(SWM_Root_CopyFile, ULONG *)
{
   ULONG  Del = GetTagUL(SWT_Root_CF_Del,      FALSE, msg+1);
   ULONG  BS  = GetTagUL(SWT_Root_CF_BuffSize, 32768, msg+1);
   ULONG  MP  = GetTagUL(SWT_Root_CF_MakePath, FALSE, msg+1);
   STRPTR Src = GetTagStr(SWT_Root_CF_Src,     NULL,  msg+1);
   STRPTR Trg = GetTagStr(SWT_Root_CF_Trg,     NULL,  msg+1);

   ULONG rc = SUCCESS;
   LONG  InBytes;
   BPTR in = NULL, out = NULL;
   UBYTE *Buff = NULL, *dp, c;

   if (SW_STRISNULL(Trg) || SW_STRISNULL(Src)) {
      rc = SWERR(o, FAILURE, SWS_Err_NullFileName, 0, 0);
      goto Done;
   }

   if (MP) {
      c   = *(dp = PathPart(Trg));
      *dp = '\0';
      if (SWMakePath(Trg) == FAILURE) {
         rc = SWERR(o, FAILURE, SWS_Err_MakeDir, Trg, 0);
         goto Done;
      }

      *dp = c;
   }

   if (!(Buff = AllocVec(BS, MEMF_ANY))) {
      rc = SWERR(o, FAILURE, SWS_Err_Malloc, 0, 0);
      goto Done;
   }

   if (!(in = Open(Src, MODE_OLDFILE))) {
      rc = SWERR(o, FAILURE, SWS_Err_FileRead, Src, 0);
      goto Done;
   }

   if (!(out = Open(Trg, MODE_NEWFILE))) {
      rc = SWERR(o, FAILURE, SWS_Err_FileWrite, Trg, 0);
      goto Done;
   }

   while ((InBytes = Read(in, Buff, BS)) > 0) {
      if (Write(out, Buff, InBytes) != InBytes) {
         rc = SWERR(o, FAILURE, SWS_Err_FileWrite, Trg, 0);
         goto Done;
      }
   }
   
   if (Del) DeleteFile(Src);

  Done:
   FreeVec(Buff);
   if (in)  Close(in);
   if (out) Close(out);
   
   SWDB_RET(rc);
} SDFT
