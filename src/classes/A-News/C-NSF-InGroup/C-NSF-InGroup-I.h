# ifndef C_NSF_INGROUP_INCLUDE_I
#  define C_NSF_INGROUP_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-InGroup.h"
#  include "Libraries/C-NGroup.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_InGroup
#  define SW_CNAME            "NSF_InGroup"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_InGroup_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_FirstGroup = 1,

      SWS_H_FirstGroup,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG SWA_NSF_InGroup_FirstGrp;
      ULONG SWA_NSF_InGroup_Killed;
      STRPTR GroupName;     // cache this for extra speed;

      Object *UI;
      ULONG Total, Found, Inv;
   } SW_IDATA;

# endif // C_NSF_INGROUP_INCLUDE_I
