# ifndef C_NDF_HLPATT_INCLUDE
#  define C_NDF_HLPATT_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 13400)

   enum {
      SWA_NDF_HLPatt_Pattern = SW_FIRSTTAG,         // [SGI] HLPatt pattern

      SWP_NDF_StyleAttrVals(SWA_NDF_HLPatt),        // [SGI] text style

      SWA_NDF_HLPatt_Case,                          // [SGI] pattern case sens
      SWA_NDF_HLPatt_FastCheck,                     // [SGI] fast check string
      SWA_NDF_HLPatt_MatchMode,                     // [SGI] match mode
   };

   enum {
      SWV_NDF_HLPatt_MatchShortest = 0,
      SWV_NDF_HLPatt_MatchLontest
   };

#  undef SW_FIRSTTAG

# endif
