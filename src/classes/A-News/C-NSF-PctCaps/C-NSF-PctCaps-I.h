# ifndef C_NSF_PctCaps_INCLUDE_I
#  define C_NSF_PctCaps_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-PctCaps.h"
#  include "Libraries/C-NGroup.h"
#  include <ctype.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_PctCaps
#  define SW_CNAME            "NSF_PctCaps"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_PctCaps_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_MinLen = 1,
      SWS_L_Pct,

      SWS_H_MinLen,
      SWS_H_Pct,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG  SWA_NSF_PctCaps_MinLen;
      ULONG  SWA_NSF_PctCaps_Pct;

      Object *UI;
      ULONG Total, Found, Inv;
   } SW_IDATA;

# endif // C_NSF_PctCaps_INCLUDE_I
