# ifndef C_NARTICLEVIEW_INCLUDE
#  define C_NARTICLEVIEW_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"
#  include <sys/types.h>

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10200)

   enum {
      SWA_NArticleView_DspList  = SW_FIRSTTAG,  // [SGI] dislay filters
      SWA_NArticleView_ToolBar,                 // [ GI] toolbar
      SWA_NArticleView_DspGlob,                 // [  I] global fitlers
      SWA_NArticleView_TitleTxt,                // [SGI] title text
      SWA_NArticleView_DspArt,                  // [SGI] last article
      SWA_NArticleView_DspGrp,                  // [SGI] last grp

      SWA_TextHide_Obj0,                        // [  I] one object
      SWA_TextHide_Obj1,                        // [  I] another object
      SWA_TextHide_DispObj,                     // [S I] which to display
      SWA_TextHide_UI,                          // [  I] add toggle button
      SWA_TextSel_Pressed,                      // [   ] for notifies
      SWA_TextCmd_Art,                          // [  I] article pointer
      SWA_TextCmd_Grp,                          // [  I] group pointer
      SWA_TextCmd_Type,                         // [  I] MM type
      SWA_TextCmd_Action,                       // [  I] MM action
      SWA_TextCmd_Var,                          // [  I] lookup variable
      SWA_TextCmd_Val,                          // [  I] lookup value
      SWA_TextScroll_Obj,                       // [  I] object to scroll
      SWA_TextScroll_IsGrp,                     // [  I] we're inserting a group
      SWA_TextScroll_TxtHeight,                 // [  I] height of text
      SWA_NDF_MimeBox_Data,                     // [  I] UUBox: Data
      SWA_NDF_MimeBox_DataSize,                 // [  I] UUBox: Data Size
      SWA_NDF_MimeBox_SaveDir,                  // [  I] save dir
      SWA_NDF_MimeBox_FName,                    // [  I] data filename
      SWA_NDF_MimeBox_Status,                   // [  I] uudecode status
      SWA_NDF_MimeBox_Grp,                      // [  I] group object
      SWA_NDF_MimeBox_Img,                      // [  I] image object
      SWA_NDF_MimeBox_Lines,                    // [  I] # lines object
      SWA_NDF_MimeBox_Art,                      // [  I] article object
      SWA_NDF_MimeBox_ArtView,                  // [  I] articleview object
      SWA_NDF_MimeBox_Type,                     // [  I] MIME type for this data
      SWA_NDF_MimeBox_Prompt,                   // [  I] T to use OK Requestors
      SWA_NDF_MimeBox_MakeDir,                  // [  I] T to make directories

      SWA_NArticleView_FirstBodyLine,           // [ G ] first body line
      SWA_NDF_MimeBox_DispSize,                 // [  I] MimeBox: display img size
      SWA_NArticleView_ArtFont,                 // [SGI] article font
      SWA_NArticleView_UseArtFont,              // [SGI] else, inherit from NNews
      SWA_NArticleView_ArtCol,                  // [SGI] article color
      SWA_NArticleView_UseArtCol,               // [SGI] else, inherit from NNews

      SWA_TextScroll_InitLine,                  // [  I] initial line at top
      SWA_TextSel_DoubleClick,                  // [ G ] set when double clicked
      SWA_TextCmd_NeedDouble,                   // [  I] only trigger on dclick
      SWA_TextCmd_UsePopMenu,                   // [  I] use a popup menu

      SWA_NArticleView_PendingDisp,             // [ G ] T if pending art display
      SWA_TextHide_Button,                      // [  I] specify toggle button
      SWA_TextHide_VLayout,                     // [  I] vertical layout

      SWA_TextCmd_Activated,                    // [ G ] pressed or doubleclicked
   };

   enum {
      SWM_NArticleView_Clear  = SW_FIRSTTAG,    // clear all lines
      SWM_NArticleView_Begin,                   // begin article
      SWM_NArticleView_NewLines,                // insert new lines
      SWM_NArticleView_End,                     // end article
      SWM_NArticleView_Abort,                   // aborted article
      SWM_NArticleView_Clip,                    // clip text
      SWM_NArticleView_SWin,                    // open search window
      SWM_NArticleView_Browse,                  // browse forward
      SWM_NArticleView_GenTxt,                  // run text by (all) filters
      SWM_NArticleView_GenObj,                  // generate obj from TxtBuff
      SWM_NArticleView_AddObj,                  // add object string
      SWM_NArticleView_SelectAll,               // select all text
      SWM_NArticleView_UnselectAll,             // unselect all text
      SWM_NArticleView_ArtFwd,                  // forward method to article
      SWM_NArticleView_Top,                     // go to top of article
      SWM_NArticleView_Bottom,                  // go to bottom of article
      SWM_NArticleView_TextHideClass,           // get texthide class
      SWM_NArticleView_TextSelClass,            // get texsel class
      SWM_NArticleView_TextCmdClass,            // get textcmd class
      SWM_NArticleView_TextScrollClass,         // get textscroll class
      SWM_NArticleView_MimeBoxClass,            // get mimebox class
      SWM_NArticleView_SaveData,                // save data block
      SWM_NArticleView_Scroll,                  // scroll up/dn
      SWM_NArticleView_MakeObjVis,              // make object visible
      SWM_NArticleView_Search,                  // search through article
      SWM_NArticleView_CtxMenuChoice,           // context menu choice
      SWM_NArticleView_DispRaw,                 // display raw article
      SWM_NArticleView_DispCooked,              // display cooked article
      SWM_NArticleView_GetDefFont,              // default font
      SWM_NArticleView_GetDefColor,             // default color
      SWM_NArticleView_ModTxt,                  // modify plain text
      SWM_NArticleView_DFwd,                    // forward to display filter
      SWM_NArticleView_SelChange,               // change in selection count
      SWM_NArticleView_SaveDataUI,              // save data block w/ prompt
   };

   enum {
      SWV_NArticleView_Browse_Move = 0,         // we can page
      SWV_NArticleView_Browse_End,              // we were at the bottom(top)
      SWV_NArticleView_Browse_NoArticle,        // there is no displayed article
      SWV_NArticleView_Browse_NoOp,             // no movement
   };

   enum {
      SWM_NDF_MimeBox_Save = SW_FIRSTTAG,       // save data to file
      SWM_NDF_MimeBox_Act,                      // action on data file
      SWM_NDF_TextCmd_MenuCall,                 // perform menu action
   };

   enum {
      SWV_NArticleView_SGroup = SW_FIRSTTAG,    // scroll group marker
   };

   enum {
      SWT_NArticleView_MT_Attr = SW_FIRSTTAG,   // Attr to check for in filter
      SWT_NArticleView_MT_TxBuff,               // text buff to modify
      SWT_NArticleView_MT_Unused,               //
      SWT_NArticleView_MT_Grp,                  // group
      SWT_NArticleView_MT_Art,                  // article
   };

   enum {
      SWT_NArticleView_SD_Win = SW_FIRSTTAG,    // window pointer
      SWT_NArticleView_SD_App,                  // App    pointer
      SWT_NArticleView_SD_Title,                // requestor title
      SWT_NArticleView_SD_InitDrawer,           // init drawer
      SWT_NArticleView_SD_InitFile,             // init filename
      SWT_NArticleView_SD_Data,                 // buffer to save
      SWT_NArticleView_SD_DataSize,             // buffer size in bytes
      SWT_NArticleView_SD_MakePath,             // make path
      SWT_NArticleView_SD_OWPrompt,             // overwrite prompt
      SWT_NArticleView_SD_SaveAs,               // prompt for save filename
      SWT_NArticleView_SD_MSBase,               // check for MS-DOS basename
      SWT_NArticleView_SD_CancPrompt,           // cancel prompt
      SWT_NArticleView_SD_ErrObj,               // error object
   };

