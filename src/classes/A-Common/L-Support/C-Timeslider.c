
# include "Libraries/C-Root.h"
# include "S-MkClass.h"

struct TimesliderData { ULONG NoDiv1000; ULONG NIMult; char buf[16]; };
struct KbSliderData   { char buf[20]; ULONG NIMult; ULONG Bytes; };

SW_MkClass(Timeslider, MUIC_Slider)
SW_MkClass(KbSlider,   MUIC_Slider)

__asm ULONG TimesliderDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct TimesliderData *idata = INST_DATA(cl, o);
   
   MkClassDispatchInit(cl);

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *obj = (Object*)DoSuperMethodA(cl, o, msg);

        if (obj) {
           struct TimesliderData *idata = INST_DATA(cl, obj);
           idata->NoDiv1000 = 
              GetTagUL(SWA_TimeSlider_NoDiv1000, FALSE,
                       ((opSet *)msg)->ops_AttrList);

           idata->NIMult =
              GetTagUL(SWA_TimeSlider_NumIncMult, idata->NoDiv1000 ? 1 : 1000,
                       ((opSet *)msg)->ops_AttrList);
        }

        return (ULONG)obj;
     }

     case MUIM_Numeric_Stringify: {
        struct MUIP_Numeric_Stringify *ns = (APTR)msg;
        ULONG d = ns->value / (idata->NoDiv1000 ? 1 : 1000), h, m, s;

        s = d % 60, d /= 60;
        m = d % 60, d /= 60;
        h = d % 24, d /= 24;

        if (d == 0 && h == 0)
           sprintf(idata->buf, "%02ld:%02ld", m, s);
        else if (d == 0)
           sprintf(idata->buf, "%02ld:%02ld:%02ld", h, m, s);
        else
           sprintf(idata->buf, "%ldd %02ld:%02ld:%02ld", d, h, m, s);
        
        return((ULONG)idata->buf);
     }

     case MUIM_Numeric_Increase:
     case MUIM_Numeric_Decrease: {
        struct MUIP_Numeric_Increase *m = (APTR)msg;

        return DoSuperMethod(cl, o, msg->MethodID, m->amount * idata->NIMult);
     }
   }

   return DoSuperMethodA(cl, o, msg);
}


__saveds __asm struct IClass *SWTimesliderClass(void)
{
   return SW_GETICLASS(TimesliderClassPtr);
}


__saveds __asm struct IClass *SWKbsliderClass(void)
{
   return SW_GETICLASS(KbSliderClassPtr);
}


__asm ULONG KbSliderDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct KbSliderData *idata = INST_DATA(cl, o);

   MkClassDispatchInit(cl);

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *obj = (Object*)DoSuperMethodA(cl, o, msg);

        if (obj) {
           struct KbSliderData *idata = INST_DATA(cl, obj);
           idata->Bytes = 
              GetTagUL(SWA_KbSlider_Bytes, FALSE, ((opSet *)msg)->ops_AttrList);

           idata->NIMult =
              GetTagUL(SWA_KbSlider_NumIncMult, idata->Bytes ? 100 : 10,
                       ((opSet *)msg)->ops_AttrList);
        }

        return (ULONG)obj;
     }

     case MUIM_Numeric_Stringify: {
        struct KbSliderData *idata = INST_DATA(cl, o);
        struct MUIP_Numeric_Stringify *m = (APTR)msg;
        return (ULONG)SWFmtSize(m->value * (idata->Bytes ? 1 : 1024), idata->buf,
                                (m->value < 1000 && !idata->Bytes) ? 0 : 2);
     }

     case MUIM_Numeric_Increase:
     case MUIM_Numeric_Decrease: {
        struct MUIP_Numeric_Increase *m = (APTR)msg;
        return DoSuperMethod(cl, o, msg->MethodID, m->amount * idata->NIMult);
     }
   }

   return DoSuperMethodA(cl, o, msg);
}
