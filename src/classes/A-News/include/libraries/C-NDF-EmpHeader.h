# ifndef C_NDF_EMPHEADER_INCLUDE
#  define C_NDF_EMPHEADER_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15000)

   enum {
      SWP_NDF_StyleAttrValsF(SWA_NDF_EmpHeader_Field, SW_FIRSTTAG),
      SWP_NDF_StyleAttrVals(SWA_NDF_EmpHeader_Data),
      SWA_NDF_EmpHeader_FoldLines,
   };

#  undef SW_FIRSTTAG   

# endif
