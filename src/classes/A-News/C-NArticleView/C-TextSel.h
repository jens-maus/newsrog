struct TextSelData { LONG sel; ULONG Secs, Micros; };

SW_MkClassPriv(TextSel, TextHideClassPtr)

/*****************************************************************************
 * Dispatcher for TextSel class                                              *
 *****************************************************************************/

__asm ULONG TextSelDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct TextSelData *idata = INST_DATA(cl, o);

   MkClassDispatchInit(cl);

   switch (msg->MethodID) {
     case MUIM_Setup: {
        if (!DoSuperMethodA(cl, o, msg)) return FALSE;
        idata->sel = FALSE;
        return TRUE;
     }
      
     case MUIM_Cleanup: {
        break;
     }

     case MUIM_HandleInput: {
        struct MUIP_HandleInput *h = (void *)msg;
        ULONG  Secs, Micros;

        if (h->imsg && h->imsg->Class == IDCMP_MOUSEBUTTONS) {
           if (h->imsg->Code==SELECTDOWN) {
              if (_isinobject(o, h->imsg->MouseX, h->imsg->MouseY)) {
                 SetAttrs(o,
                          SWA_TextHide_DispObj, 1,
                          SWA_TextSel_Pressed,  TRUE,
                          TAG_DONE);
                 idata->sel = TRUE;
              }
           } else
              if (idata->sel) {
                 set(o, SWA_TextHide_DispObj, 0);
                 idata->sel = FALSE;
                 if (_isinobject(o, h->imsg->MouseX, h->imsg->MouseY))
                    set(o, SWA_TextSel_Pressed, FALSE);

                 CurrentTime(&Secs, &Micros);

                 if (DoubleClick(idata->Secs, idata->Micros, Secs, Micros))
                    set(o, SWA_TextSel_DoubleClick, TRUE);
                 
                 idata->Secs   = Secs,
                 idata->Micros = Micros;
              }
        }
        break;
     }
   }

   return DoSuperMethodA(cl, o, msg);
}
