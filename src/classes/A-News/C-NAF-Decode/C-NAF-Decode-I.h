# ifndef C_NAF_DECODE_INCLUDE_I
#  define C_NAF_DECODE_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NAF-Decode.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NArticle.h"
#  include "Libraries/C-VarList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NAF_Decode
#  define SW_CNAME            "NAF_Decode"
#  define SW_SUPERPUB         SWC_NAFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NAF_Decode_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_FSpec = 1,

      SWS_L_Mode,
      SWS_Mode_Overwrite,
      SWS_Mode_Skip,
      SWS_Mode_Suffix,

      SWS_L_MkDir,
      
      SWS_H_FSpec,
      SWS_H_Mode,
      SWS_H_MkDir,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

#  define MimeValLen  128
#  define MimeEncLen  20
#  define MimeTypeLen 40
#  define MimeVerLen  12

   typedef struct MData
   {
      TEXT  ContentType[MimeTypeLen];
      TEXT  Encoding[MimeEncLen];
      TEXT  Boundary[MimeValLen];
      TEXT  FileName[FMSIZE];

      struct MData *MChild, *MParent;
   } MData;

   typedef struct SW_IDATA
   {
      STRPTR  SWA_NAF_Decode_FSpec;
      ULONG   SWA_NAF_Decode_Mode;
      ULONG   SWA_NAF_Decode_MkDir;
      ULONG   SWA_NAF_Decode_DelFile;

//    SWP_SysCmdAttrDefs(SWA_NAF_Decode_Cmd);

      ULONG   Count;
      TEXT    NumBuff[20];
      TEXT    FileName[FMSIZE];
      TEXT    NumFile[FMSIZE];

      Object  *UI, *Art, *Grp;
   } SW_IDATA;

#  define VLG(g) SWGetObj(g, SWA_AppBase_TopObj)

#  define VL_AllocGrp(g, o, Str) VL_AllocTop(VLG(g), o, Str)
#  define VL_FreeGrp(g, o, Str)  VL_FreeTop(VLG(g), o, Str)

# endif // C_NAF_DECODE_INCLUDE_I
