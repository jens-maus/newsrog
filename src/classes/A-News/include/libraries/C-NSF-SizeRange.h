# ifndef C_NSF_SIZERANGE_INCLUDE
#  define C_NSF_SIZERANGE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messsizes that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 13800)

   enum {
      SWA_NSF_SizeRange_Min      = SW_FIRSTTAG,     // [SGI] min size
      SWA_NSF_SizeRange_Max,                        // [SGI] max size
   };

#  undef SW_FIRSTTAG

# endif
