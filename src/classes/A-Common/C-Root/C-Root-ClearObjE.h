
/*****************************************************************************
 * clear object error code                                                   *
 *****************************************************************************/

METHOD(SWM_ClearObjError, Msg)
{
   freelent(o, SWA_ObjError);
   freelent(o, SWA_ObjErrorText);
   freelent(o, SWA_ObjErrorParam1);
   freelent(o, SWA_ObjErrorParam2);

   SWDB_RET(SUCCESS);
} SDFT
