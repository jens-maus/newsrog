# ifndef C_OBJECTLIST_INCLUDE_I
#  define C_OBJECTLIST_INCLUDE_I

#  include "Libraries/C-ObjectList.h"
#  include "Libraries/C-ClassList.h"
#  include "Libraries/C-Invocation.h"


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_Objectlist
#  define SW_CNAME            "Objectlist"
#  define SW_SUPERPUB         MUIC_Listtree
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/Objectlist_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_ObjName = 1,
      SWS_Class,
      SWS_ClassDesc,

      SWS_Err_FindClass,
      SWS_Err_ObjAlloc,

      SWS_H_OList,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA { Object *OGroup; } SW_IDATA;

# endif // C_OBJECTLIST_INCLUDE_I
