
#define UIOBJ(x) static __inline Object *x (const SWAttrMapT *Map, \
                                            const STRPTR MsgStr[], \
                                            Tag *EditA, Tag *SetA, \
                                            Object *o, Object **so)

/*****************************************************************************
 * String UI objects                                                         *
 *****************************************************************************/

UIOBJ(UI_Str)
{
   Object *e = NewObject(SWObjStringClass(), NULL,
                         StringFrame,
                         MUIA_String_MaxLen, Map->Max-1,
                         SWA_ObjString_StrAttr, (Map->SGI&SWAM_DropStr) ? Map->Min : 0,
                         TAG_DONE);

   if (Map->SGI & (SWAM_PopFile | SWAM_PopDir)) {
      // -- set up app message drag/drop ---

      DoMethod(e, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
               e, 2, SWM_ObjString_AppMsg, MUIV_TriggerValue);

      e = PopaslObject,
         MUIA_Popstring_String, e,
         MUIA_Popstring_Button, (Map->SGI & SWAM_PopFile) ?
                                PopButton(MUII_PopFile) :
                                PopButton(MUII_PopDrawer),
         MUIA_Popasl_Type,      ASL_FileRequest,
         ASLFR_DrawersOnly,     (Map->SGI & SWAM_PopDir) ? TRUE : FALSE,
      End;
   }

   if (Map->SGI & SWAM_PopFont) {
      e = PopaslObject,
         MUIA_Popstring_String, e,
         MUIA_Popstring_Button, PopButton(MUII_PopUp),
         MUIA_Popasl_Type,      ASL_FontRequest,
      End;
   }

   *EditA = MUIA_String_Contents;
   *SetA  = (Map->SGI&SWAM_StrRet) ? MUIA_String_Acknowledge : MUIA_String_Contents;

   return *so = e;
}


/*****************************************************************************
 * Background popup UI object                                                *
 *****************************************************************************/

UIOBJ(UI_BackG)
{
   *EditA = *SetA = MUIA_Imagedisplay_Spec;

   return *so = MUI_NewObject(MUIC_Popimage,
                              MUIA_Window_Title,
                              Map->Label ? MsgStr[Map->Label] :
                              "Edit Background",
                              TAG_DONE);
}


/*****************************************************************************
 * Pen popup UI object                                                       *
 *****************************************************************************/

UIOBJ(UI_Pen)
{
   *EditA = *SetA = MUIA_Pendisplay_Spec;

   return *so = PoppenObject,
      MUIA_Window_Title, Map->Label?MsgStr[Map->Label]:"Edit Color",
   End;
}


/*****************************************************************************
 * Integer UI object                                                         *
 *****************************************************************************/

UIOBJ(UI_Int)
{
   Object *e;

   if (Map->SGI & SWAM_Cycle) {
      LONG x; UBYTE **Labels = (UBYTE **)Map->Max;

      *EditA = *SetA = MUIA_Cycle_Active;

      for (x=0; x<Map->Min; x++) Labels[x] = MsgStr[x+Map->Label+1];
      Labels[x] = NULL;
      
      e = Cycle(Labels);
   } else if ((Map->SGI & SWAM_IntStr) && !(Map->SGI & SWAM_TimeSld)) {
      *EditA = MUIA_String_Integer;
      *SetA  = SWA_ObjString_NumVal;

      e = NewObject(SWObjStringClass(),      NULL,
                    StringFrame,
                    MUIA_String_Accept,      "0123456879",
                    MUIA_String_MaxLen,      8,
                    TAG_DONE);
   } else if (Map->SGI & (SWAM_TimeSld|SWAM_KbSld)) {
      *EditA = *SetA = MUIA_Numeric_Value;

      e = NewObject((Map->SGI & SWAM_TimeSld) ?
                    SWTimesliderClass() : KbSliderClass(), NULL,
                    MUIA_Numeric_Min,   Map->Min,
                    MUIA_Numeric_Max,   Map->Max,
                    MUIA_Numeric_Value, Map->Default,
                    SWA_TimeSlider_NoDiv1000, (Map->SGI & SWAM_TimeSec) ?
                                              TRUE : FALSE,
                    TAG_DONE);
   } else {
      *EditA = *SetA = MUIA_Slider_Level;
      e = Slider(Map->Min, Map->Max, Map->Default);
   }

   return *so = e;
}


/*****************************************************************************
 * Boolean UI object                                                         *
 *****************************************************************************/

UIOBJ(UI_Bool)
{
   *so = CheckMark(0);

   *EditA = *SetA = MUIA_Selected;

   if (!so) return NULL;
   
   return HGroup, MUIA_Weight, 1, Child, *so, Child, HSpace(0), End;
}


/*****************************************************************************
 * Object UI object                                                          *
 *****************************************************************************/

UIOBJ(UI_Obj)
{
   Object *tobj = SWGetObj(o, *EditA = *SetA = Map->Attr);

   return *so = (Object *)(tobj ? DoMethod(tobj, SWM_Invocation_MakeEditUI) : NULL);
}

# undef UIOBJ
