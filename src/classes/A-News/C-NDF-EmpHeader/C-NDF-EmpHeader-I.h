# ifndef C_NDF_EMPHEADER_INCLUDE_I
#  define C_NDF_EMPHEADER_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-EmpHeader.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_EmpHeader
#  define SW_CNAME            "NDF_EmpHeader"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_EmpHeader_protos.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-CText.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_HField = 1,
      SWS_L_HData,

      SWS_L_Font,
      SWS_L_Style,
      SWS_L_Color,
      SWS_L_B,
      SWS_L_U,
      SWS_L_I,

      SWS_L_FoldLines,

      SWS_H_Field,
      SWS_H_Data,
      SWS_H_FoldLines,
      
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      EHPEN_Field = 0,
      EHPEN_Data,
      EHPEN_Max
   };

   typedef struct SW_IDATA
   {
      SWP_NDF_StyleAttrDefs(SWA_NDF_EmpHeader_Field);
      SWP_NDF_StyleAttrDefs(SWA_NDF_EmpHeader_Data);
 
      MUI_PenSpec SWA_NDF_EmpHeader_Pens[EHPEN_Max];

      ULONG SWA_NDF_EmpHeader_FoldLines;

      Object  *UI;
   } SW_IDATA;

# endif // C_NDF_EMPHEADER_INCLUDE_I
