# ifndef _GENPROTO

/*****************************************************************************
 * Function: send message to target object                                   *
 *****************************************************************************/

static ULONG SendTgtMsg(Object *o, SW_IDATA *idata, LONG cmd, LONG Status)
{
   struct TCPCmd *c = &idata->Cmd[cmd];
   
   if (c->CmdObj && c->CmdMethod)
      return DoMethod(c->CmdObj, c->CmdMethod, c->Lines, Status, c->FirstNew,
                      c->LineCount, o, cmd, c->Rescode, c->Data);

   return 0;
}


/*****************************************************************************
 * Function: find idle command connection                                    *
 *   TData = 0 for TCP user connection                                       *
 *   TData = 1 for TCP data connection                                       *
 *****************************************************************************/

static Object *CmdMatch(SW_IDATA *idata, ULONG x, ULONG Port, ULONG Mode)
{
   Object *tcp = idata->Cmd[x].TCP;

   if (!tcp) return NULL;

   return idata->Cmd[x].Status        == SWV_NServer_Idle &&
          SWGetUL(tcp, SWA_NTCP_Port) == Port             &&
          SWGetUL(tcp, SWA_NTCP_Mode) == Mode             &&
          strcmp(SWGetStr(tcp, SWA_NTCP_Host), idata->SWA_NServer_Host) == 0 ?
             tcp : NULL;
}

static LONG FindIdleCmd(SW_IDATA *idata, ULONG Type, ULONG TData)
{
   LONG x;
   Object *tcp;
   ULONG Port = idata->SWA_NServer_Port ? idata->SWA_NServer_Port :
                                          SWV_NServer_NNTPPort;
   ULONG Mode = SWV_NTCP_Mode_Text;
         
   if (Type == SWV_NServer_Type_FTP) {
      Port = (TData==1 ? SWV_NServer_TCPDataPort:SWV_NServer_TCPUserPort);
      Mode = (TData==1 ? SWV_NTCP_Mode_Binary   :SWV_NTCP_Mode_Text);
   }

   // -- find idle, connected connection to the proper server ----------------

   for (x=0; x<SWV_NServer_MaxCmd; x++)
      if ((tcp = CmdMatch(idata, x, Port, Mode)) &&
          SWGetLong(tcp, SWA_NTCP_Connected)) {
         set(tcp, SWA_NTCP_Host, idata->SWA_NServer_Host);
         return x;
      }


   // -- else, find idle, not-connected TCP object to current server ---------

   for (x=0; x<SWV_NServer_MaxCmd; x++)
      if (tcp = CmdMatch(idata, x, Port, Mode)) {
         set(tcp, SWA_NTCP_Host, idata->SWA_NServer_Host);
         return x;
      }


   // -- else, make a new one if we are able, insert into TCP list -----------

   for (x=0; x<SWV_NServer_MaxCmd; x++)
      if (!idata->Cmd[x].TCP) {
         if (!(idata->Cmd[x].TCP =
               MUI_NewObject(SWC_NTCP,
                             AppBaseDefTop(idata),
                             SWA_NTCP_Host,    idata->SWA_NServer_Host,
                             SWA_NTCP_Port,    Port,
                             SWA_NTCP_Timeout, idata->SWA_NServer_CxTimeout,
                             SWA_NTCP_Mode,    Mode,
                             TAG_DONE)))
            return -1;

         DoMethod(NN_TCPList(idata), MUIM_List_InsertSingle,
                  idata->Cmd[x].TCP, MUIV_List_Insert_Bottom);

         return x;
      }

   return -1;
}


/*****************************************************************************
 * Function: timer off                                                       *
 *****************************************************************************/

static void TimerOff(SW_IDATA *idata, struct TCPCmd *c)
{
   if (c->RetryTimer) {
      DoMethod(c->RetryTimer, SWM_Timer_Remove);
      MUI_DisposeObject(c->RetryTimer);
      c->RetryTimer = NULL;
   }
}


/*****************************************************************************
 * Function: timer on                                                        *
 *****************************************************************************/

static ULONG TimerOn(Object *o, SW_IDATA *idata, struct TCPCmd *c, LONG Cmd)
{
   if (c->RetryTimer) return SUCCESS;

   if (!(c->RetryTimer = 
         MUI_NewObject(SWC_Timer,
                       SWA_Timer_Millis,  1000,
                       SWA_Timer_Method,  SWM_NServer_CxRetry,
                       SWA_Timer_UData,   Cmd,
                       TAG_DONE)))
      return FAILURE;

   FSet(c->Flags, SWF_NServer_Retrying);

   DoMethod(c->RetryTimer, SWM_Timer_Install, o, NN_App(idata));

   return SUCCESS;
}


/*****************************************************************************
 * Function: send queue functions                                            *
 *****************************************************************************/

static void ClearQueue(SW_IDATA *idata, struct TCPCmd *c)
{
   FV(c->SendQueue);
   c->SendQueue    = NULL;
   c->SendQBuffLen = 0;
}

static void AddQueue(SW_IDATA *idata, struct TCPCmd *c, UBYTE *CmdLine)
{
   ULONG Len = strlen(CmdLine)+1;
   
   c->Status  = SWV_NServer_WaitCn;

   if (c->SendQueue && Len > c->SendQBuffLen)
      ClearQueue(idata, c);
   
   if (!c->SendQueue) {
      c->SendQBuffLen = Len + 64;  // some breathing room for longer cmds
      c->SendQueue    = AV(c->SendQBuffLen);
   }

   if (c->SendQueue)
      strcpy(c->SendQueue, CmdLine);
}


/*****************************************************************************
 * Function: init/clear command                                              *
 *****************************************************************************/

static LONG InitCmd(Object *o, SW_IDATA *idata, struct TCPCmd *c,
                    ULONG LineLimit, ULONG EstLines, ULONG Interval,
                    ULONG DataCount, ULONG Type)
{
   // -- remove old data -----------------------------------------------------

   if (!FIsSet(c->Flags, SWF_NServer_NumMode))
      DoMethod(o, SWM_NServer_DelLines, c->Lines, 0, c->MaxLine-1);

   FV(c->Lines);

   TimerOff(idata, c);

   c->Lines        = NULL;
   c->LineCount    = -1;
   c->LineLimit    = LineLimit;
   c->MaxLine      = EstLines;
   c->Status       = SWV_NServer_Idle;
   c->Interval     = Interval;
   c->FirstNew     = 0;
   c->CmdObj       = NULL;
   c->CmdMethod    = 0;
   c->Flags        = 0;
   c->Rescode      = 0;
   c->DataCount    = DataCount;
   c->RetryCnt     = 0;
   c->SendQueue    = NULL;
   c->SendQBuffLen = 0;
   c->Type         = Type;

   // -- allocate new line space if needed, and clear it ---------------------

   if (EstLines) {
      ULONG size = sizeof(UBYTE *) * EstLines;
      if (!(c->Lines = (UBYTE **)AV(size))) {
         c->MaxLine = 0;
         return -1;
      }

      memset(c->Lines, 0, size);
   }

   return 0;
}

