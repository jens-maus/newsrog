# ifndef C_NSF_AGERANGE_INCLUDE
#  define C_NSF_AGERANGE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 13700)

   enum {
      SWA_NSF_AgeRange_Min      = SW_FIRSTTAG,     // [SGI] min age (secs)
      SWA_NSF_AgeRange_Max,                        // [SGI] max age (secs)
   };

#  undef SW_FIRSTTAG   

# endif