#  define AV_AddOSize 16

#  define CT_IsAddObj(s) ((s)[0]==CT_Ctrl&&(s)[1]=='A'&&(s)[2]=='O'&&(s)[3]=='{')

#  define AV_Class(c) \
       NewObject((struct IClass *)DoMethod(msg->ArtView, \
         SWM_NArticleView_##c##Class), NULL

#  define AV_TextHideClass    AV_Class(TextHide)
#  define AV_TextCmdClass     AV_Class(TextCmd)
#  define AV_TextScrollClass  AV_Class(TextScroll)
#  define AV_MimeBoxObject    AV_Class(MimeBox)

   SWM_P(SWP_NArticleView_SelChange,     LONG Count);
   SWM_P(SWP_NArticleView_DispRaw,       Object *Art);
   SWM_P(SWP_NArticleView_DispCooked,    Object *Art);
   SWM_P(SWP_NArticleView_CtxMenuChoice, Object *item);

   SWM_P(SWP_NArticleView_Search, STRPTR Pat; ULONG Case; ULONG Len;
                                  ULONG Sub; ULONG Wild; ULONG First;
                                  STRPTR OrigStr);

   SWM_P(SWP_NArticleView_MakeObjVis, Object *o);
   SWM_P(SWP_NArticleView_SaveData, UBYTE *Data; ULONG DataSize;
                                    Object *ErrObj; STRPTR SaveFile);
   SWM_P(SWP_NArticleView_Browse,   LONG  Dir);
   SWM_P(SWP_NArticleView_AddObj,   STRPTR TmpBuf; void *Data; Object *NDF);
   SWM_P(SWP_NArticleView_GenObj,   TxtBuff *Tx; Object *Grp);
   SWM_P(SWP_NArticleView_Scroll,   LONG  Lines; LONG Cols);

   SWM_P(SWP_NArticleView_Begin,    // begin
         Object *Article;           // the article object
         Object *Group);            // the group object

   SWM_P(SWP_NArticleView_NewLines, // new lines from article have arrived
         Object *Article;           // the article object
         Object *Group;             // the group object
         STRPTR *Lines;             // the lines buffer
         LONG    FirstNew;          // first new line
         LONG    LastNew;           // last new lines
         LONG    LastHdr;           // last header
         LONG    EndOfData);        // end of data

   SWM_P(SWP_NArticleView_GenTxt,   // generate/modify text buffer
         TxtBuff *Tx;               // TxtBuff to modify
         ULONG   HasHeaders;        // buffer includes headers?
         Object *Art;               // article object
         Object *Grp;               // group object
         ULONG  *ParseComplete;     // parsing complete for these lines?
         ULONG   EndOfData;         // is this the last batch of lines?
         ULONG   Mthd);             // filter method

#  undef SW_FIRSTTAG

# endif
