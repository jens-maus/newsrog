# ifndef C_NDF_HIDESIG_INCLUDE
#  define C_NDF_HIDESIG_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 12600)

   enum {
      SWA_NDF_HideSig_Separator = SW_FIRSTTAG,
      SWA_NDF_HideSig_Marker,

      SWP_NDF_StyleAttrVals(SWA_NDF_HideSig_Hidden),
      SWP_NDF_StyleAttrVals(SWA_NDF_HideSig_Shown),

      SWA_NDF_HideSig_StartMode,
      SWA_NDF_HideSig_MaxLineLen,
      SWA_NDF_HideSig_MaxSigLines,
   };

#  undef SW_FIRSTTAG   

# endif
