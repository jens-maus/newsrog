/*****************************************************************************
 *                                                                           *
 *   Handle the set methods for attributes this class understands.           *
 *                                                                           *
 *   INPUTS:                                                                 *
 *                                                                           *
 *   OUTPUTS:                                                                *
 *                                                                           *
 *   AUTHOR:      Steve Koren                                                *
 *                                                                           *
 *   DATE:        Tue Jan  2 04:00:00 1996                                   *
 *                                                                           *
 *   ALGORITHM:                                                              *
 *                                                                           *
 *****************************************************************************/

# ifndef SW_SET
#define SW_SET
static __asm ULONG SWSet(SETFNPROTO)
   SDST(ULONG)
{
   SW_IDATA *idata = INST_DATA(cl, o);
   
#  ifdef SW_USERSET
     SW_USERSET;
#  endif

   SWDB_RET(SWSetAttr(SW_ATTRMAP, (UBYTE *)idata, Tag, Data));
} SDFT
# endif
