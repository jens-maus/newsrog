# ifndef C_TIMER_INCLUDE
#   define C_TIMER_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 2100)

   enum {
      SWA_Timer_Millis= SW_FIRSTTAG,     // [SGI] milliseconds
      SWA_Timer_Running,                 // [SGI] timer running?
      SWA_Timer_Target,                  // [ GI] object to be sent trigger
      SWA_Timer_Min,                     // [SGI] timer min value (UI)
      SWA_Timer_Max,                     // [SGI] timer max value (UI)
      SWA_Timer_Method,                  // [SGI] method to send to target
      SWA_Timer_UData,                   // [SGI] user data to send to tgt
      SWA_Timer_Secs,                    // [SGI] set time by seconds
   };
   
   enum {
      SWM_Timer_Trigger = SW_FIRSTTAG,   // sent by timer
      SWM_Timer_Install,                 // install timer to app
      SWM_Timer_Remove,                  // remove timer from app
   };

#  undef SW_FIRSTTAG

   // -- defaults ------------------------------------------------------------

   SWM_P(SWP_Timer_Install, Object *Target; Object *App); // app IFF no target
   SWM_P(SWP_Timer_Trigger, ULONG UData);

# endif
