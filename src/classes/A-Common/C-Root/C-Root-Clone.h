/*****************************************************************************
 * Generate a tag list of object's attrs                                     *
 *****************************************************************************/

METHOD(SWM_Root_GetTagList, SWP_Root_GetTagList *)
{
   SWDB_RET((ULONG)SWGenTagList(SW_ATTRMAP, (UBYTE *)idata, msg->Tags));
} SDFT


/*****************************************************************************
 * return a clone of this object                                             *
 *****************************************************************************/

METHOD(SWM_Clone, Msg)
{
   Object *newobj;

   newobj = MUI_NewObject(SWGetStr(o, SWA_ObjClassLib),
                          AppBaseTopObj(SWGetObj(o, SWA_AppBase_TopObj)),
                          TAG_DONE);
   
   if (!newobj) SWDB_RET(SWERR(o, NULL, SWS_Err_ObjAlloc, 0, 0));

   SWDB_RET((ULONG)DoMethod(newobj, SWM_Assign, o));
} SDFT
