/*****************************************************************************
 * send message to all children objects                                      *
 *****************************************************************************/

METHOD(SWM_SendMsg, SWP_SendMsg *)
{
   ULONG pos = 0, rc, result = 0; APTR state;
   Object *obj;
   
   GetSem(o) {
      List   *l;

      SW_GETLH(o, l);
      SW_LISTLOOP(l, state, obj) {
         rc      = DoMethodA(obj, msg->msg);
         result += rc;
      
         if (msg->Results) (msg->Results)[pos++] = rc;
      }
      
   } RelSem(o);
   
   SWDB_RET(result);
} SDFT
