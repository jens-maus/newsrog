# include "S-Hash.h"
# include "S-FastStrStr.h"
# include "S-LinePrefix.h"

/*****************************************************************************
 * Function: reset cached values                                             *
 *****************************************************************************/

static void ResetCacheValues(SW_IDATA *idata)
{
   if (idata->CachedRefs != SWV_NArticle_NoRefs)
      FV(idata->CachedRefs);

   idata->CachedGroupCount = -1;
   idata->CachedBytes      = -1;
   idata->CachedAge        = 0;
   idata->CachedFlags      = SWF_NAF_NoFlagCache;
   idata->Cached_ds_Days   = 0;
   idata->Cached_ds_Minute = 0;
   idata->CachedSubj       = NULL;
   idata->CachedMsgID      = NULL;
   idata->CachedIDHash     = 0;
   idata->CachedRefs       = NULL;
}


/*****************************************************************************
 * Function: clear all lines                                                 *
 *****************************************************************************/

static void ClearBuffLines(Object *o, SW_IDATA *idata)
{
   ULONG x;

   for (x=0; x<ALines(); x++)
      FV(ATxtLn(x));

   FV(ATxt());
   ATxt()             = NULL;
   ALines()           = 0;
   ResetCacheValues(idata);
}


/*****************************************************************************
 * Function: Find cache filename                                             *
 *****************************************************************************/

static STRPTR HashFileName(STRPTR buff, STRPTR ID)
{
   ULONG x, h1 = 47,      h2 = 921831;
   UBYTE b1[10], b2[10];

   if (!ID) return NULL;

   for (x=0; ID[x]; x++)
      h1 ^= (h1<<3)+((ULONG)ID[x]*(x+31)*791911),
      h2 ^= (ULONG)ID[x]*(x+771)*92137;

   sprintf(b1, "%08lx", h1);
   sprintf(b2, "%08lx", h2);

   for (x=0; b1[x]; x++) {
      LONG v = (avtod(b1[x])<<1) ^ avtod(b2[x]);
      buff[x] = (v <= 9 ? v+'0' : v+'a'-10);
   }

   buff[x] = '\0';

   return buff;
}


/*****************************************************************************
 * Function: generate cached pathname                                        *
 *****************************************************************************/

static STRPTR CachedPathName(Object *o, SW_IDATA *idata, STRPTR Buff,
                             ULONG bsize, STRPTR MsgID, Msg msg, ULONG MakeDirs)
{
   UBYTE CacheFN[36];
   STRPTR cdir=GetTagUB(SWT_NArticle_CacheDir, NULL, msg+1);

   if (SW_STRISNULL(cdir)) return NULL;

   Buff[0] = '\n';

   if (!(MsgID = SW_STRDEF(MsgID, (STRPTR )DoMethod(o,SWM_NArticle_GetMsgID))))
      return NULL;

   AddPart(strcpy(Buff, cdir), SWV_NArticle_CacheDir, bsize);

   if (MakeDirs && SWMakePath(Buff) == FAILURE)
      return NULL;

   AddPart(Buff, HashFileName(CacheFN, MsgID), bsize);

   return Buff;
}


/*****************************************************************************
 * Function: return line # of 1st header/body line, or -1 if none            *
 *****************************************************************************/

static __inline LONG FirstHeaderLine(SW_IDATA *idata) { return 0; }

static LONG FirstBodyLine(SW_IDATA *idata)
{
   LONG x;

   if (ATxt())
      for (x=0; x<ALines()-1 && ATxtLn(x); x++)
         if (ATxtLn(x)[0] == '\0')
            return x+1;

   return -1;
}


/*****************************************************************************
 * Function: return last line # of header                                    *
 *****************************************************************************/

static LONG LastHeaderLine(SW_IDATA *idata)
{
   LONG fbl = FirstBodyLine(idata);
   
   return ATxt() ? (fbl < 2 ? ALines()-1 : fbl-2) : -1;
}


/*****************************************************************************
 * Function: iterate through article head, body, or both                     *
 *****************************************************************************/

static LONG FirstLine(SW_IDATA *idata, LONG Start, ULONG Parts)
{
   LONG fbl = FirstBodyLine(idata),
        lhl = LastHeaderLine(idata);
   
   if (!ATxt()                                           ||
       !Parts                                            ||
       Start >= ALines()                                 ||
       !FIsSet(Parts, SWF_NArticle_Body) && Start > lhl  ||
       !FIsSet(Parts, SWF_NArticle_Head) && fbl<0)
      return SWV_NAF_EndLine;

   return FIsSet(Parts, SWF_NArticle_Head) ? Start : max(Start, fbl);
}


static LONG NextLine(SW_IDATA *idata, LONG x, ULONG Parts)
{
   return ((++x) >= ALines() ||
           !FIsSet(Parts, SWF_NArticle_Body) && SW_STRISNULL(ATxtLn(x))) ?
        SWV_NAF_EndLine : x;
}


/*****************************************************************************
 * Function: search for and return header line, or -1 if not found           *
 *****************************************************************************/

static LONG FindHeaderLine(SW_IDATA *idata, STRPTR Header, LONG HLen)
{
   LONG  x;
   ULONG UH;

   if (!AHasHead(idata) || !ATxt() || SW_STRISNULL(Header))
      return -1;

   UH = *((ULONG *)Header);
   
   if (!HLen) HLen = strlen(Header);

   ALineLoop(x, idata, SWF_NArticle_Head, 0)
      if (*((ULONG *)ATxtLn(x)) == UH &&
          strncmp(ATxtLn(x)+4, Header+4, HLen-4) == 0)
         return x;

   return -1;
}

# define FindHeaderLineC(i,h)   (FindHeaderLine(i, h, sizeof(h)-1))
# define FindHeaderTxtC(i,h,t)  \
         ((t=FindHeaderLineC(i,h)),((t)>=0)?ATxtLn(t):NULL)
# define FindHeaderDataC(i,h,t) \
         ((t=FindHeaderLineC(i,h)),((t)>=0)?(i)->SWA_NArticle_Txt[t]+sizeof(h):NULL)


/*****************************************************************************
 * Function: get state value in header line                                  *
 *****************************************************************************/

static ULONG GetState(SW_IDATA *idata)
{
   LONG t;
   ULONG f = 0;
   STRPTR ov;

   if (!FIsSet(idata->CachedFlags, SWF_NAF_NoFlagCache))
      return (ULONG)idata->CachedFlags;

   if (!(ov = FindHeaderDataC(idata, SWF_NAHeader_State, t)))
      return 0;

   while (*ov)
      f = (f*16) + avtod(*ov), ov++;

   return idata->CachedFlags = f;
}


/*****************************************************************************
 * Function: quickly to integer->8 digit hex converstion                     *
 *****************************************************************************/

static __inline STRPTR FastItoH(STRPTR b, ULONG v)
{
   static UBYTE vals[] = "0123456789abdef";

   b[8] = '\0';
   b[7] = vals[v&0xf];  v >>= 4;
   b[6] = vals[v&0xf];  v >>= 4;
   b[5] = vals[v&0xf];  v >>= 4;
   b[4] = vals[v&0xf];  v >>= 4;
   b[3] = vals[v&0xf];  v >>= 4;
   b[2] = vals[v&0xf];  v >>= 4;
   b[1] = vals[v&0xf];  v >>= 4;
   b[0] = vals[v&0xf];

   return b;   
}


/*****************************************************************************
 * Function: set bytes value in header line                                  *
 *****************************************************************************/

static ULONG SetBytes(Object *o, SW_IDATA *idata, ULONG b)
{
   UBYTE nf[sizeof(SWF_NAHeader_Bytes) + 16], *a[1];
   LONG  l;

   if (b == ABytes(o, idata)) return b;

   l = FindHeaderLineC(idata, SWF_NAHeader_Bytes);

   idata->CachedBytes = b;

   a[0] = nf;
   
   strcpy(nf, SWF_NAHeader_Bytes);
   nf[sizeof(SWF_NAHeader_Bytes)-1] = ' ';
      
   sprintf(nf+sizeof(SWF_NAHeader_Bytes), "%ld", b);
      
   if (l < 0) {
      DoMethod(o, SWM_NArticle_AddLines, SWT_NArticle_AddLines, a, TAG_DONE);
   } else {
      FV(ATxtLn(l));
      ATxtLn(l) = SWPoolStrDup(idata->SWA_NArticle_MemPool, nf);
   }

  Done:
   return b;
}


/*****************************************************************************
 * Function: set state value in header line                                  *
 *****************************************************************************/

static ULONG SetState(Object *o, SW_IDATA *idata, ULONG s)
{
   LONG   t;
   STRPTR d;

   if (s == GetState(idata)) return s;

   d = FindHeaderDataC(idata, SWF_NAHeader_State, t);

   idata->CachedFlags = s;
   
   if (!d) {
      UBYTE nf[sizeof(SWF_NAHeader_State) + 16], *a[1];
      a[0] = nf;

      strcpy(nf, SWF_NAHeader_State);
      nf[sizeof(SWF_NAHeader_State)-1] = ' ';
      FastItoH(nf+sizeof(SWF_NAHeader_State), s);
      
      DoMethod(o, SWM_NArticle_AddLines, SWT_NArticle_AddLines, a, TAG_DONE);
   } else {
      FastItoH(d, s);
   }

  Done:
   set(o, SWA_NArticle_NewState, SWV_NArticle_NewFlags);
   return s;
}


/*****************************************************************************
 * Method: Abort receive if pending                                          *
 *****************************************************************************/

METHOD(SWM_NArticle_Abort, Msg)
{
   if (!idata->Srv) SWDB_RET(0);
   SWDB_RET(DoMethod(idata->Srv, SWM_NServer_Abort, idata->ID));
} SDFT


/*****************************************************************************
 * Function: send status msg                                                 *
 *****************************************************************************/

static ULONG SendStatMsg(Object *o, SW_IDATA *idata, Object *tgt,
                         ULONG State, ULONG rc, LONG First, LONG Last,
                         ULONG Loc)
{
   if (tgt)
      DoMethod(tgt, SWM_NArticle_StatMsg, o, State,
               ATxt(), First, Last, LastHeaderLine(idata), Loc);

   return rc;
}


/*****************************************************************************
 * Method: Fetch article from either cache or network                        *
 *****************************************************************************/

