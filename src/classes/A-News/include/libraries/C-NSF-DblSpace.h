# ifndef C_NSF_DBLSPACE_INCLUDE
#  define C_NSF_DBLSPACE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 14000)

   enum {
      SWA_NSF_DblSpace_MinLen = SW_FIRSTTAG, // [SGI] minimum subj length
      SWA_NSF_DblSpace_Pct,                  // min % of title thats dblspaced
      SWA_NSF_DblSpace_Chars,                // doublespace characters
   };

#  undef SW_FIRSTTAG   

# endif
