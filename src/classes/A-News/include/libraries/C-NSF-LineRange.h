# ifndef C_NSF_LINERANGE_INCLUDE
#  define C_NSF_LINERANGE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 11000)

   enum {
      SWA_NSF_LineRange_Min      = SW_FIRSTTAG,     // [SGI] min lines
      SWA_NSF_LineRange_Max,                        // [SGI] max lines
   };

#  undef SW_FIRSTTAG   

# endif
