# ifndef C_NAFILTER_INCLUDE_I
#  define C_NAFILTER_INCLUDE_I

#  include "Libraries/C-NAFilter.h"
#  include "MUI/TextEditor_mcc.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NAFilter
#  define SW_CNAME            "NAFilter"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NAFilter_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_WinTitle = 1,
      SWS_L_Global,
      SWS_L_Edit,
      SWS_L_AuthNote,

      SWS_L_Desc,
      SWS_L_Enabled,
      SWS_L_MarkRead,

      SWS_H_Desc,
      SWS_H_Enabled,
      SWS_H_MarkRead,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      ULONG  SWA_NAFilter_Enabled;
      ULONG  SWA_NAFilter_MarkRead;
      
      STRPTR SWA_NAFilter_Desc,
             SWA_NAFilter_Groups,
             SWA_NAFilter_AuthNote;

      Object *UIWin, *Ed, *App, *ui_s1, *ui_b1;
   } SW_IDATA;

#  define NOTP      MUIM_Notify, MUIA_Pressed, FALSE

# endif // C_NAFILTER_INCLUDE_I
