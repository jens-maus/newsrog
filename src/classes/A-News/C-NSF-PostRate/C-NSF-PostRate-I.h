# ifndef C_NSF_POSTRATE_INCLUDE_I
#  define C_NSF_POSTRATE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-PostRate.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-ClassList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_PostRate
#  define SW_CNAME            "NSF_PostRate"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_PostRate_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_MinArts = 1,
      SWS_L_MaxArts,
      SWS_L_MinPerDay,
      SWS_L_MaxPerDay,
      
      SWS_H_MinArts,
      SWS_H_MaxArts,
      SWS_H_MinPerDay,
      SWS_H_MaxPerDay,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
     SWV_NSF_PostRate_CSize = 1031,
   };
   
   typedef struct SW_IDATA
   {
      ULONG   SWA_NSF_PostRate_MinArts;
      ULONG   SWA_NSF_PostRate_MaxArts;
      ULONG   SWA_NSF_PostRate_MinPerDay;
      ULONG   SWA_NSF_PostRate_MaxPerDay;
      Object  *UI, *TM;
      ULONG Total, Found, Inv;

      Object  ***PrevThread;
      UBYTE     *PrevMatch;
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

# endif // C_NSF_POSTRATE_INCLUDE_I
