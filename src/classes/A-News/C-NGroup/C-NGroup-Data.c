# include "C-NGroup-I.h"

// -- localize menus ---------------------------------------------------------

#  ifndef _GENPROTO

struct NewMenu MainMenu[] =
{
  { NM_TITLE, (STRPTR)SWS_M_Project,      0 ,0,0,(APTR)0                  },
  { NM_ITEM,  (STRPTR)SWS_M_P_Save,      "S",0,0,(APTR)SWS_M_P_Save       },
  { NM_ITEM,  (STRPTR)SWS_M_P_Close,     "Z",0,0,(APTR)SWS_M_P_Close      },
  { NM_ITEM,  (STRPTR)SWS_M_P_CloseNoSv, "Q",0,0,(APTR)SWS_M_P_CloseNoSv  },
  { NM_ITEM,  (STRPTR)NM_BARLABEL,        0 ,0,0,(APTR)0                  },
  { NM_ITEM,  (STRPTR)SWS_M_P_ToggleWins, 0, 0,0,(APTR)SWS_M_P_ToggleWins },
  
  { NM_TITLE, (STRPTR)SWS_M_Group,        0 ,0,0,(APTR)0                  },
  { NM_ITEM,  (STRPTR)SWS_M_G_FetchNew,  "H",0,0,(APTR)SWS_M_G_FetchNew   },
  { NM_ITEM,  (STRPTR)SWS_M_G_PostNew,   "N",0,0,(APTR)SWS_M_G_PostNew    },
  { NM_ITEM,  (STRPTR)SWS_M_G_CleanCache,"L",0,0,(APTR)SWS_M_G_CleanCache },
  { NM_ITEM,  (STRPTR)SWS_M_G_Expire,    "X",0,0,(APTR)SWS_M_G_Expire     },
  { NM_ITEM,  (STRPTR)SWS_M_G_FAQ,        0, 0,0,(APTR)SWS_M_G_FAQ        },
  { NM_ITEM,  (STRPTR)SWS_M_G_ResetArtPos,0, 0,0,(APTR)SWS_M_G_ResetArtPos},
  { NM_ITEM,  (STRPTR)SWS_M_G_Stats,     "`",0,0,(APTR)SWS_M_G_Stats      },

