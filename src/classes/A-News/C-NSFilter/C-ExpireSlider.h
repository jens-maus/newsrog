
struct ExpireSliderData { char buf[48]; struct DateTime dt; };
struct DisuseSliderData { char buf[48]; };

SW_MkClass(ExpireSlider, MUIC_Slider)
SW_MkClass(DisuseSlider, MUIC_Slider)

__asm ULONG ExpireSliderDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct ExpireSliderData *idata = INST_DATA(cl, o);

   MkClassDispatchInit(cl);

   switch (msg->MethodID) {
     case MUIM_Numeric_Stringify: {
        struct MUIP_Numeric_Stringify *m = (APTR)msg;

        idata->dt.dat_Stamp.ds_Days   = m->value;
        idata->dt.dat_Stamp.ds_Minute = 0;
        idata->dt.dat_Stamp.ds_Tick   = 0;
        idata->dt.dat_Format          = FORMAT_DOS;
        idata->dt.dat_Flags           = 0;
        idata->dt.dat_StrDate         = idata->buf;
        idata->dt.dat_StrTime         = NULL;
        idata->dt.dat_StrDay          = NULL;
        DateToStr(&idata->dt);

        strcat(idata->buf, "  ");
        
        return((ULONG)idata->buf);
     }
   }

   return DoSuperMethodA(cl, o, msg);
}


__asm ULONG DisuseSliderDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct DisuseSliderData *idata = INST_DATA(cl, o);
   putreg(REG_A6, (long)cl->cl_UserData);
   geta4();

   switch (msg->MethodID) {
     case MUIM_Numeric_Stringify: {
        struct MUIP_Numeric_Stringify *m = (APTR)msg;

#       define plural(v) ((v)==1?"":"s")
        
        if (m->value <  365)
           sprintf(idata->buf, "%ld Day%s   ", m->value, plural(m->value));
        else {
           ULONG y = m->value / 365, d = m->value % 365;
           sprintf(idata->buf,"%ld Year%s, %ld Day%s  ",y,plural(y),d,plural(d));
        }
        
        return((ULONG)idata->buf);
     }
   }

   return DoSuperMethodA(cl, o, msg);
}

