# ifndef C_NSF_AGERANGE_INCLUDE_I
#  define C_NSF_AGERANGE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-AgeRange.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_AgeRange
#  define SW_CNAME            "NSF_AgeRange"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_AgeRange_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_MinAge = 1,
      SWS_L_MaxAge,
      SWS_L_LE,
      SWS_L_AAge,

      SWS_H_MinMax,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG SWA_NSF_AgeRange_Min;
      ULONG SWA_NSF_AgeRange_Max;

      Object *UI;
      ULONG Total, Found, Inv;
   } SW_IDATA;

# endif // C_NSF_AGERANGE_INCLUDE_I
