# ifndef C_NGROUP_INCLUDE_I
#  define C_NGROUP_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NGroupList.h"
#  include "Libraries/C-NServerList.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-NArticleList.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NTCPList.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-NAFilter.h"
#  include "Libraries/C-NSFList.h"
#  include "Libraries/C-NSF-InGroup.h"
#  include "Libraries/C-NSF-SubjMatch.h"
#  include "Libraries/C-NSF-HeaderMatch.h"
#  include "Libraries/C-NSF-MatchThread.h"
#  include "Libraries/C-NSF-MatchThreadBase.h"
#  include "Libraries/C-NPostList.h"
#  include "Libraries/C-ListCol.h"
#  include "Libraries/C-ListSearch.h"
#  include "Libraries/C-VarList.h"
#  include "Libraries/C-DTImg.h"
#  include "Libraries/C-FileActs.h"
#  include "S-ListImages.h"
#  include "S-CmpObj.h"
#  include "S-MkClass.h"
#  include <limits.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NGroup
#  define SW_CNAME            "NGroup"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/Ngroup_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_M_AS_Read,
      SWS_M_AS_Unread,
      SWS_M_AS_Mk,
      SWS_M_AS_UnMk,
      SWS_M_AS_Cache,
      SWS_M_AS_Perm,
      SWS_M_AS_Uncache,
      SWS_M_AS_ResetNew,
      SWS_M_AT_Read,
      SWS_M_AT_Unread,
      SWS_M_AT_Mk,
      SWS_M_AT_UnMk,
      SWS_M_AT_Cache,
      SWS_M_AT_Perm,
      SWS_M_AT_Uncache,
      SWS_M_AT_ResetNew,
      SWS_M_AG_Read,
      SWS_M_AG_Unread,
      SWS_M_AG_Mk,
      SWS_M_AG_UnMk,
      SWS_M_AG_Cache,
      SWS_M_AG_Perm,
      SWS_M_AG_Uncache,
      SWS_M_AG_ResetNew,
   };
   
   enum {
      SWS_L_Status_Idle = 1,   // status messages: idle
      SWS_L_Status_Aborted,    //   aborted xfer
      SWS_L_Status_CheckCount, //   checking count
      SWS_L_Status_ReadDesc,   //   reading description
      SWS_L_Status_ReadNums,   //   reading article numbers
      SWS_L_Status_Headers,    //   reading header info
      SWS_L_Status_LoadOV,     //   load overview
      SWS_L_Status_SaveOV,     //   save overview
      SWS_L_Status_DLArt,      //   download articles
      SWS_L_Status_CleanCache, //   cleaning cache
      SWS_L_Status_Expiring  , //   expiring old arts

      SWS_L_Name,              // labels for UI
      SWS_L_Desc,
      SWS_L_Srv,
      SWS_L_MaxA,
      SWS_L_MaxK,
      SWS_L_ExpD,
      SWS_L_DLHd,
      SWS_L_FlHd,
      SWS_L_RonE,
      SWS_L_SigF,
      SWS_L_CatE,
      SWS_BL_CE_Ask,
      SWS_BL_CE_Keep,
      SWS_BL_CE_Del,
      SWS_L_MmCn,
      SWS_L_MmCs,
      SWS_L_PreF,
      SWS_L_AVT,
      SWS_L_NaMd,
      SWS_L_NA_Exit,
      SWS_L_NA_Read,
      SWS_L_NA_Both,
      SWS_L_NA_Never,
      SWS_L_NA_DontUse,
      SWS_L_ArtFont,
      SWS_L_ArtCol,
      SWS_L_IncD,
      SWS_L_RCach,

      SWS_L_TReadDesc,         // for TCP/server status messages
      SWS_L_TCheckCnt,
      SWS_L_THeaders,
      SWS_L_TArtFetch,
      SWS_L_TExpire,
      SWS_L_TBatchArts,

      SWS_L_FT_Count,

      SWS_L_ReadNews,
      SWS_L_SelFilters,
      SWS_L_DspFilters,
      SWS_L_Options,

      SWS_L_SelFltTitle,
      SWS_L_KillFltTitle,
      SWS_L_DispFltTitle,
      SWS_L_ActFltTitle,

      SWS_L_ArticleList,
      SWS_L_ArticleReader,

      SWS_L_SelFilterTxt,
      SWS_L_KillFilterTxt,
      SWS_L_DispFilterTxt,
      SWS_L_ActFilterTxt,
     
      SWS_L_OkCanc,
      SWS_L_KeepDelete,
      SWS_L_NoSvWarn,
      SWS_L_AskDelCache,
      SWS_L_AskDelDir,
      SWS_L_AskDelBt,
      SWS_L_Warn,

      SWS_L_AskFetch,
      SWS_L_AskFetchBt,
      SWS_L_FetchCanc,
      SWS_L_FetchCancOk,

      SWS_L_SaveArt,

      SWS_M_T_Read,
      SWS_M_T_Unread,
      SWS_M_T_Mk,
      SWS_M_T_UnMk,
      SWS_M_T_Cache,
      SWS_M_T_Perm,
      SWS_M_T_Uncache,
      SWS_M_T_ResetNew,

      SWS_M_Project,
        SWS_M_P_Save,
        SWS_M_P_Close,
        SWS_M_P_CloseNoSv,
        SWS_M_P_ToggleWins,
      SWS_M_Group,
         SWS_M_G_FetchNew,
         SWS_M_G_CleanCache,
         SWS_M_G_PostNew,
         SWS_M_G_Expire,
         SWS_M_G_FAQ,
         SWS_M_G_ResetArtPos,
         SWS_M_G_Stats,
      SWS_M_Articles,
        SWS_M_A_Select,
          SWS_M_A_S_Read,
          SWS_M_A_S_Unread,
          SWS_M_A_S_Marked,
          SWS_M_A_S_Cached,
          SWS_M_A_S_Uncached,
          SWS_M_A_S_Perm,
          SWS_M_A_S_New,
        SWS_M_A_SelectAll,
        SWS_M_A_ToggleSel,
        SWS_M_A_Search,
        SWS_M_A_LimSel,
        SWS_M_A_Unlimit,
        SWS_M_A_Remove,
           SWS_M_A_R_SelArts,
           SWS_M_A_R_SelThrds,
           SWS_M_A_R_UnRemove,
        SWS_M_A_Selected,
        SWS_M_A_Thread,
        SWS_M_A_Group,
      SWS_M_Article,
        SWS_M_E_SelectAll,
        SWS_M_E_SaveTxt,
          SWS_M_E_SV_Head,
          SWS_M_E_SV_Body,
          SWS_M_E_SV_HdBd,
          SWS_M_E_SV_RHead,
          SWS_M_E_SV_RBody,
          SWS_M_E_SV_RHdBd,
        SWS_M_E_Print,
          SWS_M_E_PR_Head,
          SWS_M_E_PR_Body,
          SWS_M_E_PR_HdBd,
          SWS_M_E_PR_RHead,
          SWS_M_E_PR_RBody,
          SWS_M_E_PR_RHdBd,
        SWS_M_E_Search,
        SWS_M_E_PostFup,
        SWS_M_E_ArtClip,
          SWS_M_E_AC_Subj,
          SWS_M_E_AC_From,
          SWS_M_E_AC_MsgID,
          SWS_M_E_AC_Date,
          SWS_M_E_AC_Groups,
          SWS_M_E_AC_Head,
          SWS_M_E_AC_Body,
          SWS_M_E_AC_HdBd,
        SWS_M_E_Redisp,
          SWS_M_E_RD_Raw,
          SWS_M_E_RD_Cooked,
      SWS_M_Partial,
        SWS_M_P_SelectAll,
        SWS_M_P_Check,
        SWS_M_P_DispOne,
        SWS_M_P_DispAll,

      SWS_H_Name,              // help text UI
      SWS_H_Desc,
      SWS_H_Srv,
      SWS_H_MaxA,
      SWS_H_MaxK,
      SWS_H_ExpD,
      SWS_H_DLHd,
      SWS_H_FlHd,
      SWS_H_RonE,
      SWS_H_SigF,
      SWS_H_CatE,
      SWS_H_MmCn,
      SWS_H_MmCs,
      SWS_H_PreF,
      SWS_H_AVT,
      SWS_H_NaMd,
      SWS_H_IncD,
      SWS_H_RCach,
      SWS_H_ArtFont,
      SWS_H_ArtCol,

      SWS_H_ArtList,
      SWS_H_Th1,
      SWS_H_Th2,
      SWS_H_Th3,
      SWS_H_ReadFlt,
      SWS_H_UnrdFlt,
      SWS_H_AuthFlt,
      SWS_H_SView,
      SWS_H_DView,
      SWS_H_AView,
      SWS_H_HView,
      SWS_H_Kill,
      SWS_H_Remove,
      SWS_H_BrowseF,
      SWS_H_BrowseR,
      SWS_H_PostNew,
      SWS_H_PostFup,
      SWS_H_Stop,
      SWS_H_TUp,
      SWS_H_TDn,
      SWS_H_ThreadFwd,
      SWS_H_ThreadBck,
      SWS_H_ArtFwd,
      SWS_H_ArtBck,
      SWS_H_ALQSearch,
      SWS_H_ALSort,

      SWS_H_ALMatchHdr,
      SWS_H_ALUpdDisp,
      SWS_H_ALFromRam,
         
      SWS_L_ALQSTitle,    // for artlist qsearch
      SWS_L_ALItem,
      SWS_L_ALItems,
      SWS_L_ALQS_Hdr,
      SWS_L_ALQS_Upd,
      SWS_L_ALQS_SRam,
      SWS_L_ALQS_Loading,
      SWS_L_ALQS_Freeing,
      SWS_L_ALQS_TooBig,
      SWS_L_ALQS_OK,

      SWS_T_LocFlt,
      SWS_T_GlobFlt,

      SWS_B_Upd,
      SWS_B_Close,
      SWS_B_Edit,
      SWS_B_Run,
      SWS_H_Upd,
      SWS_H_Close,
      SWS_H_Edit,
      SWS_H_Run,

      SWS_L_RemArts,
      SWS_L_RemThrds,
      SWS_L_UnRemove,

      SWS_H_RemArts,
      SWS_H_RemThrds,
      SWS_H_UnRemove,
         
      SWS_L_KillArts,
      SWS_L_KillThrds,
      SWS_L_KillAuth,
      SWS_L_KillCurrent,
      SWS_L_KillFuture,
      SWS_L_KillCanc,
      SWS_L_KillKill,
      SWS_L_KillExp,
      SWS_L_KillLoc,
      SWS_L_KillThisGr,
      SWS_L_KillAllGr,

      SWS_H_KillType,
      SWS_H_KillWhen,
      SWS_H_KillExp,
      SWS_H_KillCanc,
      SWS_H_KillKill,
      SWS_H_KillWhere,

      SWS_Tab_Misc,
      SWS_Tab_Cache,
      SWS_Tab_StEx,

      SWS_L_FAQWinT,
      SWS_B_FAQView,
      SWS_B_FAQUpd,
      SWS_B_FAQDel,
      SWS_B_FAQSelAll,
      SWS_B_FAQAbort,
      SWS_B_FAQAct,
      SWS_H_FAQList,
      SWS_H_FAQView,
      SWS_H_FAQUpd,
      SWS_H_FAQDel,
      SWS_H_FAQSelAll,
      SWS_H_FAQAbort,
      SWS_H_FAQAct,
      SWS_T_FAQCount,
      SWS_T_FAQListFetch,
      SWS_T_FAQFetch,
      SWS_T_FAQAbort,
      SWS_T_FAQName,
      SWS_T_NoFAQs,
      SWS_T_GotFAQ,
      SWS_T_NoFAQDir,

      SWS_H_FAQb,

      SWS_T_PM_Hdrs,
      SWS_T_PM_Arts,

      SWS_GST_Count,
      SWS_GST_Percent,
      SWS_GST_Size,
      SWS_GS_Title,
      SWS_GS_Total,
      SWS_GS_Read,
      SWS_GS_Unread,
      SWS_GS_Killed,
      SWS_GS_Cached,
      SWS_GS_Permanent,
      SWS_GS_Hilighted,
      SWS_GS_New,
      SWS_GS_Displayed,

      SWS_L_MissMsg,
      SWS_L_MissCont,
         
      SWS_Err_NoSuchGroup,
      SWS_Err_Unknown,
      SWS_Err_Alloc,
      SWS_Err_MakeChMk,
      SWS_Err_RemChMk,
      SWS_Err_NoName,
      SWS_Err_MsgIdNotFound,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      SWV_NGroup_FetchMax    =10000,   // max arts to fetch at once
      SWV_NGroup_TitleLen    =   50,   // max title length;
      SWV_NGroup_ArrayInc    =  250,   // article array increment;
      SWV_NGroup_MakeChMk    =    0,   // for cache mod markers
      SWV_NGroup_RemChMk     =    1,   // for cache mod markers
      SWV_NGroup_TestChMk    =    2,   // for cache mod markers
      SWV_NGroup_BatchArtPre =    8,   // batch art prefetch amount
      SWV_NGroup_FetchHdtPre =    8,   // header fetch prefetch amount
   };

   // -- temp (non-saved) flags ----------------------------------------------

   enum {
      SWF_NGroup_OVDir      = (1<<0),  // flags: have OV directory
      SWF_NGroup_CacheChg   = (1<<1),  //    cache state change
      SWF_NGroup_NoSaveOV   = (1<<2),  //    don`t save OV on dispose
      SWF_NGroup_RES1       = (1<<3),  //    reserved: def in external hdr
      SWF_NGroup_Action     = (1<<4),  //    pending action
   };

   enum {
      SWF_NGroup_FetchBytes = (1<<0),  // use XHDR to get BYTES: during fetch
      SWF_NGroup_FetchXOver = (1<<1),  // use XOVER to fetch headers
   };