  { NM_TITLE, (STRPTR)SWS_M_Articles,     0, 0,0, (APTR)SWS_M_Articles    },
  { NM_ITEM,  (STRPTR)SWS_M_A_SelectAll, "A",0,0, (APTR)SWS_M_A_SelectAll },
  { NM_ITEM,  (STRPTR)SWS_M_A_Select,     0 ,0,0, (APTR)SWS_M_A_Select    },
  { NM_SUB,   (STRPTR)SWS_M_A_S_Read,     0 ,0,0, (APTR)SWS_M_A_S_Read    },
  { NM_SUB,   (STRPTR)SWS_M_A_S_Unread,   0 ,0,0, (APTR)SWS_M_A_S_Unread  },
  { NM_SUB,   (STRPTR)SWS_M_A_S_Marked,   0 ,0,0, (APTR)SWS_M_A_S_Marked  },
  { NM_SUB,   (STRPTR)SWS_M_A_S_Cached,   0 ,0,0, (APTR)SWS_M_A_S_Cached  },
  { NM_SUB,   (STRPTR)SWS_M_A_S_Uncached, 0 ,0,0, (APTR)SWS_M_A_S_Uncached},
  { NM_SUB,   (STRPTR)SWS_M_A_S_Perm,     0 ,0,0, (APTR)SWS_M_A_S_Perm    },
  { NM_SUB,   (STRPTR)SWS_M_A_S_New,      0 ,0,0, (APTR)SWS_M_A_S_New     },
  { NM_ITEM,  (STRPTR)SWS_M_A_ToggleSel, "T",0,0, (APTR)SWS_M_A_ToggleSel },
  { NM_ITEM,  (STRPTR)SWS_M_A_LimSel,    "&",0,0, (APTR)SWS_M_A_LimSel    },
  { NM_ITEM,  (STRPTR)SWS_M_A_Unlimit,    0 ,0,0, (APTR)SWS_M_A_Unlimit   },
  { NM_ITEM,  (STRPTR)SWS_M_A_Remove,     0 ,0,0, (APTR)SWS_M_A_Remove    },
  { NM_SUB,   (STRPTR)SWS_M_A_R_SelArts,  0 ,0,0, (APTR)SWS_M_A_R_SelArts },
  { NM_SUB,   (STRPTR)SWS_M_A_R_SelThrds, 0 ,0,0, (APTR)SWS_M_A_R_SelThrds},
  { NM_SUB,   (STRPTR)SWS_M_A_R_UnRemove, 0 ,0,0, (APTR)SWS_M_A_R_UnRemove},
  { NM_ITEM,  (STRPTR)SWS_M_A_Search,    "/",0,0, (APTR)SWS_M_A_Search    },
  { NM_ITEM,  (STRPTR)SWS_M_A_Selected,   0, 0,0, (APTR)SWS_M_A_Selected  },
  { NM_SUB,   (STRPTR)SWS_M_T_Read,      "R",0,0, (APTR)SWS_M_AS_Read     },
  { NM_SUB,   (STRPTR)SWS_M_T_Unread,    "U",0,0, (APTR)SWS_M_AS_Unread   },
  { NM_SUB,   (STRPTR)SWS_M_T_Mk,        ".",0,0, (APTR)SWS_M_AS_Mk       },
  { NM_SUB,   (STRPTR)SWS_M_T_UnMk,      ";",0,0, (APTR)SWS_M_AS_UnMk     },
  { NM_SUB,   (STRPTR)SWS_M_T_Cache,     "E",0,0, (APTR)SWS_M_AS_Cache    },
  { NM_SUB,   (STRPTR)SWS_M_T_Perm,      "P",0,0, (APTR)SWS_M_AS_Perm     },
  { NM_SUB,   (STRPTR)SWS_M_T_Uncache ,   0, 0,0, (APTR)SWS_M_AS_Uncache  },
  { NM_SUB,   (STRPTR)SWS_M_T_ResetNew ,  0, 0,0, (APTR)SWS_M_AS_ResetNew },
  { NM_ITEM,  (STRPTR)SWS_M_A_Thread,     0, 0,0, (APTR)SWS_M_A_Thread    },
  { NM_SUB,   (STRPTR)SWS_M_T_Read,      "0",0,0, (APTR)SWS_M_AT_Read     },
  { NM_SUB,   (STRPTR)SWS_M_T_Unread,    "1",0,0, (APTR)SWS_M_AT_Unread   },
  { NM_SUB,   (STRPTR)SWS_M_T_Mk,        "2",0,0, (APTR)SWS_M_AT_Mk       },
  { NM_SUB,   (STRPTR)SWS_M_T_UnMk,      "3",0,0, (APTR)SWS_M_AT_UnMk     },
  { NM_SUB,   (STRPTR)SWS_M_T_Cache,     "4",0,0, (APTR)SWS_M_AT_Cache    },
  { NM_SUB,   (STRPTR)SWS_M_T_Perm,      "5",0,0, (APTR)SWS_M_AT_Perm     },
  { NM_SUB,   (STRPTR)SWS_M_T_Uncache ,   0, 0,0, (APTR)SWS_M_AT_Uncache  },
  { NM_SUB,   (STRPTR)SWS_M_T_ResetNew ,  0, 0,0, (APTR)SWS_M_AT_ResetNew },
  { NM_ITEM,  (STRPTR)SWS_M_A_Group,      0, 0,0, (APTR)SWS_M_A_Group     },
  { NM_SUB,   (STRPTR)SWS_M_T_Read,       0, 0,0, (APTR)SWS_M_AG_Read     },
  { NM_SUB,   (STRPTR)SWS_M_T_Unread,     0, 0,0, (APTR)SWS_M_AG_Unread   },
  { NM_SUB,   (STRPTR)SWS_M_T_Mk,         0, 0,0, (APTR)SWS_M_AG_Mk       },
  { NM_SUB,   (STRPTR)SWS_M_T_UnMk,       0, 0,0, (APTR)SWS_M_AG_UnMk     },
  { NM_SUB,   (STRPTR)SWS_M_T_Cache,      0, 0,0, (APTR)SWS_M_AG_Cache    },
  { NM_SUB,   (STRPTR)SWS_M_T_Perm,       0, 0,0, (APTR)SWS_M_AG_Perm     },
  { NM_SUB,   (STRPTR)SWS_M_T_Uncache ,   0, 0,0, (APTR)SWS_M_AG_Uncache  },
  { NM_SUB,   (STRPTR)SWS_M_T_ResetNew ,  0, 0,0, (APTR)SWS_M_AG_ResetNew },

