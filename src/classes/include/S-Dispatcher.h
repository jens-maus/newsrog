
#  if defined NOLIBS || defined LOADA4
   ClassT         *SW_CLASS  = NULL;
#  else
   extern ClassT  *ThisClass;
#  endif


# ifdef SW_CATALOG
  Catalog *SW_CATALOG = NULL;
# endif

/*****************************************************************************
 *                                                                           *
 *   Dispatcher function for this class                                      *
 *                                                                           *
 *   INPUTS:                                                                 *
 *                                                                           *
 *   OUTPUTS:                                                                *
 *                                                                           *
 *   AUTHOR:      Steve Koren                                                *
 *                                                                           *
 *   DATE:        Tue Dec 12 13:41:49 1995                                   *
 *                                                                           *
 *   ALGORITHM:                                                              *
 *                                                                           *
 *****************************************************************************/

__asm ULONG SW_DISPFN(DISPFNPROTO)
{
#  ifdef LOADA4
   putreg(REG_A6, (long)cl->cl_UserData);
#  endif
   
#  ifndef NOLIBS
   geta4();
#  endif

   // -- handle each possible method this class understands ------------------

#  if defined(SW_SETUP) || defined(SW_SET) || defined(SW_GET) || defined(SW_DISPOSE)

     switch (msg->MethodID) {
        
      // -- get the value of attributes --------------------------------------
  
#     ifdef SW_GET
        case OM_GET:
          return SWGet(cl, o, msg);
#     endif


      // -- allocate a new object --------------------------------------------
  
#     ifdef SW_SETUP
        case OM_NEW: {
           TagItem    *ti = ((opSet *)msg)->ops_AttrList;
           ULONG retval = (ULONG)DoSuperMethodA(cl, o, msg);

           if (retval) {
              if (retval=SWSetup(o, retval, cl,
                                 (SW_IDATA *)INST_DATA(cl, retval),ti)) {
#               if defined SW_SET && !defined SW_NOSETUPSET
                   TagItem    *tstate = ti;
                   while (ti = NextTagItem(&tstate))
                     SWSet(ti->ti_Tag,ti->ti_Data,cl,(Object *)retval, msg);
#               endif
#               ifdef SW_SETUP_POSTSET
                {
                   SW_IDATA *idata = (SW_IDATA *)INST_DATA(cl, retval);
                   SW_SETUP_POSTSET;
                }
#               endif
              }
           }
           
           return retval;
        }
#     endif

      // -- dispose of old object --------------------------------------------

#     ifdef SW_DISPOSE
        case OM_DISPOSE: {
           SWDispose(o, (SW_IDATA *)INST_DATA(cl, o));
           goto SuperMethod;
        }
#     endif
  
      // -- set or update attributes -----------------------------------------
        

#     ifdef SW_SET
        case OM_SET:
        case OM_UPDATE: {
           TagItem    *tstate, *ti;
#          ifdef SW_NONOTIFY
              BOOL    found = FALSE;
              BOOL    super = TRUE;
#          endif
           
           tstate = ti = ((opSet *)msg)->ops_AttrList;

           while (ti = NextTagItem(&tstate)) {
#             ifdef SW_NONOTIFY
                 if (ti->ti_Tag == SWA_ObjNoNotify)
                    { super=FALSE; continue; }
#             endif
#             ifdef SW_NONOTIFY
              found |=
#             endif
              SWSet(ti->ti_Tag, ti->ti_Data, cl, o, msg);
           }

#          ifdef SW_NONOTIFY
              if (!super) return found;
#          endif
        }

        goto SuperMethod;
#       endif
     }

#  endif

#  ifdef SW_METHOD
     return SWMethod(cl, o, msg);
#  endif

   // -- if we didn't understand the method, let the superclass have a go ----

 SuperMethod:
   return (ULONG)DoSuperMethodA(cl, o ,msg);
}


/*****************************************************************************
 *                                                                           *
 *   Auto startup and shutdown functions - used by SAS/C                     *
 *                                                                           *
 *   INPUTS:                                                                 *
 *                                                                           *
 *   OUTPUTS:                                                                *
 *                                                                           *
 *   AUTHOR:      Steve Koren                                                *
 *                                                                           *
 *   DATE:        Wed Dec 13 14:08:43 1995                                   *
 *                                                                           *
 *   ALGORITHM:   These functions initialize our class, and free it again    *
 *                when the library exits.                                    *
 *                                                                           *
 *****************************************************************************/

# ifdef NOLIBS
#   define SW_STARTPARAMS
# else
#   define SW_STARTPARAMS a6 Library *libbase
# endif

__asm __saveds int SW_STARTUP(SW_STARTPARAMS)
{
   static ULONG FirstTime = TRUE;
#  if defined NOLIBS || defined LOADA4
      SW_CLASS = MUI_CreateCustomClass(NULL, SW_SUPERPUB, SW_SUPERPRI,
                                       sizeof(SW_IDATA), SW_DISPFN);

      if (!SW_CLASS) return -1;

#  endif

   // -- open a catalog, and localize message strings ------------------------

#  ifdef SW_CATALOG
      SW_CATALOG = OpenCatalog(NULL, SW_CNAME ".catalog",
                               OC_Version, SW_CATVER, TAG_DONE);
#  endif

#  ifdef SW_CATSTR
      if (FirstTime)
         SWLocalizeStringArray(SW_CATALOG, SW_CATSTR, SW_MSGSTR);
#  endif

   // -- do any other setup code the user has in mind ------------------------

#  ifdef SW_USERSETUP
      SW_USERSETUP;
#  endif

#  ifdef LOADA4
   SW_GETICLASS(SW_CLASS)->cl_UserData = (ULONG)libbase;
#  endif

   FirstTime = FALSE;

   return 0;
}

void __saveds SW_SHUTDOWN(void)
{
#  if defined NOLIBS || defined LOADA4
   if (SW_CLASS) {
#  else
   if (ThisClass) {
#  endif
      // -- do any user cleanup activities -----------------------------------

#     ifdef SW_USERCLEANUP
         SW_USERCLEANUP;
#     endif

#     ifdef SW_CATALOG
         CloseCatalog(SW_CATALOG);             // close its catalog
#     endif

#     if defined NOLIBS || defined LOADA4
         MUI_DeleteCustomClass(SW_CLASS);      // delete MUI class, or
#     endif
   }

#  if defined NOLIBS || defined LOADA4
   SW_CLASS = NULL;                            // clear class pointer.
#  endif
}
