# ifndef C_NDF_HIDEHEADER_INCLUDE
#  define C_NDF_HIDEHEADER_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-AttrVal.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15050)

   enum {
      SWA_NDF_HideHeader_Pattern = SW_FIRSTTAG,
      SWA_NDF_HideHeader_SaveVer,
   };

   enum {
      SWA_NDF_HideHeader_Pat  = SWA_Attrval_Attr,
   };

#  undef SW_FIRSTTAG   

# endif
