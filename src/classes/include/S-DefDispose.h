
/*****************************************************************************
 *                                                                           *
 *   Dispose the root object's instance data                                 *
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

#define SW_DISPOSE
static __asm __inline void SWDispose(DISPOSEFNPROTO)
   SDSV
   SWP_XX(o, idata)
   SWDB_ASSERTNULL(o)
   SWDB_ASSERTNULL(idata)
{
#  ifdef DISPOSECUST
     DISPOSECUST;
#  endif

#  ifdef SWP_NList_DisposeCust
    SWP_NList_DisposeCust;
#  endif

   SWFreeAttrs(SW_ATTRMAP, (UBYTE *)idata);      // free variable values

#  ifdef DISPOSECUST_POST
      DISPOSECUST_POST;
#  endif
} SDFV
