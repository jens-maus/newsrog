# ifndef C_NNEWS_INCLUDE
#  define C_NNEWS_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-AppBase.h"
#  include "Libraries/C-Register.h"
#  include "Libraries/C-FileTypes.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 12100)

   enum {
      SWA_NNews_DONTREUSE = SW_FIRSTTAG, // [SGI] DON`T RESUSE THIS!!!
      SWA_NNews_GroupList,               // [ G ] group list
      SWA_NNews_ServList,                // [ G ] server list
      SWA_NNews_TCPList,                 // [SGI] TCP list
      SWA_NNews_ArtMgr,                  // [ G ] article mgr
      SWA_NNews_MemPool,                 // [SGI] memory pool
      SWA_NNews_UserName,                // [SGI] user name
      SWA_NNews_UserOrg,                 // [SGI] organization
      SWA_NNews_CacheDir,                // [SGI] cache directory
      SWA_NNews_SigFile,                 // [SGI] .sig filename
      SWA_NNews_GlobSel,                 // [SGI] global select filter
      SWA_NNews_GlobKill,                // [SGI] global kill filter
      SWA_NNews_GlobDsp,                 // [SGI] global display filter
      SWA_NNews_GlobAct,                 // [SGI] global action list
      SWA_NNews_Editor,                  // [SGI] editor
      SWA_NNews_ArtListCol,              // [SGI] article list display
      SWA_NNews_HdrCacheSize,            // [SGI] header cache size
      SWA_NNews_StartNWMode,             // [SGI] startup NW mode
      SWA_NNews_VarList,                 // [SGI] variable list
      SWA_NNews_ArtTpl,                  // [SGI] article object template
      SWA_NNews_VFont,                   // [SGI] default viewer font
      SWA_NNews_ArtViewTitle,            // [SGI] article view title text
      SWA_NNews_GrpListOpen,             // [SGI] open mode for group list
      SWA_NNews_TCPListOpen,             // [SGI] open mode for TCP list
      SWA_NNews_SrvListOpen,             // [SGI] open mode for server list
      SWA_NNews_PstListOpen,             // [SGI] open mode for post queue
      SWA_NNews_CfgWinOpen,              // [SGI] open mode for config window
      SWA_NNews_GrpWinOpen,              // [SGI] open mode for each group
      SWA_NNews_GrpListLast,             // [SGI] last open for group list
      SWA_NNews_TCPListLast,             // [SGI] last open for TCP list
      SWA_NNews_SrvListLast,             // [SGI] last open for server list
      SWA_NNews_PstListLast,             // [SGI] last open for post queue
      SWA_NNews_CfgWinLast,              // [SGI] last open for config window
      SWA_NNews_ReplyLine,               // [SGI] reply line format
      SWA_NNews_PostList,                // [ G ] post queue
      SWA_NNews_UserEmail,               // [SGI] user email addrc-nn
      SWA_NNews_FUpMode,                 // [SGI] followup mode
      SWA_NNews_QuoteMode,               // [SGI] quote mode
      SWA_NNews_QuoteStr,                // [SGI] quote string
      SWA_NNews_QuoteMaxLen,             // [SGI] max quote length
      SWA_NNews_MMList,                  // [SGI] multimedia types list
      SWA_NNews_MMListOpen,              // [SGI] open mode for mm types
      SWA_NNews_MMListLast,              // [SGI] last open for mm types
      SWA_NNews_FAList,                  // [SGI] file action list
      SWA_NNews_SoftScroll,              // [SGI] softscroll
      SWA_NNews_PostWrap,                // [SGI] post wrap len
      SWA_NNews_GenMsgID,                // [SGI] whether to generate MSG IDs
      SWA_NNews_ToolBarStyle,            // [SGI] toolbar style
      SWA_NNews_ArtListWght,             // [SGI] art list weight in grp windows
      SWA_NNews_HdrList,                 // [ G ] header list
      SWA_NNews_DTImages,                // [ G ] DTObj image cache
      SWA_NNews_VColor,                  // [SGI] default viewer font color
      SWA_NNews_FAQServer,               // [SGI] faq server
      SWA_NNews_FAQDir,                  // [SGI] directory to FAQ files
      SWA_NNews_GlobHL,                  // [SGI] global highlight filter
      SWA_NNews_AutoClip,                // [SGI] autoclip support
      SWA_NNews_HdrDLType,               // [SGI] how to download headers
      SWA_NNews_PostWrapType,            // [SGI] post wrap type
      SWA_NNews_UseSigSep,               // [SGI] use signature separator line
      SWA_NNews_HdrInc,                  // [SGI] header increment
      SWA_NNews_SimulCmds,               // [SGI] max simultaneous cmds
      SWA_NNews_GrpWinMode,              // [SGI] group window mode
      SWA_NNews_AutoStartEd,             // [SGI] auto-launch editor?
      SWA_NNews_ButtonPath,              // [SGI] button path
      SWA_NNews_SvPostList,              // [SGI] saved post queue
      SWA_NNews_MaxSavedPosts,           // [SGI] max saved posts
      SWA_NNews_TagLineFile,             // [SGI] tagline file
      SWA_NNews_TagLineSep,              // [SGI] tagline separator
      SWA_NNews_EnterThread,             // [SGI] enter thread mode
      SWA_NNews_PrinterDev,              // [SGI] printer device
      SWA_NNews_FFTBLoc,                 // [SGI] Filter Toolbar location
      SWA_NNews_FetchPolicy,             // [SGI] fetch policy
      SWA_NNews_Charset,                 // [SGI] default charset
      SWA_NNews_FFTBLab,                 // [SGI] filter toolbar labels
      SWA_NNews_AutoSave,                // [SGI] auto save
      SWA_NNews_Enc_Body,                // [SGI] post body encoding
      SWA_NNews_Enc_Hdrs,                // [SGI] post hdrs encoding
      SWA_NNews_StartArt,                // [SGI] start article
      SWA_NNews_BrowseArts,              // [SGI] which articles to browse
   };

   enum {
      SWA_NNews_Editor_UNUSED = SW_FIRSTTAG, // [  I] wrap type
      SWA_NNews_Editor_InitStrAddr,      // [  I] >>ADDR<< of init contents
      SWA_NNews_Editor_CursorDisp,       // [  I] T for cursor pos display
      SWA_NNews_Editor_XTraObj1,         // [  I] extra obj displayed below curs
      SWA_NNews_Editor_FSpec,            // [  I] save to, & notify file
      SWA_NNews_Editor_CmdObj,           // [  I] command object
      SWA_NNews_Editor_AskClob,          // [  I] ask before clobbering
      SWA_NNews_Editor_ReadOnly,         // [  I] readonly mode
      SWA_NNews_Editor_NoDelayInit,      // [  I] no delay initialization
    };

   enum {
      SWV_NNews_ISO_8859_1 = 0,          // various ISO defined charsets
      SWV_NNews_ISO_8859_2,
      SWV_NNews_ISO_8859_3,
      SWV_NNews_ISO_8859_4,
      SWV_NNews_ISO_8859_5,
      SWV_NNews_ISO_8859_6,
      SWV_NNews_ISO_8859_7,
      SWV_NNews_ISO_8859_8,
      SWV_NNews_ISO_8859_9,
      SWV_NNews_ISO_8859_10,
   };
      
   enum {
      SWV_NNews_Enc_QP = 0,              // quoted-printable
      SWV_NNews_Enc_8bit,                // 8-bit
      SWV_NNews_Enc_7bit,                // 7-bit
   };
      
   enum {
      SWV_NNews_AS_Never = 0,            // fetch count defined in group
      SWV_NNews_AS_PostGrpClose,         // after group close
   };
      
   enum {
      SWV_NNews_FP_Limit = 0,            // fetch count defined in group
      SWV_NNews_FP_All,                  // fetch all articles
      SWV_NNews_FP_Prompt,               // prompt for what to do
   };
      
   enum {
      SWV_NNews_FFTB_Right = 0,          // right side
      SWV_NNews_FFTB_Left,               // left side
      SWV_NNews_FFTB_Top,                // top
      SWV_NNews_FFTB_Bottom,             // bottom
      SWV_NNews_FFTB_ArtView,            // artview toolbar
   };
      
   enum {
      SWV_NNews_ET_NoAction = 0,         // no action on entering thread
      SWV_NNews_ET_FirstAvail,           // read first available
      SWV_NNews_ET_FirstUnread,          // read first unread
   };

   enum {
      SWV_NNews_GW_Single = 0,           // single list/view window
      SWV_NNews_GW_Separate,             // separate list/view windows
   };

   enum {
      SWV_NNews_HD_Bytes = 0,            // download Bytes: header + HEAD
      SWV_NNews_HD_Normal,               // D/L headers using HEAD
      SWV_NNews_HD_XOver,                // D/L headers using XOVER
   };

   enum {
      SWV_NNews_PR_Post = 0,             // wrap when post
      SWV_NNews_PR_Type,                 // wrap as typed, not when posted
      SWV_NNews_PR_Never,                // never
   };

   enum {
      SWV_NNews_TB_HorizLab = 0,         // toolbar style
      SWV_NNews_TB_HorizNoLab,
      SWV_NNews_TB_VertLab,
      SWV_NNews_TB_VertNoLab,
   };

   enum {
      SWV_NNews_SA_PrevArt = 0,          // where to put cursor when
      SWV_NNews_SA_PrevThread,           // group is opened
      SWV_NNews_SA_First,
      SWV_NNews_SA_Last,
   };

   enum {
      SWV_NNews_BA_All = 0,              // which arts to browse: all
      SWV_NNews_BA_Unread,               // unread
      SWV_NNews_BA_New,                  // new
      SWV_NNews_BA_UnreadNew,            // unread & new
   };

   enum {
      SWM_NNews_OpenWin = SW_FIRSTTAG,   // open a sub window
      SWM_NNews_CloseWin,                // close a sub window
      SWM_NNews_Quit,                    // quit
      SWM_NNews_MenuStrip,               // generate menu strip
      SWM_NNews_NewProj,                 // post-newproj method
      SWM_NNews_MakeToolbar,             // make horiz or vert toolbar
      SWM_NNews_EditorClass,             // return editor class
      SWM_NNews_CharSetName,             // get charset name as string
   };

   enum {
      SWM_NNews_Editor_UpdCursPos = SW_FIRSTTAG, // update cursor position
      SWM_NNews_Editor_EdIns,            // insert new text
      SWM_NNews_Editor_EdSave,           // save text
      SWM_NNews_Editor_Create,           // make editor
      SWM_NNews_Editor_Destroy,          // dispose editor
      SWM_NNews_Editor_SaveEditData,     // save edit data to file
      SWM_NNews_Editor_AppMessage,       // app message
      SWM_NNews_Editor_InsFile,          // insert file contents
      SWM_NNews_Editor_UpdateEF,         // edit file has changed
      SWM_NNews_Editor_SetupDone,        // callback when editor is created
      SWM_NNews_Editor_NewContents,      // callback for new contents
      SWM_NNews_Editor_InitNotify,       // init notify on file
      SWM_NNews_Editor_HaltNotify,       // halt notify on file
      SWM_NNews_Editor_AskClob,          // ask if OK to clobber.
   };

   enum {
      SWA_NNews_FUp_Body = 0,            // only quote body
      SWA_NNews_FUp_All,                 // quote entire article
   };

   enum {
      SWV_NNews_StartNW_On = 0,          // startup networking mode: On
      SWV_NNews_StartNW_Off,             // .. off
   };

   enum {
      SWV_NNews_QM_Off = 0,              // quote modes: no quoting
      SWV_NNews_QM_PrefixQB,             // line prefixes/quote blank
      SWV_NNews_QM_PrefixSB,             // line prefixes/skip blank
   };

   enum {
      SWV_NNews_WinClosed = 0,           // close window on proj load
      SWV_NNews_WinLastSaved,            // use last win open/closed state
      SWV_NNews_WinOpen,                 // open window on proj load
   };

   SWM_P(SWP_NNews_Editor_InitNotify,    // init notify
         STRPTR FSpec);                  // file to save to, or NULL to use def

   SWM_P(SWP_NNews_Editor_SaveEditData,  // save texteditor data to file
         STRPTR FSpec);                  // file to save to

   SWM_P(SWP_NNews_Editor_NewContents,   // new contents
         ULONG Lines;                    // # of lines
         ULONG Bytes);                   // # of bytes

   SWM_P(SWP_NNews_Editor_EdIns,         // insert file data (interactive)
         STRPTR QPrefix);                // quote prefix

   SWM_P(SWP_NNews_Editor_InsFile,       // insert file data
         STRPTR FSpec;                   // file to insert
         STRPTR QPrefix;                 // quote prefix
         ULONG  Clear);                  // TRUE to clear first

   SWM_P(SWP_NNews_Editor_AppMessage,    // app message
         struct AppMessage *AMsg);

   SWM_P(SWP_NNews_OpenWin,     LONG Win);
   SWM_P(SWP_NNews_CloseWin,    LONG Win; LONG Shutdown);
   SWM_P(SWP_NNews_MenuCall,    ULONG Event);
   SWM_P(SWP_NNews_Quit,        ULONG Force);
   SWM_P(SWP_NNews_MakeToolbar, ULONG Style);

