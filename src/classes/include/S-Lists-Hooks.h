/*****************************************************************************
 * Construct hook for list                                                   *
 *****************************************************************************/

# if defined SW_NList_CstHook
static __saveds __asm Object *CstFn(a1 Object *o, a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   SW_NList_CstHook;
}
# endif


/*****************************************************************************
 * Destruct hook for list                                                    *
 *****************************************************************************/

static void Kill1N(SW_IDATA *, Object *);

static __saveds __asm VOID DstFn(a1 Object *o, a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

#  ifdef SW_NListTrackAttrNum
    if (o == idata->SWA_NListLastTLA)
       DoMethod(idata->SWA_NList_Obj, SWM_NList_Change, MUIV_List_Active_Off);
#  endif

#  ifdef SWV_NList_TrackedMax
      if (FIsSet(idata->NLFlags, SWF_NList_DelRecurFlag))
         return;
#  endif

   Kill1N(idata, o);

#  ifdef SWV_NList_TrackedMax
   {
      ULONG x;
      
      FSet(idata->NLFlags, SWF_NList_DelRecurFlag);

      if (FIsSet(idata->NLFlags,  SWF_NList_MasterFlag)) {
         for (x=0; x<SWV_NList_TrackedMax; x++)
            if (idata->SWA_NList_Tracked[x]) {
               DoMethod(idata->SWA_NList_Tracked[x], SWM_NList_RemoveObj, o);
               DoMethod(idata->SWA_NList_Tracked[x], SWM_NList_SetNotify);
            }
#        ifdef SW_NList_DispHook
            SW_NList_DispHook;
#        endif
      } else {
         if (idata->SWA_NList_Master) {
            DoMethod(idata->SWA_NList_Master, SWM_NList_RemoveObj, o);
            for (x=0; x<SWV_NList_TrackedMax; x++)
               if (idata->SWA_NList_Tracked[x])
                  DoMethod(idata->SWA_NList_Tracked[x], SWM_NList_SetNotify);
         }
      }

      FReset(idata->NLFlags, SWF_NList_DelRecurFlag);
   }
#  else
#     ifdef SW_NList_DispHook
         SW_NList_DispHook;
#     endif
#  endif
}


/*****************************************************************************
 * Sort hook for list                                                        *
 *****************************************************************************/

# if defined SW_NList_CmpAttr || defined SW_NList_CmpCode
static __saveds __asm LONG CmpFn(a1 Object *o1, a2 Object *o2,
                                 a0 struct Hook *hook)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

#  ifdef SW_NList_CmpAttr
      return strcmp(SWGetStr(o1, SW_NList_CmpAttr),
                    SWGetStr(o2, SW_NList_CmpAttr));
#  endif

#  ifdef SW_NList_CmpCode
      SW_NList_CmpCode;
#  endif
}
# endif


/*****************************************************************************
 * Set up hook data and entries                                              *
 *****************************************************************************/

static void NListSetupHooks(SW_IDATA *idata)
{
#  if defined SW_NList_CmpAttr || defined SW_NList_CmpCode
     idata->CmpHook.h_Entry = (ULONG (*)())CmpFn;
     idata->CmpHook.h_Data  = (void *)idata;
#  endif

#  ifdef SW_NList_CstHook
     idata->CstHook.h_Entry = (ULONG (*)())CstFn;
     idata->CstHook.h_Data  = (void *)idata;
#  endif

   idata->DspHook.h_Entry = (ULONG (*)())DspFn;
   idata->DspHook.h_Data  = (void *)idata;

   idata->DstHook.h_Entry = (ULONG (*)())DstFn;
   idata->DstHook.h_Data  = (void *)idata;

   idata->A4              = getreg(REG_A4);
}
