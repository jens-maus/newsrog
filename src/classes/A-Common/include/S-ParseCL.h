/*****************************************************************************
 * Generate a command line from either WB startup or CLI args                *
 *****************************************************************************/

static STRPTR SWParseCmdLine(int argc, char *argv[])
{
   STRPTR *ttypes, CmdBuff;
   
   CmdBuff = SWGetCmdLine(ttypes = ArgArrayInit(argc, argv));

   if (ttypes) ArgArrayDone();

   return CmdBuff;
}
