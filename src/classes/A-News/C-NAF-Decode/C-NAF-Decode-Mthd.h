# include "S-MSBaseName.h"
# include "S-ParseMimeHdrs.h"

static LONG DecodeMime(Object *o, SW_IDATA *idata, Object *Grp,
                       STRPTR *Lines, ULONG LineMax, MData *MD);

/*****************************************************************************
 * Function: make new MData for keeping track of MIME stuff                  *
 *****************************************************************************/

static MData *NewMData(void)
{
   return (MData *)AllocVec(sizeof(MData), MEMF_ANY|MEMF_CLEAR);
}

/*****************************************************************************
 * Function: dispose MData structure                                         *
 *****************************************************************************/

static void DelMData(MData *MD)
{
   FreeVec(MD);
}


/***************************************************************************
 * Function: push new MData structure onto a stack of them                 *
 ***************************************************************************/

static MData *PushMData(MData *MD)
{
   if (MD->MChild) return NULL;
   
   if (!(MD->MChild  = NewMData()))
      return NULL;
   
   MD->MChild->MParent = MD;

   return MD->MChild;
}


/***************************************************************************
 * Function: pop new MData structure from a stack of them                  *
 ***************************************************************************/

static void PopMData(MData *MD)
{
   if (!MD->MParent) return;

   MD->MParent->MChild = NULL;

   DelMData(MD);
}


/*****************************************************************************
 * Method: Start action                                                      *
 *****************************************************************************/

METHOD(SWM_NAFilter_Begin, SWP_NAFilter_Begin *)
{
   idata->Count = 0;
   
   SWDB_RET(SWV_NAFilter_NeedBody);
} SDFT


/*****************************************************************************
 * Function: Get uuencoded region for later decoding, return end line        *
 *****************************************************************************/

static STRPTR GetRegion(STRPTR *ArtTxt, ULONG Lines, ULONG *BuffLen)
{
   ULONG x, ArtSize;
   STRPTR Buff, bp;

   // -- generate the data for decoding --------------------------------------

   for (x=ArtSize=0; x<Lines; x++)
      ArtSize += strlen(ArtTxt[x]) + 1;

   if (!(Buff = bp = AllocVec(ArtSize+2, MEMF_ANY)))
      return NULL;

   *BuffLen = ArtSize;

   for (x=*BuffLen=0; x < Lines; x++) {
      bp += strlen(strcpy(bp, ArtTxt[x]));
      bp += strlen(strcpy(bp, "\n"));
   }

   return Buff;
}


static LONG IsMimeType(MData *MD, STRPTR Type)
{
   return Strnicmp(MD->ContentType, Type, strlen(Type)) == 0;
}


/*****************************************************************************
 * Method: See if this is beginning of MIME data                             *
 *****************************************************************************/

static LONG ParseMimeHdrs(STRPTR *Lines, ULONG LineMax, MData *MD)
{
   LONG x;

   for (x=0; x<LineMax && Lines[x][0] != '\0'; x++) {
      if (ParseMimeHdr(Lines[x], SWF_NAHeader_MimeType, NULL,
                       MD->ContentType, MimeTypeLen)) {
         if (IsMimeType(MD, "multipart/")) {
            strcpy(MD->Boundary, "--");
            ParseMimeHdr(Lines[x], NULL, "boundary", MD->Boundary+2, MimeValLen-2);
         }

         ParseMimeHdr(Lines[x], NULL, "name", MD->FileName, MimeValLen);
         if (SW_STRISNULL(MD->FileName))
            strcpy(MD->FileName, "unnamed");
      }

      ParseMimeHdr(Lines[x], SWF_NAHeader_MimeCTE, NULL, MD->Encoding, MimeEncLen);
   }

   return Lines[x][0] == '\0' ? x+1 : x;
}


/*****************************************************************************
 * Function: Save Data                                                       *
 *****************************************************************************/

