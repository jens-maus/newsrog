#  define ONODE(x) ((struct Node *)&_OBJECT(x)->o_Node)

/*****************************************************************************
 * move this object's subitems to a temporary holding list                   *
 *****************************************************************************/

static void SWM_ListToTmp(Object *o, List *TempList, LONG Pos1, LONG Pos2,
                          Object **Obj1, Object **Obj2)
{
   Object *obj, *last=NULL; APTR state;
   List   *l;

   if (Obj1) *Obj1 = NULL;
   if (Obj2) *Obj2 = NULL;
   
   SW_GETLH(o, l);                                 // copy items to temp list
   SW_LISTLOOP(l, state, obj) {
      DoMethod(o, OM_REMMEMBER, obj);
      DoMethod(obj, OM_ADDTAIL, TempList);
      if (Pos1-- == 0 && Obj1) *Obj1 = obj;
      if (Pos2-- == 0 && Obj2) *Obj2 = obj;
      last = obj;
   }

   if (Pos1 >= 0 && Obj1) *Obj1 = last;
   if (Pos2 >= 0 && Obj2) *Obj2 = last;
}


/*****************************************************************************
 * move the temp holding list items back to the real object                  *
 *****************************************************************************/

static void SWM_TmpToList(Object *o, List *TempList)
{
   Object *obj; APTR   state;

   SW_LISTLOOP(TempList, state, obj) {
      DoMethod(obj, OM_REMOVE);
      DoMethod(o, OM_ADDMEMBER, obj);              // add items back in
   }
}
