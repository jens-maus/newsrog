struct TextScrollData { Object *ScrollBar, *VGObj; ULONG IsSetup, InitLine;};

SW_MkClass(TextScroll, MUIC_Group)


/*****************************************************************************
 * Dispatcher for TextScroll class                                           *
 *****************************************************************************/

__asm ULONG TextScrollDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct TextScrollData *idata = INST_DATA(cl, o);

   MkClassDispatchInit(cl);

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *SubObj, *obj = (Object*)DoSuperMethodA(cl, o, msg);
        LONG Vis, Height = 0, State = 0;

#       define attrl ((opSet *)msg)->ops_AttrList
        if (obj) {
           idata = INST_DATA(cl, obj);

           if (!(SubObj = GetTagObj(SWA_TextScroll_Obj, NULL, attrl)))
              return NULL;

           idata->InitLine = GetTagUL(SWA_TextScroll_InitLine, LONG_MAX, attrl);
           
           set(obj, MUIA_Group_Horiz, TRUE);

           DoMethod(obj, OM_ADDMEMBER, idata->ScrollBar = PropObject,
                       PropFrame,
                       MUIA_HorizWeight, 0,
                    End);
           
           if (!idata->ScrollBar) return NULL;

           DoMethod(obj, OM_ADDMEMBER, RectangleObject, MUIA_FixWidth, 2, End);

           Vis = GetTagUL(SWA_TextScroll_TxtHeight, NULL, attrl);
           Vis = max(Vis, 1);
           Vis = min(Vis, 256);

           DoMethod(SubObj, SWM_ListSearch_Setup, &State);
           DoMethod(SubObj, SWM_CText_LineRangeHeight, 0, Vis-1, &Height);
           Height = max(Height, 5);
           Height = min(Height, 4086);

           DoMethod(obj, OM_ADDMEMBER, idata->VGObj = VirtgroupObject,
               MUIA_Background,     MUII_TextBack,
               MUIA_FixHeight,      Height,
               MUIA_Group_Horiz,    FALSE,
               MUIA_Group_Spacing,  0,
           End);

           if (GetTagObj(SWA_TextScroll_IsGrp, FALSE, attrl)) {
              Object  *SObj;
              APTR    state;
              List   *l;

              SW_GETLH(SubObj, l);
              SW_LISTLOOP(l, state, SObj) {
                 DoMethod(SubObj,       OM_REMMEMBER, SObj);
                 DoMethod(idata->VGObj, OM_ADDMEMBER, SObj);
              }

              MUI_DisposeObject(SubObj);
           } else {
              DoMethod(idata->VGObj, OM_ADDMEMBER, SubObj);
           }
        }

        return (ULONG)obj;
     }

     case MUIM_Setup: {
        idata->IsSetup = FALSE;
        break;
     }

     case MUIM_Show: {
        ULONG Height, GadH;
        
        if (idata->IsSetup) break;

        idata->IsSetup = TRUE;

        Height = SWGetUL(idata->VGObj, MUIA_Virtgroup_Height);
        GadH   = SWGetUL(idata->VGObj, MUIA_Height);

        SetAttrs(idata->ScrollBar,
                 MUIA_Prop_Entries, Height,
                 MUIA_Prop_Visible, GadH,
                 TAG_DONE);

        DoMethod(idata->ScrollBar, MUIM_Notify, MUIA_Prop_First, MUIV_EveryTime,
                 idata->VGObj, 3, MUIM_Set, MUIA_Virtgroup_Top, MUIV_TriggerValue);

        DoMethod(idata->VGObj, MUIM_Notify, MUIA_Virtgroup_Top, MUIV_EveryTime,
                 idata->ScrollBar, 3, MUIM_Set, MUIA_Prop_First, MUIV_TriggerValue);

        set(idata->VGObj, MUIA_Virtgroup_Top, idata->InitLine);

        break;
     }

     case MUIM_Cleanup: {
        DoMethod(idata->ScrollBar, MUIM_KillNotify, MUIA_Prop_First);
        DoMethod(idata->VGObj,     MUIM_KillNotify, MUIA_Virtgroup_Top);
        break;
     }
   }

   return DoSuperMethodA(cl, o, msg);
}

# undef attrl
