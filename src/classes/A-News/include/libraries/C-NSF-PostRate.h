# ifndef C_NSF_POSTRATE_INCLUDE
#  define C_NSF_POSTRATE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 16100)

   enum {
      SWA_NSF_PostRate_MinArts = SW_FIRSTTAG,     // Min in thread
      SWA_NSF_PostRate_MaxArts,                   // 0=any, 1=all
      SWA_NSF_PostRate_MinPerDay,                 // min articles per day
      SWA_NSF_PostRate_MaxPerDay,                 // max articles per day
   };

#  undef SW_FIRSTTAG   

# endif
