# ifndef C_NDF_HLQUOTES_INCLUDE_I
#  define C_NDF_HLQUOTES_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-HLQuotes.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleView.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_HLQuotes
#  define SW_CNAME            "NDF_HLQuotes"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_HLQuotes_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Chars = 1,
      SWS_L_PLen,
      SWS_L_Lines,
      SWS_L_Level,
      SWS_L_UIM,
      SWS_L_UI1,
      SWS_L_UI2,
      SWS_L_UI3,

      SWS_L_L0,
      SWS_L_L1,
      SWS_L_L2,
      SWS_L_L3,
      SWS_L_L4,
      SWS_L_L5,

      SWS_L_PreH,

      SWP_NDF_StyleLabs,

      SWS_H_Chars,
      SWS_H_PLen,
      SWS_H_Lines,
      SWS_H_Level,
      SWS_H_UIMode,

      SWS_H_Font,
      SWS_H_PreH,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      PEN_SQ1 = 0,
      PEN_SQ2,
      PEN_SQ3,
      PEN_SQ4,
      PEN_SQ5,
      PEN_Max
   };

   typedef struct ADDDATA {
      TxtBuff Full, Part;
      ULONG   Mode;
   } AddData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;
      
      STRPTR SWA_NDF_HLQuotes_Chars;
      ULONG  SWA_NDF_HLQuotes_PrefixLen;
      LONG   SWA_NDF_HLQuotes_MaxLines;
      LONG   SWA_NDF_HLQuotes_MaxLevel;
      ULONG  SWA_NDF_HLQuotes_UIMode;
      ULONG  SWA_NDF_HLQuotes_PrefixHeur;
      
      SWP_NDF_StyleAttrDefs(SWA_NDF_HLQuotes_1);
      SWP_NDF_StyleAttrDefs(SWA_NDF_HLQuotes_2);
      SWP_NDF_StyleAttrDefs(SWA_NDF_HLQuotes_3);
      SWP_NDF_StyleAttrDefs(SWA_NDF_HLQuotes_4);
      SWP_NDF_StyleAttrDefs(SWA_NDF_HLQuotes_5);

      MUI_PenSpec SWA_NDF_HLQuotes_Pens[PEN_Max];

      UBYTE BrkChars[256];
      
      Object  *UI;
   } SW_IDATA;

#  define ILCase(il,a,p) \
   case il: SWP_NDF_TStyleOn(o, idata, a, Tx, SWA_NDF_HLQuotes_Pens,p); \
            DoMethod(o, SWM_NDFilter_TBAppendTxt, Tx, s); \
            SWP_NDF_TStyleOff(o, idata, a, &Off, SWA_NDF_HLQuotes_Pens, p); \
            break

# endif // C_NDF_HLQUOTES_INCLUDE_I
