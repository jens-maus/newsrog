
/*****************************************************************************
 * reset object attributes to their defaults                                 *
 *****************************************************************************/

METHOD(SWM_Reset, Msg)
{
   SWInitAttrs(SW_ATTRMAP, (UBYTE *)idata, o);
   SWDB_RET(DoMethod(o, SWM_SendMsg, msg, NULL));
} SDFT

