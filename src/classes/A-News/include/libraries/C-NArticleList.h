# ifndef C_NARTICLELIST_INCLUDE
#  define C_NARTICLELIST_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NArticle.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 11200)

   enum {
      SWA_NArticleList_Title       = SW_FIRSTTAG,  // [SGI] title string
      SWA_NArticleList_QSH,                        // [SGI] quicksearch header pos
      SWA_NArticleList_Sort1,                      // [SGI] sort attribute
      SWA_NArticleList_Dir1,                       // [SGI] reverse sort
      SWA_NArticleList_Sort2,                      // [SGI] sort attribute
      SWA_NArticleList_Dir2,                       // [SGI] reverse sort
      SWA_NArticleList_Sort3,                      // [SGI] sort attribute
      SWA_NArticleList_Dir3,                       // [SGI] reverse sort
      SWA_NArticleList_ToolBar,                    // [ GI] toolbar
      SWA_NArticleList_ClickAttr,                  // [SGI] clicked in list
      SWA_NArticleList_ClickItem,                  // [SGI] clicked on article
      SWA_NArticleList_MemPool,                    // [SGI] memory pool
      SWA_NArticleList_UNUSED1,                    // [SGI] unused
      SWA_NArticleList_ThreadMgr,                  // [ G ] for threads
      SWA_NArticleList_Threaded,                   // [SGI] display indents
      SWA_NArticleList_ColNames,                   // [ G ] column labels
      SWA_NArticleList_ColSpecs,                   // [ G ] column names
      SWA_NArticleList_ListCol,                    // [ GI] listcol
      SWA_NArticleList_Group,                      // [  I] our parent group
      SWA_NArticleList_NoUI,                       // [  I] don`t need UI
      SWA_NArticleList_HEntries,                   // [ G ] search hdr entries
      SWA_NArticleList_SortBt,                     // [  I] sort button
      SWA_NArticleList_QSUpdDisp,                  // [SGI] upd in body srch?
      SWA_NArticleList_QSFromRam,                  // [SGI] search from RAM
   };

   enum {
      SWM_NArticleList_CtxCenter    = SW_FIRSTTAG, // center an article
      SWM_NArticleList_Click,                      // click on columns
      SWM_NArticleList_UNUSED1,                    // UNUSED
      SWM_NArticleList_SrchCmp,                    // search compare function
      SWM_NArticleList_ChangeTitle,                // new title
   };

   enum {
      SWV_NArticleList_A_Permanent = 0,            // click attributes
      SWV_NArticleList_A_Lines,
      SWV_NArticleList_A_NumGroups,
      SWV_NArticleList_A_From,
      SWV_NArticleList_A_Date,
      SWV_NArticleList_A_Time,
      SWV_NArticleList_A_Repl,
      SWV_NArticleList_A_Subject,

      SWV_NArticleList_KeyCC         = 128,
   };

   SWM_P(SWP_NArticleList_Click,       LONG Column);
   SWM_P(SWP_NArticleList_ChangeTitle, ULONG Count);
   SWM_P(SWP_NArticleList_CtxCenter,   ULONG Pos; ULONG CtxLines);

#  define NArticleListObject MUI_NewObject(SWC_NArticleList

#  undef SW_FIRSTTAG

# endif
