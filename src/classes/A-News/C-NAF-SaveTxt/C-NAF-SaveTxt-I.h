# ifndef C_NAF_SAVETXT_INCLUDE_I
#  define C_NAF_SAVETXT_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NAF-SaveTxt.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NArticle.h"
#  include "Libraries/C-VarList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NAF_SaveTxt
#  define SW_CNAME            "NAF_SaveTxt"
#  define SW_SUPERPUB         SWC_NAFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NAF_SaveTxt_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_FSpec = 1,

      SWS_L_HdrMode,
      SWS_HM_All,
      SWS_HM_Head,
      SWS_HM_Body,

      SWS_L_Mode,
      SWS_Mode_Overwrite,
      SWS_Mode_Append,
      SWS_Mode_Skip,
      SWS_Mode_Suffix,

      SWP_SysCmdLabs,
      SWS_L_MkDir,
      SWS_L_DelFile,
      
      SWS_H_FSpec,
      SWS_H_HdrMode,
      SWS_H_Mode,
      SWP_SysCmdHelpLabs,
      SWS_H_MkDir,
      SWS_H_DelFile,

      SWS_L_L0,
      SWS_L_L1,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      STRPTR  SWA_NAF_SaveTxt_FSpec;
      ULONG   SWA_NAF_SaveTxt_HdrMode;
      ULONG   SWA_NAF_SaveTxt_Mode;
      ULONG   SWA_NAF_SaveTxt_MkDir;
      ULONG   SWA_NAF_SaveTxt_DelFile;

      SWP_SysCmdAttrDefs(SWA_NAF_SaveTxt_Cmd);

      ULONG   Count;
      TEXT    NumBuff[20];
      TEXT    File[FMSIZE];

      Object  *UI, *Art, *Grp;
   } SW_IDATA;

#  define VLG(g) SWGetObj(g, SWA_AppBase_TopObj)

#  define VL_AllocGrp(g, o, Str) VL_AllocTop(VLG(g), o, Str)
#  define VL_FreeGrp(g, o, Str)  VL_FreeTop(VLG(g), o, Str)

# endif // C_NAF_SAVETXT_INCLUDE_I
