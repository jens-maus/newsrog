# ifndef C_TIMER_INCLUDE_I
#  define C_TIMER_INCLUDE_I

#  include "Libraries/C-Invocation.h"
#  include "Libraries/C-Timer.h"


   // -- this classes names, etc ---------------------------------------------

#  define SW_CLIB             SWC_Timer
#  define SW_CNAME            "Timer"
#  define SW_SUPERPUB         SWC_Root

#  include "S-Params.h"
#  include "Proto/Timer_protos.h"


   // -- our instance specific data ------------------------------------------

#  define SWV_Timer_MaxMillis 32000

   typedef struct SW_IDATA
   {
      LONG                          SWA_Timer_Millis;
      LONG                          SWA_Timer_Running;
      LONG                          SWA_Timer_Min;
      LONG                          SWA_Timer_Max;
      struct timerequest           *SWA_Timer_Req;
      struct MUI_InputHandlerNode   SWA_Timer_IHNode;
      Object                       *SWA_Timer_Target;
      ULONG                         SWA_Timer_Method;
      ULONG                         SWA_Timer_UData;
      ULONG                         Installed;
      ULONG                         TMillis;
      ULONG                         TCount;
      ULONG                         TNow;
      Object                       *App;
   } SW_IDATA;


# endif // C_TIMER_INCLUDE_I
