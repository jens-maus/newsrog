# ifndef C_NSF_REPEATCHAR_INCLUDE
#  define C_NSF_REPEATCHAR_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 12400)

   enum {
      SWA_NSF_RepeatChar_MaxCount = SW_FIRSTTAG,  // [SGI] maximum count
      SWA_NSF_RepeatChar_Chars,                   // [SGI] chars to search for
      SWA_NSF_RepeatChar_Set,                     // [SGI] treat as a set
   };

#  undef SW_FIRSTTAG   

# endif