#  define NN_GetObj(i,a)    SWGetObj((i)->SWA_AppBase_TopObj, (a))
#  define NN_GetStr(i,a)    SWGetStr((i)->SWA_AppBase_TopObj, (a))
#  define NN_GetUL(i,a)     SWGetUL((i)->SWA_AppBase_TopObj, (a))
 
#  define NN_GroupList(i)   NN_GetObj(i, SWA_NNews_GroupList)
#  define NN_ServList(i)    NN_GetObj(i, SWA_NNews_ServList)
#  define NN_PostList(i)    NN_GetObj(i, SWA_NNews_PostList)
#  define NN_SvPostList(i)  NN_GetObj(i, SWA_NNews_SvPostList)
#  define NN_TCPList(i)     NN_GetObj(i, SWA_NNews_TCPList)
#  define NN_MMTypes(i)     NN_GetObj(i, SWA_NNews_MMList)
#  define NN_HdrList(i)     NN_GetObj(i, SWA_NNews_HdrList)
#  define NN_DTImages(i)    NN_GetObj(i, SWA_NNews_DTImages)
#  define NN_MemPool(i)     (APTR *)SWGetUL ((i)->SWA_AppBase_TopObj, SWA_NNews_MemPool)
#  define NN_ArtMgr(i)      NN_GetObj(i, SWA_NNews_ArtMgr)
#  define NN_App(i)         NN_GetObj(i, SWA_AppBase_App)
#  define NN_RegObj(i)      NN_GetObj(i, SWA_AppBase_Reg)
#  define NN_RegVer(i)      SWGetUL(NN_RegObj(idata), SWA_RegLibVer)
#  define NN_CacheDir(i)    NN_GetStr(i, SWA_NNews_CacheDir)
#  define NN_UserName(i)    NN_GetStr(i, SWA_NNews_UserName)
#  define NN_UserEmail(i)   NN_GetStr(i, SWA_NNews_UserEmail)
#  define NN_ReplyLine(i)   NN_GetStr(i, SWA_NNews_ReplyLine)
#  define NN_SigFile(i)     NN_GetStr(i, SWA_NNews_SigFile)
#  define NN_UserOrg(i)     NN_GetStr(i, SWA_NNews_UserOrg)
#  define NN_Editor(i)      NN_GetStr(i, SWA_NNews_Editor)
#  define NN_QuoteStr(i)    NN_GetStr(i, SWA_NNews_QuoteStr)
#  define NN_QuoteMode(i)   NN_GetUL(i,  SWA_NNews_QuoteMode)
#  define NN_RegMsg(i,m,w)  DoMethod((i)->SWA_AppBase_TopObj, SWM_AppBase_UnregMsg, \
                                     (m),(w))
