# ifndef C_NDF_TEXTSTYLES_INCLUDE
#  define C_NDF_TEXTSTYLES_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15600)

   enum {
      SWA_NDF_TextStyles_MaxLen = SW_FIRSTTAG,
      SWA_NDF_TextStyles_Bold,
      SWA_NDF_TextStyles_Italic,
      SWA_NDF_TextStyles_Underline,
      SWA_NDF_TextStyles_WordBoundary,
   };

#  undef SW_FIRSTTAG   

# endif
