# include "S-Hash.h"


/*****************************************************************************
 * Function: Suspend event processing during big loop or something           *
 *****************************************************************************/

static ULONG StartBusy(SW_IDATA *idata)
{
   if (NN_App(idata))
      set(NN_App(idata), MUIA_Application_Sleep, TRUE);

   return 0;
}


/*****************************************************************************
 * Function: Startup event processing again after busy                       *
 *****************************************************************************/

static ULONG EndBusy(SW_IDATA *idata)
{
   if (NN_App(idata))
      set(NN_App(idata), MUIA_Application_Sleep, FALSE);

   return 0;
}


/*****************************************************************************
 * Function: Check refresh during busy loop                                  *
 *****************************************************************************/

static ULONG CheckRefresh(SW_IDATA *idata)
{
   if (NN_App(idata))
      DoMethod(NN_App(idata), MUIM_Application_CheckRefresh);
   return 0;  // so we can assign this fn to count;
}


/*****************************************************************************
 * Function: Find message ID position in header array                        *
 *****************************************************************************/

# define _IsHdr(x,h) (strncmp((x), (h), sizeof(h)-1) == 0)
# define _IsMID(x)   _IsHdr(x, SWF_NAHeader_MsgID)

# ifdef NOTDEF
static LONG MsgIDPos(STRPTR *Array, ULONG size)
{
   LONG x;
   
   for (x=0; x<size; x++)
      if (_IsMID(Array[x]))
         return x;

   return -1;
}
# endif

/*****************************************************************************
 * Function: find an articles message ID header                              *
 *****************************************************************************/

static __inline STRPTR FindMsgID(Object *art)
{
   return !art ? NULL : (STRPTR)DoMethod(art, SWM_NArticle_GetMsgID);
}


/*****************************************************************************
 * Function: Determine if article has a particular message ID                *
 *****************************************************************************/

static __inline LONG CmpMsgID(Object *art, ULONG Hash, STRPTR ID)
{
   STRPTR ID2;

   if (!art) return FALSE;

   return ID ? ((ID2=FindMsgID(art)) ? !strcmp(ID, ID2) : 0) :
      (DoMethod(art, SWM_NArticle_GetIDHash) == Hash);
}


/*****************************************************************************
 * Function: Put article into hash table - must be unique!!!                 *
 *****************************************************************************/

static LONG AddArticleInt(Object **a, Object *art, ULONG Size, ULONG Hash,
                          ULONG *MaxChain)
{
   ULONG h, Count;

   if (!Hash) Hash = DoMethod(art, SWM_NArticle_GetIDHash);

   StrHashLoop(h, Hash, Size, Count, a[h] && Count<=SWV_NArticleMgr_MaxChain);

   if (a[h])
      return -1;
   
   a[h] = art;
   *MaxChain = max(*MaxChain, Count);
   return (LONG)h;
}


/*****************************************************************************
 * Function: notify groups about new/deleted articles                        *
 *****************************************************************************/

static ULONG NotifyGroups(Object *o, SW_IDATA *idata, ULONG method, Object *art,
                          Object **arts, ULONG Count, ULONG Last)
{
   Object *Art[1];
   ULONG x;

   if (!art && (!arts || !Count))        return 0;
   if (!idata->SWA_NArticleMgr_Master)   return 0;

   for (Art[0] = art, x=0; x<SWV_NArticleMgr_MaxGrps; x++)
      if (idata->Group[x])
         DoMethod(idata->Group[x], method,
                  SWT_NArticleMgr_ArticleArray, art ? Art : arts,
                  SWT_NArticleMgr_ArticleCount, art ?   1 : Count,
                  SWT_NArticleMgr_LastSet,      Last,
                  TAG_DONE);

   if (method == SWM_NArticleMgr_NewArticles)
      DoMethod(o, SWM_NArticleMgr_ReadListUpd, art ? Art:arts, art ? 1:Count);

   return 0;
}


/*****************************************************************************
 * Method: Find total size of all articles                                   *
 *****************************************************************************/

//# define HSTATS

