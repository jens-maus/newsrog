# ifndef C_NPOST_INCLUDE
#  define C_NPOST_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 13100)

   enum {
      SWA_NPost_Subj  = SW_FIRSTTAG,   // [SGI] post host name
      SWA_NPost_Grp,                   // [SGI] groups to post to
      SWA_NPost_Refs,                  // [SGI] references
      SWA_NPost_FUps,                  // [SGI] set followups
      SWA_NPost_MsgID,                 // [SGI] msg ID
      SWA_NPost_Lines,                 // [SGI] # of lines
      SWA_NPost_Bytes,                 // [SGI] bytes of text
      SWA_NPost_SigFile,               // [SGI] signature file
      SWA_NPost_Distrib,               // [SGI] distribution line
      SWA_NPost_Server,                // [SGI] server name to post to
      SWA_NPost_Mod,                   // [SGI] text modified
      SWA_NPost_XNewsRdr,              // [SGI] newsreader
      SWA_NPost_Org,                   // [SGI] organization
      SWA_NPost_From,                  // [SGI] from
      SWA_NPost_Pct,                   // [SGI] % complete
      SWA_NPost_PostMsgID,             // [SGI] MSG ID for postingn
      SWA_NPost_PostedOK,              // [SGI] TRUE if posted OK
      SWA_NPost_MimeVer,               // [SGI] MIME version header
      SWA_NPost_UseCustHdrs,           // [SGI] TRUE to use custom headers
      SWA_NPost_PostDir,               // [SGI] post subdirectory
      SWA_NPost_NoDel,                 // [SGI] don`t delete on list remove
      SWA_NPost_TagLine,               // [SGI] tagline
      SWA_NPost_TagLinePos,            // [SGI] tagline entry position
      SWA_NPost_NewPost,               // [SGI] this is a new post
      SWA_NPost_MimeCTE,               // [ G ] Content-Transfer-Encoding
      SWA_NPost_MimeType,              // [ G ] Content-Type
   };

   enum {
      SWM_NPost_Edit = SW_FIRSTTAG,    // edit post
      SWM_NPost_CloseEdit,             // close edit window
      SWM_NPost_IPost,                 // interactive post cmd
      SWM_NPost_IPost_End,             // end interactive post
      SWM_NPost_Post,                  // post the post to the server
      SWM_NPost_Post_P1,               //   post: after status
      SWM_NPost_Post_P2,               //   post: after terminating "."
      SWM_NPost_Delete,                // delete the post & its files
      SWM_NPost_Editor,                // launch editor
      SWM_NPost_FSpec,                 // generate fspec
      SWM_NPost_UNUSED5,               // UNUSED
      SWM_NPost_CopyFile,              // copy to cache file
      SWM_NPost_UpdDisp,               // update display from file
      SWM_NPost_UpdStats,              // update file stats
      SWM_NPost_UNUSED4,               // UNUSED
      SWM_NPost_AddText,               // add text to edit file
      SWM_NPost_Abort,                 // abort pending post
      SWM_NPost_IDel,                  // interactive delete
      SWM_NPost_GenReply,              // generate reply to article
      SWM_NPost_SaveEditData,          // save texteditor data to file
      SWM_NPost_SetStyle,              // set texteditor style
      SWM_NPost_UNUSED1,               // UNUSED
      SWM_NPost_UNUSED2,               // UNUSED
      SWM_NPost_MenuCall,              // menu call
      SWM_NPost_EditMode,              // change edit mode
      SWM_NPost_UNUSED3,               // UNUSED
      SWM_NPost_Rewrap,                // rewrap lines, return string
      SWM_NPost_WrapEditTxt,           // rewrap text in editor
      SWM_NPost_MoveFiles,             // move files to indicated directory
      SWM_NPost_Archive,               // archive post if needed
      SWM_NPost_GetTagLine,            // get tagline
      SWM_NPost_InsFile,               // insert a file`s contents
   };

   enum {
      SWT_NPost_CF_Dir = SW_FIRSTTAG,  // 0 = cache->tmp, 1 = tmp->cache
      SWT_NPost_CF_Del,                // TRUE to delete source after cp
      SWT_NPost_CF_Src,                // use alternate source (tmpfile)
      SWT_NPost_CF_Stats,              // update stats
      SWT_NPost_CF_ErrWin,             // error window
      SWT_NPost_CF_Trg,                // use alternate target (cache file)

      SWT_NPost_SrvList,               // server list object
      SWT_NPost_CmdObj,                //   cmd object
      SWT_NPost_CmdMethod,             //   cmd method
      SWT_NPost_CmdData,               //   cmd data

      SWT_NPost_IF_FSpec,              // file to insert
      SWT_NPost_IF_QPrefix,            // quote prefix
   };

   enum {
      SWF_NPost_State_End = 0,         // state: end
      SWF_NPost_State_Abort,           // state: abort
   };

   enum {
      SWF_NPost_Style_Off = 0,         // turn style on
      SWF_NPost_Style_On,              // turn style off
      SWF_NPost_Style_Toggle,          // toggle style
   };

   enum {
      SWF_NPost_FSpec_Perm = 0,         // permanent fspec
      SWF_NPost_FSpec_Temp,             // temp edit copy
      SWF_NPost_FSpec_Wrapped,          // wrapped version
   };

   enum {
      SWV_NPost_GTL_Random = -1,       // for SWM_NPost_GetTagLine
      SWV_NPost_GTL_Prev   = -2,
      SWV_NPost_GTL_Next   = -3,
   };

   SWM_P(SWP_NPost_GetTagLine,         // get tagline
         LONG Pos);                    // position

   SWM_P(SWP_NPost_MoveFiles,          // move post`s files to dir
         STRPTR Dest);                 // dest directory

   SWM_P(SWP_NPost_Rewrap,             // rewrap post text
         ULONG Cols);                  // mode

   SWM_P(SWP_NPost_EditMode,           // edit mode
         ULONG Mode);                  // mode

   SWM_P(SWP_NPost_Edit,               // edit post
         ULONG ReadOnly);              // read only?

   SWM_P(SWP_NPost_MenuCall,           // menu call
         ULONG Event);                 // menu event

   SWM_P(SWP_NPost_SaveEditData,       // save texteditor data to file
         STRPTR FSpec;                 // file to save to
         ULONG  SaveWrapped);          // saved wrapped version too?

   SWM_P(SWP_NPost_CmdMsg,             // command message
         ULONG State);                 // state

   SWM_P(SWP_NPost_CloseEdit,          // close edit window
         ULONG Force;                  // don`t ask questions.
         ULONG Save;                   // save changes?
         ULONG AskDel);                // ask whether to delete

   SWM_P(SWP_NPost_FSpec,              // generate filespec
         ULONG FType);                 // type

   SWM_P(SWP_NPost_IDel,               // delete this post
         ULONG Force);                 // don`t ask questions

   SWM_P(SWP_NPost_UpdDisp,            // update display
         STRPTR File);                 // from this file

   SWM_P(SWP_NPost_UpdStats,           // update statistics
         STRPTR File);                 // from this file

   SWM_P(SWP_NPost_AddText,            // add text to edit file
         STRPTR Text;                  // the text to add
         ULONG  Tmp);                  // to the temp file?

   SWM_P(SWP_NPost_GenReply,           // generate reply to article
         Object *Grp;                  // the group for the reply
         Object *Art;                  // the article to reply to
         Object *Win);                 // window for requestor

#  undef SW_FIRSTTAG   

# endif
