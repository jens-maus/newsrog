# ifndef C_NPOST_INCLUDE_I
#  define C_NPOST_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NPost.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NPostList.h"
#  include "Libraries/C-NTCPList.h"
#  include "Libraries/C-NServerList.h"
#  include "Libraries/C-NArticleList.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NHdrList.h"
#  include "Libraries/C-NDF-Rewrap.h"
#  include "Libraries/C-VarList.h"
#  include "Libraries/S-Lists.h"
#  include "MUI/TextEditor_mcc.h"
#  include <dos/notify.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NPost
#  define SW_CNAME            "NPost"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NPost_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Subj = 1,
      SWS_L_Grp,
      SWS_L_FUps,
      SWS_L_Dist,
      SWS_L_SigF,
      SWS_L_CstH,

      SWS_L_YN,
      SWS_L_FUpWarn,
      SWS_L_AskFUp,
      SWS_L_WarnDel,
      SWS_L_AskDel,
      SWS_L_PostSucc,
      SWS_L_OK,
      SWS_L_Canceled,
      SWS_L_PEdit,
      SWS_L_PView,
      SWS_L_PViewAll,
      SWS_L_PViewEnc,
      SWS_L_PViewPost,
      SWS_L_Cursor,

      SWS_B_Post,
      SWS_B_Edit,
      SWS_B_Queue,
      SWS_B_Canc,
      SWS_B_Del,

      SWS_T_TCPTitle,
      SWS_T_TCPEnd,
      SWS_T_Edit,
      SWS_T_ArtTxt,
      SWS_T_SaveBuff,
      SWS_T_InsBuff,

      SWS_H_Subj,
      SWS_H_Grp,
      SWS_H_FUps,
      SWS_H_Dist,
      SWS_H_SigF,
      SWS_H_CstH,

      SWS_H_Post,
      SWS_H_Edit,
      SWS_H_Queue,
      SWS_H_Cancel,
      SWS_H_Del,

      SWS_H_List,
      SWS_H_WList,

      SWS_NG_H_LeftNG,
      SWS_NG_H_Right,
      SWS_NG_H_LeftFU,

      SWS_M_Text,   
      SWS_M_T_Save, 
      SWS_M_T_Ins,  
      SWS_M_T_InsQuoted,
      SWS_M_T_Clear,  
      SWS_M_Edit,   
      SWS_M_E_Undo, 
      SWS_M_E_Redo, 
      SWS_M_E_Cut,  
      SWS_M_E_Copy, 
      SWS_M_E_Paste,
      SWS_M_E_Rewrap,

      SWS_Err_Offline,
      SWS_Err_EditErr,
      SWS_Err_Malloc,
      SWS_Err_FileRead,
      SWS_Err_FileWrite,
      SWS_Err_NotAscii,
      SWS_Err_NoPost,
      SWS_Err_BadBody,
      SWS_Err_NoGroups,
      SWS_Err_NoSubj,
      SWS_Err_NoSig,
      SWS_Err_PostSend,
      SWS_Err_PostAborted,
      SWS_Err_MkPostDir,
      SWS_Err_SaveText,
      SWS_Err_LineTooLong,
      SWS_Err_BadEmail,
      SWS_Err_Rename,
      SWS_Err_BadGroups,
      SWS_Err_Body8bit,
      SWS_Warn_LineTooLong,
      SWS_Warn_QuotePct,
      SWS_Warn_TestPost,
      SWS_Warn_TestPost_Bt,
      SWS_Warn_GroupCount,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

#  define SWF_NPost_EscDot ".."     // ".." -> "."
#  define SWF_NPost_End    "."      // end of post

#  define SWV_NPost_BetaMsg "[NewsRog Beta Test Version]\r\n\r\n"
   
   enum {
      SWV_MsgIDLen           = 180,
      SWV_MaxPostLineLen     = 201,
      SWV_WarnPostLineLen    = 80,
      SWV_WarnQuotePct       = 70,
      SWV_WarnQuoteMinLines  = 10,
      SWV_MaxTaglineLen      = 80,
      SWV_MaxTaglineLines    = 8,
      SWV_WarnLotaGroups     = 4,
   };

   enum {
      SWV_NPost_ModeEdit = 0,        // editor
      SWV_NPost_ModeView,            // plain preview
      SWV_NPost_ModeViewAll,         // view all
      SWV_NPost_ModeViewEnc,         // view encoded
      SWV_NPost_ModeViewMAX,
   };

#  define ModeSeesEncoding(m) ((m)==SWV_NPost_ModeViewEnc)

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      STRPTR        SWA_NPost_Subj;
      STRPTR        SWA_NPost_Grp;
      STRPTR        SWA_NPost_Refs;
      STRPTR        SWA_NPost_FUps;
      STRPTR        SWA_NPost_MsgID;
      STRPTR        SWA_NPost_SigFile;
      STRPTR        SWA_NPost_Distrib;
      STRPTR        SWA_NPost_Server;
      ULONG         SWA_NPost_Lines;
      ULONG         SWA_NPost_Bytes;
      ULONG         SWA_NPost_Mod;
      ULONG         SWA_NPost_PostedOK;
      ULONG         SWA_NPost_UseCustHdrs;
      STRPTR        SWA_NPost_PostDir;
      ULONG         SWA_NPost_NoDel;
      STRPTR        SWA_NPost_TagLine;
      LONG          SWA_NPost_TagLinePos;
      ULONG         SWA_NPost_NewPost;
      
      Object       *UIWin, *AttrUI, *UI_Post, *TEditor,
                    *G_Group, *G_View, *G_Current;
      TEXT          FS1[FMSIZE], FS2[FMSIZE], FS3[FMSIZE], Tmp[180];
      ULONG         IDel;

      ULONG         Post_HasNonASCII;   // T if post *body* has non-ascii chars
      STRPTR        Post_CharSet;       // us-ascii or some ISO set probably
      STRPTR        Post_Encoding;      // ommit for ASCII, else quoted-printable
      STRPTR        Post_ContentType;   // text/plain for now
      STRPTR        QPLineBuff;         // quoted printable line buff
      
      Object       *CmdObj;
      ULONG         CmdMethod;
      ULONG         CmdData;
      Object       *CmdSrv;
      LONG          CmdID;
      Object       *CmdTCP;     // TCP object for posting

      Object       *VarSrchGrp, *VarSrchArt;
   } SW_IDATA;

   typedef struct _postdata {
      STRPTR Subj, Grps, FUps, ReplLine, Refs, QuotePrefix, SigFile;
      ULONG  SubjAlloc, QuoteMode;
   } PostData;

#  define AV(s)   (SWPoolAV(NN_MemPool(idata), (s)))
#  define FV(m)    SWPoolFV(NN_MemPool(idata), (UBYTE *)(m))

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define IsAscii(c) ((c) >= 32 || (c)=='\t')
#  define QPChar(c)  ((c) >= 128)


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

#  define LBSize 8192
#  define PBSize 4096

# endif // C_NPOST_INCLUDE_I