METHOD(SWM_NArticle_Fetch, ULONG *)
{
   UBYTE CmdBuff[FMSIZE];
   ULONG    Parts = GetTagUL(SWT_NArticle_Parts, SWF_NArticle_HdBd, msg+1);
   LONG    ArtNum = GetTagL (SWT_NArticle_ArtNum, -1,               msg+1);
   LONG    IncCnt = GetTagL (SWT_NArticle_IncLines, 100,            msg+1);
   STRPTR   MsgID = GetTagUB(SWT_NArticle_MsgID,  NULL,             msg+1);
   Object    *Srv = GetTagOB(SWT_NArticle_Server, NULL,             msg+1);
   STRPTR   Title = GetTagUB(SWT_NArticle_FetchTitle,    "",        msg+1);
   Object   *FObj = GetTagOB(SWT_NArticle_FetchObj, NULL,           msg+1);
   STRPTR    cdir = GetTagUB(SWT_NArticle_CacheDir, NULL,           msg+1);
   LONG      NoNw = GetTagL (SWT_NArticle_NoNetwork, FALSE,         msg+1);

   if (SW_STRISNULL(MsgID) && ArtNum<0 && !AHasHead(idata))
      SWDB_RET(FAILURE);

   // -- read message ID from header, or use parameter -----------------------

   if (!MsgID && ArtNum < 0)
      MsgID = (STRPTR)DoMethod(o, SWM_NArticle_GetMsgID);

   if (!MsgID && ArtNum < 0)
      SWDB_RET(FAILURE);

   // -- if possible, retrieve article from the disk or memory cache ---------

   if (!idata->Srv &&
       DoMethod(o, SWM_NArticle_GetCache,
                SWT_NArticle_MsgID,    MsgID,
                SWT_NArticle_Parts,    Parts,
                SWT_NArticle_CacheDir, cdir,
                TAG_DONE) == SUCCESS) {

      if (!FIsSet(GetState(idata), SWF_NArticle_NotFound)) {
         DoMethod(o, SWM_NArticle_FlagReset, SWF_NArticle_QueuedDL);
         SendStatMsg(o, idata, FObj, SWV_NArticle_Stat_Begin, 0, 0, 0, 0);
         SendStatMsg(o, idata, FObj, SWV_NArticle_Stat_NewLinesLast, 0,
                     0, ALines()-1, SWV_NArticle_Stat_LocDiskCache);
         SWDB_RET(SendStatMsg(o, idata, FObj, SWV_NArticle_Stat_End,
                              SUCCESS, 0, 0, SWV_NArticle_Stat_LocDiskCache));
      }
   }

   AResetFlg(o, SWF_NArticle_CachedHdBd);

   if (NoNw)
      SWDB_RET(FAILURE);

   // -- otherwise, get it from the NNTP server ------------------------------

   if (Srv) {
      ULONG rc;
      sprintf(CmdBuff, MsgID ? "%s %s\r\n" : "%s %ld\r\n",
              FHaveAll(Parts, SWF_NArticle_HdBd) ? "ARTICLE" :
              (FIsSet(Parts, SWF_NArticle_Head) ? "HEAD" : "BODY"),
              MsgID ? (LONG)MsgID : ArtNum);

      idata->IncLines = IncCnt;

      DoMethod(o, SWM_NArticle_Abort);  // stop any pending command

      SendStatMsg(o, idata, FObj, SWV_NArticle_Stat_Begin, 0, 0, 0, 0);
      
      if ((rc = DoMethod(Srv,
                         SWM_NServer_SendCmd,
                         SWT_NServer_Cmd,            CmdBuff,
                         SWT_NServer_CmdObj,         o,
                         SWT_NServer_CmdMethod,      SWM_NArticle_Fetch_P,
                         SWT_NServer_Interval,       100,
                         SWT_NServer_Title,          Title,
                         SWT_NServer_Data,           FObj,
                         TAG_DONE)) != SUCCESS)
         SWDB_RET(SendStatMsg(o, idata, FObj, SWV_NArticle_Stat_Abort,
                              rc, 0, 0, 0));
      SWDB_RET(SUCCESS);
   }

   DoMethod(o, SWM_NArticle_FlagReset, SWF_NArticle_QueuedDL);

   SWDB_RET(SendStatMsg(o, idata, FObj, SWV_NArticle_Stat_Abort,
                        FAILURE, 0, 0, 0));
} SDFT


/*****************************************************************************
 * Method: Parse article output                                              *
 *****************************************************************************/

