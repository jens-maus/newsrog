struct TextHideData { Object *Obj0, *Obj1, *Grp; LONG Num; };

SW_MkClass(TextHide, MUIC_Group)


/*****************************************************************************
 * Function: Find parent virtgroup                                           *
 *****************************************************************************/

static Object *ParentGroup(Object *o)
{
   for (; o; o = SWGetObj(o, MUIA_Parent))
      if (muiUserData(o) == SWV_NArticleView_SGroup)
         return SWGetObj(o, MUIA_Parent);

   return NULL;
}


/*****************************************************************************
 * Function: change subobject                                                *
 *****************************************************************************/

static void THChg(Object *o, struct TextHideData *idata, LONG New)
{
   if (New == idata->Num) return;

   if (New == 0 || New == 1)
      DoMethod(idata->Grp, MUIM_Group_InitChange);
   
   if (idata->Num == 0) DoMethod(idata->Grp, OM_REMMEMBER, idata->Obj0);
   if (idata->Num == 1) DoMethod(idata->Grp, OM_REMMEMBER, idata->Obj1);

   idata->Num = New;

   if (idata->Num == 0) DoMethod(idata->Grp, OM_ADDMEMBER, idata->Obj0);
   if (idata->Num == 1) DoMethod(idata->Grp, OM_ADDMEMBER, idata->Obj1);

   if (New == 0 || New == 1)
      DoMethod(idata->Grp, MUIM_Group_ExitChange);

   // -- try to scroll new object into visible region ------------------------
   
   if (New == 1)
      DoMethod(ParentGroup(o), SWM_NArticleView_MakeObjVis, o);
}


/*****************************************************************************
 * Dispatcher for TextHide class                                             *
 *****************************************************************************/

__asm ULONG TextHideDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct TextHideData *idata = INST_DATA(cl, o);

   MkClassDispatchInit(cl);

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *b, *obj = (Object*)DoSuperMethodA(cl, o, msg);
        ULONG VLayout;

#       define attrl ((opSet *)msg)->ops_AttrList
        if (obj) {
           idata = INST_DATA(cl, obj);
           idata->Num  = -1;
           idata->Obj0 = GetTagObj(SWA_TextHide_Obj0,   NULL,  attrl);
           idata->Obj1 = GetTagObj(SWA_TextHide_Obj1,   NULL,  attrl);
           VLayout     = GetTagUL(SWA_TextHide_VLayout, FALSE, attrl);

           if (!idata->Obj0 || !idata->Obj1)
              return NULL;

           set(idata->Grp = obj, MUIA_Group_Horiz, !VLayout);

           if (GetTagUL(SWA_TextHide_UI, TRUE, attrl)) {
              if (!(b = GetTagObj(SWA_TextHide_Button, NULL, attrl)))
                 b = ImageObject,
                    ImageButtonFrame,
                    MUIA_Image_Spec, MUII_TapeStop,
                    MUIA_CycleChain, 1,
                    MUIA_InputMode,  MUIV_InputMode_RelVerify,
                 End;

              if (VLayout) {
                 set(b, MUIA_HorizWeight, 1);
   
                 DoMethod(obj, OM_ADDMEMBER, HGroup,
                             Child, b,
                             Child, HSpace(0),
                          End);
   
                 DoMethod(obj, OM_ADDMEMBER, idata->Grp = HGroup,
                             Child, RectangleObject,
                                MUIA_FixWidth, 4,
                             End,
                             Child, MUI_MakeObject(MUIO_VBar,1),
                          End);
              } else {
                 DoMethod(obj, OM_ADDMEMBER, (VGroup,
                            Child, b,
                            Child, MUI_MakeObject(MUIO_VBar,1),
                          End));
              }

              DoMethod(b, NOTP, obj, 1, SWM_TextHide_Toggle);
           }

           THChg(obj, idata, GetTagUL(SWA_TextHide_DispObj, 0, attrl));
        }
        return (ULONG)obj;
     }

     case OM_DISPOSE: {
        THChg(o, idata, -1);
        MUI_DisposeObject(idata->Obj0);
        MUI_DisposeObject(idata->Obj1);
        break;
     }

     case OM_SET: {
        LONG New = GetTagUL(SWA_TextHide_DispObj, -99, attrl);
        if (New != -99) THChg(o, idata, New);
        break;
     }

     case SWM_TextHide_Toggle: {
        THChg(o, idata, 1 - idata->Num);
        return 0;
     }
   }

   return DoSuperMethodA(cl, o, msg);
}

# undef attrl
