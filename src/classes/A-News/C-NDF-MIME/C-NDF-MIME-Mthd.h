
# include "S-GetEOL.h"
# include "S-ParseMimeHdrs.h"

static MTFunc(MimeSegment);

/*****************************************************************************
 * Function: Is MIME type                                                    *
 *****************************************************************************/

static LONG IsMimeType(MData *MD, STRPTR Type)
{
   STRPTR s = TxBuff(&MD->ContentType);

   return s && (Strnicmp(s, Type, strlen(Type)) == 0);
}


static LONG IsMimeTypeEmpty(MData *MD)
{
   return TxLen(&MD->ContentType) == 0;
}


/*****************************************************************************
 * Function: Should we ignore this MIME type?                                *
 *****************************************************************************/

static LONG NoDisplayType(SW_IDATA *idata, MData *MD)
{
   return idata->NoDspPat &&
      MatchPatternNoCase(idata->NoDspPat, TxBuff(&MD->ContentType));
}


/***************************************************************************
 * Function: Is proper MIME version?                                       *
 ***************************************************************************/

static LONG IsGoodMimeVer(STRPTR Line)
{
   TEXT   MimeData[sizeof(SWF_NAHeader_MimeVer)+4];
   STRPTR s;
   
   s = ParseMimeHdr(Line, SWF_NAHeader_MimeVer, NULL,
                    MimeData, sizeof(MimeData));

   if (!s) return FALSE;

   return strcmp(s, SWV_NDF_MIME_Ver_Str) == 0;
}


/*****************************************************************************
 * Function: Decode data based on encoding type (base64/etc)                 *
 *****************************************************************************/

static UBYTE *mimedecode(STRPTR Buff, ULONG *Size, MData *MD, ULONG *Err,
                         ULONG *Lines)
{
   return SWMimeDecode(Buff, 0, Size, Lines, Err,
                       TxBuff(&MD->ContentEncoding));
}


/*****************************************************************************
 * Function: Initialize a MData object                                       *
 *****************************************************************************/

static MData *InitMData(Object *o, MData *MD)
{
   if (MD)
      DoMethod(o, SWM_NDFilter_TBMultiInit,
               &MD->ContentType,
               &MD->ContentEncoding,
               &MD->ContentDesc,
               &MD->PartBoundary,
               &MD->FileName,
               NULL);

   return MD;
}


/*****************************************************************************
 * Function: make new MData for keeping track of MIME stuff                  *
 *****************************************************************************/

static MData *NewMData(Object *o, SW_IDATA *idata)
{
   MData *MD = (MData *)NDF_AV(sizeof(MData));

   return InitMData(o, MD);
}


/*****************************************************************************
 * Function: dispose MData structure                                         *
 *****************************************************************************/

static void DelMData(Object *o, SW_IDATA *idata, MData *MD)
{
   if (!MD) return;
   
   DoMethod(o, SWM_NDFilter_TBMultiDispose,
            &MD->ContentType,
            &MD->ContentEncoding,
            &MD->ContentDesc,
            &MD->PartBoundary,
            &MD->FileName,
            NULL);

   NDF_FV(MD);
}


/***************************************************************************
 * Function: push new MData structure onto a stack of them                 *
 ***************************************************************************/

static MData *PushMData(Object *o, SW_IDATA *idata, MData *MD)
{
   if (MD->MChild) return NULL;
   
   if (!(MD->MChild  = NewMData(o, idata)))
      return NULL;
   
   MD->MChild->MParent = MD;

   return MD->MChild;
}


/***************************************************************************
 * Function: pop new MData structure from a stack of them                  *
 ***************************************************************************/

static void PopMData(Object *o, SW_IDATA *idata, MData *MD)
{
   if (!MD->MParent) return;

   MD->MParent->MChild = NULL;

   DelMData(o, idata, MD);
}


