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
      ULONG  ct  = SWGetUL(o, SWA_NServer_ConnectT),
             cs  = SWGetUL(o, SWA_NServer_ConnectS),
             ng  = SWGetUL(o, SWA_NServer_NGroups),
             On  = SWGetUL(o, SWA_NServer_Default),
             POk = SWGetUL(o, SWA_NServer_PostOK),
             typ = SWGetUL(o, SWA_NServer_Type);

      STRPTR np,
             v0 = On ? SWImg(idata, SWV_Img_True) : SWImg(idata, SWV_Img_False),
             v1 = SWGetStr(o, SWA_NServer_Host);

      sprintf(idata->v2, "%ld",   cs);
      sprintf(idata->v3, "%ld",   ct - cs);
      sprintf(idata->v4, "%ld%%", cs * 100 / max(ct, 1));
      sprintf(idata->v5, "%ld",   ng);
      sprintf(idata->v8, "%ld",   SWGetUL(o, SWA_NServer_MaxRetries));

      strncpy(idata->node, v1, 47);
      idata->node[47] = '\0';
      if (np = strchr(idata->node, '.'))
         *np = '\0';

      *array++ = v0,
      *array++ = defa(v1, SWS_L_None),
      *array++ = idata->v2,
      *array++ = idata->v3,
      *array++ = idata->v4,
      *array++ = ng ? idata->v5 : GetS(SWS_L_Unknown),
      *array++ = (STRPTR)DoMethod(s_CxTO ? s_CxTO : idata->FmtSlider,
                                  MUIM_Numeric_Stringify,
                                  SWGetUL(o, SWA_NServer_CxTimeout)),
      *array++ = (STRPTR)DoMethod(s_CxRT ? s_CxRT : idata->FmtSlider,
                                  MUIM_Numeric_Stringify,
                                  SWGetUL(o, SWA_NServer_RetryDelay)),
      *array++ = idata->v8,
      *array++ = idata->node,
      *array++ = GetS(POk ? SWS_L_Yes : SWS_L_No),
      *array   = GetS(SWS_L_TypeNNTP + typ);
   } else {
      ULONG x;

      for (x=0; ColSpecs[x]; x++)
         *array++ = GetS(SWS_L_Default + x);
   }
   
   return 0;
}