  { NM_TITLE, (STRPTR)SWS_M_Article,      0 ,0,0, (APTR)0                 },
  { NM_ITEM,  (STRPTR)SWS_M_E_SelectAll,  0 ,0,0, (APTR)SWS_M_E_SelectAll },
  { NM_ITEM,  (STRPTR)SWS_M_E_SaveTxt,    0, 0,0, (APTR)SWS_M_E_SaveTxt   },
  { NM_SUB,   (STRPTR)SWS_M_E_SV_Head,    0, 0,0, (APTR)SWS_M_E_SV_Head   },
  { NM_SUB,   (STRPTR)SWS_M_E_SV_Body,    0, 0,0, (APTR)SWS_M_E_SV_Body   },
  { NM_SUB,   (STRPTR)SWS_M_E_SV_HdBd,   "W",0,0, (APTR)SWS_M_E_SV_HdBd   },
  { NM_SUB,   (STRPTR)NM_BARLABEL,        0 ,0,0, (APTR)0                 },
  { NM_SUB,   (STRPTR)SWS_M_E_SV_RHead,   0, 0,0, (APTR)SWS_M_E_SV_RHead  },
  { NM_SUB,   (STRPTR)SWS_M_E_SV_RBody,   0, 0,0, (APTR)SWS_M_E_SV_RBody  },
  { NM_SUB,   (STRPTR)SWS_M_E_SV_RHdBd,   0, 0,0, (APTR)SWS_M_E_SV_RHdBd  },
  { NM_ITEM,  (STRPTR)SWS_M_E_Print,      0, 0,0, (APTR)SWS_M_E_Print     },
  { NM_SUB,   (STRPTR)SWS_M_E_PR_Head,    0, 0,0, (APTR)SWS_M_E_PR_Head   },
  { NM_SUB,   (STRPTR)SWS_M_E_PR_Body,    0, 0,0, (APTR)SWS_M_E_PR_Body   },
  { NM_SUB,   (STRPTR)SWS_M_E_PR_HdBd,    0, 0,0, (APTR)SWS_M_E_PR_HdBd   },
  { NM_SUB,   (STRPTR)NM_BARLABEL,        0 ,0,0, (APTR)0                 },
  { NM_SUB,   (STRPTR)SWS_M_E_PR_RHead,   0, 0,0, (APTR)SWS_M_E_PR_RHead  },
  { NM_SUB,   (STRPTR)SWS_M_E_PR_RBody,   0, 0,0, (APTR)SWS_M_E_PR_RBody  },
  { NM_SUB,   (STRPTR)SWS_M_E_PR_RHdBd,   0, 0,0, (APTR)SWS_M_E_PR_RHdBd  },
  { NM_ITEM,  (STRPTR)SWS_M_E_ArtClip,    0, 0,0, (APTR)SWS_M_E_ArtClip   },
  { NM_SUB,   (STRPTR)SWS_M_E_AC_Subj,    0, 0,0, (APTR)SWS_M_E_AC_Subj   },
  { NM_SUB,   (STRPTR)SWS_M_E_AC_From,    0, 0,0, (APTR)SWS_M_E_AC_From   },
  { NM_SUB,   (STRPTR)SWS_M_E_AC_MsgID,   0, 0,0, (APTR)SWS_M_E_AC_MsgID  },
  { NM_SUB,   (STRPTR)SWS_M_E_AC_Date,    0, 0,0, (APTR)SWS_M_E_AC_Date   },
  { NM_SUB,   (STRPTR)SWS_M_E_AC_Groups,  0, 0,0, (APTR)SWS_M_E_AC_Groups },
  { NM_SUB,   (STRPTR)SWS_M_E_AC_Head,    0, 0,0, (APTR)SWS_M_E_AC_Head   },
  { NM_SUB,   (STRPTR)SWS_M_E_AC_Body,    0, 0,0, (APTR)SWS_M_E_AC_Body   },
  { NM_SUB,   (STRPTR)SWS_M_E_AC_HdBd,    0, 0,0, (APTR)SWS_M_E_AC_HdBd   },
  { NM_ITEM,  (STRPTR)SWS_M_E_Redisp,     0, 0,0, (APTR)SWS_M_E_Redisp    },
  { NM_SUB,   (STRPTR)SWS_M_E_RD_Raw,     0, 0,0, (APTR)SWS_M_E_RD_Raw    },
  { NM_SUB,   (STRPTR)SWS_M_E_RD_Cooked,  0, 0,0, (APTR)SWS_M_E_RD_Cooked },
  { NM_ITEM,  (STRPTR)SWS_M_E_PostFup,   "F",0,0, (APTR)SWS_M_E_PostFup   },
  { NM_ITEM,  (STRPTR)SWS_M_E_Search,    "?",0,0, (APTR)SWS_M_E_Search    },
  
