# define  AMM_MIN 20
# define  AMM_DEF 40
# include "S-DefAskMinMax.h"
# include "S-DrawIfNoChild.h"

static Object *sp(ULONG w)
{ return MUI_NewObject(MUIC_Rectangle, MUIA_Weight, w, TAG_DONE); }


/*****************************************************************************
 * Rem old object                                                            *
 *****************************************************************************/

static void RemObj(Object *o, SW_IDATA *idata, ULONG Dispose, ULONG Change)
{
   if (!idata->SWA_OBox_EditObj) return;

   if (Change)
      if (!DoMethod(o, MUIM_Group_InitChange)) return;
   
   DoMethod(o, OM_REMMEMBER, idata->AddGroup);
   if (Change) DoMethod(o, MUIM_Group_ExitChange);

   DoMethod(idata->AOGroup, OM_REMMEMBER, idata->SWA_OBox_EditObj);

   MUI_DisposeObject(idata->AddGroup);

   if (Dispose) {
      MUI_DisposeObject(idata->SWA_OBox_EditObj);
      nnset(idata->SWA_OBox_ContentObj, idata->SWA_OBox_ContentAttr, NULL);
   }
   
   idata->SWA_OBox_EditObj = idata->AddGroup = NULL;
}


/*****************************************************************************
 * Add new object                                                            *
 *****************************************************************************/

static Object *AddObj(Object *o, SW_IDATA *idata, Object *ao)
{
   if (!ao) return NULL;

   if (!DoMethod(o, MUIM_Group_InitChange)) return NULL;
   
   idata->AddGroup = VGroup,
      Child, sp(1),
      Child, idata->AOGroup = HGroup,
         Child, sp(1),
         Child, ao,
         Child, sp(1),
      End,
      Child, sp(1),
   End;
   
   DoMethod(o, OM_ADDMEMBER, idata->AddGroup);
   DoMethod(o, MUIM_Group_ExitChange);

   setlent(ao, SWA_Invocation_IsRoot, TRUE);

   nnset(idata->SWA_OBox_ContentObj, idata->SWA_OBox_ContentAttr, ao);

   return idata->SWA_OBox_EditObj = ao;
}


/*****************************************************************************
 * Insert a single object of a given class                                   *
 *****************************************************************************/

METHOD(SWM_OBox_InsertClass, SWP_OBox_InsClass *)
   SWDB_ASSERTNULL(msg->List)
{
   SWDB_RET(DoMethod(o, SWM_OBox_InsertObject,
                     (Object *)DoMethod(msg->List, SWM_ClassList_MakeObj)));

} SDFT


/*****************************************************************************
 * Update an object after attr change in editobj                             *
 *****************************************************************************/

METHOD(SWM_OBox_UpdateObject, SWP_OBox_InsObj *)
   SWDB_ASSERTNULL(msg->o)
{
   AddObj(o, idata, SWGetObj(idata->SWA_OBox_ContentObj,
                             idata->SWA_OBox_ContentAttr));
   SWDB_RET(0);
}


/*****************************************************************************
 * Insert a single object                                                    *
 *****************************************************************************/

METHOD(SWM_OBox_InsertObject, SWP_OBox_InsObj *)
   SWDB_ASSERTNULL(msg->o)
{
   RemObj(o, idata, TRUE, TRUE);
   if (!msg->o) SWDB_RET(NULL);   // remove only, upon NULL insert
   AddObj(o, idata, msg->o);

   SWDB_RET((ULONG)msg->o);
} SDFT


/*****************************************************************************
 * Drag query request                                                        *
 *****************************************************************************/

METHOD(MUIM_DragQuery, struct MUIP_DragQuery *)
   SWDB_ASSERTNULL(msg->obj)
{
   Object *drop;
   if (msg->obj == o) SWDB_RET(MUIV_DragQuery_Refuse);

   if (!(drop = (Object *)DoMethod(msg->obj, SWM_Root_GetDragObj)))
      SWDB_RET(MUIV_DragQuery_Refuse);

   SWDB_RET((ULONG)(SWGetUL(drop, SWA_Invocation_ObjName) ?
                    MUIV_DragQuery_Accept : MUIV_DragQuery_Refuse));
} SDFT


/*****************************************************************************
 * Drag drop report                                                          *
 *****************************************************************************/

