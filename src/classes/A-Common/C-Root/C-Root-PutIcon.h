/*****************************************************************************
 * Method: assign one object to another                                      *
 *****************************************************************************/

METHOD(SWM_Root_PutIcon, SWP_Root_PutIcon *)
   SWDB_ASSERTNULL(msg->o)
{
   UBYTE *olddt = NULL;
   struct DiskObject *dobj;
   Library *IconBase = OpenLibrary(ICONNAME, 39L);
   ULONG rc = SUCCESS;

   if (!IconBase) SWDB_RET(FAILURE);

   // -- don`t overwrite old icons, if they exist ----------------------------

   if (dobj = GetDiskObject(msg->Name)) goto Done;

   // -- get default project save icon ---------------------------------------

   if (!(dobj = GetDiskObjectNew(msg->Template))) {
      rc = SWERR(o, FAILURE, SWS_Err_ReadDefIcon, msg->Template, 0);
      goto Done;
   }

   // -- fix the default tool so it points to what we want -------------------
   
   olddt                = dobj->do_DefaultTool;
   dobj->do_DefaultTool = msg->Tool;

   // -- Write it to the project file ----------------------------------------
      
   if (!PutDiskObject(msg->Name, dobj))
      rc = SWERR(o, FAILURE, SWS_Err_WriteIcon, msg->Name, 0);

  Done:
   if (dobj) {
      if (olddt) dobj->do_DefaultTool = olddt;
      FreeDiskObject(dobj);
   }
   CloseLibrary(IconBase);

   SWDB_RET(rc);
} SDFT
