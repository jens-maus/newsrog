# include "C-Template-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_Template

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_Attr1,  SWAM_NoLab, SWAM_SGI,  0),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "String1",
   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


# include "C-Template-Mthd.h"
# include "S-DefAll.h"


/*****************************************************************************
 *   Handle the methods this class understands.                              *
 *****************************************************************************/

#define SW_METHOD
static __asm __inline ULONG SWMethod(METHODFNPROTO)
   SDST(ULONG)
   SWP_XXX(cl, o, msg)
   SWDB_ASSERTCLFN()
{
   SW_IDATA *idata = INST_DATA(cl, o);

   switch (msg->MethodID) {
      SW_DO_MF(MUIM_Show);

#     include "S-RootMethodCall.h"
   }

  SuperMethod:
   SWDB_RET((ULONG)DoSuperMethodA(cl, o, msg));
} SDFT

# include "S-Dispatcher.h"