static void SaveData(Object *o, SW_IDATA *idata, Object *Grp,
                     UBYTE *Data, ULONG Size)
{
   ULONG Exists;
   STRPTR FSpec, SaveFile;

   if (!Data) return;
   
   SaveFile = FSpec = VL_AllocGrp(Grp, o, idata->SWA_NAF_Decode_FSpec);

   if (SW_STRISNULL(FSpec)) return;
   
   Exists = SWFileExists(FSpec);

   // -- create directory if asked to ----------------------------------------

   if (idata->SWA_NAF_Decode_MkDir) {
      UBYTE *pp = PathPart(SaveFile), c = *pp;
      *pp = '\0';

      if (!SWDirExists(SaveFile))
         SWMakePath(SaveFile);
      *pp = c;
   }

   // -- make suffix if file exists ------------------------------------------
            
   if (Exists && idata->SWA_NAF_Decode_Mode == SWV_NAF_Decode_Mode_Suffix) {
      ULONG num;

      for (num=1; num<(1<<16); num++) {
         sprintf(SaveFile = idata->NumFile, "%s.%04ld", FSpec, num);
         if (!SWFileExists(SaveFile)) break;
      }
      
      Exists = FALSE;
   }

   // -- save the actual text ------------------------------------------------

   if (!Exists || idata->SWA_NAF_Decode_Mode == SWV_NAF_Decode_Mode_Overwrite)
      SWWriteFileContents(SaveFile, Data, Size);

   VL_FreeGrp(Grp, o, FSpec);
}


/*****************************************************************************
 * Function: Save MIME binary data                                           *
 *****************************************************************************/

static LONG MT_Binary(Object *o, SW_IDATA *idata, Object *Grp,
                      STRPTR *Lines, ULONG LineMax, MData *MD)
{
   STRPTR Buff;
   UBYTE *Data;
   ULONG BuffLen, Size, Err;

   strcpy(idata->FileName, MSBaseName(SW_STRDEF(MD->FileName, "unknown")));
   
   Buff = GetRegion(Lines, LineMax, &BuffLen);
   Data = SWMimeDecode(Buff, BuffLen, &Size, NULL, &Err, MD->Encoding);
   FreeVec(Buff);
   SaveData(o, idata, Grp, Data, Size);
   FreeVec(Data);

   return Data ? SUCCESS : FAILURE;
}


/*****************************************************************************
 * Function: See if line matches boundary marker                             *
 *****************************************************************************/

static LONG IsBoundary(STRPTR Line, MData *MD)
{
   return !strncmp(Line, MD->Boundary, strlen(MD->Boundary));
}


/*****************************************************************************
 * Function: Find next part boundary                                         *
 *****************************************************************************/

static LONG NextBoundary(STRPTR *Lines, ULONG LineMax, MData *MD)
{
   LONG x, Found;
   MData *MDEnclose;

   // -- skip everything up to first boundary marker -------------------------

   for (x=0,Found=FALSE; x<LineMax && !Found; x++)
      if (Lines[x][0] == '-' && Lines[x][1] == '-') {
         if (IsBoundary(Lines[x], MD))
            return x;

         // -- test line against all enclosing multiparts to see if this -----
         // -- multipart ends prematurely ------------------------------------

         for (MDEnclose=MD->MParent; MDEnclose; MDEnclose=MDEnclose->MParent)
            if (IsMimeType(MDEnclose, "multipart/"))
               if (IsBoundary(Lines[x], MDEnclose))
                  return -1;
      }

   return -1;
}


/*****************************************************************************
 * Function: Save MIME binary data                                           *
 *****************************************************************************/

static LONG MT_MultiPart(Object *o, SW_IDATA *idata, Object *Grp,
                         STRPTR *Lines, ULONG LineMax, MData *MD)
{
   LONG  BoundaryPos, NextPos;
   MData *MDataPart;

   // -- first boundary line -------------------------------------------------

   if ((BoundaryPos = NextBoundary(Lines, LineMax, MD)) < 0)
      return FALSE;

   BoundaryPos++;

   // -- loop through all multipart segments ---------------------------------
   
   while (TRUE) {
      if ((NextPos = NextBoundary(Lines+BoundaryPos, LineMax-BoundaryPos, MD)) < 0)
         break;
      NextPos += BoundaryPos;

      if (MDataPart = PushMData(MD)) {
         DecodeMime(o, idata, Grp, Lines + BoundaryPos, NextPos-BoundaryPos, MD);
         PopMData(MDataPart);
      }
      
      BoundaryPos = NextPos + 1;
   }

   return SUCCESS;
}


