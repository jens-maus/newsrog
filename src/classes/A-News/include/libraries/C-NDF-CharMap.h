# ifndef C_NDF_SUBSTITUTE_INCLUDE
#  define C_NDF_SUBSTITUTE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-NArticleView.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 16300)

   enum {
      SWA_NDF_CharMap_MapInfo = SW_FIRSTTAG,           // [SGI] mapping table
   };

#  undef SW_FIRSTTAG

# endif
