# ifndef C_NSF_MATCHFOLLOWUP_INCLUDE
#  define C_NSF_MATCHFOLLOWUP_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 11900)

   enum {
      SWA_NSF_MatchFollowup_Filter = SW_FIRSTTAG,     // another condition
      SWA_NSF_MatchFollowup_FPtr,                     // ptr to above
   };

#  undef SW_FIRSTTAG   

# endif
