/*****************************************************************************
 * Function: Open debug files                                                *
 *****************************************************************************/

static void OpenDBFiles(SW_IDATA *idata)
{
   TEXT DBFSpec[64], DBVar[64], tmp[8];
   
   // -- handle debug stuff --------------------------------------------------
   
   if (GetVar("NewsRog/Debug", DBVar, sizeof(DBVar)-1, 0) > 0) {
      sprintf(tmp, "%02ld", idata->SWA_NTCP_SockID);
      
      if (strstr(DBVar, "TCPRECV") && !idata->DBRecv)
         idata->DBRecv = SWOpenAppend(strcat(strcpy(DBFSpec, "RAM:NR.TCPRECV."),
                                             tmp));
   }
}


/*****************************************************************************
 * Function: Pass data to object/method in SWA_NTCP_LineObject               *
 *****************************************************************************/

static ULONG DoLineMethod(Object *o, SW_IDATA *idata, STRPTR buff, LONG pos)
{
   Object *lo = idata->SWA_NTCP_LineObject == (Object *)SWF_NTCP_LineObjSelf ?
      o : idata->SWA_NTCP_LineObject;

   // -- pass line to object/method ------------------------------------------
   
   if (lo)
      return DoMethod(lo, idata->SWA_NTCP_LineMethod, buff, pos, o);

   return SWV_NTCP_OK;
}


/*****************************************************************************
 * Function: Check SocketBase & open if possible                             *
 *****************************************************************************/

static Library *OpenSocketLib(SW_IDATA *idata)
{
   if (!idata->SBase)
      idata->SBase = OpenLibrary("bsdsocket.library", 4);

   return idata->SBase;
}


/*****************************************************************************
 * Function: resolve host name to address                                    *
 *****************************************************************************/

static int getSockAddr(Object *o, SW_IDATA *idata)
{
  struct hostent *remote;

  set(o, SWA_NTCP_Status, GetS(SWS_L_FindingHost));

  if (!OpenSocketLib(idata)) return FALSE;

  if ((remote = gethostbyname(idata->SWA_NTCP_Host)) != NULL)
     memcpy(&idata->srvaddr.sin_addr, remote->h_addr,
            sizeof(idata->srvaddr.sin_addr));
  else if ((idata->srvaddr.sin_addr.s_addr = inet_addr(idata->SWA_NTCP_Host)) ==
           (unsigned long)-1) {
     return FALSE;
  }

  return TRUE;
}


/*****************************************************************************
 * Function: Find the local IP address                                       *
 *****************************************************************************/

static ULONG getLocalAddr(SW_IDATA *idata)
{
   long len = sizeof(idata->locaddr);

   if (getsockname(idata->SWA_NTCP_SockID, (struct sockaddr *)&idata->locaddr,
                   &len) < 0)
      return FALSE;

   idata->SWA_NTCP_LocPort   = idata->locaddr.sin_port;
   idata->SWA_NTCP_LocAddr   = idata->locaddr.sin_addr.s_addr;
   
   return TRUE;
}


/*****************************************************************************
 * Function: disconnect socket & return error                                *
 *****************************************************************************/

static ULONG DiscErr(Object *o, SW_IDATA *idata, ULONG ret, ULONG err,
                     ULONG p1, ULONG p2)
{
   DoMethod(o, SWM_NTCP_Disconnect, FALSE);
   set(o, SWA_NTCP_Status, GetS(SWS_L_Error));
   SWDB_RET(SWERR(o, ret, err, p1, p2));
}


/*****************************************************************************
 * Method: Establish socket connection to server (async)                     *
 *****************************************************************************/

