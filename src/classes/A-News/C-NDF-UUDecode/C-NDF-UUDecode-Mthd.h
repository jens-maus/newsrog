
# include "S-GetEOL.h"

/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   idata->Art = msg->Art;
   idata->Grp = msg->Grp;
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   idata->Art = 
   idata->Grp = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  Buff = TxBuff(msg->Tx), t;
   ULONG   bol, eol, InEncode = FALSE;
   TEXT    AddObj[AV_AddOSize];
   ULONG   LastPos = 0, LastUPos = 0, Lines;
   AddData *AD;
   UBYTE   c;

   // -- skip headers --------------------------------------------------------

   bol = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   // -- look for uuencoded segments -----------------------------------------

   for (; Buff[bol]; bol = eol + 1) {
      eol = GetEOL(Buff, bol);

      // -- begin of uudencoded data -----------------------------------------
      
      if (!InEncode && SWUUIsBegin(Buff + bol)) {
         InEncode = TRUE;
         Lines    = 0;

         if (idata->TextOnly) {
            idata->UUFSpec = SWUUFileName(Buff + bol);
         } else {
            if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
               SWDB_RET(SWV_NDFilter_Incomplete);

            DoMethod(o, SWM_NDFilter_TBMultiInit, &AD->UUData, &AD->FName, NULL);

            c = Buff[eol], Buff[eol] = '\0';
            DoMethod(o, SWM_NDFilter_TBAppendTxt, &AD->FName,
                     SWUUFileName(Buff + bol));
            Buff[eol] = c;
         }
         
         if (bol)
            DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch,
                     Buff, LastPos, bol-1);

         LastUPos = bol;
      }

      // -- end of uuencoded data --------------------------------------------

      if (InEncode) {
         Lines++;
         
         if (SWUUIsEnd(Buff + bol)) {
            InEncode     = FALSE;

            if (Buff[eol+1] && SWUUSize(Buff+eol+1)) {
               bol = eol + 1;
               eol = GetEOL(Buff, bol);
            }
            
            if (idata->TextOnly) {
               idata->UULines = Lines;
               idata->UUSize  = bol - LastUPos;
               t = VL_Alloc(o, idata, idata->SWA_NDF_UUDecode_DeleteTxt);
               
               DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch, t);
               DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch, "\n");

               VL_Free(o, idata, t);
            } else {
               AD->Lines    = Lines;
               AD->Complete = TRUE;
   
               DoMethod(o, SWM_NDFilter_TBAppendSub, &AD->UUData, Buff,LastUPos,eol);
   
               DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
                        strcat((STRPTR)DoMethod(msg->ArtView, SWM_NArticleView_AddObj,
                                                AddObj,AD,o), "\n"));
   
               AD      = NULL;
            }

            LastPos = eol + 1;
         }
      }
   }

   // -- nothing done?  just return original buffer --------------------------

   if (!LastPos && !InEncode)
      SWDB_RET(SWV_NDFilter_Complete);

   // -- if still in uuencoded data, add error, else rest of buffer ----------

   if (InEncode) {
      if (idata->TextOnly) {
      } else {
         AD->Complete = FALSE;
   
         DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
                  strcat((STRPTR)DoMethod(msg->ArtView, SWM_NArticleView_AddObj,
                                          AddObj,AD,o), "\n"));
      }
   } else {
      DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch,
               Buff, LastPos, TxLen(msg->Tx));
   }

   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   SWDB_RET(InEncode ? SWV_NDFilter_Incomplete : SWV_NDFilter_Complete);
} SDFT


METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object *Obj, *DTImg = NULL;
   AddData *AD  = (AddData *)msg->Data;
   ULONG   Size = 0, Err;
   UBYTE   *Data = NULL;

   if (AD->Complete)
      Data  = SWUUDecode(TxBuff(&AD->UUData), TxSize(&AD->UUData),
                         &Size, NULL, &Err, NULL);

   if (Data && idata->SWA_NDF_UUDecode_DispInline) {
      TEXT    File[FMSIZE];

      sprintf(File, "t:NR.UU.%08lx", GetUniqueID());
           
      if (DoMethod(msg->ArtView, SWM_NArticleView_SaveData,
                   Data, Size, NULL, File) == SUCCESS) {
         DTImg = DTImgObject,
            ReadListFrame,
            SWA_DTImg_FSpec,  File,
            SWA_DTImg_Del,    TRUE,
            SWA_DTImg_Adjust, FALSE,
            SWA_DTImg_Sleep,  TRUE,
         End;
      }
   }

   Obj = AV_MimeBoxObject,
      SWA_AppBase_TopObj,       SWGetObj(msg->Grp, SWA_AppBase_TopObj),
      SWA_NDF_MimeBox_Data,     Data,
      SWA_NDF_MimeBox_DataSize, Size,
      SWA_NDF_MimeBox_SaveDir,  idata->SWA_NDF_UUDecode_SaveDir,
      SWA_NDF_MimeBox_FName,    TxBuff(&AD->FName),
      SWA_NDF_MimeBox_Grp,      msg->Grp,
      SWA_NDF_MimeBox_Art,      msg->Art,
      SWA_NDF_MimeBox_ArtView,  msg->ArtView,
      SWA_NDF_MimeBox_Img,      DTImg,
      SWA_NDF_MimeBox_Lines,    AD->Lines,
      SWA_NDF_MimeBox_DispSize, idata->SWA_NDF_UUDecode_DispSize,
      SWA_NDF_MimeBox_Prompt,   idata->SWA_NDF_UUDecode_Prompt,
      SWA_NDF_MimeBox_MakeDir,  idata->SWA_NDF_UUDecode_MakeDir,
      SWA_NDF_MimeBox_Status,
         !Data && AD->Complete           ? GetS(SWS_L_SError)  :
         !Data                           ? GetS(SWS_L_SIncomp) :
         Err                             ? SWDecodeErrStr(Err) :
         GetS(SWS_L_SOK),
   End;

   SWDB_RET(Obj);
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemAddObj, SWP_NDFilter_RemAddObj *)
{
   AddData *AD = (AddData *)msg->Data;

   if (!AD) SWDB_RET(0);

   DoMethod(o, SWM_NDFilter_TBMultiDispose, &AD->UUData, &AD->FName, NULL);

   NDF_FV(AD);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Find last line we should try parsing to                           *
 *****************************************************************************/

METHOD(SWM_NDFilter_LastParseLine, SWP_NDFilter_LastParseLine *)
{
   ULONG x, LastLine = msg->Last, InUU = FALSE;

   for (x=msg->First; x<=msg->Last; x++) {
      if (SWUUIsBegin(msg->Lines[x])) {
         InUU     = TRUE;
         LastLine = (x > msg->First) ? x - 1 : msg->First;
         continue;
      }

      if (SWUUIsEnd(msg->Lines[x]))
         InUU = FALSE;
   }

   if (!InUU) LastLine = msg->Last;

   SWDB_RET(LastLine);
} SDFT


/*****************************************************************************
 * Method: Modify buffer, but don't insert any objects                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxtNoObjs, ULONG *)
{
   ULONG rc;

   // -- fake call to SWM_NDFilter_ModTxt, and set TextOnly flag -------------
   
   idata->TextOnly = TRUE;
   msg[0]          = SWM_NDFilter_ModTxt;
   
   rc = DoMethodA(o, (Msg)msg);

   msg[0]          = SWM_NDFilter_ModTxtNoObjs;
   idata->TextOnly = FALSE;
   
   SWDB_RET(rc);
}


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   static TEXT VarBuf[16];
   static TEXT FName[64];
   STRPTR s;

   VL_FindRetNum("Size",  idata->UUSize,  VarBuf,  "%ld");
   VL_FindRetNum("Lines", idata->UULines, VarBuf,  "%ld");

   if (!strcmp(msg->Name, "Name")) {
      strncpy(FName, idata->UUFSpec, 63)[63] = '\0';

      for (s=FName; *s; s++)
         if (*s == '\n') { *s = '\0'; break; }

      SWDB_RET(FName);
   }

   if (s = (STRPTR)DoMethodA(idata->Art, (Msg)msg)) SWDB_RET(s);

   SWDB_RET(DoMethodA(idata->Grp, (Msg)msg));
} SDFT


