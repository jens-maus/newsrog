# ifndef C_NDF_SUBSTITUTE_INCLUDE
#  define C_NDF_SUBSTITUTE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-AttrVal4.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15700)

   enum {
      UNUSED1 = SW_FIRSTTAG,         // [SGI] Substitute pattern
   };

   enum {
      SWA_NDF_Substitute_Case    = SWA_Attrval4_Num0,
      SWA_NDF_Substitute_Wild    = SWA_Attrval4_Num1,
      SWA_NDF_Substitute_Longest = SWA_Attrval4_Num2,
      SWA_NDF_Substitute_Pattern = SWA_Attrval4_Attr,
      SWA_NDF_Substitute_Replace = SWA_Attrval4_Val0,
      SWA_NDF_Substitute_Image   = SWA_Attrval4_Val1,
      SWA_NDF_Substitute_Mask    = SWA_Attrval4_Val2,
   };

#  undef SW_FIRSTTAG

# endif
