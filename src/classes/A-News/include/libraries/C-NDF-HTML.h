# ifndef C_NDF_HTML_INCLUDE
#  define C_NDF_HTML_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 14100)

   enum {
      SWA_NDF_HTML_1 = SW_FIRSTTAG,                // [SGI] 
   };

#  undef SW_FIRSTTAG

# endif
