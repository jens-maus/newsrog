/*****************************************************************************
 * Display hook for list                                                     *
 *****************************************************************************/

static __saveds __asm LONG DspFn(a2 char **array, a1 Object *o,
                                 a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   if (o) {
      *array = "boo";
   } else {
      ULONG x;

      for (x=SWS_T_Subj; x<=SWS_T_Subj; x++)
         *array++ = GetS(x);
   }
   
   return 0;
}
