# ifndef C_ATTRVAL4_INCLUDE
#  define C_ATTRVAL4_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 3200)

   enum {
      SWA_Attrval4_Attr   = SW_FIRSTTAG,       // [SGI] attribute
      SWA_Attrval4_Val0,                       // [SGI] string value
      SWA_Attrval4_Val1,                       // [SGI] string value
      SWA_Attrval4_Val2,                       // [SGI] string value
      SWA_Attrval4_Val3,                       // [SGI] string value
      SWA_Attrval4_Num0,                       // [SGI] numeric value
      SWA_Attrval4_Num1,                       // [SGI] numeric value
      SWA_Attrval4_Num2,                       // [SGI] numeric value
      SWA_Attrval4_Num3,                       // [SGI] numeric value
   };

#  undef SW_FIRSTTAG   

# endif
