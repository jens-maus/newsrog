/*****************************************************************************
 * Display hook for list                                                     *
 *****************************************************************************/

static STRPTR FP(STRPTR s)
{
   STRPTR fp = FilePart(s);

   return SW_STRDEF(fp, s);
}

static __saveds __asm LONG DspFn(a2 char **array, a1 Object *o,
                                 a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

#  define defa(v,d) SW_STRDEF(v, GetS(d))

   if (o) {
      STRPTR v0 = SWGetStr(o, SWA_FAct_Type),
             v1 = SWGetStr(o, SWA_FAct_Action),
             v2 = SWGetStr(o, SWA_FAct_Command),
             v3 = SWGetStr(o, SWA_FAct_Args),
             v4 = SWGetStr(o, SWA_FAct_Input),
             v5 = SWGetStr(o, SWA_FAct_Output),
             v6 = SWGetStr(o, SWA_FAct_Dir);
      ULONG  n0 = SWGetUL(o,  SWA_FAct_Stack),
             n1 = SWGetUL(o,  SWA_FAct_Pri);

      if (v2) v2 = FP(v2);
      if (v4) v4 = FP(v4);
      if (v5) v5 = FP(v5);
      if (v6) v6 = FP(v6);

      sprintf(idata->Stack, "%ld", n0);
      sprintf(idata->Pri,   "%ld", n1);
      
      *array++ = SW_STRDEF(v0, GetS(SWS_L_Unset)),
      *array++ = SW_STRDEF(v1, GetS(SWS_L_Unset)),
      *array++ = SW_STRDEF(v2, GetS(SWS_L_Unset)),
      *array++ = SW_STRDEF(v3, GetS(SWS_L_Unset)),
      *array++ = SW_STRDEF(v4, GetS(SWS_L_Unset)),
      *array++ = SW_STRDEF(v5, GetS(SWS_L_Unset)),
      *array++ = idata->Stack,
      *array++ = idata->Pri,
      *array   = v6;
   } else {
      *array++ = GetS(SWS_T_Type),
      *array++ = GetS(SWS_T_Action),
      *array++ = GetS(SWS_T_Cmd),
      *array++ = GetS(SWS_T_Args),
      *array++ = GetS(SWS_T_Input),
      *array++ = GetS(SWS_T_Output),
      *array++ = GetS(SWS_T_Stack),
      *array++ = GetS(SWS_T_Pri),
      *array   = GetS(SWS_T_Dir);
   }
   
   return 0;
}
