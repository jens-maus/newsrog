# ifndef C_NSF_SUBJMATCH_INCLUDE_I
#  define C_NSF_SUBJMATCH_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-SubjMatch.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_SubjMatch
#  define SW_CNAME            "NSF_SubjMatch"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_SubjMatch_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Str = 1,
      SWS_L_Case,
      SWS_L_SubStr,

      SWS_H_Str,
      SWS_H_Case,
      SWS_H_SubStr,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      STRPTR SWA_NSF_SubjMatch_Str;
      ULONG  SWA_NSF_SubjMatch_Case;
      ULONG  SWA_NSF_SubjMatch_MatchMsgID;
      ULONG  SWA_NSF_SubjMatch_SubStr;

      Object *UI;
      ULONG Total, Found, Inv, PatLen;
   } SW_IDATA;

# endif // C_NSF_SUBJMATCH_INCLUDE_I
