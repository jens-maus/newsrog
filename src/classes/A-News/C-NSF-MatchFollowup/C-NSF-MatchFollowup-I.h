# ifndef C_NSF_MATCHFOLLOWUP_INCLUDE_I
#  define C_NSF_MATCHFOLLOWUP_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSF-MatchFollowup.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-ClassList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_MatchFollowup
#  define SW_CNAME            "NSF_MatchFollowup"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_MatchFollowup_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_H_New = 1,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
     SWV_NSF_MatchFollowup_CSize    = 1031,
     SWV_NSF_MatchFollowup_MaxFDist = 512,
   };

#  define ParentUnknown ((Object *)-1)

   typedef struct MatchData
   {
      Object *PrevArt, *PrevParent;
      UBYTE  PrevMatch;
      UBYTE  pad[3];
   } MatchData;

   typedef struct SW_IDATA
   {
      Object *SWA_NSF_MatchFollowup_Filter;
      ULONG   SWA_NSF_MatchFollowup_Level;
      Object *TM;
      ULONG Total, Found, Inv;

      MatchData *MD;
      Object   **Followups;
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE

# endif // C_NSF_MATCHFOLLOWUP_INCLUDE_I
