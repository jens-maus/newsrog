# define  SWFINLINE __inline
# include "S-FastItoA.h"

/*****************************************************************************
 * Display hook for list                                                     *
 *****************************************************************************/

static __saveds __asm LONG DspFn(a2 char **array, a1 Object *o,
                                 a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

#  define defa(v,d) SW_STRDEF(v, GetS(d))

   if (o) {
      struct SWS_NArticle_OVInfo OVInfo;
      ULONG  f;
      STRPTR tsubj, v3 = idata->SubjBuff;

      OVInfo.SNameBuff = idata->NameBuff;
      OVInfo.SNBuffLen = SWV_NArticleList_NameBuffLen;

      DoMethod(o, SWM_NArticle_OVInfo, &OVInfo);
      f = OVInfo.Flags;
      
      idata->FBuff[0] = '0',  idata->FBuff[1] = '\0';

      // -- figure out the # of spaces to indent --------------------------------
      if (idata->SWA_NArticleList_Threaded && idata->Threaded) {
         ULONG rc, is;
         
         rc = DoMethod(idata->SWA_NArticleList_ThreadMgr,
                       SWM_NArticleMgr_ThreadRefs, o, OVInfo.Hash);
         is  = min(rc, SWV_NArticleList_MaxIndent) * SWV_NArticleList_IndentPer;
         memset(v3, ' ', is); v3 += is;
      }

      FastItoA(idata->FBuff,   OVInfo.Followups,  FALSE);
      FastItoA(idata->GrpBuff, OVInfo.GroupCount, FALSE);

      if (FIsSet(f, SWF_NArticle_Read)) {
         strcpy(v3, MUIX_I);
         v3 += sizeof(MUIX_I)-1;
      }

      if (FIsSet(f, SWF_NArticle_UserMk)) {
         strcpy(v3, MUIX_B);
         v3 += sizeof(MUIX_B)-1;
      }

      if (FIsSet(f, SWF_NArticle_Highlight)) {
         strcpy(v3, MUIX_PH);
         v3 += sizeof(MUIX_PH)-1;
      }

      tsubj = OVInfo.Subject;
      if (tsubj) tsubj = SWV_NArticle_SubjNoRe(tsubj);

      strncpy(v3, defa(tsubj, SWS_L_Unknown), SWV_NArticleList_MaxSubLen);

      v3[SWV_NArticleList_MaxSubLen] = '\0';

      if (OVInfo.Bytes == 0)
         strcpy(idata->SizeBuff, GetS(SWS_L_QMark));
      else
         SWFmtSize(OVInfo.Bytes, idata->SizeBuff, 1);

      if (OVInfo.Age) {
         struct DateTime dt;
         ULONG  AgeTmp, Len;

         // -- format posting date/time --------------------------------------
         
         FastItoA(idata->TimeBuff,   OVInfo.ds_Minute / 60, TRUE);
         idata->TimeBuff[2] = ':';
         FastItoA(idata->TimeBuff+3, OVInfo.ds_Minute % 60, TRUE);
         
         dt.dat_Stamp           = OVInfo.ds;
         dt.dat_Format          = FORMAT_DOS;
         dt.dat_Flags           = DTF_SUBST;
         dt.dat_StrDay          = NULL;
         dt.dat_StrDate         = idata->DateBuff,
         dt.dat_StrTime         = NULL;

         if (OVInfo.ds_Days == idata->Prev_ds_Days) {
            dt.dat_StrDate = NULL;
            strcpy(idata->DateBuff, idata->PrevDateBuff);
         } else {
            DateToStr(&dt);
            idata->Prev_ds_Days = OVInfo.ds.ds_Days;
            strcpy(idata->PrevDateBuff, idata->DateBuff);
         }

         // -- format article age --------------------------------------------
         
         AgeTmp = OVInfo.Age;

         FastItoA(idata->AgeBuff,  AgeTmp / SWV_NArticle_AgeTicksPerDay, FALSE);
         Len = strlen(idata->AgeBuff);

         idata->AgeBuff[Len++] = 'd';
         idata->AgeBuff[Len++] = ' ';

         AgeTmp %= SWV_NArticle_AgeTicksPerDay;
         
         FastItoA(idata->AgeBuff + Len,  AgeTmp / SWV_NArticle_AgeTicksPerHr,
                  TRUE);
         Len += 2;
         idata->AgeBuff[Len++] = ':';

         AgeTmp %= SWV_NArticle_AgeTicksPerHr;
         FastItoA(idata->AgeBuff + Len,  AgeTmp / SWV_NArticle_AgeTicksPerMin,
                  TRUE);
      } else {
         strcpy(idata->DateBuff, GetS(SWS_L_Unknown));
         strcpy(idata->AgeBuff,  GetS(SWS_L_Unknown));
         idata->TimeBuff[0] = '\0';
      }

      *array++ =
         FIsSet(f, SWF_NArticle_Permanent) ? SWImg(idata,SWV_NAL_Img_Disk)   :
         FIsSet(f, SWF_NArticle_CachedBody)? SWImg(idata,SWV_NAL_Img_Drawer) :
         FIsSet(f, SWF_NArticle_NotFound)  ? SWImg(idata,SWV_NAL_Img_Broken) :
         FIsSet(f, SWF_NArticle_QueuedDL)  ? SWImg(idata,SWV_NAL_Img_Net)    :
         FIsSet(f, SWF_NArticle_NewArt)    ? SWImg(idata,SWV_NAL_Img_NewArt) :
            "  ";
      *array++ = defa(OVInfo.Lines, SWS_L_Unknown),
      *array++ = (OVInfo.GroupCount == 0 ? GetS(SWS_L_Unknown):idata->GrpBuff),
      *array++ = defa(idata->NameBuff, SWS_L_Unknown),
      *array++ = idata->DateBuff,
      *array++ = idata->TimeBuff,
      *array++ = idata->FBuff,
      *array++ = idata->SubjBuff;
      *array++ = idata->SizeBuff;
      *array   = idata->AgeBuff;
   } else {
      ULONG x;

      for (x=0; x<=(SWS_L_LongTitleLast - SWS_L_LongTitle0); x++) 
         *array++ = GetS(SWS_L_Blank + x);
   }
   
   return 0;
}
