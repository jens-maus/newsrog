# ifndef C_NNEWS_INCLUDE_I
#  define C_NNEWS_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-AppBase.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NTCPList.h"
#  include "Libraries/C-NServerList.h"
#  include "Libraries/C-NPostList.h"
#  include "Libraries/C-NGroupList.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-NArticleList.h"
#  include "Libraries/C-Register.h"
#  include "Libraries/C-ListCol.h"
#  include "Libraries/C-NSFList.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-NAFilter.h"
#  include "Libraries/C-VarList.h"
#  include "Libraries/C-DTImg.h"
#  include "S-MkClass.h"
#  include "MUI/TextEditor_mcc.h"
#  include <proto/usergroup.h>
#  include <diskfont/diskfont.h>


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NNews
#  define SW_CNAME            "NNews"
#  define SW_SUPERPUB         SWC_AppBase
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NNews_protos.h"


   // -- in case we don`t have this...
   struct passwd
   {
     char  *pw_name;               /* Username */
     char  *pw_passwd;             /* Encrypted password */
     long   pw_uid;                /* User ID */
     long   pw_gid;                /* Group ID */
     char  *pw_gecos;              /* Real name etc */
     char  *pw_dir;                /* Home directory */
     char  *pw_shell;              /* Shell */
   };


   // -- catalog strings (must match with SW_CATSTR) -------------------------

