# include "C-NServer-I.h"


# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NServer_Host,       SWAM_NoLab, SWAM_SGI,  NULL, 256),
   SWAM_IN(SWA_NServer_Port,       SWAM_NoLab, SWAM_SGI,  SWV_NServer_NNTPPort),
   SWAM_IN(SWA_NServer_ConnectT,   SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NServer_ConnectS,   SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NServer_AvgCPS,     SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NServer_CnDays,     SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NServer_CnMin,      SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NServer_CnTick,     SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NServer_NGroups,    SWAM_NoLab, SWAM_SGI,  0),
   SWAM_PT(SWA_NServer_MemPool,    SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_BO(SWA_NServer_Default,    SWAM_NoLab, SWAM_SGI,  FALSE),
   SWAM_IN(SWA_NServer_MaxRetries, SWAM_NoLab, SWAM_SGI,  5),
   SWAM_IN(SWA_NServer_CxTimeout,  SWAM_NoLab, SWAM_SGI,  1000*30),
   SWAM_IN(SWA_NServer_RetryDelay, SWAM_NoLab, SWAM_SGI,  1000*20),
   SWAM_IN(SWA_NServer_PostOK,     SWAM_NoLab, SWAM_SGIN, FALSE),
   SWAM_SA(SWA_NServer_AuthUser,   SWAM_NoLab, SWAM_SGI,  NULL, 256),
   SWAM_SA(SWA_NServer_AuthPass,   SWAM_NoLab, SWAM_SGI,  NULL, 256),
   SWAM_IN(SWA_NServer_Type,       SWAM_NoLab, SWAM_SGI,  SWT_NServer_Type_NNTP),
   
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Waiting: TCP connect...",
   "Waiting: NNTP connect...",
   "Waiting: FTP connect...",
   "Waiting: Cmd results...",
   "Retry Delay (%ld)...",
   "NNTP Authentication...",
   "FTP Login...",

   "Fetching FTP directory...",
   "Fetching FTP file...",
   "FTP Data Connection...",

   SWX_B("Error: ") "Operation cannot be\nperformed in offline mode.",
   SWX_B("Error: ") "User authentication failed.  This may be\n"
      "due to an invalid user ID or password.",
   SWX_B("Error: ") "Authentication requested by server,\n"
      "but no user information was defined.",
   SWX_B("Error: ") "Authentication requested by server,\n"
      "but no password information was defined.",
   
   NULL,
};

static ULONG CreateObj(Object *o, SW_IDATA *idata)
{
   setlent(o, SWA_ObjNoChildCopy, TRUE);

   return SUCCESS;
}


/*****************************************************************************
 * Function: abort all TCP connections                                       *
 *****************************************************************************/

static void AbortAll(Object *o, SW_IDATA *idata)
{
   ULONG x;

   for (x=0; x<SWV_NServer_MaxCmd; x++)
      DoMethod(o, SWM_NServer_DelCmd, x);
}


/*****************************************************************************
 * Function: Clear FTP command data after we're done                         *
 *****************************************************************************/

static void ClearFTP(Object *o, SW_IDATA *idata, LONG ID)
{
   struct FTPCmd *ftpc = &idata->FTPCmd[ID];

   if (ID < 0) return;

   ftpc->Connected = FALSE;

   if (NN_TCPList(idata) && ftpc->DataCnx)
      DoMethod(NN_TCPList(idata), SWM_NList_RemoveObj, ftpc->DataCnx);

   FV(ftpc->Str1);
   FV(ftpc->DataBuff);
   
   memset(ftpc, 0, sizeof(*ftpc));
}


static void ClearFTPAll(Object *o, SW_IDATA *idata)
{
   ULONG x;

   for (x=0; x<SWV_NServer_MaxFTPCmd; x++)
      ClearFTP(o, idata, x);
}


static void DestroyObj(Object *o, SW_IDATA *idata)
{
   AbortAll(o, idata);
   ClearFTPAll(o, idata);

   if (idata->DBSend) Close(idata->DBSend);
   if (idata->DBRecv) Close(idata->DBRecv);
}


# define SETUPCUST  if (CreateObj((Object *)retval, idata) != SUCCESS) \
                        SWDB_RET(FAILURE);

# define DISPOSECUST DestroyObj(o, idata);
   
# define SW_USERSET                                               \
     if (Tag == SWA_AppBase_TopObj)                               \
       idata->SWA_AppBase_TopObj  = (Object *)Data,               \
       idata->SWA_NServer_MemPool = NN_MemPool(idata);            \
     if (Tag == SWA_NServer_Default &&                            \
         idata->SWA_NServer_Type == SWV_NServer_Type_FTP)         \
         Data = FALSE;                                            \
     if (Tag == SWA_NServer_Type && Data == SWV_NServer_Type_FTP) \
         idata->SWA_NServer_Default = FALSE

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods                    \
  SW_DO_MF(SWM_NServer_SendCmd);       \
  SW_DO_MF(SWM_NServer_RecvLine);      \
  SW_DO_MF(SWM_NServer_DelLines);      \
  SW_DO_MF(SWM_NServer_AdoptLines);    \
  SW_DO_MF(SWM_NServer_Abort);         \
  SW_DO_MF(SWM_NServer_DelCmd);        \
  SW_DO_MF(SWM_NServer_ResetStats);    \
  SW_DO_MF(SWM_NServer_Connect);       \
  SW_DO_MF(SWM_NServer_Connected);     \
  SW_DO_MF(SWM_NServer_Done);          \
  SW_DO_MF(SWM_NServer_CxRetry);       \
  SW_DO_MF(SWM_NServer_CxCancel);      \
  SW_DO_MF(SWM_NServer_GetTCP);        \
  SW_DO_MF(SWM_NServer_RecvBin);       \
  SW_DO_MF(SWM_NServer_GetFTPDir);     \
  SW_DO_MF(SWM_NServer_GetFTPDir_P1);  \
  SW_DO_MF(SWM_NServer_GetFTPFile);    \
  SW_DO_MF(SWM_NServer_GetFTPFile_P1); \
  SW_DO_MF(SWM_NServer_AbortFTP);      \

# include "C-NServer-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