METHOD(SWM_NTCP_Connect, Msg)
{
   const LONG ONE         = 1;
   const LONG MaxSendLine = 1024;
   LONG  sas = 1, ID;
   ULONG SrvType;

   // -- no-op if we`re already open -----------------------------------------
   
   if (!OpenSocketLib(idata)) {
      DoMethod(o, SWM_NTCP_Disconnect, FALSE);
      set(o, SWA_NTCP_Status, GetS(SWS_L_NoNW));
      SWDB_RET(-1);
   }

   // -- create a new socket -------------------------------------------------

   if ((ID = idata->SWA_NTCP_SockID) == -1) {
      set(o, SWA_NTCP_Status, GetS(SWS_L_Init));

      if ((ID = idata->SWA_NTCP_SockID = socket(AF_INET, SOCK_STREAM, 0)) < 0)
         SWDB_RET(DiscErr(o, idata, -1, SWS_Err_SocketCreate, NULL, NULL));

      SrvType = (idata->SWA_NTCP_Mode == SWV_NTCP_Mode_Text ?
                 IPTOS_LOWDELAY : IPTOS_THROUGHPUT);
      
      setsockopt(ID, SOL_SOCKET, SO_KEEPALIVE, &ONE, sizeof(ONE));
      setsockopt(ID, SOL_SOCKET, SO_REUSEADDR, &ONE, sizeof(ONE));
      setsockopt(ID, SOL_SOCKET, SO_SNDLOWAT,  &MaxSendLine, sizeof(MaxSendLine));
      setsockopt(ID, SOL_SOCKET, IP_TOS, &SrvType,   sizeof(SrvType));
      
      IoctlSocket(ID, FIONBIO, (STRPTR)&sas);

      idata->srvaddr.sin_port        = htons(idata->SWA_NTCP_Port);
      idata->srvaddr.sin_family      = AF_INET;
      idata->locaddr.sin_port        = htons(INADDR_ANY);
      idata->locaddr.sin_family      = AF_INET;
      idata->locaddr.sin_addr.s_addr = idata->SWA_NTCP_InitAddr;

      if (getSockAddr(o, idata) == FALSE)
         SWDB_RET(DiscErr(o, idata, -1, SWS_Err_ResolveHost,
                          (ULONG)idata->SWA_NTCP_Host, NULL));

      if (idata->SWA_NTCP_Listen) {
         if (bind(ID, (struct sockaddr *)&idata->locaddr,
                  sizeof(idata->locaddr)) < 0)
            SWDB_RET(DiscErr(o, idata, -1, SWS_Err_Bind, 0, 0));
            
         if (listen(ID, 1) < 0)
            SWDB_RET(DiscErr(o, idata, -1, SWS_Err_Listen,
                             (ULONG)idata->SWA_NTCP_Host, idata->SWA_NTCP_Port));

         if (getLocalAddr(idata) == FALSE)
            SWDB_RET(DiscErr(o, idata, -1, SWS_Err_ResolveHost,
                             (ULONG)idata->SWA_NTCP_Host, NULL));
      }

      idata->SWA_NTCP_Connected = FALSE;
   }

   // -- establish connection to host ----------------------------------------

   if (!idata->SWA_NTCP_Connected && !idata->SWA_NTCP_Listen) {
      DoMethod(o, SWM_NTCP_ResetStats);
   
      if (connect(ID, (struct sockaddr *)&idata->srvaddr,
                  sizeof(struct sockaddr_in)) < 0) {
         if (Errno() != EINPROGRESS)
            SWDB_RET(DiscErr(o, idata, -1, SWS_Err_ConnectFailed,
                             (ULONG)idata->SWA_NTCP_Host, idata->SWA_NTCP_Port));
      }

      idata->Timeout = idata->SWA_NTCP_Timeout / 1000;

      if (idata->SWA_AppBase_TopObj)
         DoMethod(idata->CxTimer, SWM_Timer_Install, o, NN_App(idata));
      
      idata->SWA_NTCP_SendSel     = TRUE;
      idata->SWA_NTCP_SendSelObj  = o;
      idata->SWA_NTCP_SendSelMthd = SWM_NTCP_ConnectDone;

      getLocalAddr(idata);
   }

   SWDB_RET(ID);
}  SDFT


/*****************************************************************************
 * Method: Connection has been established                                   *
 *****************************************************************************/

