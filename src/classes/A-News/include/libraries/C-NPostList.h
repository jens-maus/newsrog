# ifndef C_NPOSTLIST_INCLUDE
#  define C_NPOSTLIST_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NServer.h"
#  include "Libraries/C-NPost.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 13000)

   enum {
      SWA_NPostList_ListCol = SW_FIRSTTAG,    // [SGI] list columns
      SWA_NPostList_Saved,                    // [  I] stores saved posts
      SWA_NPostList_PostDir,                  // [ G ] post queue dir
   };

   enum {
      SWM_NPostList_Post = SW_FIRSTTAG,       // post selected
      SWM_NPostList_Post_P1,                  // pushmethod Post_P2
      SWM_NPostList_Post_P2,                  // post next article
      SWM_NPostList_Abort,                    // abort postings
      SWM_NPostList_OnOffLine,                // on or offline
      SWM_NPostList_AppMessage,               // appmessage
      SWM_NPostList_GenReply,                 // generate reply to grp/art
      SWM_NPostList_XferPost,                 // transfer to other postqueue
      SWM_NPostList_XferSel,                  // transfer selected posts
      SWM_NPostList_RemArch,                  // remove old archived posts
      SWM_NPostList_NGClass,                  // return NGList class ptr
   };

   enum {
      SWA_NGList_Str = SW_FIRSTTAG,
      SWA_NGList_HelpL,
      SWA_NGList_HelpR,
      SWA_NGList_Attr,
      SWA_NGList_AttrUI,
   };

   enum {
      SWM_NGList_GroupsToString = SW_FIRSTTAG,
      SWM_NGList_StrToGroups,
      SWM_NGList_StrToGroupsRO,
      SWM_NGList_DelSel,
      SWM_NGList_StrAck,
      SWM_NGList_Restore,
      SWM_NGList_DClick,
   };

   SWM_P(SWP_NPostList_XferPost,  Object *Post; Object *Dest);

   SWM_P(SWP_NPostList_OnOffLine,  ULONG State);
   SWM_P(SWP_NPostList_AppMessage, struct AppMessage *AMsg);

   SWM_P(SWP_NPostList_GenReply,              // generate reply
         Object *Grp;                         // to this group...
         Object *Art;                         // and this article
         Object *Win);                        // window for requestor

#  define NPostListObject MUI_NewObject(SWC_NPostList

#  undef SW_FIRSTTAG

# endif
