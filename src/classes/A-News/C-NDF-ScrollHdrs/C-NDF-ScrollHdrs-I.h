# ifndef C_NDF_SCROLLHDRS_INCLUDE_I
#  define C_NDF_SCROLLHDRS_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-ScrollHdrs.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NArticleView.h"
#  include <graphics/text.h>
#  include <diskfont/diskfont.h>
#  include <proto/diskfont.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_ScrollHdrs
#  define SW_CNAME            "NDF_ScrollHdrs"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_ScrollHdrs_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Lines = 1,

      SWS_H_Lines,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      CNUM                        = 256,
      SWV_NDF_ScrollHdrs_MinWidth = 80,    // minimum wrap width
   };

   typedef struct FData
   {
      UWORD   CharWidths[CNUM];
      ULONG   MaxWidth;
   } FData;

   typedef struct ADDDATA {
      TxtBuff Headers;
   } AddData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;

      LONG   SWA_NDF_ScrollHdrs_Lines;

      Object  *UI;
   } SW_IDATA;

# endif // C_NDF_SCROLLHDRS_INCLUDE_I
