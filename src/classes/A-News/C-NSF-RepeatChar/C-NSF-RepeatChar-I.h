# ifndef C_NSF_REPEATCHAR_INCLUDE_I
#  define C_NSF_REPEATCHAR_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-RepeatChar.h"
#  include "Libraries/C-NGroup.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_RepeatChar
#  define SW_CNAME            "NSF_RepeatChar"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_RepeatChar_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_MaxC = 1,
      SWS_L_Chars,
      SWS_L_Set,

      SWS_H_MaxC,
      SWS_H_Chars,
      SWS_H_Set,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG  SWA_NSF_RepeatChar_MaxCount;
      STRPTR SWA_NSF_RepeatChar_Chars;
      ULONG  SWA_NSF_RepeatChar_Set;

      UBYTE  Chars[256];

      Object *UI;
      ULONG Total, Found, Inv;
   } SW_IDATA;

# endif // C_NSF_REPEATCHAR_INCLUDE_I
