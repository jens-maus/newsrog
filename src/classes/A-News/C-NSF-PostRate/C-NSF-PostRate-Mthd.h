/*****************************************************************************
 * Method: Begin match tests                                                 *
 *****************************************************************************/

METHOD(SWM_NSFilter_Begin, SWP_NSFilter_Begin *)
{
   ULONG rc = DoSuperMethodA(cl, o, msg);
      
   idata->Total = idata->Found = 0;
   idata->Inv   = SWGetUL(o, SWA_NSFilter_Inv);

   idata->TM = msg->Grp ? SWGetObj(msg->Grp, SWA_NGroup_ThreadMgr) : NULL;

   idata->PrevThread = AllocVec(SWV_NSF_PostRate_CSize * sizeof(Object **),
                                MEMF_ANY | MEMF_CLEAR);
   idata->PrevMatch  = AllocVec(SWV_NSF_PostRate_CSize * sizeof(UBYTE),
                                MEMF_ANY | MEMF_CLEAR);

   if (!idata->PrevThread || !idata->PrevMatch)
      rc = FAILURE;
   else
      memset(idata->PrevMatch, 2, SWV_NSF_PostRate_CSize * sizeof(UBYTE));

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Determine if article is a match                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_Match, SWP_NSFilter_Match *)
{
   ULONG Match = 2;
   
   idata->Total++;

   if (idata->TM) {
      Object *Base,
             **Thread = (Object **)
                  DoMethod(idata->TM, SWM_NArticleMgr_GetThreadArts,
                           msg->Art, &Base),
             **TPos = Thread;
      ULONG HPos = ((ULONG)(Thread)>>2) % SWV_NSF_PostRate_CSize;
      LONG  ArtCount, Oldest, Age, PostRate;


      // -- look for cached answer from previous threads ---------------------
      
      if (Thread && Thread == idata->PrevThread[HPos])
         if ((Match = idata->PrevMatch[HPos]) != 2) goto Done;

      // -- didn`t find one - gotta search all the thread articles -----------

      ArtCount  = 1;
      
      if (!Thread) {
         ArtCount = 1;
         Oldest   = DoMethod(Base, SWM_NArticle_Age, NULL);
      } else {
         Oldest    = 0;
         
         for (; !ThreadArtEnd(*TPos); TPos++) {
            ArtCount++;
            Age = DoMethod(*TPos, SWM_NArticle_Age, NULL);
            if (Age > Oldest)   Oldest   = Age;
         }
      }

      PostRate = ArtCount /
         max((Oldest + SWV_NArticle_AgeTicksPerDay) /
             SWV_NArticle_AgeTicksPerDay, 1);

      if (ArtCount >= idata->SWA_NSF_PostRate_MinArts   &&
          ArtCount <= idata->SWA_NSF_PostRate_MaxArts   &&
          PostRate >= idata->SWA_NSF_PostRate_MinPerDay &&
          PostRate <= idata->SWA_NSF_PostRate_MaxPerDay)
         Match = TRUE;
      else
         Match = FALSE;

      // -- cache the answer in case we find another art in this thread later- 

      idata->PrevThread[HPos] = Thread;
      idata->PrevMatch[HPos]  = (Match ? 1 : 0);
   }

  Done:
   if (idata->Inv) Match = !Match;

   if (Match) idata->Found++;
   
   SWDB_RET(Match);
} SDFT
   

/*****************************************************************************
 * Method: End match tests                                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_End, Msg)
{
   FreeVec(idata->PrevThread);
   FreeVec(idata->PrevMatch);
   idata->PrevThread = NULL;
   idata->PrevMatch  = NULL;
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Update statistics after xfer                                      *
 *****************************************************************************/

METHOD(SWM_NSFilter_TrackStats, SWP_NSFilter_TrackStats *)
{
   *msg->Total = idata->Total;
   *msg->Found = idata->Found;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NSFilter_UI, Msg)
{
   SWDB_RET(idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o));
} SDFT


/*****************************************************************************
 * Method: Insert a new object                                               *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
   if (idata->UI)
      SWKillAttrUI(SW_ATTRMAP, idata->UI, o);
   idata->UI = NULL;

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT
