# include "S-MkClass.h"

typedef struct NGListData {
   SWP_NNews_AttrDefs;

   Object *Str, *L, *LV, *L1, *LV1, *GL;

   ULONG Recur, A4, Open;
   UBYTE Sep;

   STRPTR RestoreVal;
};

static ULONG __stdargs DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1,...)
{
   return DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
}


SW_MkClass(NGList, MUIC_Popobject)

static LONG FindGrp(struct NGListData *idata, STRPTR gn)
{
   LONG x;
   STRPTR lgn;

   SW_MUIListLoop(idata->L, x, lgn, TRUE)
      if (!strcmp(lgn, gn)) 
         return TRUE;

   return FALSE;
}

static void InsGrp(struct NGListData *idata, STRPTR gn, ULONG InsLoc)
{
   if (!SW_STRISNULL(gn) && !FindGrp(idata, gn))
      DoMethod(idata->L, MUIM_List_InsertSingle, gn, InsLoc);
}

static void SetupROGrps(struct NGListData *idata)
{
   LONG x;
   Object *g;
   STRPTR gn;

   set(idata->L1, MUIA_List_Quiet, TRUE);

   DoMethod(idata->L1, MUIM_List_Clear);

   idata->GL = NN_GroupList(idata);
   
   SW_MUIListLoop(idata->GL, x, g, TRUE) {
      gn = SWGetStr(g, SWA_NGroup_Name);

      if (!SW_STRISNULL(gn) && !FindGrp(idata, gn))
         DoMethod(idata->L1, MUIM_List_InsertSingle,
                  gn, MUIV_List_Insert_Sorted);
   }

   set(idata->L1, MUIA_List_Quiet, FALSE);
}


/*****************************************************************************
 * Dispatcher for TextHide class                                             *
 *****************************************************************************/

#  define attrl ((opSet *)msg)->ops_AttrList

