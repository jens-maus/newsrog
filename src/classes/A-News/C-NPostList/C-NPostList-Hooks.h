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
      STRPTR v0 = SWGetStr(o, SWA_NPost_Subj),
             v1 = SWGetStr(o, SWA_NPost_Grp);
      ULONG  li = SWGetUL(o, SWA_NPost_Lines),
             si = SWGetUL(o, SWA_NPost_Bytes);

      if (si) {
         sprintf(idata->v0, "%ld", li);
         SWFmtSize(si, idata->v1, 2);
      }

      *array++ = si == 0 ? GetS(SWS_L_Empty) : idata->v0,
      *array++ = si == 0 ? GetS(SWS_L_Empty) : idata->v1,
      *array++ = defa(v0, SWS_L_Unset),
      *array   = defa(v1, SWS_L_Unset);
   } else {
      ULONG x;

      for (x=0; ColSpecs[x]; x++)
         *array++ = GetS(SWS_T_Lines + x);
   }
   
   return 0;
}
