# ifndef C_NGROUP_INCLUDE
#  define C_NGROUP_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10500)

   enum {
      SWA_NGroup_Name  = SW_FIRSTTAG,       // [SGI] group name
      SWA_NGroup_Desc,                      // [SGI] group description
      SWA_NGroup_Server1,                   // [SGI] use alternate server
      SWA_NGroup_RdDays,                    // [SGI] last read DateStamp
      SWA_NGroup_RdMin,                     // [SGI] last read DateStamp
      SWA_NGroup_RdTick,                    // [SGI] last read DateStamp
      SWA_NGroup_HistArtCount,              // [SGI] historical artical  count
      SWA_NGroup_HistArtKill,               // [SGI] historical articals killed
      SWA_NGroup_HistArtRead,               // [SGI] historical articals read
      SWA_NGroup_PrevArtNum,                // [SGI] prev max article number
      SWA_NGroup_FirstArtNum,               // [SGI] 1st article number
      SWA_NGroup_LastArtNum,                // [SGI] last article number
      SWA_NGroup_AvailArtNum,               // [SGI] available # from server
      SWA_NGroup_NewArtNum,                 // [SGI] # of new articles
      SWA_NGroup_MaxArticles,               // [SGI] max articles to read
      SWA_NGroup_ExpireDays,                // [SGI] expire articles limit
      SWA_NGroup_Flags,                     // [SGI] flags
      SWA_NGroup_Status,                    // [SGI] status string
      SWA_NGroup_MemPool,                   // [SGI] memory pool
      SWA_NGroup_HdrPctDone,                // [ G ] % complete, header fetch
      SWA_NGroup_ArtMgr,                    // [SGI] article manager object
      SWA_NGroup_ReaderOpen,                // [SGI] T if reader is open
      SWA_NGroup_SrvList,                   // [SGI] server list object
      SWA_NGroup_DLHdrStart,                // [SGI] T to D/L headers on start
      SWA_NGroup_FullHeaders,               // [SGI] T to D/L all headers
      SWA_NGroup_ThreadMgr,                 // [SGI] ptr to thread mgr
      SWA_NGroup_ThreadFT,                  // [SGI] thread filtering type
      SWA_NGroup_ReadFT,                    // [SGI] read filtering type
      SWA_NGroup_AuthFT,                    // [SGI] auth filtering type
      SWA_NGroup_AuthFTName,                // [SGI] auth filtering name
      SWA_NGroup_SelFlt,                    // [SGI] selection filter
      SWA_NGroup_KillFlt,                   // [SGI] kill filter
      SWA_NGroup_GrpDefFlt,                 // [SGI] group def filter
      SWA_NGroup_Sort1,                     // [SGI] save for articlelist
      SWA_NGroup_Dir1,                      // [SGI] save for articlelist
      SWA_NGroup_Sort2,                     // [SGI] save for articlelist
      SWA_NGroup_Dir2,                      // [SGI] save for articlelist
      SWA_NGroup_Sort3,                     // [SGI] save for articlelist
      SWA_NGroup_Dir3,                      // [SGI] save for articlelist
      SWA_NGroup_ReadOnExit,                // [SGI] mark all arts read on exit
      SWA_NGroup_ReadArtCnt,                // [SGI] read arts in group
      SWA_NGroup_TotArtCnt,                 // [SGI] total arts in group
      SWA_NGroup_DispFlt,                   // [SGI] display filter list
      SWA_NGroup_SigFile,                   // [SGI] group specific sig file
      SWA_NGroup_CacheExit,                 // [SGI] what to do w/ cache on exit
      SWA_NGroup_BadGroup,                  // [ G ] T if group not found
      SWA_NGroup_DLQPctDone,                // [ G ] % complete, DL queue
      SWA_NGroup_ArtListCol,                // [ G ] artlist colview
      SWA_NGroup_ActFlt,                    // [SGI] group action filter
      SWA_NGroup_MemCacheNum,               // [SGI] memory cache num
      SWA_NGroup_MemCacheKb,                // [SGI] memory cache kb
      SWA_NGroup_PrefetchNum,               // [SGI] # articles to prefetch
      SWA_NGroup_ArtViewTitle,              // [SGI] group specific art title
      SWA_NGroup_LastFile,                  // [SGI] last save file
      SWA_NGroup_LastDir,                   // [SGI] last save dir
      SWA_NGroup_LastPatt,                  // [SGI] last save patt
      SWA_NGroup_PermFlags,                 // [SGI] stored flags
      SWA_NGroup_LastMsgID,                 // [SGI] msg ID, art from prev sess
      SWA_NGroup_LastRdSrv,                 // [SGI] last read server
      SWA_NGroup_LastRdMsgE,                // [SGI] last read message start
      SWA_NGroup_LastRdMsgC,                // [SGI] last read message count
      SWA_NGroup_NewArtMd,                  // [SGI] new article flag mode
      SWA_NGroup_ArtView,                   // [ G ] artview, if open
      SWA_NGroup_ArtFont,                   // [SGI] article font
      SWA_NGroup_UseArtFont,                // [SGI] else, inherit from NNews
      SWA_NGroup_ArtCol,                    // [SGI] article color
      SWA_NGroup_UseArtCol,                 // [SGI] else, inherit from NNews
      SWA_NGroup_IncrDisp,                  // [SGI] incremental art dsp cnt
      SWA_NGroup_ReadnCache,                // [SGI] cache while reading
      SWA_NGroup_MaxKeep,                   // [SGI] max arts to keep on exit
      SWA_NGroup_Moderated,                 // [SGI] moderated status
   };

   enum {
      SWM_NGroup_CheckStats = SW_FIRSTTAG,  // get count of new articles;
      SWM_NGroup_CheckStats_P,              // parse output from above;
      SWM_NGroup_ReadDesc,                  // get description for group;
      SWM_NGroup_ReadDesc_P,                // parse output from above;
      SWM_NGroup_Fetch,                     // fetch headers from group;
      SWM_NGroup_Fetch_P1,                  // parse1: after article #s;
      SWM_NGroup_Fetch_P2,                  // parse2: header lines;
      SWM_NGroup_Abort,                     // abort pending IO;
      SWM_NGroup_Abbrev,                    // get abbreviated group name;
      SWM_NGroup_FetchAdd,                  // add fetched articles
      SWM_NGroup_OpenReaderWin,             // open reader window
      SWM_NGroup_CloseReaderWin,            // close reader window
      SWM_NGroup_StartRead,                 // start reading the group
      SWM_NGroup_StopRead,                  // stop  reading the group
      SWM_NGroup_OVName,                    // make overview dirname
      SWM_NGroup_SaveOV,                    // save group overview
      SWM_NGroup_LoadOV,                    // load group overview
      SWM_NGroup_MenuCall,                  // menu call
      SWM_NGroup_ArtListClick,              // selection in article list
      SWM_NGroup_ArtListDClick,             // process doubledlick in artlist
      SWM_NGroup_ChgSelFlt,                 // re-run select filters
      SWM_NGroup_ChgUIFlt,                  // change UI selection filters
      SWM_NGroup_ChgThreadFlt,              // change thread filter
      SWM_NGroup_ChgReadFlt,                // change read filter
      SWM_NGroup_ChgAuthFlt,                // change auth filter
      SWM_NGroup_DeleteOVDir,               // delete overview dir
      SWM_NGroup_Mark,                      // mark as (un)read or marked
      SWM_NGroup_ArtHide,                   // hide articles
      SWM_NGroup_CacheSel,                  // (un)cache selected
      SWM_NGroup_DLFetchNext,               // fetch next in DL queue
      SWM_NGroup_TestOVDir,                 // test for OV dir
      SWM_NGroup_CleanCache,                // clean cache articles
      SWM_NGroup_Cmp,                       // compare 2 groups
      SWM_NGroup_GL_Hdrs,                   // batch DL headers
      SWM_NGroup_GL_Hdrs_P,                 // finish up above
      SWM_NGroup_GL_Arts,                   // batch DL arts & cache
      SWM_NGroup_GL_Arts_P1,                // after header d/l
      SWM_NGroup_GL_Arts_P2,                // after article d/l
      SWM_NGroup_NoSaveOV,                  // don`t save OV on dispose
      SWM_NGroup_GL_Mark,                   // batch mark read/unread
      SWM_NGroup_UNUSED,                    // UNUSED
      SWM_NGroup_OnOffLine,                 // fix up gads for on/offline state
      SWM_NGroup_KillSel,                   // kill selected arts
      SWM_NGroup_ChgDspFlt,                 // change display filters
      SWM_NGroup_Browse,                    // browse news
      SWM_NGroup_RunActFlt,                 // run action filters
      SWM_NGroup_MarkRead,                  // mark 1 article as read
      SWM_NGroup_SaveArt,                   // save article text
      SWM_NGroup_ExpireOld,                 // expire old articles
      SWM_NGroup_ExpireOld_P1,              //   expire old articles, 1
      SWM_NGroup_ALQSWin,                   // pop up artlist qsearch win
      SWM_NGroup_ViewMsgID,                 // view a msg ID
      SWM_NGroup_Post,                      // post reply to article
      SWM_NGroup_PostNew,                   // post new
      SWM_NGroup_PostFup,                   // post followup
      SWM_NGroup_OVDir,                     // get OVDir
      SWM_NGroup_DLQAdd,                    // add to DL queue
      SWM_NGroup_ALSearchUIUpd,             // update artlist search UI
      SWM_NGroup_LimitToSel,                // limit listview to selected arts
      SWM_NGroup_OpenFAQList,               // open FAQ list window
      SWM_NGroup_CloseFAQList,              // close FAQ list window
      SWM_NGroup_ChgHLFlt,                  // change highlight filter
      SWM_NGroup_UpdArtList,                // update article list entries
      SWM_NGroup_Fetch_XOV_P1,              // XOVER style fetch
      SWM_NGroup_Fetch_XOV_P2,              // XOVER style fetch
      SWM_NGroup_ToggleWins,                // toggle window mode
      SWM_NGroup_GroupStats,                // display group stats window
      SWM_NGroup_CloseStatWin,              // close stat window
      SWM_NGroup_MPSelect,                  // selected related multiparts
      SWM_NGroup_MPCheck,                   // check for multiparts
      SWM_NGroup_MPOne,                     // display one part
      SWM_NGroup_MPAll,                     // display all parts
      SWM_NGroup_WinCtrl,                   // windows to back/front
      SWM_NGroup_Redisp,                    // redisplay article
      SWM_NGroup_Select,                    // select some articles
      SWM_NGroup_Remove,                    // remove arts or threads
      SWM_NGroup_UnRemove,                  // restore removed arts
   };

   enum {
      SWA_FAQList_LocalDir  = SW_FIRSTTAG,  // [  I] local FAQ storage directory
      SWA_FAQList_Group,                    // [  I] group object for this FAQ
   };

   enum {
      SWM_FAQList_DelFAQ  = SW_FIRSTTAG,    // delete FAQ from local cache
      SWM_FAQList_UpdList,                  // update FAQ list from file
      SWM_FAQList_AbortFAQ,                 // abort FAQ transfer
      SWM_FAQList_FetchFAQList,             // fetch FAQ list
      SWM_FAQList_FetchFAQList_P1,          //   INTERNAL
      SWM_FAQList_FAQListClk,               // process FAQ list clikc
      SWM_FAQList_DelSelFAQ,                // delete selected FAQs
      SWM_FAQList_FetchFAQ,                 // get FAQ from server
      SWM_FAQList_FetchFAQ_P1,              //   INTERNAL
      SWM_FAQList_OnOffLine,                // go on or offline
      SWM_FAQList_Act,                      // multimedia action
   };

   enum {
      SWV_NGroup_Moderated_No = 0,          // not moderated
      SWV_NGroup_Moderated_Yes,             // moderated
   };


   SWM_P(SWP_NGroup_Select,                 // select some articles
         ULONG Type);                       // criteria

   SWM_P(SWP_NGroup_WinCtrl,                // window control
         ULONG Win;                         // window
         ULONG BackFront;                   // to back or front
         ULONG X;                           // position of window
         ULONG Y);

   SWM_P(SWP_FAQList_FAQListClk,            // faq list click
         STRPTR Act);                       // action

   SWM_P(SWP_FAQList_DelFAQ,                // delete FAQ by name
         STRPTR FAQName);                   // FAQ name

   SWM_P(SWP_NGroup_SaveArt,                // save article
         ULONG Parts;                       // parts to save
         STRPTR FSpec;                      // file to save to (NULL=request)
         ULONG Attr);                       // attr (or SWA_NDFilter_T_RAW)


   SWM_P(SWP_FAQList_FetchFAQ,              // fetch FAQ from server
         STRPTR FAQName);                   // FAQ name

   SWM_P(SWP_FAQList_OnOffLine,             // go on or offline
         ULONG State);                      // state

   SWM_P(SWP_NGroup_Redisp,                 // redisplay
         ULONG Type);                       // type (raw or cooked)

   enum {
      SWV_NGroup_Redisp_Raw,                // redisplay raw
      SWV_NGroup_Redisp_Cooked,             // redisplay cooked
   };

   enum {
      SWV_NGroup_WC_AL,                     // artlist window
      SWV_NGroup_WC_AV,                     // artview window
      SWV_NGroup_WC_Back,                   // window to front
      SWV_NGroup_WC_Front,                  // window to front
      SWV_NGroup_WC_Keep = -1,              // no change to pos or size
   };
   
   enum {
      SWV_GC_CheckStats,                    // Group cmd: check stats;
      SWV_GC_ReadDesc,                      // Group cmd: read description;
      SWV_GC_FetchHdr,                      // Group cmd: fetch headers;
      SWV_GC_BatchDL,                       // Group cmd: Batch D
      SWV_GC_GL_Hdrs,                       // Group cmd: fetch hdrs, save OV
      SWV_GC_GL_Arts,                       // Group cmd: Fetch arts, cache
      SWV_GC_ExpireArts,                    // Group cmd: Expire old articles
      SWV_GC_Private1,                      // private
      SWV_GC_MAX,                           // max # of cmds;
   };

   enum {
      SWV_NGroup_NA_Exit = 0,               // reset at exit
      SWV_NGroup_NA_Read,                   // reset when read
      SWV_NGroup_NA_Both,                   // reset at exit & when read
      SWV_NGroup_NA_Never,                  // never reset
      SWV_NGroup_NA_DontUse,                // don`t use them
      SWV_NGroup_NA_MAX,                    // max
   };

   enum {
      SWV_NGroup_DLQ_MakePerm = 0,    // cmds for DLQueue: make permanent
      SWV_NGroup_DLQ_Cache,           // cache on HD
      SWV_NGroup_DLQ_Read,            // read
      SWV_NGroup_DLQ_Action,          // action filters
      SWV_NGroup_DLQ_PreFetch,        // prefetch article
   };

   enum {
      SWV_NGroup_CE_Ask = 0,                // ask what to do
      SWV_NGroup_CE_Keep,                   // keep cached on close
      SWV_NGroup_CE_Del,                    // delete cached on close

      SWV_NGroup_CE_MAX,
   };