static void ClearCmd(Object *o, SW_IDATA *idata, struct TCPCmd *c)
{
   ClearQueue(idata, c);
   InitCmd(o, idata, c, 0, 0, 0, 0, 0);
}


/*****************************************************************************
 * Function: Send command to server, reset stats, etc                        *
 *****************************************************************************/

static void RunCmd(Object *o, SW_IDATA *idata, LONG cmd, UBYTE *CmdStr)
{
   struct TCPCmd *c = &idata->Cmd[cmd];

   if (SW_STRISNULL(CmdStr)) return;
   
   FSet(c->Flags, SWF_NServer_1stLine);
   c->Status  = SWV_NServer_RunCmd;

   if (FIsSet(c->Flags, SWF_NServer_NewConnect | SWF_NServer_NewCmd))
      set(c->TCP, SWA_NTCP_Title, GetS(SWS_S_WaitForCmd));

   // -- handle debug output -------------------------------------------------

   if (idata->DBSend) {
      STRPTR s;
      ULONG  nl;

      for (s = CmdStr, nl = TRUE; *s; s++) {
         if (nl) {
            FPrintf(idata->DBSend, "%02ld: ", cmd);
            nl = FALSE;
         }

         if (*s == '\r') continue;
         if (*s == '\n') nl = TRUE;
            
         FPutC(idata->DBSend, *s);
      }
   }

   DoMethod(c->TCP, SWM_NTCP_SendStr, CmdStr);
}


/*****************************************************************************
 * Function: Open debug files                                                *
 *****************************************************************************/

static void OpenDBFiles(SW_IDATA *idata)
{
   TEXT DBFSpec[64], DBVar[64];
   
   // -- handle debug stuff --------------------------------------------------
   
   if (GetVar("NewsRog/Debug", DBVar, sizeof(DBVar)-1, 0) > 0) {
      if (strstr(DBVar, "NNTPSEND") && !idata->DBSend)
         idata->DBSend = SWOpenAppend(strcat(strcpy(DBFSpec, "RAM:NR.NNTPSEND."),
                                             idata->SWA_NServer_Host));
      
      if (strstr(DBVar, "NNTPRECV") && !idata->DBRecv)
         idata->DBRecv = SWOpenAppend(strcat(strcpy(DBFSpec, "RAM:NR.NNTPRECV."),
                                             idata->SWA_NServer_Host));
   }
}


/*****************************************************************************
 * Method: Connect to server, if needed                                      *
 *****************************************************************************/

METHOD(SWM_NServer_Connect, SWP_NServer_Connect *)
{
   struct TCPCmd *c = &idata->Cmd[msg->ID];

   OpenDBFiles(idata);
      
   // -- connect to the server -----------------------------------------------

   if (SWGetLong(c->TCP, SWA_NTCP_Connected)) SWDB_RET(TRUE);

   set(o, SWA_NServer_ConnectT, idata->SWA_NServer_ConnectT+1);

   if ((LONG)DoMethod(c->TCP, SWM_NTCP_Connect) < 0)
      SWDB_RET(DoMethod(o, SWM_Root_AdoptObjError, c->TCP, FALSE));

   SetAttrs(c->TCP,  // notify ourselves if the connection completes
            SWA_NTCP_Title,       GetS(SWS_S_WaitForTCP),
            SWA_NTCP_ConnectObj,  o, 
            SWA_NTCP_ConnectMthd, SWM_NServer_Connected,
            SWA_NTCP_LineObject,  o,              // send lines to us
            SWA_NTCP_LineMethod,  SWM_NServer_RecvLine,
            SWA_NTCP_CmdState,    msg->ID,        // offset into cmd array
            TAG_DONE);

   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: Connection is made                                                *
 *****************************************************************************/

METHOD(SWM_NServer_Connected, SWP_NTCP_Connected *)
{
   LONG          cmd = SWGetLong(msg->TCP, SWA_NTCP_CmdState);
   struct TCPCmd  *c = &idata->Cmd[cmd];

   FSet(c->Flags, SWF_NServer_NewConnect); // set to parse 1st result line

   set(c->TCP, SWA_NTCP_Title,
       GetS(c->Type == SWV_NServer_Type_NNTP ? SWS_S_WaitForNNTP :
                                               SWS_S_WaitForFTP));

   SendTgtMsg(o, idata, cmd, SWV_NServer_Connected);

   if (idata->DBSend)
      FPrintf(idata->DBSend, "%02ld: [Connected]\n", cmd);

   if (idata->DBRecv)
      FPrintf(idata->DBRecv, "%02ld: [Connected]\n", cmd);
   
   SWDB_RET(0);
}
   

/*****************************************************************************
 * Method: Send command using a free connection if available                 *
 *****************************************************************************/
/*
 Supported xfers:
    Normal, LineLimit, ExplicitDone, DataCount
    Pass SWT_NServer_ID to reuse.
    SWT_NServer_Cmd = command string to send
*/

METHOD(SWM_NServer_SendCmd, ULONG *)
{
   struct TagItem *ti = (struct TagItem *)(msg+1);
   struct TCPCmd *c;
   LONG   ID        = GetTagL (SWT_NServer_ID,  -1,                     ti), cmd;
   UBYTE *CmdStr    = GetTagUB(SWT_NServer_Cmd, NULL,                   ti);
   UBYTE *Title     = GetTagUB(SWT_NServer_Title, "",                   ti);
   ULONG  Type      = GetTagUL(SWT_NServer_Type, SWV_NServer_Type_NNTP, ti);

   if (!NN_TCPList(idata) ||                     // we need a TCPList;
       SW_STRISNULL(idata->SWA_NServer_Host) ||  // and a server to send to;
       !CmdStr)                                  // and a command string;
      SWDB_RET(FAILURE);

   if (!SWGetUL(NN_TCPList(idata), SWA_NTCPList_Online))
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_Offline, 0, 0));

   if ((cmd=ID) < 0) {                              // if not given one already,;
      if ((cmd = FindIdleCmd(idata, Type, 0)) < 0)  // find idle cmd;
         SWDB_RET(FAILURE);                         // ooops, no more free!;

      (c = &idata->Cmd[cmd])->MaxLine = 0;
      ClearCmd(o, idata, c);

      DoMethod(c->TCP, SWM_NTCP_ResetStats);
                                                    
      if (InitCmd(o, idata, c,
                  GetTagUL(SWT_NServer_LineLimit, SWF_NServer_NoLineLimit, ti),
                  GetTagUL(SWT_NServer_EstLines,  50,                  ti),
                  GetTagUL(SWT_NServer_Interval,  SWF_NServer_ItvNone, ti),
                  GetTagUL(SWT_NServer_DataCount, 1,                   ti),
                  Type) < 0)
         SWDB_RET(FAILURE);
   }

   c            = &idata->Cmd[cmd];
   c->Flags     = GetTagUL(SWT_NServer_Flags,     0, ti);
   c->CmdObj    = GetTagOB(SWT_NServer_CmdObj, NULL, ti);
   c->CmdMethod = GetTagUL(SWT_NServer_CmdMethod, 0, ti);
   c->Data      = GetTagUL(SWT_NServer_Data,      0, ti);
   c->Interval  = GetTagUL(SWT_NServer_Interval,  c->Interval, ti);
   
   if (Title) c->Title = Title;

   // -- connect to the server and send message ------------------------------

   if (ID < 0) { // don`t send begin if reusing old ID
      SendTgtMsg(o, idata, cmd, SWV_NServer_Begin);
      FSet(c->Flags, SWF_NServer_NewCmd);
   }

   if (!DoMethod(o, SWM_NServer_Connect, cmd)) {
      SendTgtMsg(o, idata, cmd, SWV_NServer_Abort);
      SWDB_RET(FAILURE);
   }

   // -- save command for later, in case we need it for a resend -------------
   
   AddQueue(idata, c, CmdStr);
   
   if (SWGetUL(c->TCP, SWA_NTCP_Connected))
      RunCmd(o, idata, cmd, CmdStr);

   SWDB_RET(SUCCESS);
}  SDFT


