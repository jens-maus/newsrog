# include "C-NNews-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NNews

// -- main menu --------------------------------------------------------------

static struct NewMenu MainMenu[] =
{
   SW_AppBase_Menu_Proj_NoReg,
  { NM_ITEM,  (STRPTR)SWS_M_QuitNoSv,     0 ,0,0,(APTR)SWS_M_QuitNoSv   },
   
   SW_AppBase_Menu_Edit,

  { NM_TITLE, (STRPTR)SWS_M_Network,      0 ,0,0,(APTR)0                },
  { NM_ITEM,  (STRPTR)SWS_M_Online,       0 ,0,0,(APTR)SWS_M_Online     },
  { NM_ITEM,  (STRPTR)SWS_M_Offline,      0 ,0,0,(APTR)SWS_M_Offline    },
   
  { NM_TITLE, (STRPTR)SWS_M_OpenWin,      0 ,0,0,(APTR)0                },
  { NM_ITEM,  (STRPTR)SWS_M_Launcher,     0 ,0,0,(APTR)SWS_M_Launcher   },
  { NM_ITEM,  (STRPTR)SWS_M_GroupWin,     0 ,0,0,(APTR)SWS_M_GroupWin   },
  { NM_ITEM,  (STRPTR)SWS_M_ServWin,      0 ,0,0,(APTR)SWS_M_ServWin    },
  { NM_ITEM,  (STRPTR)SWS_M_TCPWin,       0 ,0,0,(APTR)SWS_M_TCPWin     },
  { NM_ITEM,  (STRPTR)SWS_M_PostWin,      0 ,0,0,(APTR)SWS_M_PostWin    },
  { NM_ITEM,  (STRPTR)SWS_M_MMWin,        0 ,0,0,(APTR)SWS_M_MMWin      },
  { NM_ITEM,  (STRPTR)SWS_M_ConfigWin,    0 ,0,0,(APTR)SWS_M_ConfigWin  },

   SW_AppBase_Menu_Settings,

  { NM_END,   (STRPTR)NULL,               0, 0,0, (APTR)0               },
};


// -- attribute map ----------------------------------------------------------

# define UI (SWAM_UI|SWAM_Notify)
# define AVT "\\33bFrom:\\33n ${ArtUserName}  \\33bSubj\\33n: ${ArtSubj}"
# define BG SWAM_NewCG2
# define HG SWAM_NewHG
# define EG SWAM_EndG
# define DefFaqD "/pub/usenet-by-group/${GroupName}"

static STRPTR ModeLab[4];
static STRPTR Tabs[6];
static STRPTR WinModes[4];
static STRPTR WinModeG[3];
static STRPTR QModeLab[4];
static STRPTR TBModeLab[5];
static STRPTR HDLabs[5];
static STRPTR PRLabs[5];
static STRPTR GWMLabs[4];
static STRPTR ETLabs[5];
static STRPTR FFTBLabs[6];
static STRPTR FPLabs[4];
static STRPTR CSLabs[12];
static STRPTR ASLabs[4];
static STRPTR EBLabs[4];
static STRPTR EHLabs[3];
static STRPTR SALabs[6];
static STRPTR BALabs[6];

# define DefRepl "\"${ArtUserAbbrev}\" == \"${ArtUserName}\" writes:\\n"

