
/*****************************************************************************
 * find the Nth child object                                                 *
 *****************************************************************************/

METHOD(SWM_NthChild, SWP_NthChild *)
{
   APTR state; Object *obj; ULONG count = 0; ULONG rc = NULL;
   
   GetSemEx(o) {
      List *l;
      
      SW_GETLH(o, l);
      SW_LISTLOOP(l, state, obj)
         if (count++ == msg->Num) {
            rc = (ULONG)obj;
            break;
         }
   } RelSem(o);
   
   SWDB_RET(rc);
} SDFT

