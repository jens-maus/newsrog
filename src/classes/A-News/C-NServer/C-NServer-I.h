# ifndef C_NSERVER_INCLUDE_I
#  define C_NSERVER_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NServer.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-Timer.h"
#  include "Libraries/S-Lists.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NServer
#  define SW_CNAME            "NServer"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NServer_protos.h"
#  include "Libraries/C-NTCPList.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_S_WaitForTCP = 1,
      SWS_S_WaitForNNTP,
      SWS_S_WaitForFTP,
      SWS_S_WaitForCmd,
      SWS_S_WaitRetry,
      SWS_S_Auth,
      SWS_S_FTPLogin,

      SWS_T_FetchDir,
      SWS_T_FetchFile,
      SWS_T_FTPData,

      SWS_Err_Offline,
      SWS_Err_AuthFailed,
      SWS_Err_NoAuthUser,
      SWS_Err_NoAuthPass,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   enum {
      SWV_NServer_MaxCmd     = 32,      // max simulataneous commands
      SWV_NServer_MaxFTPCmd  =  8,      // max simulataneous commands
      SWF_NServer_NewConnect = (1<<30), // new connection flag
      SWF_NServer_1stLine    = (1<<29), // 1st line in new data set
      SWF_NServer_IsDone     = (1<<28), // explicitly marked as done.
      SWF_NServer_NewCmd     = (1<<27), // new command
      SWF_NServer_Retrying   = (1<<26), // retrying
      SWF_NServer_SentUser   = (1<<25), // sent AUTHINFO user
      SWF_NServer_SentPass   = (1<<24), // sent AUTHINFO pass
      SWF_NServer_SentPort   = (1<<23), // sent FTP port command
         
      SWV_NServer_Quanta   = 50,   // increase line array in this increment
      SWV_NServer_NNTPPort = 119,  // NNTP TCP port number
      SWV_NServer_TCPUserPort = 21,
      SWV_NServer_TCPDataPort = 20,
   };

#  define SWF_NServer_EscDot ".."  // ".." -> "."

#  define IsNum(c) ((c)>='0'&&(c)<='9')
#  define NNTP_IsEnd(x) (c->Type == SWV_NServer_Type_NNTP && \
                         (x)[0] == '.' && (x)[1] == '\0')
#  define FTP_IsEnd(x)  (c->Type == SWV_NServer_Type_FTP && \
                         IsNum((x)[0]) && IsNum((x)[1]) &&IsNum((x)[2]) && \
                         x[3]==' ')

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      STRPTR        SWA_NServer_Host;
      ULONG         SWA_NServer_ConnectT;
      ULONG         SWA_NServer_ConnectS;
      ULONG         SWA_NServer_AvgCPS;
      ULONG         SWA_NServer_CnDays;
      ULONG         SWA_NServer_CnMin;
      ULONG         SWA_NServer_CnTick;
      ULONG         SWA_NServer_NGroups;
      APTR         *SWA_NServer_MemPool;
      ULONG         SWA_NServer_Default;
      ULONG         SWA_NServer_MaxRetries;
      ULONG         SWA_NServer_CxTimeout;
      ULONG         SWA_NServer_RetryDelay;
      ULONG         SWA_NServer_PostOK;
      STRPTR        SWA_NServer_AuthUser;
      STRPTR        SWA_NServer_AuthPass;
      ULONG         SWA_NServer_Type;
      ULONG         SWA_NServer_Port;
      BPTR          DBSend, DBRecv;
      
      struct TCPCmd {
         Object     *TCP;         // tcp connection object
         STRPTR     *Lines;       // input line array buffer
         ULONG       LineCount;   // count of used lines in buffer
         ULONG       MaxLine;     // # of allocated lines in array buffer
         ULONG       LineLimit;   // limit for line array size
         ULONG       Status;      // status of this connection
         ULONG       Interval;    // interval line count for status reports
         ULONG       DataCount;   // count of data items
         LONG        FirstNew;    // 1st new line since last status report
         Object     *CmdObj;      // object to send progress msgs to
         ULONG       CmdMethod;   // method to send progress msgs to
         ULONG       Rescode;     // result code for command
         ULONG       Flags;       // flags
         STRPTR      Title;       // title (txt status) for TCP
         ULONG       Data;        // user data to send
         ULONG       RetryCnt;    // connect retry count
         ULONG       RetryTime;   // retry time left
         Object     *RetryTimer;  // retry timer
         STRPTR      SendQueue;   // deferred send text if needed for resend
         ULONG       SendQBuffLen;// buffer size of send queue
         ULONG       Type;        // NNTP or TCP
      } Cmd[SWV_NServer_MaxCmd];

      struct FTPCmd {
         Object     *CmdObj;       // object to send progress msgs to
         ULONG       CmdMethod;    // method to send progress msgs to
         ULONG       CmdData;      // user data to send
         ULONG       Cmd;          // TCP Cmd
         Object     *DataCnx;      // data connection (FROM server TO us)
         ULONG       Connected;    // T if connected
         ULONG       State;        // state machine state
         STRPTR      Str1;         // string1 for additional cmd
         UBYTE      *DataBuff;     // buffer data in RAM
         ULONG       DataBuffSize; // data buffer size
         ULONG       DataBuffPos;  // data buffer pos
         ULONG       FTPType;      // FTP type: ASCII/Image/etc
      } FTPCmd[SWV_NServer_MaxFTPCmd];
   } SW_IDATA;

#  define IDInRange(x) ((x) >= 0 && (x) < SWV_NServer_MaxCmd)

#  define FV(x)   SWPoolFV(idata->SWA_NServer_MemPool, (UBYTE *)(x))
#  define AV(x)   SWPoolAV(idata->SWA_NServer_MemPool, (ULONG)(x))
#  define RV(s,m) SWPoolRV(idata->SWA_NServer_MemPool, (s), (STRPTR *)(m))
#  define SD(x)   SWPoolStrDup(idata->SWA_NServer_MemPool, (x))
#  define UC(x) (int) (((int) x) & 0xff)

#  define IsWhite(c) ((c)==' '||(c)=='\t')

# endif // C_NSERVER_INCLUDE_I
