# include "C-NGroupList-I.h"

# ifndef _GENPROTO


/*****************************************************************************
 * Function: Create a new group                                              *
 *****************************************************************************/

Object *MkNewGrp(Object *o, SW_IDATA *idata, STRPTR Name)
{
   if (SWGetUL(idata->SWA_NList_L, MUIA_List_Entries) < SWV_NGroupList_MaxUnreg ||
       NN_RegMsg(idata, GetS(SWS_L_UnregMsg), _win(o)) == SUCCESS)
      return MUI_NewObject(SWC_NGroup,
                           AppBaseDefTop(idata),
                           SWA_NGroup_Name, SW_STRDEF(Name, ""),
                           TAG_DONE);
   
   return NULL;
}

/*****************************************************************************
 * Function: Search for a group by name or position in the list              *
 *****************************************************************************/

Object *FindGroup(SW_IDATA *idata, STRPTR Name)
{
   Object *g;
   ULONG  x;

   if (SW_STRISNULL(Name)) return NULL;

   if (Name[0] >= '0' && Name[0] <= '9' && !strchr(Name, '.')) {
      // -- search by group number -------------------------------------------
      DoMethod(idata->SWA_NList_L, MUIM_List_GetEntry, atol(Name), &g);
      return g;
   } else {
      // -- search by name ---------------------------------------------------
      SW_MUIListLoop(idata->SWA_NList_L, x, g, TRUE)
         if (strcmp(SWGetStr(g, SWA_NGroup_Name), Name) == 0)
            return g;
   }

   return NULL;
}

   
/*****************************************************************************
 * Method: process arexx command                                             *
 *****************************************************************************/

