# ifndef C_NARTICLEMGR_INCLUDE
#  define C_NARTICLEMGR_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NArticle.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10700)

   enum {
      SWA_NArticleMgr_Count     = SW_FIRSTTAG, // [ G ] # of articles
      SWA_NArticleMgr_MemPool,                 // [SGI] memory pool
      SWA_NArticleMgr_OVPct,                   // [SGI] overview % done
      SWA_NArticleMgr_Threads,                 // [SGI] T to keep thread info
      SWA_NArticleMgr_Master,                  // [SGI] T if we own the arts
   };

   enum {
      SWM_NArticleMgr_KillArts = SW_FIRSTTAG,  // kill articles
      SWM_NArticleMgr_Cache,                   // cache matching articles
      SWM_NArticleMgr_UnCache,                 // uncache matching articles
      SWM_NArticleMgr_GetCache,                // read articles from cache
      SWM_NArticleMgr_Clear,                   // clear array
      SWM_NArticleMgr_Match,                   // find matching articles
      SWM_NArticleMgr_Expire,                  // expire old articles
      SWM_NArticleMgr_Find,                    // find by MsgID
      SWM_NArticleMgr_HashStats,               // internal hash stats
      SWM_NArticleMgr_Add,                     // add new articles
      SWM_NArticleMgr_SaveOverview,            // save overview file
      SWM_NArticleMgr_LoadOverview,            // load overview file
      SWM_NArticleMgr_AddGroup,                // to notify about new articles
      SWM_NArticleMgr_RemGroup,                // remove
      SWM_NArticleMgr_FilterList,              // filter an article list
      SWM_NArticleMgr_MakeThreads,             // generate threads (expensive)
      SWM_NArticleMgr_ThreadInfo,              // get thread info about article
      SWM_NArticleMgr_ThreadBase,              // find thread base
      SWM_NArticleMgr_ThreadParent,            // find thread parent
      SWM_NArticleMgr_ThreadRefs,              // # of followups
      SWM_NArticleMgr_SameThread,              // TRUE if arts in same thread
      SWM_NArticleMgr_GetThreadArts,           // get arts in same thread
      SWM_NArticleMgr_RemoveOV,                // remove overview
      SWM_NArticleMgr_CleanCache,              // clean cache directory
      SWM_NArticleMgr_CDGroupName,             // get cache dir group name
      SWM_NArticleMgr_DelArts,                 // delete articles

      SWM_NArticleMgr_NewArticles,             // sent to groups when arts are
      SWM_NArticleMgr_DelArticles,             // added/deleted

      SWM_NArticleMgr_ReadListAdd,             // add to read articles list
      SWM_NArticleMgr_ReadListRem,             // remove from read articles list
      SWM_NArticleMgr_ReadListLoad,            // load read articles list
      SWM_NArticleMgr_ReadListSave,            // save read articles list
      SWM_NArticleMgr_ReadListRemBl,           // remove blank entries
      SWM_NArticleMgr_ReadListUpd,             // update read arts from list
      SWM_NArticleMgr_MultiPart_Find,          // find related parts to document
      SWM_NArticleMgr_MultiPart_Free,          // free multipart structure
      SWM_NArticleMgr_MultiPart_Check,         // see if we have all parts
   };

   enum {
      SWV_NArticleMgr_Filter_UseTbl = -1,      // filter from hash table
   };

   enum {
      SWT_NArticleMgr_Article   = SW_FIRSTTAG, // article ptr
      SWT_NArticleMgr_ArticleArray,            // array of articles
      SWT_NArticleMgr_ArticleCount,            // # in array
      SWT_NArticleMgr_MsgID,                   // message id
      SWT_NArticleMgr_LastSet,                 // this is the last article set
      SWT_NArticleMgr_CacheDir,                // cache directory
      SWT_NArticleMgr_MP_Include0,             // include part 0?
   };


   // -- multipart information -----------------------------------------------

   typedef struct _mpinfo {
      LONG   MaxPart;  // max parts
      STRPTR MsgID;    // message ID of article
      Object *Art;     // only valid right after call to MultiPart_Find.
      LONG   ArrayPos; // position in input array
   } MPInfo;

   SWM_P(SWP_NArticleMgr_MultiPart_Free,       MPInfo *MP);
   SWM_P(SWP_NArticleMgr_MultiPart_Check, MPInfo *MP;
         WORD *Missing; ULONG MArrayMax);

   SWM_P(SWP_NArticleMgr_ReadListAdd,    Object *Art);
   SWM_P(SWP_NArticleMgr_ReadListRem,    Object *Art);
   SWM_P(SWP_NArticleMgr_ReadListLoad,   STRPTR CacheDir);
   SWM_P(SWP_NArticleMgr_ReadListSave,   STRPTR CacheDir);
   SWM_P(SWP_NArticleMgr_ReadListUpd,    Object **ArtArray; ULONG ArtCount);

   SWM_P(SWP_NArticleMgr_Find,       STRPTR  MsgID);
   SWM_P(SWP_NArticleMgr_Group,      Object *Group);

   SWM_P(SWP_NArticleMgr_ThreadInfo,           // info about threads
         Object  *Art1;                        // article pointer
         Object  *Art2;                        // article pointer
         Object **Base1;                       // returned ptr to base art
         Object **Base2;                       // returned ptr to base art
         ULONG   *Nesting);                    // returned chain position

   SWM_P(SWP_NArticleMgr_ThreadBase,           // find thread base
         Object *Art);

   SWM_P(SWP_NArticleMgr_CDGroupName,          // cache dir group name
         STRPTR CDir;                          // cache dir name buff
         STRPTR GroupName;                     // grp name buff
         ULONG  GroupNameSize);                // buff size

   SWM_P(SWP_NArticleMgr_ThreadParent,         // find thread parent
         Object *Art);

   SWM_P(SWP_NArticleMgr_ThreadRefs,           // # of followups
         Object *Art;                          // article
         ULONG   Hash);                        // hash value

   SWM_P(SWP_NArticleMgr_SameThread,           // T if arts in same thread
         Object *Art1;                         // 1st article
         Object *Art2);                        // 2nd article

   SWM_P(SWP_NArticleMgr_GetThreadArts,        // get arts in same thread
         Object *Article;                      // an article in a thread [IN]
         Object **Base);                       // base article [OUT]

#  define ThreadArtEnd(x) ((x)==NULL||(x)==(Object *)-1)
   

#  undef SW_FIRSTTAG   

# endif