METHOD(MUIM_DragDrop, struct MUIP_DragDrop *)
   SWDB_ASSERTNULL(msg->obj)
{
   DoMethod(_app(o), MUIM_Application_PushMethod, o, 2,
            SWM_OBox_InsertObject,
            (Object *)DoMethod((Object *)DoMethod(msg->obj, SWM_Root_GetDragObj),
                               SWM_Clone));
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Return a combination classlist/editbox using this object                  *
 *****************************************************************************/

METHOD(SWM_OBox_Editor, SWP_OBox_Editor *)
{
   Object *clist = MUI_NewObject(SWC_Classlist,InputListFrame,TAG_DONE),
          *ro, *bg,
          *cb = PopobjectObject,
                   MUIA_Popstring_Button, SimpleButton(msg->str),
                   MUIA_Popobject_Object, ListviewObject,
                   MUIA_Listview_Input, TRUE,
                   MUIA_Listview_List, clist, End,
                End,
          *eb = SimpleButton(GetS(SWS_L_Edit)),
          *sb = SimpleButton(GetS(SWS_L_Save)),
          *lb = SimpleButton(GetS(SWS_L_Load));

#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE
#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE 

   DoMethod(clist, DCLK, o,  2, SWM_OBox_InsertClass, clist);
   DoMethod(clist, DCLK, cb, 2, MUIM_Popstring_Close,  TRUE);
   DoMethod(eb,    NOTP, o,  1, SWM_OBox_Edit);
   DoMethod(sb,    NOTP, o,  1, SWM_OBox_Save);
   DoMethod(lb,    NOTP, o,  1, SWM_OBox_Load);

   set(o,  MUIA_ShortHelp, GetS(SWS_H_OBox));
   set(eb, MUIA_ShortHelp, GetS(SWS_H_Edit));
   set(sb, MUIA_ShortHelp, GetS(SWS_H_Save));
   set(lb, MUIA_ShortHelp, GetS(SWS_H_Load));

   bg = VGroup,
          Child, cb,
          Child, HGroup, Child, eb, Child, sb, Child, lb, End,
        End;
   
   ro = VGroup,
           Child, idata->SWA_OBox_VirtGroup ?
               ScrollgroupObject,
                  MUIA_Scrollgroup_Contents, VirtgroupObject,
                     VirtualFrame, Child, o,
                  End,
               End : o,
           Child, bg,
        End;

   nfset(o, MUIA_Draggable, !idata->SWA_OBox_VirtGroup);
   
   DoMethod(o, SWM_OBox_UpdateObject);

   SWDB_RET((ULONG)ro);
} SDFT



/*****************************************************************************
 * Method: Edit object                                                       *
 *****************************************************************************/

METHOD(SWM_OBox_Edit, Msg)
{
   if (!idata->SWA_OBox_EditObj) SWDB_RET(0);

   SWDB_RET((ULONG)DoMethod(idata->SWA_OBox_EditObj,
                            SWM_Invocation_ShowEditUI, _app(o)));
} SDFT


/*****************************************************************************
 * Method: Save object                                                       *
 *****************************************************************************/

METHOD(SWM_OBox_Save, Msg)
{
   if (!idata->SWA_OBox_EditObj) SWDB_RET(0);

   SWDB_RET((ULONG)DoMethod(idata->SWA_OBox_EditObj, SWM_Root_M_SaveAs,
                            SWT_Root_SL_InitDrawer,  SWF_DefObjDir,
                            SWT_Root_SL_InitPattern, SWF_DefObjPattern,
                            TAG_DONE));
} SDFT


/*****************************************************************************
 * Method: Load object                                                       *
 *****************************************************************************/

METHOD(SWM_OBox_Load, Msg)
{
   Object *lo = (Object *)DoMethod(o, SWM_Root_M_Open,
                                   SWT_Root_SL_InitDrawer,  SWF_DefObjDir,
                                   SWT_Root_SL_InitPattern, SWF_DefObjPattern,
                                   TAG_DONE);
   
   if (!lo) SWDB_RET(0);

   RemObj(o, idata, TRUE, TRUE);
   SWDB_RET((ULONG)AddObj(o, idata, lo));
} SDFT


/*****************************************************************************
 * Method: Get drag object                                                   *
 *****************************************************************************/

METHOD(SWM_Root_GetDragObj, Msg)
{
   SWDB_RET((ULONG)idata->SWA_OBox_EditObj);
} SDFT


/*****************************************************************************
 * Method: Hide UI                                                           *
 *****************************************************************************/

METHOD(MUIM_Cleanup, Msg)
   SWDB_ASSERTNULL(msg->List)
{
   ULONG rc = DoSuperMethodA(cl,o,msg);
   RemObj(o, idata, FALSE, FALSE);
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Add forwarded method                                              *
 *****************************************************************************/

METHOD(SWM_OBox_MethodForward, SWP_OBox_MethodForward *)
   SWDB_ASSERTNULL(msg->List)
{
   ULONG x;

   for (x=0; x<MAXMTHF; x++)
      if (!idata->Target[x]) break;

   if (x >= MAXMTHF) SWDB_RET(FAILURE);  // ooops, no space left

   idata->Target[x] = msg->Target;
   idata->Method[x] = msg->Method;
   
   SWDB_RET(SUCCESS);
} SDFT