__asm ULONG NGListDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct NGListData *idata = INST_DATA(cl, o);

   MkClassDispatchInit(cl);

   switch (msg->MethodID) {
      case OM_DISPOSE: {
         FreeVec(idata->RestoreVal);
         break;
      }
      
      case OM_NEW: {
         Object *obj, *Str, *L, *LV, *L1, *LV1, *b_Del, *b_Close, *b_Rest,
                *TmpGrp = NULL, *p, *AttrUI, *tmpobj;
         STRPTR HelpL, HelpR;
         ULONG  Attr, found;
         List *uilist;
         APTR state;
         
         Str        = GetTagObj(SWA_NGList_Str,     NULL, attrl);
         HelpL      = GetTagStr(SWA_NGList_HelpL,   NULL, attrl);
         HelpR      = GetTagStr(SWA_NGList_HelpR,   NULL, attrl);
         Attr       = GetTagUL(SWA_NGList_Attr,     0,    attrl);
         AttrUI     = GetTagObj(SWA_NGList_AttrUI,  NULL, attrl);

         // -- copy part of the UI to a scratch list for safe keeping --------
         
         if (Attr && AttrUI) {
            if (!(Str = (Object *)DoMethod(AttrUI, MUIM_FindUData, Attr)))
               return NULL;

            if (!(p = SWGetObj(Str, MUIA_Parent))) return NULL;

            TmpGrp = GroupObject, End;
            if (!TmpGrp) return NULL;

            found = FALSE;
            SW_GETLH(p, uilist);
            SW_LISTLOOP(uilist, state, tmpobj) {
               if (tmpobj == Str)
                  found = TRUE;

               if (found) {
                  DoMethod(p,      OM_REMMEMBER, tmpobj);

                  if (tmpobj != Str)
                     DoMethod(TmpGrp, OM_ADDMEMBER, tmpobj);
               }
            }
         }
         
         if (!Str)
            Str = StringObject, StringFrame, End;

         SetAttrs(Str,
                  MUIA_CycleChain,    1,
                  MUIA_String_MaxLen, 8192,
                  MUIA_String_Reject, " ",
                  TAG_DONE);

         
         // -- now make our object -------------------------------------------
         
         obj = (Object*)DoSuperNew(cl, o,
                                   MUIA_Popstring_Button, PopButton(MUII_PopUp),
                                   MUIA_Popstring_String, Str,
                                   MUIA_Popobject_Object, VGroup,
                                      Child, HGroup,
                                         Child, LV = ListviewObject,
                                            MUIA_ShortHelp,      HelpL,
                                            MUIA_CycleChain,     1,
                                            MUIA_Listview_Input, TRUE,
                                            MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
                                            MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
                                            MUIA_Listview_List, L = ObjListObject,
                                               InputListFrame,
                                               MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
                                               MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
                                               MUIA_List_DragSortable,  TRUE,
                                            End,
                                         End,
                                         Child, BalanceObject, End,
                                         Child, LV1=ListviewObject,
                                            MUIA_ShortHelp,      HelpR,
                                            MUIA_CycleChain,     1,
                                            MUIA_Listview_Input, TRUE,
                                            MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
                                            MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
                                            MUIA_Listview_List, L1 = ObjListObject,
                                               ReadListFrame,
                                               MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
                                               MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
                                               MUIA_List_DragSortable,  TRUE,
                                               MUIA_List_ShowDropMarks, FALSE,
                                            End,
                                         End,
                                      End,
                                      Child, HGroup,
                                         MUIA_Group_SameSize, TRUE,
                                         Child, b_Del   = MkBt(SWS_NG_L_Del, SWS_NG_H_Del),
                                         Child, b_Rest  = MkBt(SWS_NG_L_Rest, SWS_NG_H_Rest),
                                         Child, b_Close = MkBt(SWS_NG_L_Close, SWS_NG_H_Close),
                                      End,
                                   End,
                                   TAG_MORE, msg);
   
         if (!obj) {
            MUI_DisposeObject(TmpGrp);
            return NULL;
         }

         set(L,  SWA_ObjList_Object, obj);
         set(L1, SWA_ObjList_Object, obj);

         idata                     = INST_DATA(cl, obj);
         idata->Str                = Str;
         idata->LV                 = LV;
         idata->L                  = L;
         idata->LV1                = LV1;
         idata->L1                 = L1;
         idata->SWA_AppBase_TopObj = GetTagObj(SWA_AppBase_TopObj, NULL,  attrl);
         idata->A4                 = getreg(REG_A4);

         DoMethod(b_Del,   NOTP, obj, 1, SWM_NGList_DelSel);
         DoMethod(b_Close, NOTP, obj, 2, MUIM_Popstring_Close, TRUE);
         DoMethod(b_Rest,  NOTP, obj, 1, SWM_NGList_Restore);

         DoMethod(LV1, DCLK, obj, 1, SWM_NGList_DClick);
         
         // -- reconstruct the UI with our addition --------------------------
         
         if (Attr && AttrUI) {

            DoMethod(p, OM_ADDMEMBER, obj);  // add ourselves.
            
            SW_GETLH(TmpGrp, uilist);  // then add the rest of the UI we saved
            SW_LISTLOOP(uilist, state, tmpobj) {
               DoMethod(TmpGrp, OM_REMMEMBER, tmpobj);
               DoMethod(p,     OM_ADDMEMBER, tmpobj);
            }

            MUI_DisposeObject(TmpGrp);
         }

         return (ULONG)obj;
     }
   
      case SWM_NGList_GroupsToString: {
         LONG x, GrpCount, TotSize = 2;
         STRPTR g, groups;
   
         SW_MUIListLoop(idata->L, x, g, TRUE)
            TotSize += strlen(g) + 1;

         GrpCount = x - 1;

         if (!(groups = AllocVec(TotSize, MEMF_ANY)))
            return FAILURE;
   
         groups[0] = '\0';
         
         SW_MUIListLoop(idata->L, x, g, TRUE)
            strcat(strcat(groups, g), x != GrpCount ? "," : "");

         idata->Recur++;
         set(idata->Str, MUIA_String_Contents, groups);
         idata->Recur--;
         
         FreeVec(groups);
         SetupROGrps(idata);
   
         return SUCCESS;
      }

      case OM_GET: {
         switch (((opGet *)msg)->opg_AttrID) {
           case SWA_NGList_Str:
            *((ULONG *)(((opGet *)msg)->opg_Storage)) = (ULONG)idata->Str;
            return 1;
         }
         break;
      }
      
      case SWM_NGList_StrToGroups: {
         UBYTE c;
         LONG x = 0, LastPos = 0;
         STRPTR GL;

         if (idata->Recur) return SUCCESS;

         if (!(GL = SWGetStr(idata->Str, MUIA_String_Contents)))
            return FAILURE;

         set(idata->L, MUIA_List_Quiet, TRUE);
         DoMethod(idata->L, MUIM_List_Clear);
   
         while (TRUE) {
            if ((GL[x] == '\0' || GL[x] == ',') && LastPos != x) {
               c = GL[x], GL[x] = '\0';
   
               InsGrp(idata, GL+LastPos, MUIV_List_Insert_Bottom);
   
               GL[x] = c;
            }
   
            if (GL[x] == ',' || GL[x] == ' ')
               LastPos = x+1;
            
            if (!GL[x]) break;
            x++;
         }
         
         set(idata->L, MUIA_List_Quiet, FALSE);

         return SUCCESS;
      }

      case SWM_NGList_StrToGroupsRO: {
         ULONG rc = DoMethod(o, SWM_NGList_StrToGroups);
         SetupROGrps(idata);
         return rc;
      }
       
      case SWM_NGList_StrAck: {
         SetupROGrps(idata);
         return SUCCESS;
      }

      case SWM_NGList_DClick: {
         STRPTR g;

         DoMethod(idata->L1, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &g);

         if (!g) return FAILURE;
         
         InsGrp(idata, g, MUIV_List_Insert_Bottom);
         DoMethod(o, SWM_NGList_GroupsToString);
         return SUCCESS;
      }
       
      case SWM_ObjList_DragDrop: {
         struct SWP_ObjList_DragDrop *mdq = (struct SWP_ObjList_DragDrop *)msg;
         Object *g, *sgl = SWGetObj(idata->GL, SWA_NGroupList_SG_L);
         STRPTR t;
         LONG x, DropLoc;
         
         set(idata->L, MUIA_List_Quiet, TRUE);
         DropLoc = SWGetUL(idata->L, MUIA_List_DropMark);
         
         if (mdq->obj == SWGetObj(idata->GL, SWA_NList_L))
            SW_MUIListLoopSel(mdq->obj, x, g, TRUE)
               InsGrp(idata, SWGetStr(g, SWA_NGroup_Name), DropLoc);
         else if ((mdq->obj == idata->L1 || mdq->obj == sgl) && mdq->target == idata->L)
            SW_MUIListLoopSel(mdq->obj, x, t, TRUE)
               InsGrp(idata, t, DropLoc);
         else if (mdq->obj == idata->L && mdq->target == idata->L1)
            DoMethod(idata->L, MUIM_List_Remove, MUIV_List_Remove_Selected);
            
         set(idata->L, MUIA_List_Quiet, FALSE);
         
         DoMethod(o, SWM_NGList_GroupsToString);
    
         return SUCCESS;
      }

      case SWM_ObjList_DragQuery: {
         struct SWP_ObjList_DragQuery *mdq = (struct SWP_ObjList_DragQuery *)msg;

         return (mdq->target == idata->L &&
                 ((mdq->obj   == idata->L1 ||
                   mdq->obj   == SWGetObj(idata->GL, SWA_NGroupList_SG_L) ||
                   (mdq->obj == SWGetObj(idata->GL, SWA_NList_L)))) ||
                 mdq->target == idata->L1 && mdq->obj == idata->L) ?
            (ULONG)MUIV_DragQuery_Accept :
            (ULONG)MUIV_DragQuery_Refuse;
      }

      case SWM_NGList_DelSel: {
         DoMethod(idata->L, MUIM_List_Remove, MUIV_List_Remove_Selected);
         DoMethod(o, SWM_NGList_GroupsToString);
         
         return SUCCESS;
      }

      case MUIM_Popstring_Open: {
         if (!idata->Open) {
            STRPTR Groups;
            
            idata->Open = TRUE;
            idata->GL   = NN_GroupList(idata);
            
            DoMethod(o, SWM_NGList_StrToGroupsRO);
   
            DoMethod(idata->Str, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
                     o, 1, SWM_NGList_StrToGroups);
   
            DoMethod(idata->Str, MUIM_Notify, MUIA_String_Acknowledge, MUIV_EveryTime,
                     o, 1, SWM_NGList_StrAck);

            if ((Groups = SWGetStr(idata->Str, MUIA_String_Contents)) &&
                !idata->RestoreVal)
               if (idata->RestoreVal = AllocVec(strlen(Groups)+1, MEMF_ANY))
                  strcpy(idata->RestoreVal, Groups);
         }

         break;
      }

      case MUIM_Popstring_Close: {
         DoMethod(idata->Str, MUIM_KillNotifyObj, MUIA_String_Contents,    o);
         DoMethod(idata->Str, MUIM_KillNotifyObj, MUIA_String_Acknowledge, o);
         idata->Open = FALSE;

         FreeVec(idata->RestoreVal);
         idata->RestoreVal = NULL;
         
         break;
      }

      case SWM_NGList_Restore: {
         if (idata->RestoreVal) {
            set(idata->Str, MUIA_String_Contents, idata->RestoreVal);
            SetupROGrps(idata);
         }
         return SUCCESS;
      }
   }
   
   return DoSuperMethodA(cl, o, msg);
}
