/*****************************************************************************
 * Display hook for list                                                     *
 *****************************************************************************/

static __saveds __asm LONG DspFn(a2 char **array, a1 Object *o,
                                 a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   if (o) {
      STRPTR pat = SWGetStr(o, SWA_NDF_HideHeader_Pat);

      *array   = SW_STRDEF(pat, GetS(SWS_L_Unset));
   } else {
      *array   = GetS(SWS_T_Pattern);
   }
   
   return 0;
}
