
/*****************************************************************************
 * Function: find default server                                             *
 *****************************************************************************/

static Object *DefSrv(SW_IDATA *idata)
{
   ULONG x;
   Object *lo;

   // -- first see if there already is a default server ----------------------
   
   SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
      if (SWGetUL(lo, SWA_NServer_Default) &&
          SWGetUL(lo, SWA_NServer_Type) == SWV_NServer_Type_NNTP)
         return lo;

   // -- didn`t find one - so make one ---------------------------------------
   
   SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
      if (SWGetUL(lo, SWA_NServer_Type) == SWV_NServer_Type_NNTP) {
         set(lo, SWA_NServer_Default, TRUE);
         return lo;
      }

   return NULL;
}


/*****************************************************************************
 * Method: MUI Setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   if (!DoSuperMethodA(cl, o, msg)) SWDB_RET(FALSE);

   SWMakeImg(idata, SWV_Img_True,  CheckMark(TRUE));
   SWMakeImg(idata, SWV_Img_False, CheckMark(FALSE));

   SWCreateImages(idata);
   
   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: MUI Cleanup                                                       *
 *****************************************************************************/
   
METHOD(MUIM_Cleanup, Msg)
{
   SWDeleteImages(idata);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Set the default server                                            *
 *****************************************************************************/

METHOD(SWM_NServerList_ResetDefault, Msg)
{
   Object *Def;

   if (Def = DefSrv(idata))
      set(Def, SWA_NServer_Default, FALSE);

   SWDB_RET(SUCCESS);   
} SDFT


/*****************************************************************************
 * Method: Process menu event                                                *
 *****************************************************************************/

METHOD(SWM_AppBase_MenuCall, SWP_AppBase_MenuCall *)
{
   switch (msg->Call) {
     case 0:
     default: SWDB_RET(DoMethodA(idata->SWA_AppBase_TopObj, msg));
   }
} SDFT


/*****************************************************************************
 * Method: Look for NNTPSERVER env var, make new server if found             *
 *****************************************************************************/

METHOD(SWM_NServerList_NNTPSERVER, SWP_NServerList_NNTPSERVER *)
{
   Object *s;
   UBYTE Serv[FMSIZE];
   
   if (msg->CheckErr)
      DoMethod(o, SWM_ClearObjError);
   
   if (GetVar("NNTPSERVER", Serv, FMSIZE, 0) > 0) {
      if (s = MkInsObj(idata, NULL)) {
         set(s, SWA_NServer_Host, Serv);
         DoMethod(o, SWM_NList_Add, s, NULL);
      }
   } else
      if (msg->CheckErr)
         SWERR(o, NULL, SWS_Err_NoEnv, 0, 0);

   if (msg->CheckErr)
      DoMethod(o, SWM_Root_CheckErr, _win(o));
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: get the default server to send to                                 *
 *****************************************************************************/

METHOD(SWM_NServerList_GetDefault, ULONG *)
{
   Object *SDef = DefSrv(idata),
          *Srv = NULL;
   UBYTE  *Name = GetTagUB(SWT_NServerList_ServerName, NULL, msg+1);

   if (!SW_STRISNULL(Name))
      Srv = (Object *)DoMethod(o, SWM_NList_FindObj, SWA_NServer_Host, Name);

   if (!Srv && !SDef) {
      DoMethod(o, SWM_NServerList_NNTPSERVER, FALSE);
      SDef = DefSrv(idata);
   }

   if (!Srv && !SDef)
      SWDB_RET(SWERR(o, NULL, SWS_Err_NoServ, 0, 0));

   SWDB_RET(Srv ? Srv : SDef);
} SDFT


/*****************************************************************************
 * Method: get PopSrvList class pointer                                      *
 *****************************************************************************/

METHOD(SWM_NServerList_PopSrvListClass, Msg)
{
   SWDB_RET(PopSrvListClass());
} SDFT


/*****************************************************************************
 * Method: Find a server, and create one if we didn't find it                *
 *****************************************************************************/

METHOD(SWM_NServerList_FindCreate, SWP_NServerList_FindCreate *)
{
   Object *Srv;
   
   if (!msg->ServName) SWDB_RET(NULL);

   Srv = (Object *)DoMethod(o, SWM_NList_FindObj,
                            SWA_NServer_Host, msg->ServName);

   if (Srv && SWGetUL(Srv, SWA_NServer_Type != msg->ServType))
      SWDB_RET(NULL);
   
   if (!Srv) {
      if (Srv = (Object *)DoMethod(o, SWM_NList_Add, NULL))
         SetAttrs(Srv,
                  SWA_NServer_Host, msg->ServName,
                  SWA_NServer_Type, msg->ServType,
                  TAG_DONE);
   }
   
   SWDB_RET(Srv);
} SDFT


/*****************************************************************************
 * Function: Find server                                                     *
 *****************************************************************************/

static Object *FindServ(Object *o, SW_IDATA *idata, STRPTR Name)
{
   Object *g;
   
   if (Name[0] >= '0' && Name[0] <= '9' && !strchr(Name, '.')) {
      // -- search by number -------------------------------------------------
      DoMethod(idata->SWA_NList_L, MUIM_List_GetEntry, atol(Name), &g);
      return g;
   }

   // -- search by name ------------------------------------------------------
   
   return (Object *)DoMethod(o, SWM_NList_FindObj, SWA_NServer_Host, Name);
}


/*****************************************************************************
 * Method: process arexx command                                             *
 *****************************************************************************/

METHOD(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *)
{
   STRPTR cmd = ARG0(msg->Rx);
   Object *Srv;
   LONG x;
   UBYTE TmpBuff[32];

   // -- SERVEREXISTS --------------------------------------------------------

   if (!strncmp(cmd, "SERVEREXISTS ", 13)) {
      set(msg->App, MUIA_Application_RexxString, FindServ(o, idata, cmd+13) ? "1" :"0");
      goto OK;
   }

   // -- SERVERCOUNT ---------------------------------------------------------

   if (!strcmp(cmd, "SERVERCOUNT")) {
      SW_MUIListLoop(idata->SWA_NList_L, x, Srv, TRUE);
      sprintf(TmpBuff, "%ld", x);
      set(msg->App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }

   // -- GETSERVER -----------------------------------------------------------

   if (!strncmp(cmd, "GETSERVER ", 10)) {
      x   = atol(cmd+10);
      DoMethod(o, MUIM_List_GetEntry, x, &Srv);
      if (!Srv) goto Err;
      
      set(msg->App, MUIA_Application_RexxString, SWGetStr(Srv, SWA_NServer_Host));
      goto OK;
   }

   // -- ADDSERVER -----------------------------------------------------------

   if (!strncmp(cmd, "ADDSERVER ", 10)) {
      if (FindServ(o, idata, cmd+10))        goto Err;
      if (!(Srv = MkInsObj(idata, cmd+10)))  goto Err;
      
      DoMethod(o, MUIM_List_InsertSingle, Srv, MUIV_List_Insert_Bottom);
      goto OK;
   }

   // -- GETDEFSERVER --------------------------------------------------------

   if (!strcmp(cmd, "GETDEFSERVER")) {
      if (!(Srv = DefSrv(idata))) goto Err;
      set(msg->App, MUIA_Application_RexxString, SWGetStr(Srv, SWA_NServer_Host));
      goto OK;
   }

   // -- SETDEFSERVER --------------------------------------------------------

   if (!strncmp(cmd, "SETDEFSERVER ", 13)) {
      if (!(Srv = FindServ(o, idata, cmd+13))) goto Err;
      
      DoMethod(o, SWM_NServerList_ResetDefault);
      set(Srv, SWA_NServer_Default, TRUE);
      goto OK;
   }

   SWDB_RET(SWV_AppBase_RexxNotFound);

  OK:
   *msg->rc = RC_OK;
   SWDB_RET(SWV_AppBase_RexxFound);

  Err:
   *msg->rc = RC_ERROR;
   SWDB_RET(SWV_AppBase_RexxFound);
} SDFT