/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   FData **FD = (FData **)msg->FltData;

   // -- parse no display pattern --------------------------------------------
   
   if (!SW_STRISNULL(idata->SWA_NDF_MIME_NoDispTypes)) {
      ULONG PBLen = strlen(idata->SWA_NDF_MIME_NoDispTypes) * 2 + 2;

      if (!(idata->NoDspPat = NDF_AV(PBLen)))
         SWDB_RET(FAILURE);

      if (ParsePatternNoCase(idata->SWA_NDF_MIME_NoDispTypes,
                             idata->NoDspPat, PBLen) < 0) {
         NDF_FV(idata->NoDspPat);
         idata->NoDspPat = NULL;
      }
   }

   // -- allocate buffers and stuff ------------------------------------------
   
   if (!(*FD = (FData *)NDF_AV(sizeof(FData))))
      SWDB_RET(FAILURE);

   if (!((*FD)->MD = NewMData(o, idata)))
      SWDB_RET(FAILURE);

   (*FD)->MD->BadMimeVer = TRUE;
   (*FD)->RecurCount     = 0;

   idata->Art = msg->Art;
   idata->Grp = msg->Grp;
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   FData **FD = (FData **)msg->FltData;

   if ((*FD)->MD)
      DelMData(o, idata, (*FD)->MD);

   NDF_FV(*FD);
   *FD = NULL;

   NDF_FV(idata->NoDspPat);
   idata->NoDspPat = NULL;
   
   idata->Art = 
   idata->Grp = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Scan MIME headers                                               *
 *****************************************************************************/

static ULONG ScanMimeHdrs(Object *o, SW_IDATA *idata, STRPTR Buff, MData *MD)
{
   STRPTR s;
   TEXT   HeaderVal[MI_HVSize];
   ULONG  bol, eol, l = 0;

   // -- look for MIME-Version header ----------------------------------------

   for (bol=0; Buff[bol] && Buff[bol]!='\n' && ++l<32; bol = eol + 1) {
      eol = GetEOL(Buff, bol);

      if (SWP_HasHeaderNoCase(Buff + bol, SWF_NAHeader_MimeVer))
         if (IsGoodMimeVer(Buff + bol))
            MD->BadMimeVer = FALSE;

      if (s = ParseMimeHdr(Buff + bol, SWF_NAHeader_MimeType, NULL,
                           HeaderVal, sizeof(HeaderVal))) {
         DoMethod(o, SWM_NDFilter_TBSet, &MD->ContentType,  HeaderVal);

         if (IsMimeType(MD, "multipart/")) {
            DoMethod(o, SWM_NDFilter_TBSet, &MD->PartBoundary, "--");

            if (s = ParseMimeHdr(Buff + bol, NULL, "boundary",
                                 HeaderVal, sizeof(HeaderVal)))
               DoMethod(o, SWM_NDFilter_TBAppendTxt, &MD->PartBoundary,
                        HeaderVal);
         }

         if (s = ParseMimeHdr(Buff + bol, NULL, "name",
                              HeaderVal, sizeof(HeaderVal)))
            DoMethod(o, SWM_NDFilter_TBSet, &MD->FileName, HeaderVal);

         continue;
      }

      if (s = ParseMimeHdr(Buff + bol, SWF_NAHeader_MimeCD, "filename",
                           HeaderVal, sizeof(HeaderVal))) {
         DoMethod(o, SWM_NDFilter_TBSet, &MD->FileName, HeaderVal);
         continue;
      }

      if (s = ParseMimeHdr(Buff + bol, SWF_NAHeader_MimeCTE, NULL,
                           HeaderVal, sizeof(HeaderVal))) {
         DoMethod(o, SWM_NDFilter_TBSet, &MD->ContentEncoding, HeaderVal);
         continue;
      }
   }

   if (TxLen(&MD->ContentType) != 0) {
      if (TxLen(&MD->FileName) == 0)
         DoMethod(o, SWM_NDFilter_TBSet, &MD->FileName, "unnamed");
   }

   return bol;
}

   
/*****************************************************************************
 * Function: See if line matches boundary marker                             *
 *****************************************************************************/

