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
      STRPTR np,
             v1 = SWGetStr(o, SWA_NTCP_Host),
             v2 = SWGetStr(o, SWA_NTCP_Title),
             v3 = SWGetStr(o, SWA_NTCP_Status);
      ULONG bps = SWGetUL(o, SWA_NTCP_BPS);

      strncpy(idata->node, v1, 47);
      idata->node[47] = '\0';
      if (np = strchr(idata->node, '.'))
         *np = '\0';
      
      sprintf(idata->v4, "%ld.%1ld Kb/s", bps/1024, ((bps%1024)*10)/1024);
      SWFmtSize(SWGetUL(o, SWA_NTCP_RecvBytes), idata->v5, 1);
      sprintf(idata->v6, "%ld", SWGetUL(o, SWA_NTCP_RecvLines));

      *array++ = defa(v1, SWS_L_None),
      *array++ = defa(v2, SWS_L_Untitled),
      *array++ = SW_STRDEF(v3, ""),
      *array++ = idata->v4,
      *array++ = idata->v5,
      *array++ = idata->v6,
      *array   = idata->node;
   } else {
      ULONG x;

      for (x=0; ColSpecs[x]; x++)
         *array++ = GetS(SWS_L_Node + x);
   }
   
   return 0;
}
