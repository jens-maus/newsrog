# ifndef C_NSF_LINERANGE_INCLUDE_I
#  define C_NSF_LINERANGE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-LineRange.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_LineRange
#  define SW_CNAME            "NSF_LineRange"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_LineRange_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_MinLines = 1,
      SWS_L_MaxLines,
      SWS_L_LE,
      SWS_L_ALines,

      SWS_H_MinMax,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG SWA_NSF_LineRange_Min;
      ULONG SWA_NSF_LineRange_Max;

      Object *UI;
      ULONG Total, Found, Inv;
   } SW_IDATA;

# endif // C_NSF_LINERANGE_INCLUDE_I