#  define BF_Groups   "PROGDIR:Images/B.Groups"
#  define BF_Servers  "PROGDIR:Images/B.Servers"
#  define BF_Config   "PROGDIR:Images/B.Config"
#  define BF_TCP      "PROGDIR:Images/B.Network"
#  define BF_Posts    "PROGDIR:Images/B.PostQueue"
#  define BF_MMType   "PROGDIR:Images/B.MMTypes"

   enum {
      SWS_L_Groups = 1,
      SWS_L_Servers,
      SWS_L_TCP,
      SWS_L_Config,
      SWS_L_Posts,
      SWS_L_MM,

      SWS_CC_Groups,
      SWS_CC_Servers,
      SWS_CC_TCP,
      SWS_CC_Config,
      SWS_CC_Posts,
      SWS_CC_MM,

      SWS_H_Groups,
      SWS_H_Servers,
      SWS_H_TCP,
      SWS_H_Config,
      SWS_H_Posts,
      SWS_H_MM,

      SWS_T_Groups,
      SWS_T_Servers,
      SWS_T_Post,
      SWS_T_TCP,
      SWS_T_MM,
      SWS_T_Cfg,
      SWS_T_NNews,

      SWS_L_CfgTab0,
      SWS_L_CfgTab1,
      SWS_L_CfgTab2,
      SWS_L_CfgTab3,
      SWS_L_CfgTab4,
      SWS_L_CfgLast,

      SWS_L_FltTab0,
      SWS_L_FltTab1,
      SWS_L_FltTab2,
      SWS_L_FltTab3,
      SWS_L_FltLast,

      SWS_L_POTab0,
      SWS_L_POTab1,
      SWS_L_POTab2,
      SWS_L_POTab3,
      SWS_L_POLast,

      SWS_L_PR_O,
        SWS_L_PR_1,
        SWS_L_PR_2,
        SWS_L_PR_3,

      SWS_L_UserName,
      SWS_L_UserEmail,
      SWS_L_UserOrg,
      SWS_L_ReplyLine,
      SWS_L_CacheDir,
      SWS_L_SigFile,
      SWS_L_Editor,
      SWS_L_HdrCS,
      SWS_L_StartNW,
      SWS_L_StartNWOn,
      SWS_L_StartNWOff,
      SWS_L_VFont,
      SWS_L_VCol,
      SWS_L_AVTitle,
      SWS_L_SoftS,
      SWS_L_QuoteMode,
      SWS_L_QM1,
        SWS_L_QM2,
        SWS_L_QM3,
      SWS_L_QuoteStr,
      SWS_L_UseSS,
      SWS_L_PWrap,
      SWS_L_GenMID,
      SWS_L_ALWght,
      SWS_L_FAQS,
      SWS_L_FAQD,
      SWS_L_AutoC,
      SWS_L_HDType,
        SWS_L_HDType0,
        SWS_L_HDType1,
        SWS_L_HDType2,
      SWS_L_HDInc,
      SWS_L_SimCmd,
      SWS_L_GWM,
        SWS_L_GWMode_1,
        SWS_L_GWMode_2,
      SWS_L_AEd,
      SWS_L_MaxSvPst,
      SWS_L_TagFile,
      SWS_L_TagSep,
      SWS_L_ETType,
        SWS_L_ETType0,
        SWS_L_ETType1,
        SWS_L_ETType2,
      SWS_L_PrtDev,
      SWS_L_FFTBLoc,
        SWS_L_FFTBLoc0,
        SWS_L_FFTBLoc1,
        SWS_L_FFTBLoc2,
        SWS_L_FFTBLoc3,
        SWS_L_FFTBLoc4,
      SWS_L_FFTBLab,
      SWS_L_FetchPol,
        SWS_L_FetchPol0,
        SWS_L_FetchPol1,
        SWS_L_FetchPol2,
      SWS_L_Charset,
        SWS_L_Charset1,   // various ISO-8859-X codesets
        SWS_L_Charset2,
        SWS_L_Charset3,
        SWS_L_Charset4,
        SWS_L_Charset5,
        SWS_L_Charset6,
        SWS_L_Charset7, 
        SWS_L_Charset8,
        SWS_L_Charset9,
        SWS_L_Charset10,
      SWS_L_AutoSave,
        SWS_L_AutoSave0,
        SWS_L_AutoSave1,
      SWS_L_EncBody,
        SWS_L_EB_QP,
        SWS_L_EB_8bit,
        SWS_L_EB_7bit,
      SWS_L_EncHdrs,
        SWS_L_EH_QP,
        SWS_L_EH_8bit,
      SWS_L_StArt,
        SWS_L_SA_PrevArt,
        SWS_L_SA_PrevThread,
        SWS_L_SA_First,
        SWS_L_SA_Last,
      SWS_L_BrArts,
        SWS_L_BA_All,
        SWS_L_BA_Unread,
        SWS_L_BA_New,
        SWS_L_BA_UnreadNew,

      SWS_L_TBS,
      SWS_TBS_HL,
      SWS_TBS_HNL,
      SWS_TBS_VL,
      SWS_TBS_VNL,

      SWS_L_GL_O,
        SWS_L_GL_1,
        SWS_L_GL_2,
        SWS_L_GL_3,
      
      SWS_L_SL_O,
        SWS_L_SL_1,
        SWS_L_SL_2,
        SWS_L_SL_3,
      
      SWS_L_TCPL_O,
        SWS_L_TCPL_1,
        SWS_L_TCPL_2,
        SWS_L_TCPL_3,

      SWS_L_PstL_O,
        SWS_L_PstL_1,
        SWS_L_PstL_2,
        SWS_L_PstL_3,

      SWS_L_MM_O,
        SWS_L_MM_1,
        SWS_L_MM_2,
        SWS_L_MM_3,

      SWS_L_Cfg_O,
        SWS_L_Cfg_1,
        SWS_L_Cfg_2,
        SWS_L_Cfg_3,

      SWS_L_GW_O,
        SWS_L_GW_1,
        SWS_L_GW_2,

      SWS_H_UserName,
      SWS_H_UserEmail,
      SWS_H_UserOrg,
      SWS_H_ReplyLine,
      SWS_H_CacheDir,
      SWS_H_SigFile,
      SWS_H_Editor,
      SWS_H_HdrCS,
      SWS_H_StartNW,
      SWS_H_VFont,
      SWS_H_VCol,
      SWS_H_AVTitle,
      SWS_H_SoftS,
      SWS_H_QuoteMode,
      SWS_H_QuoteStr,
      SWS_H_WinMode,
      SWS_H_UseSS,
      SWS_H_PRType,
      SWS_H_PWrap,
      SWS_H_GenMID,
      SWS_H_TBStyle,
      SWS_H_ALWght,
      SWS_H_FAQS,
      SWS_H_FAQD,
      SWS_H_AutoC,
      SWS_H_HDType,
      SWS_H_HDInc,
      SWS_H_SimCmd,
      SWS_H_GWMode,
      SWS_H_AEd,
      SWS_H_MaxSvPst,
      SWS_H_TagFile,
      SWS_H_TagSep,
      SWS_H_ETType,
      SWS_H_PrtDev,
      SWS_H_FFTBLoc,
      SWS_H_FFTBLab,
      SWS_H_FetchPol,
      SWS_H_Charset,
      SWS_H_AutoSave,
      SWS_H_EncBody,
      SWS_H_EncHdrs,
      SWS_H_StartArt,
      SWS_H_BrowseArts,

      SWS_L_Canceled,
      SWS_B_OK,
      SWS_B_OKCanc,

      SWS_L_ExitQuery,

      SWS_M_QuitNoSv,
      
      SWS_M_Network,
        SWS_M_Online,
        SWS_M_Offline,
      
      SWS_M_OpenWin,
        SWS_M_Launcher,
        SWS_M_GroupWin,
        SWS_M_ServWin,
        SWS_M_TCPWin,
        SWS_M_PostWin,
        SWS_M_MMWin,
        SWS_M_ConfigWin,

      SWS_L_FTypes,
      SWS_L_FActs,

      SWS_L_PToSend,
      SWS_L_PSent,

      SWS_LE_YN,
      SWS_LE_AskMod,
      SWS_TE_Loading,
      SWS_TE_InsBuff,
      SWS_TE_SaveBuff,
      SWS_LE_Cursor,
      SWS_HE_Insert,
      SWS_HE_Save,
      SWS_HE_Cut,
      SWS_HE_Copy,
      SWS_HE_Paste,
      SWS_HE_Undo,
      SWS_HE_Redo,
      SWS_HE_Bold,
      SWS_HE_Italic,
      SWS_HE_Uline,
      
      SW_AppBase_CatStrDef,
      
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      SWV_GrpListWin = 0,
      SWV_TcpListWin,
      SWV_SrvListWin,
      SWV_PstListWin,
      SWV_MMListWin,
      SWV_CfgListWin,

      SWV_NNewsWin,

      SWV_WinMax,
      SWV_LastProjWin = SWV_PstListWin,
   };

   enum {
      PEN_VCol = 0,
      PEN_Max
   };

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      Object     *SWA_NNews_GroupList;
      Object     *SWA_NNews_ServList;
      Object     *SWA_NNews_PostList;
      Object     *SWA_NNews_SvPostList;
      Object     *SWA_NNews_TCPList;
      Object     *SWA_NNews_MMList;
      Object     *SWA_NNews_FAList;
      Object     *SWA_NNews_ArtMgr;
      Object     *SWA_NNews_HdrList;
      APTR       *SWA_NNews_MemPool;
      STRPTR      SWA_NNews_UserName;
      STRPTR      SWA_NNews_UserEmail;
      STRPTR      SWA_NNews_UserOrg;
      ULONG       SWA_NNews_DONTREUSE;
      STRPTR      SWA_NNews_CacheDir;
      STRPTR      SWA_NNews_SigFile;
      STRPTR      SWA_NNews_Editor;
      Object     *SWA_NNews_ArtListCol;
      Object     *SWA_NNews_GlobSel;
      Object     *SWA_NNews_GlobHL;
      Object     *SWA_NNews_GlobKill;
      Object     *SWA_NNews_GlobDsp;
      Object     *SWA_NNews_GlobAct;
      ULONG       SWA_NNews_HdrCacheSize;
      ULONG       SWA_NNews_StartNWMode;
      Object     *SWA_NNews_VarList;
      Object     *SWA_NNews_ArtTpl;
      STRPTR      SWA_NNews_VFont;
      STRPTR      SWA_NNews_ArtViewTitle;
      ULONG       SWA_NNews_GrpListOpen;
      ULONG       SWA_NNews_TCPListOpen;
      ULONG       SWA_NNews_SrvListOpen;
      ULONG       SWA_NNews_MMListOpen;
      ULONG       SWA_NNews_PstListOpen;
      ULONG       SWA_NNews_CfgWinOpen;
      ULONG       SWA_NNews_GrpWinOpen;
      ULONG       SWA_NNews_GrpListLast;
      ULONG       SWA_NNews_TCPListLast;
      ULONG       SWA_NNews_SrvListLast;
      ULONG       SWA_NNews_MMListLast;
      ULONG       SWA_NNews_PstListLast;
      ULONG       SWA_NNews_CfgWinLast;
      STRPTR      SWA_NNews_ReplyLine;
      ULONG       SWA_NNews_FUpMode;
      ULONG       SWA_NNews_QuoteMode;
      STRPTR      SWA_NNews_QuoteStr;
      ULONG       SWA_NNews_QuoteMaxLen;
      ULONG       SWA_NNews_SoftScroll;
      ULONG       SWA_NNews_PostWrap;
      ULONG       SWA_NNews_GenMsgID;
      ULONG       SWA_NNews_ToolBarStyle;
      ULONG       SWA_NNews_ArtListWght;
      DTImgCache *SWA_NNews_DTImages;
      MUI_PenSpec SWA_NNews_Pens[PEN_Max];
      STRPTR      SWA_NNews_FAQServer;
      STRPTR      SWA_NNews_FAQDir;
      ULONG       SWA_NNews_AutoClip;
      ULONG       SWA_NNews_HdrDLType;
      ULONG       SWA_NNews_PostWrapType;
      ULONG       SWA_NNews_UseSigSep;
      ULONG       SWA_NNews_HdrInc;
      ULONG       SWA_NNews_SimulCmds;
      ULONG       SWA_NNews_GrpWinMode;
      ULONG       SWA_NNews_AutoStartEd;
      STRPTR      SWA_NNews_ButtonPath;
      ULONG       SWA_NNews_MaxSavedPosts;
      STRPTR      SWA_NNews_TagLineFile;
      STRPTR      SWA_NNews_TagLineSep;
      ULONG       SWA_NNews_EnterThread;
      STRPTR      SWA_NNews_PrinterDev;
      ULONG       SWA_NNews_FFTBLoc;
      ULONG       SWA_NNews_FFTBLab;
      ULONG       SWA_NNews_FetchPolicy;
      ULONG       SWA_NNews_Charset;
      ULONG       SWA_NNews_AutoSave;
      ULONG       SWA_NNews_Enc_Body;
      ULONG       SWA_NNews_Enc_Hdrs;
      ULONG       SWA_NNews_StartArt;
      ULONG       SWA_NNews_BrowseArts;

      Object      *Obj, *ArtLst,*DTImg;

      Object     *Config, *MMGrp, *PLGrp, *ToolBarGrp;
      ULONG       LastTBStyle;

      STRPTR CfgTabs[SWS_L_CfgLast - SWS_L_CfgTab0 + 2];
      STRPTR FltTabs[SWS_L_FltLast - SWS_L_FltTab0 + 2];
      STRPTR MMTabs[3];
      STRPTR PLTabs[3];

      struct NWin {
         Object  *Window;
         Object  *RGroup;
         Object **Contents;
         STRPTR   Title;
         ULONG    ID;
         ULONG    OpenVar;
         ULONG    LastVar;
      } Win[SWV_WinMax];
   } SW_IDATA;

