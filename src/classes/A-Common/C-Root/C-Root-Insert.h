/*****************************************************************************
 * insert an array of items into this object at a given position             *
 *****************************************************************************/

MRT SWM_DoInsert(Object *o, SW_IDATA *idata, LONG Count, LONG Pos,
                Object **Entries)
{
   List   TempList; Object *insobj;

   NewList(&TempList);
   
   if (!InitChange(o, idata)) SWDB_RET(0);

   if (Pos == MUIV_List_Insert_Bottom) Pos = LONG_MAX;

   SWM_ListToTmp(o, &TempList, Pos-1, 0, &insobj, NULL);

   while (Count)                                   // insert new objects
      Insert(&TempList, ONODE(Entries[--Count]), insobj?ONODE(insobj):NULL);

   SWM_TmpToList(o, &TempList);

   ExitChange(o, idata);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * insert list of objects at given position                                  *
 *****************************************************************************/

METHOD(SWM_Insert, SWP_Insert *)
{
   SWDB_RET(SWM_DoInsert(o, idata, msg->count, msg->pos, msg->Entries));
} SDFT


/*****************************************************************************
 * insert single object at given position                                    *
 *****************************************************************************/

METHOD(SWM_InsertSingle, SWP_InsertSingle *)
{
   SWDB_RET(SWM_DoInsert(o, idata, 1, msg->pos, &msg->Entry));
} SDFT


/*****************************************************************************
 * find ordinal position of child                                            *
 *****************************************************************************/

METHOD(SWM_FindChildPos, SWP_Compare *)
{
   APTR state; Object *obj; List *l; LONG pos=0;
   
   SW_GETLH(o, l);                                 // copy items to temp list

   SW_LISTLOOP(l, state, obj) {
      if (obj == msg->o) SWDB_RET((ULONG)pos);
      pos++;
   }

   SWDB_RET((ULONG)-1);
} SDFT
