# ifndef C_NSF_MATCHTHREADBASE_INCLUDE
#  define C_NSF_MATCHTHREADBASE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 12000)

   enum {
      SWA_NSF_MatchThreadBase_Filter = SW_FIRSTTAG,     // another condition
      SWA_NSF_MatchThreadBase_FPtr,                     // ptr to above
   };

#  undef SW_FIRSTTAG   

# endif
