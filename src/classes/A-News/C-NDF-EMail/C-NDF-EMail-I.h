# ifndef C_NDF_EMAIL_INCLUDE_I
#  define C_NDF_EMAIL_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-EMail.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NArticle.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-VarList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_EMail
#  define SW_CNAME            "NDF_EMail"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_EMail_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_H_EMail = 1,
      SWS_H_MsgID,
      SWS_L_EMailTxt,
      SWS_L_MsgIDTxt,

      SWS_L_EMailAct,
      SWS_L_ReqDot,
      SWS_L_DClk,
      SWS_L_PopM,

      SWS_H_EMailAct,
      SWS_H_ReqDot,
      SWS_H_DClk,
      SWS_H_PopM,

      SWS_L_SelCol,
      SWS_H_SelCol,
      SWS_H_SelColMsgID,

      SWP_NDF_StyleLabs,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      RefType_Unknown,
      RefType_EMail,
      RefType_MsgID,
   };

   enum {
      PEN_EMail = 0,
      PEN_SelEMail,
      PEN_MsgID,
      PEN_SelMsgID,
      PEN_Max
   };

   typedef struct ADDDATA {
      Object  *Art, *Grp;
      TxtBuff EMail;
      ULONG   RefType;
   } AddData;

#  define IsDig(c) ((c)>='0'&&(c)<='9')
#  define IsHexDig(c) ((c)>='0'&&(c)<='9' || (c)>='a'&&(c)<='f' || (c)>='A'&&(c)<='F')

#  define UserBrkChrs    "@<>(){}[]#\";:`'!%^&=\\|? \n\t"
#  define DomainBrkChrs  "@<>(){}[]#\";:`'!%^&=\\|? \n\t"
#  define MaxURLLen      256

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;

      STRPTR SWA_NDF_EMail_Action;
      ULONG  SWA_NDF_EMail_RequireDot;
      ULONG  SWA_NDF_EMail_DClick;
      ULONG  SWA_NDF_EMail_PopMenus;

      SWP_NDF_StyleAttrDefs(SWA_NDF_EMail);
      SWP_NDF_StyleAttrDefs(SWA_NDF_EMail_MsgID);
 
      MUI_PenSpec SWA_NDF_EMail_Pens[PEN_Max];

      TEXT MsgID[128];

      Object  *UI;
   } SW_IDATA;

# endif // C_NDF_EMAIL_INCLUDE_I
