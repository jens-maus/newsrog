
/*****************************************************************************
 * Function: delete sub-objects                                              *
 *****************************************************************************/

static void DelSub(Object *o)
{
   Object *obj; APTR state; List *l;

   SW_GETLH(o, l);
   SW_LISTLOOP(l, state, obj) {
      DoMethod(o, OM_REMMEMBER, obj);
      MUI_DisposeObject(obj);
   }
}


/*****************************************************************************
 * Function: Add subobjects                                                  *
 *****************************************************************************/

static Object *AddSub(Object *o, Object *o1)
{
   Object *obj; APTR state; List *l;

   SW_GETLH(o1, l);
   SW_LISTLOOP(l, state, obj) {
      Object *new = (Object *)DoMethod(obj, SWM_Clone);

      if (!new)
         return (Object *)SWERR(o, NULL, SWS_Err_ObjAlloc, 0, 0);

      DoMethod(o, OM_ADDMEMBER, (ULONG)new);
   }

   return o;
}

static ULONG InUse = FALSE;

static TagItem *FindCopyTags(void)
{
   if (!InUse) { InUse = TRUE; return atags; }
   return AllocVec(sizeof(TagItem)*MAXATTRS,MEMF_ANY);
}

static void FreeCopyTags(TagItem *t)
{
   if (t == atags) InUse = FALSE; else FreeVec(t);
}


/*****************************************************************************
 * Method: assign one object to another                                      *
 *****************************************************************************/

METHOD(SWM_Assign, SWP_Compare *)
   SWDB_ASSERTNULL(msg->o)
{
   TagItem *t;

   if (!InitChange(o, idata)) SWDB_RET(0);

   GetSemEx(o) {
      DelSub(o);
      DoMethod(msg->o, SWM_Root_GetTagList, t = FindCopyTags());
      SetAttrsA(o, t);
      FreeCopyTags(t);
      AddSub(o, msg->o);
   } RelSem(o);

   ExitChange(o, idata);
   
  Done:
   SWDB_RET((ULONG)o);
} SDFT


/*****************************************************************************
 * Method: assign one object to another, with type checking                  *
 *****************************************************************************/

METHOD(SWM_Root_AssignSafe, SWP_AssignSafe *)
   SWDB_ASSERTNULL(msg->o)
{
   ULONG rc;
   UBYTE *c1, *c2;
   
   if (!msg->o) SWDB_RET(NULL);
   
   if (!SW_SAMECLASS_SAFE(o, msg->o, c1, c2))
      rc = SWERR(o, NULL, SWS_Err_WrongObjType,
                 SWGetStr(msg->o, SWA_ObjClassName),
                 SWGetStr(o,      SWA_ObjClassName));
   else
      rc = DoMethod(o, SWM_Assign, msg->o);
   
   if (msg->DisposeFlag) MUI_DisposeObject(msg->o);
   SWDB_RET(rc);
} SDFT

