# ifndef C_NTCP_INCLUDE_I
#  define C_NTCP_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NTCP.h"
#  include "Libraries/C-Timer.h"
#  include "Libraries/C-NNews.h"
#  include <sys/socket.h>
#  include <sys/ioctl.h>
#  include <sys/errno.h>
#  include <proto/socket.h>
#  include <proto/timer.h>
#  include <netdb.h>
#  include <netinet/ip.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NTCP
#  define SW_CNAME            "NTCP"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NTCP_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Host = 1,
      SWS_L_Port,

      SWS_L_Init,
      SWS_L_Connecting,
      SWS_L_Connected,
      SWS_L_Disconnected,
      SWS_L_Sending,
      SWS_L_Receiving,
      SWS_L_FindingHost,
      SWS_L_Error,
      SWS_L_NoNW,

      SWS_Err_SocketCreate,
      SWS_Err_ResolveHost,
      SWS_Err_ConnectFailed,
      SWS_Err_SendError,
      SWS_Err_RecvError,
      SWS_Err_Alloc,
      SWS_Err_CxTimeout,
      SWS_Err_Listen,
      SWS_Err_Bind,
      
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   enum {
      SWF_NTCP_RawBuffSize  = 4096,
      SWF_NTCP_LineBuffSize = 8192,
   };
   
#  define SocketBase (idata->SBase)

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      STRPTR             SWA_NTCP_Host;
      LONG               SWA_NTCP_Port;
      LONG               SWA_NTCP_LocAddr;  // for listen type connections
      LONG               SWA_NTCP_LocPort;
      LONG               SWA_NTCP_InitAddr;
      LONG               SWA_NTCP_Timeout;
      LONG               SWA_NTCP_SockID;
      Object            *SWA_NTCP_LineObject;
      LONG               SWA_NTCP_LineMethod;
      ULONG              SWA_NTCP_CmdState;
      ULONG              SWA_NTCP_SendSel;
      Object            *SWA_NTCP_SendSelObj;
      ULONG              SWA_NTCP_SendSelMthd;
      Object            *SWA_NTCP_ConnectObj;
      ULONG              SWA_NTCP_ConnectMthd;
      ULONG              SWA_NTCP_Connected;
      STRPTR             SWA_NTCP_Title;
      STRPTR             SWA_NTCP_Status;
      LONG               SWA_NTCP_SentBytes;
      LONG               SWA_NTCP_RecvBytes;
      LONG               SWA_NTCP_RecvLines;
      LONG               SWA_NTCP_NukeCR;
      LONG               SWA_NTCP_BPS;
      ULONG              SWA_NTCP_Mode;
      ULONG              SWA_NTCP_Listen;

      BPTR               DBRecv;

      LONG               OrigSockID;       // before ACCEPT
      LONG               Timeout;
      TEXT               cmsg[48];

      Object            *CxTimer;          // connection timer

      Library           *SBase;            // net library
      struct sockaddr_in srvaddr;          // address for socket (remote)
      struct sockaddr_in locaddr;          // address for socket (local)
      STRPTR             RawBuff;          // raw input buffer
      STRPTR             LineBuff;         // input line buffer
      LONG               InBytes;          // bytes in receive buffer
      LONG               RawPos;           // input buffer position
      LONG               LinePos;          // line buffer position
      LONG               LineBuffSize;     // line buffer size

      struct timeval     TVStart;
      struct timeval     TVPrev;           // last time stamp
   } SW_IDATA;

#  undef FD_ZERO
#  define FD_ZERO(p) memset((char *)(p), 0, sizeof(*(p)))

# endif // C_NTCP_INCLUDE_I