static LONG IsBoundary(STRPTR Buff, LONG bol, MData *MD)
{
   return TxLen(&MD->PartBoundary) > 2 &&
      !strncmp(Buff+bol, TxBuff(&MD->PartBoundary), TxLen(&MD->PartBoundary));
}


/*****************************************************************************
 * Function: Find next part boundary                                         *
 *****************************************************************************/

static LONG NextBoundary(STRPTR Buff, LONG StartPos, MData *MD)
{
   LONG bol, Found;
   MData *MDEnclose;

   // -- skip everything up to first boundary marker -------------------------

   for (bol=StartPos,Found=FALSE; Buff[bol]&&!Found; bol=GetEOL(Buff,bol)+1)
      if (Buff[bol] == '-' && Buff[bol+1] == '-') {
         if (IsBoundary(Buff, bol, MD))
            return bol;

         // -- test line against all enclosing multiparts to see if this -----
         // -- multipart ends prematurely ------------------------------------

         for (MDEnclose=MD->MParent; MDEnclose; MDEnclose=MDEnclose->MParent)
            if (IsMimeType(MDEnclose, "multipart/"))
               if (IsBoundary(Buff, bol, MDEnclose))
                  return -1;
      }

   return -1;
}


/*****************************************************************************
 * Function: return text for multipart MIME type                             *
 *****************************************************************************/

static MTFunc(MT_MultiPart)
{
   LONG  BoundaryPos, NextPos, ObjCount = 0;
   UBYTE c;
   MData *MDataPart;

   if ((BoundaryPos = NextBoundary(Buff, 0, MD)) < 0) {
      // error - no boundary markers
      return FALSE;
   }

   if (!OutTxt)
      MD->MimeObj = VGroup,
         MUIA_Group_Spacing, 10,
         MUIA_InnerBottom,   0,
         MUIA_InnerTop,      0,
         MUIA_InnerLeft,     0,
         MUIA_InnerRight,    0,
      End;

   BoundaryPos = GetEOL(Buff, BoundaryPos)+1;

   while (TRUE) {
      if ((NextPos = NextBoundary(Buff, BoundaryPos, MD)) < 0)
         break;

      c = Buff[NextPos], Buff[NextPos] = '\0';

      MDataPart = PushMData(o, idata, MD);
      MimeSegment(o, idata, Buff + BoundaryPos, MDataPart, msg, TRUE,
                  OutTxt);

      if (OutTxt) {
         // -- text-only mode ------------------------------------------------
         DoMethod(o, SWM_NDFilter_TBAppendTxt, OutTxt, "\n");
      } else {
         // -- binary mode - imbedded object ---------------------------------
         if (MDataPart->MimeObj) {
            Object *vg;
            
            sprintf(idata->TmpBuff, "\033c%s %s   %s %s",
                    GetS(SWS_L_PName),
                    TxBuff(&MDataPart->FileName),
                    GetS(SWS_L_PType),
                    TxBuff(&MDataPart->ContentType));
   
            DoMethod(MD->MimeObj, OM_ADDMEMBER,
                     vg = VGroup,
                        TextFrame,
                        MUIA_Background, MUII_ListBack,
                        Child, TextObject,
                           MUIA_Text_Contents, idata->TmpBuff,
                        End,
                     End);
   
            if (!SW_STRISNULL(TxBuff(&MDataPart->ContentDesc))) {
               DoMethod(vg, OM_ADDMEMBER,
                        TextObject,
                           MUIA_Text_Contents, TxBuff(&MDataPart->ContentDesc),
                           MUIA_Text_PreParse, "\033c",
                        End);
            }
   
            DoMethod(MD->MimeObj, OM_ADDMEMBER,
                     HGroup,
                        Child, RectangleObject,
                           MUIA_FixWidth, 10,
                        End,
                        Child, MDataPart->MimeObj,
                     End);
            ObjCount++;
         }
      }

      PopMData(o, idata, MDataPart);
      
      Buff[NextPos] = c;
      BoundaryPos   = GetEOL(Buff, NextPos) + 1;
   }

   if (!OutTxt && !ObjCount)
      DoMethod(MD->MimeObj, OM_ADDMEMBER, RectangleObject, End);

   return TRUE;
}