#  define NN_IsPirate(i)    (SWGetUL(NN_RegObj(i), SWA_RegIsPirated) == SWV_IsPirate)
#  define NN_VarList(i)     NN_GetObj(i, SWA_NNews_VarList)
#  define NN_ArtTpl(i)      NN_GetObj(i, SWA_NNews_ArtTpl)
#  define NN_VFont(i)       NN_GetStr(i, SWA_NNews_VFont)
#  define NN_VColor(i)      NN_GetStr(i, SWA_NNews_VColor)
#  define NN_GrpWinOpen(i)  NN_GetUL(i,  SWA_NNews_GrpWinOpen)
#  define NN_MMTypes(i)     NN_GetObj(i, SWA_NNews_MMList)
#  define NN_MMActs(i)      NN_GetObj(i, SWA_NNews_FAList)
#  define NN_GlobKill(i)    NN_GetObj(i, SWA_NNews_GlobKill)
#  define NN_SoftScroll(i)  NN_GetUL(i,  SWA_NNews_SoftScroll)
#  define NN_UseSS(i)       NN_GetUL(i,  SWA_NNews_UseSigSep)
#  define NN_PostWrap(i)    NN_GetUL(i,  SWA_NNews_PostWrap)
#  define NN_PostWrapType(i)NN_GetUL(i,  SWA_NNews_PostWrapType)
#  define NN_GenMsgID(i)    NN_GetUL(i,  SWA_NNews_GenMsgID)
#  define NN_ArtListWght(i) NN_GetUL(i,  SWA_NNews_ArtListWght)
#  define NN_FAQServer(i)   NN_GetUL(i,  SWA_NNews_FAQServer)
#  define NN_FAQDir(i)      NN_GetUL(i,  SWA_NNews_FAQDir)
#  define NN_AutoClip(i)    NN_GetUL(i,  SWA_NNews_AutoClip)
#  define NN_HdrDL(i)       NN_GetUL(i,  SWA_NNews_HdrDLType)
#  define NN_HdrInc(i)      NN_GetUL(i,  SWA_NNews_HdrInc)
#  define NN_SimulCmds(i)   NN_GetUL(i,  SWA_NNews_SimulCmds)
#  define NN_GrpWinMode(i)  NN_GetUL(i,  SWA_NNews_GrpWinMode)
#  define NN_AutoStartEd(i) NN_GetUL(i,  SWA_NNews_AutoStartEd)
#  define NN_ButtonPath(i)  NN_GetStr(i, SWA_NNews_ButtonPath)
#  define NN_MaxSavedPosts(i) NN_GetUL(i,  SWA_NNews_MaxSavedPosts)
#  define NN_TagLineFile(i) NN_GetStr(i, SWA_NNews_TagLineFile)
#  define NN_TagLineSep(i)  NN_GetStr(i, SWA_NNews_TagLineSep)
#  define NN_EnterThread(i) NN_GetUL(i,  SWA_NNews_EnterThread)
#  define NN_PrinterDev(i)  NN_GetStr(i,  SWA_NNews_PrinterDev)
#  define NN_FFTBLoc(i)     NN_GetUL(i,  SWA_NNews_FFTBLoc)
#  define NN_FFTBLab(i)     NN_GetUL(i,  SWA_NNews_FFTBLab)
#  define NN_FetchPolicy(i) NN_GetUL(i,  SWA_NNews_FetchPolicy)
#  define NN_AutoSave(i)    NN_GetUL(i,  SWA_NNews_AutoSave)
#  define NN_EncHdrs(i)     NN_GetUL(i,  SWA_NNews_Enc_Hdrs)
#  define NN_EncBody(i)     NN_GetUL(i,  SWA_NNews_Enc_Body)
#  define NN_StartArt(i)    NN_GetUL(i,  SWA_NNews_StartArt)
#  define NN_BrowseArts(i)  NN_GetUL(i,  SWA_NNews_BrowseArts)

