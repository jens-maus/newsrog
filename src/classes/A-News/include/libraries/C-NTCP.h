# ifndef C_NTCP_INCLUDE
#  define C_NTCP_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include <sys/types.h>
#  include <sys/time.h>
#  include <devices/timer.h>

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10200)

   enum {
      SWA_NTCP_Host  = SW_FIRSTTAG,     // [SGI] server domain name
      SWA_NTCP_Port,                    // [SGI] port number
      SWA_NTCP_SockID,                  // [SGI] socket ID
      SWA_NTCP_Timeout,                 // [SGI] timeout value
      SWA_NTCP_LineObject,              // [SGI] object to pass lines to
      SWA_NTCP_LineMethod,              // [SGI] message for line object
      SWA_NTCP_CmdState,                // [SGI] for external status tracking
      SWA_NTCP_SendSel,                 // [SGI] should we select for send?
      SWA_NTCP_SendSelObj,              // [SGI] obj called upon send select
      SWA_NTCP_SendSelMthd,             // [SGI] mthd called upon send select
      SWA_NTCP_ConnectObj,              // [SGI] obj called upon send select
      SWA_NTCP_ConnectMthd,             // [SGI] mthd called upon send select
      SWA_NTCP_Connected,               // [SGI] T if connection established
      SWA_NTCP_Title,                   // [SGI] connection title
      SWA_NTCP_Status,                  // [SGI] connection status
      SWA_NTCP_SentBytes,               // [SGI] sent bytes
      SWA_NTCP_RecvBytes,               // [SGI] receive bytes
      SWA_NTCP_RecvLines,               // [SGI] receive lines
      SWA_NTCP_BPS,                     // [SGI] recent BPS average
      SWA_NTCP_NukeCR,                  // [SGI] remove CR`s from incoming lines
      SWA_NTCP_Mode,                    // [SGI] text or binary
      SWA_NTCP_Listen,                  // [SGI] TRUE if we should listen
      SWA_NTCP_LocPort,                 // [SGI] local port
      SWA_NTCP_LocAddr,                 // [SGI] local address
      SWA_NTCP_InitAddr,                // [  I] instead of hostname
   };

   enum {
      SWM_NTCP_Connect  = SW_FIRSTTAG,  // start connecting to server
      SWM_NTCP_ConnectDone,             // finished connecting
      SWM_NTCP_Disconnect,              // disconnect from server
      SWM_NTCP_Send,                    // send data to server
      SWM_NTCP_SendStr,                 // send ASCII string to server
      SWM_NTCP_RecvSync,                // receive line sychronously
      SWM_NTCP_RecvAsync,               // receive line asychronously
      SWM_NTCP_ResetStats,              // reset byte/line stats
      SWM_NTCP_XferDone,                // reset stats at end of a logical xfer
      SWM_NTCP_CxTimeout,               // connection timeout
      SWM_NTCP_Accept,                  // accept a connection
      SWM_NTCP_MaxSocket,               // max socket
      SWM_NTCP_RunningTime,             // get running time for this op
      SWM_NTCP_OkToSend,                // for sync sending
   };

   enum {
      SWV_NTCP_Mode_Text = 0,
      SWV_NTCP_Mode_Binary,
   };

   enum {
      SWV_NTCP_OK        = 0,           // returns from LineMethod
      SWV_NTCP_Disconnect,
      SWV_NTCP_Reconnect,
      SWV_NTCP_NoOp,
   };

   SWM_P(SWP_NTCP_RunningTime, struct timeval *Running);
   SWM_P(SWP_NTCP_Send,        UBYTE *Data; ULONG Length);
   SWM_P(SWP_NTCP_SendStr,    UBYTE *Data);
   SWM_P(SWP_NTCP_RecvSync,   UBYTE *Data; ULONG MaxLen);
   SWM_P(SWP_NTCP_RecvASync,  fd_set *rfds; fd_set *wfds);
   SWM_P(SWP_NTCP_AsyncLine,  UBYTE *Line; LONG  LineLen; Object *TCP);
   SWM_P(SWP_NTCP_Connected,  Object *TCP);
   SWM_P(SWP_NTCP_Disconnect, ULONG Delete);

   enum {
      SWF_NTCP_LineObjSelf  = -1,  // send line method to self

      SWF_NTCP_Disconnected = -1,  // disconnect message to line method
      SWF_NTCP_Deleted      = -2,  // deleted    message to line method
   };

#  undef SW_FIRSTTAG

# endif
