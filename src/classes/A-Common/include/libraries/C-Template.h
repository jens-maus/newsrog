# ifndef C_TEMPLATE_INCLUDE
#  define C_TEMPLATE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 3000)

   enum {
      SWA_Attr1 = SW_FIRSTTAG,               // attribute
   };
   
   enum {
      SWM_Method1 = SW_FIRSTTAG,             // method
   };

   // -- defaults ------------------------------------------------------------

   enum {
      SWF_DefBlorg = 1,
   };

#  undef SW_FIRSTTAG

# endif
