# ifndef C_NSF_SIZERANGE_INCLUDE_I
#  define C_NSF_SIZERANGE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-SizeRange.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_SizeRange
#  define SW_CNAME            "NSF_SizeRange"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_SizeRange_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_MinSize = 1,
      SWS_L_MaxSize,
      SWS_L_LE,
      SWS_L_ASize,

      SWS_H_MinMax,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG SWA_NSF_SizeRange_Min;
      ULONG SWA_NSF_SizeRange_Max;

      Object *UI;
      ULONG Total, Found, Inv;
   } SW_IDATA;

# endif // C_NSF_SIZERANGE_INCLUDE_I