METHOD(SWM_NTCP_ConnectDone, Msg)
{
   OpenDBFiles(idata);

   if (idata->DBRecv)
      FPrintf(idata->DBRecv, "[Connected]\n");

   set(o, SWA_NTCP_Status,    GetS(SWS_L_Connected));
   idata->SWA_NTCP_Connected  = TRUE;

   DoMethod(idata->CxTimer, SWM_Timer_Remove);
   
   if (idata->SWA_NTCP_ConnectObj && idata->SWA_NTCP_ConnectMthd)
      DoMethod(idata->SWA_NTCP_ConnectObj, idata->SWA_NTCP_ConnectMthd, o);

   idata->SWA_NTCP_ConnectObj  = NULL;
   idata->SWA_NTCP_ConnectMthd = 0;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: init data buffers                                               *
 *****************************************************************************/

static void __inline ResetPos(SW_IDATA *idata)
{
   idata->RawPos    =
   idata->InBytes   =
   idata->LinePos   = 0;
}
   
static LONG __inline InitBuffs(Object *o, SW_IDATA *idata)
{
   if (!idata->RawBuff) {
      idata->RawBuff     = AllocVec(SWF_NTCP_RawBuffSize,  MEMF_ANY);
      ResetPos(idata);
   }
   
   if (!idata->LineBuff && 
       idata->SWA_NTCP_Mode == SWV_NTCP_Mode_Text)
      idata->LineBuff    = AllocVec(idata->LineBuffSize =
                                    SWF_NTCP_LineBuffSize, MEMF_ANY);

   return idata->RawBuff &&
      (idata->LineBuff || (idata->SWA_NTCP_Mode == SWV_NTCP_Mode_Binary));
}


/*****************************************************************************
 * Method: Delete socket connection to server                                *
 *****************************************************************************/

METHOD(SWM_NTCP_Disconnect, SWP_NTCP_Disconnect *)
{
   if (idata->DBRecv)
      FPrintf(idata->DBRecv, "[Disconnected]\n");

   if (OpenSocketLib(idata)) {
      if (idata->SWA_NTCP_SockID != -1) {
         shutdown(idata->SWA_NTCP_SockID, 2);
         CloseSocket(idata->SWA_NTCP_SockID);
      }
   }
      
   DoMethod(idata->CxTimer, SWM_Timer_Remove);

   DoLineMethod(o, idata, NULL,
                msg->Delete ? SWF_NTCP_Deleted : SWF_NTCP_Disconnected);

   idata->SWA_NTCP_SockID     = -1;
   idata->SWA_NTCP_Connected  = FALSE;
   idata->SWA_NTCP_LineObject = NULL;
   idata->SWA_NTCP_LineMethod = 0;

   ResetPos(idata);
   DoMethod(o, SWM_NTCP_ResetStats);

   SetAttrs(o,
            SWA_NTCP_Status, GetS(SWS_L_Disconnected),
            SWA_NTCP_Title,  "",
            TAG_DONE);

   CloseLibrary(idata->SBase);
   idata->SBase = NULL;

   SWDB_RET(0);
}  SDFT


/*****************************************************************************
 * Method: Send data to server                                               *
 *****************************************************************************/

METHOD(SWM_NTCP_Send, SWP_NTCP_Send *)
{
   STRPTR os = idata->SWA_NTCP_Status;
   ULONG  Sleeping = FALSE;
   Object *App = NN_App(idata);
   
   if (!msg->Data || idata->SWA_NTCP_SockID < 0) SWDB_RET(-1);

   if (!OpenSocketLib(idata)) {
      DoMethod(o, SWM_NTCP_Disconnect, FALSE);
      set(o, SWA_NTCP_Status, GetS(SWS_L_NoNW));
      SWDB_RET(-1);
   }

   set(o, SWA_NTCP_Status, GetS(SWS_L_Sending));

   // -- hang around until we can send the data ------------------------------
   
   while (!DoMethod(o, SWM_NTCP_OkToSend)) {
      if (!Sleeping) {
         set(App, MUIA_Application_Sleep, TRUE);
         Sleeping = TRUE;
      }
      Delay(6);
      DoMethod(App, MUIM_Application_CheckRefresh);
   }

   if (Sleeping)
      set(App, MUIA_Application_Sleep, FALSE);
   
   if (send(idata->SWA_NTCP_SockID, msg->Data, msg->Length, 0) != msg->Length) {
      DoMethod(o, SWM_NTCP_Disconnect, FALSE);
      set(o, SWA_NTCP_Status, GetS(SWS_L_Error));
      SWDB_RET(SWERR(o, -1, SWS_Err_SendError, idata->SWA_NTCP_Host,
                     idata->SWA_NTCP_Port));
   }

   SetAttrs(o, SWA_NTCP_Status, os,
            SWA_NTCP_SentBytes, idata->SWA_NTCP_SentBytes + msg->Length,
            TAG_DONE);

   SWDB_RET(0);
}  SDFT


/*****************************************************************************
 * Method: Send ASCII string to server                                       *
 *****************************************************************************/

METHOD(SWM_NTCP_SendStr, SWP_NTCP_SendStr *)
{
   if (SW_STRISNULL(msg->Data)) SWDB_RET(0);
   
   SWDB_RET(DoMethod(o, SWM_NTCP_Send, msg->Data, strlen(msg->Data)));
}  SDFT


/*****************************************************************************
 * Method: See if it's OK to send more data                                  *
 *****************************************************************************/

METHOD(SWM_NTCP_OkToSend, Msg)
{
   LONG s = idata->SWA_NTCP_SockID;
   fd_set wfds;

   if (s < 0) SWDB_RET(FALSE);
   
   FD_ZERO(&wfds);
   FD_SET(s, &wfds);

   SWDB_RET(WaitSelect(s+1, NULL, &wfds, NULL, NULL, NULL) == 1);
}  SDFT


/*****************************************************************************
 * Method: Receive ASCII lines from server                                   *
 *****************************************************************************/

METHOD(SWM_NTCP_RecvSync, SWP_NTCP_RecvSync *)
{
   // NOT IMPLEMENTED YET
   if (!OpenSocketLib(idata)) SWDB_RET(-1);
   SWDB_RET(0);
}  SDFT


/*****************************************************************************
 * Function: check for write select, do action                               *
 *****************************************************************************/

static void DoSendSel(SW_IDATA *idata, fd_set *wfds)
   SDST(ULONG)
   SWDB_OBJMSG(__FUNC__, o)
{
   if (idata->SWA_NTCP_SockID < 0                   || // need a socket...
       !wfds                                        || // and a descriptor set
       !FD_ISSET(idata->SWA_NTCP_SockID, wfds))        // and us selected
      return;

   if (idata->SWA_NTCP_SendSelObj && idata->SWA_NTCP_SendSelMthd)
      DoMethod(idata->SWA_NTCP_SendSelObj, idata->SWA_NTCP_SendSelMthd);

   idata->SWA_NTCP_SendSel     = 0;
   idata->SWA_NTCP_SendSelObj  = NULL;
   idata->SWA_NTCP_SendSelMthd = 0;
} SDFT


/*****************************************************************************
 * Function: Get start time for network op                                   *
 *****************************************************************************/

static ULONG GetStartTime(SW_IDATA *idata, struct timeval Now)
{
   if (idata->TVStart.tv_sec) return 0;

   idata->TVPrev  = idata->TVStart = Now;
   return 1;
}

   
/*****************************************************************************
 * Method: Compute running time so far for this operation                    *
 *****************************************************************************/

METHOD(SWM_NTCP_RunningTime, SWP_NTCP_RunningTime *)
{
   struct timeval Buff,
                 *Running = msg->Running ? msg->Running : &Buff;
   
   GetSysTime(Running);
   GetStartTime(idata, *Running);
   
   SubTime(Running,  &idata->TVStart);

   SWDB_RET(Running->tv_sec);
} SDFT


/*****************************************************************************
 * Method: Compute BPS values                                                *
 *****************************************************************************/

static void ComputeBPS(Object *o, SW_IDATA *idata)
{
   struct timeval TVDiff, TVDiff1;
   
   GetSysTime(&TVDiff);
   TVDiff1 = TVDiff;

   if (GetStartTime(idata, TVDiff))
      return;

   SubTime(&TVDiff1, &idata->TVPrev);   // find time difference last time

   if (!TVDiff1.tv_sec && TVDiff1.tv_micro < 500000)
      return;

   idata->TVPrev = TVDiff;              // keep track of last time

   SubTime(&TVDiff,  &idata->TVStart);  // find time difference to start

   idata->SWA_NTCP_BPS =
      idata->SWA_NTCP_RecvBytes * 10 /
         (TVDiff.tv_sec * 10 + (TVDiff.tv_micro / 100000) + 1);

   set(o, SWA_NTCP_RecvBytes, idata->SWA_NTCP_RecvBytes);
}


/*****************************************************************************
 * Method: Receive ASCII lines from server & call object method              *
 *****************************************************************************/

METHOD(SWM_NTCP_RecvAsync, SWP_NTCP_RecvASync *)
{
   ULONG LineCode = SWV_NTCP_OK;
   
   if (idata->SWA_NTCP_SockID < 0)  // don`t do anything if no socket desc.
      SWDB_RET(0);

   if (!OpenSocketLib(idata)) SWDB_RET(-1);

   DoSendSel(idata, msg->wfds);     // check for select on write

   // -- see if we got disconnected as part of the send ----------------------
      
   if (idata->SWA_NTCP_SockID   < 0) {
      if (idata->DBRecv)
         FPrintf(idata->DBRecv, "[Disconnected after select on send]\n");
      SWDB_RET(0);
   }
   
   // -- allocate memory for buffer ------------------------------------------

   if (!InitBuffs(o, idata)) {
      set(o, SWA_NTCP_Status, GetS(SWS_L_Error));
      SWDB_RET(SWERR(o, -1, SWS_Err_Alloc, 0, 0));
   }

   // -- read a new buffer of data if needed ---------------------------------
   
   if (idata->RawPos == idata->InBytes) {
      LONG Sock = idata->SWA_NTCP_SockID;

      if (idata->DBRecv)
         FPrintf(idata->DBRecv, "Buffer exhausted, doing recv...\n");
      
      // check to see if we have data available ------------------------------

      if (msg->rfds)
         if (!FD_ISSET(Sock, msg->rfds)) {
            if (idata->DBRecv)
               FPrintf(idata->DBRecv, "No select bit set, waiting...\n");

            SWDB_RET(0);
         }

      // -- receive the data -------------------------------------------------

      idata->SWA_NTCP_Status = GetS(SWS_L_Receiving);

      idata->RawPos = 0;
      idata->InBytes = recv(Sock, idata->RawBuff, SWF_NTCP_RawBuffSize, 0);

      if (idata->InBytes <= 0) {
         idata->InBytes = 0;

         if (idata->DBRecv)
            FPrintf(idata->DBRecv, "[recv returned bytes <0: disconnected].\n");

         DoMethod(o, SWM_NTCP_Disconnect, FALSE);
         set(o, SWA_NTCP_Status, GetS(SWS_L_Error));

         SWDB_RET(SWERR(o, -1, SWS_Err_RecvError, idata->SWA_NTCP_Host,
                        idata->SWA_NTCP_Port));
      } else {
         if (idata->DBRecv)
            FPrintf(idata->DBRecv, "Read %ld input bytes.\n", idata->InBytes);
      }

      idata->SWA_NTCP_RecvBytes += idata->InBytes;
      ComputeBPS(o, idata);
   } else {
      if (idata->DBRecv)
         FPrintf(idata->DBRecv, "Getting data from current buffer...\n");
   }

   // -- search through for line boundaries if in text mode ------------------

   if (idata->SWA_NTCP_Mode == SWV_NTCP_Mode_Text) {
      if (idata->DBRecv)
         FPrintf(idata->DBRecv, "Parsing buffer into lines...\n");

      // -- text mode --------------------------------------------------------
      while (idata->RawPos < idata->InBytes) {
         UBYTE c;
   
         if (idata->LinePos < idata->LineBuffSize) {
            c = idata->LineBuff[idata->LinePos++] = idata->RawBuff[idata->RawPos++];
   
            if (c == '\r' && idata->SWA_NTCP_NukeCR) {
                                  // nuke CR`s.  We only care about LFs.  CR`s are
               idata->LinePos--;  // evil, repulsive, and cause cancer in
               continue;          // labratory rats.
            }
         } else                   // if no more room in buffer, don`t even try
            c='\0', idata->RawPos++;
            
         if (c == '\n') {
            idata->LineBuff[--idata->LinePos] = '\0';
            
            if (idata->LinePos > 0 && idata->LineBuff[idata->LinePos-1] == 0x0D)
               idata->LineBuff[--idata->LinePos] = '\0';
   
            if (idata->DBRecv)
               FPrintf(idata->DBRecv, "Found EOL: %s\nProcessing...\n", idata->LineBuff);

            if ((LineCode = DoLineMethod(o, idata, idata->LineBuff, idata->LinePos))
                != SWV_NTCP_OK)
               break;

            if (idata->DBRecv)
               FPrintf(idata->DBRecv, "End processing.\n");
   
            ++idata->SWA_NTCP_RecvLines;
                        
            idata->LinePos = 0;
         }
      }
   } else {
      // -- binary mode - just send data over --------------------------------
      LineCode      = DoLineMethod(o, idata, idata->RawBuff, idata->InBytes);
      idata->RawPos = idata->InBytes;
   }

   if (idata->DBRecv)
      FPrintf(idata->DBRecv, "Done processessing buffer.\n");

   switch (LineCode) {
     case SWV_NTCP_OK:
      idata->SWA_NTCP_Status = GetS(SWS_L_Connected);
      break;
      
     case SWV_NTCP_Disconnect:
      DoMethod(o, SWM_NTCP_Disconnect, FALSE);
      break;

     case SWV_NTCP_Reconnect:  break;
   }
   
   SWDB_RET(0);
}  SDFT


/*****************************************************************************
 * Function: reset CPS values                                                *
 *****************************************************************************/

static void ResetCPS(Object *o, SW_IDATA *idata)
{
   idata->TVStart.tv_sec   = 
   idata->TVStart.tv_micro = 0;

   set(o, SWA_NTCP_BPS, 0);
}


/*****************************************************************************
 * Method: Reset connection statistics                                       *
 *****************************************************************************/

METHOD(SWM_NTCP_ResetStats, Msg)
{
   SetAttrs(o,
            SWA_NTCP_SentBytes, 0,
            SWA_NTCP_RecvBytes, 0,
            SWA_NTCP_RecvLines, 0,
            TAG_DONE);

   ResetCPS(o, idata);

   SWDB_RET(0);
}  SDFT


/*****************************************************************************
 * Method: Reset connection statistics                                       *
 *****************************************************************************/

METHOD(SWM_NTCP_XferDone, Msg)
{
   SetAttrs(o,
            SWA_NTCP_Status, GetS(SWS_L_Connected),
            SWA_NTCP_Title,  "",
            TAG_DONE);
   
   ResetCPS(o, idata);

   SWDB_RET(0);
}  SDFT


/*****************************************************************************
 * Method: connection timer timeout                                          *
 *****************************************************************************/

METHOD(SWM_NTCP_CxTimeout, Msg)
{
   if (idata->SWA_NTCP_SockID < 0) SWDB_RET(0);

   if (--idata->Timeout) {
      sprintf(idata->cmsg, GetS(SWS_L_Connecting), idata->Timeout);
   
      set(o, SWA_NTCP_Status, idata->cmsg);
      SWDB_RET(SUCCESS);
   }

   DoMethod(idata->CxTimer, SWM_Timer_Remove);

   SWERR(o, NULL, SWS_Err_CxTimeout,idata->SWA_NTCP_Host,idata->SWA_NTCP_Port);

   DoMethod(NN_App(idata), MUIM_Application_PushMethod, o, 2,
            SWM_NTCP_Disconnect, FALSE);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: accept a connection                                               *
 *****************************************************************************/

METHOD(SWM_NTCP_Accept, Msg)
{
   struct sockaddr_in SrvAddr;
   LONG   len = sizeof(SrvAddr),
          OldSockID = idata->SWA_NTCP_SockID;

   if (!OpenSocketLib(idata)) SWDB_RET(FAILURE);

   if (!idata->SWA_NTCP_Listen || OldSockID < 0)
      SWDB_RET(FAILURE);

   idata->SWA_NTCP_SockID=accept(OldSockID,(struct sockaddr *)&SrvAddr,&len);

   if (idata->SWA_NTCP_SockID >= 0) {
      idata->OrigSockID = OldSockID;
      shutdown(OldSockID, 2);
      CloseSocket(OldSockID);
      SWDB_RET(SUCCESS);
   }

   SWDB_RET(FAILURE);
} SDFT


/*****************************************************************************
 * Method: return max socket number for this TCP object                      *
 *****************************************************************************/

METHOD(SWM_NTCP_MaxSocket, Msg)
{
   SWDB_RET(idata->SWA_NTCP_Listen && (idata->OrigSockID < 0) ? -1 :
            idata->SWA_NTCP_SockID);
} SDFT
