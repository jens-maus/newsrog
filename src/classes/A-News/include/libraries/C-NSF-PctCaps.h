# ifndef C_NSF_PCTCAPS_INCLUDE
#  define C_NSF_PCTCAPS_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 12500)

   enum {
      SWA_NSF_PctCaps_MinLen = SW_FIRSTTAG,    // [SGI] minimum subj length
      SWA_NSF_PctCaps_Pct,                     // maximum % caps
   };

#  undef SW_FIRSTTAG   

# endif