  { NM_TITLE, (STRPTR)SWS_M_Partial,      0 ,0,0, (APTR)0                 },
  { NM_ITEM,  (STRPTR)SWS_M_P_SelectAll,  0 ,0,0, (APTR)SWS_M_P_SelectAll },
  { NM_ITEM,  (STRPTR)SWS_M_P_Check,      0 ,0,0, (APTR)SWS_M_P_Check     },

  { NM_END,   (STRPTR)NULL,               0, 0,0, (APTR)0                 },
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Idle",
   "Aborted",
   "Checking Msg Count...",
   "Reading Description...",
   "Finding Article Numbers...",
   "Reading Header Data...",
   "Loading Overview...",
   "Saving Overview...",
   "D/L'ing Articles (%ld of %ld, %ld%%)...",
   "Cleaning Cache...",
   "Expiring old articles...",

   "Group Name: ",
   "Group Description: ",
   "Server: ",
   "Max Articles: ",
   "Max to Keep on Exit: ",
   "Expiration(Days): ",
   "D/L New on Startup: ",
   "Use Full Headers: ",
   "Mark Read on Exit: ",
   "Signature File: ",
   "Cached Articles: ",
   "Ask at Exit",
   "Keep at Exit",
   "Delete at Exit",
   "Mem cache articles: ",
   "Mem cache size: ",
   "Thread Prefetch: ",
   "ArticleView Title: ",
   "New Article Marker: ",
   "Reset at exit",
   "Reset when read",
   "Both",
   "Never",
   "Don't use",
   "Article Font: ",
   "Article Color: ",
   "Incremental Loading: ",
   "Read & Cache: ",

   "Get Description: ",
   "Check Msg #'s: ",
   "D/L Headers: ",
   "Fetch Article: ",
   "Fetching Expire Info: ",
   "Fetching Articles: ",

   "(Shown: %%ld of %ld)",

   "Read News",
   "Select Filters",
   "Display/Action Filters",
   "Group Options",

   "Select Filter",
   "Kill Filter",
   "Article Display Filter",
   "Article Action Filter",

   "Article/Thread Selector",
   "Article Text",

   "Selections filters can be toggled on or off to change the list of "
      "articles displayed.  For example, you can define a filter to "
      "display only " MUIX_B "FS:" MUIX_N " posts in a marketplace "
      "group, and another to display only " MUIX_B "WTB:" MUIX_N " "
      "posts, in effect subsetting the group.",

   "Kill filters match articles that should never appear in this "
      "group, no matter what other settings are chosen.  Killed "
      "articles cannot be retrieved.  The kill selections are "
      "processed when the group is loaded, or on new news when "
      "the headers are fetched from the news server.",

   "Article Display filters modify the display of the article by, "
      "for example, hiding various headers, or highlighting URLs "
      "for sending to a web browser.",

   "Article Action filters perform various actions on selected "
      "articles (e.g. saving them to disk files).",

   "_Ok|*_Cancel",
   "_Keep|*_Delete|_Cancel",
   SWX_B("Warning: ") "Closing this group without\n"
      "saving will lose the current state.  Read\n"
      "articles will appear unread later, and all\n"
      "other article state will be lost.",
   SWX_B("Note: ") "There are %ld cached\n"
      "articles in this group.",
   SWX_B("Note: ") "This action will delete the overview\n"
      "directory for group %s.",
   "*_Delete|_Keep",
   "Warning!",