#  define SWV_NGroup_ChMarkDir  "CMod"
#  define SWV_DefOVRoot         "PROGDIR:Cache"
#  define SWV_ArtKillFlt        "Thread/Author Kill"

   enum {
      PEN_ArtCol = 0,
      PEN_Max
   };

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      UBYTE      *SWA_NGroup_Name;
      UBYTE      *SWA_NGroup_Desc;
      UBYTE      *SWA_NGroup_Server1;
      ULONG       SWA_NGroup_RdDays;
      ULONG       SWA_NGroup_RdMin;
      ULONG       SWA_NGroup_RdTick;
      ULONG       SWA_NGroup_HistArtCount;
      ULONG       SWA_NGroup_HistArtKill;
      ULONG       SWA_NGroup_HistArtRead;
      ULONG       SWA_NGroup_PrevArtNum;
      ULONG       SWA_NGroup_FirstArtNum;
      ULONG       SWA_NGroup_LastArtNum;
      ULONG       SWA_NGroup_AvailArtNum;
      ULONG       SWA_NGroup_NewArtNum;
      ULONG       SWA_NGroup_MaxArticles;
      ULONG       SWA_NGroup_MaxKeep;
      ULONG       SWA_NGroup_ExpireDays;
      Object     *SWA_NGroup_ArtMgr;
      ULONG       SWA_NGroup_Flags;
      ULONG       SWA_NGroup_PermFlags;
      STRPTR      SWA_NGroup_Status;
      APTR       *SWA_NGroup_MemPool;
      ULONG       SWA_NGroup_ReaderOpen;
      Object     *SWA_NGroup_SrvList;
      ULONG       SWA_NGroup_DLHdrStart;
      ULONG       SWA_NGroup_FullHeaders;
      Object     *SWA_NGroup_ThreadMgr;
      LONG        SWA_NGroup_ThreadFT;
      LONG        SWA_NGroup_ReadFT;
      LONG        SWA_NGroup_AuthFT;
      STRPTR      SWA_NGroup_AuthFTName;
      Object     *SWA_NGroup_SelFlt;
      Object     *SWA_NGroup_KillFlt;
      Object     *SWA_NGroup_DispFlt;
      Object     *SWA_NGroup_GrpDefFlt;
      Object     *SWA_NGroup_ActFlt;
      ULONG       SWA_NGroup_Sort1;
      ULONG       SWA_NGroup_Dir1;
      ULONG       SWA_NGroup_Sort2;
      ULONG       SWA_NGroup_Dir2;
      ULONG       SWA_NGroup_Sort3;
      ULONG       SWA_NGroup_Dir3;
      ULONG       SWA_NGroup_ReadOnExit;
      ULONG       SWA_NGroup_ReadArtCnt;
      ULONG       SWA_NGroup_TotArtCnt;
      STRPTR      SWA_NGroup_SigFile;
      ULONG       SWA_NGroup_CacheExit;
      ULONG       SWA_NGroup_BadGroup;
      Object     *SWA_NGroup_ArtListCol;
      ULONG       SWA_NGroup_MemCacheNum;
      ULONG       SWA_NGroup_MemCacheKb;
      ULONG       SWA_NGroup_PrefetchNum;
      STRPTR      SWA_NGroup_ArtViewTitle;
      STRPTR      SWA_NGroup_LastFile;
      STRPTR      SWA_NGroup_LastDir;
      STRPTR      SWA_NGroup_LastPatt;
      STRPTR      SWA_NGroup_LastMsgID;
      STRPTR      SWA_NGroup_LastRdSrv;
      ULONG       SWA_NGroup_LastRdMsgE;
      ULONG       SWA_NGroup_LastRdMsgC;
      ULONG       SWA_NGroup_NewArtMd;
      Object     *SWA_NGroup_ArtView;
      STRPTR      SWA_NGroup_ArtFont;
      ULONG       SWA_NGroup_UseArtFont;
      ULONG       SWA_NGroup_UseArtCol;
      ULONG       SWA_NGroup_IncrDisp;
      ULONG       SWA_NGroup_ReadnCache;
      ULONG       SWA_NGroup_Moderated;
      MUI_PenSpec SWA_NGroup_Pens[PEN_Max];

      Object  *ReaderWin, *ArtViewWin, *StatMsg, *AttrUI, *StatWin;
      Object  *App, *ALQSearch;

      STRPTR  OVDir;
      STRPTR  StatWinTitle;

      Object  *ArtList, *FFToolBar, *FFTBGrp;
      Object  *Filter_Read,   // pre-defined UI filters
              *Filter_Auth,
              *Filter_ThBase,
              *Filter_MsgID,
              *Filter_BaseMsgID,
              *Filter_GlobSel,
              *Filter_GlobHL,
              *Filter_GlobKill,
              *Filter_GlobDsp,
              *Filter_GlobAct;
      Object  *StrF_Auth;

      Object  *B_Up, *B_Dn, *B_Read, *B_Unrd, *B_Th, *B_Auth, *G_Thrd,
              *B_Th1, *B_Th2, *B_Th3, *B_ThCnt, *ALQS_cg, *ALQS_upd, *ALQS_sram;

      Object  *ProgGauge;
      LONG     ProgPos;
      Object  *B_ProgStop;

      ULONG    MaxArticles;
      ULONG    SimulCmds;
      LONG     LastClickCol;
      Object  *OldestArt;
      
      struct  {
         Object *Obj;           // obj to track in the gauge
         ULONG   Attr;          // attr to track
         STRPTR  Status;        // status message
         ULONG   AbortButton;   // T if we can be aborted
      } ProgInfo[SWV_GC_MAX];

      LONG     FetchFirst;      // article header fetch state info
      LONG     FetchLast;   
      LONG     FetchLineCnt;    // count of lines in buffer
      LONG     FetchLineMax;    // max lines in fetch buffer
      LONG     FetchArtCnt;     // fetched article count
      UBYTE  **FetchLines;      // fetched line storage
      ULONG    FetchCnt;        // fetch count
      ULONG    FetchPrevCnt;    // prev fetch count
      ULONG   *FetchNums;       // article numbers in group
      LONG     FetchNumCnt;     // number of entries in above array
      LONG     FetchNumPos;     // current position in numbers array
      Object  *FetchObj;        // object to notify about new articles
      ULONG    FetchMthd;       // method to call to submit them
      Object  *FetchQObj;       // object to query for a message ID
      ULONG    FetchQMthd;      // method to query for a message ID
      ULONG    FetchStart;      // first entry to fetch
      ULONG    FetchFlags;      // fetch flags
      ULONG    FetchCmdCnt;     // command count
      ULONG    FetchCmdMax;     // command count max
      STRPTR  *FetchOverview;   // fetch overview records
      ULONG    FetchOVCount;    // count of overview fields
      ULONG    FetchBytesCnt;   // bytes header value
   
      struct {   
         Object  *Srv;          // server we`re reading from;
         ULONG    ID;           // ID number so we can cancel, etc;
         Object  *CmdObj;       // object to signal when we`re done;
         ULONG    CmdMethod;    // method to send to object
         ULONG    CmdData;      // data to send
         STRPTR   CmdTitle;     // command TCP title
         STRPTR   ProgMsg;      // progress display title
      } SrvInfo[SWV_GC_MAX];

      LONG      PrevThPos[SWF_NGROUP_ThreadFlt_MAX];

      LONG      BrowseNoBase;   // base of thread was gone
      LONG      BrowseThPos;    // browse thread position
      LONG      ArtListQuiet;   // artlist quiet
      Object  **KList;          // ptr to killed list
      
      Object  **GrpArticles;    // articles in group
      ULONG     GrpArticleCnt;  // number of articles in array
      ULONG     GrpArticleMax;  // size of array

      Object  **SelArticles;    // selected articles
      ULONG     SelArticleCnt;  // number of articles in array
      ULONG     SelArticleMax;  // size of array

      Object  **UIArticles;     // displayed UI articles
      ULONG     UIArticleCnt;   // number of articles in array
      ULONG     UIArticleMax;   // size of array

      Object  **CacheArticles;  // cached articles
      ULONG     CacheArticleCnt;// number of articles in array
      ULONG     CacheArticleMax;// size of array

      Object  **BatchArticles;  // articles to batch D/L
      ULONG     BatchArticleCnt;// count of articles
      ULONG     BatchArticlePos;// position in batch array
         
      Object  **DLQueue;        // articles to download body
      UBYTE    *DLQueueCmd;     // what to do with them
      LONG      DLCnt;          // count of articles in above
      LONG      DLMax;          // size of queue
      LONG      DLNum;          // # of articles (for statmsg)
      LONG      DLPendCmd;      // pending cmd
      LONG      DLInstRead;     // instant read (not through queue)
      Object   *DLPending;      // pending download article
      STRPTR    DLStatMsg;      // status msg

      TEXT     *VarBuf;         // for variable generation

      Object   *FAQListWin;     // FAQ list window
      Object   *FAQListObj;     // FAQ list object
      STRPTR    FAQWinTitle;    // FAQ window title

      BPTR      DBOut;          // debug file
      ULONG     DBType;         // debug type
   } SW_IDATA;

   typedef struct FAQEntry
   {
      ULONG Cached;
      TEXT  FAQName[FMSIZE];
   } FAQEntry;
   
   SW_MkClassExterns(FAQList);

   enum {
      SWV_DB_Fetch   = (1<<0),
      SWV_DB_BatchDL = (1<<1),
   };

