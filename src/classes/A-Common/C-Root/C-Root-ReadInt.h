/*****************************************************************************
 * read object from disk, of known type                                      *
 *****************************************************************************/

METHOD(SWM_ReadInt, SWP_Read *)
   SWDB_ASSERTNULL(msg->fp)
{
   ULONG x, Children;

   if (!msg->fp) SWDB_RET(NULL);

   if (!InitChange(o, idata)) SWDB_RET(NULL);

   SWReadAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, o);
   if (SW_READ(msg->fp, Children)) SWDB_RET(NULL);

   for (x=Children; x; --x) {
      APTR newobj=(APTR)DoMethod(o, SWM_Read, msg->fp);

      if (!newobj) {
         DelSub(o);
         ExitChange(o, idata);
         SWDB_RET(NULL);
      } else
         DoMethod(o, OM_ADDMEMBER, (ULONG)newobj);
   }

   ExitChange(o, idata);

   SWDB_RET((ULONG)o);
} SDFT

