# ifndef C_NSF_THREADCOUNT_INCLUDE
#  define C_NSF_THREADCOUNT_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 13300)

   enum {
      SWA_NSF_ThreadCount_Min      = SW_FIRSTTAG,     // [SGI] min in thread
      SWA_NSF_ThreadCount_Max,                        // [SGI] max  in thread
   };

#  undef SW_FIRSTTAG   

# endif
