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
      STRPTR v0 = SWGetStr(o, SWA_VarList_Var),
             v1 = SWGetStr(o, SWA_VarList_Val);

      if (SWGetUL(o, SWA_VarList_RO))
         v0 = strcat(strcpy(idata->VarBuf, MUIX_PH), v0);
      
      *array++ = SW_STRDEF(v0, GetS(SWS_L_UnsetVar)),
      *array   = SW_STRDEF(v1, GetS(SWS_L_UnsetVal));
   } else {
      *array++ = GetS(SWS_L_Var),
      *array   = GetS(SWS_L_Val);
   }
   
   return 0;
}