   "The number of new articles available to be\n"
   "downloaded from the server (%ld) is larger\n"
   "than this group's max article setting (%ld).\n"
   "You may download all %ld articles, download\n"
   "only %ld articles, or cancel this operation.",
   "_All %ld|*_Only %ld|Cancel",
   "Fetch canceled.",
   "*_OK",

   "Save Article Text",

   "Set Read",
   "Set Unread",
   "Mark",
   "Unmark",
   "Cache",
   "Cache Permanent",
   "Uncache",
   "Reset New",

   "Group Project",
     "Save",
     "Close",
     "Close/NoSave",
     "Toggle Windows",
   "Group",
      "Fetch New",
      "Clean Cache",
      "Post New...",
      "Expire Articles",
      "View FAQ...",
      "Reset Article Pos",
      "Group Stats...",
   "Article List",
      "Select",
         "Read",
         "Unread",
         "Marked",
         "Cached",
         "Uncached",
         "Permanent",
         "New",
      "Select All",
      "Toggle Selected",
      "Search...",
      "Limit to Selected",
      "Unlimit",
      "Remove",
          "Selected Articles",
          "Selected Threads",
          "Un-Remove",
      "Selected Articles",
      "All in Thread",
      "All in Group",
   "Viewer Article",
      "Select All Text",
      "Save Text",
         "Headers...",
         "Body...",
         "Hdrs+Body...",
         "Raw Headers...",
         "Raw Body...",
         "Raw Hdrs+Body...",
      "Print",
         "Headers",
         "Body",
         "Hdrs+Body",
         "Raw Headers...",
         "Raw Body...",
         "Raw Hdrs+Body...",
      "Search...",
      "Post Followup...",
      "Clip",
         "Subject",
         "From",
         "Message-ID",
         "Date",
         "Groups",
         "Headers",
         "Body",
         "Hdrs+Body",
      "Redisplay",
         "Raw (No Filters)",
         "Normal",
   "Partial (Multipart)",
      "Select Related Parts",
      "Check for Parts...",
      "Display One Part",
      "Display All Parts",

   "The name of the group to fetch\n"
      "from the news server.",
   "The group description.",
   "The server from which to read the group.\n"
      "If left blank, the default server will be\n"
      "used.  By setting this value, you can obtain\n"
      "some groups from different servers than others.",
   "The maximum number of article headers to fetch\n"
      "from the server.  If the number available on\n"
      "the server is smaller, this has no effect.",
   "The maximum number of articles headers to keep\n"
      "locally when this group is closed.  Any\n"
      "over this amount are discarded.",
   "The number of days to retain header informain in\n"
      "the local cache.  Setting this to a large value\n"
      "may use a considerable amount of disk space.",
   "If set, new new header information will be read\n"
      "from the news server when the group window is\n"
      "opened.  This happens asynchronously, so\n"
      "messages can be read while it is happening.\n"
      "If unset, new news must be fetched manually\n"
      "using the " MUIX_I "News/Fetch New" MUIX_N "  menu or the\n"
      MUIX_I "NewNews" MUIX_N "  button.",
   "If set, the full set of header information is\n"
      "obtained from the server when an article is\n"
      "downloaded.  This includes auxilliary headers\n"
      "such as " MUIX_I "Path:" MUIX_N "  that are not normally downloaded.",
   "If set, all unread articles in a group will be\n"
      "marked as read when the group is closed.",
   "If set, the contents of this file overrides\n"
      "the signature file setting from the main\n"
      "preferences.  This allows group specific\n"
      "sig files.",
   "This specifies the action that should be taken\n"
      "for cached articles when the group is closed.\n"
      "They can be deleted or kept.  The default is\n"
      "to ask.  This does not affect permanent\n"
      "articles.",
   "The maximum number of articles to fetch from\n"
      "the server for this group.",
   "The maximum size of the local article memory\n"
      "cache for this group.",
   "When browsing threads, subsequent articles can be\n"
      "prefetched so that they are instantly available.\n"
      "This is the number of following articles to\n"
      "prefetch when an article is read.  This has no\n"
      "effect in offline mode.  Only as many articles\n"
      "as will fit in the memory cache will be prefetched.",
   "A group specific article view title.  If blank, the\n"
      "title from the main preferences is used.",
   "Handling for the new article marker.  Options are:\n"
      SWX_B("  Reset at exit") " - New article markers\n"
         "  are removed when the group is closed.\n\n"
      SWX_B("  Reset when read") " - New article markers\n"
         "  are removed when that article is read.\n\n"
      SWX_B("  Both") " - New article markers are removed\n"
         "  both when the groups is closed, and when\n"
         "  articles are read.\n\n"
      SWX_B("  Never") " - New article markers are never\n"
         "  removed automatically, and must be removed\n"
         "  using the menu option.\n\n"
      SWX_B("  Don't Use") " - New article markers are not\n"
         "  set in the first place.",
   "If enabled, articles viewed interactively will be\n"
      "loaded incrementally.  That is, the first 100 lines\n"
      "of the article will be displayed while the rest\n"
      "is still loading.  Otherwise, the full article\n"
      "is fetched from the network before any of it is\n"
      "displayed, which can be preferable for groups\n"
      "containing binaries.",
   "If checked, articles are written to the disk cache\n"
      "for this group as you read them interactively.\n"
      "Otherwise, interactively read articles are not\n"
      "copied to the disk cache.",
   "If enabled, this font is used as the default text\n"
      "font for articles in this group.  If unset, the\n"
      "font from the main preferences window is used.",
   "If enabled, this color is used as the default text\n"
      "color for articles in this group.  If unset, the\n"
      "color from the main preferences window is used.",