SWAttrMapT SW_ATTRMAP[] = {
   // -- keep 1st 5 in this order, due to some dispose bug somewhere!;
   SWAM_OB (SWA_NNews_TCPList,   SWAM_NoLab,     SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_ServList,  SWAM_NoLab,     SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_GroupList, SWAM_NoLab,     SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_PostList,  SWAM_NoLab,     SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_SvPostList,SWAM_NoLab,     SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_MMList,    SWAM_NoLab,     SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_FAList,    SWAM_NoLab,     SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_ArtMgr,    SWAM_NoLab,     SWAM_SGIN),
   SWAM_PT (SWA_NNews_MemPool,   SWAM_NoLab,     SWAM_SGIN, NULL),
   SWAM_PT (SWA_NNews_DTImages,  SWAM_NoLab,     SWAM_SGIN, NULL),
   SWAM_PT (SWA_NNews_ButtonPath,SWAM_NoLab,     SWAM_SGIN, NULL),

   SWAM_UICTL(SWAM_NewRG,  SWS_L_POTab0, 5, Tabs),

   SWAM_SA (SWA_NNews_UserName,  SWS_L_UserName, UI|BG, NULL, 80),
   SWAM_SA (SWA_NNews_UserEmail, SWS_L_UserEmail,UI,    NULL, 80),
   SWAM_SA (SWA_NNews_UserOrg,   SWS_L_UserOrg,  UI, "Amiga Site",  48),
   SWAM_SA (SWA_NNews_ReplyLine, SWS_L_ReplyLine,UI|EG,  DefRepl,    256),
   SWAM_SA (SWA_NNews_CacheDir,  SWS_L_CacheDir, UI|BG|SWAM_PopDir,
            "PROGDIR:Cache",FMSIZE),
   SWAM_IB (SWA_NNews_HdrCacheSize,SWS_L_HdrCS,  UI|SWAM_KbSld,2000,50,50*1000),
   SWAM_CY (SWA_NNews_HdrDLType, SWS_L_HDType, UI,
            SWV_NNews_HD_Bytes, 3, HDLabs),
   SWAM_IB (SWA_NNews_HdrInc,    SWS_L_HDInc,    UI, 100, 25, 10000),
   SWAM_IB (SWA_NNews_SimulCmds, SWS_L_SimCmd,   UI, 8, 1, 20),
   SWAM_CY (SWA_NNews_EnterThread, SWS_L_ETType,  UI,
            SWV_NNews_ET_NoAction, 3, ETLabs),
   SWAM_CY (SWA_NNews_FetchPolicy,SWS_L_FetchPol, UI, SWV_NNews_FP_Limit,
           3, FPLabs),
   SWAM_SA (SWA_NNews_FAQDir,    SWS_L_FAQD,     UI, DefFaqD, FMSIZE-1),
   SWAM_SA (SWA_NNews_FAQServer, SWS_L_FAQS,     UI|EG,"rtfm.mit.edu", 256),

   SWAM_SA (SWA_NNews_SigFile,   SWS_L_SigFile,  UI|BG|SWAM_PopFile,
            "PROGDIR:.sig",FMSIZE),
   SWAM_SA (SWA_NNews_Editor,    SWS_L_Editor,   UI|SWAM_PopFile,
            "c:ed ${File}", FMSIZE),
   SWAM_CY (SWA_NNews_StartNWMode, SWS_L_StartNW, UI,
            SWV_NNews_StartNW_On, 2, ModeLab),
   SWAM_PF (SWA_NNews_VFont,     SWS_L_VFont,  SWAM_UI, "helvetica/13",
            MAXFONTNAME),
   SWAM_PA (SWA_NNews_VColor,    SWS_L_VCol,   SWA_NNews_Pens,
            PEN_VCol, MPEN_TEXT, SWAM_UI),
   SWAM_BO (SWA_NNews_AutoClip,  SWS_L_AutoC,  UI, FALSE),
   SWAM_SA (SWA_NNews_ArtViewTitle, SWS_L_AVTitle, UI, AVT, 1024),
   SWAM_IB (SWA_NNews_SoftScroll,   SWS_L_SoftS,   UI, 0, 0, 10),
   SWAM_CY (SWA_NNews_AutoSave,     SWS_L_AutoSave, UI | SWAM_CyNoLab,
            SWV_NNews_AS_Never, 2, ASLabs),
   SWAM_SA (SWA_NNews_PrinterDev,   SWS_L_PrtDev,   UI, "PRT:", 64),
   SWAM_CY (SWA_NNews_StartArt,     SWS_L_StArt,    UI,
            SWV_NNews_SA_PrevThread, 4, SALabs),
   SWAM_CY (SWA_NNews_BrowseArts,   SWS_L_BrArts,   UI|EG,
            SWV_NNews_BA_All,        4, BALabs),

   SWAM_CY (SWA_NNews_GrpListOpen, SWS_L_GL_O, UI|BG, SWV_NNews_WinLastSaved,
            3, WinModes),
   SWAM_CY (SWA_NNews_SrvListOpen, SWS_L_SL_O, UI, SWV_NNews_WinLastSaved,
            3, WinModes),
   SWAM_CY (SWA_NNews_TCPListOpen, SWS_L_TCPL_O, UI, SWV_NNews_WinLastSaved,
            3, WinModes),
   SWAM_CY (SWA_NNews_PstListOpen, SWS_L_PstL_O, UI, SWV_NNews_WinLastSaved,
            3, WinModes),
   SWAM_CY (SWA_NNews_MMListOpen,  SWS_L_MM_O,  UI, SWV_NNews_WinLastSaved,
            3, WinModes),
   SWAM_CY (SWA_NNews_CfgWinOpen,  SWS_L_Cfg_O, UI, SWV_NNews_WinLastSaved,
            3, WinModes),
   SWAM_CY (SWA_NNews_GrpWinOpen,  SWS_L_GW_O, UI, SWV_NNews_WinClosed,
            2, WinModeG),
   SWAM_CY (SWA_NNews_GrpWinMode,  SWS_L_GWM, UI, SWV_NNews_GW_Single,
            2, GWMLabs),
   SWAM_CY (SWA_NNews_ToolBarStyle, SWS_L_TBS, UI, SWV_NNews_TB_HorizLab,
            4, TBModeLab),
   SWAM_TX (SWS_L_FFTBLoc, NULL),
   SWAM_CY (SWA_NNews_FFTBLoc,     SWS_L_FFTBLoc,  UI|HG|SWAM_CyNoLab,
            SWV_NNews_FFTB_Right, 5, FFTBLabs),
   SWAM_BO (SWA_NNews_FFTBLab,     SWS_L_FFTBLab,  UI|EG, TRUE),
   SWAM_IB (SWA_NNews_ArtListWght,  SWS_L_ALWght,  UI|EG, 30, 10, 90),

   SWAM_CY (SWA_NNews_QuoteMode,   SWS_L_QuoteMode,  UI|BG,
            SWV_NNews_QM_PrefixQB, 3, QModeLab),
   SWAM_SA (SWA_NNews_QuoteStr,    SWS_L_QuoteStr,  UI, "${ArtUserAbbrev}> ", 256),
   SWAM_CY (SWA_NNews_Charset,     SWS_L_Charset,   UI, SWV_NNews_ISO_8859_1,
            10, CSLabs),
   SWAM_CY (SWA_NNews_PostWrapType,SWS_L_PR_O,   UI, SWV_NNews_PR_Post,
            3, PRLabs),
   SWAM_IB (SWA_NNews_PostWrap,    SWS_L_PWrap,  UI, 70, 32, 200),
   SWAM_IB (SWA_NNews_MaxSavedPosts,SWS_L_MaxSvPst, UI, 10, 0, 100),
   SWAM_SA (SWA_NNews_TagLineFile,  SWS_L_TagFile, UI|SWAM_PopFile, "", FMSIZE),
   SWAM_SA (SWA_NNews_TagLineSep,   SWS_L_TagSep, UI, "%%", 32),
   SWAM_TX (SWS_L_EncBody, NULL),
   SWAM_CY (SWA_NNews_Enc_Body,    SWS_L_EncBody, UI|SWAM_CyNoLab|HG,
            SWV_NNews_Enc_QP, 3, EBLabs),
   SWAM_CY (SWA_NNews_Enc_Hdrs,    SWS_L_EncHdrs, UI|EG,  SWV_NNews_Enc_QP,
            2, EHLabs),
   SWAM_TX (SWS_L_UseSS, NULL),
   SWAM_BO (SWA_NNews_UseSigSep,   SWAM_NoLab,  UI|HG, TRUE),
   SWAM_BO (SWA_NNews_GenMsgID,    SWS_L_GenMID, UI,    TRUE),
   SWAM_BO (SWA_NNews_AutoStartEd, SWS_L_AEd,    UI|EG, FALSE),

   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   
   SWAM_OB (SWA_NNews_GlobSel,     SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_GlobHL,      SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_GlobKill,    SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_GlobDsp,     SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_GlobAct,     SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_ArtListCol,  SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_VarList,     SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_HdrList,     SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB (SWA_NNews_ArtTpl,      SWAM_NoLab, SWAM_SGIN),

   SWAM_IN (SWA_NNews_GrpListLast, SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_IN (SWA_NNews_TCPListLast, SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_IN (SWA_NNews_SrvListLast, SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_IN (SWA_NNews_PstListLast, SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_IN (SWA_NNews_MMListLast,  SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_IN (SWA_NNews_CfgWinLast,  SWAM_NoLab, SWAM_SGI, FALSE),

   SWAM_BO (SWA_NNews_DONTREUSE,   SWAM_NoLab, SWAM_SGI, FALSE),

   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_B MUIX_U "G" MUIX_N MUIX_B "roups",
   MUIX_B MUIX_U "S" MUIX_N MUIX_B "ervers",
   MUIX_B MUIX_U "N" MUIX_N MUIX_B "etwork",
   MUIX_B MUIX_U "C" MUIX_N MUIX_B "onfig",
   MUIX_B MUIX_U "P" MUIX_N MUIX_B "osts",
   MUIX_B MUIX_U "M" MUIX_N MUIX_B "ultimedia",

   "g",
   "s",
   "n",
   "c",
   "p",
   "m",

   "Show/edit list of news groups.",
   "Show/edit list of news servers.",
   "Show list of TCP (network) connections.",
   "Show/edit configuration options.",
   "Show/edit post queue.",
   "Show/edit multimedia types.",

   APPNAME ": News Groups",
   APPNAME ": NNTP Servers",
   APPNAME ": Post Queue",
   APPNAME ": TCP Connections",
   APPNAME ": Multimedia Types",
   APPNAME ": Configuration Options",
   APPNAME ": Launcher",

   "Project Options",
   "Program Options",
   "Article View Opts",
   "Filter Options",
   "Variables",
   "Headers",
   
   "Glob-Select",
   "Glob-Kill",
   "Glob-Display",
   "Glob-Actions",
   "Glob-Highlight",

   "User Options",
   "News Options",
   "Misc Options",
   "Window Options",
   "Post Options",

   "Post Wrap Mode: ",
     "When Post is Sent",
     "As Post is Typed",
     "Never",
   
   "Real Name: ",
   "User Email: ",
   "Organization: ",
   "Reply Line:",
   "Cache Dir: ",
   "Signature File: ",
   "Editor: ",
   "Header Memory Cache: ",
   "Startup Network Mode: ",
   "Online",
   "Offline",
   "Article Text Font: ",
   "Article Text Color: ",
   "Article Title: ",
   "Soft Scroll: ",
   "Quote Mode: ",
       "Off",
       "Line Prefix/Quote Blank",
       "Line Prefix/Skip Blank",
   "Quote Prefix: ",
   "Use Sig Separator: ",
   "Post Wrap Column: ",
   "Gen Msg IDs: ",
   "Article List Weight: ",
   "FAQ Server: ",
   "FAQ Directory: ",
   "AutoClip: ",
   "Header D/L Method: ",
       "Fetch Standard+Bytes Headers",
       "Fetch Standard Headers",
       "Fetch via overview (XOVER)",
   "Header D/L Increment: ",
   "Max Simultaneous Cmds: ",
   "Group Window Mode: ",
       "Single List/View Window",
       "Separate List/View Windows",
   "AutoStart Editor: ",
   "Max Saved Posts: ",
   "Tagline File: ",
   "Tagline Separator: ",
   "Enter Thread Action: ",
      "No Action",
      "Read 1st Available Article",
      "Read 1st Unread Article",
   "Printer Device: ",
   "Filter Toolbar Location: ",
      "Right",
      "Left",
      "Top",
      "Bottom",
      "ArtView Toolbar",
   "Labels: ",
   "Header Fetch Policy: ",
      "Fetch <= MaxArticles",
      "Fetch All",
      "Prompt if > MaxArticles",
   "Charset: ",
      "ISO-8859-1",
      "ISO-8859-2",
      "ISO-8859-3",
      "ISO-8859-4",
      "ISO-8859-5",
      "ISO-8859-6",
      "ISO-8859-7",
      "ISO-8859-8",
      "ISO-8859-9",
      "ISO-8859-10",
   "AutoSave Project: ",
      "Never",
      "After Closing Groups",
   "Body Encoding: ",
      "Quoted-Printable",
      "8-bit",
      "7-bit",
   "Hdrs: ",
      "Quoted-Printable",
      "8-bit",
   "Start Article: ",
      "Previous Article",
      "Previous Thread",
      "First in List",
      "Last in List",
   "Browse Articles: ",
      "All",
      "Unread",
      "New",
      "Unread&New",

   "Toolbar Style: ",
   "Horizontal/Labels",
   "Horizontal/No Labels",
   "Vertical/Labels",
   "Vertical/No Labels",
   
   "Group List Window: ",
      "Closed on project load",
      "Last saved state",
      "Opened on project load",
   
   "Server List Window: ",
      "Closed on project load",
      "Last saved state",
      "Opened on project load",

   "TCP List Window: ",
      "Closed on project load",
      "Last saved state",
      "Opened on project load",

   "Post Queue Window: ",
      "Closed on project load",
      "Last saved state",
      "Opened on project load",

   "Multimedia Types Window: ",
      "Closed on project load",
      "Last saved state",
      "Opened on project load",

   "Config Window: ",
      "Closed on project load",
      "Last saved state",
      "Opened on project load",

   "Group Windows: ",
      "Closed on project load",
      "Last saved state",

   "Your real name.",
   "Your email address.",
   "The name of the organization\nyou are posting from.",
   "The reply line for article posting.",
   "The directory for storing cached news messages\n"
      "and headers.  Performance will be improved\n"
      "if this is on a volume that uses disk cache.\n"
      "Workbench icons can be dropped on this gadget.",
   "The contents of this file will be appended to\n"
      "your posts.  This value is overridden by the\n"
      "group specific value, if set.  Workbench icons\n"
      "can be dropped on this gadget.",
   "The name of the editor you wish to use to edit\n"
      "posts.",
   "This is an (approximate) maximum size for the memory\n"
      "header cache.  The cache may exceed this size if\n"
      "the sum of the headers in all currently open groups\n"
      "exceeds this size.",
   "The default startup  networking mode.  A networking\n"
      "protocol stack must be present for online mode.",
   "The default font to use for viewing articles.",
   "The default color to use for viewing articles.",
   "The title to display above the article view.\n"
      "Any group or article variables may be used.",
   "SoftScroll setting for the article view.  Zero\n"
      "will disable softscroll.  Higher settings use\n"
      "more scroll steps.",
   "The quoting mode for posting\n"
      "followups to articles.  Choices are:\n"
      SWX_B("  Off") " - No quoting.\n\n"
      SWX_B("  Line Prefix/Quote Blank") " - Prefix\n"
      "   each line with the quote prefix, quoting\n"
      "   blank lines.\n\n"
      SWX_B("  Line Prefix/Skip Blank") " - The same,\n"
      "   but don't quote blank lines.",

   "The line prefix used\nto make quoted text.",

   "The window open/close settings for the project.\n"
      "Windows may be opened when their project is\n"
      "loaded, kept closed, or set to the previous\n"
      "open/close state saved with the project file.",

   "If checked, a signature separator line will be\n"
      "automatically added between the post body and\n"
      "the sig file.  Otherwise, the separator line,\n"
      "consisting of two dashes and a space (-- ),\n"
      "must be part of the signature file itself.",

   "The manner in which posts should be linewrapped.\n"
      "This may done:\n"
      SWX_B("When Post is Sent") " - Lines are wrapped when the\n"
      "  post is sent to the network.  Line breaks need\n"
      "  not be added in the editor except to separate\n"
      "  paragraphs.\n"
      SWX_B("As Post is Typed") " - Line breaks are inserted\n"
      "  automatically in the editor, and are not changed\n"
      "  when the post is sent.\n"
      SWX_B("Never") " - Neither of the above.",
         
   "The maximum line length used to wrap posted\n"
   "article text.  This should generally be less\n"
   "than 80 characters.",

   "If set, message IDs for posts are generated\n"
   "by NewsRog.  Otherwise, they are not\n"
   "transmitted with the post and it is up to\n"
   "the news server to generate message IDs.",

   "The toolbar style, which may be horizontal\n"
   "or vertical, and include labels or not.",

   "The vertical weight for the article list\n"
   "portion of group windows.  For example, if\n"
   "set to 30, then ratio of article list space\n"
   "to article view space will be 30:70.",

   "The FTP server for fetching group FAQs.",
   "The directory on the FAQ server where\n"
      "FAQs for a particular group are located.",

   "If set, text will be copied to the clipboard\n"
      "from an article view as soon as it is\n"
      "selected.  Otherwise, it must be explicitly\n"
      "copied using the copy icon or menu.",

   "This option controls the manner in which headers are\n"
      "fetched from the server:\n\n"
      SWX_B("Standard+Bytes Header") " - This option will cause NewsRog\n"
      "to fetch the full set of headers from the server,\n"
      "explicitly asking for the article size if the server\n"
      "does not provide it by default.\n\n"
       SWX_B("Standard Headers") " - This option is like the above\n"
      "but it will not explicitly ask for the article size.\n"
      "Some servers provide this by default.\n\n"
      SWX_B("Overview (XOVER)") " - This option will fetch a limited\n"
      "but still useful set of headers from the server using\n"
      "the XOVER command.  On servers that correctly support\n"
      "XOVER, this option is typically much faster than the above.",

   "This controls the number of headers fetched in one\n"
      "batch before updating the article list.  For example,\n"
      "if this number is 200, then the article list will be\n"
      "updated every 200 articles while the headers are\n"
      "loading in a group.  This can be set according to\n"
      "the speed of your server, network connection, and the\n"
      "header D/L method you have chosen above.",

   "Sometimes, for example during batch downloads, NewsRog\n"
      "issues multiple commands to the server without waiting\n"
      "for the results of the first to come in.  This improves\n"
      "streaming throughput of the NNTP connection, but a few\n"
      "servers don't cope well with this.  This option controls\n"
      "the maximum number of such simultaneous commands issued\n"
      "by NewsRog at once.",

   "This setting controls whether group windows are\n"
      "opened as a single window which combines both\n"
      "the group list and the article viewer, or as\n"
      "a separate window for each.",

   "If selected, the external text editor will be\n"
      "automatically started each time the post\n"
      "editor window is opened.  Otherwise, it must\n"
      "be started explicitly.",

   "This is the maximum number of sent posts\n"
      "to be archived in the post queue window.\n"
      "If zero, no posts will be archived.",

   "A file containing random taglines to be appended\n"
      "to posts.  Tagline entries are separated by the\n"
      "tag separator string.  If empty, no taglines\n"
      "will be generated.",

   "A string that separates tagline entries in a tag\n"
      "file.  For example, if set to %%, then:\n\n"
      "This is entry one\n"
      "%%\n"
      "And this is entry two.",

   "The action to perform when moving from All Threads\n"
   "mode to One Thread mode.  May be either: \n\n"
   SWX_B("  No Action ") " - No action is performed\n\n"
   SWX_B("  Read 1st Available Article ") " - The first\n"
   "  listed article will be automatically displayed\n"
   "  in the article view area.\n\n"
   SWX_B("  Read 1st Unread Article ") " - Similar, but\n"
   "  the first unread article will be displayed.",

   "The device to which printed\n"
   "articles should be sent.",

   "The location within the group window\n"
   "for the filter function toolbar.  Group\n"
   "windows must be closed and re-opened for\n"
   "changes in this setting to take effect.",

   "If selected, filter toolbar icons will\n"
   "be labeled with the filter's description.",

   "The policy to use when fetching new article headers\n"
      "from the server when opening groups windows:\n\n"
      SWX_B("  Fetch <= MaxArticles") " - Up to the MaxArticles\n"
      "  count set in the group window will be fetched\n"
      "  and the remainder silently ignored.\n\n"
      SWX_B("  Fetch All") " - All article headers will\n"
      "  be fetched, and MaxArticles will be ignored.\n\n"
      SWX_B("  Prompt if > MaxArticles") " - If there are\n"
      "  more than MaxArticles headers to be fetched,\n"
      "  you will be prompted for what to do.",

   "The charset that you use to read and write articles.\n"
     "This value will be transmitted as the charset parameter\n"
     "to the MIME Content-Type field, and must be one of the\n"
     "ISO-8859-X values defined by RFC2046.",

   "The NewsRog project file may be automatically saved\n"
     "after a newsgroup that was open is closed, to insure\n"
     "that the news statistics are properly saved.",

   "The encoding used for generating posts when the\n"
     "post body contains non-ASCII characters.  If\n"
     "set to 7-bit, no such postings will be allowed.",

   "The encoding used for generating posts when the\n"
     "post headers contain non-ASCII characters.  If\n"
     "set to 7-bit, no such postings will be allowed.",

   "When a group window is opened, this setting\n"
     "determines where the list cursor is placed:\n\n"
      SWX_B("  Previous Article ") "- The cursor starts at the\n"
      "  same article it was left on when the group was\n"
      "  closed, unless that article has expired.\n\n"
      SWX_B("  Previous Thread ") "- Like the above, except\n"
      "  the cursor is left on the entry point to\n"
      "  the previous thread, if available.\n\n"
      SWX_B("  First in List ") "- The cursor is placed at\n"
      "  the beginning of the list of articles.\n\n"
      SWX_B("  Last in List ") "- The cursor is placed at\n"
      "  the End of the list of articles.",

   "When browsing through articles using the\n"
      "browse functions, this setting controls\n"
      "which articles are displayed.",

   "Canceled.",
   "*_Ok",
   "Ok|*_Cancel",

   "Do you wish to exit NewsRog without saving\n"
      "the current project?",

   "Quit (NoSave)",
      
   "Network",
      "Online Mode",
      "Offline Mode",

   "Open Window",
      "Launcher...",
      "Newsgroups...",
      "Servers...",
      "TCP Connections...",
      "Post Queue...",
      "Multimedia Types...",
      "Configuration...",

   "Multimedia Types",
   "Multimedia Actions",

   "Outgoing Posts",
   "Sent Posts",

   "_Proceed|*_Cancel",
   SWX_B("Warning: ") "Proceeding with this action\n"
      "will lose changes made to this text.",
   "Loading Text Editor...",
   "Insert Text from File...",
   "Save Text to File...",
   "Cursor: ",
   "Insert text from a file into the editor.",
   "Save text to a disk file.",
   "Cut selected text & copy to the clipboard.",
   "Copy selected text to the clipboard.",
   "Paste text from the\nclipboard into the editor.",
   "Undo the last change.",
   "Redo the last change.",
   "Make the selected text " SWX_B("bold."),
   "Make the selected text " SWX_I("italic."),
   "Make the selected text " SWX_U("underlined."),
      
   SW_AppBase_CatStr,

   NULL,
};

static HelpMap SW_HELPMAP[] = {
   SWA_NNews_UserName,      SWS_H_UserName,
   SWA_NNews_UserEmail,     SWS_H_UserEmail,
   SWA_NNews_UserOrg,       SWS_H_UserOrg,
   SWA_NNews_ReplyLine,     SWS_H_ReplyLine,
   SWA_NNews_CacheDir,      SWS_H_CacheDir,
   SWA_NNews_SigFile,       SWS_H_SigFile,
   SWA_NNews_Editor,        SWS_H_Editor,
   SWA_NNews_HdrCacheSize,  SWS_H_HdrCS,
   SWA_NNews_VFont,         SWS_H_VFont,
   SWA_NNews_VColor,        SWS_H_VCol,
   SWA_NNews_ArtViewTitle,  SWS_H_AVTitle,
   SWA_NNews_QuoteMode,     SWS_H_QuoteMode,
   SWA_NNews_QuoteStr,      SWS_H_QuoteStr,
   SWA_NNews_GrpListOpen,   SWS_H_WinMode,
   SWA_NNews_TCPListOpen,   SWS_H_WinMode,
   SWA_NNews_SrvListOpen,   SWS_H_WinMode,
   SWA_NNews_PstListOpen,   SWS_H_WinMode,
   SWA_NNews_MMListOpen,    SWS_H_WinMode,
   SWA_NNews_CfgWinOpen,    SWS_H_WinMode,
   SWA_NNews_GrpWinOpen,    SWS_H_WinMode,
   SWA_NNews_SoftScroll,    SWS_H_SoftS,
   SWA_NNews_PostWrapType,  SWS_H_PRType,
   SWA_NNews_PostWrap,      SWS_H_PWrap,
   SWA_NNews_GenMsgID,      SWS_H_GenMID,
   SWA_NNews_ToolBarStyle,  SWS_H_TBStyle,
   SWA_NNews_ArtListWght,   SWS_H_ALWght,
   SWA_NNews_FAQServer,     SWS_H_FAQS,
   SWA_NNews_FAQDir,        SWS_H_FAQD,
   SWA_NNews_AutoClip,      SWS_H_AutoC,
   SWA_NNews_HdrDLType,     SWS_H_HDType,
   SWA_NNews_UseSigSep,     SWS_H_UseSS,
   SWA_NNews_HdrInc,        SWS_H_HDInc,
   SWA_NNews_SimulCmds,     SWS_H_SimCmd,
   SWA_NNews_GrpWinMode,    SWS_H_GWMode,
   SWA_NNews_AutoStartEd,   SWS_H_AEd,
   SWA_NNews_MaxSavedPosts, SWS_H_MaxSvPst,
   SWA_NNews_TagLineFile,   SWS_H_TagFile,
   SWA_NNews_TagLineSep,    SWS_H_TagSep,
   SWA_NNews_EnterThread,   SWS_H_ETType,
   SWA_NNews_PrinterDev,    SWS_H_PrtDev,
   SWA_NNews_FFTBLoc,       SWS_H_FFTBLoc,
   SWA_NNews_FFTBLab,       SWS_H_FFTBLab,
   SWA_NNews_FetchPolicy,   SWS_H_FetchPol,
   SWA_NNews_Charset,       SWS_H_Charset,
   SWA_NNews_AutoSave,      SWS_H_AutoSave,
   SWA_NNews_Enc_Body,      SWS_H_EncBody,
   SWA_NNews_Enc_Hdrs,      SWS_H_EncHdrs,
   SWA_NNews_StartArt,      SWS_H_StartArt,
   SWA_NNews_BrowseArts,    SWS_H_BrowseArts,
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


/*****************************************************************************
 * Function: make objects                                                    *
 *****************************************************************************/

static ULONG MakeObjs(Object *o, SW_IDATA *idata)
{
   if (!(idata->SWA_NNews_TCPList =
         MUI_NewObject(SWC_NTCPList,    AppBaseTopObj(o), TAG_DONE))   ||
       !(idata->SWA_NNews_ArtMgr  =
         MUI_NewObject(SWC_NArticleMgr, AppBaseTopObj(o), TAG_DONE))   ||
       !(idata->SWA_NNews_ArtTpl  =
         MUI_NewObject(SWC_NArticle,
                       SWA_NArticle_MemPool, idata->SWA_NNews_MemPool,
                       TAG_DONE))                                      ||
       !(idata->SWA_NNews_ServList =
         MUI_NewObject(SWC_NServerList, AppBaseTopObj(o), TAG_DONE))   ||
       !(idata->SWA_NNews_PostList =
         MUI_NewObject(SWC_NPostList,   AppBaseTopObj(o), TAG_DONE))   ||
       !(idata->SWA_NNews_SvPostList =
         MUI_NewObject(SWC_NPostList,
                       AppBaseTopObj(o),
                       SWA_NPostList_Saved, TRUE,
                       TAG_DONE))                                      ||
       !(idata->SWA_NNews_MMList =
         MUI_NewObject(SWC_FileTypes,   TAG_DONE))                     ||
       !(idata->SWA_NNews_FAList =
         MUI_NewObject(SWC_FileActs,    TAG_DONE))                     ||
       !(idata->SWA_NNews_GroupList =
         MUI_NewObject(SWC_NGroupList,  AppBaseTopObj(o), TAG_DONE))) {
      return FAILURE;
   }

   idata->Obj                          = o;

   idata->Win[SWV_GrpListWin].Contents = &idata->SWA_NNews_GroupList;
   idata->Win[SWV_SrvListWin].Contents = &idata->SWA_NNews_ServList;
   idata->Win[SWV_TcpListWin].Contents = &idata->SWA_NNews_TCPList;
   idata->Win[SWV_PstListWin].Contents = &idata->PLGrp;
   idata->Win[SWV_MMListWin].Contents  = &idata->MMGrp;
   idata->Win[SWV_CfgListWin].Contents = &idata->Config;
   idata->Win[SWV_NNewsWin].Contents   = &idata->Obj;

   idata->Win[SWV_GrpListWin].Title    = GetS(SWS_T_Groups);
   idata->Win[SWV_SrvListWin].Title    = GetS(SWS_T_Servers);
   idata->Win[SWV_TcpListWin].Title    = GetS(SWS_T_TCP);
   idata->Win[SWV_PstListWin].Title    = GetS(SWS_T_Post);
   idata->Win[SWV_MMListWin].Title     = GetS(SWS_T_MM);
   idata->Win[SWV_CfgListWin].Title    = GetS(SWS_T_Cfg);
   idata->Win[SWV_NNewsWin].Title      = GetS(SWS_T_NNews);

   idata->Win[SWV_GrpListWin].ID       = MAKE_ID('G','r','p','l');
   idata->Win[SWV_SrvListWin].ID       = MAKE_ID('S','r','v','l');
   idata->Win[SWV_TcpListWin].ID       = MAKE_ID('T','c','p','l');
   idata->Win[SWV_PstListWin].ID       = MAKE_ID('P','s','t','l');
   idata->Win[SWV_MMListWin].ID        = MAKE_ID('M','m','T','y');
   idata->Win[SWV_CfgListWin].ID       = MAKE_ID('C','o','n','f');
   idata->Win[SWV_NNewsWin].ID         = MAKE_ID('B','l','r','g');

   idata->Win[SWV_GrpListWin].LastVar  = SWA_NNews_GrpListLast;
   idata->Win[SWV_SrvListWin].LastVar  = SWA_NNews_SrvListLast;
   idata->Win[SWV_TcpListWin].LastVar  = SWA_NNews_TCPListLast;
   idata->Win[SWV_PstListWin].LastVar  = SWA_NNews_PstListLast;
   idata->Win[SWV_MMListWin].LastVar   = SWA_NNews_MMListLast;
   idata->Win[SWV_CfgListWin].LastVar  = SWA_NNews_CfgWinLast;
   idata->Win[SWV_NNewsWin].LastVar    = NULL;

   idata->Win[SWV_GrpListWin].OpenVar  = SWA_NNews_GrpListOpen;
   idata->Win[SWV_SrvListWin].OpenVar  = SWA_NNews_SrvListOpen;
   idata->Win[SWV_TcpListWin].OpenVar  = SWA_NNews_TCPListOpen;
   idata->Win[SWV_PstListWin].OpenVar  = SWA_NNews_PstListOpen;
   idata->Win[SWV_MMListWin].OpenVar   = SWA_NNews_MMListOpen;
   idata->Win[SWV_CfgListWin].OpenVar  = SWA_NNews_CfgWinOpen;
   idata->Win[SWV_NNewsWin].OpenVar    = NULL;

   return SUCCESS;
}

   
/*****************************************************************************
 * SetColSpecs                                                               *
 *****************************************************************************/

static void SetColSpecs(SW_IDATA *idata)
{
   SetAttrs(idata->SWA_NNews_ArtListCol,
            SWA_ListCol_ColSpecs, SWGetUL(idata->ArtLst, SWA_NArticleList_ColSpecs),
            SWA_ListCol_ColNames, SWGetUL(idata->ArtLst, SWA_NArticleList_ColNames),
            SWA_ListCol_FixWidth, FALSE,
            TAG_DONE);
}


/*****************************************************************************
 * Function: obtain user full name, if UserGroup library is around           *
 *****************************************************************************/

static void UserFullName(STRPTR FullName, STRPTR Login)
{
   struct passwd *pw;
   struct Library *UserGroupBase;

   FullName[0] = Login[0] = '\0';

   if (!(UserGroupBase = OpenLibrary(USERGROUPNAME, 4)))
      return;

   if (pw = getpwuid(getuid())) {
      if (FullName)
          strncpy(FullName, pw->pw_gecos, 31);
      if (Login)
          strncpy(Login,    pw->pw_name,  31);
   }

   FullName[31] = '\0';
   Login[31]    = '\0';
      
   CloseLibrary(UserGroupBase);
}


/*****************************************************************************
 * Get a valid email & host if possible                                      *
 *****************************************************************************/

static void GetUserJunk(Object *o, SW_IDATA *idata)
{
   TEXT FullName[31], Login[31], email[64];

   UserFullName(FullName, Login);

   sprintf(email, "%s@nospam.nowhere", Login);

   if (SW_STRISNULL(idata->SWA_NNews_UserName))
      set(o, SWA_NNews_UserName, FullName);

   if (SW_STRISNULL(idata->SWA_NNews_UserEmail))
      set(o, SWA_NNews_UserEmail, email);
}


/*****************************************************************************
 * Function: new global selection filter                                     *
 *****************************************************************************/

static Object *NewGlobSel(Object *o, SW_IDATA *idata, ULONG Use)
{
   return MUI_NewObject(SWC_NSFList,
                        SWA_NSFList_ShowGroups, TRUE,
                        SWA_NSFList_Use,        Use,
                        AppBaseTopObj(o),
                        TAG_DONE);
}


/*****************************************************************************
 * Function: create global filters                                           *
 *****************************************************************************/

static ULONG CreateGlobFlt(Object *o, SW_IDATA *idata)
{
   if (!idata->SWA_NNews_GlobSel)
      idata->SWA_NNews_GlobSel  = NewGlobSel(o, idata, SWV_NSFList_Use_Select);

   if (!idata->SWA_NNews_GlobHL)
      idata->SWA_NNews_GlobHL   = NewGlobSel(o, idata, SWV_NSFList_Use_Hlight);
   
   if (!idata->SWA_NNews_GlobKill)
      idata->SWA_NNews_GlobKill = NewGlobSel(o, idata, SWV_NSFList_Use_Kill);


   if (!idata->SWA_NNews_GlobDsp)
      idata->SWA_NNews_GlobDsp  =
         MUI_NewObject(SWC_NSFList,
                       SWA_NSFList_ShowGroups, TRUE,
                       SWA_NSFList_FltType,    "#?NDFilter#?",
                       AppBaseTopObj(o),
                       TAG_DONE);

   if (!idata->SWA_NNews_GlobAct)
      idata->SWA_NNews_GlobAct  =
         MUI_NewObject(SWC_NSFList,
                       SWA_NSFList_ShowGroups, TRUE,
                       SWA_NSFList_FltType,    "#?NAFilter#?",
                       AppBaseTopObj(o),
                       TAG_DONE);
   
   if (!idata->SWA_NNews_GlobSel  ||
       !idata->SWA_NNews_GlobHL   ||
       !idata->SWA_NNews_GlobKill ||
       !idata->SWA_NNews_GlobDsp  ||
       !idata->SWA_NNews_GlobAct) {
      MUI_DisposeObject(idata->SWA_NNews_GlobSel);
      MUI_DisposeObject(idata->SWA_NNews_GlobHL);
      MUI_DisposeObject(idata->SWA_NNews_GlobKill);
      MUI_DisposeObject(idata->SWA_NNews_GlobDsp);
      MUI_DisposeObject(idata->SWA_NNews_GlobAct);
      return FAILURE;
   }

   return SUCCESS;
}


/*****************************************************************************
 * Function: Close all windows                                               *
 *****************************************************************************/

static void CloseAll(Object *o, SW_IDATA *idata, ULONG Last, ULONG Shutdown)
{
   ULONG x;

   for (x=0; x < Last; x++)
      DoMethod(o, SWM_NNews_CloseWin, x, Shutdown);
}


/*****************************************************************************
 * Create instance data for this object                                      *
 *****************************************************************************/

static ULONG CreateObj(Object *o, SW_IDATA *idata)
{
   ULONG x, rc = FAILURE;

   if (!idata->SWA_AppBase_TopObj)
      idata->SWA_AppBase_TopObj = o;

   idata->ArtLst = MUI_NewObject(SWC_NArticleList,
                                 SWA_NArticleList_NoUI, TRUE,
                                 TAG_DONE);
   
   if (!idata->ArtLst) goto Done;

   idata->DTImg = DTImgObject, End;
   if (!idata->DTImg) goto Done;

   if (!(idata->SWA_NNews_DTImages =
         (DTImgCache*)DoMethod(idata->DTImg, SWM_DTImg_MakeImgCache)))
      goto Done;
   
   if (!(idata->SWA_NNews_ArtListCol = MUI_NewObject(SWC_ListCol,
          SWA_ListCol_ColSpecs, SWGetUL(idata->ArtLst, SWA_NArticleList_ColSpecs),
          SWA_ListCol_ColNames, SWGetUL(idata->ArtLst, SWA_NArticleList_ColNames),
          SWA_ListCol_FixWidth, FALSE,
          TAG_DONE)))
      goto Done;

   SetColSpecs(idata);

   DoMethod(idata->SWA_NNews_ArtListCol, SWM_ListCol_HideCols, 8, 9, -1);

   if (!(idata->SWA_NNews_VarList = MUI_NewObject(SWC_VarList,
                                                  AppBaseDefTop(idata),
                                                  TAG_DONE)))
      goto Done;
 
   if (!(idata->SWA_NNews_HdrList = MUI_NewObject(SWC_NHdrList,
                                                  AppBaseDefTop(idata),
                                                  TAG_DONE)))
      goto Done;


   GetUserJunk(o, idata);
   
   SetAttrs(o,            
            SWA_AppBase_ExitMode, SWV_AppBase_EM_Save,
            TAG_DONE);

   if (!(idata->SWA_NNews_MemPool = SWPoolCreate(MEMF_ANY|MEMF_CLEAR,
                                                 1024*80, 512, NULL)))
      goto Done;

   for (x=0; x<(SWS_L_CfgLast - SWS_L_CfgTab0 + 1); x++)
      idata->CfgTabs[x]   = GetS(SWS_L_CfgTab0 + x),
      idata->CfgTabs[x+1] = NULL;

   for (x=0; x<(SWS_L_FltLast - SWS_L_FltTab0 + 1); x++)
      idata->FltTabs[x]   = GetS(SWS_L_FltTab0 + x),
      idata->FltTabs[x+1] = NULL;

   idata->MMTabs[0] = GetS(SWS_L_FTypes);
   idata->MMTabs[1] = GetS(SWS_L_FActs);
   idata->MMTabs[2] = NULL;
   
   idata->PLTabs[0] = GetS(SWS_L_PToSend);
   idata->PLTabs[1] = GetS(SWS_L_PSent);
   idata->PLTabs[2] = NULL;
   
   if (CreateGlobFlt(o, idata) != SUCCESS)
      goto Done;

   if (MakeObjs(o, idata) != SUCCESS) goto Done;

   SetAttrs(o,
            GroupFrame,
            MUIA_Group_Horiz,    TRUE,
            MUIA_Background,     MUII_GroupBack,
            TAG_DONE);

   SetAttrs(o,
            SWA_AppBase_Cost1,     30,
            SWA_AppBase_Cost2,     35,
            SWA_AppBase_AppName,   APPNAME,
            SWA_AppBase_UnregSave, FALSE,
            TAG_DONE);

   DoMethod(SWGetObj(o, SWA_AppBase_App), MUIM_Notify,
            MUIA_Application_DoubleStart, TRUE,
            o, 5, MUIM_Application_PushMethod, o, 2,
            MUIV_Application_ReturnID_Quit, MUIV_Application_ReturnID_Quit);
   
   rc = SUCCESS;

  Done:
   return rc;
}


/*****************************************************************************
 * Function: clear all our lists                                             *
 *****************************************************************************/

static void ClrLists(SW_IDATA *idata)
{
   DoMethod(idata->SWA_NNews_GroupList,   MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_ServList,    MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_MMList,      MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_HdrList,     MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_FAList,      MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_TCPList,     MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_GlobSel,     MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_GlobHL,      MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_GlobAct,     MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_GlobKill,    MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_GlobDsp,     MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_PostList,    MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_SvPostList,  MUIM_List_Clear);
   DoMethod(idata->SWA_NNews_ArtMgr,      SWM_NArticleMgr_Clear);
}


/*****************************************************************************
 * Function: create an image button object                                   *
 *****************************************************************************/

static Object *MkIB(SW_IDATA *idata, ULONG l, ULONG h, STRPTR Img,
                    LONG Ctrl, ULONG Style)
{
   Object *g = VGroup,
      Child, DTImgObject,
         MUIA_InnerTop,        0,
         MUIA_InnerBottom,     0,
         MUIA_InnerLeft,       0,
         MUIA_InnerRight,      0,
         MUIA_Frame,           MUIV_Frame_ImageButton,
         MUIA_InputMode,       MUIV_InputMode_RelVerify,
         MUIA_ControlChar,     Ctrl,
         MUIA_ShortHelp,       GetS(h),
         MUIA_CycleChain,      1,
         SWA_DTImg_FSpec,      Img,
         SWA_DTImg_Adjust,     TRUE,
         SWA_DTImg_ImgCache,   idata->SWA_NNews_DTImages,
         SWA_DTImg_Path,       idata->SWA_NNews_ButtonPath,
      End,
   End;

   if (TBHasLab(Style) && g)
      DoMethod(g, OM_ADDMEMBER, CLabel(GetS(l)));

   return g;
}


/*****************************************************************************
 * Dispose instance data for this object                                     *
 *****************************************************************************/

static void DisposeObj(Object *o, SW_IDATA *idata)
{
   DoMethod(idata->SWA_NNews_GlobSel,  SWM_NSFilter_HideUI);
   DoMethod(idata->SWA_NNews_GlobHL,   SWM_NSFilter_HideUI);
   DoMethod(idata->SWA_NNews_GlobKill, SWM_NSFilter_HideUI);
   DoMethod(idata->SWA_NNews_GlobDsp,  SWM_NDFilter_HideUI);
   DoMethod(idata->SWA_NNews_GlobAct,  SWM_NAFilter_HideUI);
   MUI_DisposeObject(idata->ArtLst);

   //  -- close all our windows ----------------------------------------------

   CloseAll(o, idata, SWV_WinMax, TRUE);
   ClrLists(idata);

   DoMethod(idata->DTImg, SWM_DTImg_FreeImgCache, idata->SWA_NNews_DTImages);

   MUI_DisposeObject(idata->DTImg);

   // -- so objs don`t try to talk to the application object during dispose --

   set(o, SWA_AppBase_App, NULL);

}

static void DisposeObjPost(Object *o, SW_IDATA *idata)
{
   MUI_DisposeObject(idata->Config);
   MUI_DisposeObject(idata->MMGrp);
   MUI_DisposeObject(idata->PLGrp);
   SWPoolDelete(idata->SWA_NNews_MemPool);
}

# define SW_USERSETUP if (FirstTime) \
                         SWLocalizeNewMenu(SW_CATALOG, SW_CATSTR, MainMenu); \
                      EditorInit(libbase);

# define SW_USERCLEANUP EditorExit();

# define SW_SETUP_POSTSET                                     \
         if (CreateObj((Object *)retval, idata) != SUCCESS) { \
            CoerceMethod(cl, (Object *)retval,OM_DISPOSE);    \
            SWDB_RET(NULL);                                   \
         }

# define DISPOSECUST       DisposeObj(o, idata);
# define DISPOSECUST_POST  DisposeObjPost(o, idata);

# define SW_USERSET \
     if (Tag == SWA_AppBase_TopObj) SetTopObj(idata, (Object *)Data)

# define SW_Methods                    \
   SW_AppBase_Methods;                 \
   SW_DO_MF(SWM_NNews_OpenWin);        \
   SW_DO_MF(SWM_NNews_CloseWin);       \
   SW_DO_MF(SWM_NNews_MenuStrip);      \
   SW_DO_MF(SWM_NNews_MakeToolbar);    \
   SW_DO_MF(SWM_NNews_EditorClass);    \
   SW_DO_MF(SWM_NNews_CharSetName);    \
   SW_DO_MF(SWM_AppBase_MenuCall);     \
   SW_DO_MF(SWM_AppBase_ShowAppUI);    \
   SW_DO_MF(SWM_AppBase_PostNewProj);  \
   SW_DO_MF(SWM_AppBase_PostSaveProj); \
   SW_DO_MF(SWM_AppBase_RexxCmd);      \
   SW_DO_MF(SWM_VarList_Search);

# include "C-Editor.h"
# include "C-NNews-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
