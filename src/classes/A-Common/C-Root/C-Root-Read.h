/*****************************************************************************
 * read object from disk                                                     *
 *****************************************************************************/

METHOD(SWM_Read, SWP_Read *)
   SWDB_ASSERTNULL(msg->fp)
{
   UBYTE ClassName[SW_MaxClassNameLen];
   static UBYTE BadClass[SW_MaxClassNameLen];
   Object *newobj;

   if (!msg->fp) SWDB_RET(NULL);

   // -- read stored object class, and nuke the newline ----------------------
   
   if (READCNAME(msg->fp, ClassName))
      SWDB_RET(SWERR(o, NULL, SWS_Err_FileRead, 0, 0));

   SW_REMOVENL(ClassName);

   // -- make an object of that type -----------------------------------------

   newobj = MUI_NewObject(ClassName,
                          AppBaseTopObj(SWGetObj(o, SWA_AppBase_TopObj)),
                          TAG_DONE);

   if (!newobj)
      SWDB_RET(SWERR(o,NULL,SWS_Err_ObjAllocNew,strcpy(BadClass,ClassName),0));

   // -- call its read method  -----------------------------------------------
      
   if (DoMethod(newobj, SWM_ReadInt, msg->fp) == NULL) {
      MUI_DisposeObject(newobj);
      SWDB_RET(SWERR(o, NULL, SWS_Err_ObjRead, strcpy(BadClass,ClassName),0));
   }

   SWDB_RET((ULONG)newobj);
} SDFT

