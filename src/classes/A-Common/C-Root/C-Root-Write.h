/*****************************************************************************
 * write object to disk                                                      *
 *****************************************************************************/

METHOD(SWM_Write, SWP_Write *)
   SWDB_OBJMSG(__FUNC__, o)
{
   ULONG Children = SWGetUL(o, SWA_ObjChildCount);

   if (SWWriteAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, SW_CLIB))
      SWDB_RET(FAILURE);
   
   if (SW_WRITE(msg->fp, Children)) SWDB_RET(NULL);

   SWDB_RET(DoMethod(o, SWM_SendMsg, msg, NULL));  // write children
} SDFT
