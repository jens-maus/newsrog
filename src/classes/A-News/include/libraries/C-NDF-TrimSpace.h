# ifndef C_NDF_TRIMSPACE_INCLUDE
#  define C_NDF_TRIMSPACE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15300)

   enum {
      SWA_NDF_TrimSpace_MaxLines = SW_FIRSTTAG,
   };

#  undef SW_FIRSTTAG   

# endif
