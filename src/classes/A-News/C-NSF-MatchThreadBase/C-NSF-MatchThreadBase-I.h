# ifndef C_NSF_MATCHTHREADBASE_INCLUDE_I
#  define C_NSF_MATCHTHREADBASE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-MatchThreadBase.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-ClassList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_MatchThreadBase
#  define SW_CNAME            "NSF_MatchThreadBase"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_MatchThreadBase_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_H_New = 1,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
     SWV_NSF_MatchThreadBase_CSize = 1031,
   };
   
   typedef struct SW_IDATA
   {
      Object *SWA_NSF_MatchThreadBase_Filter;
      Object *TM;
      ULONG Total, Found, Inv;

      Object  **PrevBase;
      UBYTE    *PrevMatch;
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

# endif // C_NSF_MATCHTHREADBASE_INCLUDE_I
