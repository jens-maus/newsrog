# ifndef C_NDF_TRIMSPACE_INCLUDE_I
#  define C_NDF_TRIMSPACE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-TrimSpace.h"
#  include "Libraries/C-NArticleView.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_TrimSpace
#  define SW_CNAME            "NDF_TrimSpace"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_TrimSpace_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_MaxLines = 1,

      SWS_H_MaxLines,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG   SWA_NDF_TrimSpace_MaxLines;

      Object  *UI;
   } SW_IDATA;

# endif // C_NDF_TRIMSPACE_INCLUDE_I
