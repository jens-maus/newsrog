# ifndef C_NSERVER_INCLUDE
#  define C_NSERVER_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10100)

   enum {
      SWA_NServer_Host  = SW_FIRSTTAG,   // [SGI] server host name
      SWA_NServer_UNUSED1,               // unused
      SWA_NServer_ConnectT,              // [SGI] connect attempts
      SWA_NServer_ConnectS,              // [SGI] connect successes
      SWA_NServer_AvgCPS,                // [SGI] average CPS for server
      SWA_NServer_CnDays,                // [SGI] last connection DateStamp
      SWA_NServer_CnMin,                 // [SGI] last connection DateStamp
      SWA_NServer_CnTick,                // [SGI] last connection DateStamp
      SWA_NServer_NGroups,               // [SGI] known # of groups on server
      SWA_NServer_MemPool,               // [ GI] pool for reading lines
      SWA_NServer_Default,               // [SGI] the default server
      SWA_NServer_MaxRetries,            // [SGI] max connect retries
      SWA_NServer_RetryDelay,            // [SGI] retry delay
      SWA_NServer_CxTimeout,             // [SGI] connection timeout
      SWA_NServer_PostOK,                // [SGI] server says OK to post?
      SWA_NServer_AuthUser,              // [SGI] AUTHINFO user
      SWA_NServer_AuthPass,              // [SGI] AUTHINFO PASS
      SWA_NServer_Type,                  // [SGI] NNTP or FTP
      SWA_NServer_Port,                  // [SGI] port to connect to
   };

   enum {
      SWM_NServer_SendCmd = SW_FIRSTTAG, // send command to server
      SWM_NServer_RecvLine,              // receive incoming data
      SWM_NServer_DelLines,              // delete some lines from buffer
      SWM_NServer_AdoptLines,            // adopt lines (nserver won`t free)
      SWM_NServer_Abort,                 // abort receiving data
      SWM_NServer_DelCmd,                // delete TCP connection for cmd
      SWM_NServer_ResetStats,            // reset statistics
      SWM_NServer_Connect,               // connect to server
      SWM_NServer_Connected,             // method upon successful connect
      SWM_NServer_Done,                  // data stream is done
      SWM_NServer_CxRetry,               // retry connection
      SWM_NServer_CxCancel,              // cancel connection retry
      SWM_NServer_GetTCP,                // find TCP object for a connection
      SWM_NServer_RecvBin,               // receive binary data (FTP)
      SWM_NServer_GetFTPDir,             // retrieve FTP dir listing
      SWM_NServer_GetFTPDir_P1,          //   internal
      SWM_NServer_GetFTPFile,            // retrieve FTP file
      SWM_NServer_GetFTPFile_P1,         //   internal
      SWM_NServer_AbortFTP,              // Abort FTP command
   };

   enum {
      SWT_NServer_Type_NNTP = 0,
      SWT_NServer_Type_FTP,
   };

   enum {
      SWT_NServer_Cmd     = SW_FIRSTTAG, // tags for SWM_NServer_SendCmd: cmd
      SWT_NServer_LineLimit,             // max lines to read from server (opt)
      SWT_NServer_EstLines,              // estimated # of result lines (opt)
      SWT_NServer_Interval,              // call CmdObj every N lines (opt)
      SWT_NServer_CmdObj,                // call a method in this object (opt)
      SWT_NServer_CmdMethod,             // call this method (opt)
      SWT_NServer_Flags,                 // flags (below)
      SWT_NServer_Title,                 // title for command
      SWT_NServer_DataCount,             // count of data items
      SWT_NServer_ID,                    // ID, to re-use same connection
      SWT_NServer_Data,                  // user data
      SWT_NServer_Type,                  // NNTP or FTP
      SWT_NServer_FTPCmdObj,             // DELTE THIS
      SWT_NServer_FTPCmdMethod,          // DELETE THIS
      SWT_NServer_FTPData,               // call this method (opt)
      SWT_NServer_FTPDir,                // FTP directory
      SWT_NServer_FTPFile,               // FTP file
      SWT_NServer_FTPUser,               // FTP user
      SWT_NServer_FTPPass,               // FTP password
      SWT_NServer_FTPType,               // FTP type: ascii/image/etc
   };

   enum {
      SWV_NServer_FTPType_ASCII = 0,
      SWV_NServer_FTPType_Image,
   };

   enum {
      SWF_NServer_NumMode     = (1<<0),
      SWF_NServer_SkipSpc     = (1<<1),  // skip initial space in line
      SWF_NServer_ExplicitDone= (1<<2),  // target object will msg when done
      SWF_NServer_NoLineLimit = (1<<30), // read as many lines as you want.
      SWF_NServer_ItvNone     = (1<<30), // don`t send reports until done
   };

   enum {                                // command status:
      SWV_NServer_Idle     = 0,          //    idle
      SWV_NServer_WaitCn,                //    waiting for connection
      SWV_NServer_Begin,                 //    before running command
      SWV_NServer_RunCmd,                //    while running command
      SWV_NServer_Rescode,               //    got rescode - check please
      SWV_NServer_NextItem,              //    next data set
      SWV_NServer_Done,                  //    normal finish
      SWV_NServer_Abort,                 //    abnormal abort

      SWV_NServer_RescodeOK,             // return these after checking
      SWV_NServer_RescodeMore,           // another rescode coming
      SWV_NServer_RescodeBad,            // bad rescode

      SWV_NServer_Connected,             // established connection
      SWV_NServer_BadFTPDir,             // no FTP directory
   };

   enum {
      SWV_NServer_MaxCmdLen = 512,
   };

   enum {
      SWV_NServer_Type_NNTP = 0,
      SWV_NServer_Type_FTP,
   };

   SWM_P(SWP_NServer_FTPStatMsg,         // -- FTP status message
         LONG     Status;                // status
         LONG     ID;                    // ID for cancel/etc
         STRPTR   Buff;                  // result buffer
         LONG     BuffSize;              // size of result buffer
         Object  *NServer;               // server object, and
         ULONG    Rescode;               // Rescode from command
         ULONG    Data);                 // user data

   SWM_P(SWP_NServer_StatusMsg,          // -- status msgs during async cmds -
         UBYTE  **LineBuff;              // received line buffer
         LONG     Status;                // status of command
         LONG     FirstNew;              // first new line (inclusive)
         LONG     LastNew;               // last new line  (inclusive)
         Object  *NServer;               // server object, and
         LONG     ID;                    // ID for cancel/delete lines/etc.
         ULONG    Rescode;               // Rescode from command
         ULONG    Data);                 // any data you want

   SWM_P(SWP_NServer_DelLines,           // -- delete lines from buffer ------
         UBYTE  **LineBuff;              // line buffer
         LONG     FirstLine;             // first line to delete (inclusive)
         LONG     LastLine);             // lastline to delete (inclusive)

   SWM_P(SWP_NServer_AdoptLines,         // -- adopt lines - don`t free ------
         UBYTE  **LineBuff;              // line buffer
         LONG     FirstLine;             // first line to adopt (inclusive)
         LONG     LastLine;              // lastline to adopt (inclusive)
         UBYTE  **TargetBuff;            // target buffer to copy to, or NULL
         LONG     FixHdrs);              // TRUE to concat header lines

   SWM_P(SWP_NServer_Abort,              // -- abort current command ---------
         LONG     ID);                   // ID (offset in array)

   SWM_P(SWP_NServer_AbortFTP,           // -- abort FTP command -------------
         LONG     ID);                   // ID (offset in array)

   SWM_P(SWP_NServer_DelCmd,             // -- delete current command --------
         LONG     ID);                   // ID (offset in array)

   SWM_P(SWP_NServer_Connect,            // -- connect to server -------------
         LONG     ID);                   // ID (offset in array)

   SWM_P(SWP_NServer_Done,               // -- signal end of logical xfer ----
         LONG     ID);                   // ID (offset in array)

   SWM_P(SWP_NServer_GetTCP,             // -- get TCP object for an ID ------
         LONG     ID);                   // ID (offset in array)

   // -- result codes from NNTP server ---------------------------------------

   enum {
      SWV_NSR_SrvReadyPost    = 200,
      SWV_NSR_SrvReadyNoPost  = 201,
      SWV_NSR_QuitClosing     = 205,
      SWV_NSR_Group           = 211,
      SWV_NSR_List            = 215,
      SWV_NSR_Article         = 220,
      SWV_NSR_Head            = 221,
      SWV_NSR_Body            = 222,
      SWV_NSR_XOverData       = 224,
      SWV_NSR_NewGroups       = 231,
      SWV_NSR_PostedOK        = 240,
      SWV_NSR_AuthAccepted    = 250,
      SWV_NSR_AuthAccepted2   = 281,  // obsolete rescode

      SWV_NSR_OKToPost        = 340,
      SWV_NSR_ContinueAuth    = 350,
      SWV_NSR_AuthRequired2   = 381,  // obsolete rescode

      SWV_NSR_ServiceDiscont  = 400,
      SWV_NSR_NoSuchGroup     = 411,
      SWV_NSR_NoGrpSelected   = 412,
      SWV_NSR_NoArtSelected   = 420,
      SWV_NSR_NoArtNum        = 423,
      SWV_NSR_NoSuchArt       = 430,
      SWV_NSR_NoPosting       = 440,
      SWV_NSR_PostingFailed   = 441,
      SWV_NSR_AuthRequired    = 450,
      SWV_NSR_AuthRejected    = 452,
      SWV_NSR_AuthRequired3   = 480,  // obsolete rescode
      SWV_NSR_AuthRejected2   = 482,  // obsolete rescode

      SWV_NSR_UnknownCmd      = 500,
      SWV_NSR_SyntaxErr       = 501,
      SWV_NSR_AccessDenied    = 502,
      SWV_NSR_CmdNotPerformed = 503,
   };

   // -- result codes from FTP server ---------------------------------------

   enum {
      SWV_NSR_FTP_StartingXFer1 = 125,  // data connection already open
      SWV_NSR_FTP_StartingXFer2 = 150,  // new data connection
      SWV_NSR_FTP_CommandOK     = 200,  // PORT or other command OK
      SWV_NSR_FTP_ConnectOK     = 220,
      SWV_NSR_FTP_CloseDataOK   = 226,
      SWV_NSR_FTP_LoggedIn      = 230,
      SWV_NSR_FTP_CmdSuccess    = 250,
      SWV_NSR_FTP_NeedPass      = 331,
   };

