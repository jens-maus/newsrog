
/*****************************************************************************
 * set object error code                                                     *
 *****************************************************************************/

METHOD(SWM_SetObjError, SWP_ObjError *)
{
   SWDB_ERROR(msg->Text);

   if (getlent(o, SWA_ObjError)) SWDB_RET(msg->rc); // don`t overwrite errors

#  define UPD(a, v) if (v) setlent(o, (a), (v))

   UPD(SWA_ObjError,       msg->Error);
   UPD(SWA_ObjErrorText,   msg->Text);
   UPD(SWA_ObjErrorParam1, msg->Param1);
   UPD(SWA_ObjErrorParam2, msg->Param2);

   SWDB_RET(msg->rc);
} SDFT


/*****************************************************************************
 * method: adopt object error code                                           *
 *****************************************************************************/

METHOD(SWM_Root_AdoptObjError, SWP_Root_AdoptObjErr *)
{
   if (getlent(msg->o, SWA_ObjError) && !getlent(o, SWA_ObjError)) {
      DoMethod(o, SWM_SetObjError, 0, TRUE,
               getlent(msg->o, SWA_ObjErrorText),
               getlent(msg->o, SWA_ObjErrorParam1),
               getlent(msg->o, SWA_ObjErrorParam2));
      DoMethod(msg->o, SWM_ClearObjError);
   }

   SWDB_RET(msg->rc);
} SDFT