#  define NOTP    MUIM_Notify, MUIA_Pressed, FALSE


#  define TBHasLab(style) (style == SWV_NNews_TB_HorizLab || \
                           style == SWV_NNews_TB_VertLab)

#  define TBGrp(style)                                               \
   MUI_NewObject(MUIC_Group,                                         \
                 MUIA_Group_Horiz, (style==SWV_NNews_TB_HorizLab ||  \
                                    style==SWV_NNews_TB_HorizNoLab), \
                 MUIA_Group_SameSize,  TRUE,                         \
                 MUIA_InnerTop,        0,                            \
                 MUIA_InnerBottom,     0,                            \
                 MUIA_InnerLeft,       0,                            \
                 MUIA_InnerRight,      0

#  define ebi_Bold   "PROGDIR:Images/IFF.eb_Bold"
#  define ebi_Copy   "PROGDIR:Images/IFF.eb_Copy"
#  define ebi_Copy_G "PROGDIR:Images/IFF.eb_Copy_G"
#  define ebi_Cut    "PROGDIR:Images/IFF.eb_Cut"
#  define ebi_Cut_G  "PROGDIR:Images/IFF.eb_Cut_G"
#  define ebi_Insert "PROGDIR:Images/IFF.eb_Insert"
#  define ebi_Italic "PROGDIR:Images/IFF.eb_Italic"
#  define ebi_Paste  "PROGDIR:Images/IFF.eb_Paste"
#  define ebi_Save   "PROGDIR:Images/IFF.eb_Save"
#  define ebi_Uline  "PROGDIR:Images/IFF.eb_Uline"
#  define ebi_Undo   "PROGDIR:Images/IFF.eb_Undo"
#  define ebi_Undo_G "PROGDIR:Images/IFF.eb_Undo_G"
#  define ebi_Redo   "PROGDIR:Images/IFF.eb_Redo"
#  define ebi_Redo_G "PROGDIR:Images/IFF.eb_Redo_G"


# endif // C_NNEWS_INCLUDE_I
