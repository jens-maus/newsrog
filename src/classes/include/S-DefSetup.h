
/*****************************************************************************
 *                                                                           *
 *   Set up the root object's instance data                                  *
 *                                                                           *
 *   INPUTS:                                                                 *
 *                                                                           *
 *   OUTPUTS:                                                                *
 *                                                                           *
 *   AUTHOR:      Steve Koren                                                *
 *                                                                           *
 *   DATE:        Tue Jan  2 03:59:33 1996                                   *
 *                                                                           *
 *   ALGORITHM:                                                              *
 *                                                                           *
 *****************************************************************************/

# ifndef SW_SETUP
#define SW_SETUP
static __asm __inline ULONG SWSetup(SETUPFNPROTO)
   SDST(ULONG)
   SWP_XX(o, idata)
   SWDB_ASSERTNULL(o)
   SWDB_ASSERTNULL(idata)
{
   memset(idata, 0, sizeof(SW_IDATA));           // clear our data space
   SWInitAttrs(SW_ATTRMAP, (UBYTE *)idata, (Object *)retval);

#  ifdef SETUPCUST
     SETUPCUST;
#  endif

#  ifdef SWP_NList_SetupCust
    SWP_NList_SetupCust;
#  endif

   SWDB_RET(retval);
} SDFT
# endif
