
# include "S-MCCHeader.h"

// -- Class library open/close functions -------------------------------------

Library *SWSupportBase;
SWLibraries SWLibs;

/*****************************************************************************
 *                                                                           *
 *   Library close fn - close the libraries we opened, etc                   *
 *                                                                           *
 *   INPUTS:                                                                 *
 *                                                                           *
 *   OUTPUTS:                                                                *
 *                                                                           *
 *   AUTHOR:      Steve Koren                                                *
 *                                                                           *
 *   DATE:        Mon Jan  1 16:51:53 1996                                   *
 *                                                                           *
 *   ALGORITHM:                                                              *
 *                                                                           *
 *****************************************************************************/

void ClassExitFunc(Library *libbase)
{
   SW_SHUTDOWN();

   SWCloseLibs(SW_LIBSTRUCTVER, &SWLibs);
   SW_CLOSELIB(SWSupportBase);
}


/*****************************************************************************
 *                                                                           *
 *   User library init function - open all the libs we need.                 *
 *                                                                           *
 *   INPUTS:                                                                 *
 *                                                                           *
 *   OUTPUTS:                                                                *
 *                                                                           *
 *   AUTHOR:      Steve Koren                                                *
 *                                                                           *
 *   DATE:        Mon Jan  1 16:51:25 1996                                   *
 *                                                                           *
 *   ALGORITHM:                                                              *
 *                                                                           *
 *****************************************************************************/

BOOL ClassInitFunc(Library *libbase)
{
   if (!(SWSupportBase = OpenLibrary(SWSUPPORTNAME, SWSUPPORTVER)) ||
       SWOpenLibs(SW_LIBSTRUCTVER, &SWLibs)                        ||
       SW_STARTUP(libbase)) goto Fail;
  
   return TRUE;

  Fail:
   ClassExitFunc(libbase);
   return FALSE;
}

