# ifndef C_NSF_MATCHTHREAD_INCLUDE_I
#  define C_NSF_MATCHTHREAD_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-MatchThread.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-ClassList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_MatchThread
#  define SW_CNAME            "NSF_MatchThread"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_MatchThread_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Mode = 1,
      SWS_L_Any,
      SWS_L_All,
      
      SWS_H_New,
      SWS_H_Mode,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
     SWV_NSF_MatchThread_CSize = 1031,
   };
   
   typedef struct SW_IDATA
   {
      Object *SWA_NSF_MatchThread_Filter;
      ULONG   SWA_NSF_MatchThread_Mode;
      Object  *UI, *TM;
      ULONG Total, Found, Inv;

      Object  ***PrevThread;
      UBYTE     *PrevMatch;
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

# endif // C_NSF_MATCHTHREAD_INCLUDE_I