#  define NSR_ResCodeLine(msg,pos) (atol((msg)->LineBuff[pos]))
#  define NSR_ResCode(msg)         ((msg)->Rescode)
#  define NSR_Line(msg,pos)        ((msg)->LineBuff[pos])
#  define NSR_Lines(msg)           ((msg)->LineBuff)
#  define NSR_First(msg)           ((msg)->FirstNew)
#  define NSR_Last(msg)            ((msg)->LastNew)
#  define NSR_Count(msg)           (NSR_Last(msg)-NSR_First(msg)+1)
#  define NSR_Size(msg)            (NSR_Count(msg)*sizeof(ULONG))
#  define NSR_Srv(msg)             ((msg)->NServer)
#  define NSR_ID(msg)              ((msg)->ID)
#  define NSR_Status(msg)          ((msg)->Status)
#  define NSR_Data(msg)            ((msg)->Data)
#  define NSR_Buff(msg)            ((msg)->Buff)
#  define NSR_BuffSize(msg)        ((msg)->BuffSize)
#  define NSR_IsStatus(msg,s)      (NSR_Status(msg)==(s))
#  define NSR_SDone(msg)           NSR_IsStatus(msg, SWV_NServer_Done)
#  define NSR_SRescode(msg)        NSR_IsStatus(msg, SWV_NServer_Rescode)
#  define NSR_SBegin(msg)          NSR_IsStatus(msg, SWV_NServer_Begin)
#  define NSR_SConnect(msg)        NSR_IsStatus(msg, SWV_NServer_Connected)
#  define NSR_SRun(msg)            NSR_IsStatus(msg, SWV_NServer_RunCmd)
#  define NSR_SAbort(msg)          NSR_IsStatus(msg, SWV_NServer_Abort)
#  define NSR_SNext(msg)           NSR_IsStatus(msg, SWV_NServer_NextItem)
#  define NSR_SBad(msg)            NSR_IsStatus(msg, SWV_NServer_RescodeBad)
#  define NSR_SAnyEnd(msg)         (NSR_SDone(msg)||NSR_SAbort(msg)||NSR_SBad(msg))
#  define NSR_SBadFTPDir(msg)       NSR_IsStatus(msg, SWV_NServer_BadFTPDir)

#  define NSR_Adopt(msg,f,l,d,hd)  DoMethod(NSR_Srv(msg), SWM_NServer_AdoptLines, \
                                            NSR_Lines(msg), (f), (l), (d), (hd))

#  define SWV_NServer_EOL     "\r\n"
#  define SWV_NServer_PostCmd "POST" SWV_NServer_EOL
#  define SWV_NServer_EndCmd  "."    SWV_NServer_EOL

   // TBD - need queue mechanism if we run out of cmd TCP sockets

#  undef SW_FIRSTTAG   

# endif