METHOD(SWM_NArticle_Fetch_P, SWP_NServer_StatusMsg *)
{
   if (NSR_SBegin(msg)) {
      idata->Srv         = NSR_Srv(msg);
      idata->ID          = NSR_ID(msg);
      ResetCacheValues(idata);

      SWDB_RET(0);
   }
   
   // -- look at result code ----------------------------------------

   if (NSR_SRescode(msg)) {
      switch (idata->XFerState = NSR_ResCode(msg)) {
        case SWV_NSR_Article:
        case SWV_NSR_Head:
        case SWV_NSR_Body:
         SWDB_RET(SWV_NServer_RescodeOK);

        default:
         ASetFlg(o, SWF_NArticle_NotFound);
         SWDB_RET(SendStatMsg(o, idata, (Object *)msg->Data,
                              SWV_NArticle_Stat_NotFound,
                              SWV_NServer_RescodeBad, 0, 0, 0));
      }
   }

   // -- here come some lines ------------------------------------------------
   
   if (NSR_SRun(msg) || NSR_SDone(msg)) {
      LONG   t, LineCnt, OldBytes, CurrLines;
      ULONG newsize;
      STRPTR Lines;
      UWORD OldFlags;

      // -- we didn`t get any lines?  bad! -----------------------------------
      
      if (idata->XFerState && NSR_Last(msg) < 0) {
         ASetFlg(o, SWF_NArticle_NotFound);
         
         SWDB_RET(SendStatMsg(o, idata, (Object *)msg->Data,
                              SWV_NArticle_Stat_NotFound,
                              SWV_NServer_RescodeBad, 0, 0, 0));
      }

      // -- clear out the appropriate section of the article -----------------
      
      switch (idata->XFerState) {
        case SWV_NSR_Article:
        case SWV_NSR_Head:
         OldFlags = GetState(idata);
         OldBytes = (LONG)DoMethod(o, SWM_NArticle_GetBytes);
         
         DoMethod(o, SWM_NArticle_FreeText, SWF_NArticle_HdBd);
         break;

        case SWV_NSR_Body:
         DoMethod(o, SWM_NArticle_FreeText, SWF_NArticle_Body);
         DoMethod(o, SWM_NArticle_AddSeparator);
         break;
      }

      // -- add the new lines ------------------------------------------------
      
      if (RV(ASize(ALines() + NSR_Count(msg)), &ATxt()) < 0); // TBD: ERROR

      newsize = ALines() + 
         NSR_Adopt(msg, NSR_First(msg), NSR_Last(msg), ATxt() + ALines(),
                   idata->XFerState != SWV_NSR_Body);

      Lines = FindHeaderDataC(idata, SWF_NAHeader_Lines, t);

      if (!NSR_SRun(msg) || idata->IncLines)
         SendStatMsg(o, idata, (Object *)msg->Data,
                     NSR_SRun(msg) ? SWV_NArticle_Stat_NewLines
                                   : SWV_NArticle_Stat_NewLinesLast,
                     0, ALines(), newsize-1, SWV_NArticle_Stat_LocNet);

      ALines()   = newsize;
      LineCnt    = Lines ? atol(Lines) : 15;
      LineCnt    = max(LineCnt, 15);
      CurrLines  = ALines() - FirstBodyLine(idata) + 1;

      set(o, SWA_NArticle_PctDL, CurrLines * 100 / LineCnt);

      // -- reset Size & Flags headers that we might have stomped on ---------
      
      if (idata->XFerState == SWV_NSR_Article ||
          idata->XFerState == SWV_NSR_Head) {
         SetState(o, idata, OldFlags);
         SetBytes(o, idata, OldBytes);
         DoMethod(o, SWM_NArticle_FixISOHdrs, TAG_DONE);
         DoMethod(o, SWM_NArticle_FixHdrCase, TAG_DONE);
      }

      idata->XFerState = 0;  // don`t re-init article next time
   }

   // -- oops - we aborted ---------------------------------------------------
   
   if (NSR_SAbort(msg)) {
      ASetFlg(o, SWF_NArticle_NotFound);

      SendStatMsg(o, idata, (Object *)msg->Data, SWV_NArticle_Stat_Abort,
                  0, 0, 0, 0);
   }

   // -- done ----------------------------------------------------------------
   
   if (NSR_SAnyEnd(msg)) {
      AResetFlg(o, SWF_NArticle_QueuedDL);

      if (NSR_SBad(msg)) {
         ASetFlg(o, SWF_NArticle_NotFound);

         SendStatMsg(o, idata, (Object *)msg->Data, SWV_NArticle_Stat_NotFound,
                     0, 0, 0, SWV_NArticle_Stat_LocNet);
      }

      if (NSR_SDone(msg)) {
         AResetFlg(o, SWF_NArticle_NotFound);

         SendStatMsg(o, idata, (Object *)msg->Data, SWV_NArticle_Stat_End,
                     0, 0, 0, SWV_NArticle_Stat_LocNet);
      }

      ResetCacheValues(idata);
      idata->Srv = NULL, idata->ID = 0;
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Free message text                                                 *
 *****************************************************************************/

METHOD(SWM_NArticle_AddSeparator, Msg)
{
   if (RV(ASize(ALines()+1), &ATxt()) < 0)
      SWDB_RET(FAILURE);

   ATxtLn(ALines()++) = SWPoolNullStr();

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Free message text                                                 *
 *****************************************************************************/

METHOD(SWM_NArticle_FreeText, SWP_NArticle_FreeText *)
{
   LONG x, line;

   // -- return if the text is gone already ----------------------------------

   if (!ATxt())
      SWDB_RET(0);

   ResetCacheValues(idata);
   
   // -- delete all of buffer ------------------------------------------------

   if (FHaveAll(msg->Parts, SWF_NArticle_HdBd)) {
      ClearBuffLines(o, idata);
      SWDB_RET(0);
   }

   // -- delete parts of buffer ----------------------------------------------
   
   ALineLoop(x, idata, msg->Parts, 0) {
      FV(ATxtLn(x));
      ATxtLn(x) = NULL;
   }
   
   // -- pack and resize buffer ----------------------------------------------

   for (line=0; line<ALines(); line++)   // find 1st non-NULL
      if (ATxtLn(line))
          break;

   if (line == ALines()) {               // if none, del buffer
      ClearBuffLines(o, idata);
      SWDB_RET(0);
   }

   if (line != 0)                        // if >0, slide rest up
      for (x=line; x < ALines(); x++)
         ATxtLn(x-line) = ATxtLn(x), ATxtLn(x) = NULL;

   for (line=0; line < ALines(); line++) // count lines
      if (!ATxtLn(line))
         break;

   ALines() = (RV(ASize(line), &ATxt()) < 0) ? 0:line;

   SWDB_RET(0);
}


/*****************************************************************************
 * Method: Find header data                                                  *
 *****************************************************************************/

METHOD(SWM_NArticle_FindHeader, SWP_NArticle_FindHeader *)
{
   LONG HLen  = strlen(msg->Header ? msg->Header : ""),
        HLine = FindHeaderLine(idata, msg->Header, HLen);

   SWDB_RET(HLine < 0 ? NULL : (ULONG)(ATxtLn(HLine)+HLen+1));
} SDFT


/*****************************************************************************
 * Function: construct comment for cache file                                *
 *****************************************************************************/

static STRPTR GenCacheComment(Object *o, SW_IDATA *idata,
                              STRPTR Buff, ULONG BuffLen)
{
   sprintf(Buff, "ID:%08lx", DoMethod(o, SWM_NArticle_GetIDHash));

   return Buff;
}


/*****************************************************************************
 * Method: Write article to disk cache                                       *
 *****************************************************************************/


METHOD(SWM_NArticle_Cache, Msg)
{
#  define MaxCL 32

   UBYTE FName[FMSIZE];
   LONG  hd = AHasHead(idata), bd = AHasBody(idata);
   ULONG rc = FAILURE;

   if (!ATxt()) goto Done;

   if (AChkFlgAll(o, SWF_NArticle_CachedHdBd) ||
       hd && AChkFlg(o,SWF_NArticle_CachedHead))
      SWDB_RET(SUCCESS);

   ASetFlg(o, (hd ? SWF_NArticle_CachedHead : 0) |
              (bd ? SWF_NArticle_CachedBody : 0));

   rc = DoMethod(o, SWM_NArticle_SaveText,
                 SWT_NArticle_FSpec, CachedPathName(o,idata, FName, FMSIZE,
                                                    NULL, msg, TRUE),
                 TAG_DONE);

   if (rc == SUCCESS) {
      UBYTE  Comment[MaxCL+1];
      STRPTR c = GenCacheComment(o, idata, Comment, MaxCL);

      if (!c || !SetComment(FName, c)) {
         DeleteFile(FName);
         rc = FAILURE;
      }
   }

  Done:
   if (rc != SUCCESS)
      AResetFlg(o, SWF_NArticle_CachedHdBd|SWF_NArticle_Permanent);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Remove article from disk cache                                    *
 *****************************************************************************/

METHOD(SWM_NArticle_UnCache, Msg)
{
   ULONG rc = SUCCESS;
   UBYTE FName[FMSIZE];

   if (!ATxt()) SWDB_RET(0);

   rc = SWDeleteFile(CachedPathName(o,idata,FName,FMSIZE,NULL,msg,FALSE));

   if (rc == SUCCESS)
      AResetFlg(o, SWF_NArticle_CachedHdBd|SWF_NArticle_Permanent);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Read article from cache: 0 if read OK                             *
 *****************************************************************************/

METHOD(SWM_NArticle_HasParts, ULONG *)
{
   ULONG  Parts = GetTagUL(SWT_NArticle_Parts, SWF_NArticle_HdBd, msg+1);

   switch (Parts) {
      case SWF_NArticle_Head: SWDB_RET(AHasHead(idata));
      case SWF_NArticle_Body: SWDB_RET(AHasBody(idata));
      case SWF_NArticle_HdBd: SWDB_RET(AHasHdBd(idata));
    }

   SWDB_RET(FALSE);
} SDFT


/*****************************************************************************
 * Method: Read article from cache: 0 if read OK                             *
 *****************************************************************************/

METHOD(SWM_NArticle_GetCache, ULONG *)
{
   UBYTE FName[FMSIZE];
   STRPTR FSpec = GetTagUB(SWT_NArticle_FSpec, NULL,              msg+1),
          MsgID = GetTagUB(SWT_NArticle_MsgID, NULL,              msg+1);
   ULONG  Parts = GetTagUL(SWT_NArticle_Parts, SWF_NArticle_HdBd, msg+1);
   ULONG rc;
   ULONG OldState = GetState(idata);

   // -- if requested parts are already loaded, and we`re not in force mode, -
   // -- and it`s not an overview, don`t bother to fetch it from disk. -------

   if (!GetTagUL(SWT_NArticle_CacheForce, FALSE, msg+1) &&
       (FIsSet(Parts, SWF_NArticle_Body)  && AHasBody(idata) ||
        !FIsSet(Parts, SWF_NArticle_Body) && AHasHead(idata)))
      SWDB_RET(SUCCESS);

   if (!FSpec)
      FSpec = CachedPathName(o, idata, FName, FMSIZE, MsgID,(Msg)msg, FALSE);

   // -- load the requested parts from the file ------------------------------
   
   if ((rc = DoMethod(o, SWM_NArticle_LoadText,
                      SWT_NArticle_FSpec, FSpec,
                      SWT_NArticle_Parts, Parts,
                      TAG_DONE)) == SUCCESS) {
      SetState(o, idata, OldState |
               (GetState(idata) & (SWF_NArticle_CachedHdBd|
                                   SWF_NArticle_Permanent)) |
               (AHasHead(idata)?(SWF_NArticle_CachedHead):0) |
               (AHasBody(idata)?(SWF_NArticle_CachedBody):0));
   }
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Search article for pattern                                        *
 *****************************************************************************/

METHOD(SWM_NArticle_Search, ULONG *)
{
   STRPTR Pat    = GetTagUB (SWT_NArticle_Pattern,        NULL,     msg+1),
          PPat   = GetTagUB (SWT_NArticle_ParsedPattern,  NULL,     msg+1),
          Hdr    = GetTagUB (SWT_NArticle_Header,         NULL,     msg+1),
          PPBuff = NULL, 
          Data   = NULL; 
   ULONG  CaseS  = GetTagUL (SWT_NArticle_CaseSens, NULL,           msg+1),
          Parts  = GetTagUL (SWT_NArticle_Parts, SWF_NArticle_HdBd, msg+1),
          StartL = GetTagUL (SWT_NArticle_StartLine, 0,             msg+1),
          Wild   = GetTagUL (SWT_NArticle_Wild,           TRUE,     msg+1),
          SubStr = GetTagUL (SWT_NArticle_SubLine,        TRUE,     msg+1),
         *LinePtr= GetTagTyp(SWT_NArticle_LinePtr, NULL, msg+1,   ULONG *);
   LONG   loc, x, foundline = -1;
   LONG   HLen   = Hdr ? strlen(Hdr) : 0;

   if (!ATxt() || (!Pat && !PPat)) SWDB_RET(NULL);

   // -- parse the pattern ---------------------------------------------------

   if (!PPat && Wild) {
      LONG len = strlen(Pat)*2+2;
      if (!(PPat = PPBuff = AV(len))) goto Done;
      if ((CaseS ? ParsePattern : ParsePatternNoCase)(Pat, PPat, len) < 0)
         goto Done;
   }

   // - find header line, if asked for a particular one ----------------------

   if (Hdr) {
      STRPTR HStr;
      ULONG  Found = FALSE;

      if ((loc = FindHeaderLine(idata, Hdr, HLen)) < 0)
         goto Done;

      HStr = ATxtLn(loc)+ ++HLen;

      if (*((ULONG *)Hdr) == MAKE_ID('S','u','b','j'))
         HStr = SWV_NArticle_SubjNoRe(HStr);

      if (Wild) {
         if ((CaseS ? MatchPattern : MatchPatternNoCase)(PPat, HStr))
            Found = TRUE;
      } else
         if (findstr(HStr, Pat, LONG_MAX, strlen(Pat), CaseS, SubStr))
            Found = TRUE;

      if (Found)
         Data = ATxtLn(loc)+HLen, foundline = loc;
   } else {
      ULONG PatLen;

      if (!Wild) PatLen = strlen(Pat);
      
      ALineLoop(x, idata, Parts, StartL) {
         if (Wild) {
            if ((CaseS ? MatchPattern : MatchPatternNoCase)(PPat, ATxtLn(x))) {
               Data = ATxtLn(x), foundline = x;
               break;
            }
         } else {
            if (findstr(ATxtLn(x), Pat, LONG_MAX, PatLen, CaseS, SubStr)) {
               Data = ATxtLn(x), foundline = x;
               break;
            }
         }
      }
   }

  Done:
    FV(PPBuff);

   if (LinePtr) *LinePtr = foundline;

   SWDB_RET(Data);
} SDFT


/*****************************************************************************
 * Method: Save text of article into file                                    *
 *****************************************************************************/

METHOD(SWM_NArticle_SaveText, ULONG *)
{
   LONG    x,
           UseFlt   = FALSE,
           rc       = FAILURE;
   BPTR    fp       = NULL;
   STRPTR  FSpec    = GetTagUB(SWT_NArticle_FSpec,    NULL,              msg+1),
           Prefix   = GetTagUB(SWT_NArticle_Prefix,   NULL,              msg+1);
   ULONG   Append   = GetTagUL(SWT_NArticle_Append,   FALSE,             msg+1),
           QMode    = GetTagUL(SWT_NArticle_QMode,    SWV_NNews_QM_Off,  msg+1),
           SvAttr   = GetTagUL(SWT_NArticle_SvAttr,   SWA_NDFilter_T_RAW,msg+1),
           Parts    = GetTagUL(SWT_NArticle_Parts,    SWF_NArticle_HdBd, msg+1);
   Object  *Grp     = GetTagObj(SWT_NArticle_Grp,     NULL,              msg+1),
           *ArtView = GetTagObj(SWT_NArticle_ArtView, NULL,              msg+1);
   STRPTR  LPrefix;
   TxtBuff Tx; 
   
   if (SW_STRISNULL(FSpec) || !Parts || !ATxt()) SWDB_RET(FAILURE);

   // -- see if we should save a filtered version ----------------------------
   
   if (ArtView && SvAttr != SWA_NDFilter_T_RAW) {
      UseFlt = TRUE;
      DoMethod(ArtView, SWM_NArticleView_DFwd, SWM_NDFilter_TBInit, &Tx);

      // -- ask for header no matter what, coz some dsp filters need it ------
      
      if (!(DoMethod(ArtView, SWM_NArticleView_ModTxt,
                     SWT_NArticle_Parts,         Parts,
                     SWT_NArticle_Prefix,        Prefix,
                     SWT_NArticle_QMode,         QMode,
                     SWT_NArticleView_MT_Attr,   SvAttr,
                     SWT_NArticleView_MT_Grp,    Grp,
                     SWT_NArticleView_MT_Art,    o,
                     SWT_NArticleView_MT_TxBuff, &Tx,
                     TAG_DONE)))
         SWDB_RET(FAILURE);
   }

   // -- open the save file --------------------------------------------------
   
   if (Append)
      if (fp = Open(FSpec, MODE_OLDFILE))
         Seek(fp, 0, OFFSET_END);

   if (!fp)
      if (!(fp = Open(FSpec, MODE_NEWFILE))) {
         // SWERR(o, NULL, SWS_Err_WriteTxt, FSpec, 0);
         SWDB_RET(FAILURE);
      }

   // -- save with, or without, filters --------------------------------------
   
   if (UseFlt) {
      if (Write(fp, TxBuff(&Tx), TxLen(&Tx)) != TxLen(&Tx))
         goto Done;
   } else {
      // -- save with no filters ---------------------------------------------

      ALineLoop(x, idata, Parts, 0) {
         // -- print quote prefix if needed ----------------------------------
   
         if (LPrefix = LinePrefix(ATxtLn(x), Prefix, QMode))
            if (FPuts(fp, Prefix))
               goto Done;
   
         // -- print line ----------------------------------------------------
   
         if (FPuts(fp, ATxtLn(x)) || FPuts(fp, "\n"))
            goto Done;
      }
   }

   rc = SUCCESS;

  Done:
   Close(fp);

   if (UseFlt)
      DoMethod(ArtView, SWM_NArticleView_DFwd, SWM_NDFilter_TBDispose, &Tx);
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Load text of article from file:                                   *
 *****************************************************************************/

METHOD(SWM_NArticle_LoadText, ULONG *)
{
   LONG   size;
   BPTR   fp;
   ULONG  Parts       = GetTagUL(SWT_NArticle_Parts, SWF_NArticle_HdBd, msg+1);
   STRPTR FSpec       = GetTagUB(SWT_NArticle_FSpec, NULL,              msg+1),
          inline;

   if (!FSpec || !Parts) SWDB_RET(FAILURE);

   if (!(fp = Open(FSpec, MODE_OLDFILE))) {
      //SWERR(o, 3, SWS_Err_ReadTxt, FSpec, 0);
      SWDB_RET(FAILURE);
   }

   ClearBuffLines(o, idata);

   if (!(inline =           AV(SWV_NAF_MaxLL)))  goto Done;
   if (!(ATxt() = (STRPTR *)AV(ASize(size=5))))  goto Done;
   
   while (FGets(fp, inline, SWV_NAF_MaxLL)) {
      ULONG sl = strlen(inline);
      if (!sl) break;
      inline[--sl] = '\0';
      
      if (ALines() >= size) { size += 200; RV(ASize(size), &ATxt()); }
      ATxtLn(ALines()) = SWPoolStrDup(idata->SWA_NArticle_MemPool, inline);

      if (SW_STRISNULL(ATxtLn(ALines())) && !FIsSet(Parts,SWF_NArticle_Body)) {
         FV(ATxtLn(ALines()));   // don`t read body if not asked to
         break;
      }

      ALines()++;
   }
   
   RV(ASize(ALines()), &ATxt());

  Done:
   Close(fp);
   FV(inline);
   SWDB_RET(SUCCESS);
} SDFT

   
/*****************************************************************************
 * Method: Parse article output                                              *
 *****************************************************************************/

METHOD(SWM_NArticle_AddLines, ULONG *)
{
   STRPTR *Lines = GetTagTyp(SWT_NArticle_AddLines, NULL, msg+1, STRPTR *);
   LONG LineCount= GetTagL  (SWT_NArticle_AddLineCount,  1,      msg+1);
   ULONG Where   = GetTagUL (SWT_NArticle_AddWhere, SWF_NArticle_Head, msg+1);
   ULONG Alloc   = GetTagUL (SWT_NArticle_AddAlloc,      TRUE,   msg+1);
   ULONG FixISO  = GetTagUL (SWT_NArticle_AddFixISOHdrs, FALSE,  msg+1);
   ULONG FixCase = GetTagUL (SWT_NArticle_AddFixHdrCase, FALSE,  msg+1);
   LONG x, addbefore;

   if (!Lines || !LineCount) SWDB_RET(0);

   ResetCacheValues(idata);
   
   if (FIsSet(Where, SWF_NArticle_Head))
      addbefore = LastHeaderLine(idata) + 1;
   
   if (FIsSet(Where, SWF_NArticle_Body))
      addbefore = ALines()+1;

   if (RV(ASize(ALines()+LineCount), &ATxt()) < 0)
      SWDB_RET(0);

   for (x=ALines()-1; x>=addbefore; x--)
      ATxtLn(x+LineCount) = ATxtLn(x);

   for (x=0; x<LineCount; x++)
      ATxtLn(x+addbefore) = Alloc ?
         SWPoolStrDup(idata->SWA_NArticle_MemPool, Lines[x]) : Lines[x];

   ALines() += LineCount;

#  ifdef DEBUGTCP
      for (x=0; x<LineCount; x++)
         Printf("%s: %s\n", __FUNC__, ATxtLn(x+addbefore));
#  endif

   if (FixISO)
      DoMethod(o, SWM_NArticle_FixISOHdrs, TAG_DONE);

   if (FixCase)
      DoMethod(o, SWM_NArticle_FixHdrCase, TAG_DONE);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: find count of separate items in a string                        *
 *****************************************************************************/

static __inline SHORT CountElem(STRPTR Data, ULONG Sep)
{
   STRPTR p;
   SHORT count;

   if (!Data) return -1;
   
   for (count=0; Data && *Data; count++, Data=(p=strchr(Data, Sep))?p+1:NULL);

   return count;
}


/*****************************************************************************
 * Function: return eithere cached value or header value                     *
 *****************************************************************************/

static __inline LONG CachedValue(Object *o, SW_IDATA *idata, SHORT *Val,
                                 STRPTR Header, ULONG Sep)
{
   if ((LONG)(*Val) >= 0)
      return (LONG)(*Val);
   
   *Val = CountElem((STRPTR)DoMethod(o, SWM_NArticle_FindHeader, Header), Sep);

   return (*Val >= 0) ? (LONG)(*Val) : 0;
}


/*****************************************************************************
 * Method: Count newsgroups this article is posted to, (0 for error)         *
 *****************************************************************************/

METHOD(SWM_NArticle_GroupCount, Msg)
{
   SWDB_RET(CachedValue(o,idata, &idata->CachedGroupCount,
                        SWF_NAHeader_Groups,','));
} SDFT


/*****************************************************************************
 * Function: search string components                                        *
 *****************************************************************************/

static LONG FindComponent(STRPTR Str,STRPTR Component,ULONG Complen,ULONG Sep)
{
   LONG   Pos;
   STRPTR Next;

   if (!Str || !Component) return -1;
   if (!Complen) Complen = strlen(Component);

   for (Pos=0;; Str = Next+1, Pos++)
      if (!(Next = strchr(Str, Sep)))
         return !strcmp(Str, Component) ? Pos : -1;
      else if (Complen == Next - Str && strncmp(Str, Component, Complen) == 0)
         return Pos;
}



/*****************************************************************************
 * Method: Determine if article is posted to a particular group              *
 *****************************************************************************/

METHOD(SWM_NArticle_InGroup, SWP_NArticle_InGroup *)
{
   LONG   t;
   STRPTR groups = FindHeaderDataC(idata, SWF_NAHeader_Groups, t);

   if (!ATxt()) SWDB_RET(FALSE);

   SWDB_RET(FindComponent(groups, msg->Group, 0, ',') >= 0);
} SDFT


/*****************************************************************************
 * Function: Convert date to minute number                                   *
 *****************************************************************************/

STRPTR Months[] = {
   "jan", "feb", "mar", "apr", "may", "jun",
   "jul", "aug", "sep", "oct", "nov", "dec",
   NULL,
};

static LONG FindMonth(STRPTR str)
{
   LONG x;
   for (x=0; Months[x]; x++)
      if (tolower(str[0]) == Months[x][0] &&
          tolower(str[1]) == Months[x][1] &&
          tolower(str[2]) == Months[x][2])
         return x+1;
   return 0;
}

static LONG FindTZ(STRPTR str)
{
   LONG x, tzoff;
   static STRPTR TZ[] = { "GMT", "PST", "PDT", "MST", "MDT",
                          "CST", "CDT", "EST", "EDT", NULL };
                 
   static LONG TZOff[]= { 0, -8, -9, -7, -8, -6, -7, -5, -6 };

   if (str[0] == '-' || str[0] == '+') {
      tzoff = atol(str);
      return (tzoff/100) * 60 + (tzoff%100);
   }

   for (x=0; TZ[x]; x++)
      if (!strcmp(TZ[x], str))
         return TZOff[x] * 60;

   return 0;
}


# define TPD_DIGITS "0123456789"

static ULONG NetDateToDS(STRPTR Date, struct DateStamp *ds, ULONG AddTZOffset)
{
   STRPTR p;
   LONG x, ly, lyrs;
   ULONG D, Mo, Y, H, M, S, TZ;
   static ULONG MDays[]  = { 0, 0, 31, 59, 90, 120, 151,
                             181, 212, 243, 273, 304, 334 };

   if (SW_STRISNULL(Date)) return FAILURE;     // oops - no string.
   
   if (p = strchr(Date, ',')) Date = p+1;      // skip day name, if any;

   while (*Date == ' ') Date++;                // skip whitespace to days;

   D = (*Date++-'0');
   if (*Date != ' ' && *Date != '-')
      D = D*10 + (*Date++-'0');
   if (D > 31)
      return FAILURE;                          // bad days;

   if (*Date=='-'||*Date==' ') Date++;         // skip separator to month
      else return FAILURE;

   if (!(Mo = FindMonth(Date))) return FAILURE;// can`t find month;
   Date += 3;                                  // skip to year;
   if (*Date != ' ' && *Date != '-')           // can`t find year
      return FAILURE;
   Date++;

   x=strspn(Date, TPD_DIGITS);
   if (x != 2 && x != 4) return FAILURE;       // wrong # of digits in year;
   Y = atol(Date);
   if (Y < 78)  Y += 2000;                     // fix Y2000 bug;
   if (Y < 100) Y += 1900;
   Date += x;

   while (*Date == ' ') Date++;                // skip whitespace to hour;

   H = (*Date++-'0');
   if (*Date != ':' && *Date != ' ')
      H = H * 10 + (*Date++-'0');
   if (H > 23) return FAILURE;                 // bad hour;

   if (*Date++ != ':') return FAILURE;         // no minute

   if ((M = (*Date++-'0') * 10 + (*Date++-'0')) > 59)
      return FAILURE;                          // bad minute

   if (*Date != ':' && *Date != ' ') return FAILURE;  // huh?

   if (*Date++ == ':')
      if ((S = (*Date++-'0') * 10 + (*Date++-'0')) > 59)
         return FAILURE;                          // bad second

   while (*Date == ' ') Date++;                // skip whitespace to TZ;

   TZ = FindTZ(Date);                          // find TZ;

   ly = (!(Y&0x03) && !(Y%100==0) && !(Y=2000)); // is this a leapyear?

   lyrs = ((Y-1-1976)/4) -                     // leapyears since 197>8<;
          ((Y-1-1900)/100 - (Y>2000?1:0));     // not including year Y;
   
   ds->ds_Days = lyrs          * 366       +   // 366 days per leapyear;
                 (Y-1978-lyrs) * 365       +   // 365 days per non-leapyear;
                 MDays[Mo]+((ly&&Mo>2)?1:0)+   // days to start of month
                 (D-1);                        // days in the month
   
   ds->ds_Minute = H * 60 + M - (AddTZOffset ? Loc->loc_GMTOffset + TZ : 0);

   while (ds->ds_Minute < 0)
      ds->ds_Days -=1, ds->ds_Minute += 24 * 60;
   
   while (ds->ds_Minute >= 24 * 60)
      ds->ds_Days +=1, ds->ds_Minute -= 24 * 60;

   ds->ds_Tick = TICKS_PER_SECOND * S;         // seconds into minute
   
   return SUCCESS;
}


/*****************************************************************************
 * Function: parse article date string, return age                           *
 *****************************************************************************/

static LONG ParseArticleDate(STRPTR Date, struct DateStamp *dsart,
                             ULONG AddTZOffset)
{
   LONG   Diff;
   struct DateStamp dsnow;

   if (NetDateToDS(Date, dsart, AddTZOffset) != SUCCESS) return LONG_MAX;

   DateStamp(&dsnow);

   Diff = (dsnow.ds_Days-dsart->ds_Days)*(24*60*SWV_NArticle_AgeTicksPerMin) +
          (dsnow.ds_Minute - dsart->ds_Minute) * SWV_NArticle_AgeTicksPerMin +
          (dsnow.ds_Tick   - dsart->ds_Tick) / TICKS_PER_SECOND /
             SWV_NArticle_SecsPerAgeTick;

   return Diff;
}


/*****************************************************************************
 * Method: parse article date string, return age                             *
 *****************************************************************************/

METHOD(SWM_NArticle_ParseDate, SWP_NArticle_ParseDate *)
{
   LONG t;
   struct DateStamp dbuff, *dsart = msg->DS ? msg->DS : &dbuff;
   STRPTR Date = SW_STRDEF(msg->Date,
                           FindHeaderDataC(idata, SWF_NAHeader_Date, t));

   if (SW_STRISNULL(Date))
      SWDB_RET(LONG_MAX);

   SWDB_RET(ParseArticleDate(Date, dsart, msg->AddTZOffset));
} SDFT

   
/*****************************************************************************
 * Method: Find articles age in units of 4-sec since 1978                    *
 *****************************************************************************/

METHOD(SWM_NArticle_Age, SWP_NArticle_Age *)
{
   LONG   t, Diff;
   STRPTR Date;
   struct DateStamp dbuff, *dsart = msg->DS ? msg->DS : &dbuff;

   if (idata->CachedAge) {
      if (msg->DS)
         msg->DS->ds_Days   = idata->Cached_ds_Days,
         msg->DS->ds_Minute = idata->Cached_ds_Minute,
         msg->DS->ds_Tick   = 0;

      SWDB_RET(idata->CachedAge);
   }

   if (!(Date = FindHeaderDataC(idata, SWF_NAHeader_Date, t)))
      SWDB_RET(LONG_MAX);

   Diff = ParseArticleDate(Date, dsart, TRUE);

   idata->Cached_ds_Days   = dsart->ds_Days;
   idata->Cached_ds_Minute = dsart->ds_Minute;

   SWDB_RET(idata->CachedAge = Diff);
} SDFT

   
/*****************************************************************************
 * Method: Set flag for given article                                        *
 *****************************************************************************/

METHOD(SWM_NArticle_FlagSet, SWP_NArticle_Flags *)
{
   SWDB_RET(SetState(o, idata,
                     (GetState(idata) & ~SWF_NArticle_CachedHead)|msg->Flags));
} SDFT


/*****************************************************************************
 * Method: Reset flag for given article                                      *
 *****************************************************************************/

METHOD(SWM_NArticle_FlagReset, SWP_NArticle_Flags *)
{
   SWDB_RET(SetState(o, idata,
                     GetState(idata) & ~(msg->Flags|SWF_NArticle_CachedHead)));
} SDFT


/*****************************************************************************
 * Method: Determine whether any of flags are set                            *
 *****************************************************************************/

METHOD(SWM_NArticle_FlagTest, SWP_NArticle_Flags *)
{
   SWDB_RET(GetState(idata) & msg->Flags);
} SDFT



/*****************************************************************************
 * Method: Change permanent state                                            *
 *****************************************************************************/

METHOD(SWM_NArticle_SetPerm, ULONG *)
{
   STRPTR cdir =GetTagUB(SWT_NArticle_CacheDir,NULL,                    msg+1);
   ULONG PFlg  =GetTagUL(SWT_NArticle_Perm,    SWF_NArticle_PermToggle, msg+1);
   ULONG f       = AChkFlg(o, SWF_NArticle_AllFlags),
         Current = FIsSet(f, SWF_NArticle_Permanent),
         NewState;

   // -- see if we`re already in the proper state ----------------------------

   if (PFlg == SWF_NArticle_PermYes &&  Current ||
       PFlg == SWF_NArticle_PermNo  && !Current) SWDB_RET(0);

   // -- otherwise, figure out desired new state -----------------------------

   switch (PFlg) {
     case SWF_NArticle_PermNo:     NewState = FALSE;    break;
     case SWF_NArticle_PermYes:    NewState = TRUE;     break;
     case SWF_NArticle_PermToggle: NewState = !Current; break;
     default: SWDB_RET(0);
   }

   if (NewState) ASetFlg(o,   SWF_NArticle_Permanent);
   else          AResetFlg(o, SWF_NArticle_Permanent);

   if (DoMethod(o, NewState ? SWM_NArticle_Cache : SWM_NArticle_UnCache,
                SWT_NArticle_CacheDir, cdir,
                TAG_DONE) != SUCCESS)
      if (NewState) AResetFlg(o, SWF_NArticle_Permanent);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: Parse name field                                                *
 *****************************************************************************/

static void ParseFromField(SW_IDATA *idata,  STRPTR FullName,
                           STRPTR NameBuff,  ULONG  NameBuffLen,
                           STRPTR EmailBuff, ULONG  EmailBuffLen)
{
   STRPTR N = FullName, t;
   LONG  x, len;

   if (!N) N = (STRPTR)FindHeaderDataC(idata, SWF_NAHeader_From, x);

   if (NameBuff)  NameBuff[0]  = '\0';
   if (EmailBuff) EmailBuff[0] = '\0';
   
   if (SW_STRISNULL(N) || (!NameBuff && !EmailBuff)) return;

   // -- handle: "Name" <email> ----------------------------------------------

   if (N[0] == '"' && (t = strchr(N+1, '"'))) {
      if (NameBuff) {
         strncpy(NameBuff, N+1, x = min(NameBuffLen-1, t-N-1));
         NameBuff[x] = '\0';
      }

      if (EmailBuff && (N = strchr(t, '<')))
         if (t = strchr(N+1, '>')) {
            strncpy(EmailBuff, N+1, x=min(EmailBuffLen-1, t-N-1));
            EmailBuff[x] = '\0';
         }

      return;
   }

   // -- handle: email (Name) ------------------------------------------------

   if ((t=strchr(N, '(')) && t!=N && t[-1] == ' ') {
      if (NameBuff) {
         x = max(strlen(t+1)-1, 0);
         strncpy(NameBuff, t+1, x = min(NameBuffLen-1, x));
         NameBuff[x] = '\0';
      }

      if (EmailBuff) {
         while (*--t == ' ');
         strncpy(EmailBuff, N, x=min(EmailBuffLen-1, t-N+1));
         EmailBuff[x] = '\0';
      }

      return;
   }

   // -- handle: Name <email> ------------------------------------------------

   if (t=strchr(N, '<')) {
      if (NameBuff) {
         x = min(NameBuffLen-1, t>N ? t-N-1 : 0);
         strncpy(NameBuff, N, x);
         NameBuff[x] = '\0';
      }

      if (EmailBuff && (N = strchr(t, '>'))) {
         strncpy(EmailBuff, t+1, x=min(EmailBuffLen-1, N-t-1));
         EmailBuff[x] = '\0';
      }

      if (!SW_STRISNULL(NameBuff))
         return;
   }

   len = strlen(N);

   if (NameBuff) {
      strncpy(NameBuff, N, x = min(NameBuffLen-1, len));
      NameBuff[x] = '\0';
   }

   if (EmailBuff && strchr(N, '@')) {
      strncpy(EmailBuff, N, x = min(EmailBuffLen-1, len));
      EmailBuff[x] = '\0';
   }
}


/*****************************************************************************
 * Function: get user email addr                                             *
 *****************************************************************************/

static STRPTR GetEmail(SW_IDATA *idata, STRPTR Buff, ULONG BuffLen)
{
   ParseFromField(idata, NULL, NULL, NULL, Buff, BuffLen);
   return Buff;
}


/*****************************************************************************
 * Function: get user name                                                   *
 *****************************************************************************/

static STRPTR GetShortName(SW_IDATA *idata, STRPTR Buff, ULONG BuffLen)
{
   ParseFromField(idata, NULL, Buff, BuffLen, NULL, NULL);
   return Buff;
}


/*****************************************************************************
 * Function: get user abbrev                                                 *
 *****************************************************************************/

STRPTR GetUserAbbrev(SW_IDATA *idata, STRPTR Buff)
{
   ULONG x, y, cpos;
   TEXT Name[64];
   STRPTR atsign;

#  define MaxAbLen  5
#  define IsComp(c) ((c) == ' ' || (c) == '.' || (c) == ',' || (c) == '-' || \
                     (c) == '_' || (c) == '&' || (c) == '+')

#  define IsAbIgn(c) ((c)=='('||(c)==')')

   // -- get user name, minus any @... garbage -------------------------------

   if (atsign = strchr(GetShortName(idata, Name, 64), '@'))
      *atsign = '\0';

   // -- make an acronym -----------------------------------------------------

   for (x=y=cpos=0; Name[x] && cpos < MaxAbLen; x++)
      if (IsComp(Name[x])) {
         if (!IsAbIgn(Name[y]))
            Buff[cpos++] = Name[y];
         while (Name[++x] == ' ');
         if (!IsAbIgn(Name[y]))
            y = x;
      }

   Buff[cpos++] = Name[y];
   Buff[cpos]   = '\0';

#  undef IsComp
#  undef MaxAbLen

   return Buff;
}


/*****************************************************************************
 * Method: Make short name from full name                                    *
 *****************************************************************************/

METHOD(SWM_NArticle_ParseName, SWP_NArticle_ParseName *)
{
   ParseFromField(idata, msg->FullName,
                  msg->NameBuff, msg->NameBuffLen,
                  msg->EmailBuff, msg->EmailBuffLen);

   SWDB_RET(msg->NameBuff ? msg->NameBuff : msg->EmailBuff);
} SDFT


/*****************************************************************************
 * Function: compare article                                                 *
 *****************************************************************************/

static STRPTR CmpFields[] = {
   "(Off)",
   "Article Age",
   "Author Name",
   "Group Count",
   "Reference Order",
   "Line Count",
   "Subject",
   "# Followups",
   SWF_NAHeader_From,
   SWF_NAHeader_Distrib,
   "Read < Unread",
   "Perm < Not Perm",
   "Size",
   NULL,
};

static LONG ArtCmp(Object *o1, Object *o2, Class *cl,
                   LONG Force, struct SWS_NArticle_CmpInfo *CmpInfo)
{
   ULONG Pos;
   STRPTR v1, v2;
   LONG  t, Cmp = 0;
   SW_IDATA *idata1 = INST_DATA(cl, o1);
   SW_IDATA *idata2 = INST_DATA(cl, o2);

   if (o1 == o2) return 0;
   
   for (Pos=0; Pos < CmpInfo->Count; Pos++) {
      ULONG Attr   = CmpInfo->Attrs[Pos];

      switch (Attr) {
        case SWV_NArticle_Off: break;
         
        case SWV_NArticle_SortAge:
         Cmp= AAge(o2, idata2) - AAge(o1, idata1);
         break;
         
        case SWV_NArticle_SortAuthor: {
           TEXT  b1[64], b2[64];
           Cmp=Stricmp(GetShortName(idata1,b1,64), GetShortName(idata2,b2,64));
           break;
        }
         
        case SWV_NArticle_SortGroups:
         Cmp= AGrpCnt(o1, idata1) - AGrpCnt(o2, idata2);
         break;
   
        case SWV_NArticle_SortRef: {
           Object *b1, *b2;
           
           if (Cmp = DoMethod(CmpInfo->ArtMgr, SWM_NArticleMgr_ThreadInfo,
                              o1, o2, &b1, &b2, NULL))
              return Cmp; // don`t reverse followups!

           if (!b1 || !b2) return 0;
                 
           if (b1 == o1 && b2 == o2) { Force = TRUE; break; }

           CmpInfo->Attrs[Pos] = SWV_NArticle_Off;
           Cmp = ArtCmp(b1, b2, cl, TRUE, CmpInfo);
           CmpInfo->Attrs[Pos] = Attr;

           break;
        }
         
        case SWV_NArticle_SortLines:
         v1 = FindHeaderDataC(idata1, SWF_NAHeader_Lines, t);
         v2 = FindHeaderDataC(idata2, SWF_NAHeader_Lines, t);
         Cmp=(v1 && v2) ? atol(v1) - atol(v2) : 0;
         break;
   
        case SWV_NArticle_SortSubj:
         v1 = ASubj(o1, idata1);
         v2 = ASubj(o2, idata2);

         Cmp = Stricmp(SWV_NArticle_SubjNoRe(v1), SWV_NArticle_SubjNoRe(v2));
         break;

        case SWV_NArticle_SortFollowups:
         Cmp = idata1->SWA_NArticle_Followups - idata2->SWA_NArticle_Followups;
         break;
         
        case SWV_NArticle_SortFrom:
        case SWV_NArticle_SortDistrib:
         v1 = (STRPTR)DoMethod(o1,SWM_NArticle_FindHeader, CmpFields[Attr]);
         v2 = (STRPTR)DoMethod(o2,SWM_NArticle_FindHeader, CmpFields[Attr]);
         Cmp = v1 && v2 ? Stricmp(v1, v2) : 0;
         break;

        case SWV_NArticle_SortRead: {
           ULONG f1 = AFlags(o1, idata1), f2 = AFlags(o2, idata2);

           Cmp = AFlagCmp(f1, f2, SWF_NArticle_Read);
           break;
        }

        case SWV_NArticle_SortPerm: {
           ULONG f1 = AFlags(o1, idata1), f2 = AFlags(o2, idata2);

           Cmp = AFlagCmp(f1, f2, SWF_NArticle_Permanent);
           break;
        }

        case SWV_NArticle_SortSize:
         Cmp = ABytes(o1, idata1) - ABytes(o2, idata2);
         break;
   
      }

      if (Cmp) break;
   }

   if (!Cmp)
      if (Force)
         Cmp = AAge(o2, idata2) - AAge(o1, idata1);
      else return Cmp;

   return (CmpInfo->Dirs[Pos] == SWV_NArticle_ForwardSort) ? Cmp : -Cmp;
}
   

/*****************************************************************************
 * Method: Compare 2 articles                                                *
 *****************************************************************************/

METHOD(SWM_NArticle_Cmp, SWP_NArticle_Cmp *)
{
   if (!msg->Article) SWDB_RET(CmpFields);

   SWDB_RET(ArtCmp(o, msg->Article, cl, FALSE, msg->CmpInfo));
} SDFT


/*****************************************************************************
 * Method: Get MsgID (Cached)                                                *
 *****************************************************************************/

METHOD(SWM_NArticle_GetMsgID, Msg)
{
   LONG t;
   if (idata->CachedMsgID) SWDB_RET(idata->CachedMsgID);

   SWDB_RET(idata->CachedMsgID=FindHeaderDataC(idata, SWF_NAHeader_MsgID, t));
} SDFT


/*****************************************************************************
 * Method: Get Size (Cached)                                                 *
 *****************************************************************************/

METHOD(SWM_NArticle_GetBytes, Msg)
{
   LONG   t;
   STRPTR s;
   
   if (idata->CachedBytes >= 0) SWDB_RET(idata->CachedBytes);

   if (!(s = FindHeaderDataC(idata, SWF_NAHeader_Bytes, t)))
      s = "0";
   
   SWDB_RET(idata->CachedBytes = atol(s));
} SDFT


/*****************************************************************************
 * Method: Get Subj (Cached)                                                 *
 *****************************************************************************/

METHOD(SWM_NArticle_GetSubj, Msg)
{
   LONG t;
   
   if (idata->CachedSubj) SWDB_RET(idata->CachedSubj);

   SWDB_RET(idata->CachedSubj=FindHeaderDataC(idata, SWF_NAHeader_Subject, t));
} SDFT


/*****************************************************************************
 * Method: Hash filename                                                     *
 *****************************************************************************/

METHOD(SWM_NArticle_HashFName, ULONG *)
{
   STRPTR Str  = GetTagUB(SWT_NArticle_HashStr, NULL, msg+1);
   STRPTR Buff = GetTagUB(SWT_NArticle_HashBuff,  NULL, msg+1);

   if (SW_STRISNULL(Str) || !Buff) SWDB_RET(NULL);

   SWDB_RET(HashFileName(Buff, Str));
} SDFT


/*****************************************************************************
 * Method: Kill article (preserve MsgID, Flags, Groups)                      *
 *****************************************************************************/

METHOD(SWM_NArticle_Kill, ULONG *)
{
   STRPTR cdir = GetTagUB(SWT_NArticle_CacheDir, NULL, msg+1);

   ULONG   f = GetState(idata);
   LONG    L_MsgID, L_Groups, L_State, L_Date;
   STRPTR  T_MsgID, T_Groups, T_State, T_Date;

   if (SW_STRISNULL(cdir))
      SWDB_RET(FAILURE);
   
   if (FIsSet(GetState(idata), SWF_NArticle_Permanent))
      SWDB_RET(FAILURE);

   if (DoMethod(o, SWM_NArticle_FastUnCache,
                SWT_NArticle_CacheDir, cdir,
                TAG_DONE) != SUCCESS)
      SWDB_RET(FAILURE);

   L_MsgID  = FindHeaderLineC(idata, SWF_NAHeader_MsgID);
   L_Groups = FindHeaderLineC(idata, SWF_NAHeader_Groups);
   L_State  = FindHeaderLineC(idata, SWF_NAHeader_State);
   L_Date   = FindHeaderLineC(idata, SWF_NAHeader_Date);

   if (L_MsgID < 0 || L_Groups < 0 || L_Date < 0)
      SWDB_RET(FAILURE);

   T_MsgID  = ATxtLn(L_MsgID),  ATxtLn(L_MsgID)  = NULL;
   T_Groups = ATxtLn(L_Groups), ATxtLn(L_Groups) = NULL;
   T_Date   = ATxtLn(L_Date),   ATxtLn(L_Date)   = NULL;
   if (L_State >= 0)
      T_State  = ATxtLn(L_State),  ATxtLn(L_State)  = NULL;

   ClearBuffLines(o, idata);

   ALines() = L_State>=0 ? 4 : 3;
   ATxt()   = (STRPTR *)AV(ASize(ALines()));

   ATxtLn(0) = T_MsgID;
   ATxtLn(1) = T_Groups;
   ATxtLn(2) = T_Date;
   if (L_State >= 0)
      ATxtLn(3) = T_State;

   SetState(o, idata,
            (f | SWF_NArticle_Killed | SWF_NArticle_Read) &
              ~(SWF_NArticle_QueuedDL | SWF_NArticle_NewArt));
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Inc # of followups (for speed, avoid get/set)                     *
 *****************************************************************************/

METHOD(SWM_NArticle_IncFollowups, Msg)
{
   SWDB_RET(++idata->SWA_NArticle_Followups);
} SDFT


/*****************************************************************************
 * Method: Get a bunch of overview info at once (for performance)            *
 *****************************************************************************/

METHOD(SWM_NArticle_OVInfo, SWP_NArticle_OVInfo *)
{
   STRPTR ts;
   LONG t;

   if (!ATxt()) {
      msg->OVInfo->GroupCount   = 0;
      msg->OVInfo->Subject      = "";
      msg->OVInfo->Hash         = 0;
      msg->OVInfo->Followups    = 0;
      msg->OVInfo->Flags        = GetState(idata);
      msg->OVInfo->Age          = 0;
      msg->OVInfo->ds.ds_Days   = 0;
      msg->OVInfo->ds.ds_Minute = 0;
      msg->OVInfo->ds.ds_Tick   = 0;
      msg->OVInfo->Lines        = 0;
      msg->OVInfo->SNameBuff[0] = '\0';
      msg->OVInfo->Bytes        = 0;      
      SWDB_RET(0);
   }
   
   msg->OVInfo->Hash = 
      idata->CachedIDHash ? idata->CachedIDHash :
         DoMethod(o, SWM_NArticle_GetIDHash);

   msg->OVInfo->GroupCount   = AGrpCnt(o, idata);
   msg->OVInfo->Subject      = ASubj(o, idata);
   msg->OVInfo->Followups    = idata->SWA_NArticle_Followups;
   msg->OVInfo->Flags        = GetState(idata);
   msg->OVInfo->Age          = AAge(o, idata);
   msg->OVInfo->Bytes        = ABytes(o, idata);
   msg->OVInfo->ds.ds_Days   = idata->Cached_ds_Days;
   msg->OVInfo->ds.ds_Minute = idata->Cached_ds_Minute;
   msg->OVInfo->ds.ds_Tick   = 0;

   msg->OVInfo->Lines  = ts  = FindHeaderDataC(idata, SWF_NAHeader_Lines, t);

   // -- some line headers have trailing spaces ------------------------------
   if (ts && ts[0]) {
      t = strlen(ts)-1;
      while (ts[0] && ts[t] == ' ')
         ts[t--] = '\0';
   }

   GetShortName(idata, msg->OVInfo->SNameBuff, msg->OVInfo->SNBuffLen);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Get a bunch of overview info at once (for performance)            *
 *****************************************************************************/

METHOD(SWM_NArticle_GetIDHash, Msg)
{
   if (idata->CachedIDHash) SWDB_RET(idata->CachedIDHash);

   SWDB_RET(idata->CachedIDHash =
            StrHash((STRPTR)DoMethod(o, SWM_NArticle_GetMsgID)));
} SDFT


/*****************************************************************************
 * Method: Uncache w/o forced file remove if possible                        *
 *****************************************************************************/

METHOD(SWM_NArticle_FastUnCache, ULONG *)
{
   STRPTR cdir = GetTagUB(SWT_NArticle_CacheDir, NULL, msg+1);

   if (!FIsSet(GetState(idata), SWF_NArticle_CachedHdBd))
      SWDB_RET(SUCCESS);

   SWDB_RET(DoMethod(o, SWM_NArticle_UnCache,
                     SWT_NArticle_CacheDir, cdir,
                     TAG_DONE));
} SDFT


/*****************************************************************************
 * Method: Check if we can expire an article                                 *
 *****************************************************************************/

METHOD(SWM_NArticle_CanExpire, SWP_NArticle_CanExpire *)
{
   ULONG Flags = GetState(idata);

   SWDB_RET(((AAge(o, idata) > msg->AgeLimit) ||
             FIsSet(Flags, SWF_NArticle_Expired)) &&
            !FIsSet(Flags, SWF_NArticle_Permanent));
} SDFT


/*****************************************************************************
 * Function: Find article size                                               *
 *****************************************************************************/

static LONG ByteSize(SW_IDATA *idata, ULONG Parts)
{
   LONG x, size=0;

   if (!ATxt()) return 0;
   
   ALineLoop(x, idata, Parts, 0)
      size += strlen(ATxtLn(x))+1;

   return size;
}


/*****************************************************************************
 * Method: Determine byte size of article                                    *
 *****************************************************************************/

METHOD(SWM_NArticle_ByteSize, ULONG *)
{
   SWDB_RET(ByteSize(idata, GetTagUL(SWT_NArticle_Parts,
                                     SWF_NArticle_HdBd, msg+1)));
} SDFT


/*****************************************************************************
 * Function: htoul                                                           *
 *****************************************************************************/

static ULONG htoul(STRPTR *s)
{
   ULONG val = 0;
   UBYTE c;
   if (!s) return 0;

   while (TRUE) {
      c = (*s)[0];
      if (!c || !isdigit(c) && (tolower(c)<'a' || tolower(c)>'f'))
         break;

      (*s)++;
      
      val = val * 16 + avtod(c);
   }

   return val;
}


/*****************************************************************************
 * Method: parse file comment                                                *
 *****************************************************************************/

METHOD(SWM_NArticle_ParseComment, SWP_NArticle_ParseComment *)
{
   STRPTR Comment = msg->Comment;
   ULONG  ID;
   
   if (!Comment)                   SWDB_RET(FALSE);
   if (strncmp(Comment, "ID:", 3)) SWDB_RET(FALSE);

   Comment += 3;

   ID = htoul(&Comment);

   SWDB_RET(ID);
} SDFT


/*****************************************************************************
 * Method: get cached reference ID list                                      *
 *****************************************************************************/

METHOD(SWM_NArticle_Refs, Msg)
{
   STRPTR lastpos, rpos, Refs;
   LONG   t, RefCount;
   UBYTE  c;

   if (idata->CachedRefs) SWDB_RET(idata->CachedRefs);
   
   if (!(Refs = FindHeaderDataC(idata, SWF_NAHeader_Ref, t)))
      SWDB_RET(idata->CachedRefs = SWV_NArticle_NoRefs);
   
   // -- first count refs ----------------------------------------------------

   for (RefCount=0, rpos=strchr(Refs, '<'); rpos;  rpos = strchr(rpos+1, '<'))
      RefCount++;    

   if (!RefCount)
      SWDB_RET(idata->CachedRefs = SWV_NArticle_NoRefs);

   if (!(idata->CachedRefs = (ULONG *)AV((RefCount+1) * sizeof(ULONG))))
      SWDB_RET(NULL);

   idata->CachedRefs[0] = RefCount;

   lastpos  = rpos = Refs + strlen(Refs);
   
   while (TRUE) {
      while (lastpos[-1] == ' ') lastpos--;

      rpos = lastpos;
      while (rpos[0] != '<' && rpos >= Refs ) rpos--;
      if (rpos < Refs) break;

      c = *lastpos, *lastpos = '\0';

      idata->CachedRefs[RefCount--] = StrHash(rpos);

      *lastpos = c, lastpos = --rpos;
   }

   SWDB_RET(idata->CachedRefs);
}


/*****************************************************************************
 * Function: date format hook                                                *
 *****************************************************************************/

static __saveds __asm void DateFmtFn(a0 struct Hook *hook, a1 ULONG c)
{
   STRPTR Buff = (STRPTR)hook->h_Data;

   Buff[(((ULONG *)Buff)[-1])++] = (UBYTE)c;
}



/*****************************************************************************
 * Function: check for a date variable                                       *
 *****************************************************************************/

static STRPTR GetDateVar(Object *o, SW_IDATA *idata, STRPTR Name)
{
   ULONG x;
   struct DateStamp ds;
   static TEXT DateBuff[64];
   struct Hook DateHook;
   static struct DateVars { UBYTE *VarName; UBYTE *Template; } DateVar[] =
   {
      "ArtAmPm",           "%p",
      "ArtDayNum",         "%d",
      "ArtDayNumSp",       "%e",
      "ArtHour12",         "%I",
      "ArtHour12Sp",       "%Q",
      "ArtHour24",         "%H",
      "ArtHour24Sp",       "%q",
      "ArtMin",            "%M",
      "ArtMonth",          "%B",
      "ArtMonthNum",       "%m",
      "ArtMonthShort",     "%b",
      "ArtSec",            "%S",
      "ArtWeekday",        "%A",
      "ArtWeekdayShort",   "%a",
      "ArtYear2",          "%y",
      "ArtYear4",          "%Y",
      NULL,             NULL,
   };

   for (x=0; DateVar[x].VarName; x++)
      if (!strcmp(DateVar[x].VarName, Name))
         break;

   if (!DateVar[x].VarName) return NULL;

   DateHook.h_Entry = (ULONG (*)())DateFmtFn;
   DateHook.h_Data  = (void *)(DateBuff + sizeof(ULONG));

   memset(DateBuff, 0, sizeof(ULONG));

   DoMethod(o, SWM_NArticle_ParseDate, NULL, &ds, FALSE);

   FormatDate(Loc, DateVar[x].Template, &ds, &DateHook);

   return DateBuff + sizeof(ULONG);
}


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   LONG t;
   static TEXT VarBuf[128];
   STRPTR s;

   if (!AHasHead(idata))
      SWDB_RET(NULL);
   
   VL_FindRetNum("ArtLines", idata->SWA_NArticle_Lines, VarBuf, "%ld");

   VL_FindRet("ArtSubj",      DoMethod(o, SWM_NArticle_GetSubj));
   VL_FindRet("ArtMsgID",     DoMethod(o, SWM_NArticle_GetMsgID));
   VL_FindRet("ArtDate",      FindHeaderDataC(idata, SWF_NAHeader_Date,     t));
   VL_FindRet("ArtRef",       FindHeaderDataC(idata, SWF_NAHeader_Ref,      t));
   VL_FindRet("ArtFrom",      FindHeaderDataC(idata, SWF_NAHeader_From,     t));

   VL_FindRet("ArtGroups",    FindHeaderDataC(idata, SWF_NAHeader_Groups,   t));
   VL_FindRet("ArtSender",    FindHeaderDataC(idata, SWF_NAHeader_Sender,   t));
   VL_FindRet("ArtDistrib",   FindHeaderDataC(idata, SWF_NAHeader_Distrib,  t));
   VL_FindRet("ArtNewsRd",    FindHeaderDataC(idata, SWF_NAHeader_X_NewsRd, t));
   VL_FindRet("ArtFollowup",  FindHeaderDataC(idata, SWF_NAHeader_Followup, t));
   VL_FindRet("ArtKeywords",  FindHeaderDataC(idata, SWF_NAHeader_Keywords, t));
   VL_FindRet("ArtOrganiz",   FindHeaderDataC(idata, SWF_NAHeader_Organiz,  t));
   VL_FindRet("ArtPath",      FindHeaderDataC(idata, SWF_NAHeader_Path,     t));
   VL_FindRet("ArtMimeVer",   FindHeaderDataC(idata, SWF_NAHeader_MimeVer,  t));
   VL_FindRet("ArtMimeType",  FindHeaderDataC(idata, SWF_NAHeader_MimeType, t));
   VL_FindRet("ArtUserName",  GetShortName(idata, VarBuf, 128));
   VL_FindRet("ArtUserEmail", GetEmail(idata, VarBuf, 128));
   VL_FindRet("ArtUserAbbrev",GetUserAbbrev(idata, VarBuf));

   if (s = GetDateVar(o, idata, msg->Name))
      SWDB_RET(s);

   if (!strncmp(msg->Name, "ArtPart", 7)) {
      ULONG Part, MaxPart;

      if (!DoMethod(o, SWM_NArticle_ParseMPInfo, &Part, &MaxPart, NULL, NULL))
         SWDB_RET("");

      VL_FindRetNum("ArtPartNum",  Part,    VarBuf,  "%ld");
      VL_FindRetNum("ArtPartTot",  MaxPart, VarBuf,  "%ld");
      VL_FindRetNum("ArtPartNum,2", Part,    VarBuf, "%02ld");
      VL_FindRetNum("ArtPartTot,2", MaxPart, VarBuf, "%02ld");
      VL_FindRetNum("ArtPartNum,3", Part,    VarBuf, "%03ld");
      VL_FindRetNum("ArtPartTot,3", MaxPart, VarBuf, "%03ld");
      VL_FindRetNum("ArtPartNum,4", Part,    VarBuf, "%04ld");
      VL_FindRetNum("ArtPartTot,4", MaxPart, VarBuf, "%04ld");
   }
   
   SWDB_RET(NULL);
} SDFT


/*****************************************************************************
 * Method: process arexx command                                             *
 *****************************************************************************/

METHOD_NOINLINE(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *)
{
   STRPTR cmd = ARG0(msg->Rx), Val;
   Object *App = msg->App;
   TEXT TmpBuff[256];
   ULONG x;

   if (!strncmp(cmd, "FINDHEADER ", 11)) {
      strncpy(TmpBuff, cmd+11, 250)[250] = '\0';
      x = strlen(TmpBuff);
      if (TmpBuff[x-1] != ':') strcat(TmpBuff, ":"), x++;
      
      Val = (STRPTR)DoMethod(o, SWM_NArticle_FindHeader, TmpBuff);

      set(App, MUIA_Application_RexxString, SW_STRDEF(Val, ""));
      
      goto OK;
   }

   if (!strcmp(cmd, "ISREAD")) {
      set(App, MUIA_Application_RexxString,
          (GetState(idata) & SWF_NArticle_Read) ? "1" : "0");
      goto OK;
   }

   if (!strcmp(cmd, "ISPERM")) {
      set(App, MUIA_Application_RexxString,
          (GetState(idata) & SWF_NArticle_Permanent) ? "1" : "0");
      goto OK;
   }

   if (!strcmp(cmd, "ISUSERMK")) {
      set(App, MUIA_Application_RexxString,
          (GetState(idata) & SWF_NArticle_UserMk) ? "1" : "0");
      goto OK;
   }

   if (!strcmp(cmd, "ISNEW")) {
      set(App, MUIA_Application_RexxString,
          (GetState(idata) & SWF_NArticle_NewArt) ? "1" : "0");
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


/*****************************************************************************
 * Function: See if this the beginning of an ISO word                        *
 *****************************************************************************/

static __inline LONG IsISOBegin(STRPTR s, UBYTE *Encoding)
{
   LONG x, qcount, qspos;

   if (s[0] != '=' || s[1] != '?') return 0;

   if (Encoding) *Encoding = '\0';
   
   for (qcount=0, x=2; x<75; x++)
      if (s[x] == '?') {
         if (++qcount == 1 && Encoding)
            *Encoding = s[x+1];
         else if (qcount == 2)
            qspos = x + 1;
         else if (qcount == 3 && s[x+1] == '=')
            return qspos;
      } else if (s[x] == ' ')
         return 0;

   return 0;
}


/*****************************************************************************
 * Function: Return hex value from 2 ASCII hex digits                        *
 *****************************************************************************/

static __inline LONG HexVal(STRPTR s)
{
   return (avtod(s[0])<<4) | avtod(s[1]);
}


/*****************************************************************************
 * Function: Fix an ISO string                                               *
 *****************************************************************************/

static STRPTR FixISO(STRPTR s)
{
   ULONG x, y, z, qspos;
   UBYTE Encoding, c;

   for (x=y=0; s[x]; x++)
      if (qspos = IsISOBegin(s+x, &Encoding)) {
         // -- quoted-printable header ---------------------------------------
         
         if (Encoding=='Q' || Encoding=='q') {
            for (x+=qspos; s[x] && !(s[x] == '?' && s[x+1] == '='); x++) {
               if (s[x] == '_')
                  s[y++] = ' ';
               else if (s[x] == '=')
                  ++x, s[y++] = HexVal(s + x++);
               else
                  s[y++] = s[x];
            }
   
            x++;
   
         } else if (Encoding=='B' || Encoding=='b') {
            // -- base64 header ----------------------------------------------
            
            for (x+=qspos, z = x; s[z] && !(s[z] == '?' && s[z+1] == '='); z++);
            c    = s[z];
            s[z] = '\0';
            SWBase64Decode(s + x, 0, NULL, NULL, NULL, s+y);
            y += strlen(s+y);
            s[z] = c;
            x = z+1;
         }

         if (s[x+1] == ' ' && IsISOBegin(s+x+2, NULL))
            x++;
      } else s[y++] = s[x];

   s[y] = '\0';
   
   return s;
}


/*****************************************************************************
 * Method: Fix ISO headers (=? ?=)                                           *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NArticle_FixISOHdrs, ULONG *)
{
   LONG x;
   
   ALineLoop(x, idata, SWF_NArticle_Head, 0)
      FixISO(ATxtLn(x));
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Normalize header case                                             *
 *****************************************************************************/

METHOD_NOINLINE(SWM_NArticle_FixHdrCase, ULONG *)
{
   LONG x, y;

   struct { STRPTR Hdr; ULONG Len; } Hdrs[] = {
      SWF_NAHeader_Subject,  sizeof(SWF_NAHeader_Subject)-1,
      SWF_NAHeader_Lines,    sizeof(SWF_NAHeader_Lines)-1,
      SWF_NAHeader_Date,     sizeof(SWF_NAHeader_Date)-1,
      SWF_NAHeader_Ref,      sizeof(SWF_NAHeader_Ref)-1,
      SWF_NAHeader_From,     sizeof(SWF_NAHeader_From)-1,
      SWF_NAHeader_Xref,     sizeof(SWF_NAHeader_Xref)-1,
      SWF_NAHeader_MsgID,    sizeof(SWF_NAHeader_MsgID)-1,
      SWF_NAHeader_Bytes,    sizeof(SWF_NAHeader_Bytes)-1,
      SWF_NAHeader_Groups,   sizeof(SWF_NAHeader_Groups)-1,
      SWF_NAHeader_Sender,   sizeof(SWF_NAHeader_Sender)-1,
      SWF_NAHeader_Distrib,  sizeof(SWF_NAHeader_Distrib)-1,
      SWF_NAHeader_Followup, sizeof(SWF_NAHeader_Followup)-1,
      SWF_NAHeader_Keywords, sizeof(SWF_NAHeader_Keywords)-1,
      SWF_NAHeader_Organiz,  sizeof(SWF_NAHeader_Organiz)-1,
      SWF_NAHeader_Path,     sizeof(SWF_NAHeader_Path)-1,
      SWF_NAHeader_MimeVer,  sizeof(SWF_NAHeader_MimeVer)-1,
      SWF_NAHeader_MimeType, sizeof(SWF_NAHeader_MimeType)-1,
      SWF_NAHeader_MimeDesc, sizeof(SWF_NAHeader_MimeDesc)-1,
      SWF_NAHeader_MimeCTE,  sizeof(SWF_NAHeader_MimeCTE)-1,
      NULL,                  0,
   };
   
   ALineLoop(x, idata, SWF_NArticle_Head, 0)
      for (y=0; Hdrs[y].Hdr; y++)
         if (tolower(Hdrs[y].Hdr[0]) == tolower(ATxtLn(x)[0]) &&
             !Strnicmp(ATxtLn(x), Hdrs[y].Hdr, Hdrs[y].Len))
            if (strncmp(ATxtLn(x), Hdrs[y].Hdr, Hdrs[y].Len))
               strncpy(ATxtLn(x), Hdrs[y].Hdr, Hdrs[y].Len);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Copy given header to clipboard                                    *
 *****************************************************************************/

METHOD(SWM_NArticle_ClipHeader, SWP_NArticle_ClipHeader *)
{
   struct IFFHandle *iff;
   STRPTR s = (STRPTR)DoMethod(o, SWM_NArticle_FindHeader, msg->Header);

   if (!s) SWDB_RET(FAILURE);

   if (!(iff = SWOpenClipWrite(0))) SWDB_RET(FAILURE);

   SWTextToClip(iff, s);
   SWCloseClipWrite(iff);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Copy article parts to clipboard                                   *
 *****************************************************************************/

METHOD(SWM_NArticle_Clipboard, SWP_NArticle_Clipboard *)
{
   LONG x;
   struct IFFHandle *iff;

   if (!(iff = SWOpenClipWrite(0))) SWDB_RET(FAILURE);

   // -- copy indicated parts to clipborad -----------------------------------
   
   ALineLoop(x, idata, msg->Parts, 0) {
      SWTextToClip(iff, ATxtLn(x));
      SWTextToClip(iff, "\n");
   }
      
   SWCloseClipWrite(iff);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Find # of lines in article                                        *
 *****************************************************************************/

METHOD(SWM_NArticle_GetLines, Msg)
{
   LONG t, l = 0;
   STRPTR lh;

   if (lh = FindHeaderDataC(idata, SWF_NAHeader_Lines, t))
      l = atol(lh);
   
   SWDB_RET(l);
} SDFT


/*****************************************************************************
 * Method: Parse Multipart Info (subject [m/n]).  Return T if it has some.   *
 *****************************************************************************/

METHOD(SWM_NArticle_ParseMPInfo, SWP_NArticle_ParseMPInfo *)
{
   STRPTR Subj0 = (idata->CachedSubj ? idata->CachedSubj :
                   (STRPTR)DoMethod(o, SWM_NArticle_GetSubj)),
          Subj1, s0, s1;
   LONG m, n;

   // -- parse subject -------------------------------------------------------

   if (!Subj0) SWDB_RET(FALSE);

   Subj0 = SWV_NArticle_SubjNoRe(Subj0);

   // -- see if this is part of the same MP string ---------------------------

   if (msg->Art) {
      LONG  t;
      STRPTR Auth0 = FindHeaderDataC(idata, SWF_NAHeader_From,     t),
             Auth1 = (STRPTR)DoMethod(msg->Art, SWM_NArticle_FindHeader,
                                      SWF_NAHeader_From);

      Subj1 = (STRPTR)DoMethod(msg->Art, SWM_NArticle_GetSubj);      
      Subj1 = SWV_NArticle_SubjNoRe(Subj1);

      if (SW_STRISNULL(Subj1) || strcmp(Auth0, Auth1) != 0)
         SWDB_RET(FALSE);
   }
   
   // -- parse part numbers [n/m] or (n/m) -----------------------------------

   for (s0=Subj0; *s0; s0++)
      if (*s0=='(' || *s0=='[') {
         s1 = s0++;
         if (!IsDigit(*s0)) continue;
         n=m=0;
         while (IsDigit(*s0)) n = n * 10 + (*s0++)-'0';
         if (*s0++ != '/') continue;
         while (IsDigit(*s0)) m = m * 10 + (*s0++)-'0';
         if (*s0!=')' && *s0!=']') continue;

         if (msg->SubjPos) *msg->SubjPos = s1 - Subj0;
         if (msg->Part)    *msg->Part    = n;
         if (msg->MaxPart) *msg->MaxPart = m;

         SWDB_RET(n >= 0 && m > 0 && n <= m && m > 1 &&
                  (!msg->Art || strncmp(Subj0, Subj1, s1 - Subj0) == 0));
      }

   SWDB_RET(FALSE);
} SDFT


/*****************************************************************************
 * Method: Get article text as an AllocVec'ed buffer                         *
 *****************************************************************************/

METHOD(SWM_NArticle_GetTxt, ULONG *)
{
   STRPTR Buff, bp;
   ULONG  Parts   = GetTagUL(SWT_NArticle_Parts, SWF_NArticle_HdBd, msg+1),
         *ResLen  = GetTagTyp(SWT_NArticle_ResLen, NULL, msg+1, ULONG *),
          Size    = 1,
          x;

   // -- first, count size we need to allocate -------------------------------
   
   ALineLoop(x, idata, Parts, 0)
      Size += strlen(ATxtLn(x)) + 1;

   // -- allocate enough space for this buffer -------------------------------
   
   if (!(bp = Buff = AllocVec(Size, MEMF_ANY))) SWDB_RET(NULL);

   // -- copy the text into our buffer ---------------------------------------

   ALineLoop(x, idata, Parts, 0)
      bp   += strlen(strcpy(bp, ATxtLn(x))),
      *bp++ = '\n',
      *bp   = '\0';

   if (ResLen) *ResLen = bp - Buff;

   SWDB_RET(Buff);
} SDFT
