# ifndef C_NDF_ROT13_INCLUDE
#  define C_NDF_ROT13_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15200)

   enum {
      SWA_NDF_ROT13_UNUSED = SW_FIRSTTAG,
      SWA_NDF_ROT13_UNUSED2,
      SWA_NDF_ROT13_UNUSED3,
   };

#  undef SW_FIRSTTAG   

# endif