/*****************************************************************************
 * Function: allocate more space in line array if needed                     *
 *****************************************************************************/

static LONG GetLineSpace(SW_IDATA *idata, LONG cmd, LONG cline)
{
   struct TCPCmd *c = &idata->Cmd[cmd];

   if (cline >= c->MaxLine) {
      c->MaxLine += SWV_NServer_Quanta;
      
      if (SWPoolRV(idata->SWA_NServer_MemPool, c->MaxLine * sizeof(UBYTE *),
                   (UBYTE **)&(c->Lines)))
         SWDB_RET(-1);
   }

   SWDB_RET(0);
}


/*****************************************************************************
 * Function: Check whether we`re at the end of a data segment                *
 *****************************************************************************/

static LONG CheckTerm(struct TCPCmd *c, UBYTE *RawLine)
{
   return ((((c->LineCount+1) >= c->LineLimit     ||
             (NNTP_IsEnd(RawLine) || FTP_IsEnd(RawLine)) && !c->DataCount) &&
            !FIsSet(c->Flags, SWF_NServer_ExplicitDone)) ||
           FIsSet(c->Flags, SWF_NServer_IsDone));
}


/*****************************************************************************
 * Function: send periodic status report to target object                    *
 *****************************************************************************/

// BUGS: this is some ugly-ass code.

static ULONG SendStatusRpt(Object *o, SW_IDATA *idata, LONG cmd,
                           struct TCPCmd *c, ULONG Done, ULONG EOD,
                           ULONG Rescode)
{
   LONG MaxClrLine   = c->LineCount;
   UBYTE  **Lines    = c->Lines;
   LONG  Flags       = c->Flags;
   Object *CmdObj    = c->CmdObj;
   ULONG   CmdMethod = c->CmdMethod;
   LONG    FirstNew  = c->FirstNew;
   LONG    LineCount = c->LineCount;
   ULONG   Res       = c->Rescode;
   ULONG   Data      = c->Data;
   ULONG   Free      = FALSE;
   ULONG   rc        = SUCCESS;

   if (((c->LineCount - c->FirstNew + 1) >= c->Interval) || Done || EOD ||
       Rescode) {
      if (!Done || Rescode) {
         if (Rescode) c->Rescode = Rescode;
         rc = SendTgtMsg(o, idata, cmd,
                         EOD ? SWV_NServer_NextItem :
                         (Rescode ? SWV_NServer_Rescode : SWV_NServer_RunCmd));

         c->LineCount = -1;  // start over from beginning of buffer
         c->FirstNew  = 0;

         if (FIsSet(c->Flags, SWF_NServer_IsDone))
            Done = TRUE;

         if (rc == SWV_NServer_RescodeMore)    // get another rescode
            FSet(c->Flags, SWF_NServer_1stLine);
      }

      if (Done || rc == SWV_NServer_RescodeBad) {
         c->Lines = NULL; // so ClearCmd doesn`t nuke it
         Free     = TRUE;
   
         DoMethod(c->TCP, SWM_NTCP_XferDone);  // send end to TCP object;
         ClearCmd(o, idata, c);                // delete our data

         if (CmdObj && CmdMethod)
            DoMethod(CmdObj, CmdMethod, Lines,
                     rc == SWV_NServer_RescodeBad ? rc : SWV_NServer_Done,
                     FirstNew,
                     LineCount, o, cmd, Res, Data);
      }
 
      if (Lines)
         if (!FIsSet(Flags, SWF_NServer_NumMode))
            DoMethod(o, SWM_NServer_DelLines, Lines, 0, MaxClrLine);

      if (c->Lines)
         memset(c->Lines, 0, sizeof(UBYTE *)*c->MaxLine);

      if (Free)
         FV(Lines);
   }

   SWDB_RET(rc);
}
   

/*****************************************************************************
 * Function: abort pending receive                                           *
 *****************************************************************************/

static ULONG AbortRecv(Object *o, SW_IDATA *idata, LONG cmd, struct TCPCmd *c)
{
   if (idata->DBSend)
      FPrintf(idata->DBSend, "%02ld: [Disconnected]\n", cmd);

   if (idata->DBRecv)
      FPrintf(idata->DBRecv, "%02ld: [Disconnected]\n", cmd);

   DoMethod(o, SWM_Root_AdoptObjError, c->TCP, 0);

   SendTgtMsg(o, idata, cmd, SWV_NServer_Abort);
   ClearCmd(o, idata, c);

   return SWV_NTCP_Disconnect;
}


/*****************************************************************************
 * Method: abort connection by telling TCP to disconnect                     *
 *****************************************************************************/

METHOD(SWM_NServer_Abort, SWP_NServer_Abort *)
{
   if (msg->ID < 0) SWDB_RET(0);
   
   TimerOff(idata, &idata->Cmd[msg->ID]);

   if (IDInRange(msg->ID) && idata->Cmd[msg->ID].TCP)
      DoMethod(idata->Cmd[msg->ID].TCP, SWM_NTCP_Disconnect, FALSE);

  SWDB_RET(0);
} SDFT

   
/*****************************************************************************
 * Server Retry                                                              *
 *****************************************************************************/

