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
      ULONG  On   = SWGetUL(o, SWA_NSFilter_Enabled),
             TNow = SWGetUL(o, SWA_NSFilter_TotNow),
             TPct = SWGetUL(o, SWA_NSFilter_FoundNow) * 1000 / max(TNow, 1);
      STRPTR v1   = On ? SWImg(idata,SWV_Img_True):SWImg(idata, SWV_Img_False),
             v2   = SWGetUL (o, SWA_NSFilter_Inv) ? SWImg(idata,SWV_Img_True) :
                                                    SWImg(idata,SWV_Img_False),
             v3   = SWGetStr(o, SWA_NSFilter_Desc),
             v4   = SWGetStr(o, SWA_ObjClassName),
             v7   = SWGetStr(o, SWA_NSFilter_Groups);

      strcpy(idata->DescBuff, On ? MUIX_B : "");
      strncat(idata->DescBuff, defa(v3, SWS_L_Unset), 255);
      idata->DescBuff[255] = '\0';

      sprintf(idata->matchbuff, "%ld",        TNow);
      sprintf(idata->pctbuff,   "%ld.%1ld%%", TPct/10, TPct%10);

      *array++ = v1,
      *array++ = v2,
      *array++ = idata->DescBuff,
      *array++ = !SW_STRISNULL(v4) ? v4+4 : GetS(SWS_L_None),
      *array++ = idata->matchbuff,
      *array++ = idata->pctbuff,
      *array   = SW_STRDEF(v7, GetS(SWS_L_AllGrps));
   } else {
      ULONG x;

      for (x=SWS_T_Active; x<=SWS_T_Groups; x++)
         *array++ = GetS(x);
   }
   
   return 0;
}
