# ifndef C_NSF_MATCHTHREAD_INCLUDE
#  define C_NSF_MATCHTHREAD_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 11600)

   enum {
      SWA_NSF_MatchThread_Filter = SW_FIRSTTAG,     // another condition
      SWA_NSF_MatchThread_FPtr,                     // ptr to above
      SWA_NSF_MatchThread_Mode,                     // 0=any, 1=all
   };

   enum {
      SWV_NSF_MatchThread_Or  = 0,                  // modes
      SWV_NSF_MatchThread_And,

      SWV_NSF_MatchThread_Any = SWV_NSF_MatchThread_Or,
      SWV_NSF_MatchThread_All = SWV_NSF_MatchThread_And,
   };

#  undef SW_FIRSTTAG   

# endif
