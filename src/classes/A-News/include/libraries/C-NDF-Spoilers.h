# ifndef C_NDF_SPOILERS_INCLUDE
#  define C_NDF_SPOILERS_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 16200)

   enum {
      SWA_NDF_Spoilers_FmtStr = SW_FIRSTTAG,

      SWP_NDF_StyleAttrVals(SWA_NDF_Spoilers),  // [SGI] text style

   };

#  undef SW_FIRSTTAG   

# endif
