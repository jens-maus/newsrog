# ifndef C_NDF_TEXTSTYLES_INCLUDE_I
#  define C_NDF_TEXTSTYLES_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-TextStyles.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_TextStyles
#  define SW_CNAME            "NDF_TextStyles"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_TextStyles_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

#  define TCount 4
   
   enum {
      SWS_L_MaxLen = 1,
      SWS_L_Bold,
      SWS_L_Italic,
      SWS_L_Underline,
      SWS_L_WordB,

      SWS_H_MaxLen,
      SWS_H_Bold,
      SWS_H_Italic,
      SWS_H_Underline,
      SWS_H_WordB,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG SWA_NDF_TextStyles_MaxLen;
      ULONG SWA_NDF_TextStyles_Bold;
      ULONG SWA_NDF_TextStyles_Italic;
      ULONG SWA_NDF_TextStyles_Underline;
      ULONG SWA_NDF_TextStyles_WordBoundary;

      Object *UI;
   } SW_IDATA;

#  define TSC_B '*'
#  define TSC_I '/'
#  define TSC_U '_'


# endif // C_NDF_TEXTSTYLES_INCLUDE_I
