# ifndef C_NSF_LIMITXPOST_INCLUDE
#  define C_NSF_LIMITXPOST_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 12900)

   enum {
      SWA_NSF_LimitXPost_Min      = SW_FIRSTTAG,     // [SGI] min groups
      SWA_NSF_LimitXPost_Max,                        // [SGI] max groups
   };

#  undef SW_FIRSTTAG   

# endif