/*****************************************************************************
 * Function: return text for binary MIME objects                             *
 *****************************************************************************/

static MTFunc(MT_Binary)
{
   ULONG Size, Err, Lines;
   UBYTE *Data;
   Object *DTImg = NULL;
   STRPTR t;

   if (OutTxt) {
      // -- text-only mode ---------------------------------------------------
      
      idata->MDSize  = strlen(Buff);
      idata->MDLines = SWCountLines(Buff);
      idata->MD      = MD;
      
      if (t = VL_Alloc(o, idata, idata->SWA_NDF_MIME_DelTxtBin))
         DoMethod(o, SWM_NDFilter_TBAppendTxt, OutTxt, t);
      
      DoMethod(o, SWM_NDFilter_TBAppendTxt, OutTxt, "\n");
      
      VL_Free(o, idata, t);

      idata->MD = NULL;
   } else {
      // -- binary mode - imbedded object ------------------------------------
   
      if (!(Data = mimedecode(Buff, &Size, MD, &Err, &Lines)))
         return FALSE;
   
      if (Data && idata->SWA_NDF_MIME_DispInline) {
         TEXT    File[FMSIZE];
   
         sprintf(File, "t:NR.MB.%08lx", GetUniqueID());
         
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
      
      MD->MimeObj = AV_MimeBoxObject,
         SWA_AppBase_TopObj,       SWGetObj(msg->Grp, SWA_AppBase_TopObj),
         SWA_NDF_MimeBox_Data,     Data,
         SWA_NDF_MimeBox_DataSize, Size,
         SWA_NDF_MimeBox_SaveDir,  idata->SWA_NDF_MIME_SaveDir,
         SWA_NDF_MimeBox_FName,    TxBuff(&MD->FileName),
         SWA_NDF_MimeBox_Type,     TxBuff(&MD->ContentType),
         SWA_NDF_MimeBox_Grp,      msg->Grp,
         SWA_NDF_MimeBox_Art,      msg->Art,
         SWA_NDF_MimeBox_ArtView,  msg->ArtView,
         SWA_NDF_MimeBox_Img,      DTImg,
         SWA_NDF_MimeBox_Lines,    Lines,
         SWA_NDF_MimeBox_DispSize, idata->SWA_NDF_MIME_DispSize,
         SWA_NDF_MimeBox_Prompt,   idata->SWA_NDF_MIME_Prompt,
         SWA_NDF_MimeBox_MakeDir,  idata->SWA_NDF_MIME_MakeDir,
         SWA_NDF_MimeBox_Status,   !Data              ? GetS(SWS_L_SError)  :
                                   Err                ? SWDecodeErrStr(Err) :
                                   GetS(SWS_L_SOK),
      End;
   }

   return TRUE;
}


/*****************************************************************************
 * Function: return text for text MIME objects                               *
 *****************************************************************************/

static MTFunc(MT_Text)
{
   UBYTE *Data;
   ULONG Err, x, y, Size, Headers;
   TxtBuff Tx;
   
   if (!(Data = mimedecode(Buff, &Size, MD, &Err, NULL)))
      return FALSE;

   Data[Size] = '\0';

   DoMethod(o, SWM_NDFilter_TBInit, &Tx);

   Headers = IsMimeType(MD, "message/rfc822");

   // -- skip leading newlines in an RFC822 message --------------------------
   
   y=0;
   if (Headers)
      while (Data[y] && (Data[y] == '\n' || Data[y] == '\r'))
         y++;

   // -- normalize end of lines ----------------------------------------------

   for (x=0; Data[y]; y++, x++) {
      Data[x] = Data[y];
      if (Data[y] == '\n' && Data[y+1] == '\r') y++;
   }
   
   Data[y] = '\0';

   TxBuff(&Tx) = Data;
   TxLen(&Tx)  = 
   TxSize(&Tx) = y;

   DoMethod(msg->ArtView, SWM_NArticleView_GenTxt, &Tx, Headers,
            msg->Art, msg->Grp, NULL, TRUE,
            OutTxt ? SWM_NDFilter_ModTxtNoObjs : SWM_NDFilter_ModTxt);

   if (OutTxt) {
      // -- text-only mode ---------------------------------------------------
      DoMethod(o, SWM_NDFilter_TBAppend,    OutTxt, &Tx);
      DoMethod(o, SWM_NDFilter_TBAppendTxt, OutTxt, "\n");
   } else {
      // -- binary mode - imbedded object ------------------------------------
      MD->MimeObj = (Object *)DoMethod(msg->ArtView,SWM_NArticleView_GenObj,
                                       &Tx, NULL);
   }

   DoMethod(o, SWM_NDFilter_TBDispose, &Tx);
   
   return TRUE;
}


/*****************************************************************************
 * Function: see if this is a MIME segment                                   *
 *****************************************************************************/

static LONG IsMimeSegment(MData *MD)
{
   return !MD->BadMimeVer &&
      (!TxIsEmpty(&MD->ContentEncoding) || !TxIsEmpty(&MD->ContentType));
}


/*****************************************************************************
 * Function: return text for a MIME segment                                  *
 *****************************************************************************/

static MTFunc(MimeSegment)
{
   Buff += ScanMimeHdrs(o, idata, Buff, MD);

   // -- return if this wasn`t a MIME document -------------------------------

   if (!IsMimeSegment(MD))
      return FALSE;

   // -- skip things that match our no-display pattern -----------------------
   
   if (NoDisplayType(idata, MD))
      return TRUE;

   // -- handle various types ------------------------------------------------
   
   if (IsMimeType(MD, "multipart/"))
      return MTCall(MT_MultiPart);

   if (IsMimeType(MD, "text/")          ||
       IsMimeType(MD, "message/rfc822") ||
       IsMimeTypeEmpty(MD))
      return MTCall(MT_Text);
  
   if (IsMimeType(MD, "image/")        ||
       IsMimeType(MD, "audio/")        ||
       IsMimeType(MD, "video/")        ||
       IsMimeType(MD, "message/")      ||
       IsMimeType(MD, "application/"))
      return MTCall(MT_Binary);
 
   return TRUE;
}


/*****************************************************************************
 * See if this article has MIME headers                                      *
 *****************************************************************************/

static ULONG HasMimeHdrs(Object *o, SW_IDATA *idata,
                         SWP_NDFilter_ModTxt *msg, STRPTR Buff)
{
   ULONG IsMime;
   MData MD;

   if (!*msg->HasHeaders) return FALSE;
      
   InitMData(o, &MD);
   ScanMimeHdrs(o, idata, Buff, &MD);
   IsMime = IsMimeSegment(&MD);

   return IsMime;
}
   


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   FData  *FD = (FData *)msg->FltData;
   MData  *MD = FD->MD;
   STRPTR  Buff = TxBuff(msg->Tx);
   TEXT    AddObj[AV_AddOSize];
   AddData *AD;
   ULONG   HdrPos;

   if (FD->RecurCount)
      SWDB_RET(SWV_NDFilter_Complete);

   // -- if this isn`t a MIME article, do nothing ----------------------------

   if (!HasMimeHdrs(o, idata, msg, Buff))
      SWDB_RET(SWV_NDFilter_Complete);

   // -- if not at end of article, display msg & keep reading ----------------

   if (!msg->EndOfData) {
      DoMethod(o, SWM_NDFilter_TBSet,       msg->Tx, GetS(SWS_L_Reading));
      DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx, "\n");

      SWDB_RET(SWV_NDFilter_Incomplete);
   }

   // -- increment recursion count -------------------------------------------

   FD->RecurCount++;

   // -- parse MIME segment --------------------------------------------------

   if (!MimeSegment(o, idata, Buff, MD, msg, msg->EndOfData, NULL)) {
      FD->RecurCount--;
      SWDB_RET(SWV_NDFilter_Complete);
   }

   // -- add object ----------------------------------------------------------

   if (*msg->HasHeaders) {
      if (!(AD = (AddData *)NDF_AV(sizeof(AddData)))) {
         FD->RecurCount--;
         SWDB_RET(SWV_NDFilter_Incomplete);
      }

      AD->MimeTop = FD->MD->MimeObj;

      HdrPos = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

      if (HdrPos)
         DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch,
                  TxBuff(msg->Tx), 0, HdrPos-1);

      DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
               strcat((STRPTR)DoMethod(msg->ArtView, SWM_NArticleView_AddObj,
                                       AddObj,AD,o), "\n"));
   }

   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   FD->RecurCount--;
   SWDB_RET(SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object *Obj;
   AddData *AD  = (AddData *)msg->Data;

   Obj         = AD->MimeTop;
   AD->MimeTop = NULL;

   SWDB_RET(Obj);
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemAddObj, SWP_NDFilter_RemAddObj *)
{
   AddData *AD = (AddData *)msg->Data;

   if (!AD) SWDB_RET(0);

   MUI_DisposeObject(AD->MimeTop);

   NDF_FV(AD);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Find last line we should try parsing to                           *
 *****************************************************************************/

METHOD(SWM_NDFilter_LastParseLine, SWP_NDFilter_LastParseLine *)
{
   SWDB_RET(msg->Last);
} SDFT


/*****************************************************************************
 * Method: Modify buffer, but don't insert any objects                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxtNoObjs, SWP_NDFilter_ModTxt *)
{
   FData  *FD = (FData *)msg->FltData;
   MData  *MD = FD->MD;
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG  HdrPos;

   // -- if this isn`t a MIME article, do nothing ----------------------------

   if (!HasMimeHdrs(o, idata, msg, Buff))
      SWDB_RET(SWV_NDFilter_Complete);

   // -- if not at end of article, display msg & keep reading ----------------

   if (!msg->EndOfData) {
      DoMethod(o, SWM_NDFilter_TBSet, msg->Tx, GetS(SWS_L_Incomp));

      FD->RecurCount--;
      SWDB_RET(SWV_NDFilter_Incomplete);
   }

   // -- increment recursion count -------------------------------------------

   FD->RecurCount++;

   // -- Add headers if needed -----------------------------------------------
   
   if (HdrPos = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders))
      DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch,
               TxBuff(msg->Tx), 0, HdrPos-1);

   // -- parse MIME segment, text only ---------------------------------------

   if (!MimeSegment(o, idata, Buff, MD, msg, msg->EndOfData, msg->Scratch)) {
      FD->RecurCount--;
      SWDB_RET(SWV_NDFilter_Complete);
   }

   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   FD->RecurCount--;
   
   SWDB_RET(SWV_NDFilter_Complete);
}


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   static TEXT VarBuf[16];
   STRPTR s;

   if (idata->MD) {
      VL_FindRetNum("Size",  idata->MDSize,  VarBuf,  "%ld");
      VL_FindRetNum("Lines", idata->MDLines, VarBuf,  "%ld");
      VL_FindRet("MIMEType", TxBuff(&idata->MD->ContentType));

      s = TxBuff(&idata->MD->ContentDesc);
      VL_FindRet("MIMEDesc", s ? s : "");

      s = TxBuff(&idata->MD->FileName);
      VL_FindRet("Name", s ? s : "");
   }

   if (s = (STRPTR)DoMethodA(idata->Art, (Msg)msg)) SWDB_RET(s);

   SWDB_RET(DoMethodA(idata->Grp, (Msg)msg));
} SDFT
