/*****************************************************************************
 * Method: default dispatcher                                                *
 *****************************************************************************/

# ifndef SW_METHOD
# ifdef SW_Methods
# define SW_METHOD

static __asm __inline ULONG SWMethod(METHODFNPROTO) \
   SDST(ULONG)
   SWP_XXX(cl, o, msg)
   SWDB_ASSERTCLFN()
{
   SW_IDATA *idata = INST_DATA(cl, o);

   switch (msg->MethodID) {
      SW_Methods;
   }

#  ifdef SW_LISTMETHODS
      switch (msg->MethodID) {
#       include "S-Lists-MethodCall.h"
      }
#  endif

#  ifndef SW_NOROOTMETHODS
      switch (msg->MethodID) {
#        include "S-RootMethodCall.h"
      }
#  endif

  SuperMethod:
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT

# endif
# endif
