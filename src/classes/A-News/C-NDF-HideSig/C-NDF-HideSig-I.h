# ifndef C_NDF_HIDESIG_INCLUDE_I
#  define C_NDF_HIDESIG_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-HideSig.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleView.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_HideSig
#  define SW_CNAME            "NDF_HideSig"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_HideSig_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Separator = 1,
      SWS_L_Marker,
      SWS_L_StartMode,
      SWS_L_MaxLL,
      SWS_L_MaxSL,

      SWS_L_Hidden,
      SWS_L_Shown,

      SWS_L_Font,
      SWS_L_Style,
      SWS_L_Color,
      SWS_L_B,
      SWS_L_U,
      SWS_L_I,

      SWS_H_Separator,
      SWS_H_Marker,
      SWS_H_StartMode,
      SWS_H_MaxLL,
      SWS_H_Hidden,
      SWS_H_Shown,
      SWS_H_MaxSL,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

#  define MaxPL 256
#  define DefPt "(--|-- )"
   
   enum {
      EHPEN_Hidden = 0,
      EHPEN_Shown,
      EHPEN_Max
   };

   typedef struct ADDDATA {
      TxtBuff Sig;
   } AddData;

   typedef struct FData
   {
      STRPTR  PPat;
   } FData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;

      STRPTR  SWA_NDF_HideSig_Separator;
      STRPTR  SWA_NDF_HideSig_Marker;
      ULONG   SWA_NDF_HideSig_StartMode;
      ULONG   SWA_NDF_HideSig_MaxLineLen;
      ULONG   SWA_NDF_HideSig_MaxSigLines;

      SWP_NDF_StyleAttrDefs(SWA_NDF_HideSig_Hidden);
      SWP_NDF_StyleAttrDefs(SWA_NDF_HideSig_Shown);
 
      MUI_PenSpec SWA_NDF_HideSig_Pens[EHPEN_Max];
      ULONG   TextOnly;

      Object  *UI;
   } SW_IDATA;

# endif // C_NDF_HIDESIG_INCLUDE_I
