# ifndef C_NMPMGR_INCLUDE_I
#  define C_NMPMGR_INCLUDE_I

//#  define SW_NListTrackAttrNum 1

#  include "S-ListImages.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NMPMgr.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/S-Lists.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NMPMgr
#  define SW_CNAME            "NMPMgr"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NMPMgr_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_None = 1,

      SWS_T_Subj,
      
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

#  define SW_NList_CmpAttr  SWA_NMPMgr_Subj
#  define SW_NList_DispHook MUI_DisposeObject(o)
#  define SW_NList_NewObj   MkInsObj(idata, NULL)

   enum {
      SWV_Img_True = 0,
      SWV_Img_False,
      SWV_Img_MAX,
   };

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NList_AttrDefs;
      SWP_Img_AttrDefs;
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

# endif // C_NMPMGR_INCLUDE_I
