# ifndef C_LISTSEARCH_INCLUDE
#  define C_LISTSEARCH_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 90000)

   enum {
      SWA_ListSearch_Dynamic = SW_FIRSTTAG, // [SGI] dynamic searching
      SWA_ListSearch_Case,                  // [SGI] match case
      SWA_ListSearch_SubLine,               // [SGI] match part of line
      SWA_ListSearch_First,                 // [SGI] only 1st match
      SWA_ListSearch_StrLabel,              // [SGI] string label
      SWA_ListSearch_Title,                 // [SGI] window title
      SWA_ListSearch_List,                  // [SGI] points to list
      SWA_ListSearch_SearchObj,             // [SGI] obj to notify
      SWA_ListSearch_SearchMethod,          // [SGI] method to call
      SWA_ListSearch_CmpMethod,             // [SGI] method to call to cmp
      SWA_ListSearch_WindowID,              // [SGI] window ID
      SWA_ListSearch_ItemName,              // [SGI] name of items
      SWA_ListSearch_UI,                    // [SGI] small, optional UI
      SWA_ListSearch_PrevSrchPos,           // [SGI] pos to start search at
      SWA_ListSearch_ItemNamePl,            // [SGI] name of items(plural)
      SWA_ListSearch_ObjAttr,               // [SGI] object attribute
      SWA_ListSearch_Flags,                 // [SGI] flags
      SWA_ListSearch_InitSrchPos,           // [SGI] beginning search line
      SWA_ListSearch_PrevPat,               // [SGI] previous pattern
      SWA_ListSearch_StatusTxt,             // [ G ] status text line
      SWA_ListSearch_UpdSrchCnt,            // [  I] how often to update searching...
   };

   enum {
      SWM_ListSearch_OpenWin = SW_FIRSTTAG, // open search window
      SWM_ListSearch_CloseWin,              // close search window
      SWM_ListSearch_Search,                // search
      SWM_ListSearch_FromTop,               // search from top

      SWM_ListSearch_Setup,                 // setup before searching
      SWM_ListSearch_HLMatch,               // hilight match
      SWM_ListSearch_ListChg,               // list changed
   };

   enum {
      SWV_ListSearch_CmpMethod_Found    = -1, // possible returns from cmp method
      SWV_ListSearch_CmpMethod_NotFound = -2, // possible returns from cmp method
                                              // anything else is a string ptr!
   };

   enum {
      SWV_ListSearch_SimpleStr = -1,        // simple attribute
      SWV_ListSearch_ObjAttr   = -2,        // search by obj attribute
   };

   enum {
      SWF_ListSearch_HLMethod = (1<<0),     // use highlight method
   };

   SWM_P(SWP_ListSearch_OpenWin,    Object *App);
   SWM_P(SWP_ListSearch_Setup,      ULONG *State);
   SWM_P(SWP_ListSearch_Search_I,   STRPTR Str; ULONG Dyn);

   SWM_P(SWP_ListSearch_Search, STRPTR ParsedPat; ULONG Case; ULONG First);
   SWM_P(SWP_ListSearch_Cmp, Object *Entry; STRPTR Pat; ULONG Case; ULONG Len;
         ULONG SubLine; ULONG Wild; STRPTR OrigStr);

   SWM_P(SWP_ListSearch_HLMatch, LONG Line; STRPTR Str;
         ULONG Case; ULONG First; ULONG Wild);

#  undef SW_FIRSTTAG   

# endif
