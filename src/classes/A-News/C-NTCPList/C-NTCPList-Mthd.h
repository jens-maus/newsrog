
/*****************************************************************************
 * Method: Write object to disk                                              *
 *****************************************************************************/

// -- overload Read/Write since TCPList doesn`t save contents.  --------------

# define SWM_WRITE_F
METHOD(SWM_Write, SWP_Write *)
   SWDB_OBJMSG(__FUNC__, o)
{
   if (SWWriteAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, SW_CLIB))
      SWDB_RET(FAILURE);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Read object from disk                                             *
 *****************************************************************************/

# define SWM_READINT_F
METHOD(SWM_ReadInt, SWP_Read *)
   SWDB_OBJMSG(__FUNC__, o)
{
   UBYTE ClassName[SW_MaxClassNameLen];

   if (SWReadAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, o)) SWDB_RET(NULL);
   if (READCNAME(msg->fp, ClassName))                       SWDB_RET(NULL);
   if (!DoSuperMethodA(cl, o, msg))                         SWDB_RET(NULL);

   SWDB_RET((ULONG)o);
} SDFT


/*****************************************************************************
 * Method: Determin max socket currently open                                *
 *****************************************************************************/

METHOD(SWM_NTCPList_MaxSocket, SWP_NTCPList_MaxSocket *)
{
   LONG x, s, MaxS = 0;
   Object *lo;

   if (msg->rfds) FD_ZERO(msg->rfds);
   if (msg->wfds) FD_ZERO(msg->wfds);

   SW_MUIListLoop(o, x, lo, TRUE) {
      if ((s = DoMethod(lo, SWM_NTCP_MaxSocket)) >= 0) {
         if (msg->rfds)                                  FD_SET(s, msg->rfds);
         if (SWGetUL(lo, SWA_NTCP_SendSel) && msg->wfds) FD_SET(s, msg->wfds);

         MaxS = max(MaxS, s);
      }
   }

   SWDB_RET(MaxS);
} SDFT


/*****************************************************************************
 * Method: Discover if we`re online or not                                   *
 *****************************************************************************/

METHOD(SWM_NTCPList_Online, SWP_NTCPList_Online *)
{
   switch (msg->Mode) {
     case SWV_NTCPList_Online_On:
      if (!idata->SocketBase)
         idata->SocketBase = OpenLibrary("bsdsocket.library", 4);
      break;
      
     case SWV_NTCPList_Online_Off: {
        DoMethod(o, SWM_NList_ObjMethod, SWM_NTCP_Disconnect, FALSE);

        CloseLibrary(idata->SocketBase);
        idata->SocketBase = NULL;
        break;
     }

     case SWV_NTCPList_Online_Test: break;
        
     default: SWDB_RET(0);
   }

   set(o, SWA_NTCPList_Online, idata->SocketBase ? TRUE : FALSE);

   // -- send message back to event loop so it can open/close net lib --------
   
   DoMethod(NN_App(idata), MUIM_Application_ReturnID,
            idata->SWA_NTCPList_Online ? SWV_NTCPList_Online_On :
            SWV_NTCPList_Online_Off);
   
   SWDB_RET(idata->SWA_NTCPList_Online);
} SDFT


/*****************************************************************************
 * Method: Process menu event                                                *
 *****************************************************************************/

METHOD(SWM_AppBase_MenuCall, SWP_AppBase_MenuCall *)
{
  SWDB_RET(DoMethodA(idata->SWA_AppBase_TopObj, msg));
} SDFT
