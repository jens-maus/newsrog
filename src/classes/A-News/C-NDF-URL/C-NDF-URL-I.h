# ifndef C_NDF_URL_INCLUDE_I
#  define C_NDF_URL_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-URL.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-VarList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_URL
#  define SW_CNAME            "NDF_URL"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_URL_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_H_URL = 1,
      SWS_L_URLTxt,

      SWS_L_URLPat,
      SWS_L_URLCase,
      SWS_L_URLFC,
      SWS_L_URLAct,
      SWS_L_URLDClk,
      SWS_L_URLPopM,

      SWS_H_URLPat,
      SWS_H_URLCase,
      SWS_H_URLFC,
      SWS_H_URLAct,
      SWS_H_URLDClk,
      SWS_H_URLPopM,

      SWS_L_SelCol,
      SWS_H_SelCol,

      SWP_NDF_StyleLabs,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      PEN_URL = 0,
      PEN_SelURL,
      PEN_Max
   };

#  define MaxURLLen 512
#  define MaxPL 256
#  define DefPt "(HTTP|FTP):#[~ \">]"
#  define DefFC "HTTP:|FTP:"

   typedef struct ADDDATA {
      Object  *Art, *Grp;
      TxtBuff URL;
   } AddData;

   typedef struct FData
   {
      STRPTR  PPat;
      struct  PatBounds pb;
   } FData;

   typedef struct SW_IDATA
   {
      SWP_NDFilter_AttrDefs;
      SWP_NNews_AttrDefs;

      STRPTR SWA_NDF_URL_Pattern;
      ULONG  SWA_NDF_URL_Case;
      STRPTR SWA_NDF_URL_FastCheck;
      STRPTR SWA_NDF_URL_Action;
      ULONG  SWA_NDF_URL_DClick;
      ULONG  SWA_NDF_URL_PopMenus;

      SWP_NDF_StyleAttrDefs(SWA_NDF_URL);
 
      MUI_PenSpec SWA_NDF_URL_Pens[PEN_Max];

      Object  *UI;
   } SW_IDATA;

# endif // C_NDF_URL_INCLUDE_I
