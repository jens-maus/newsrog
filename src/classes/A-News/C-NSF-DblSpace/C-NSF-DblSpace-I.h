# ifndef C_NSF_DBLSPACE_INCLUDE_I
#  define C_NSF_DBLSPACE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-DblSpace.h"
#  include "Libraries/C-NGroup.h"
#  include <ctype.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_DblSpace
#  define SW_CNAME            "NSF_DblSpace"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_DblSpace_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_MinLen = 1,
      SWS_L_Pct,
      SWS_L_Chars,

      SWS_H_MinLen,
      SWS_H_Pct,
      SWS_H_Chars,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG  SWA_NSF_DblSpace_MinLen;
      ULONG  SWA_NSF_DblSpace_Pct;
      STRPTR SWA_NSF_DblSpace_Chars;

      Object *UI;
      ULONG Total, Found, Inv;
      UBYTE DblChars[256];
   } SW_IDATA;

# endif // C_NSF_DBLSPACE_INCLUDE_I
