# ifndef C_ATTRVAL_INCLUDE
#  define C_ATTRVAL_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 2700)

   enum {
      SWA_Attrval_Attr   = SW_FIRSTTAG,       // [SGI] attribute
      SWA_Attrval_Val,                        // [SGI] string value
      SWA_Attrval_Num,                        // [SGI] numeric value
   };

#  undef SW_FIRSTTAG   

# endif
