# ifndef C_NDF_SCROLLHDRS_INCLUDE
#  define C_NDF_SCROLLHDRS_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 13900)

   enum {
      SWA_NDF_ScrollHdrs_Lines = SW_FIRSTTAG,    // scroll region size
   };

#  undef SW_FIRSTTAG

# endif