#  define NOTP  MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK  MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

#  define AV(s)   (SWPoolAV(idata->SWA_NGroup_MemPool, (s)))
#  define SD(s)   (SWPoolStrDup(idata->SWA_NGroup_MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->SWA_NGroup_MemPool, (UBYTE *)(m))
#  define RV(s,m) (SWPoolRV(idata->SWA_NGroup_MemPool, (s), (UBYTE **)(m)))
#  define AVDup(s) SWPoolStrDup(idata->SWA_NGroup_MemPool, (s))
#  define ASize(x) ((x)*sizeof(Object *))

#  define GP_DataInt(idata, f, d) GF_GetExt(idata->FetchLines, f, d, \
                                            SWV_NGroup_OVCount)
#  define GP_Size(idata) (SWV_NGroup_OVCount * SWV_NGroup_FetchMax)

#  define GP_NoName(idata) (SW_STRISNULL(idata->SWA_NGroup_Name))

#  define CFlags (SWF_NArticle_CachedBody | SWF_NArticle_Permanent)

#  define ThrdMode(idata) ((idata)->SWA_NGroup_ThreadFT==SWF_NGroup_ThreadFlt_Base)

LONG AskUnCache(Object *o, SW_IDATA *idata, ULONG buttons, ULONG prompt,
                ULONG Save, LONG DefReply, LONG RemReply,
                LONG KeepReply);