   "The main article/thread list shows articles\n"
      "in the group.  Double clicking on a thread\n"
      "will view articles in that thread, or view\n"
      "the article itself.  The list of displayed\n"
      "articles can be restricted in various ways.",

   "Toggle between viewing:\n"
      SWX_B("  All Articles\n")
      SWX_B("  All Threads\n")
      SWX_B("  One Thread\n\n")
      "Currently the mode is " SWX_B("One Thread.\n")
      "Pressing this button will move to " SWX_B("All Articles.\n")
      "Hotkey: 't'",

   "Toggle between viewing:\n"
      SWX_B("  All Articles\n")
      SWX_B("  All Threads\n")
      SWX_B("  One Thread\n\n")
      "Currently the mode is " SWX_B("All Threads.\n")
      "Pressing this button will move to " SWX_B("One Thread\n")
      "Hotkey: 't'",

   "Toggle between viewing:\n"
      SWX_B("  All Articles\n")
      SWX_B("  All Threads\n")
      SWX_B("  One Thread\n\n")
      "Currently the mode is " SWX_B("All Articles.\n")
      "Pressing this button will move to " SWX_B("All Threads.")
      "Hotkey: 't'",

   "If selected, then read articles are displayed.\n"
      "The list of articles will change accordingly.\n"
      "In " MUIX_B "Thread" MUIX_N " mode, threads will be listed if\n"
      "any article in the thread is read.  Hotkey: 'R'",

   "If selected, then unread articles are displayed.\n"
      "The list of articles will change accordingly.\n"
      "In " MUIX_B "Thread" MUIX_N " mode, threads will be listed if\n"
      "any article in the thread is unread.  Hotkey: 'U'",

   "Toggle between viewing:\n"
      MUIX_B "  Articles by All Authors"  MUIX_N "\n"
      MUIX_B "  Articles by One Author"   MUIX_N "\n\n"
      "The list of articles will change accordingly.\n"
      "Double Clicking on an author name in the\n"
      "article list will move to " MUIX_B "One Author" MUIX_N "\n"
      "mode.  Hotkey: 'A'",

   "Change the active view (selection) filters.\nHotkey: 'v'",
   "Change the active display filters.\nHotkey: 'V'",
   "Perform batch actions on articles.\nHotkey: '!'",
   "Change the active highlight filters.\nHotkey: 'H'",

   "Kill articles or threads.  Hotkey: 'k'",

   "Remove articles or threads without\n"
      "killing.  Hotkey: 'z' (lower case) to\n"
      "remove selected articles, and 'Z' (upper\n"
      "case) to remove selected threads.",

   "Browse news.  Pages down in current article.\n"
      "after reaching the bottom of the article,\n"
      "the next article in the same thread is\n"
      "displayed.  After the last article in the\n"
      "thread, the next thread is shown, and so on.\n"
      "Hotkey: '.'",

   "Browse news backwards. Pages up in current article.\n"
      "after reaching the top of the article,\n"
      "the previous article in the same thread is\n"
      "displayed.  After the first article in the\n"
      "thread, the previous thread is shown, and so on.\n"
      "Hotkey: ','",

