# ifndef C_NDF_UUDECODE_INCLUDE_I
#  define C_NDF_UUDECODE_INCLUDE_I

#  include <proto/iffparse.h>
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-UUDecode.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-VarList.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-DTImg.h"
#  include "S-MkClass.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_UUDecode
#  define SW_CNAME            "NDF_UUDecode"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_UUDecode_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_SaveD = 1,
      SWS_L_DI,
      SWS_L_MD,
      SWS_L_Pr,
      SWS_L_DspS,
      SWS_L_DTxt,

      SWS_L_SOK,
      SWS_L_SError,
      SWS_L_SIncomp,

      SWS_H_SaveD,
      SWS_H_DI,
      SWS_H_MD,
      SWS_H_Pr,
      SWS_H_DspS,
      SWS_H_DTxt,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct ADDDATA {
      TxtBuff UUData;
      ULONG   Complete;
      TxtBuff FName;
      ULONG   Lines;
   } AddData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;
      
      STRPTR SWA_NDF_UUDecode_SaveDir;
      STRPTR SWA_NDF_UUDecode_DeleteTxt;
      ULONG  SWA_NDF_UUDecode_DispInline;
      ULONG  SWA_NDF_UUDecode_MakeDir;
      ULONG  SWA_NDF_UUDecode_Prompt;
      ULONG  SWA_NDF_UUDecode_DispSize;

      ULONG  TextOnly;
      ULONG  UUSize;
      ULONG  UULines;
      STRPTR UUFSpec;
      
      Object *UI, *Art, *Grp;
   } SW_IDATA;


#  define DefDTxt "  [--- ${Lines} uuencoded lines deleted ---]"

# endif // C_NDF_UUDECODE_INCLUDE_I