void KillGaugeNfy(Object *o, SW_IDATA *idata, LONG Cmd);
void InitGaugeNfy(Object *o, SW_IDATA *idata, LONG Cmd);
void StartGauge(Object *o, SW_IDATA *idata, Object *Track, ULONG Attr,
                STRPTR Status, LONG Cmd, ULONG AbortButton);
void StopGauge(Object *o, SW_IDATA *idata, LONG Cmd);
void InitNameFields(Object *o, SW_IDATA *idata, STRPTR NewName);
STRPTR OVDir(Object *o, SW_IDATA *idata, STRPTR Sub);
void SetArtWinTitle(SW_IDATA *idata);
LONG SetActiveObj(Object *l, Object *Obj);
STRPTR MakeCacheMkName(Object *o, SW_IDATA *idata, STRPTR Buff);
void UpdGaugeStat(Object *o, SW_IDATA *idata, STRPTR Status, LONG Cmd);
void ArtListQuiet(SW_IDATA *idata, ULONG Quiet);
void AbortAll(Object *o, SW_IDATA *idata);
void AppSleep(SW_IDATA *idata, ULONG sleep);
void ArtListAdd(SW_IDATA *idata, Object **ArtArray, ULONG ArtCount, ULONG Clear);
LONG CacheMk(Object *o, SW_IDATA *idata, ULONG Mode);
ULONG IncArray(SW_IDATA *idata, ULONG Count, ULONG Size, Object ***a);
Object **GetSelArts(SW_IDATA *idata, ULONG Threads, ULONG *Count);
void ExpireOld(Object *o, SW_IDATA *idata, ULONG Final, ULONG Oldest);
void ResetNew(Object *o, SW_IDATA *idata);
void AddPopSrv(Object *o, SW_IDATA *idata);
void FreeFetchData(SW_IDATA *idata);
STRPTR MakeTitle(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG Str);
void DLQueueClear(Object *o, SW_IDATA *idata);
void ClearBatchDL(SW_IDATA *idata);
ULONG DLQueueAdd(Object *o, SW_IDATA *idata, Object *Article, UBYTE Cmd, LONG Last);
ULONG BeginLFCmd(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG Method, ULONG Action, ULONG *msg);
ULONG EndLFCmd(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG State);
LONG RemFromList(Object **InList, ULONG *Size, Object **KillList, ULONG KLSize);
void MkFlt(SW_IDATA *idata, ULONG Type, ULONG Exp, ULONG ExpOK, ULONG Where);
void SelectAuth(SW_IDATA *idata);
ULONG BrowseBase(Object *o, SW_IDATA *idata, ULONG NewMode, ULONG Action);
void SendCmdState(Object *o, SW_IDATA *idata, ULONG Cmd, ULONG State);
void MsgBegin(Object *o, SW_IDATA *idata, SWP_NServer_StatusMsg *msg,
              ULONG st, ULONG Cmd);
