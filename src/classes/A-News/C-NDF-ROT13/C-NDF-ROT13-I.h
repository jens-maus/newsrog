# ifndef C_NDF_ROT13_INCLUDE_I
#  define C_NDF_ROT13_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-ROT13.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_ROT13
#  define SW_CNAME            "NDF_ROT13"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_ROT13_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

#  define TCount 4
   
   enum {
      SWS_H_ROT13 = 1,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      STRPTR SWA_NDF_ROT13_UNUSED;
      ULONG  SWA_NDF_ROT13_UNUSED2;
      ULONG  SWA_NDF_ROT13_UNUSED3;
   } SW_IDATA;

# endif // C_NDF_ROT13_INCLUDE_I
