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
      struct DateTime dt;
      STRPTR gn = idata->abbrev,
             v2 = SWGetStr(o, SWA_NGroup_Server1),
             ds = SWGetStr(o, SWA_NGroup_Desc),
             tp = idata->time;
      ULONG aac = SWGetUL(o, SWA_NGroup_AvailArtNum),
            nac = SWGetUL(o, SWA_NGroup_NewArtNum),
            tac = SWGetUL(o, SWA_NGroup_TotArtCnt),
            rac = SWGetUL(o, SWA_NGroup_ReadArtCnt),
            flg = SWGetUL(o, SWA_NGroup_PermFlags);

      rac = min(rac, tac);

      sprintf(idata->v3, "%ld", min(nac, aac));
      sprintf(idata->v4, "%ld", aac);
      sprintf(idata->v5, "%ld", tac);
      sprintf(idata->v6, "%ld", tac - rac);
      sprintf(idata->v9, "%ld", rac);

      if (!idata->SWA_NGroupList_FullNames) {
         DoMethod(o, SWM_NGroup_Abbrev, idata->abbrev, SWV_NGroupList_AbbrevLen);
      } else gn = SWGetStr(o, SWA_NGroup_Name);

      dt.dat_Stamp.ds_Days   = SWGetUL(o, SWA_NGroup_RdDays);
      dt.dat_Stamp.ds_Minute = SWGetUL(o, SWA_NGroup_RdMin);
      dt.dat_Stamp.ds_Tick   = 0;
      dt.dat_Format          = FORMAT_DOS;
      dt.dat_Flags           = DTF_SUBST;
      dt.dat_StrDay          = NULL;
      dt.dat_StrDate         = idata->date;
      dt.dat_StrTime         = idata->time;

      if (dt.dat_Stamp.ds_Days) {
         DateToStr(&dt);
         tp += strcspn(tp, ":")+1;
         tp[strcspn(tp, ":")] = '\0';

         tp = idata->date + strlen(idata->date) - 1;
         
         while (tp >= idata->date && *tp == ' ')
            *tp-- = '\0';
      } else {
         idata->time[0] = '\0';
         strcpy(idata->date, GetS(SWS_L_Never));
      }

      *array++ = SWGetUL(o,SWA_NGroup_BadGroup)   ? SWImg(idata,SWV_Img_Broken):
                 SWGetUL(o,SWA_NGroup_ReaderOpen) ? SWImg(idata,SWV_Img_WinOpen):
                 FIsSet(flg, SWF_NGroup_NewBatch) ? SWImg(idata,SWV_Img_NewBatch):
                 SWImg(idata,SWV_Img_WinClose);

      *array++ = defa(gn, SWS_L_Unnamed),
      *array++ = defa(v2, SWS_L_Default),
      *array++ = idata->v3,
      *array++ = idata->v4,
      *array++ = idata->v5,
      *array++ = idata->v6,
      *array++ = idata->date,
      *array++ = idata->time,
      *array++ = idata->v9,
      *array++ = defa(ds, SWS_L_Unset),
      *array   = GetS(SWGetUL(o,SWA_NGroup_Moderated) ? SWS_L_ModY : SWS_L_ModN);
   } else {
      *array++ = "",
      *array++ = GetS(SWS_L_Name),
      *array++ = GetS(SWS_L_Server),
      *array++ = GetS(SWS_L_NewCount),
      *array++ = GetS(SWS_L_AvailCount),
      *array++ = GetS(SWS_L_TotArtCnt),
      *array++ = GetS(SWS_L_UnReadArtCnt),
      *array++ = GetS(SWS_L_LastRead1),
      *array++ = GetS(SWS_L_LastRead2),
      *array++ = GetS(SWS_L_ReadArtCnt);
      *array++ = GetS(SWS_L_Description);
      *array   = GetS(SWS_L_Moderated);
   }
   
   return 0;
}