void MsgEnd(Object *o, SW_IDATA *idata, SWP_NServer_StatusMsg *msg, ULONG Cmd);
Object *CheckSendCmd(Object *o, SW_IDATA *idata, ULONG *msg, ULONG Cmd);
ULONG CheckResCode(Object *o, SW_IDATA *idata,
                   SWP_NServer_StatusMsg *msg, ULONG OKCode);
void FetchHeaders(Object *o, SW_IDATA *idata, LONG Pos);
LONG FetchBatchArt(Object *o, SW_IDATA *idata, LONG Pos);
void ClearFetchLines(SW_IDATA *idata);
void ThrdMgrAdd(Object *o, SW_IDATA *idata, Object **Articles, ULONG ArtCnt);
void CountRead(Object *o, SW_IDATA *idata);
void NukeArtArrays(SW_IDATA *idata);
ULONG MemQueueAdd(SW_IDATA *idata, Object *Art);
void MemQueueDel(SW_IDATA *idata, ULONG All, Object *Art);
void ArtPrefetch(Object *o, SW_IDATA *idata);
ULONG ClosedGetOVStart(Object *o, SW_IDATA *idata);
void ClosedGetOVEnd(Object *o, SW_IDATA *idata);
void SetSortNotify(Object *o, SW_IDATA *idata);
Object *CreateALQSrch(Object *o, SW_IDATA *idata);
ULONG LimitSel(Object *o, SW_IDATA *idata);
ULONG ALQSWin(Object *o, SW_IDATA *idata);
ULONG ALSearchUIUpd(Object *o, SW_IDATA *idata);
Object *MkTB(SW_IDATA *idata, ULONG h, STRPTR Image, LONG Cmd);
Object *MkIB(SW_IDATA *idata, ULONG h, STRPTR Image, LONG Cmd);
Object *MkBt(ULONG l, ULONG h);
Object *ImageBt(SW_IDATA *idata, ULONG Mode, ULONG Ctrl, STRPTR Img,
                ULONG Help);
