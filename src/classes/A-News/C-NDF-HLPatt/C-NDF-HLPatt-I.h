# ifndef C_NDF_HLPATT_INCLUDE_I
#  define C_NDF_HLPATT_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-HLPatt.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NNews.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_HLPatt
#  define SW_CNAME            "NDF_HLPatt"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_HLPatt_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_H_HLPatt = 1,
      SWS_L_HLPattTxt,

      SWS_L_HLPattPat,
      SWS_L_HLPattCase,
      SWS_L_HLPattFC,
      SWS_L_HLPattMode,
      SWS_L_HLPattModeShort,
      SWS_L_HLPattModeLong,

      SWS_H_HLPattPat,
      SWS_H_HLPattCase,
      SWS_H_HLPattFC,
      SWS_H_HLPattMode,

      SWP_NDF_StyleLabs,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      PEN_HLPatt = 0,
      PEN_Max
   };

#  define MaxPL 256
#  define DefPt "Amiga"
#  define DefFC "Amiga|AmigaOS"

   typedef struct FData
   {
      STRPTR  PPat;
      struct  PatBounds pb;
   } FData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;

      STRPTR SWA_NDF_HLPatt_Pattern;
      ULONG  SWA_NDF_HLPatt_Case;
      STRPTR SWA_NDF_HLPatt_FastCheck;
      ULONG  SWA_NDF_HLPatt_MatchMode;

      MUI_PenSpec SWA_NDF_HLPatt_Pens[PEN_Max];

      SWP_NDF_StyleAttrDefs(SWA_NDF_HLPatt);
 
      Object  *UI;
   } SW_IDATA;

# endif // C_NDF_HLPATT_INCLUDE_I
