# include "Libraries/C-Root.h"
# include "Libraries/C-E_ListMgr.h"

static struct MUI_CustomClass *ObjStringClass = NULL;

struct ObjStringData { ULONG StrAttr, LastVal; };

__asm ULONG ObjStringDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   TagItem    *ti;
   struct ObjStringData *idata = INST_DATA(cl, o);

   putreg(REG_A6, (long)cl->cl_UserData);
   geta4();

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *obj = (Object*)DoSuperMethodA(cl, o, msg);
        if (obj) {
           idata = INST_DATA(cl, obj);
           if (ti = FindTagItem(SWA_ObjString_StrAttr, ((opSet *)msg)->ops_AttrList))
              idata->StrAttr = ti->ti_Data;
           SetAttrs(obj, MUIA_Draggable,          TRUE,
                         MUIA_CycleChain,         1,
                         MUIA_String_AdvanceOnCR, TRUE, TAG_DONE);
        }
        return (ULONG)obj;
     }

     case OM_GET: {
        if (((struct opGet *)msg)->opg_AttrID == SWA_ObjString_NumVal) {
           ULONG *Store = ((struct opGet *)msg)->opg_Storage;

           if (Store) *Store = SWGetUL(o, MUIA_String_Integer);

           return 1;
        }
      
        break;
      }
      
     case OM_SET:
     case OM_UPDATE: {
        ULONG rc;
        
        if (ti=FindTagItem(MUIA_String_Contents,((opSet *)msg)->ops_AttrList)) {
           rc = DoSuperMethodA(cl, o, msg);
           set(o, SWA_ObjString_NumVal, SWGetUL(o, MUIA_String_Integer));
           return rc;
        }
        
   
        if (ti=FindTagItem(SWA_ObjString_NumVal,((opSet *)msg)->ops_AttrList)) {
           if (idata->LastVal == ti->ti_Data) {
              ti->ti_Tag = TAG_IGNORE;
              rc = DoSuperMethodA(cl, o, msg);
              ti->ti_Tag = SWA_ObjString_NumVal;
              return rc;
           }
         
           SetSuperAttrs(cl, o,
                         MUIA_NoNotify,       TRUE,
                         MUIA_String_Integer, idata->LastVal = ti->ti_Data,
                         TAG_DONE);
        }
     }
      
     break;

     case MUIM_DragQuery: {
        Object *drag_o = ((struct MUIP_DragQuery *)msg)->obj;
        if (drag_o == o)                        return MUIV_DragQuery_Refuse;
        if (OCLASS(drag_o) == OCLASS(o)) {
           UBYTE *s1, *s2;
           get(drag_o, MUIA_String_Accept, &s1);
           get(o,      MUIA_String_Accept, &s2);
           if (!s1 && !s2) return MUIV_DragQuery_Accept;
           if (!s1 || !s2) return MUIV_DragQuery_Refuse;
           return (ULONG)(!strcmp(s1, s2) ? MUIV_DragQuery_Accept
                          : MUIV_DragQuery_Refuse);
        }

        if (OCLASS(drag_o) == SWObjListClass()) SWDB_RET(MUIV_DragQuery_Accept);

        if (!idata->StrAttr)                    return MUIV_DragQuery_Refuse;

        if (!(drag_o = (Object *)DoMethod(drag_o, SWM_Root_GetDragObj)))
           return MUIV_DragQuery_Refuse;
        
        return (ULONG)(get(drag_o, idata->StrAttr, NULL)
                       ? MUIV_DragQuery_Accept : MUIV_DragQuery_Refuse);
     }
      
     case MUIM_DragDrop: {
        Object *drag_o = ((struct MUIP_DragQuery *)msg)->obj;
        if (OCLASS(drag_o) == OCLASS(o)) {
           set(o, MUIA_String_Contents, SWGetUL(drag_o, MUIA_String_Contents));
           set(o, SWA_ObjString_NumVal, SWGetUL(o,      MUIA_String_Integer));
           return 0;
        }

        // -- if objlist, append its primary attribute -----------------------

        if (OCLASS(drag_o) == SWObjListClass()) {
           UBYTE *c, *val;
           get(o, MUIA_String_Contents, &c);

           val = (UBYTE *)DoMethod((Object *)muiUserData(drag_o),
                                   SWM_E_Listmgr_StrCopy, c);

           if (val && c) {
              set(o,  MUIA_String_Contents, val);
              FreeVec(val);
              return 0;
           }
        }

        if (!(drag_o = (Object *)DoMethod(drag_o, SWM_Root_GetDragObj)))
           return MUIV_DragQuery_Refuse;
        
        set(o, MUIA_String_Contents, SWGetStr(drag_o, idata->StrAttr));
        return 0;
     }

     case SWM_ObjString_AppMsg: {
        UBYTE FSpec[FMSIZE];
        struct AppMessage *AMsg = ((struct SWP_AppMsg *)msg)->AMsg;

        if (AMsg->am_NumArgs > 0) {
           NameFromLock(AMsg->am_ArgList[0].wa_Lock, FSpec, FMSIZE);
           AddPart(FSpec, AMsg->am_ArgList[0].wa_Name, FMSIZE);
           set(o, MUIA_String_Contents, FSpec);
        }
        return 0;
     }
     
   }
   
   return DoSuperMethodA(cl, o, msg);
}


__saveds __asm struct IClass *SWObjStringClass(void)
{ return SW_GETICLASS(ObjStringClass); }

struct MUI_CustomClass *ObjStringInit(Library *libbase)
{
   ObjStringClass = MUI_CreateCustomClass(NULL, MUIC_String, NULL,
                                          sizeof(struct ObjStringData),
                                          ObjStringDispatcher);

   SW_GETICLASS(ObjStringClass)->cl_UserData = (ULONG)libbase;

   return ObjStringClass;
}

void ObjStringExit(void)
{
   if (ObjStringClass) MUI_DeleteCustomClass(ObjStringClass);
}
