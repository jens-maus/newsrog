/*****************************************************************************
 * Display hook for list                                                     *
 *****************************************************************************/

static __saveds __asm LONG DspFn(a2 char **array, a1 Object *o,
                                 a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

#  define bval(o,a) SWImg(idata, SWGetUL(o, a) ? SWV_Img_True : SWV_Img_False)
   
   if (o) {
      STRPTR s, pat = SWGetStr(o, SWA_NStrFilter_Pattern);

      strcpy(idata->Hdr, Headers[SWGetUL(o, SWA_NStrFilter_Header)]);
      if (s = strchr(idata->Hdr, ':')) *s = '\0';
      
      *array++ = bval(o, SWA_NStrFilter_Case),
      *array++ = bval(o, SWA_NStrFilter_Wild),
      *array++ = bval(o, SWA_NStrFilter_Anchor),
      *array++ = idata->Hdr,
      *array   = SW_STRDEF(pat, GetS(SWS_L_Unset));
   } else {
      ULONG x;

      for (x=0; x<5; x++)
         *array++ = GetS(SWS_L_Case + x);
   }
   
   return 0;
}