static LONG FindArtPos(SW_IDATA *, Object *, STRPTR, ULONG);
METHOD(SWM_NArticleMgr_HashStats, Msg)
{
#  ifdef HSTATS
   ULONG h, hv, notfound=0;
   ULONG x, maxlen=0;
   static LONG lens[64];

   memset(lens, 0, 64*sizeof(LONG));

   if (!AArray() || !ACount()) SWDB_RET(0);
   
   for (x=0; x<ASize(); x++) {
      STRPTR ID = FindMsgID(Art(x));
      if (ID) {
         ULONG found, len;

         StrHashLoop(h, StrHash(ID), ASize(), len, len <= idata->MaxChain)
            if (found=CmpMsgID(Art(h), StrHash(ID), ID))
               break;

         if (FindArtPos(idata, Art(x), NULL, NULL) < 0) {
            Printf("NF: %s\n", ID);
            notfound++;
         }

         if (FindArtPos(idata, NULL, ID, NULL) < 0) {
            Printf("NF 2: %s\n", ID);
            notfound++;
         }

         maxlen = max(maxlen, len);
         lens[len]++;
      }
   }

   Printf("Table size: %ld  Used: %ld  Notfound: %ld\n",
          ASize(), ACount(), notfound);
   for (hv=0, h=1; h<=maxlen; hv += h * lens[h], ++h)
      Printf("  %ld: %ld\n", h, lens[h]);

   Printf("Max chain: %ld  Average*100: %ld\n",
          maxlen, hv*100 / ACount());
      
#  endif

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: Free array and all articles                                     *
 *****************************************************************************/

static void FreeThreadArray(SW_IDATA *idata, ULONG FreeContents)
{
   ULONG x;
   
   if (FreeContents && idata->SWA_NArticleMgr_Responses)
      for (x=0; x<ASize(); x++)
         if (RespA(x))
            FV(RespA(x));

   FV(idata->SWA_NArticleMgr_PrevPos);
   FV(idata->SWA_NArticleMgr_LocRefs);
   FV(idata->SWA_NArticleMgr_Responses);

   idata->SWA_NArticleMgr_PrevPos   = NULL;
   idata->SWA_NArticleMgr_LocRefs   = NULL;
   idata->SWA_NArticleMgr_Responses = NULL;
}


/*****************************************************************************
 * Function: Free threading data                                             *
 *****************************************************************************/

static void FreeArticleArray(SW_IDATA *idata)
{
   ULONG x;

   if (idata->SWA_NArticleMgr_Master)
      for (x=0; x<ASize(); x++)
         DisposeObject(Art(x));

   FV(AArray());
   AArray()                    = NULL;
   ACount()                    = 0;
   ASize()                     = 0;
   idata->SWA_NArticleMgr_SPos = 0;

   FreeThreadArray(idata, TRUE);
}


/*****************************************************************************
 * Function: Increase size of hash table (slow)                              *
 *****************************************************************************/

static ULONG SizeArticleArray(SW_IDATA *idata, ULONG NewCount)
{
   Object **a;
   ULONG  x, NewSize, MaxChain;

   // -- just return if we`re under 70% full ---------------------------------
   
   if ((NewCount * 100 / max(ASize(), 1)) < 70)
      return SUCCESS;

   // -- determine next hash table size increment ----------------------------

   if (!(NewSize = HTSizes[idata->SWA_NArticleMgr_SPos++]))
      return FAILURE;

   // -- free old thread info ------------------------------------------------

   FreeThreadArray(idata, TRUE);

   // -- allocate new article array ------------------------------------------

   if (!(a = (Object **)AV(NewSize * sizeof(Object *)))) {
      FreeArticleArray(idata);
      return FAILURE;
   }

   memset(a, 0, NewSize * sizeof(Object *));
   idata->ThreadMod = TRUE;

   // -- copy articles from old array ----------------------------------------
   
   for (x=MaxChain=0; x<ASize(); x++)
      if (Art(x))
         if (AddArticleInt(a, Art(x), NewSize, NULL, &MaxChain) < 0)
            if (idata->SWA_NArticleMgr_Master)
               DisposeObject(Art(x));

   FV(AArray());

   AArray() = a;
   ASize()  = NewSize;
   idata->MaxChain = MaxChain;

   return SUCCESS;
}


/*****************************************************************************
 * Function: Add an article (must not be dupe)                               *
 *****************************************************************************/

static LONG AddArticle(SW_IDATA *idata, Object *art, ULONG Hash)
{
   LONG rv;
   
   if (SizeArticleArray(idata, ACount() + 1) == FAILURE || !AArray())
      return -1;

   idata->ThreadMod = TRUE;
   if ((rv = AddArticleInt(AArray(), art, ASize(), Hash, &idata->MaxChain)) >= 0)
      ACount()++;
   
   return rv;
}


/*****************************************************************************
 * Function: Find an article`s position in the array by MsgID, -1 = not found*
 *****************************************************************************/

static LONG FindArtPos(SW_IDATA *idata, Object *art, STRPTR ID, ULONG Hash)
{
   ULONG h, Count, found = 0;

   if (!AArray()) return -1;

   if (!Hash) {
      if (art) Hash = DoMethod(art, SWM_NArticle_GetIDHash);
      else {
         if (SW_STRISNULL(ID)) return -1;
         Hash = StrHash(ID);
      }
   }

   if (art) {
      StrHashLoop(h, Hash, ASize(), Count, Count <= idata->MaxChain)
         if (Art(h) && (found = (Art(h) == art)))
            break;
   } else
      StrHashLoop(h, Hash, ASize(), Count, Count <= idata->MaxChain)
         if (Art(h) && (found = CmpMsgID(Art(h), Hash, ID)))
            break;

   return found ? (LONG)h : -1;
}


/*****************************************************************************
 * Function: Add article only if it doesn`t exist: 0=added, 1=freed          *
 *****************************************************************************/

static ULONG AddArticleUniqFree(SW_IDATA *idata, Object *art)
{
   STRPTR ID    = FindMsgID(art);
   ULONG  Hash  = StrHash(ID);

   if (FindArtPos(idata, NULL, ID, Hash) >= 0) {
      if (idata->SWA_NArticleMgr_Master)
         DisposeObject(art);
      return SWV_NArticleMgr_Freed;
   }
   
   AddArticle(idata, art, Hash);

   return SWV_NArticleMgr_Added;
}

   
/*****************************************************************************
 * Function: Remove an article from the array (no dispose)                   *
 *****************************************************************************/

static Object *RemArticleInt(SW_IDATA *idata, LONG pos)
{
   Object *art;
   if (pos < 0) return NULL;

   art = Art(pos), Art(pos) = NULL;

   if (art) {
      ACount()--;
      idata->ThreadMod = TRUE;
   }

   return art;
}

static void DisposeOne(SW_IDATA *idata, LONG pos)
{
   Object *art = RemArticleInt(idata, pos);
   
   if (idata->SWA_NArticleMgr_Master && art)
      DisposeObject(art);
}


/*****************************************************************************
 * Function: return unused headermatch filter                                *
 *****************************************************************************/

static LONG FindFreeMT(SW_IDATA *idata)
{
   LONG x;

   for (x=0; x<idata->MTMax; x++)
      if (!idata->MTUse[x])
         return x;

   return -1;
}


static Object *AllocMTFlt(SW_IDATA *idata, Object *SubFlt, ULONG thrdmode)
{
   LONG  x;
   ULONG TMode = SWGetUL(SubFlt, SWA_NSFilter_ThreadMode);

   if (!thrdmode || TMode == SWV_NSFilter_TM_Base)
      return SubFlt;
   
   // -- if no free slot, resize array ---------------------------------------
   
   if ((x = FindFreeMT(idata)) < 0) {
      idata->MTMax += 8;
      if (RV(idata->MTMax * sizeof(Object *), &idata->MTFlt) < 0)
         return NULL;

      if (RV(idata->MTMax * sizeof(UBYTE), &idata->MTUse) < 0)
         return NULL;
   }

   if (x < 0)
      if ((x = FindFreeMT(idata)) < 0)
         return NULL;
      
   if (!idata->MTFlt[x])
      idata->MTFlt[x] = MUI_NewObject(SWC_NSF_MatchThread, TAG_DONE);

   if (!idata->MTFlt[x]) return NULL;

   idata->MTUse[x] = TRUE;
   SetAttrs(idata->MTFlt[x],
            SWA_NSF_MatchThread_FPtr, SubFlt,
            SWA_NSF_MatchThread_Mode, TMode == SWV_NSFilter_TM_Any ?
               SWV_NSF_MatchThread_Any :
               SWV_NSF_MatchThread_All,
            TAG_DONE);
   return idata->MTFlt[x];
}


static void FreeMTFlt(SW_IDATA *idata, Object *MT)
{
   LONG x;

   for (x=0; x<idata->MTMax; x++)
      if (idata->MTFlt[x] == MT) {
         idata->MTUse[x] = FALSE;
         set(idata->MTFlt[x], SWA_NSF_MatchThread_FPtr, NULL);
         return;
      }
}


/*****************************************************************************
 * Method: See if article meets condition                                    *
 *   Stack[0] = (0 = match all), (1 = and), (2 = or)                         *
 *   Stack[1] = TRUE to invert match                                         *
 *   Stack[2] = Group pointer                                                *
 *   Stack[3] = Select filter object...                                      *
 *   Stack[4] = Select filter object...                                      *
 *   Stack[N] = NULL                                                         *
 *****************************************************************************/

static LONG StartEndMatch(SW_IDATA *idata, ULONG *Stack, ULONG BE,
                          ULONG thrdmode)
{
   LONG    x;
   Object *Flt;

   idata->FltOK = SUCCESS;

   // -- startmatch ----------------------------------------------------------
   
   if (BE == SWM_NSFilter_Begin && Stack && Stack[0]) {
      if (!idata->FltList[idata->FltPos])
         idata->FltList[idata->FltPos] =
            MUI_NewObject(SWC_NSFList,
                          SWA_NSFList_NoDispose, TRUE,
                          SWA_NList_NoNotifies,  TRUE,
                          TAG_DONE);

      if (!idata->FltList[idata->FltPos]) {
         idata->FltOK = FAILURE;
         return 0;
      }

      for (x=3; Stack[x]; x++)
         if (SWGetUL((Object *)Stack[x], SWA_NSFilter_Enabled))
            DoMethod(idata->FltList[idata->FltPos], MUIM_List_InsertSingle,
                     AllocMTFlt(idata, (Object *)Stack[x], thrdmode),
                     MUIV_List_Insert_Bottom);

      SetAttrs(idata->FltList[idata->FltPos],
               SWA_NSFilter_Inv, Stack[1],
               SWA_NSFList_Mode, Stack[0] == 1,
               TAG_DONE);

      idata->FltOK = DoMethod(idata->FltList[idata->FltPos++], BE, Stack[2]);
   }

   // -- endmatch ------------------------------------------------------------
   
   if (BE == SWM_NSFilter_End && Stack && Stack[0]) {
      DoMethod(idata->FltList[--idata->FltPos], BE, Stack[2]);

      if (thrdmode)
         SW_MUIListLoop(idata->FltList[idata->FltPos], x, Flt, TRUE)
            FreeMTFlt(idata, Flt);
      
      DoMethod(idata->FltList[idata->FltPos], MUIM_List_Clear);
   }

   return 0;
}

static ULONG IsMatch(SW_IDATA *idata, Object *Art, ULONG *Stack)
{
   if (!Art || idata->FltOK == FAILURE) return FALSE;
   if (!Stack || !Stack[0])             return TRUE;

   return DoMethod(idata->FltList[idata->FltPos-1], SWM_NSFilter_Match,
                   Art, Stack[2]);
}


/*****************************************************************************
 * Function: Get article count & Array from tags                             *
 *****************************************************************************/

static void GetArticleArray(SW_IDATA *idata, ULONG *msg,
                            ULONG *ArtCount, Object ***ArtArray, ULONG UseDef)
{
   *ArtCount = GetTagUL(SWT_NArticleMgr_ArticleCount, 0, msg);
   *ArtArray = GetTagTyp(SWT_NArticleMgr_ArticleArray, NULL, msg, Object **);

   if (!*ArtArray && UseDef) {
      *ArtArray = AArray();
      *ArtCount = ASize();
   }
}


/*****************************************************************************
 * Method: Delete matching articles from memory                              *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_DelArts, ULONG *)
{
   ULONG x;
   ULONG  ArtCount;
   Object **ArtArray;

   GetArticleArray(idata, msg+1, &ArtCount, &ArtArray, FALSE);
   
   NotifyGroups(o, idata, SWM_NArticleMgr_DelArticles, NULL,
                ArtArray, ArtCount, TRUE);

   for (x=0; x<ArtCount; x++)
      DisposeOne(idata, FindArtPos(idata, ArtArray[x], NULL, NULL));

   DoMethod(o, SWM_NArticleMgr_MakeThreads);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Delete matching articles from memory                              *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_KillArts, ULONG *)
{
   ULONG  x;
   ULONG  ArtCount;
   Object **ArtArray;
   STRPTR CDir      = GetTagUB(SWT_NArticleMgr_CacheDir, NULL, msg+1);

   GetArticleArray(idata, msg+1, &ArtCount, &ArtArray, FALSE);

   NotifyGroups(o, idata, SWM_NArticleMgr_DelArticles, NULL,
                ArtArray, ArtCount, TRUE);

   for (x=0; x<ArtCount; x++)
      DoMethod(ArtArray[x], SWM_NArticle_Kill,
               SWT_NArticle_CacheDir, CDir,
               TAG_DONE);

  Done:
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Put matching articles in cache                                    *
 *     stack[1] = cache dir                                                  *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_Cache, ULONG *)
{
   // NOT IMPLEMENTED
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Remove matching articles from cache                               *
 *    stack[1] = cache dir                                                   *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_UnCache, ULONG *)
{
   ULONG x, count;

   ALoop(idata, x, msg+2, count, 50, ASize(), AArray(), FALSE)
      DoMethod(Art(x), SWM_NArticle_UnCache,
               SWT_NArticle_CacheDir, (STRPTR)msg[1],
               TAG_DONE);

   SWDB_RET(0);
} SDFT
   

/*****************************************************************************
 * Method: Read matching articles from cache & insert (NOT IMPLEMENTED)      *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_GetCache, ULONG *)
{
   SWDB_RET(0);
} SDFT
   

/*****************************************************************************
 * Method: Add new article(s) to the database                                *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_Add, ULONG *)
{
   ULONG x, pos;

   ULONG  LastSet   = GetTagUL(SWT_NArticleMgr_LastSet, FALSE,  msg+1);
   Object *Art      = GetTagOB(SWT_NArticleMgr_Article, NULL,   msg+1);
   ULONG  ArtCount;
   Object **ArtArray;

   GetArticleArray(idata, msg+1, &ArtCount, &ArtArray, FALSE);
   
   if (!Art && !ArtCount) SWDB_RET(0);

   if (Art)
      if (AddArticleUniqFree(idata, Art) == SWV_NArticleMgr_Freed)
         SWDB_RET(0);

   for (x=pos=0; ArtArray && x<ArtCount; x++)
      if (AddArticleUniqFree(idata, ArtArray[x]) == SWV_NArticleMgr_Added)
         ArtArray[pos++] = ArtArray[x];

   ArtCount = pos;
   
   DoMethod(o, SWM_NArticleMgr_MakeThreads);

   NotifyGroups(o, idata, SWM_NArticleMgr_NewArticles, Art, ArtArray, ArtCount,
                LastSet);

   SWDB_RET(0);
} SDFT
   

/*****************************************************************************
 * Method: Find an article based on its message ID                           *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_Find, SWP_NArticleMgr_Find *)
{
   LONG pos = FindArtPos(idata, NULL, msg->MsgID, NULL);

   SWDB_RET(pos >=0 ? Art(pos) : NULL);
} SDFT

   
/*****************************************************************************
 * Function: allocate a disk buffer - if not enough mem, try smaller         *
 *****************************************************************************/

static STRPTR AllocIOBuff(Object *o, SW_IDATA *idata, ULONG *Size)
{
   STRPTR Buff;

   if (!Size) return NULL;

   *Size = SWV_NArticleMgr_IOBuffSize;

   while (*Size >= 16*1024 && !(Buff = AV(*Size))) *Size /= 2;

   if (!Buff)
      SWERR(o, NULL, SWS_Err_Alloc, 0, 0);

   return Buff;
}

   
/*****************************************************************************
 * Method: Save overview file                                                *
 *    Msg[1] = Save path                                                     *
 *    Msg[2] = optional .Data string                                         *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_SaveOverview, ULONG *)
{
   STRPTR Data,
          Path  = (STRPTR)msg[1],
          UData = (STRPTR)msg[2],
          Buff  = NULL;
   BPTR   fp = NULL;
   ULONG  x, y, rc = FAILURE, BLen=0, count, count2 = 0, records=0, BSize;
   UBYTE  FNBuff[FMSIZE];

   if (SW_STRISNULL(Path)) {
      SWERR(o, NULL, SWS_Err_SaveOV_NoPath, 0, 0);
      goto Done;
   }

   if (SWMakePath(Path) != SUCCESS) {
      SWERR(o, NULL, SWS_Err_SaveOV_Mkdir, Path, 0);
      goto Done;
   }

   if (!(Buff = AllocIOBuff(o, idata, &BSize))) goto Done;

   AddPart(strcpy(FNBuff, Path), SWV_DefOVFile, FMSIZE);

   if (!(fp = Open(FNBuff, MODE_NEWFILE))) {
      SWERR(o, NULL, SWS_Err_SaveOV_Open, Path, SWV_DefOVFile);
      goto Done;
   }

   set(o, SWA_NArticleMgr_OVPct, 0);

   ALoop(idata, x, msg+3, count, 50, ASize(), AArray(), FALSE) {
      if (DoMethod(Art(x), SWM_NArticle_CanExpire, LONG_MAX))
         continue;

      for (y=0; OVFields[y]; y++) {
         Data = (STRPTR)DoMethod(Art(x), SWM_NArticle_FindHeader, OVFields[y]);
         if (!SW_STRISNULL(Data)) {
            ULONG dlen    = strlen(Data),
                  incsize = strlen(OVFields[y]) + dlen + 2;

#           ifdef DEBUGTCP
               Printf("%s: %s\n", __FUNC__, Data);
#           endif

            if (BLen > (BSize - incsize - 4)) {
               if (Write(fp, Buff, BLen) != BLen) {
                  SWERR(o, NULL, SWS_Err_SaveOV_Write, Path, SWV_DefOVFile);
                  goto Done;
               }
               BLen = 0;
            }

            strcpy(Buff+BLen, OVFields[y]); BLen += strlen(OVFields[y]);
            Buff[BLen++] = ' ';
            strcpy(Buff+BLen, Data);        BLen += dlen;
            Buff[BLen++] = '\n';
         }
      }

      Buff[BLen++] = '.';
      Buff[BLen++] = '\n';

      records++;

      if (count2++ > 100) {  // store % complete
         count2 = 0;
         set(o, SWA_NArticleMgr_OVPct, x*100/ASize());
      }
   }

   if (BLen > 0)
      if (Write(fp, Buff, BLen) != BLen) {
         SWERR(o, NULL, SWS_Err_SaveOV_Write, Path, SWV_DefOVFile);
         goto Done;
      }

   Close(fp);
   set(o, SWA_NArticleMgr_OVPct, 100);

   // -- write overview data to the data file --------------------------------

   AddPart(strcpy(FNBuff, Path), SWV_DefOVData, FMSIZE);
   if (fp = Open(FNBuff, MODE_NEWFILE)) {
      FPrintf(fp, SWV_OVD_Records "%ld\n", records);
      FPuts(fp, UData);
   } else goto Done;

   rc = SUCCESS;

 Done:
   if (fp) Close(fp);
   FV(Buff);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: load overview data file                                         *
 *****************************************************************************/

static ULONG LoadOVData(STRPTR Path, STRPTR GBuff)
{
   ULONG Total = 1000;  // guess if we don`t know;
   BPTR fp;
   UBYTE line[FMSIZE];
   
   // -- read initial data from overview data file ---------------------------

   if (GBuff) GBuff[0] = '\0';
      
   AddPart(Path, SWV_DefOVData, FMSIZE);
   
   if (fp = Open(Path, MODE_OLDFILE)) {
      while (FGets(fp, line, FMSIZE)) {
         SW_REMOVENL(line);

         if (!strncmp(line, SWV_OVD_Records, sizeof(SWV_OVD_Records)-1))
            Total = max(atol(line+sizeof(SWV_OVD_Records)-1), 1);

         if (!strncmp(line, SWV_OVD_Group, sizeof(SWV_OVD_Group)-1))
            if (GBuff) strcpy(GBuff, line + sizeof(SWV_OVD_Group)-1);
      }

      Close(fp);
   }
   
   return Total;
}


/*****************************************************************************
 * Method: Load overview file                                                *
 *   msg[1] = OV load path                                                   *
 *   msg[2] = expire age in artticks                                         *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_LoadOverview, ULONG *)
{
   LONG    line, count=0, AAPos=0, Start=FALSE, InBytes = 0,
           InBPos = 0, LinePos, ReadTot = 0, Found = FALSE;
   Object  *art, **AArray = NULL, *MA = NN_ArtTpl(idata);
   STRPTR  in, Path = (STRPTR)msg[1];
   STRPTR  *Data = NULL, Partial = NULL;
   ULONG   x, DataSize = 0, DataCount = 0, Part = FALSE;
   UBYTE   FNBuff[FMSIZE];
   ULONG   rc = FAILURE, Total, Recs = 0, BSize;
   BPTR    fp = NULL;
   STRPTR  Buff = NULL;

   if (SW_STRISNULL(Path)) {
      SWERR(o, FAILURE, SWS_Err_LoadOV_NoPath, 0, 0);
      goto Done;
   }

   if (!SWDirExists(Path)) SWDB_RET(SUCCESS);

   if (!(AArray = (Object **)AV(SWV_NArticleMgr_OVLoadInt*sizeof(Object *)))) {
      SWERR(o, FAILURE, SWS_Err_Alloc, 0, 0);
      goto Done;
   }

   if (!(Partial = (STRPTR)AV(SWV_NArticleMgr_OVLineSize))) {
      SWERR(o, FAILURE, SWS_Err_Alloc, 0, 0);
      goto Done;
   }

   if (!(Buff = AllocIOBuff(o, idata, &BSize))) goto Done;

   set(o, SWA_NArticleMgr_OVPct, 0);

   Total = LoadOVData(strcpy(FNBuff, Path), NULL);

   AddPart(strcpy(FNBuff, Path), SWV_DefOVFile, FMSIZE);

   if (!SWFileExists(FNBuff)) {
      rc = SUCCESS;
      goto Done;
   }

   // -- read all the lines from the file ------------------------------------

   if (!(fp = Open(FNBuff, MODE_OLDFILE))) {
      SWERR(o, NULL, SWS_Err_LoadOV_Open, Path, SWV_DefOVFile);
      goto Done;
   }

   Start = TRUE;
   StartBusy(idata);

   while (TRUE) {
      // -- read more data from buffer ---------------------------------------

      if (InBPos >= InBytes) {
         if ((InBytes = Read(fp, Buff, BSize)) < 0) {
            SWERR(o, NULL, SWS_Err_LoadOV_Read, Path, SWV_DefOVFile);
            goto Done;
         }

         if (InBytes == 0) break;
         
         InBPos = 0;
      }

      // -- find next newline ------------------------------------------------

      for (LinePos = -1, x=InBPos; x<InBytes; x++)
         if (Buff[x] == '\n') {
            LinePos = x;
            Buff[x] = '\0';
            break;
         }

      if (LinePos < 0) {
         strncpy(Partial, Buff+InBPos, InBytes - InBPos);
         Partial[InBytes - InBPos] = '\0';
         Part   = TRUE;
         InBPos = InBytes;
         continue;
      }

      if (Part) {
         strcat(Partial, Buff + InBPos);
         in   = Partial;
         Part = FALSE;
      } else
         in = Buff + InBPos;

      if (_IsMID(in))
         if (FindArtPos(idata, NULL, in+sizeof(SWF_NAHeader_MsgID),NULL) >= 0)
            Found = TRUE;

      if (in[0] == '.' && in[1] == '\0') {
         ACheckRI(count, 50);

         if (!Found) {
            ReadTot++;
            Data[DataCount++] = NULL;
         }

         if (!(++Recs % 25))  // store % complete;
         set(o, SWA_NArticleMgr_OVPct, Recs * 50 / Total);
         InBPos = LinePos + 1;
         Found  = FALSE;
         continue;
      }

      if (Found) {
         InBPos = LinePos + 1;
         continue;
      }

      if ((DataCount+1) >= DataSize) {
         DataSize += 512;
         if (RV(DataSize * sizeof(STRPTR), &Data) < 0) {
            SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
            goto Done;
         }
      }

      if (!(Data[DataCount++] = SDup(in))) {
         SWERR(o, FAILURE, SWS_Err_Alloc, 0, 0);
         goto Done;
      }

      InBPos = LinePos + 1;
   }

   Close(fp);
   fp = NULL;
      

   // -- now build objects for all that data ---------------------------------
      
   StartEndMatch(idata, msg+3, SWM_NSFilter_Begin, FALSE);
   Start = TRUE;

   for (x=Recs = 0; x<DataCount; ) {
      STRPTR *ArtLines;
      ULONG  Hash;
      
      if (++Recs % 25)
         set(o, SWA_NArticleMgr_OVPct, 50 + Recs * 50 / max(ReadTot, 1));

      for (line=0; line<=SWV_NArticleMgr_MaxOV; line++) {
         if (!Data[line+x]) break;
         if (*((ULONG *)Data[line+x]) == MAKE_ID('M','e','s','s'))
            Hash = StrHash(Data[line+x]+sizeof(SWF_NAHeader_MsgID));
      }

      if (line < 2) {
         x += line + 1;
         continue;
      }

      if (!(ArtLines = (STRPTR *)AV(line * sizeof(STRPTR)))) {
         SWERR(o, FAILURE, SWS_Err_Alloc, 0, 0);
         goto Done;
      }

      memcpy(ArtLines, Data+x, line*sizeof(STRPTR));

      art = NewObject(OCLASS(MA), NULL,
                      SWA_NArticle_MemPool, idata->SWA_NArticleMgr_MemPool,
                      SWA_NArticle_Txt,     ArtLines,
                      SWA_NArticle_Lines,   line,
                      TAG_DONE);
         
      if (!art) {
         rc = SWERR(o, FAILURE, SWS_Err_LoadOV_Art, 0, 0);
         goto Done;
      }

      if (IsMatch(idata, art, msg+3)) {
         AddArticle(idata, art, Hash);
         AArray[AAPos++] = art;

         if (AAPos >= SWV_NArticleMgr_OVLoadInt) {
            NotifyGroups(o, idata, SWM_NArticleMgr_NewArticles,
                         NULL, AArray, AAPos, FALSE);
            AAPos = 0;
         }
      } else {
         DisposeObject(art);
      }

      x += line + 1;

      ACheckR(count, 25);
      count++;
   } 

   // - send any left over ones ----------------------------------------------
   NotifyGroups(o, idata, SWM_NArticleMgr_NewArticles, NULL, AArray, AAPos, TRUE);
   set(o, SWA_NArticleMgr_OVPct, 100);
   CheckRefresh(idata);

   rc = SUCCESS;

 Done:
   FV(Data);
   FV(AArray);
   FV(Buff);
   FV(Partial);
   
   if (Start) {
      StartEndMatch(idata, msg+3, SWM_NSFilter_End, FALSE);
      DoMethod(o, SWM_NArticleMgr_MakeThreads);
      EndBusy(idata);
   }
   if (fp) Close(fp);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: find a group in the registered list                             *
 *****************************************************************************/

static LONG FindGrpPos(SW_IDATA *idata, Object *Group)
{
   LONG x;
   
   for (x=0; x<SWV_NArticleMgr_MaxGrps; x++)
      if (idata->Group[x] == Group)
         return x;

   return -1;
}


/*****************************************************************************
 * Function: see if article is in any open groups                            *
 *****************************************************************************/

static ULONG ArtInUse(SW_IDATA *idata, Object *art, Object **grpdef,
                      ULONG Grps)
{
   ULONG x;

   for (x=0; x<Grps; x++)
      if (grpdef[x] &&
          DoMethod(grpdef[x], SWM_NSFilter_Match, art, idata->Group[x]))
         return TRUE;

   return FALSE;   
}
                      

/*****************************************************************************
 * Function: remove old articles that aren't in a group, if needed           *
 *****************************************************************************/

static ULONG ShrinkMgr(Object *o, SW_IDATA *idata)
{
   ULONG x, TotSize = 0, count, IDSize = 0,
         Limit = SWGetUL(idata->SWA_AppBase_TopObj,
                         SWA_NNews_HdrCacheSize) * 1024;
   Object *GrpDefs[SWV_NArticleMgr_MaxGrps];

   for (x=0; x<SWV_NArticleMgr_MaxGrps; x++)
      GrpDefs[x] = idata->Group[x] ?
         SWGetObj(idata->Group[x], SWA_NGroup_GrpDefFlt) : NULL;

   // -- first, determine the total byte size --------------------------------
   
   ALoopAll(idata, x, count, 250, ASize(), AArray()) {
      if (!IDSize) IDSize = SWGetUL(Art(x), SWA_NArticle_IDataSize) + 16;
      TotSize += DoMethod(Art(x), SWM_NArticle_ByteSize, TAG_DONE) + IDSize;
   }

   ALoopAll(idata, x, count, 250, ASize(), AArray()) {
      if (ArtInUse(idata, Art(x), GrpDefs, SWV_NArticleMgr_MaxGrps)) {
         continue;
      }

      if (DoMethod(Art(x), SWM_NArticle_FlagTest, SWF_NArticle_Expired)) {
         DisposeOne(idata, x);
         continue;
      }

      if (TotSize > Limit) {
         TotSize -= (DoMethod(Art(x), SWM_NArticle_ByteSize, TAG_DONE)+IDSize);
         DisposeOne(idata, x);
      }
   }

   return SUCCESS;
}


/*****************************************************************************
 * Method: Add a group to notify about changes                               *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_AddGroup, SWP_NArticleMgr_Group *)
{
   LONG p = FindGrpPos(idata, msg->Group);

   if (p >= 0) {
      idata->GrpCnt[p]++;
      SWDB_RET(SUCCESS);
   }

   p = FindGrpPos(idata, NULL);

   if (p >= 0) {
      idata->Group[p]  = msg->Group;
      idata->GrpCnt[p] = 1;
      SWDB_RET(SUCCESS);
   }

   SWDB_RET(FAILURE);
} SDFT


/*****************************************************************************
 * Method: Remove a group to notify about changes                            *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_RemGroup, SWP_NArticleMgr_Group *)
{
   LONG p = FindGrpPos(idata, msg->Group);

   if (p < 0) SWDB_RET(FAILURE);
      
   if (!--idata->GrpCnt[p])
      idata->Group[p] = NULL;

   SWDB_RET(ShrinkMgr(o, idata));
} SDFT


/*****************************************************************************
 * Method: Filter a list of articles                                         *
 *   Stack[1] = Pointer to input array of articles, or UseTbl for all        *
 *   Stack[2] = Count of articles in input array                             *
 *   Stack[3] = Pointer to ptr to output array                               *
 *   Stack[4] = Pointer to ULONG size of output array                        *
 *   Stack[5] = last used element in output array                            *
 *   Stack[6] = threadmode                                                   *
 *   Stack[7] = And/OR (see StartEndMatch)                                   *
 *   Stack[8] = TRUE to invert match                                         *
 *   Stack[9] = Group pointer                                                *
 *   Stack[10] = Select filter object...                                     *
 *   Stack[N] = NULL                                                         *
 *                                                                           *
 * Result = # entries in output array                                        *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_FilterList, ULONG *)
{
   Object  **InArray   = (Object **) msg[1];
   ULONG     InCount   =             msg[2];
   Object ***OutArray  = (Object ***)msg[3];
   ULONG    *OutSize   = (ULONG *)   msg[4];
   ULONG     OutCount  =             msg[5];
   ULONG     x, count;

   // -- if no input array, use our own hash table ---------------------------

   if (!OutSize || !OutArray) SWDB_RET(0);

   if (InArray == (Object **)SWV_NArticleMgr_Filter_UseTbl) {
      InArray = AArray();
      InCount = ASize();
   }

   if (!InCount) SWDB_RET(OutCount);

   // -- iterate through input list, copying matching to output list ---------

   ALoop(idata, x, msg+7, count, 500, InCount, InArray, msg[6]) {
      if (OutCount >= *OutSize) {
         *OutSize += 250;
         if (RV(*OutSize * sizeof(Object *), OutArray) < 0)
            SWDB_RET(SWERR(o, NULL, SWS_Err_Alloc, 0, 0));
      }

      (*OutArray)[OutCount++] = InArray[x];
   }

   SWDB_RET(OutCount);
} SDFT


/*****************************************************************************
 * Function: Find subject in subj hash table                                 *
 *****************************************************************************/

static __inline LONG FindSubj(STRPTR Subj, Object **Tbl, LONG NHSize,
                              LONG MaxChain, ULONG *h, ULONG *hv, ULONG *Count)
{
   if (!Subj) return FALSE;

   Subj = SWV_NArticle_SubjNoRe(Subj);

   StrHashLoop(*h, *hv=StrHash(Subj), NHSize, *Count,
               Tbl[*h] && (*Count)<=MaxChain) {
      STRPTR Subj2 = (STRPTR)DoMethod(Tbl[*h], SWM_NArticle_GetSubj);
      if (SWV_NArticle_SameSubj(Subj, Subj2))
         return TRUE;
   }

   return FALSE;
}


/*****************************************************************************
 * Function: Find article based on references                                *
 *****************************************************************************/

static LONG FindRef(SW_IDATA *idata, Object *Art)
{
   LONG  apos;
   ULONG x, *Refs = (ULONG *)DoMethod(Art, SWM_NArticle_Refs);

   if (!Refs || Refs == SWV_NArticle_NoRefs)
      return -1;
   
   for (x=Refs[0]; x>0; --x)
      if ((apos = FindArtPos(idata, NULL, NULL, Refs[x])) >= 0)
         return apos;

   return -1;
}


/*****************************************************************************
 * Function: build name hash table                                           *
 *****************************************************************************/

static LONG MakeSubjHash(SW_IDATA *idata, Object ***Tbl, LONG *NHMaxChain)
{
   ULONG x, h, hv, Count;
   LONG  NHSize;
   STRPTR Subj;

   if (!ACount()) return 0;

   *NHMaxChain = 0;
   NHSize = ASize() * 2 + 1; // HTSizes[idata->SWA_NArticleMgr_SPos+1];

   if (!(*Tbl = (Object **)AV(sizeof(Object *) * NHSize))) return 0;
   memset(*Tbl, 0, sizeof(Object *) * NHSize);

   // -- build a hash table by message ID ------------------------------------

   for (x=0; x<ASize(); x++) {
      if (!Art(x)) continue;

      // -- skip ones that we can find references for
      if (FindRef(idata, Art(x)) >= 0)
         continue;
      
      if (!(Subj = (STRPTR)DoMethod(Art(x), SWM_NArticle_GetSubj))) continue;

      if (FindSubj(Subj, (*Tbl), NHSize, *NHMaxChain, &h, &hv, &Count)) {
         if ((LONG)DoMethod(Art(x), SWM_NArticle_Age, NULL) >
             (LONG)DoMethod((*Tbl)[h], SWM_NArticle_Age, NULL))
            (*Tbl)[h] = Art(x);
      } else {
         while ((*Tbl)[h] && Count < 25)
            h = StrHashPos(hv, ++Count, NHSize);
         if (!(*Tbl)[h])
            (*Tbl)[h]   = Art(x),
            *NHMaxChain = max(*NHMaxChain, Count);
      }
   }

   return NHSize;
}


/*****************************************************************************
 * Function: add to response array                                           *
 *****************************************************************************/

static ULONG AddRespArray(SW_IDATA *idata, Object ***RArray, Object *Resp,
                          UWORD *RArrayPos)
{
   if (!*RArrayPos || (*RArray)[*RArrayPos] == (Object *)(-1)) {
      UWORD Size = !*RArrayPos ? 3 : *RArrayPos + 2 + (*RArrayPos >> 1);

      if (RV(Size * sizeof(Object *), RArray) < 0) {
         *RArrayPos = 0;
         return FAILURE;
      }

      // -- mark end of allocated space --
      (*RArray)[Size-1] = (Object *)-1;
   }

   (*RArray)[(*RArrayPos)++] = Resp;

   return SUCCESS;
}


/*****************************************************************************
 * Method: Generate thread info                                              *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_MakeThreads, Msg)
{
   LONG x, count, apos, NHSize, NHMaxChain = 0;
   UWORD *RTmp;
   Object **NHTable = NULL;

   if (!idata->SWA_NArticleMgr_Threads || !idata->ThreadMod)
      SWDB_RET(0);

   FreeThreadArray(idata, TRUE);

   if (!ACount()) SWDB_RET(0);

   // -- allocate memory -----------------------------------------------------

   if (!(RTmp = (UWORD *)AV(ASize()* sizeof(UWORD)))                           ||
       !(idata->SWA_NArticleMgr_PrevPos   = (LONG*)AV(ASize()*sizeof(ULONG)))  ||
       !(idata->SWA_NArticleMgr_LocRefs   = (UWORD*)AV(ASize()*sizeof(UWORD))) ||
       !(idata->SWA_NArticleMgr_Responses =
         (Object ***)AV(ASize() * sizeof(Object **)))) {

      FreeThreadArray(idata, FALSE);
      SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
      goto Done;
   }

   memset(idata->SWA_NArticleMgr_PrevPos,   -1, ASize() * sizeof(ULONG));
   memset(idata->SWA_NArticleMgr_LocRefs,    0, ASize() * sizeof(UWORD));
   memset(RTmp,                              0, ASize() * sizeof(UWORD));
   memset(idata->SWA_NArticleMgr_Responses,  0, ASize() * sizeof(Object **));

   // -- make hash table of subjects  ----------------------------------------

   if (!(NHSize = MakeSubjHash(idata, &NHTable, &NHMaxChain))) {
      FreeThreadArray(idata, FALSE);
      SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
      goto Done;
   }
      
   // -- loop through articles, finding references ---------------------------
   
   ALoopAll(idata, x, count, 500, ASize(), AArray()) {
      set(Art(x), SWA_NArticle_Followups, 0);

      // -- loop through references in reverse order & try to find. ----------
      if ((PrevPos(x) = FindRef(idata, Art(x))) < 0) {
         ULONG Count, h, hv;
         if (FindSubj((STRPTR)DoMethod(Art(x),SWM_NArticle_GetSubj),
                      NHTable, NHSize, NHMaxChain, &h, &hv, &Count))
            if (NHTable[h] != Art(x))
               PrevPos(x) = FindArtPos(idata, NHTable[h], NULL, NULL);
      }
   }

   // -- now figure out the chain position for each one ----------------------

   ALoopAll(idata, x, count, 500, ASize(), AArray()) {
      LONG y, prev;

      for (y=prev=x, apos=0;
           ((y=PrevPos(y)) >= 0) && apos < SWV_NArticleMgr_MaxLocRef;
           apos++,prev=y)
         DoMethod(Art(y), SWM_NArticle_IncFollowups);

      if (apos < SWV_NArticleMgr_MaxLocRef) {
         LocRef(x) = (UWORD)apos;
         if (prev != x)
            if (AddRespArray(idata, &RespA(prev), Art(x), &RTmp[prev])
                == FAILURE) {
               FreeThreadArray(idata, TRUE);
               SWERR(o, NULL, SWS_Err_Alloc, 0, 0);
               goto Done;
            }
      } else {
         LocRef(x) = (UWORD)0, PrevPos(x) = -1;
         set(Art(x), SWA_NArticle_Followups, 0);
      }
   }

  Done:
   FV(RTmp);
   FV(NHTable);

   idata->ThreadMod = FALSE;

   SWDB_RET(0);         
} SDFT


/*****************************************************************************
 * Method: Find ancestor to a threaded article                               *
 *****************************************************************************/

static __inline LONG FindAncestor(SW_IDATA *idata, LONG pos, LONG pos2,
                                  LONG *last)
{
   for (*last=pos; (pos=PrevPos(pos)) >= 0 && pos != pos2; *last=pos);

   return pos;
}


/*****************************************************************************
 * Method: Get info about threads                                            *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ThreadInfo, SWP_NArticleMgr_ThreadInfo *)
{
   LONG pos1, pos2, last1, last2;
   
   if (!msg->Art1 || !idata->SWA_NArticleMgr_PrevPos)
      SWDB_RET(0);

   if ((pos1 = FindArtPos(idata, msg->Art1, NULL, NULL)) < 0) {
      if (msg->Base1) *msg->Base1 = NULL;
      if (msg->Base2) *msg->Base2 = NULL;
      SWDB_RET(0);
   }

   if (msg->Nesting)   (*msg->Nesting)   = (ULONG)LocRef(pos1);

   if (!msg->Art2) SWDB_RET(0);

   if ((pos2 = FindArtPos(idata, msg->Art2, NULL, NULL)) < 0) {
      if (msg->Base1) *msg->Base1 = NULL;
      if (msg->Base2) *msg->Base2 = NULL;
      SWDB_RET(0);
   }

   if (FindAncestor(idata, pos1, pos2, &last1) >= 0)
      SWDB_RET(1);
   
   if (FindAncestor(idata, pos2, pos1, &last2) >= 0)
      SWDB_RET(-1);

   if (msg->Base1 && msg->Base2) {
      *msg->Base1 = Art(last1),
      *msg->Base2 = Art(last2);
      for (last1=pos1; (pos1=PrevPos(pos1)) >= 0; last1 = pos1)
         if (FindAncestor(idata, pos2, pos1, &last2) >= 0) {
            *msg->Base1 = Art(last1),
            *msg->Base2 = Art(last2);
            SWDB_RET(0);
         }
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: find base article in thread                                     *
 *****************************************************************************/

static __inline Object *ThreadBase(SW_IDATA *idata, Object *Art)
{
   LONG pos;

   if (!Art) return NULL;

   if ((pos = FindArtPos(idata, Art, NULL, NULL)) < 0)
      return NULL;

   FindAncestor(idata, pos, -1, &pos);

   return  pos >= 0 ? Art(pos) : NULL;
}


/*****************************************************************************
 * Method: Find thread base article                                          *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ThreadBase, SWP_NArticleMgr_ThreadBase *)
{
   SWDB_RET(ThreadBase(idata, msg->Art));
} SDFT


/*****************************************************************************
 * Method: See if two articles are in the same thread                        *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_SameThread, SWP_NArticleMgr_SameThread *)
{
   SWDB_RET(ThreadBase(idata, msg->Art1) == ThreadBase(idata, msg->Art2));
} SDFT


/*****************************************************************************
 * Method: Get list of articles in thread                                    *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_GetThreadArts, SWP_NArticleMgr_GetThreadArts *)
{
   LONG pos;

   if (!msg->Article) SWDB_RET(NULL);

   if ((pos = FindArtPos(idata, msg->Article, NULL, NULL)) < 0)
      return NULL;

   FindAncestor(idata, pos, -1, &pos);
   if (pos < 0) SWDB_RET(NULL);

   if (msg->Base) *msg->Base = Art(pos);

   SWDB_RET(RespA(pos));
} SDFT


/*****************************************************************************
 * Method: Find thread parent article                                        *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ThreadParent, SWP_NArticleMgr_ThreadParent *)
{
   LONG pos;
   
   if ((pos = FindArtPos(idata, msg->Art, NULL, NULL)) < 0)
      SWDB_RET(NULL);

   SWDB_RET((pos = PrevPos(pos)) >= 0 ? Art(pos) : NULL);
} SDFT


/*****************************************************************************
 * Method: Get info about threads                                            *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ThreadRefs, SWP_NArticleMgr_ThreadRefs *)
{
   LONG pos;
   
   if ((pos = FindArtPos(idata, msg->Art, NULL, msg->Hash)) < 0)
      SWDB_RET(0);

   SWDB_RET(LocRef(pos));
} SDFT


/*****************************************************************************
 * Function: remove directory and its files                                  *
 *****************************************************************************/

static ULONG RmDir(Object *o, SW_IDATA *idata, STRPTR FNBuff, STRPTR ACBuff)
{
#  define CCBuff 65536
   struct ExAllControl *eac = AllocDosObject(DOS_EXALLCONTROL, TAG_DONE);
   struct ExAllData    *ead;
   ULONG more, count = 100, rc = FAILURE;
   struct ExAllData    *buff  = (void *)AV(CCBuff);
   BPTR  lk;

   if (!eac || !buff) {
      rc = SWERR(o, FAILURE, SWS_Err_Alloc, 0, 0);
      goto Done;
   }
   
   if (lk = Lock(FNBuff, ACCESS_READ)) {
      for (more=1,eac->eac_LastKey=0; more; ) {
         more = ExAll(lk, buff, CCBuff, ED_NAME, eac);

         if (!more && (IoErr() != ERROR_NO_MORE_ENTRIES)) {
            UnLock(lk);
            SWERR(o, FAILURE, SWS_Err_RemOVDir_Scan, 0, 0);
            goto Done;
         }
      
         if (eac->eac_Entries == 0) continue;

         for (ead = buff; ead; ead = ead->ed_Next) {
            AddPart(strcpy(ACBuff, FNBuff), ead->ed_Name, FMSIZE);
            DeleteFile(ACBuff);

            ACheckRI(count, 50);
         }
      }
   }

   UnLock(lk);
   rc = SWDeleteFile(FNBuff);
   
  Done:
   FV(buff);
   if (eac) FreeDosObject(DOS_EXALLCONTROL, eac);
   return rc;
}


/*****************************************************************************
 * Method: Remove Overview files                                             *
 *    Msg[1] = OV path                                                       *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_RemoveOV, ULONG *)
{
   STRPTR Path = (STRPTR)msg[1];
   UBYTE  FNBuff[FMSIZE], ACBuff[FMSIZE];
   ULONG  rc = SUCCESS;

   StartBusy(idata);

   if (SW_STRISNULL(Path)) goto Done;

   // -- remove the overview file --------------------------------------------

   AddPart(strcpy(FNBuff, Path), SWV_DefOVFile, FMSIZE);
   if (SWDeleteFile(FNBuff) == FAILURE)
      rc = FAILURE;

   // -- remove the overview data file ---------------------------------------

   AddPart(strcpy(FNBuff, Path), SWV_DefOVData, FMSIZE);
   if (SWDeleteFile(FNBuff) == FAILURE)
      rc = FAILURE;

   // -- remove cached articles ----------------------------------------------

   AddPart(strcpy(FNBuff, Path), SWV_NArticle_CacheDir, FMSIZE);

   if (RmDir(o, idata, FNBuff, ACBuff) == FAILURE) {
      rc = FAILURE;
      goto Done;
   }
   
   // -- remove FAQ files ----------------------------------------------------

   AddPart(strcpy(FNBuff, Path), SWV_NGroup_FAQDir, FMSIZE);
      
   if (RmDir(o, idata, FNBuff, ACBuff) == FAILURE) {
      rc = FAILURE;
      goto Done;
   }

   // -- lastly, delete the dir itself ---------------------------------------

   if (!SWDeleteFile(Path))
      rc = FAILURE;

   if (rc = FAILURE)
      SWERR(o, NULL, SWS_Err_RemOVDir_Rem, Path, 0);

 Done:
   EndBusy(idata);

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Return count of matching articles (except expired ones)           *
 *****************************************************************************/

// msg[1]   = ptr to ULONG for bytesize, or NULL
// msg[>=2] = filter info (see StartEndMatch)

METHOD(SWM_NArticleMgr_Match, ULONG *)
{
   ULONG  x, count, Found = 0;
   ULONG *TotSize = (ULONG *)msg[1];
   
   if (!AArray()) SWDB_RET(NULL);

   if (TotSize) *TotSize = 0;

   ALoop(idata, x, msg+2, count, 500, ASize(), AArray(), FALSE)
      if (!DoMethod(Art(x), SWM_NArticle_FlagTest, SWF_NArticle_Expired)) {
         Found++;
         if (TotSize)
            *TotSize += DoMethod(Art(x), SWM_NArticle_ByteSize, TAG_DONE);
      }

   SWDB_RET(Found);
} SDFT

   
/*****************************************************************************
 * Method: Clean cache for one group                                         *
 *    msg[1] = group name                                                    *
 *    msg[2] = cache dir                                                     *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_CleanCache, ULONG *)
{
   STRPTR GrpName = (STRPTR)msg[1];
   struct ExAllData *buff  = (void *)AV(CCBuff);
   BPTR   dlock = NULL;
   struct ExAllControl *eac = AllocDosObject(DOS_EXALLCONTROL, TAG_DONE);
   ULONG  more, count, tot, ID, x;
   struct ExAllData *ead;
   UBYTE  fspec[FMSIZE];
   ULONG  rc = FAILURE;
   static UBYTE baddir[FMSIZE];
   Object *art = NN_ArtTpl(idata);
   STRPTR CDir = (STRPTR)msg[2];

   if (!GrpName) goto Done;

   if (!art) {
      SWERR(o, FAILURE, SWS_Err_CleanCache_Art, 0, 0);
      goto Done;
   }

   if (!buff || !eac) {
      SWERR(o, FAILURE, SWS_Err_Alloc, 0, 0);
      goto Done;
   }

   AddPart(strcpy(fspec, CDir), SWV_NArticle_CacheDir, FMSIZE);

   if (!(dlock = Lock(strcpy(baddir, fspec), ACCESS_READ))) {
//    SWERR(o, FAILURE, SWS_Err_CleanCache_Lock, baddir, 0);
      goto Done;
   }

   StartBusy(idata);

   ALoop(idata, x, msg+3, count, 500, ASize(), AArray(), FALSE)
      DoMethod(Art(x), SWM_NArticle_FlagReset,
               SWF_NArticle_CachedHdBd | SWF_NArticle_Permanent);

   set(o, SWA_NArticleMgr_OVPct, 0);

   for (count=tot=0,more=1,eac->eac_LastKey=0; more; ) {
      more = ExAll(dlock, buff, CCBuff, ED_COMMENT, eac);

      if (!more && (IoErr() != ERROR_NO_MORE_ENTRIES)) {
         SWERR(o, FAILURE, SWS_Err_CleanCache_Scan, baddir, 0);
         goto Done;
      }

      if (eac->eac_Entries == 0) continue;
      
      for (ead = buff; ead; ead = ead->ed_Next) {
         LONG pos;
         
         ACheckR(count, 25);
         count++, tot++;

         if (!(tot % 5))
            set(o, SWA_NArticleMgr_OVPct, tot*100 / max(eac->eac_Entries,1));

         ID = DoMethod(art, SWM_NArticle_ParseComment, ead->ed_Comment);

         AddPart(strcpy(fspec, CDir), SWV_NArticle_CacheDir, FMSIZE);
         AddPart(fspec, ead->ed_Name, FMSIZE);

         // -- if we have a record, update its cache state.  if not,
         // -- delete the cache file.

         if ((pos = FindArtPos(idata, NULL, NULL, ID)) >= 0) {
            if (DoMethod(Art(pos), SWM_NArticle_GetCache,
                         SWT_NArticle_MsgID,      FindMsgID(Art(pos)),
                         SWT_NArticle_Parts,      SWF_NArticle_Head,
                         SWT_NArticle_CacheDir,   CDir,
                         SWT_NArticle_CacheForce, TRUE,
                         TAG_DONE) == SUCCESS)
               DoMethod(Art(pos),SWM_NArticle_FlagSet,SWF_NArticle_CachedHdBd);
            else
               DoMethod(Art(pos), SWM_NArticle_UnCache,
                        SWT_NArticle_CacheDir,    CDir,
                        TAG_DONE);
         } else
            DeleteFile(fspec);
      }
   }

   rc = SUCCESS;

  Done:
   EndBusy(idata);
   FV(buff);
   if (dlock) UnLock(dlock);
   if (eac)   FreeDosObject(DOS_EXALLCONTROL, eac);
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: Clear read list                                                 *
 *****************************************************************************/

static void ClearReadList(SW_IDATA *idata)
{
   idata->ReadListFirst = 
   idata->ReadListLast  = 0;
}


/*****************************************************************************
 * Method: Clear all articles                                                *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_Clear, Msg)
{
   FreeArticleArray(idata);
   SWDB_RET(0);
}


/*****************************************************************************
 * Method: get group name given cache dir                                    *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_CDGroupName, SWP_NArticleMgr_CDGroupName *)
{
   if (SW_STRISNULL(msg->CDir)) SWDB_RET(NULL);
   
   LoadOVData(msg->CDir, msg->GroupName);

   SWDB_RET(SW_STRISNULL(msg->GroupName) ? NULL : msg->GroupName);
} SDFT


/*****************************************************************************
 * Read list support fns                                                     *
 *****************************************************************************/

static __inline LONG RLNext(LONG x)
{
   return ++x >= SWV_NArticleMgr_ReadListSize ? 0 : x;
}

static __inline LONG RLPrev(LONG x)
{
   return --x < 0 ? SWV_NArticleMgr_ReadListSize-1 : x;
}

static ULONG *AllocReadList(SW_IDATA *idata)
{
   if (idata->ReadList) return idata->ReadList;

   return idata->ReadList = AllocVec(SWV_NArticleMgr_ReadListBytes,
                                     MEMF_ANY|MEMF_CLEAR);
}

static void BuildReadSet(SW_IDATA *idata)
{
   ULONG x, pos;
   
   if (!idata->ReadList) return;

   if (!idata->ReadSet)
      if (!(idata->ReadSet = AllocVec(sizeof(rc_set), MEMF_ANY)))
         return;

   memset(idata->ReadSet, 0, sizeof(rc_set));

   for (x=idata->ReadListFirst; x != idata->ReadListLast; x = RLNext(x))
      if (idata->ReadList[x]) {
         pos = idata->ReadList[x] % RC_SETSIZE;
         FD_SET(pos, idata->ReadSet);
      }
}


/*****************************************************************************
 * Method: Add article to read list                                          *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ReadListAdd, SWP_NArticleMgr_ReadListAdd *)
{
   if (DoMethod(msg->Art, SWM_NArticle_FlagTest, SWF_NArticle_Read))
      SWDB_RET(SUCCESS);

   DoMethod(msg->Art, SWM_NArticle_FlagSet, SWF_NArticle_Read);
   
   // -- don`t worry about non-crossposted articles --------------------------

   if (DoMethod(msg->Art, SWM_NArticle_GroupCount) <= 1)
      SWDB_RET(SUCCESS);

   // -- add article to the read list ----------------------------------------

   if (AllocReadList(idata)) {
      idata->ReadList[idata->ReadListLast] =
         DoMethod(msg->Art, SWM_NArticle_GetIDHash);

      idata->ReadListLast = RLNext(idata->ReadListLast);
      if (idata->ReadListLast == idata->ReadListFirst)
         idata->ReadListFirst = RLNext(idata->ReadListFirst);
   }

   SWDB_RET(SUCCESS);

} SDFT


/*****************************************************************************
 * Method: Remove article from read list                                     *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ReadListRem, SWP_NArticleMgr_ReadListRem *)
{
   LONG  x;
   ULONG Hash;

   if (!DoMethod(msg->Art, SWM_NArticle_FlagTest, SWF_NArticle_Read))
      SWDB_RET(SUCCESS);

   DoMethod(msg->Art, SWM_NArticle_FlagReset, SWF_NArticle_Read);
   
   // -- don`t worry about non-crossposted articles --------------------------
   
   if (DoMethod(msg->Art, SWM_NArticle_GroupCount) <= 1)
      SWDB_RET(SUCCESS);

   if (!idata->ReadList) SWDB_RET(SUCCESS);

   // -- remove article from read list ---------------------------------------

   Hash = DoMethod(msg->Art, SWM_NArticle_GetIDHash);

   for (x=idata->ReadListFirst; x != idata->ReadListLast; x = RLNext(x))
      if (idata->ReadList[x] == Hash) {
         idata->ReadList[x] = 0;
         break;
      }

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Remove blank entries                                              *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ReadListRemBl, Msg)
{
   LONG x, y;
   
   if (!idata->ReadList) SWDB_RET(SUCCESS);

   // -- compress empty ones out ---------------------------------------------

   for (x=y=idata->ReadListFirst; x != idata->ReadListLast; x = RLNext(x))
      if (idata->ReadList[x]) {
         idata->ReadList[y] = idata->ReadList[x];
         y = RLNext(y);
      }

   idata->ReadListLast = y;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Save read list                                                    *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ReadListSave, SWP_NArticleMgr_ReadListSave *)
{
   BPTR fp;
   TEXT FSpec[FMSIZE];

   if (!idata->ReadList) SWDB_RET(FAILURE);

   AddPart(strcpy(FSpec, msg->CacheDir), SWV_RL_FSpec, FMSIZE);

   // -- read list is not required, so no error if we can`t save it ----------
   
   if (!(fp = Open(FSpec, MODE_NEWFILE)))
      SWDB_RET(FAILURE);

   if (Write(fp, &idata->ReadListFirst, sizeof(LONG)) != sizeof(LONG) ||
       Write(fp, &idata->ReadListLast,  sizeof(LONG)) != sizeof(LONG))
      goto Done;

   Write(fp, idata->ReadList, SWV_NArticleMgr_ReadListBytes);
   
  Done:
   Close(fp);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Save read list                                                    *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ReadListLoad, SWP_NArticleMgr_ReadListLoad *)
{
   BPTR fp;
   TEXT FSpec[FMSIZE];

   if (!AllocReadList(idata))
      SWDB_RET(FAILURE);

   AddPart(strcpy(FSpec, msg->CacheDir), SWV_RL_FSpec, FMSIZE);

   // -- read list is not required, so no error if we can`t load it ----------

   if (!(fp = Open(FSpec, MODE_OLDFILE)))
      SWDB_RET(FAILURE);

   if (Read(fp, &idata->ReadListFirst, sizeof(LONG)) != sizeof(LONG) ||
       Read(fp, &idata->ReadListLast,  sizeof(LONG)) != sizeof(LONG)) {
      ClearReadList(idata);
      goto Done;
   }

   if (Read(fp, idata->ReadList, SWV_NArticleMgr_ReadListBytes) !=
       SWV_NArticleMgr_ReadListBytes)
      ClearReadList(idata);

  Done:
   Close(fp);

   SWDB_RET(SUCCESS);
} SDFT



/*****************************************************************************
 * Method: Update group read articles from the read list cache               *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_ReadListUpd, SWP_NArticleMgr_ReadListUpd *)
{
   LONG x, y;
   ULONG pos, Hash;
   
   if (!idata->ReadList) SWDB_RET(FAILURE);
   if (!msg->ArtArray || !msg->ArtCount) SWDB_RET(SUCCESS);

   BuildReadSet(idata);

   for (y=0; y<msg->ArtCount; y++) {
      if (DoMethod(msg->ArtArray[y], SWM_NArticle_FlagTest, SWF_NArticle_Read) ||
          DoMethod(msg->ArtArray[y], SWM_NArticle_GroupCount) <= 1)
         continue;

      Hash = DoMethod(msg->ArtArray[y], SWM_NArticle_GetIDHash);
      pos  = Hash % RC_SETSIZE;

      if (!FD_ISSET(pos, idata->ReadSet))
         continue;
      
      for (x=idata->ReadListFirst; x != idata->ReadListLast; x = RLNext(x))
         if (idata->ReadList[x] == Hash) {
            DoMethod(msg->ArtArray[y], SWM_NArticle_FlagSet, SWF_NArticle_Read);
            break;
         }
   }

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Find article from tag info                                      *
 *****************************************************************************/

static Object *ArtFromTag(SW_IDATA *idata, TagItem *ti)
{
   STRPTR ID   = GetTagStr(SWT_NArticleMgr_MsgID,   NULL, ti);
   Object *Art = GetTagObj(SWT_NArticleMgr_Article, NULL, ti);
   LONG Pos;

   if (Art) return Art;
      
   if (SW_STRISNULL(ID)) return NULL;

   if ((Pos = FindArtPos(idata, NULL, ID, 0)) < 0)
      return NULL;

   return Art(Pos);
}


/*****************************************************************************
 * Method: Find other components of multipart document                       *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_MultiPart_Find, ULONG *)
{
   LONG x, Part, MaxPart = 0, count;
   Object *Art      = ArtFromTag(idata, (TagItem *)(msg+1));
   ULONG  ArtCount;
   Object **ArtArray;
   MPInfo *MP;

   GetArticleArray(idata, msg+1, &ArtCount, &ArtArray, TRUE);
   
   // -- find article & subject ----------------------------------------------
   
   if (!Art)
      SWDB_RET(SWERR(o, NULL, SWS_Err_NoMsgID, 0, 0));

   // -- get/init part info --------------------------------------------------

   if (!DoMethod(Art, SWM_NArticle_ParseMPInfo, &Part, &MaxPart, NULL, NULL))
      SWDB_RET(SWERR(o, NULL, SWS_Err_MP_BadSubj, 0, 0));
      
   if (!(MP = (MPInfo *)AV(sizeof(MPInfo) * (MaxPart + 1))))
      SWDB_RET(SWERR(o, NULL, SWS_Err_Alloc, 0, 0));
   
   for (x=0; x<=MaxPart; x++)
      MP[x].MaxPart = MaxPart;
   
   // -- search for other similar subjects -----------------------------------

   ALoopAll(idata, x, count, 1000, ArtCount, ArtArray) {
      STRPTR ID;
      LONG n, m;
      
      if (!DoMethod(ArtArray[x], SWM_NArticle_ParseMPInfo, &n, &m, NULL, Art))
         continue;

      if (n > MaxPart || m > MaxPart) continue;
      
      // -- save it`s message-ID ---------------------------------------------
      
      ID = FindMsgID(ArtArray[x]);
      if (SW_STRISNULL(ID)) continue;

      // -- if we already have one, pick one with the most lines -------------
      
      if (MP[n].MsgID) {
         LONG Lines0  = DoMethod(ArtArray[x], SWM_NArticle_GetLines),
              Lines1  = DoMethod(ArtArray[MP[n].ArrayPos], SWM_NArticle_GetLines);

         if (Lines0 > Lines1) {
            FV(MP[n].MsgID);
            MP[n].MsgID    = SDup(ID),
            MP[n].Art      = ArtArray[x],
            MP[n].ArrayPos = x;
         }
      } else
         MP[n].MsgID    = SDup(ID),
         MP[n].Art      = ArtArray[x],
         MP[n].ArrayPos = x;
   }

   SWDB_RET(MP);
} SDFT


/*****************************************************************************
 * Method: Free a MPInfo structure                                           *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_MultiPart_Free, SWP_NArticleMgr_MultiPart_Free *)
{
   LONG x;

   if (!msg->MP) SWDB_RET(SUCCESS);
   
   for (x=0; x<=msg->MP[0].MaxPart; x++)
      FV(msg->MP[x].MsgID);

   FV(msg->MP);

   SWDB_RET(SUCCESS);   
} SDFT


/*****************************************************************************
 * Method: See if we have all parts of a multipart sequence                  *
 *         (return # of missing parts)                                       *
 *****************************************************************************/

METHOD(SWM_NArticleMgr_MultiPart_Check, SWP_NArticleMgr_MultiPart_Check *)
{
   LONG x, Pos = 0, Missing=0;

   if (!msg->MP) SWDB_RET(LONG_MAX);

   // -- look for missing articles -------------------------------------------

   for (x=1; x<=msg->MP[0].MaxPart; x++)
      if (SW_STRISNULL(msg->MP[x].MsgID) || !msg->MP[x].Art ||
          DoMethod(msg->MP[x].Art, SWM_NArticle_FlagTest,
                   SWF_NArticle_NotFound)) {
         Missing++;
         if (Pos < (msg->MArrayMax-1) && msg->Missing)
            msg->Missing[Pos++] = x;
      }

   if (Pos < msg->MArrayMax && msg->Missing)
      msg->Missing[Pos] = -1;

   SWDB_RET(Missing);
} SDFT