Object *ImageB(ULONG i, LONG Cmd, LONG h);
ULONG ChgHLFlt(Object *o, SW_IDATA *idata, Object **Articles, ULONG ArtCnt);
ULONG KillFltArray(Object *o, SW_IDATA *idata,
                          Object **ArtArray, ULONG ArtCount);
void SetWinCtrls(Object *o, SW_IDATA *idata, Object *Win);
void OpenDBFiles(Object *o, SW_IDATA *idata);
ULONG MPReportMissing(SW_IDATA *idata, MPInfo *MP);
ULONG RemArts(Object *o, SW_IDATA *idata, ULONG Threads, ULONG SameAuth,
              Object *DelObj, ULONG DelMeth, ULONG MarkAsRem);
ULONG IsOnline(SW_IDATA *idata);
void FetchIfPossible(Object *o, SW_IDATA *idata, Object *Art);
void MoveToStartArt(Object *o, SW_IDATA *idata);
Object *NextArt(SW_IDATA *idata, LONG *ArtNum, LONG Dir, ULONG Flags, ULONG TestRes);

extern SWAttrMapT SW_ATTRMAP[];
extern struct NewMenu MainMenu[];
extern STRPTR SW_CATSTR[];
extern HelpMap SW_HELPMAP[];


METHOD_EXTERN(SWM_NGroup_StartRead, Msg);
METHOD_EXTERN(SWM_NGroup_StopRead, SWP_NGroup_StopRead *);
METHOD_EXTERN(SWM_NGroup_CloseReaderWin, Msg);
METHOD_EXTERN(SWM_NGroup_OpenReaderWin, ULONG *);
METHOD_EXTERN(SWM_NGroup_CheckStats, ULONG *);
METHOD_EXTERN(SWM_NGroup_CheckStats_P, SWP_NServer_StatusMsg *);
METHOD_EXTERN(SWM_NGroup_ReadDesc, ULONG *);
METHOD_EXTERN(SWM_NGroup_ReadDesc_P, SWP_NServer_StatusMsg *);
METHOD_EXTERN(SWM_NGroup_Fetch, ULONG *);
METHOD_EXTERN(SWM_NGroup_Fetch_P1, SWP_NServer_StatusMsg *);
METHOD_EXTERN(SWM_NGroup_Fetch_P2, SWP_NServer_StatusMsg *);
METHOD_EXTERN(SWM_NGroup_Fetch_XOV_P1, SWP_NServer_StatusMsg *);
METHOD_EXTERN(SWM_NGroup_Fetch_XOV_P2, SWP_NServer_StatusMsg *);
METHOD_EXTERN(SWM_NGroup_Abort, SWP_NGroup_Abort *);
METHOD_EXTERN(SWM_NGroup_FetchAdd, SWP_NGroup_Fetch *);
METHOD_EXTERN(SWM_NArticleMgr_NewArticles, ULONG *);
METHOD_EXTERN(SWM_NGroup_SaveOV, ULONG *);
METHOD_EXTERN(SWM_NGroup_LoadOV, ULONG *);
METHOD_EXTERN(SWM_NGroup_DLFetchNext, Msg);
METHOD_EXTERN(SWM_NArticle_StatMsg, SWP_NArticle_StatMsg *);
METHOD_EXTERN(SWM_NGroup_MenuCall, SWP_NGroup_MenuCall *);
METHOD_EXTERN(SWM_NGroup_DeleteOVDir, ULONG *);
METHOD_EXTERN(SWM_NGroup_CacheSel, SWP_NGroup_CacheSel *);
METHOD_EXTERN(SWM_NGroup_TestOVDir, Msg);
METHOD_EXTERN(SWM_NGroup_CleanCache, Msg);
METHOD_EXTERN(SWM_NGroup_GL_Arts_P1, ULONG *);
METHOD_EXTERN(SWM_NGroup_GL_Arts_P2, SWP_NServer_StatusMsg *);
METHOD_EXTERN(SWM_NGroup_NoSaveOV, Msg);
METHOD_EXTERN(SWM_NGroup_GL_Mark, SWP_NGroup_GL_Mark *);
METHOD_EXTERN(SWM_NGroup_OnOffLine, SWP_NGroup_OnOffLine *);
METHOD_EXTERN(SWM_NGroup_KillSel, SWP_NGroup_KillSel *);
METHOD_EXTERN(SWM_NArticleMgr_DelArticles, ULONG *);
METHOD_EXTERN(SWM_NGroup_ChgDspFlt, Msg);
METHOD_EXTERN(SWM_NGroup_RunActFlt, Msg);
METHOD_EXTERN(SWM_NGroup_Browse, SWP_NGroup_Browse *);
METHOD_EXTERN(SWM_NGroup_SaveArt, SWP_NGroup_SaveArt *);
METHOD_EXTERN(SWM_NGroup_ExpireOld, ULONG *);
METHOD_EXTERN(SWM_NGroup_ExpireOld_P1, SWP_NServer_StatusMsg *);
METHOD_EXTERN(SWM_NNews_NewProj, Msg);
METHOD_EXTERN(SWM_NGroup_OpenFAQList, Msg);
METHOD_EXTERN(SWM_NGroup_CloseFAQList, Msg);
METHOD_EXTERN(SWM_NGroup_ChgHLFlt, Msg);
METHOD_EXTERN(SWM_NGroup_UpdArtList, SWP_NGroup_UpdArtList *);
METHOD_EXTERN(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *);
METHOD_EXTERN(SWM_NGroup_ToggleWins, Msg);
METHOD_EXTERN(SWM_NGroup_GroupStats, ULONG *);
METHOD_EXTERN(SWM_NGroup_CloseStatWin, Msg);
METHOD_EXTERN(SWM_NGroup_MPSelect, ULONG *);
METHOD_EXTERN(SWM_NGroup_MPCheck, ULONG *);
METHOD_EXTERN(SWM_NGroup_MPOne, ULONG *);
METHOD_EXTERN(SWM_NGroup_MPAll, ULONG *);
METHOD_EXTERN(SWM_NGroup_WinCtrl, SWP_NGroup_WinCtrl *);
METHOD_EXTERN(SWM_NGroup_Select, SWP_NGroup_Select *);
METHOD_EXTERN(SWM_NGroup_Remove, SWP_NGroup_Remove *);
METHOD_EXTERN(SWM_NGroup_UnRemove, Msg);
METHOD_EXTERN(SWM_NGroup_ViewMsgID, ULONG *);

# endif // C_NGROUP_INCLUDE_I