METHOD_NOINLINE(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *)
{
   STRPTR cmd = ARG0(msg->Rx);
   Object *Grp, *App = msg->App;
   LONG x;
   UBYTE TmpBuff[32];

   // -- ONLINE --------------------------------------------------------------

   if (!strcmp(cmd, "ONLINE")) {
      DoMethod(NN_TCPList(idata), SWM_NTCPList_Online, SWV_NTCPList_Online_On);
      goto OK;
   }

   // -- ISONLINE ------------------------------------------------------------

   if (!strcmp(cmd, "ISONLINE")) {
      set(App, MUIA_Application_RexxString, idata->Online ? "1" : "0");
      goto OK;
   }

   // -- OFFLINE -------------------------------------------------------------

   if (!strcmp(cmd, "OFFLINE")) {
      DoMethod(NN_TCPList(idata), SWM_NTCPList_Online, SWV_NTCPList_Online_Off);
      goto OK;
   }

   // -- OPENGROUP -----------------------------------------------------------

   if (!strncmp(cmd, "OPENGROUP ", 10)) {
      if (!(Grp = FindGroup(idata, cmd+10)))
         goto Err;
         
      DoMethod(Grp, SWM_NGroup_StartRead);
      goto OK;
   }

   // -- ISGROUPOPEN ---------------------------------------------------------

   if (!strncmp(cmd, "ISGROUPOPEN ", 12)) {
      if (!(Grp = FindGroup(idata, cmd+12)))
         goto Err;
         
      set(App, MUIA_Application_RexxString,
          SWGetUL(Grp, SWA_NGroup_ReaderOpen) ? "1" : "0");
      goto OK;
   }

   // -- GROUPEXISTS ---------------------------------------------------------

   if (!strncmp(cmd, "GROUPEXISTS ", 12)) {
      set(App, MUIA_Application_RexxString, FindGroup(idata, cmd+12)?"1" :"0");
      goto OK;
   }

   // -- GROUPCOUNT ----------------------------------------------------------

   if (!strcmp(cmd, "GROUPCOUNT")) {
      SW_MUIListLoop(idata->SWA_NList_L, x, Grp, TRUE);
      sprintf(TmpBuff, "%ld", x);
      set(App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }

   // -- GETGROUP ------------------------------------------------------------

   if (!strncmp(cmd, "GETGROUP ", 9)) {
      Grp = FindGroup(idata, cmd+9);
      if (!Grp) goto Err;
      
      set(App, MUIA_Application_RexxString, SWGetStr(Grp, SWA_NGroup_Name));
      goto OK;
   }

   // -- CLOSEGROUP ----------------------------------------------------------
   
   if (!strncmp(cmd, "CLOSEGROUP ", 11)) {
      ULONG  Save = TRUE, Force = FALSE;
      STRPTR Params;
      
      cmd += 11;
      if (Params = strchr(cmd, ' ')) {
         if (strstr(Params, "NOSAVE")) Save  = FALSE;
         if (strstr(Params, "FORCE"))  Force = TRUE;
      }
      
      if (!(Grp = FindGroup(idata, cmd)))
         goto Err;
         
      DoMethod(Grp, SWM_NGroup_StopRead, Save, Force);
      goto OK;
   }
   
   // -- UPDCOUNT ------------------------------------------------------------

   if (!strncmp(cmd, "UPDCOUNT ", 9)) {
      if (!(Grp = FindGroup(idata, cmd+9)))
         goto Err;

      DoMethod(Grp, SWM_NGroup_CheckStats);
      goto OK;
   }

   // -- MARKREAD ------------------------------------------------------------

   if (!strncmp(cmd, "MARKREAD ", 9)) {
      if (!(Grp = FindGroup(idata, cmd+9)))
         goto Err;
         
      DoMethod(Grp, SWM_NGroup_GL_Mark, TRUE);
      goto OK;
   }

   // -- MARKUNREAD ----------------------------------------------------------

   if (!strncmp(cmd, "MARKUNREAD ", 11)) {
      if (!(Grp = FindGroup(idata, cmd+11)))
         goto Err;
         
      DoMethod(Grp, SWM_NGroup_GL_Mark, FALSE);
      goto OK;
   }

   // -- SORTGROUPS ----------------------------------------------------------

   if (!strcmp(cmd, "SORTGROUPS")) {
      DoMethod(o, MUIM_List_Sort);
      goto OK;
   }

   // -- ADDGROUP ------------------------------------------------------------

   if (!strncmp(cmd, "ADDGROUP ", 9)) {
      if (FindGroup(idata, cmd+9))            goto Err;
      if (!(Grp = MkNewGrp(o, idata, cmd+9))) goto Err;
      
      DoMethod(o, MUIM_List_InsertSingle, Grp, MUIV_List_Insert_Bottom);
      goto OK;
   }

   // -- OPENSERVERGROUPS ----------------------------------------------------

   if (!strcmp(cmd, "OPENSERVERGROUPS")) {
      DoMethod(o, SWM_NGroupList_OpenSGWin);
      goto OK;
   }

   // -- CLOSESERVERGROUPS ---------------------------------------------------

   if (!strcmp(cmd, "CLOSESERVERGROUPS")) {
      DoMethod(o, SWM_NGroupList_CloseSGWin);
      goto OK;
   }

   // -- CLEARSERVERGROUPS ---------------------------------------------------

   if (!strcmp(cmd, "CLEARSERVERGROUPS")) {
      if (!idata->SGWin) goto Err;
      
      DoMethod(idata->SWA_NGroupList_SG_L, MUIM_List_Clear);
      goto OK;
   }

   // -- FETCHSERVERGROUPS ---------------------------------------------------

   if (!strncmp(cmd, "FETCHSERVERGROUPS", 17)) {
      STRPTR Pat = "*";
      if (!idata->SGWin) goto Err;

      if (cmd[17] != '\0' && cmd[18] != '\0')
         Pat = cmd + 18;
      
      set(idata->s_FetchPat, MUIA_String_Contents, Pat);
      DoMethod(o, SWM_NGroupList_FetchSG);
      
      goto OK;
   }

   // -- SGFETCHPENDING ------------------------------------------------------

   if (!strcmp(cmd, "SGFETCHPENDING")) {
      set(App, MUIA_Application_RexxString, idata->GL_Srv ? "1" : "0");
      goto OK;
   }

   // -- SERVERGROUPCOUNT ----------------------------------------------------

   if (!strcmp(cmd, "SERVERGROUPCOUNT")) {
      if (!idata->SGWin) goto Err;

      sprintf(TmpBuff, "%ld", SWGetUL(idata->SWA_NGroupList_SG_L, MUIA_List_Entries));

      set(App, MUIA_Application_RexxString, TmpBuff);
      goto OK;
   }

   // -- GETSERVERGROUP ------------------------------------------------------

   if (!strncmp(cmd, "GETSERVERGROUP ", 15)) {
      if (!idata->SGWin) goto Err;

      x   = atol(cmd+15);
      DoMethod(idata->SWA_NGroupList_SG_L, MUIM_List_GetEntry, x, &Grp);
      if (!Grp) goto Err;
      
      set(App, MUIA_Application_RexxString, Grp);
      goto OK;
   }

   // -- group cmd -----------------------------------------------------------

   if (!strncmp(cmd, "GROUPCMD ", 9)) {
      STRPTR GrpCmd, OrigCmd;
      UBYTE  c;
      cmd += 9;
      GrpCmd = strchr(cmd, ' ');

      if (SW_STRISNULL(GrpCmd) || GrpCmd[1] == '\0') goto Err;

      c = *GrpCmd, *GrpCmd = '\0';
      Grp = FindGroup(idata, cmd);
      *GrpCmd = c;

      if (!Grp) goto Err;
      
      GrpCmd++;
      OrigCmd       = ARG0(msg->Rx);
      ARG0(msg->Rx) = GrpCmd;
      x             = DoMethodA(Grp, msg);
      ARG0(msg->Rx) = OrigCmd;
      SWDB_RET(x);
   }


   SWDB_RET(SWV_AppBase_RexxNotFound);

  OK:
   *msg->rc = RC_OK;
   SWDB_RET(SWV_AppBase_RexxFound);

  Err:
   *msg->rc = RC_ERROR;
   SWDB_RET(SWV_AppBase_RexxFound);
} SDFT

# endif
