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
      STRPTR v0 = SWGetStr(o, SWA_NHdrList_Hdr),
             v1 = SWGetStr(o, SWA_NHdrList_Val),
             v2 = SWGetStr(o, SWA_NHdrList_Grp);

      strcpy(idata->ModeBuff, MUIX_I);
      strcat(idata->ModeBuff,
             GetS(SWGetUL(o,  SWA_NHdrList_Mode) == SWV_NHdrList_Any ?
                  SWS_L_ModeAny : SWS_L_ModeAll));

      *array++ = SW_STRDEF(v0, GetS(SWS_L_UnsetHdr)),
      *array++ = SW_STRDEF(v1, GetS(SWS_L_UnsetVal)),
      *array++ = SW_STRDEF(v2, GetS(SWS_L_UnsetGrp)),
      *array   = idata->ModeBuff;
   } else {
      *array++ = GetS(SWS_L_Hdr),
      *array++ = GetS(SWS_L_Val),
      *array++ = GetS(SWS_L_Grp),
      *array   = GetS(SWS_L_Mode);
   }
   
   return 0;
}
