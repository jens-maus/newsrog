/*****************************************************************************
 * Hook: Display                                                             *
 *****************************************************************************/

static __asm LONG SW_OList_DspFn(a2 char **array,
                                 a1 struct MUIS_Listtree_TreeNode *Node,
                                 a0 struct Hook *hook)
{
   putreg(REG_A4, (ULONG)hook->h_Data);  // use proper A4
   
   if (Node)
      *array++ = SWGetStr(Node->tn_User, SWA_ObjClassName),
      *array   = SWGetStr(Node->tn_User, SWA_Invocation_ObjName);
   else
      *array++ = GetS(SWS_Class),
      *array   = GetS(SWS_ObjName);

   return 0;
}

static struct Hook SW_OList_DspHook = {
        {NULL, NULL}, (void *)SW_OList_DspFn, NULL, NULL };


/*****************************************************************************
 * Rebuild the list if our source group changed                              *
 *****************************************************************************/

METHOD(SWM_OList_Refresh, SWP_OList_Obj *)
{
   set(o, MUIA_Listtree_Quiet, TRUE);

   DoMethod(o, MUIM_Listtree_Remove,
            MUIV_Listtree_Remove_ListNode_Root,
            MUIV_Listtree_Remove_TreeNode_All, 0);

   idata->OGroup = msg->o;     // save current object;

   DoMethod(idata->OGroup, SWM_Invocation_AddToListTree, o,
            MUIV_Listtree_Insert_ListNode_Root,
            MUIV_Listtree_Insert_PrevNode_Tail,
            SWV_AddToListTreeNoSelf);
   
   set(o, MUIA_Listtree_Quiet, FALSE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Remove a single element from the list                                     *
 *****************************************************************************/

METHOD(SWM_OList_ObjRem, Msg)
   SWDB_ASSERTNULL(idata->OGroup)
{
   Object *Active = (Object *)DoMethod(o, SWM_OList_ActiveObj),
          *Parent = SWGetObj(Active, MUIA_Parent);

   if (!Active || !Parent) SWDB_RET(NULL);

   // -- remove the node from the group object -------------------------------

   DoMethod(Parent, OM_REMMEMBER, Active);      // remove the node
   MUI_DisposeObject(Active);                   // dispose of the node

   SWDB_RET(SUCCESS);                           // return
} SDFT


/*****************************************************************************
 * Returns active object, or NULL if none                                    *
 *****************************************************************************/

METHOD(SWM_OList_ActiveObj, Msg)
   SWDB_ASSERTNULL(idata->OGroup)
{
   TreeNode *Active = SWGetType(o, MUIA_Listtree_Active, TreeNode *);

   SWDB_RET((ULONG)(Active==MUIV_Listtree_Active_Off ? NULL:Active->tn_User));
} SDFT


/*****************************************************************************
 * Exchange two items in the list                                            *
 *****************************************************************************/

METHOD(MUIM_List_Exchange,struct MUIP_List_Exchange *)
   SWDB_ASSERTNULL(idata->OGroup)
{
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Figure out where to insert a given element                                *
 *****************************************************************************/

static void GetInsPos(Object *o, SW_IDATA *idata, Object **InsObj, ULONG *Pos,
                      ULONG Flags)
{
   TreeNode *ListNode,
            *PrevNode = SWGetType(o, MUIA_Listtree_Active, TreeNode *);

   // -- this is all pretty revolting.  find a cleaner way, eh? --------------

   if ((LONG)PrevNode == MUIV_Listtree_Active_Off) {

      // -- if no active node, just insert at the root level
      
      *Pos      = -1;
      *InsObj   = idata->OGroup;
   } else if (SWGetUL(PrevNode->tn_User, SWA_Invocation_GroupObj) &&
              !(Flags & SWF_OList_SameLevel)) {

      // -- if active node is a group, insert at the end of it ---------------

      DoMethod(o, MUIM_Listtree_Open,        // open the node for convenience
               MUIV_Listtree_Open_ListNode_Active,
               MUIV_Listtree_Open_TreeNode_Active, 0);
         
      *InsObj   = PrevNode->tn_User;
      *Pos      = -1;
   } else {

      // -- otherwise, insert after current node -----------------------------

      ListNode = (TreeNode *)DoMethod(o, MUIM_Listtree_GetEntry, PrevNode,
                                   MUIV_Listtree_GetEntry_Position_Parent,0);

      *InsObj   = ListNode ? ListNode->tn_User : idata->OGroup;
      *Pos=DoMethod(*InsObj, SWM_FindChildPos, PrevNode->tn_User)+1;
   }
}


/*****************************************************************************
 * Insert a single element to the list                                       *
 *****************************************************************************/

METHOD(SWM_OList_InsertObject, SWP_OList_InsObj *)
   SWDB_ASSERTNULL(idata->OGroup)
   SWDB_ASSERTNULL(msg->o)
{
   Object *InsObj; ULONG Pos, rc;

   if (!msg->o) SWDB_RET(FAILURE);

   DoMethod(msg->o, SWM_Invocation_RemFromListTree, _app(o));
   
   GetInsPos(o, idata, &InsObj, &Pos, msg->Flags);

   set(o, MUIA_Listtree_Quiet, TRUE);
   rc = DoMethod(InsObj, SWM_InsertSingle, msg->o, Pos);
   set(o, MUIA_Listtree_Quiet, FALSE);

   // -- make new node the active node, for user convenience -----------------

   set(o, MUIA_Listtree_Active, getlent(msg->o, SWA_OList_TreeNode));
   
   SWDB_RET(rc);
} SDFT

   
/*****************************************************************************
 * Insert a single object of a given class                                   *
 *****************************************************************************/

METHOD(SWM_OList_InsertClass, SWP_OList_InsClass *)
   SWDB_ASSERTNULL(idata->OGroup)
   SWDB_ASSERTNULL(msg->List)
{
   Object *newobj = (Object *)DoMethod(msg->List, SWM_ClassList_MakeObj);
   
   // -- find class from class list, & create an object of it ----------------

   if (!newobj) SWDB_RET(NULL);

   SWDB_RET(DoMethod(o, SWM_OList_InsertObject, newobj, 0));
} SDFT


/*****************************************************************************
 * Clone active object                                                       *
 *****************************************************************************/

METHOD(SWM_OList_ObjClone, Msg)
   SWDB_ASSERTNULL(idata->OGroup)
{
   Object *Active = (Object *)DoMethod(o, SWM_OList_ActiveObj);

   if (!Active) SWDB_RET(NULL);

   SWDB_RET(DoMethod(o, SWM_OList_InsertObject, DoMethod(Active, SWM_Clone),
                     SWF_OList_SameLevel));
} SDFT


/*****************************************************************************
 * Save active object                                                        *
 *****************************************************************************/

METHOD(SWM_OList_ObjSave, Msg)
   SWDB_ASSERTNULL(idata->OGroup)
{
   Object *Active = (Object *)DoMethod(o, SWM_OList_ActiveObj);
   Object *Win    = SWGetObj(o, MUIA_WindowObject);
   
   if (!Active || !Win) SWDB_RET(NULL);

   DoMethod(Active,SWM_Root_M_OpenAssign,Win,SWF_DefObjDir,SWF_DefObjPattern);
   DoMethod(Active, SWM_Root_CheckErr, Win);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Insert object from disk                                                   *
 *****************************************************************************/

METHOD(SWM_OList_ObjIns, Msg)
   SWDB_ASSERTNULL(idata->OGroup)
{
   Object *Win = SWGetObj(o, MUIA_WindowObject),
          *InsObj, *NewObj;
   ULONG  Pos;

   if (!Win) SWDB_RET(NULL);

   GetInsPos(o, idata, &InsObj, &Pos, 0);

   if (NewObj = (Object *)DoMethod(InsObj, SWM_Root_M_Open,Win,
                                   SWF_DefObjDir,SWF_DefObjPattern))
      DoMethod(InsObj, SWM_InsertSingle, NewObj, Pos);

   DoMethod(InsObj, SWM_Root_CheckErr, Win);

   // -- make new node the active node, for user convenience -----------------

   set(o, MUIA_Listtree_Active, getlent(NewObj, SWA_OList_TreeNode));

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Reset object to defaults                                                  *
 *****************************************************************************/

METHOD(SWM_OList_ObjReset, Msg)
   SWDB_ASSERTNULL(idata->OGroup)
{
   Object *Active = (Object *)DoMethod(o, SWM_OList_ActiveObj);

   if (!Active) SWDB_RET(NULL);

   SWDB_RET(DoMethod(Active, SWM_Reset));
} SDFT


/*****************************************************************************
 * Edit active object                                                        *
 *****************************************************************************/

METHOD(SWM_OList_ObjEdit, Msg)
   SWDB_ASSERTNULL(idata->OGroup)
{
   Object *Active = (Object *)DoMethod(o, SWM_OList_ActiveObj);

   if (!Active) SWDB_RET(NULL);

   SWDB_RET(DoMethod(Active, SWM_Invocation_ShowEditUI, _app(o)));
} SDFT


/*****************************************************************************
 * Drag query request                                                        *
 *****************************************************************************/

METHOD(MUIM_DragQuery, struct MUIP_DragQuery *)
   SWDB_ASSERTNULL(idata->OGroup)
{
   Object *drop;
   if (msg->obj == o) SWDB_RET(DoSuperMethodA(cl, o, msg));

   if (!(drop = (Object *)DoMethod(msg->obj, SWM_Root_GetDragObj)))
      SWDB_RET(MUIV_DragQuery_Refuse);

   SWDB_RET((ULONG)(SWGetUL(drop, SWA_Invocation_ObjName) ?
                    MUIV_DragQuery_Accept : MUIV_DragQuery_Refuse));
} SDFT


/*****************************************************************************
 * Empty tree pointers                                                       *
 *****************************************************************************/

static void SWM_OList_Empty_I(Object *o, Object *Group)
{
   Object *obj; APTR state; List *l;

   // -- clear out this object`s contents ------------------------------------

   DoMethod(Group, SWM_Invocation_RemFromListTree, _app(o));

   SW_GETLH(Group, l);
   SW_LISTLOOP(l, state, obj) {
      SWM_OList_Empty_I(o, obj);            // if we got children, recurse
      DoMethod(Group, OM_REMMEMBER, obj);   // delete object
   }
}

   
/*****************************************************************************
 * Rebuild Group from ListTree contents                                      *
 *****************************************************************************/

static void SWM_OList_Rebuild_I(Object *o, Object *Group, TreeNode *Entries)
{
   LONG Pos;

   // -- add in contents from TreeNode list ----------------------------------

   for (Pos=0; ; Pos++) {
      TreeNode *Entry = (TreeNode *)
         DoMethod(o, MUIM_Listtree_GetEntry, Entries, Pos,
                  MUIV_Listtree_GetEntry_Flags_SameLevel);

      if (!Entry) break;
      
      SWDB_ASSERTNULL(Entry->tn_User);                     // can`t be NULL

      DoMethod(Group, OM_ADDMEMBER, Entry->tn_User);       // add to Group

      SWM_OList_Rebuild_I(o, Entry->tn_User, Entry);
   }

   // -- reset pointers to treenode ------------------------------------------

   nfset  (Group, MUIA_Group_Forward,      TRUE);
   setlent(Group, SWA_OList_TreeNode, Entries);
   setlent(Group, SWA_OList_ListObj,  o);
}


/*****************************************************************************
 * Drag drop report                                                          *
 *****************************************************************************/

METHOD(MUIM_DragDrop, struct MUIP_DragDrop *)
   SWDB_ASSERTNULL(idata->OGroup)
{
   // -- handle dragsort inside this object itself ---------------------------
   
   if (msg->obj == o) {
      ULONG rc = DoSuperMethodA(cl, o, msg);

      SWM_OList_Empty_I(o, idata->OGroup);
      
      SWM_OList_Rebuild_I(o, idata->OGroup,
                          (TreeNode *)MUIV_Listtree_GetEntry_ListNode_Root);

      SWDB_RET(rc);
   }

   // -- if another object, add clone of its object --------------------------

    DoMethod(o, SWM_OList_InsertObject,
       DoMethod((Object *)DoMethod(msg->obj, SWM_Root_GetDragObj),
                SWM_Clone), SWF_OList_SameLevel);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Get drag object                                                   *
 *****************************************************************************/

METHOD(SWM_Root_GetDragObj, Msg)
{
   SWDB_RET(DoMethod(o, SWM_OList_ActiveObj));
} SDFT