#  define SWV_NGroup_GroupDir   "Groups"
#  define SWV_NGroup_FAQDir     "FAQs"
#  define SWV_FAQList_FAQType   "application/X-FAQ"

   enum {
      SWV_NGroup_SortOff = 0,               // no sort
      SWV_NGroup_SortName,
      SWV_NGroup_SortLastRead,
      SWV_NGroup_SortNewCnt,
      SWV_NGroup_SortAvailCnt,
      SWV_NGroup_SortArtCnt,
      SWV_NGroup_SortUnreadCnt,
      SWV_NGroup_SortServer,
   };

   enum {
      SWV_NGroup_Begin    = SW_FIRSTTAG,    // begin command state
      SWV_NGroup_End,                       // end command state
      SWV_NGroup_Abort,                     // command aborted
      SWV_NGroup_NewProgMsg,                // new progress msg
   };

   enum {
      SWT_NGroup_Server   = SW_FIRSTTAG,    // tag: server/list;
      SWT_NGroup_CmdObj,                    // object to signal after cmd;
      SWT_NGroup_CmdMethod,                 // method to send to object w/ state
      SWT_NGroup_CmdData,                   // data to send
      SWT_NGroup_CmdState,                  // returned to CmdObj/CmdMethod
      SWT_NGroup_CmdGroup,                  // ptr to group
      SWT_NGroup_ArtMgr,                    // ptr to article manager

      SWT_NGroup_FetchFirst,                // first article number;
      SWT_NGroup_FetchLast,                 // last article article;
      SWT_NGroup_FetchObj,                  // object to notify;
      SWT_NGroup_FetchMthd,                 // method to call;
      SWT_NGroup_FetchQObj,                 // object to query for articles
      SWT_NGroup_FetchQMthd,                // method to query for articles;
                                            // gets passed message ID.;
      SWT_NGroup_FetchStCnt,                // fetch start count

      SWT_NGroup_OVRoot,                    // root of overview tree
      SWT_NGroup_OVBuff,                    // buffer to hold result
      SWT_NGroup_OVBuffLen,                 // length of buffer

      SWT_NGroup_DelOV_NoPrompt,            // don`t ask about deletion
      SWT_NGroup_DelOV_Force,               // force delete even if flag reset
      SWT_NGroup_DelOV_Win,                 // window for requestor

      SWT_NGroup_SaveOV_Force,              // save 

      SWT_NGroup_ProgMsg,                   // progress message
      SWT_NGroup_ID,                        // Server ID

      SWT_NGroup_GrpWinMode,                // overrides def when opening wins
      SWT_NGroup_Interactive,               // use interactive windows?

      SWT_NGroup_VMID_UpdArtList,           // show in article list?
      SWT_NGroup_VMID_UpdArtView,           // show in article view?
      SWT_NGroup_VMID_MsgID,                // Message-ID
      SWT_NGroup_VMID_NoReq,                // no requestors or user interaction
      SWT_NGroup_VMID_BaseArt,              // go to base article only
   };

   enum {
      SWV_NGroup_DelOV_Error = 0,
      SWV_NGroup_DelOV_OK,
      SWV_NGroup_DelOV_Cancel,
   };

   enum {
      SWF_NGroup_ThreadFlt_All = 0,         // show all articles
      SWF_NGroup_ThreadFlt_Base,            // show base entries
      SWF_NGroup_ThreadFlt_1Thread,         // show entries in 1 thread
      SWF_NGROUP_ThreadFlt_MAX = SWF_NGroup_ThreadFlt_1Thread + 1,

      SWF_NGroup_ThreadFlt_Up,              // up or down in above order
      SWF_NGroup_ThreadFlt_Dn,              // up or down in above order
   };

   enum {
      SWF_NGroup_ReadFlt_All = 0,           // show all articles
      SWF_NGroup_ReadFlt_Read,              // show read articles
      SWF_NGroup_ReadFlt_Unread,            // show unread articles

      SWV_NGroup_ReadFlt_Buttons,           // examine buttons
   };

   enum {
      SWF_NGroup_AuthFlt_All = 0,           // all authors
      SWF_NGroup_AuthFlt_1Auth,             // 1 author
   };

   // -- saved flags ---------------------------------------------------------

   enum {
      SWF_NGroup_WinOpen   = (1<<0),  // win was open at last proj save
      SWF_NGroup_NewBatch  = (1<<1)   // new batch arts since last open
   };

   enum {
      SWF_NGroup_TempGroup = (1<<3),  // group overview not saved!
   };

   enum {
      SWV_NGroup_Browse_Browse = 0,   // browse in current article
      SWV_NGroup_Browse_ArtMov,       // move to next/prev article
      SWV_NGroup_Browse_ThreadMov,    // move to next/prev thread
      SWV_NGroup_Browse_1stAvail,     // show first available article
      SWV_NGroup_Browse_1stUnread     // show first unread article
   };

   SWM_P(SWP_NGroup_UpdArtList,             // update article list
         Object *Art);                      // 1 art to update, else update all

   SWM_P(SWP_NGroup_DLQAdd,                 // add to DL queue
         Object *Art;                       // article to DL
         ULONG  Cmd;                        // cmd (read, etc...)
         ULONG  Last);                      // TRUE if last article

   SWM_P(SWP_NGroup_Remove,                 // remove articles or threads
         ULONG Threads);                    // art/thread flag

   SWM_P(SWP_NGroup_KillSel,                // kill selected
         Object *Type;
         Object *When;
         Object *Exp;
         Object *ExpOk;
         Object *Where);

   SWM_P(SWP_NGroup_OnOffLine,              // toggle on/offline state
         ULONG State);                      // new state

   SWM_P(SWP_NGroup_Browse,                 // browse
         LONG Dir;                          // direction (1 or -1)
         LONG Mode);                        // mode; browse, artmov, threadmov

   SWM_P(SWP_NGroup_GL_Mark,                // batch mark read/unread
         ULONG Read);                       // TRUE for read, else unread

   SWM_P(SWP_NGroup_Fetch,                  // fetched article header info;
         UBYTE  **Data;                     // data lines;
         ULONG  DCount;                     // count of data lines;
         Object *Group);                    // group pointer;

   SWM_P(SWP_NGroup_Abbrev,                 // make abbreviated group name;
         STRPTR Buff;                       // buffer;
         ULONG  BuffLen);                   // buffer length;

   SWM_P(SWP_NGroup_Abort,                  // abort current command;
         ULONG Cmd);                        // what to abort;

   SWM_P(SWP_NGroup_MenuCall,               // menu call
         ULONG Call);                       // which item was chosen

   SWM_P(SWP_NGroup_StopRead,               // stop reading group
         ULONG Save;                        // save state?
         ULONG Force);                      // force exit

   SWM_P(SWP_NGroup_ArtListClick,           // clicked in article list
         ULONG Col);                        // which column?

   SWM_P(SWP_NGroup_ChgThreadFlt,           // change thread filtering
         ULONG Type;                        // all, thread base, or 1 thread
         ULONG Immediate;                   // re-filter now?
         ULONG DoAction);                   // perform thread-enter action?

   SWM_P(SWP_NGroup_ChgReadFlt,             // change read filtering
         ULONG Type;                        // all, thread base, or 1 thread
         ULONG Immediate);                  // re-filter now?

   SWM_P(SWP_NGroup_ChgAuthFlt,             // change read filtering
         ULONG Type;                        // all, or 1 auth
         ULONG Immediate);                  // re-filter now?

   SWM_P(SWP_NGroup_ChgSelFlt,              // change select filters
         ULONG First);                      // 1st new in group list

   SWM_P(SWP_NGroup_ChgUIFlt,               // change select filters
         ULONG First);                      // 1st new in group list

   SWM_P(SWP_NGroup_Mark,                   // mark articles
         ULONG Val;                         // TRUE=marked, FALSE=unmarked
         ULONG Mode;                        // 1=read/unread, 0=mk/unmk, 2=resetnew
         ULONG Threads;                     // TRUE for thread mode
         ULONG All);                        // TRUE for all in group

   SWM_P(SWP_NGroup_CacheSel,               // (un)cache selected
         ULONG Cache;                       // TRUE to cache, else un-cache
         ULONG Perm;                        // TRUE for permanent
         ULONG Threads;                     // TRUE for thread mode
         ULONG All);                        // TRUE for all in group

   SWM_P(SWP_NGroup_MarkRead,               // mark article as read
         Object *Art);                      // article ptr

   SWM_P(SWP_NGroup_Post,                   // post article
         Object *Art);                      // article to followup to, or NULL

   enum {
      SWV_NGroup_MaxNameLen =  180,         // max group name length;
      SWV_NGroup_QS_MinFree = (1024*1024*4),// min memory free for qsearch
   };

#  define GF_Data(msg,p)       ((msg)->Data[p])
#  define GF_DataCount(msg)    ((msg)->DCount)
#  define GF_Group(msg)        ((msg)->Group)

#  undef SW_FIRSTTAG   

# endif
