# ifndef C_NSF_INGROUP_INCLUDE
#  define C_NSF_INGROUP_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 11400)

   enum {
      SWA_NSF_InGroup_FirstGrp = SW_FIRSTTAG,  // [SGI] must be 1st group?
      SWA_NSF_InGroup_Killed,                  // T to match killed arts too
   };

#  undef SW_FIRSTTAG   

# endif
