# include "Libraries/C-Root.h"
# include "Libraries/C-E_ListMgr.h"

static struct MUI_CustomClass *ObjListClass = NULL;

struct ObjListData { Object *nobj; };

__asm ULONG ObjListDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct ObjListData *idata = INST_DATA(cl, o);

   putreg(REG_A6, (long)cl->cl_UserData);
   geta4();

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *obj = (Object*)DoSuperMethodA(cl, o, msg);

        if (obj) {
           idata = INST_DATA(cl, obj);
           idata->nobj = GetTagObj(SWA_ObjList_Object, NULL,
                                   ((opSet *)msg)->ops_AttrList);

           set(obj, MUIA_Dropable, TRUE);
        }
        return (ULONG)obj;
     }

     case OM_GET: {
        struct opGet *m = ((struct opGet *)msg);
        if (m->opg_AttrID == SWA_ObjList_Object)
           if (m->opg_Storage)
              *((ULONG *)m->opg_Storage) = (ULONG)idata->nobj;
        break;
     }

     case OM_SET:
     case OM_UPDATE: {
        TagItem    *ti;

        if (ti=FindTagItem(SWA_ObjList_Object,((opSet *)msg)->ops_AttrList))
           idata->nobj = (Object *)ti->ti_Data;

        break;
     }

     case MUIM_DragQuery: {
        struct MUIP_DragQuery *m = ((struct MUIP_DragQuery *)msg);

        if (m->obj == o) break;  // go to supermethod;
        if (idata->nobj)
           return DoMethod(idata->nobj, SWM_ObjList_DragQuery, m->obj, o);
        break;
     }
      
     case MUIM_DragDrop: {
        struct MUIP_DragDrop *m = ((struct MUIP_DragDrop *)msg);
        ULONG rc = 0;

        if (m->obj == o) rc = DoSuperMethodA(cl, o, msg);

        if (idata->nobj)
           DoMethod(idata->nobj, SWM_ObjList_DragDrop, m->obj, m->x, m->y, o);

        return rc;
     }

     case SWM_Root_GetDragObj: return DoMethodA(idata->nobj, msg);
   }

   return DoSuperMethodA(cl, o, msg);
}


__saveds __asm struct IClass *SWObjListClass(void)
{ return SW_GETICLASS(ObjListClass); }

struct MUI_CustomClass *ObjListInit(Library *libbase)
{
   ObjListClass = MUI_CreateCustomClass(NULL, MUIC_List, NULL,
                                        sizeof(struct ObjListData),
                                        ObjListDispatcher);

   SW_GETICLASS(ObjListClass)->cl_UserData = (ULONG)libbase;

   return ObjListClass;
}

void ObjListExit(void)
{
   if (ObjListClass) MUI_DeleteCustomClass(ObjListClass);
}