#  define NN_Class(c) \
       NewObject((struct IClass *)DoMethod(idata->SWA_AppBase_TopObj, \
         SWM_NNews_##c##Class), NULL

#  define NN_EditorObject NN_Class(Editor)

#  define VL_Alloc(o, i, Str) \
   (STRPTR)DoMethod(NN_VarList(i), SWM_VarList_ReplaceValues, Str, o)

#  define VL_Free(o, i, Str) \
   DoMethod(NN_VarList(i), SWM_VarList_FreeString, Str, o)

#  define VL_AllocTop(Top, o, Str) \
   (STRPTR)DoMethod(SWGetObj(Top, SWA_NNews_VarList), SWM_VarList_ReplaceValues, Str, o)

#  define VL_FreeTop(Top, o, Str) \
   (STRPTR)DoMethod(SWGetObj(Top, SWA_NNews_VarList), SWM_VarList_FreeString, Str, o)

#  define VL_AllocApp(app, o, Str) \
   VL_AllocAppCtrl((Object *)muiUserData(app), o, Str)

#  define VL_FreeApp(app, o, Str) \
   VL_FreeAppCtrl((Object *)muiUserData(app), o, Str)

#  define SWP_NNews_Attrs    SWP_AppBase_Attrs
#  define SWP_NNews_AttrDefs SWP_AppBase_AttrDefs

#  define NN_MMBase "X-NewsRog"

#  define BF_Dir       "PROGDIR:Images/"
#  define BF_Sort      BF_Dir "IFF.Sort"
#  define BF_1Auth     BF_Dir "IFF.1Author"
#  define BF_BrowseF   BF_Dir "IFF.BrowseF"
#  define BF_BrowseR   BF_Dir "IFF.BrowseR"
#  define BF_Read      BF_Dir "IFF.Read"
#  define BF_Remove    BF_Dir "IFF.Remove"
#  define BF_Unread    BF_Dir "IFF.Unread"
#  define BF_View      BF_Dir "IFF.View"
#  define BF_HLight    BF_Dir "IFF.HLight"
#  define BF_FAQ       BF_Dir "IFF.FAQ"
#  define BF_1Art      BF_Dir "IFF.1Art"
#  define BF_2Art      BF_Dir "IFF.2Art"
#  define BF_3Art      BF_Dir "IFF.3Art"
#  define BF_Kill      BF_Dir "IFF.Kill"
#  define BF_Clip      BF_Dir "IFF.Clip"
#  define BF_Search    BF_Dir "IFF.Search"
#  define BF_Action    BF_Dir "IFF.Action"
#  define BF_PNew      BF_Dir "IFF.PostNew"
#  define BF_PFup      BF_Dir "IFF.PostFup"
#  define BF_AFwd      BF_Dir "IFF.ArtFwd"
#  define BF_ABack     BF_Dir "IFF.ArtBack"
#  define BF_TFwd      BF_Dir "IFF.ThreadFwd"
#  define BF_TBack     BF_Dir "IFF.ThreadBack"

#  define APPNAME     "NewsRog"
#  define APPNAMECAP  "NEWSROG"
#  define APPVERSTR   "1.8b"

   enum {
      SWV_NNews_MaxOpenGroups = 24,
   };

#  undef SW_FIRSTTAG

# endif