static ULONG Server_Retry(Object *o, SW_IDATA *idata, LONG cmd,
                          struct TCPCmd *c, ULONG Rescode)
{
   // -- if over the retry limit, just abort ---------------------------------

   if (c->RetryCnt++ >= idata->SWA_NServer_MaxRetries) {
      TimerOff(idata, c);
      SWDB_RET(AbortRecv(o, idata, cmd, c));
   }

   c->RetryTime = idata->SWA_NServer_RetryDelay / 1000;

   if (TimerOn(o, idata, c, cmd) != SUCCESS)
      SWDB_RET(AbortRecv(o, idata, cmd, c));
   
   SWDB_RET(SWV_NTCP_Disconnect);
}


/*****************************************************************************
 * Function: transmit user info                                              *
 *****************************************************************************/

static ULONG XmitAuthInfo(Object *o, SW_IDATA *idata,
                          ULONG Cmd, struct TCPCmd *c, ULONG Title,
                          STRPTR UserStr, STRPTR PassStr,
                          STRPTR UserVal, STRPTR PassVal,
                          ULONG NeedUser, ULONG NeedPass)
{
   TEXT CmdBuff[80];

   UserVal = SW_STRDEF(UserVal, idata->SWA_NServer_AuthUser);
   PassVal = SW_STRDEF(PassVal, idata->SWA_NServer_AuthPass);

   set(c->TCP, SWA_NTCP_Title, GetS(Title));
   
   if (NeedUser) {
      if (SW_STRISNULL(UserVal)) {
         SWERR(o, FAILURE, SWS_Err_NoAuthUser, 0, 0);
         AbortRecv(o, idata, Cmd, c);
         return SWV_NTCP_Disconnect;
      }

      sprintf(CmdBuff, "%s %s" SWV_NServer_EOL, UserStr, UserVal);
      DoMethod(c->TCP, SWM_NTCP_SendStr, CmdBuff);
      FSet(c->Flags, SWF_NServer_SentUser);
   } else if (NeedPass) {
      if (SW_STRISNULL(PassVal)) {
         SWERR(o, FAILURE, SWS_Err_NoAuthPass, 0, 0);
         AbortRecv(o, idata, Cmd, c);
         return SWV_NTCP_Disconnect;
      }
   
      sprintf(CmdBuff, "%s %s" SWV_NServer_EOL, PassStr, PassVal);
      DoMethod(c->TCP, SWM_NTCP_SendStr, CmdBuff);
      FSet(c->Flags, SWF_NServer_SentPass);
   }

   return SWV_NTCP_OK;
}


/*****************************************************************************
 * Function: Authorization done - see if it worked or not                    *
 *****************************************************************************/

static ULONG AuthDone(Object *o, SW_IDATA *idata,
                      ULONG Cmd, struct TCPCmd *c, ULONG Accepted)
{
   FReset(c->Flags, SWF_NServer_SentUser|SWF_NServer_SentPass|
                    SWF_NServer_SentPort);

   if (!Accepted) {
      SWERR(o, FAILURE, SWS_Err_AuthFailed, 0, 0);
      AbortRecv(o, idata, Cmd, c);
      return SWV_NTCP_Disconnect;
   }

   // -- it must have worked... resend the original command ---------------
      
   RunCmd(o, idata, Cmd, c->SendQueue);

   return SWV_NTCP_OK;
}


/*****************************************************************************
 * Function: send AUTHINFO for NNTP authorization                            *
 *****************************************************************************/

static ULONG SendAuthInfoNNTP(Object *o, SW_IDATA *idata,
                              ULONG Cmd, struct TCPCmd *c, STRPTR Line)
{
   ULONG Rescode = atol(Line);

   if (c->Type != SWV_NServer_Type_NNTP) return SWV_NTCP_NoOp;
   
   // -- see if the rescode we got was asking for authentication ----------
      
   if (Rescode == SWV_NSR_AuthRequired  ||
       Rescode == SWV_NSR_AuthRequired2 ||
       Rescode == SWV_NSR_AuthRequired3 ||
       Rescode == SWV_NSR_ContinueAuth)
      // -- shit, it was.  this sucks, beavis! ----------------------------
      return XmitAuthInfo(o, idata, Cmd, c, SWS_S_Auth,
                          "AUTHINFO user", "AUTHINFO pass",
                          NULL,             NULL,
                          !FIsSet(c->Flags, SWF_NServer_SentUser),
                          !FIsSet(c->Flags, SWF_NServer_SentPass));

   // -- authentication finished one way or the other ------------------------
   
   if (FIsSet(c->Flags, SWF_NServer_SentUser|SWF_NServer_SentPass))
      return AuthDone(o, idata, Cmd, c,
                      Rescode == SWV_NSR_AuthAccepted ||
                      Rescode == SWV_NSR_AuthAccepted2);

   return SWV_NTCP_NoOp;
}


/*****************************************************************************
 * Function: send AUTHINFO for NNTP authorization                            *
 *****************************************************************************/

static ULONG SendAuthInfoFTP(Object *o, SW_IDATA *idata,
                             ULONG Cmd, struct TCPCmd *c, STRPTR Line)
{
   ULONG Rescode = atol(Line);

   if (c->Type != SWV_NServer_Type_FTP) return SWV_NTCP_NoOp;

   // -- see if we`re just connecting to this server -------------------------
   
   if (Rescode == SWV_NSR_FTP_ConnectOK ||
       Rescode == SWV_NSR_FTP_NeedPass)
      return  XmitAuthInfo(o, idata, Cmd, c, SWS_S_FTPLogin,
                           "USER",        "PASS",
                           "anonymous",   "me@here.com",
                           Rescode == SWV_NSR_FTP_ConnectOK,
                           Rescode == SWV_NSR_FTP_NeedPass);

   // -- authentication finished one way or the other ------------------------
   
   if (FIsSet(c->Flags, SWF_NServer_SentUser|SWF_NServer_SentPass))
      return AuthDone(o, idata, Cmd, c, Rescode == SWV_NSR_FTP_LoggedIn);

   return SWV_NTCP_NoOp;
}


/*****************************************************************************
 * Function: send AUTHINFO user info                                         *
 *****************************************************************************/

static ULONG SendAuthInfo(Object *o, SW_IDATA *idata,
                          ULONG Cmd, struct TCPCmd *c, STRPTR Line)
{
   if (c->Type == SWV_NServer_Type_NNTP)
      return SendAuthInfoNNTP(o, idata, Cmd, c, Line);

   if (c->Type == SWV_NServer_Type_FTP)
      return SendAuthInfoFTP(o, idata, Cmd, c, Line);
         
    return SWV_NTCP_NoOp;
}


