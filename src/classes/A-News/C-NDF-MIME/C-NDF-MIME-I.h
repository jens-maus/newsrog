# ifndef C_NDF_MIME_INCLUDE_I
#  define C_NDF_MIME_INCLUDE_I

#  include <proto/iffparse.h>
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-MIME.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NArticle.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-VarList.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-DTImg.h"
#  include "S-MkClass.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_MIME
#  define SW_CNAME            "NDF_MIME"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_MIME_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_SaveD = 1,
      SWS_L_DI,
      SWS_L_MD,
      SWS_L_Pr,
      SWS_L_DspS,
      SWS_L_DTxtB,
      SWS_L_NoDsp,

      SWS_L_SOK,
      SWS_L_SError,
      SWS_L_SIncomp,

      SWS_L_PName,
      SWS_L_PType,

      SWS_H_SaveD,
      SWS_H_DI,
      SWS_H_MD,
      SWS_H_Pr,
      SWS_H_DspS,
      SWS_H_DTxtB,
      SWS_H_NoDsp,

      SWS_L_Reading,
      SWS_L_Incomp,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct ADDDATA {
      Object  *MimeTop;
   } AddData;

   typedef struct MData
   {
      ULONG    BadMimeVer;
      TxtBuff  ContentType;
      TxtBuff  ContentEncoding;
      TxtBuff  ContentDesc;
      TxtBuff  PartBoundary;
      TxtBuff  FileName;

      struct MData *MChild, *MParent;
      Object *MimeObj;
   } MData;

   typedef struct FData
   {
      ULONG  RecurCount;
      MData  *MD;
   } FData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;

      STRPTR SWA_NDF_MIME_SaveDir;
      ULONG  SWA_NDF_MIME_DispInline;
      ULONG  SWA_NDF_MIME_MakeDir;
      ULONG  SWA_NDF_MIME_Prompt;
      ULONG  SWA_NDF_MIME_DispSize;
      STRPTR SWA_NDF_MIME_DelTxtBin;
      STRPTR SWA_NDF_MIME_NoDispTypes;

      STRPTR NoDspPat;
      TEXT   TmpBuff[256];

      MData *MD;
      ULONG MDSize, MDLines;
      
      Object *UI, *Art, *Grp;
   } SW_IDATA;

#  define IsWhite(c) ((c)==' '||(c)=='\t')
#  define SWV_NDF_MIME_Ver_Str "1.0"
#  define MI_HVSize            128

#  define MTFunc(name) \
   ULONG name(Object *o, SW_IDATA *idata, STRPTR Buff, MData *MD, \
             SWP_NDFilter_ModTxt *msg, ULONG EndOfData, TxtBuff *OutTxt)

#  define MTCall(name) name(o, idata, Buff, MD, msg, EndOfData, OutTxt)

#  define DefDTxt "  [--- ${Lines} lines ${MIMEType} data deleted ---]"

# endif // C_NDF_MIME_INCLUDE_I