   "Post a new article to this group.\nHotkey: 'N'",
   "Post a followup article to this group.\nHotkey: 'F'",

   "Cancel any pending network transfer.",

   "Change the thread mode to a less\n"
      "restrictive view.  Hotkey: 'u'",

   "Change the thread mode to a more\n"
      "restrictive view.  Hotkey: 'd'",

   "Forward Thread - This button moves to the\n"
   "next available thread and views the first\n"
   "article in that thread.  Hotkey: ']'",

   "Backward Thread - This button moves to the\n"
   "previous available thread and views the\n"
   "first article in that thread.  Hotkey: '['",

   "Forward Article - This button moves to the\n"
   "next article in the present thread, and upon\n"
   "reaching the end of the thread, moves to the\n"
   "first article in the next thread.  Hotkey: ')'",

   "Backward Article - This button moves to the\n"
   "previous article in the present thread, and upon\n"
   "reaching the end of the thread, moves to the\n"
   "first article in the next thread.  Hotkey: '('",

   "Quick Search: search and select\n"
   "matching articles.  Hotkey: '/'",

   "Select the sort criteria.  Hotkey: '='",

   "Selects which header to match when searching\n"
   "the article list.  Also, it is possible to\n"
   "search through all header fields, or through\n"
   "the bodies of cached articles.",

   "When searching through bodies of cached\n"
   "articles, the article display will be\n"
   "updated if this option is checked.",

   "If this option is checked, then ALL articles\n"
      "are loaded into RAM and searching happens from\n"
      "memory instead of disk, which is many times\n"
      "faster but may take a great deal of memory.",
      
   "Article Search",
   "Article",
   "Articles",
   "Match " SWX_U("H") "eader: ",
   SWX_U("U") "pdate Disp: ",
   "Search from RAM: ",
   "Loading %ld...",
   "Freeing %ld...",
   "The total article size in this group is approximately\n"
      "%s, which is too large for this operation\n"
      "to succeed.  There is currently %s fast RAM\n"
      "available, and 4 Mb are reserved.",
   "*_Ok",
      
   "Local Group Filters",
   "Global Filters",

   "_Update",
   "_Close",
   "_Edit",
   "_Run",
   "Redisplay the article list based on\n"
      "the current filter settings.",
   "Close the popup window.",
   "Edit the active filter.",
   "Run the selected actions against the\n"
      "currently selected articles.",

   "Remove Selected Articles",
   "Remove Selected Threads",
   "Un-Remove",

   "Removes the selected articles from the group,\n"
      "but in a non-permanent manner.  Articles removed\n"
      "in this manner will be seen again the next time\n"
      "this group is opened, or when the Un-Remove\n"
      "command is used.  Hotkey: 'z' (lower case).",
      
   "Removes the selected threads from the group,\n"
      "but in a non-permanent manner.  Articles removed\n"
      "in this manner will be seen again the next time\n"
      "this group is opened, or when the Un-Remove\n"
      "command is used.  Hotkey: 'Z' (upper case).",
      
   "Restores removed articles to the group.  Closing\n"
      "and reopening the group will also do this.",
      
   "Kill Selected Articles",
   "Kill Selected Threads",
   "Kill all by same Author",
   "Kill Current Articles",
   "Kill Current & Future Articles",
   "_Cancel",
   "_Kill",
   "Expiration: ",
   "Location: ",
   "Kill in This Group",
   "Kill in All Groups",

   SWX_B("Selected") " kills selected articles, or\n"
      "   threads if in " SWX_B("All Threads") " mode.\n"
   SWX_B("Threads") " kills selected articles and all\n"
      "   other articles in the same thread.\n"
   SWX_B("Author") " kills selected articles and all\n"
      "   other articles by the same author.\n\n"
   "This setting can be cycled with the 't' key.",

   SWX_B("Current") " kills articles present now.\n"
   SWX_B("Current & Future") " kills current articles\n"
      "   and creates a kill filter entry to kill future\n"
      "   articles in the same thread or by the same.\n"
      "   author.\n\n"
   "This setting can be cycled with the 'w' key.",

   "Expiration for the kill filter entry created\n"
      "for future kills.  This is a number of days\n"
      "that the filter must exist without matching\n"
      "anything before it is expired.",
   "Cancel without killing.",
   "Perform selected kill action.",

