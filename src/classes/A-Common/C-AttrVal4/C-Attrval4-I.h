# ifndef C_ATTRVAL4_INCLUDE_I
#  define C_ATTRVAL4_INCLUDE_I

#  include "Libraries/C-Invocation.h"
#  include "Libraries/C-Attrval4.h"


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_Attrval4
#  define SW_CNAME            "Attrval4"
#  define SW_SUPERPUB         SWC_Root

#  include "S-Params.h"
#  include "Proto/Attrval4_protos.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      STRPTR       SWA_Attrval4_Attr;
      STRPTR       SWA_Attrval4_Val0;
      STRPTR       SWA_Attrval4_Val1;
      STRPTR       SWA_Attrval4_Val2;
      STRPTR       SWA_Attrval4_Val3;
      ULONG        SWA_Attrval4_Num0;
      ULONG        SWA_Attrval4_Num1;
      ULONG        SWA_Attrval4_Num2;
      ULONG        SWA_Attrval4_Num3;
   } SW_IDATA;

# endif // C_ATTRVAL4_INCLUDE_I
