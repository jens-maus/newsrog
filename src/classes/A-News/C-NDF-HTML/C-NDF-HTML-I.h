# ifndef C_NDF_HTML_INCLUDE_I
#  define C_NDF_HTML_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-HTML.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NNews.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_HTML
#  define SW_CNAME            "NDF_HTML"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_HTML_protos.h"
#  include "Libraries/C-NDFilter.h"
#  include "MUI/HTMLview_mcc.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_1 = 1,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      HTAG_HTML,
      HTAG_BODY,
   };
   
   enum {
      BeginTag,
      EndTag,
   };
   
   typedef struct ADDDATA {
      Object  *Art, *Grp;
      TxtBuff HTML;
   } AddData;

   typedef struct SW_IDATA
   {
      SWP_NDFilter_AttrDefs;
      SWP_NNews_AttrDefs;

      Object  *UI;
   } SW_IDATA;

# endif // C_NDF_HTML_INCLUDE_I
