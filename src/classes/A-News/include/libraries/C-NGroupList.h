# ifndef C_NGROUPLIST_INCLUDE
#  define C_NGROUPLIST_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NGroup.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10800)

   enum {
      SWA_NGroupList_List      = SW_FIRSTTAG,  // [SGI] list list
      SWA_NGroupList_FullNames,                // [SGI] dsp grp full names
      SWA_NGroupList_MemPool,                  // [SGI] memory pool
      SWA_NGroupList_Sort1,                    // [SGI] sort attribute
      SWA_NGroupList_Dir1,                     // [SGI] reverse sort
      SWA_NGroupList_Sort2,                    // [SGI] sort attribute
      SWA_NGroupList_Dir2,                     // [SGI] reverse sort
      SWA_NGroupList_Sort3,                    // [SGI] sort attribute
      SWA_NGroupList_Dir3,                     // [SGI] reverse sort
      SWA_NGroupList_ListCol,                  // [SGI] list columns
      SWA_NGroupList_UpdInt,                   // [SGI] update interval
      SWA_NGroupList_Upd,                      // [SGI] T to periodically
      SWA_NGroupList_UpdOpen,                  // [SGI] update on startup
      SWA_NGroupList_LastFetchPat,             // [SGI] last fetch pattern
      SWA_NGroupList_BatchDLFlt,               // [SGI] batch download filter
      SWA_NGroupList_LastSGDate,               // [SGI] last servergroup date
      SWA_NGroupList_SGUpdateInt,              // [SGI] SrvGrp update interval
      SWA_NGroupList_SGPatt,                   // [SGI] SrvGrp pattern
      SWA_NGroupList_SG_L,                     // [ G ] servergroup list
      SWA_NGroupList_SG_LV,                    // [ G ] servergroup listview
   };

   enum {
      SWM_NGroupList_UNUSED    = SW_FIRSTTAG,  // UNUSED
      SWM_NGroupList_SWin,                     // open search window
      SWM_NGroupList_DoGroup,                  // call methods for mult. groups
      SWM_NGroupList_DoGroup_I,                // iterator for above
      SWM_NGroupList_DoGroup_Abort,            // abort above
      SWM_NGroupList_ChangeMode,               // online/offline mode change
      SWM_NGroupList_Delete,                   // remove groups
      SWM_NGroupList_OpenSGWin,                // open server groups win
      SWM_NGroupList_CloseSGWin,               // close server groups win
      SWM_NGroupList_FetchSG,                  // fetch server groups
      SWM_NGroupList_FetchSG_P,                // parse result from above
      SWM_NGroupList_AbortSG,                  // abort server group xfer
      SWM_NGroupList_SortSG,                   // sort server groups
      SWM_NGroupList_SaveSG,                   // save server groups
      SWM_NGroupList_LoadSG,                   // load server groups
      SWM_NGroupList_SubSG,                    // subscribe to selected
      SWM_NGroupList_SubNewsRC,                // subscribe from newsrc file
      SWM_NGroupList_Subscribe,                // subscribe to list of groups
      SWM_NGroupList_ImportNRC,                // import newsrc
      SWM_NGroupList_ImportCD,                 // import from cache dir
      SWM_NGroupList_UpdCheck,                 // check for new news
      SWM_NGroupList_ClipSG,                   // clip server group list
      SWM_NGroupList_NewSG,                    // show new server groups
      SWM_NGroupList_BrowseSG,                 // browse a group
   };

   SWM_P(SWP_NGroupList_DoGroup,       ULONG Cmd; ULONG All);
   SWM_P(SWP_NGroupList_DoGroup_Abort, ULONG Cmd);
   SWM_P(SWP_NGroupList_ChangeMode,    ULONG Online);
   SWM_P(SWP_NGroupList_SaveSG,        STRPTR Path);
   SWM_P(SWP_NGroupList_LoadSG,        STRPTR Path);
   SWM_P(SWP_NGroupList_BrowseSG,      STRPTR Group);
   SWM_P(SWP_NGroupList_SubSG,         ULONG InsertPos);
   SWM_P(SWP_NGroupList_SubNewsRC,     STRPTR Path; ULONG AddUnSub);
   SWM_P(SWP_NGroupList_Subscribe,     STRPTR *Groups; ULONG Count;
                                       ULONG *Dups; ULONG InsertPos;
                                       Object *Win);

#  undef SW_FIRSTTAG

# endif
