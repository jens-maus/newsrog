# ifndef C_NSF_SUBJMATCH_INCLUDE
#  define C_NSF_SUBJMATCH_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 11700)

   enum {
      SWA_NSF_SubjMatch_Str = SW_FIRSTTAG,          // subject to match
      SWA_NSF_SubjMatch_Case,                       // case sens?
      SWA_NSF_SubjMatch_MatchMsgID,                 // match MsgID instead
      SWA_NSF_SubjMatch_SubStr,                     // substring match
   };

#  undef SW_FIRSTTAG   

# endif
