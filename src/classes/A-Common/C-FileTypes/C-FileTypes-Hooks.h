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
      STRPTR v0 = SWGetStr(o, SWA_FileTypes_FT),
             v1 = SWGetStr(o, SWA_FileTypes_Patt);

      *array++ = SW_STRDEF(v0, GetS(SWS_L_UnsetType)),
      *array++ = GetS(SWS_FT_0 + SWGetUL(o, SWA_FileTypes_Method)),
      *array   = SW_STRDEF(v1, GetS(SWS_L_UnsetPatt));
   } else {
      *array++ = GetS(SWS_L_FType),
      *array++ = GetS(SWS_L_FMeth),
      *array   = GetS(SWS_L_FPatt);
   }
   
   return 0;
}
