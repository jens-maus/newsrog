/*****************************************************************************
 *                                                                           *
 *   Handle the get methods for attributes this class understands.           *
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

#define SW_GET
static __asm __inline ULONG SWGet(GETFNPROTO)
   SDST(ULONG)
   SWP_XXX(cl, o, msg)
   SWDB_ASSERTCLFN()
{
#  ifdef SW_USERGET
     SW_USERGET;
#  endif

   if (SWGetAttr(SW_ATTRMAP, (UBYTE *)INST_DATA(cl, o),
                 ((opGet *)msg)->opg_AttrID,
                 (ULONG *)((opGet *)msg)->opg_Storage))
      SWDB_RET(1);
     
   SWDB_RET((ULONG)DoSuperMethodA(cl, o, msg));
} SDFT
