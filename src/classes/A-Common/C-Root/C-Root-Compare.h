/*****************************************************************************
 * compare two objects                                                       *
 *****************************************************************************/

METHOD(SWM_Compare, SWP_Compare *)
{
   Object *obj1, *obj2;
   ULONG compare = 0;
   SW_IDATA *idata2 = INST_DATA(cl, msg->o);
   APTR state1, state2;
   List *l1, *l2;

   if (SWCmpAttrs(SW_ATTRMAP, (UBYTE *)idata, (UBYTE *)idata2))
      SWDB_RET(1);
   
   SW_GETLH(o,l1); SW_GETLH(msg->o,l2);
   if (!l1 && !l2) SWDB_RET(0);
   if (!l1 || !l2) SWDB_RET(1);
   
   state1 = l1->lh_Head, state2 = l2->lh_Head;

   GetSem(o) {
      while (1) {
         obj1  = NextObject(&state1), obj2  = NextObject(&state2);
         
         if (!obj1 && !obj2) break;
         
         if (!obj1 || !obj2 || DoMethod(obj1, SWM_Compare, obj2))
            { compare = 1; break; }
      }
   } RelSem(o);
   
   SWDB_RET(compare);
} SDFT