   SWX_B("This Group") " kills only articles in the\n"
      "   current group\n"
   SWX_B("All Groups") " kills articles in other groups\n"
      "   as well as this group.\n\n"
   "This setting can be cycled with the 'l' key.",

   "Misc Options",
   "Cache Options",
   "Startup/Exit Options",

   "FAQs",
   "View FAQ",
   "Update List",
   "Delete FAQ",
   "Select All",
   "Abort",
   "Action...",
   "This window is used to fetch Frequently Asked\n"
      "Questions (FAQ) documents from a FAQ FTP server.\n"
      "they can then be displayed or cached locally for\n"
      "future reference.",
   "View a selected FAQ",
   "Update the list of FAQs\n"
      "from the FAQ server.",
   "Delete the selected FAQs from the local disk.\n"
      "Data on the remote server is unaffected.",
   "Select all FAQs in the list.",
   "Abort any pending FAQ network I/O.",
   "Perform a Multimedia Action on a FAQ.  FAQs have\n"
      "a hardcoded MIME type of " SWX_B(SWV_FAQList_FAQType) ".\n"
      "The variable " SWX_I("${FAQFile}") " may be used in multimedia\n"
      "actions and points to the filename of the selected\n"
      "FAQ.  All group variables may be used as well.",
   "%ld FAQs",
   "Fetching FAQ List...",
   "Fetching FAQ...",
   "FAQ Transfer Aborted.",
   MUIX_U MUIX_B "FAQ Name",
   "No FAQs found.",
   "Done Fetching FAQ.",
   "FAQ directory not found: %s",

   "Display the list of available FAQs\n"
      "for this group.  Hotkey: 'Q'",

   "D/L Headers: %%ld%%%% (%ld of %ld, %ld.%02ld/sec)\nET: %ld:%02ld   Remaining: %ld:%02ld",
   "D/L Articles: %%ld%%%% (%ld of %ld, %ld.%02ld/sec)\nET: %ld:%02ld   Remaining: %ld:%02ld",

   "Count",
   "Percent",
   "Size",
   "Group Statistics",
   "Total Articles",
   "# Read",
   "# Unread",
   "# Killed",
   "# Cached",
   "# Permanent",
   "# Hilighted",
   "# New",
   "# Displayed",

   SWX_B("Note: ") "%ld of %ld parts could not be located:\n  %s",
   "*_Continue",
      
   SWX_B("Error: ") "Group `%s' not found on server.",
   SWX_B("Error: ") "Unknown error code: %ld",
   SWX_B("Error: ") "Out of memory.",
   SWX_B("Error: ") "Problem creating group modify file.",
   SWX_B("Error: ") "Problem removing group modify file.",
   SWX_B("Error: ") "This group has no name.",
   SWX_B("Error: ") "Message-ID:\n  %s\nnot found.",
         
   NULL,
};

// -- help map ---------------------------------------------------------------

HelpMap SW_HELPMAP[] = {
   SWA_NGroup_Name,        SWS_H_Name,
   SWA_NGroup_Desc,        SWS_H_Desc,
   SWA_NGroup_Server1,     SWS_H_Srv,
   SWA_NGroup_MaxArticles, SWS_H_MaxA,
   SWA_NGroup_MaxKeep,     SWS_H_MaxK,
   SWA_NGroup_ExpireDays,  SWS_H_ExpD,
   SWA_NGroup_DLHdrStart,  SWS_H_DLHd,
   SWA_NGroup_ReadOnExit,  SWS_H_RonE,
   SWA_NGroup_SigFile,     SWS_H_SigF,
   SWA_NGroup_FullHeaders, SWS_H_FlHd,
   SWA_NGroup_CacheExit,   SWS_H_CatE,
   SWA_NGroup_MemCacheNum, SWS_H_MmCn,
   SWA_NGroup_MemCacheKb,  SWS_H_MmCs,
   SWA_NGroup_PrefetchNum, SWS_H_PreF,
   SWA_NGroup_ArtViewTitle,SWS_H_AVT,
   SWA_NGroup_NewArtMd,    SWS_H_NaMd,
   SWA_NGroup_IncrDisp,    SWS_H_IncD,
   SWA_NGroup_ReadnCache,  SWS_H_RCach,
   NULL, NULL,
};

# endif

