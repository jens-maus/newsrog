# include "C-NTCP-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NTCP

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_NTCP_SockID,     SWAM_NoLab,   SWAM_SGIN, -1),
   SWAM_SA(SWA_NTCP_Host,       SWS_L_Host,   SWAM_UI,   NULL, 256),
   SWAM_IB(SWA_NTCP_Port,       SWS_L_Port,   SWAM_UI,   119, 1, 65535),
   SWAM_IB(SWA_NTCP_Timeout,    SWAM_NoLab,   SWAM_SGI,  30*1000, 1, 1000*60*24),
   SWAM_PT(SWA_NTCP_LineObject, SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_IN(SWA_NTCP_LineMethod, SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_IN(SWA_NTCP_CmdState,   SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_IN(SWA_NTCP_SendSel,    SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_PT(SWA_NTCP_SendSelObj, SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_IN(SWA_NTCP_SendSelMthd,SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_PT(SWA_NTCP_ConnectObj, SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_IN(SWA_NTCP_ConnectMthd,SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_IN(SWA_NTCP_Connected,  SWAM_NoLab,   SWAM_SGIN, FALSE),
   SWAM_SA(SWA_NTCP_Title,      SWAM_NoLab,   SWAM_SGIN, NULL, 256),
   SWAM_PT(SWA_NTCP_Status,     SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_IN(SWA_NTCP_SentBytes,  SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_IN(SWA_NTCP_RecvBytes,  SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_IN(SWA_NTCP_RecvLines,  SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_IN(SWA_NTCP_BPS,        SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_BO(SWA_NTCP_NukeCR,     SWAM_NoLab,   SWAM_SGIN, TRUE),
   SWAM_IN(SWA_NTCP_Mode,       SWAM_NoLab,   SWAM_SGIN, SWV_NTCP_Mode_Text),
   SWAM_BO(SWA_NTCP_Listen,     SWAM_NoLab,   SWAM_SGIN, FALSE),
   SWAM_IB(SWA_NTCP_LocPort,    SWAM_NoLab,   SWAM_SGIN, INADDR_ANY, 0, 65535),
   SWAM_IN(SWA_NTCP_LocAddr,    SWAM_NoLab,   SWAM_SGIN, 0),
   SWAM_IN(SWA_NTCP_InitAddr,   SWAM_NoLab,   SWAM_SGIN, 0),

   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Address",
   "Port",

   "Initializing...",
   "Connecting (%ld)...",
   "Connected",
   "Disconnected",
   "Sending...",
   "Receiving...",
   "Finding Host...",
   "Error",
   "No Network",

   SWX_B("Error: ") "Unable to create socket.",
   SWX_B("Error: ") "Unable to resolve host address\n(Host: %s)",
   SWX_B("Error: ") "Connect failed to host.\n(Host: %s Port: %ld)",
   SWX_B("Error: ") "Socket send error.\n(Host: %s Port: %ld)",
   SWX_B("Error: ") "Socket recv error.\n(Host: %s Port: %ld)",
   SWX_B("Error: ") "Out of memory.",
   SWX_B("Error: ") "Connection timed out.\n(Host: %s Port: %ld)",
   SWX_B("Error: ") "listen() failed\n(Host: %s Port: %ld)",
   SWX_B("Error: ") "Unable to bind to port",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

struct timerequest  *TimerIO   = NULL;
struct Library      *TimerBase = NULL;

static ULONG InitLib(void)
{
   if (!(TimerIO = (struct timerequest *) AllocVec(sizeof(struct timerequest),
                                                   MEMF_ANY|MEMF_CLEAR)))
      return FAILURE;
   
   if (OpenDevice(TIMERNAME, UNIT_MICROHZ, TimerIO, 0)) {
      FreeVec(TimerIO);
      return FAILURE;
   }

   TimerBase = (Library *)TimerIO->tr_node.io_Device;

   SWDB_RET(SUCCESS);
}

static ULONG SetupObj(Object *o, SW_IDATA *idata)
{
   if (!(idata->CxTimer = 
         MUI_NewObject(SWC_Timer,
                       SWA_Timer_Millis,  1000,
                       SWA_Timer_Method,  SWM_NTCP_CxTimeout,
                       TAG_DONE)))
      return FAILURE;

   idata->OrigSockID = -1;
   
   return SUCCESS;
}

# define SW_USERSETUP    if (InitLib() != SUCCESS) SWDB_RET(NULL);

# define SW_USERCLEANUP  CloseDevice((struct IORequest *)TimerIO);          \
                         FreeVec(TimerIO);                                  \


# define SW_SETUP_POSTSET if (SetupObj((Object *)retval, idata) != SUCCESS) \
                             SWDB_RET(NULL);

# define DISPOSECUST     DoMethod(o, SWM_NTCP_Disconnect, TRUE);  \
                         MUI_DisposeObject(idata->CxTimer);       \
                         FreeVec(idata->RawBuff);                 \
                         FreeVec(idata->LineBuff);                \
                         if (idata->DBRecv) Close(idata->DBRecv); \


# define SW_Methods               \   
  SW_DO_MF(SWM_NTCP_MaxSocket);   \
  SW_DO_MF(SWM_NTCP_RecvAsync);   \
  SW_DO_MF(SWM_NTCP_Connect);     \
  SW_DO_MF(SWM_NTCP_ConnectDone); \
  SW_DO_MF(SWM_NTCP_Disconnect);  \
  SW_DO_MF(SWM_NTCP_Send);        \
  SW_DO_MF(SWM_NTCP_SendStr);     \
  SW_DO_MF(SWM_NTCP_RecvSync);    \
  SW_DO_MF(SWM_NTCP_ResetStats);  \
  SW_DO_MF(SWM_NTCP_XferDone);    \
  SW_DO_MF(SWM_NTCP_CxTimeout);   \
  SW_DO_MF(SWM_NTCP_Accept);      \
  SW_DO_MF(SWM_NTCP_RunningTime); \
  SW_DO_MF(SWM_NTCP_OkToSend);    \

# include "C-NTCP-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
