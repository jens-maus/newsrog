# ifndef C_NDF_SPOILERS_INCLUDE_I
#  define C_NDF_SPOILERS_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-Spoilers.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-VarList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_Spoilers
#  define SW_CNAME            "NDF_Spoilers"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_Spoilers_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_FmtTxt = 1,

      SWS_L_Font,
      SWS_L_Style,
      SWS_L_Color,
      SWS_L_B,
      SWS_L_U,
      SWS_L_I,

      SWS_H_FmtTxt,
      SWS_H_Style,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------
   
   enum {
      SWV_MaxPgBrk = 128,
   };

   enum {
      EHPEN_SPText = 0,
      EHPEN_Max
   };

   typedef struct ADDDATA {
      ULONG    PageCount;
      TxtBuff *Pages;
   } AddData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;

      STRPTR  SWA_NDF_Spoilers_FmtStr;

      SWP_NDF_StyleAttrDefs(SWA_NDF_Spoilers);
 
      MUI_PenSpec SWA_NDF_Spoilers[EHPEN_Max];

      ULONG BrkPos[SWV_MaxPgBrk];

      // -- for varlist lookup ---
      AddData *TmpAD;
      ULONG CurrentPage;

      Object  *UI, *Art, *Grp;
   } SW_IDATA;

# define DefFmt "Spoiler ${Page} (${pad ${Lines}} Lines, ${bytes ${Size}})"

# endif // C_NDF_SPOILERS_INCLUDE_I
