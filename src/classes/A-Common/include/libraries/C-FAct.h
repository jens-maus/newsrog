# ifndef C_FACT_INCLUDE
#  define C_FACT_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "S-SysCmdVars.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 2700)

   enum {
      SWA_FAct_Type   = SW_FIRSTTAG,       // [SGI] media type
      SWA_FAct_Action,                     // [SGI] action name
      SWP_SysCmdAttrVals(SWA_FAct),        // [SGI] syscmd stuff
      SWA_FAct_CmdType,                    // [SGI] CLI/WB/ARexx
   };

   enum {
      SWM_FAct_Execute,                    // execute the action
   };

   enum {
      SWV_FAct_CLI,
      SWV_FAct_ARexx,
      SWV_FAct_WB,
   };

   SWM_P(SWP_FAct_Execute,
         Object *VarList;
         Object *VarObj);

#  undef SW_FIRSTTAG   

# endif
