# ifndef C_NDFILTER_INCLUDE_I
#  define C_NDFILTER_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDFilter.h"
#  include "MUI/TextEditor_mcc.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDFilter
#  define SW_CNAME            "NDFilter"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDFilter_protos.h"
#  include "Libraries/C-CText.h"
#  include "Libraries/C-DTImg.h"
#  include "Libraries/C-NGroup.h"
#  include <libraries/asl.h>

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_WinTitle = 1,
      SWS_L_Global,
      SWS_L_Edit,
      SWS_L_AuthNote,

      SWS_L_Desc,
      SWS_L_Enabled,
      SWS_L_UseOnOffBt,
      SWS_L_ForPrint,
      SWS_L_ForSave,
      SWS_L_ForReply,

      SWS_H_Desc,
      SWS_H_Enabled,
      SWS_H_UseOnOffBt,
      SWS_H_TBOnOff,
      SWS_H_ForPrint,
      SWS_H_ForSave,
      SWS_H_ForReply,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      ULONG  SWA_NDFilter_Enabled,
             SWA_NDFilter_UseOnOffBt,
             SWA_NDFilter_T_Print,
             SWA_NDFilter_T_Save,
             SWA_NDFilter_T_NetReply,
             SWA_NDFilter_T_MailReply;

      STRPTR SWA_NDFilter_Desc,
             SWA_NDFilter_Groups,
             SWA_NDFilter_AuthNote,
             SWA_NDFilter_OnOffBtImg;

      STRPTR TBDesc;
      ULONG  TBDescLen;

      Object *UIWin, *Ed, *App, *ui_s1, *ui_b1;

      void   *MemPool;
   } SW_IDATA;

#  define AV(s)   (SWPoolAV(idata->MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->MemPool, (UBYTE *)(m))
#  define RV(s,m) (SWPoolRV(idata->MemPool, (s), (UBYTE **)(m)))

# endif // C_NDFILTER_INCLUDE_I
