# include "S-MkClass.h"

struct PopSrvListData {
   SWP_NNews_AttrDefs;
   Object *SList;
   Object *SStr;
   Object *SrvObj;
   Object *LObj;
   ULONG   SrvAttr;
   ULONG   CopyOnDisp;
};

SW_MkClass(PopSrvList, MUIC_Popobject)

static ULONG __stdargs DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1,...)
{
   return DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
}

static ULONG FindSrv(Object *SList, STRPTR InitSrv)
{
   ULONG x;
   Object *a;
   
   if (!SW_STRISNULL(InitSrv))
      SW_MUIListLoop(SList, x, a, TRUE)
         if (!strcmp(SWGetStr(a, SWA_NServer_Host), InitSrv))
            return x;

   return MUIV_List_Active_Off;
}


/*****************************************************************************
 * Dispatcher for PopSrvList class                                           *
 *****************************************************************************/

__asm ULONG PopSrvListDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct PopSrvListData *idata = INST_DATA(cl, o);
   TEXT srv[256];

   MkClassDispatchInit(cl);

#  define attrl ((opSet *)msg)->ops_AttrList

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *s, *p, *obj, *SList, *AttrUI, *SrvObj, *lv, *def=NULL, *grp1,
               *TopObj;
        ULONG x, SrvAttr, DefBt, CopyOnDisp;
        STRPTR InitSrv;

        TopObj     = GetTagObj(SWA_AppBase_TopObj,       NULL,  attrl);
        CopyOnDisp = GetTagUL(SWA_PopSrvList_CopyOnDisp, FALSE, attrl);
        AttrUI     = GetTagObj(SWA_PopSrvList_AttrUI,    NULL,  attrl);
        SrvAttr    = GetTagUL(SWA_PopSrvList_SrvAttr,    NULL,  attrl);
        SrvObj     = GetTagObj(SWA_PopSrvList_SrvObj,    NULL,  attrl);
        DefBt      = GetTagUL(SWA_PopSrvList_DefBt,      TRUE,  attrl);

        if (!AttrUI || !SrvAttr) return NULL;

        if (!(s = (Object *)DoMethod(AttrUI, MUIM_FindUData, SrvAttr)))
           return NULL;

        if (!(p = SWGetObj(s, MUIA_Parent))) return NULL;

        DoMethod(p, OM_REMMEMBER, s);

        if (DefBt) grp1 = HGroup,
              Child, def = MkBt(SWS_B_Def, SWS_H_Def),
              Child, s,
           End;
        else grp1 = s;
        
        obj = (Object *)DoSuperNew(cl, o,
           MUIA_Popstring_String, grp1,
           MUIA_Popstring_Button, PopButton(MUII_PopUp),
           MUIA_Popobject_Object, SList = NServerListObject,
              AppBaseTopObj(TopObj),
              SWA_NList_Master,           SWGetObj(TopObj, SWA_NNews_ServList),
              SWA_NServerList_Tiny,       TRUE,
              SWA_NServerList_HostStrObj, s,
           End,
           TAG_MORE, attrl);

        if (def) set(def, MUIA_HorizWeight, 1);
        
        if (!obj) return NULL;

        DoMethod(p, OM_ADDMEMBER, obj);
        
        idata                     = INST_DATA(cl, obj);
        idata->SList              = SList;
        idata->SStr               = s;
        idata->SrvAttr            = SrvAttr;
        idata->SrvObj             = SrvObj;
        idata->SWA_AppBase_TopObj = TopObj;
        idata->CopyOnDisp         = CopyOnDisp;
        InitSrv                   = SWGetStr(SrvObj, SrvAttr);
        x                         = FindSrv(SList, InitSrv);

        DoMethod(SList, SWM_NList_Change, x);
        set(SList, MUIA_List_Active, x);
        set(s,     MUIA_String_Contents, InitSrv);

        lv = SWGetObj(SList, SWA_NList_LV);
        DoMethod(lv, DCLK, obj, 2, MUIM_Popstring_Close, TRUE);
        DoMethod(lv, DCLK, obj, 1, SWM_PopSrvList_FixHS);

        if (def) DoMethod(def, NOTP, obj, 1, SWM_PopSrvList_SetDef);

        return (ULONG)obj;
     }

     case OM_DISPOSE: {
        if (idata->CopyOnDisp)
           strcpy(srv, SWGetStr(idata->SrvObj, idata->SrvAttr));

        set(idata->SList, MUIA_List_Active, MUIV_List_Active_Off);
        
        if (idata->CopyOnDisp)
           set(idata->SrvObj, idata->SrvAttr, srv);
        
        break;
     }

     case SWM_PopSrvList_SetDef: {
        set(idata->SList, MUIA_List_Active, MUIV_List_Active_Off);
        set(idata->SrvObj, idata->SrvAttr, "");
        SetAttrs(idata->SStr,
                 MUIA_String_Contents, "",
                 MUIA_Disabled, TRUE,
                 TAG_DONE);
        break;
     };
      
     case SWM_PopSrvList_FixHS: {
        strcpy(srv, SWGetStr(idata->SStr, MUIA_String_Contents));

        set(idata->SrvObj, idata->SrvAttr, srv);
   
        set(idata->SList, MUIA_List_Active, MUIV_List_Active_Off);

        DoMethod(idata->SStr, MUIM_Notify, MUIA_String_Contents,
                 MUIV_EveryTime, o, 3,
                 MUIM_Set, idata->SrvAttr, MUIV_TriggerValue);

        SetAttrs(idata->SStr,
                 MUIA_String_Contents, srv,
                 MUIA_Disabled,        FALSE,
                 TAG_DONE);

        return 0;
     }
   }

   return DoSuperMethodA(cl, o, msg);
}