/*****************************************************************************
 * Method: Accept an async line, process, and save it                        *
 *****************************************************************************/

METHOD(SWM_NServer_RecvLine, SWP_NTCP_AsyncLine *)
{
   LONG cmd = SWGetLong(msg->TCP, SWA_NTCP_CmdState);
   struct TCPCmd *c = &idata->Cmd[cmd];
   LONG lc, Rescode = 0;

   // -- make sure we didn`t get disconnected --------------------------------
   
   if (msg->LineLen==SWF_NTCP_Disconnected || msg->LineLen==SWF_NTCP_Deleted) {
      if (msg->LineLen == SWF_NTCP_Disconnected) {
         if (FIsSet(c->Flags, SWF_NServer_Retrying)) {
            FReset(c->Flags, SWF_NServer_Retrying);
            SWDB_RET(SWV_NTCP_OK);
         }
      }
      
      set(o, SWA_NServer_PostOK, FALSE);

      AbortRecv(o, idata, cmd, c);
      if (msg->LineLen == SWF_NTCP_Deleted)
         c->TCP = NULL;
      SWDB_RET(SWV_NTCP_Disconnect);
   }

   if (!IDInRange(cmd) || !c->Lines) SWDB_RET(SWV_NTCP_Disconnect);


   // -- handle debug output -------------------------------------------------

   if (idata->DBRecv)
      FPrintf(idata->DBRecv, "%02ld: %s\n", cmd, msg->Line);


   // -- if new connection, check status -------------------------------------

   if (FIsSet(c->Flags, SWF_NServer_NewConnect)) {
      struct DateStamp ds;
      
      Rescode = atol(msg->Line);
      FReset(c->Flags, SWF_NServer_NewConnect);

      if (c->Type == SWV_NServer_Type_NNTP &&
          !(Rescode==SWV_NSR_SrvReadyPost || Rescode==SWV_NSR_SrvReadyNoPost) ||
          c->Type == SWV_NServer_Type_FTP  &&
          !(Rescode==SWV_NSR_FTP_ConnectOK))
         SWDB_RET(Server_Retry(o, idata, cmd, c, Rescode));

      SetAttrs(o,
               SWA_NServer_PostOK, (Rescode == SWV_NSR_SrvReadyPost),
               SWA_NServer_ConnectS, idata->SWA_NServer_ConnectS+1);

      TimerOff(idata, c);
      
      DateStamp(&ds);

      SetAttrs(o,
               SWA_NServer_CnDays,   ds.ds_Days,
               SWA_NServer_CnMin,    ds.ds_Minute,
               SWA_NServer_CnTick,   ds.ds_Tick,
               TAG_DONE);

      if (c->Type == SWV_NServer_Type_NNTP) {
         RunCmd(o, idata, cmd, c->SendQueue);
         SWDB_RET(SWV_NTCP_OK);
      }

      if (c->Type == SWV_NServer_Type_FTP)
         FSet(c->Flags, SWF_NServer_1stLine);
   }

   // -- set connection title ------------------------------------------------

   if (FIsSet(c->Flags, SWF_NServer_NewCmd)) {
      set(c->TCP, SWA_NTCP_Title, c->Title);
      FReset(c->Flags, SWF_NServer_NewCmd);
   }
      

   // -- handle NNTP authentication request ----------------------------------

   if (FIsSet(c->Flags, SWF_NServer_1stLine)) {
      ULONG auth = SendAuthInfo(o, idata, cmd, c, msg->Line);

      if (auth == SWV_NTCP_OK || auth == SWV_NTCP_Disconnect)
         SWDB_RET(auth);
   }


   // -- store line in buffer if it`s not the end marker ---------------------

   if (c->Type == SWV_NServer_Type_FTP || !NNTP_IsEnd(msg->Line)) {
      if (GetLineSpace(idata, cmd, lc = ++c->LineCount))
         SWDB_RET(AbortRecv(o, idata, cmd, c));

      if (FIsSet(c->Flags, SWF_NServer_NumMode)) {
         c->Lines[lc] = (UBYTE *)atol(msg->Line);
      } else {
         if (!(c->Lines[lc] = AV(msg->LineLen+1)))
            SWDB_RET(AbortRecv(o, idata, cmd, c));

         strcpy(c->Lines[lc], msg->Line +
                (strcmp(msg->Line, SWF_NServer_EscDot) == 0 ? 1 : 0) +
                (FIsSet(c->Flags, SWF_NServer_SkipSpc) ?
                 strcspn(msg->Line, " ") + 1 : 0));
      }

      if (FIsSet(c->Flags, SWF_NServer_1stLine)) {
         FReset(c->Flags, SWF_NServer_1stLine);
         Rescode = atol(msg->Line);
      }
   }

   if (c->Type == SWV_NServer_Type_FTP  && FTP_IsEnd(msg->Line) ||
       c->Type == SWV_NServer_Type_NNTP && NNTP_IsEnd(msg->Line)) {
      c->DataCount--;
      FSet(c->Flags, SWF_NServer_1stLine);
   }

   //  -- send status report to object/method if needed ----------------------

   SendStatusRpt(o, idata, cmd, c, CheckTerm(c, msg->Line),
                 NNTP_IsEnd(msg->Line) || FTP_IsEnd(msg->Line), Rescode);

   SWDB_RET(SWV_NTCP_OK);
}  SDFT


/*****************************************************************************
 * Method: Accept async binary data                                          *
 *****************************************************************************/

METHOD(SWM_NServer_RecvBin, SWP_NTCP_AsyncLine *)
{
   LONG FTPCmd         = SWGetLong(msg->TCP, SWA_NTCP_CmdState);
   struct FTPCmd *ftpc = &idata->FTPCmd[FTPCmd];

   // -- see if other end hung up on us --------------------------------------
   
   if (msg->LineLen==SWF_NTCP_Disconnected || msg->LineLen==SWF_NTCP_Deleted)
      SWDB_RET(SWV_NTCP_Disconnect);

   // -- see if we need to allocate more space -------------------------------
   
   if ((ftpc->DataBuffPos + msg->LineLen) >= ftpc->DataBuffSize) {
      ftpc->DataBuffSize = ftpc->DataBuffSize * 3 / 2 + msg->LineLen;
      ftpc->DataBuffSize = max(ftpc->DataBuffSize, 4096);
      if (RV(ftpc->DataBuffSize, &ftpc->DataBuff) < 0) {
         ftpc->DataBuffSize = ftpc->DataBuffPos = 0;
         SWDB_RET(SWV_NTCP_Disconnect);
      }
   }

   memcpy(ftpc->DataBuff + ftpc->DataBuffPos, msg->Line, msg->LineLen);
   ftpc->DataBuffPos += msg->LineLen;

   SWDB_RET(SWV_NTCP_OK);
} SDFT


