# ifndef C_NHDRLIST_INCLUDE
#   define C_NHDRLIST_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"
#   include "Libraries/C-AttrVal4.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 13600)

   enum {
      SWA_NHdrList_Hdr    = SWA_Attrval4_Attr,
      SWA_NHdrList_Val    = SWA_Attrval4_Val0,
      SWA_NHdrList_Grp    = SWA_Attrval4_Val1,
      SWA_NHdrList_Mode   = SWA_Attrval4_Num0,
   };

   enum {
      SWV_NHdrList_Any = 0,
      SWV_NHdrList_All,
   };

   // -- macros to make easier to use ----------------------------------------

#  undef SW_FIRSTTAG

# endif
