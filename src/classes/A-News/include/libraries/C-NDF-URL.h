# ifndef C_NDF_URL_INCLUDE
#  define C_NDF_URL_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "S-SysCmdVars.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 12700)

   enum {
      SWA_NDF_URL_Pattern = SW_FIRSTTAG,         // [SGI] URL pattern
      SWA_NDF_URL_SelColor,                      // [SGI] selected color

      SWP_NDF_StyleAttrVals(SWA_NDF_URL),        // [SGI] text style

      SWA_NDF_URL_Case,                          // [SGI] pattern case sens
      SWA_NDF_URL_FastCheck,                     // [SGI] fast check string
      SWA_NDF_URL_Action,                        // [SGI] multimedia action
      SWA_NDF_URL_DClick,                        // [SGI] trigger on dclick
      SWA_NDF_URL_PopMenus,                      // [SGI] use popup menus
   };

#  undef SW_FIRSTTAG

# endif