/*****************************************************************************
 * Funciton: Decode MIME document                                            *
 *****************************************************************************/

static LONG DecodeMime(Object *o, SW_IDATA *idata, Object *Grp,
                       STRPTR *Lines, ULONG LineMax, MData *MD)
{
   ULONG HdrEnd;
   
   HdrEnd = ParseMimeHdrs(Lines, LineMax, MD);

   // -- return if this wasn`t a MIME document -------------------------------

   if (!MD->Encoding[0] && !MD->ContentType[0])
      return FAILURE;

   if (IsMimeType(MD, "multipart/"))
      return MT_MultiPart(o, idata, Grp, Lines+HdrEnd, LineMax-HdrEnd, MD);

   if (IsMimeType(MD, "image/")        ||
       IsMimeType(MD, "audio/")        ||
       IsMimeType(MD, "video/")        ||
       IsMimeType(MD, "message/")      ||
       IsMimeType(MD, "application/"))
      return MT_Binary(o, idata, Grp, Lines+HdrEnd, LineMax-HdrEnd, MD);

   return FAILURE;
}


/*****************************************************************************
 * Function: UUDecode                                                        *
 *****************************************************************************/

static void UUDecode(Object *o, SW_IDATA *idata, Object *Grp,
                     STRPTR *Lines, ULONG LineMax)
{
   STRPTR Buff;
   UBYTE *Data;
   LONG x, EndLine;
   ULONG Err, Size, BuffLen;
   
   for (x=0; x<LineMax; x++) {
      if (SWUUIsBegin(Lines[x])) {
         strcpy(idata->FileName, MSBaseName(SWUUFileName(Lines[x])));

         for (EndLine=x+1; EndLine < LineMax; EndLine++)
            if (SWUUIsEnd(Lines[EndLine]))
               break;

         Buff = GetRegion(Lines+x, EndLine-x+1, &BuffLen);
         Data = SWUUDecode(Buff, BuffLen, &Size, NULL, &Err, NULL);
         FreeVec(Buff);
         SaveData(o, idata, Grp, Data, Size);
         FreeVec(Data);
         x = EndLine + 1;
      }
   }
}


/*****************************************************************************
 * Method: perform action                                                    *
 *****************************************************************************/

METHOD(SWM_NAFilter_Action, SWP_NAFilter_Action *)
{
   ULONG  ArtLines;
   STRPTR *ArtTxt;
   MData *MD;
   
   if (!msg->Grp || !msg->Art) SWDB_RET(SWV_NAFilter_Error);

   ArtTxt   = SWGetType(msg->Art, SWA_NArticle_Txt, STRPTR *);
   ArtLines = SWGetUL(msg->Art, SWA_NArticle_Lines);

   if (!ArtTxt || !ArtLines) SWDB_RET(SWV_NAFilter_Error);
   if (!(MD = NewMData()))   SWDB_RET(SWV_NAFilter_Error);

   if (!ArtTxt || !ArtLines) SWDB_RET(SWV_NAFilter_Error);

   idata->Count++;
   idata->Grp = msg->Grp;
   idata->Art = msg->Art;

   if (DecodeMime(o, idata, msg->Grp, ArtTxt, ArtLines, MD) != SUCCESS)
      UUDecode(o, idata, msg->Grp, ArtTxt, ArtLines);

   DelMData(MD);
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: End action                                                        *
 *****************************************************************************/

METHOD(SWM_NAFilter_End, SWP_NAFilter_End *)
{
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   ULONG rc;
   
   sprintf(idata->NumBuff, "%04ld", idata->Count);

   VL_FindRet("Num",  idata->NumBuff);
   VL_FindRet("File", idata->FileName);
   
   // -- search article object for variables ---------------------------------

   if (rc = DoMethodA(idata->Art, (Msg)msg))
      SWDB_RET(rc);

   // -- search group object for variables -----------------------------------

   SWDB_RET(DoMethodA(idata->Grp, (Msg)msg));
} SDFT

