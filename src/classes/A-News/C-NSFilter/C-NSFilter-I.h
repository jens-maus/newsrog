# ifndef C_NSFILTER_INCLUDE_I
#  define C_NSFILTER_INCLUDE_I

#  include "Libraries/C-NSFilter.h"
#  include "Libraries/C-ClassList.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-DTImg.h"
#  include "MUI/TextEditor_mcc.h"
#  include "S-MkClass.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSFilter
#  define SW_CNAME            "NSFilter"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSFilter_protos.h"
#  include <libraries/asl.h>

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_WinTitle = 1,
      SWS_L_Global,
      SWS_L_Edit,
      SWS_L_Stats,
      SWS_L_AuthNote,

      SWS_L_Desc,
      SWS_L_Enabled,
      SWS_L_Invert,
      SWS_L_ExpDate,
      SWS_L_ExpDisuse,
      SWS_L_ThreadMode,
         SWS_L_TM_Base,
         SWS_L_TM_Any,
         SWS_L_TM_All,

      SWS_L_Hist,
      SWS_L_LastCount,
      SWS_L_Matched,
      SWS_L_Of,
      SWS_L_LastMtch,
      SWS_L_ExpOn,
      SWS_L_Upd,
      SWS_L_Reset,
      SWS_L_UseOnOffBt,
      SWS_L_Never,

      SWS_H_Desc,
      SWS_H_Enabled,
      SWS_H_Invert,
      SWS_H_ExpDate,
      SWS_H_ExpDisuse,
      SWS_H_ThreadMode,
      SWS_H_Upd,
      SWS_H_Reset,
      SWS_H_UseOnOffBt,
      SWS_H_TBOnOff,

      SWS_B_New,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   enum {
      SWV_NSF_ExpMax = 365 * 2 + 1,
      SWV_NSF_ExpDef = 14,
   };
   
   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      
      ULONG  SWA_NSFilter_TotHist,
             SWA_NSFilter_FoundHist,
             SWA_NSFilter_TotNow,
             SWA_NSFilter_FoundNow,
             SWA_NSFilter_Inv,
             SWA_NSFilter_ExpireDate,
             SWA_NSFilter_ExpireDateOK,
             SWA_NSFilter_LastMatchDay,
             SWA_NSFilter_LastMatchDayDsp,
             SWA_NSFilter_ExpDisuse,
             SWA_NSFilter_ExpDisuseOK,
             SWA_NSFilter_Enabled,
             SWA_NSFilter_ThreadMode,
             SWA_NSFilter_UseOnOffBt;

      STRPTR SWA_NSFilter_Desc,
             SWA_NSFilter_Groups,
             SWA_NSFilter_AuthNote,
             SWA_NSFilter_OnOffBtImg;

      STRPTR TBDesc;
      ULONG  TBDescLen;

      Object *UIWin, *App, *ui_s1, *ui_b1, *ui_b2, *hm1, *hm2, *lm1, *lm2,
             *es, *du, *lmdate, *eodate, *upd, *UIGroup, *UIChild, *ClassLB, *PopB,
             *Ed;

      void   *MemPool;
   } SW_IDATA;

#  define MaxFltDescLen 256

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

#  define AV(s)   (SWPoolAV(idata->MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->MemPool, (UBYTE *)(m))
#  define RV(s,m) (SWPoolRV(idata->MemPool, (s), (UBYTE **)(m)))

# endif // C_NSFILTER_INCLUDE_I
