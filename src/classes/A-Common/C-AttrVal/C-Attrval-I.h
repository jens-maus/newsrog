# ifndef C_ATTRVAL_INCLUDE_I
#  define C_ATTRVAL_INCLUDE_I

#  include "Libraries/C-Invocation.h"
#  include "Libraries/C-Attrval.h"


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_Attrval
#  define SW_CNAME            "Attrval"
#  define SW_SUPERPUB         SWC_Root

#  include "S-Params.h"
#  include "Proto/Attrval_protos.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      STRPTR       SWA_Attrval_Attr;
      STRPTR       SWA_Attrval_Val;
      ULONG        SWA_Attrval_Num;
   } SW_IDATA;

# endif // C_ATTRVAL_INCLUDE_I
