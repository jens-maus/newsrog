# include "C-Timer-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_Timer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IB(SWA_Timer_Millis,  SWAM_NoLab, SWAM_SGI,   1000, 1, 10000000),
   SWAM_IN(SWA_Timer_Min,     SWAM_NoLab, SWAM_SGI,   1000),
   SWAM_IN(SWA_Timer_Max,     SWAM_NoLab, SWAM_SGI,   1000*60*10),
   SWAM_BO(SWA_Timer_Running, SWAM_NoLab, SWAM_SGI,   TRUE),
   SWAM_PT(SWA_Timer_Target,  SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_IN(SWA_Timer_Method,  SWAM_NoLab, SWAM_SGIN,  SWM_Timer_Trigger),
   SWAM_IN(SWA_Timer_UData,   SWAM_NoLab, SWAM_SGIN,  NULL),

   SWP_Root_Attrs,
   SWAM_DONE
};

static void SetTime(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (Tag == SWA_Timer_Running) {
      if (!idata->App) return;
      
      DoMethod(o, Data ? SWM_Timer_Install : SWM_Timer_Remove,
               idata->SWA_Timer_Target, idata->App);
      return;
   }

   if (Tag == SWA_Timer_Secs)
      Tag = SWA_Timer_Millis, Data *= 1000;
   
   if (Tag == SWA_Timer_Millis) {
      idata->TNow = 0;
      
      if (Data <= SWV_Timer_MaxMillis) {
         idata->TMillis                     = 
         idata->SWA_Timer_IHNode.ihn_Millis = 
         idata->SWA_Timer_Millis            = Data;
         idata->TCount                      = 1;
      } else {
         idata->SWA_Timer_Millis            = Data;
         idata->TCount                      = (Data+500) / 1000;
         idata->TMillis                     = 
         idata->SWA_Timer_IHNode.ihn_Millis = 1000;
      }
      
      return;
   }
}

# define SW_USERSET SetTime(o, idata, Tag, Data)

# define SETUPCUST  idata->SWA_Timer_IHNode.ihn_Object  = (APTR)retval;      \
                    idata->SWA_Timer_IHNode.ihn_Method  = SWM_Timer_Trigger; \
                    idata->SWA_Timer_IHNode.ihn_Flags   = MUIIHNF_TIMER;     \
                    SetTime((Object *)retval, idata, SWA_Timer_Millis, 1000);

# define DISPOSECUST DoMethod(o, SWM_Timer_Remove);

# define SW_Methods                    \   
  SW_DO_MF(SWM_Timer_Trigger);         \
  SW_DO_MF(SWM_Timer_Install);         \
  SW_DO_MF(SWM_Timer_Remove);          \
  SW_DO_MF(SWM_Invocation_MakeEditUI); \

# include "C-Timer-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
