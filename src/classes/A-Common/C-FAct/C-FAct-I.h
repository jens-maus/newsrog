# ifndef C_FACT_INCLUDE_I
#  define C_FACT_INCLUDE_I

#  include "Libraries/C-Invocation.h"
#  include "Libraries/C-FAct.h"
#  include "Libraries/C-VarList.h"


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_FAct
#  define SW_CNAME            "FAct"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/FAct_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Type = 1,
      SWS_L_Action,

      SWS_H_Type,
      SWS_H_Action,
      
      SWP_SysCmdLabs,
      SWP_SysCmdHelpLabs,

      SWS_Err_NoCmd,
      SWS_Err_RexxErr,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      STRPTR       SWA_FAct_Type;
      STRPTR       SWA_FAct_Action;
      SWP_SysCmdAttrDefs(SWA_FAct);
      ULONG        SWA_FAct_CmdType;
   } SW_IDATA;


#  define VL_Alloc(vl, o, Str) \
   (STRPTR)DoMethod((vl), SWM_VarList_ReplaceValues, (Str), (o))

#  define VL_Free(vl, o, Str) \
   DoMethod((vl), SWM_VarList_FreeString, (Str), (o))

# endif // C_FACT_INCLUDE_I