/*****************************************************************************
 * Method: Reset statistics                                                  *
 *****************************************************************************/

METHOD(SWM_NServer_ResetStats, SWP_NTCP_AsyncLine *)
{
   SetAttrs(o,
            SWA_NServer_ConnectS, 0,
            SWA_NServer_ConnectT, 0,
            SWA_NServer_CnDays,   0,
            SWA_NServer_CnMin,    0,
            SWA_NServer_CnTick,   0,
            SWA_NServer_NGroups,  0,
            TAG_DONE);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Delete TCP connection for a command                               *
 *****************************************************************************/

METHOD(SWM_NServer_DelCmd, SWP_NServer_DelCmd *)
{
   struct TCPCmd *c = &idata->Cmd[msg->ID];

   if (!IDInRange(msg->ID)) SWDB_RET(0);

   // -- removing it from the TCPList also deletes the TCP object --
   if (NN_TCPList(idata))
      DoMethod(NN_TCPList(idata), SWM_NList_RemoveObj, c->TCP);

   ClearCmd(o, idata, c);
   c->TCP = NULL;

   SWDB_RET(NULL);
} SDFT


/*****************************************************************************
 * Method: Delete range of lines from line buffer                            *
 *****************************************************************************/

METHOD(SWM_NServer_DelLines, SWP_NServer_DelLines *)
{
   LONG x;

   if (!msg->LineBuff) SWDB_RET(0);
   
   for (x=msg->FirstLine; x<=msg->LastLine; x++) {
      FV(msg->LineBuff[x]);
      msg->LineBuff[x] = NULL;
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Adopt lines from the server line list to some other array.        *
 * Optionally concatenate header lines, like this:                           *
 *                                                                           *
 *   References: <foo>                                                       *
 *     <bar>                                                                 *
 * ->                                                                        *
 *   References: <foo> <bar>                                                 *
 *                                                                           *
 * Returns count of lines added to target buffer, or 0 for error.            *
 *****************************************************************************/

METHOD(SWM_NServer_AdoptLines, SWP_NServer_AdoptLines *)
{
   LONG   x, y, InHdrs = msg->FixHdrs;
   STRPTR t;

   if (!msg->TargetBuff) SWDB_RET(0);

   for (x=msg->FirstLine, y=0; x<=msg->LastLine; x++) {
      if (InHdrs) {
         if (IsWhite(*msg->LineBuff[x]) && y > 0) {
            if (RV(strlen(msg->TargetBuff[y-1]) + strlen(msg->LineBuff[x]) + 1,
                   &msg->TargetBuff[y-1]) < 0)
               SWDB_RET(0);

            for (t = msg->LineBuff[x]; *t && IsWhite(t[1]); t++);
            
            strcat(msg->TargetBuff[y-1], t);

            FV(msg->LineBuff[x]);
            msg->LineBuff[x] = NULL;

            continue;
         } else
            if (!*msg->LineBuff[x])
               InHdrs = FALSE;
      }
      
      msg->TargetBuff[y++] = msg->LineBuff[x];
      msg->LineBuff[x]     = NULL;
   }
   
   SWDB_RET(y);
} SDFT


/*****************************************************************************
 * Method: Mention that we're done with this connection                      *
 *****************************************************************************/

METHOD(SWM_NServer_Done, SWP_NServer_Done *)
{
   struct TCPCmd *c = &idata->Cmd[msg->ID];

   FSet(c->Flags, SWF_NServer_IsDone);
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Delete range of lines from line buffer                            *
 *****************************************************************************/

METHOD(SWM_NServer_CxRetry, SWP_Timer_Trigger *)
{
   struct TCPCmd *c = &idata->Cmd[msg->UData];

   if (--c->RetryTime) {
      TEXT cmsg[80];

      sprintf(cmsg, GetS(SWS_S_WaitRetry), c->RetryTime);
      
      SetAttrs(c->TCP,
               SWA_NTCP_LineObject,  o,
               SWA_NTCP_LineMethod,  SWM_NServer_CxCancel,
               SWA_NTCP_Title,       cmsg,
               TAG_DONE);

      SWDB_RET(SUCCESS);
   }

   TimerOff(idata, c);
   
   if (!DoMethod(o, SWM_NServer_Connect, msg->UData)) {
      AbortRecv(o, idata, msg->UData, c);
      SWDB_RET(FAILURE);
   }

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Delete range of lines from line buffer                            *
 *****************************************************************************/

METHOD(SWM_NServer_CxCancel, SWP_NTCP_AsyncLine *)
{
   LONG cmd = SWGetLong(msg->TCP, SWA_NTCP_CmdState);
   struct TCPCmd *c = &idata->Cmd[cmd];

   AbortRecv(o, idata, cmd, c);

   if (msg->LineLen == SWF_NTCP_Deleted)
      c->TCP = NULL;

   SWDB_RET(SWV_NTCP_Disconnect);
} SDFT


/*****************************************************************************
 * Method: Delete range of lines from line buffer                            *
 *****************************************************************************/

METHOD(SWM_NServer_GetTCP, SWP_NServer_GetTCP *)
{
   struct TCPCmd *c = &idata->Cmd[msg->ID];

   if (!IDInRange(msg->ID)) SWDB_RET(0);

   SWDB_RET(c->TCP);
} SDFT


/*****************************************************************************
 * Function: Send FTP status info                                            *
 *****************************************************************************/

static ULONG SendFTPStatMsg(Object *o, SW_IDATA *idata, LONG cmd, LONG Status,
                            ULONG Rescode)
{
   struct FTPCmd *c = &idata->FTPCmd[cmd];
   
   if (c->CmdObj && c->CmdMethod)
      return DoMethod(c->CmdObj, c->CmdMethod, Status, cmd,
                      c->DataBuff, c->DataBuffPos, o, Rescode, c->CmdData);

   return 0;
}

   
/*****************************************************************************
 * Function: Find idle FTP command                                           *
 *****************************************************************************/

static LONG FindIdleFTP(SW_IDATA *idata, LONG ID)
{
   LONG x;
   
   if (ID >= 0) return ID;

   for (x=0; x<SWV_NServer_MaxFTPCmd; x++)
      if (!idata->FTPCmd[x].Connected)
         return x;

   return -1;
}

   
/*****************************************************************************
 * Method: FTP fetch directory                                               *
 *****************************************************************************/

METHOD(SWM_NServer_GetFTPDir, ULONG *)
{
   struct  TagItem *ti = (struct TagItem *)(msg+1);
   STRPTR  Title       = GetTagUB(SWT_NServer_Title,    "",            ti);
   STRPTR  Dir         = GetTagUB(SWT_NServer_FTPDir,   NULL,          ti);
   LONG    ID          = GetTagL (SWT_NServer_ID,       -1,            ti);
   Object *CmdObj      = GetTagOB(SWT_NServer_CmdObj,   NULL,          ti);
   ULONG   CmdMethod   = GetTagUL(SWT_NServer_CmdMethod, 0,            ti);
   ULONG   CmdData     = GetTagUL(SWT_NServer_Data,      0,            ti);
   LONG    FTPCmd      = FindIdleFTP(idata, ID);
   struct  FTPCmd *ftpc= &idata->FTPCmd[FTPCmd];
   TEXT    CmdBuff[FMSIZE+8];
   UBYTE   c = '\0';

   if (FTPCmd < 0) SWDB_RET(FAILURE);

   ftpc->Connected = TRUE;
   ftpc->CmdObj    = CmdObj;
   ftpc->CmdMethod = CmdMethod;
   ftpc->CmdData   = CmdData;
   ftpc->State     = 0;

   if (strlen(Dir) >= FMSIZE)
      c=Dir[FMSIZE-1], Dir[FMSIZE-1] = '\0';
      
   sprintf(CmdBuff, "CWD %s" SWV_NServer_EOL, Dir);

   if (c) Dir[FMSIZE-1] = c;

   if (DoMethod(o, SWM_NServer_SendCmd,
                SWT_NServer_Cmd,       CmdBuff,
                SWT_NServer_CmdObj,    o,
                SWT_NServer_CmdMethod, SWM_NServer_GetFTPDir_P1,
                SWT_NServer_Data,      FTPCmd,
                SWT_NServer_Title,     SW_STRDEF(Title, GetS(SWS_T_FetchDir)),
                SWT_NServer_Flags,     SWF_NServer_ExplicitDone,
                SWT_NServer_Type,      SWV_NServer_Type_FTP,
                TAG_DONE) != SUCCESS)
      SWDB_RET(FAILURE);

   SWDB_RET(SUCCESS);
} SDFT



/*****************************************************************************
 * Function: Send FTP Port info                                              *
 *****************************************************************************/

static ULONG SendFTPDataPort(Object *CtrlTCP, Object *DataTCP)
{
   TEXT   CmdBuff[48];
   ULONG  FTPAddr = SWGetUL(DataTCP, SWA_NTCP_LocAddr),
          FTPPort = SWGetUL(DataTCP, SWA_NTCP_LocPort);
   UBYTE *a = (UBYTE *)&FTPAddr, *p = ((UBYTE *)&FTPPort)+2;
   
   sprintf(CmdBuff, "PORT %ld,%ld,%ld,%ld,%ld,%ld" SWV_NServer_EOL,
           UC(a[0]), UC(a[1]), UC(a[2]), UC(a[3]), UC(p[0]), UC(p[1]));

   DoMethod(CtrlTCP, SWM_NTCP_SendStr, CmdBuff);

   return SUCCESS;
}


/*****************************************************************************
 * Function: Make TCP FTP object                                             *
 *****************************************************************************/

ULONG MakeTCPFTPObj(Object *o, SW_IDATA *idata, ULONG FTPCmd, Object *tcp)
{
   Object *co = idata->FTPCmd[FTPCmd].DataCnx =
      MUI_NewObject(SWC_NTCP,
                    AppBaseDefTop(idata),
                    SWA_NTCP_Host,       idata->SWA_NServer_Host,
                    SWA_NTCP_Port,       SWV_NServer_TCPDataPort,
                    SWA_NTCP_Timeout,    idata->SWA_NServer_CxTimeout,
                    SWA_NTCP_Mode,       SWV_NTCP_Mode_Binary,
                    SWA_NTCP_LineMethod, SWM_NServer_RecvBin,
                    SWA_NTCP_LineObject, o,
                    SWA_NTCP_CmdState,   FTPCmd,
                    SWA_NTCP_Listen,     TRUE,
                    SWA_NTCP_InitAddr,   SWGetUL(tcp, SWA_NTCP_LocAddr),
                    SWA_NTCP_Title,      GetS(SWS_T_FTPData),
                    TAG_DONE);

   if (!co) return SWV_NServer_RescodeBad;

   DoMethod(NN_TCPList(idata), MUIM_List_InsertSingle, co,
            MUIV_List_Insert_Bottom);
      
   if ((LONG)DoMethod(co, SWM_NTCP_Connect) < 0)
      return SWV_NServer_RescodeBad;

   SendFTPDataPort(tcp, co);

   return 0;
}

/*****************************************************************************
 * Method: FTP fetch dir, internal                                           *
 *****************************************************************************/

METHOD(SWM_NServer_GetFTPDir_P1, SWP_NServer_StatusMsg *)
{
   LONG    FTPCmd = NSR_Data(msg);
   Object *tcp = idata->Cmd[msg->ID].TCP;

   if (NSR_SBegin(msg)) {
      SendFTPStatMsg(o, idata, FTPCmd, SWV_NServer_Begin, 0);
      idata->FTPCmd[FTPCmd].Cmd = msg->ID;
   }
   
   if (NSR_SNext(msg)) {
      switch (idata->FTPCmd[FTPCmd].State++) {
        case 0:  /* CWD command results */
           if (NSR_ResCode(msg) != SWV_NSR_FTP_CmdSuccess) {
              SendFTPStatMsg(o, idata, FTPCmd, SWV_NServer_BadFTPDir,
                             NSR_ResCode(msg));
              SWDB_RET(SWV_NServer_RescodeBad);
           }

           if (MakeTCPFTPObj(o, idata, FTPCmd, tcp) == SWV_NServer_RescodeBad)
              SWDB_RET(SWV_NServer_RescodeBad);
           
           break;

        case 1:  /* PORT command results */
           if (NSR_ResCode(msg) != SWV_NSR_FTP_CommandOK)
              SWDB_RET(SWV_NServer_RescodeBad);

           DoMethod(tcp, SWM_NTCP_SendStr, "NLST" SWV_NServer_EOL);
           break;
               
        case 2:  /* NLST beginning */
           if (NSR_ResCode(msg) != SWV_NSR_FTP_StartingXFer1 &&
               NSR_ResCode(msg) != SWV_NSR_FTP_StartingXFer2) {
              SendFTPStatMsg(o, idata, FTPCmd, SWV_NServer_BadFTPDir,
                             NSR_ResCode(msg));
              SWDB_RET(SWV_NServer_RescodeBad);
           }

           if (DoMethod(idata->FTPCmd[FTPCmd].DataCnx,SWM_NTCP_Accept)!=SUCCESS)
              SWDB_RET(SWV_NServer_RescodeBad);

           break;

        case 3:  /* NLST finished */
           if (NSR_ResCode(msg) != SWV_NSR_FTP_CloseDataOK &&
               NSR_ResCode(msg) != SWV_NSR_FTP_CmdSuccess) {
              SendFTPStatMsg(o, idata, FTPCmd, SWV_NServer_BadFTPDir,
                             NSR_ResCode(msg));
              SWDB_RET(SWV_NServer_RescodeBad);
           }

           DoMethod(NSR_Srv(msg), SWM_NServer_Done, NSR_ID(msg));
           break;
      }
   }
   
   if (NSR_SAnyEnd(msg)) {
      SendFTPStatMsg(o, idata, FTPCmd, NSR_Status(msg), 0);
      ClearFTP(o, idata, FTPCmd);
   }

   SWDB_RET(0);
}


/*****************************************************************************
 * Method: FTP fetch file                                                    *
 *****************************************************************************/

METHOD(SWM_NServer_GetFTPFile, ULONG *)
{
   struct  TagItem *ti = (struct TagItem *)(msg+1);
   STRPTR  Title       = GetTagUB(SWT_NServer_Title,    "",            ti);
   STRPTR  Dir         = GetTagUB(SWT_NServer_FTPDir,   NULL,          ti);
   STRPTR  File        = GetTagUB(SWT_NServer_FTPFile,  NULL,          ti);
   LONG    ID          = GetTagL (SWT_NServer_ID,       -1,            ti);
   Object *CmdObj      = GetTagOB(SWT_NServer_CmdObj,   NULL,          ti);
   ULONG   CmdMethod   = GetTagUL(SWT_NServer_CmdMethod, 0,            ti);
   ULONG   CmdData     = GetTagUL(SWT_NServer_Data,      0,            ti);
   ULONG   FTPType     = GetTagUL(SWT_NServer_FTPType,
                                  SWV_NServer_FTPType_ASCII, ti);
   LONG    FTPCmd      = FindIdleFTP(idata, ID);
   struct  FTPCmd *ftpc= &idata->FTPCmd[FTPCmd];
   TEXT    CmdBuff[FMSIZE+8];
   UBYTE   c;

   if (FTPCmd < 0 || SW_STRISNULL(File) || SW_STRISNULL(Dir)) SWDB_RET(FAILURE);

   ftpc->Connected = TRUE;
   ftpc->CmdObj    = CmdObj;
   ftpc->CmdMethod = CmdMethod;
   ftpc->CmdData   = CmdData;
   ftpc->State     = 0;
   ftpc->FTPType   = FTPType;
   ftpc->Str1      = AV(strlen(File) + 8);

   if (strlen(Dir) >= FMSIZE)
      c=Dir[FMSIZE-1], Dir[FMSIZE-1] = '\0';
      
   sprintf(CmdBuff,    "CWD %s"  SWV_NServer_EOL, Dir);
   sprintf(ftpc->Str1, "RETR %s" SWV_NServer_EOL, File);

   if (c) Dir[FMSIZE-1] = c;

   if (DoMethod(o, SWM_NServer_SendCmd,
                SWT_NServer_Cmd,       CmdBuff,
                SWT_NServer_CmdObj,    o,
                SWT_NServer_CmdMethod, SWM_NServer_GetFTPFile_P1,
                SWT_NServer_Data,      FTPCmd,
                SWT_NServer_Title,     SW_STRDEF(Title, GetS(SWS_T_FetchFile)),
                SWT_NServer_Flags,     SWF_NServer_ExplicitDone,
                SWT_NServer_Type,      SWV_NServer_Type_FTP,
                TAG_DONE) != SUCCESS)
      SWDB_RET(FAILURE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: FTP fetch file, internal                                          *
 *****************************************************************************/

METHOD(SWM_NServer_GetFTPFile_P1, SWP_NServer_StatusMsg *)
{
   LONG    FTPCmd = NSR_Data(msg);
   Object *tcp = idata->Cmd[msg->ID].TCP;

   if (NSR_SBegin(msg)) {
      SendFTPStatMsg(o, idata, FTPCmd, SWV_NServer_Begin, 0);
      idata->FTPCmd[FTPCmd].Cmd = msg->ID;
   }
   
   if (NSR_SNext(msg)) {
      switch (idata->FTPCmd[FTPCmd].State++) {
        case 0:  /* CWD command results */
           if (NSR_ResCode(msg) != SWV_NSR_FTP_CmdSuccess)
              SWDB_RET(SWV_NServer_RescodeBad);

           if (MakeTCPFTPObj(o, idata, FTPCmd, tcp) == SWV_NServer_RescodeBad)
              SWDB_RET(SWV_NServer_RescodeBad);
           
           break;

        case 1:  /* PORT command results */
           if (NSR_ResCode(msg) != SWV_NSR_FTP_CommandOK)
              SWDB_RET(SWV_NServer_RescodeBad);

           DoMethod(tcp, SWM_NTCP_SendStr, idata->FTPCmd[FTPCmd].Str1);

           break;
               
        case 2:  /* RETR beginning */
           if (NSR_ResCode(msg) != SWV_NSR_FTP_StartingXFer1 &&
               NSR_ResCode(msg) != SWV_NSR_FTP_StartingXFer2)
              SWDB_RET(SWV_NServer_RescodeBad);

           if (DoMethod(idata->FTPCmd[FTPCmd].DataCnx,SWM_NTCP_Accept)!=SUCCESS)
              SWDB_RET(SWV_NServer_RescodeBad);

           break;

        case 3:  /* RETR finished */
           if (NSR_ResCode(msg) != SWV_NSR_FTP_CloseDataOK &&
               NSR_ResCode(msg) != SWV_NSR_FTP_CmdSuccess)
              SWDB_RET(SWV_NServer_RescodeBad);
           DoMethod(NSR_Srv(msg), SWM_NServer_Done, NSR_ID(msg));
           break;
      }
   }
   
   if (NSR_SAnyEnd(msg)) {
      SendFTPStatMsg(o, idata, FTPCmd, NSR_Status(msg), 0);
      ClearFTP(o, idata, FTPCmd);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Abort FTP command                                                 *
 *****************************************************************************/

METHOD(SWM_NServer_AbortFTP, SWP_NServer_AbortFTP *)
{
   if (msg->ID < 0) SWDB_RET(0);

   SWDB_RET(DoMethod(o, SWM_NServer_Abort, idata->FTPCmd[msg->ID].Cmd));
} SDFT

# endif
