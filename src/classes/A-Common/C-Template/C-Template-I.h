# ifndef C_TEMPLATE_INCLUDE_I
#  define C_TEMPLATE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-Template.h"


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_Template
#  define SW_CNAME            "Template"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/Template_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_String1 = 1,
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

#  ifdef SW_IDATA
   typedef struct SW_IDATA
   {
      ULONG SWA_Attr1;
   } SW_IDATA;
#  endif


# endif // C_TEMPLATE_INCLUDE_I
