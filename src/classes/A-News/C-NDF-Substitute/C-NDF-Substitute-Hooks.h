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
      STRPTR pat = SWGetStr(o, SWA_NDF_Substitute_Pattern),
             rep = SWGetStr(o, SWA_NDF_Substitute_Replace),
             img = SWGetStr(o, SWA_NDF_Substitute_Image),
             msk = SWGetStr(o, SWA_NDF_Substitute_Mask);

      *array++ = bval(o, SWA_NDF_Substitute_Case),
      *array++ = bval(o, SWA_NDF_Substitute_Wild),
      *array++ = bval(o, SWA_NDF_Substitute_Longest),
      *array++ = SW_STRDEF(pat, GetS(SWS_L_Unset)),
      *array++ = SW_STRDEF(rep, GetS(SWS_L_Unset)),
      *array++ = FilePart(SW_STRDEF(img, GetS(SWS_L_Unset))),
      *array   = FilePart(SW_STRDEF(msk, GetS(SWS_L_Unset)));
   } else {
      ULONG x;

      for (x=0; x<7; x++)
         *array++ = GetS(SWS_L_Case + x);
   }
   
   return 0;
}
