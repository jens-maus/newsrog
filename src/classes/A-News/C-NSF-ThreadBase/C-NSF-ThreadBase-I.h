# ifndef C_NSF_THREADBASE_INCLUDE_I
#  define C_NSF_THREADBASE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-ThreadBase.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_ThreadBase
#  define SW_CNAME            "NSF_ThreadBase"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_ThreadBase_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_H_TS = 1,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      Object *TM;
      ULONG Total, Found, Inv;
   } SW_IDATA;

# endif // C_NSF_THREADBASE_INCLUDE_I
