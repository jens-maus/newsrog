# ifndef C_NDF_HLQUOTES_INCLUDE
#  define C_NDF_HLQUOTES_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "S-SysCmdVars.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 12800)

   enum {
      SWA_NDF_HLQuotes_Chars = SW_FIRSTTAG,    // [SGI] separator chars
      SWA_NDF_HLQuotes_PrefixLen,              // [SGI] max prefix length
      SWA_NDF_HLQuotes_MaxLines,               // [SGI] max quote lines
      SWA_NDF_HLQuotes_MaxLevel,               // [SGI] max quote level

      SWP_NDF_StyleAttrVals(SWA_NDF_HLQuotes_1),
      SWP_NDF_StyleAttrVals(SWA_NDF_HLQuotes_2),
      SWP_NDF_StyleAttrVals(SWA_NDF_HLQuotes_3),
      SWP_NDF_StyleAttrVals(SWA_NDF_HLQuotes_4),
      SWP_NDF_StyleAttrVals(SWA_NDF_HLQuotes_5),

      SWA_NDF_HLQuotes_UIMode,                 // [SGI] UI mode
      SWA_NDF_HLQuotes_PrefixHeur,             // [SGI] use prefix heuristics
   };

   enum {
      SWV_NDF_HLQuotes_UIM_None = 0,           // none - just highlight
      SWV_NDF_HLQuotes_UIM_Toggle,             // toggle between text/scroll
      SWV_NDF_HLQuotes_UIM_Scroll,             // use scroller
   };

#  undef SW_FIRSTTAG

# endif
