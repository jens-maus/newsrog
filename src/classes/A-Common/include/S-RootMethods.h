# define ft static __inline ULONG

//# define SW_DEBUG_READ

#  ifndef SWM_USERCOMPARE
#    define SWM_USERCOMPARE 0
#  endif

#  ifndef SWM_USERRESET
#    define SWM_USERRESET 
#  endif

#  ifndef SWM_USERREADINT
#    define SWM_USERREADINT 1
#  endif

#  ifndef SWM_USERWRITE
#    define SWM_USERWRITE 0
#  endif


/*****************************************************************************
 * Assign one object to another                                              *
 *****************************************************************************/

# if !defined SWM_ASSIGN_F && (defined SWM_USERASSIGN || defined SWM_USERPOSTASSIGN)
#   define SWM_ASSIGN_F

METHOD(SWM_Assign, SWP_Compare *)
   SWDB_ASSERTNULL(msg->o)
{
   ULONG rc;
#  ifndef SWM_NOIDATA2            //  user assign doesn't need this
      SW_IDATA *idata2 = INST_DATA(cl, msg->o);
#  endif
#  ifndef SWM_NOASSIGNAPP
      Object *App = SWGetObj(o, MUIA_ApplicationObject);
#  endif

#  ifdef SWM_USERASSIGN
   SWM_USERASSIGN;
#  endif

   rc = DoSuperMethodA(cl, o, msg);

#  ifdef SWM_USERPOSTASSIGN
   SWM_USERPOSTASSIGN;
#  endif

   SWDB_RET(rc);
} SDFT
# endif


/*****************************************************************************
 * Generate a tag list of object's attrs                                     *
 *****************************************************************************/

# ifndef SWM_GENTAG_F
#   define SWM_GENTAG_F

METHOD(SWM_Root_GetTagList, SWP_Root_GetTagList *)
{
   SWDB_RET(DoSuperMethod(cl, o, SWM_Root_GetTagList,
               SWGenTagList(SW_ATTRMAP, (UBYTE *)idata, msg->Tags)));
} SDFT
# endif


/*****************************************************************************
 * compare two objects                                                       *
 *****************************************************************************/

METHOD(SWM_Compare, SWP_Compare *)
   SWDB_ASSERTNULL(msg->o)
{
   SW_IDATA *idata2 = INST_DATA(cl, msg->o);

   if (SWCmpAttrs(SW_ATTRMAP, (UBYTE *)idata, (UBYTE *)idata2) ||
       (SWM_USERCOMPARE))
      SWDB_RET(1);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * reset object attributes to their defaults                                 *
 *****************************************************************************/

METHOD(SWM_Reset, Msg)
{
   SWInitAttrs(SW_ATTRMAP, (UBYTE *)idata, o);
   SWM_USERRESET;
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * read object from disk, of known type                                      *
 *****************************************************************************/

# ifndef SWM_READINT_F
METHOD(SWM_ReadInt, SWP_Read *)
   SWDB_ASSERTNULL(msg->fp)
{
   UBYTE ClassName[SW_MaxClassNameLen];

#  ifdef SW_DEBUG_READ
#    define SW_READINTERR(e) { Printf("%s\n", e); goto Error; }
#  else
#    define SW_READINTERR(e) goto Error;
#  endif
   
   if (SWReadAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, o))
      SW_READINTERR("Error reading attrs: " SW_CNAME);
      
   if (!(SWM_USERREADINT))
      SW_READINTERR("Error reading user attrs: " SW_CNAME);
   
   if (READCNAME(msg->fp, ClassName))
      SW_READINTERR("Error reading superclass: " SW_CNAME);
   
   if (!DoSuperMethodA(cl, o, msg))
      SW_READINTERR("Error reading superclass: " SW_CNAME);

#  ifdef SWM_USERREADPOST
      SWM_USERREADPOST;
#  endif

   SWDB_RET((ULONG)o);

#  undef SW_READINTERR

  Error:
   SWDB_RET(NULL);
} SDFT
# endif


/*****************************************************************************
 * write object to disk                                                      *
 *****************************************************************************/

# ifndef SWM_WRITE_F
METHOD(SWM_Write, SWP_Write *)
   SWDB_ASSERTNULL(msg->fp)
{
   if (SWWriteAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, SW_CLIB) ||
       (SWM_USERWRITE))
      SWDB_RET(FAILURE);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT
# endif

# undef ft
