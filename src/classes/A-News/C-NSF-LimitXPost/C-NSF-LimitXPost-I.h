# ifndef C_NSF_LIMITXPOST_INCLUDE_I
#  define C_NSF_LIMITXPOST_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-LimitXPost.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_LimitXPost
#  define SW_CNAME            "NSF_LimitXPost"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_LimitXPost_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Min = 1,
      SWS_L_Max,
      SWS_L_LE,
      SWS_L_AGroups,

      SWS_H_MinMax,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG SWA_NSF_LimitXPost_Min;
      ULONG SWA_NSF_LimitXPost_Max;

      Object *UI;
      ULONG Total, Found, Inv;
   } SW_IDATA;

# endif // C_NSF_LIMITXPOST_INCLUDE_I
