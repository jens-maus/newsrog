
# ifndef C_ROOT_INCLUDE
#   define C_ROOT_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"

    // -- messages that this class understands -------------------------------
    
#   define SW_FIRSTTAG         (int)(TAG_USER + 0)
    
    enum {
       SWA_ObjChildCount = SW_FIRSTTAG,
       SWA_ObjError,                            // [SGI] object error
       SWA_ObjErrorText,                        // [SGI] object error text
       SWA_ObjErrorParam1,                      // [SGI] object error param1
       SWA_ObjErrorParam2,                      // [SGI] object error param1
       SWA_ObjNoNotify,                         // [S  ] no notifications
       SWA_ObjClassLib,                         // [ G ] class library name
       SWA_ObjClassName,                        // [ G ] class name
       SWA_ObjClassHash,                        // [ G ] class name hash value
       SWA_ObjPointer,                          // [ G ] object pointer itself
       SWA_ObjNoChildCopy,                      // [SGI] no copy children
       SWA_ObjClassVer,                         // [ G ] object class version
    };
    
    enum {
       SWM_Semaphore_Obtain = SW_FIRSTTAG,      // obtain semaphore for object
       SWM_Semaphore_ObtainShared,              // obtain shared semaphore
       SWM_Semaphore_Release,                   // release semaphore
       SWM_SendMsg,                             // send message to children
       SWM_Write,                               // Write object to file
       SWM_Read,                                // Read object from file
       SWM_ReadInt,                             // Read object w/ known type
       SWM_Save,                                // save object to a fspec
       SWM_Load,                                // load object from a fspec
       SWM_Clone,                               // Make a clone of this object
       SWM_Assign,                              // Assign one object to another
       SWM_Reset,                               // reset to defaults
       SWM_Compare,                             // test objects for equality
       SWM_NthChild,                            // find nth child
       SWM_Insert,                              // insert children
       SWM_InsertSingle,                        // insert one child
       SWM_SetObjError,                         // set   object error
       SWM_ClearObjError,                       // clear object error
       SWM_FindChildPos,                        // find ordinal pos of child
       SWM_NoForwardSet,                        // turn off MUIA_Group_Forward
       SWM_Root_RequestFileName,                // request filename
       SWM_Root_CheckErr,                       // check & display error
       SWM_Root_M_SaveAs,                       // request file & save object
       SWM_Root_M_Open,                         // open & load new object
       SWM_Root_M_OpenAssign,                   // open & load this object
       SWM_Root_CheckErrAll,                    // check error for children too
       SWM_Root_GetTagList,                     // get tag list for attrs
       SWM_Root_GetDragObj,                     // return drag/drop object
       SWM_Root_ListDoMethod,                   // send method to all in a list
       SWM_Root_SetLentAttr,                    // create/set lent attribute
       SWM_Root_FreeLentAttr,                   // free lent attribute
       SWM_Root_GetLentAttr,                    // get lent attribute value
       SWM_Root_AssignSafe,                     // Assign w/ class checking
       SWM_Root_M_Save,                         // save, ask before overwrite!
       SWM_Root_RequestFNData,                  // hook for saving requester data
       SWM_Root_MFForward,                      // -- fns to work around MUI
       SWM_Root_MFObjReg,                       // -- PushMethod/dispose problem.
       SWM_Root_MFObjUnReg,                     // -- hope for KillPushMethod!
       SWM_Root_SysCmd,                         // execute command
       SWM_Root_PutIcon,                        // make icon file
       SWM_Root_AdoptObjError,                  // adopt error messages
       SWM_Root_CopyFile,                       // copy a file to another
    };

    enum {
       SWT_Root_InFile = SW_FIRSTTAG,           // input filename
       SWT_Root_OutFile,                        // output filename
       SWT_Root_Cmd,                            // command to execute
       SWT_Root_Arg,                            // args for command
       SWT_Root_Stack,                          // stack size
       SWT_Root_Pri,                            // priority
       SWT_Root_Dir,                            // directory

       SWT_Root_CF_Src,                         // file cp source
       SWT_Root_CF_Trg,                         // file cp target
       SWT_Root_CF_Del,                         // file cp del source after cp
       SWT_Root_CF_BuffSize,                    // file cp buffer size
       SWT_Root_CF_MakePath,                    // file cp create target path

       SWT_Root_ASync,                          // for SysCmd
    };

    enum {
       SWV_ListDoMethod_App       = -222,       // replaced with app pointer
       SWV_ListDoMethod_Entry     = -223,       // replaced with entry pointer
       SWV_ListDoMethod_Win       = -224,       // replaced with window pointer
       SWV_ListDoMethod_Pos       = -225,       // replaced with list position
       SWV_ListDoMethod_Active    = -1,         // do for active list entry
       SWV_ListDoMethod_Selected  = -2,         // do for selected list entry
       SWV_ListDoMethod_All       = -3,         // do for all list entries
    };

    // -- method parameters --------------------------------------------------

    SWM_P(SWP_Read,           BPTR fp);
    SWM_P(SWP_Write,          BPTR fp);
    SWM_P(SWP_Save,           UBYTE *Name);
    SWM_P(SWP_Load,           UBYTE *Name);
    SWM_P(SWP_Compare,        Object *o);
    SWM_P(SWP_AssignSafe,     Object *o; ULONG DisposeFlag);
    SWM_P(SWP_SendMsg,        Msg msg; ULONG *Results);
    SWM_P(SWP_NthChild,       ULONG Num);
    SWM_P(SWP_InsertSingle,   Object *Entry;    LONG pos);
    SWM_P(SWP_Insert,         Object **Entries; LONG count; LONG pos);
    SWM_P(SWP_ObjError,       ULONG rc;ULONG Error;UBYTE *Text;
                              ULONG Param1; ULONG Param2);
    SWM_P(SWP_CheckErr,       Object *Win);
    SWM_P(SWP_Root_GetTagList,ULONG *Tags);
    SWM_P(SWP_Root_LendAttr,  ULONG Attr; ULONG Val);
    SWM_P(SWP_Root_FreeAttr,  ULONG Attr);
    SWM_P(SWP_Root_GetAttr ,  ULONG Attr);
    SWM_P(SWP_Root_MFObjReg,  Object *o);
    SWM_P(SWP_Root_MFObjUnReg,Object *o);
    SWM_P(SWP_Root_AdoptObjErr,Object *o; ULONG rc);
    SWM_P(SWP_Root_RequestFNData, UBYTE *Dir; UBYTE *File; UBYTE *Patt;
                             UBYTE *FSpec);
    SWM_P(SWP_Root_PutIcon,   STRPTR Template; STRPTR Name; STRPTR Tool);

#   define SWM_ObjClone(o)    ((Object *)DoMethod(o, SWM_Clone))

#   define SWERR(o,rc,e,p1,p2)DoMethod(o,SWM_SetObjError,rc,TRUE, GetS(e),p1,p2)
#   define SWERR_NM(o,rc,e,p1,p2)DoMethod(o,SWM_SetObjError,rc,TRUE,e,p1,p2)
#   define SWERR_P(p1,p2)     DoMethod(o,SWM_SetObjError,0, FALSE,NULL,   p1,p2)

#   define setlent(o, a, v)   DoMethod(o, SWM_Root_SetLentAttr,  a, v)
#   define setlentfwd(o,a,v)  DoMethod(o, SWM_Root_SetLentAttrR, a, v)
#   define getlent(o, a)      DoMethod(o, SWM_Root_GetLentAttr,  a)
#   define freelent(o, a)     DoMethod(o, SWM_Root_FreeLentAttr, a)

    enum {
       SWT_Root_SL_InitDrawer = SW_FIRSTTAG,  // initial drawer
       SWT_Root_SL_InitPattern,               // initial pattern
       SWT_Root_SL_Win,                       // window ptr
       SWT_Root_SL_AskOverwrite,              // TRUE to ask before overwriting
       SWT_Root_SL_SaveMode,                  // TRUE to save, else FALSE
       SWT_Root_SL_Title,                     // title  for requestor
       SWT_Root_SL_Buffer,                    // buffer for result
       SWT_Root_SL_Name,                      // filename
       SWT_Root_SL_BackSuffix,                // backup suffix, or NULL
       SWT_Root_SL_InitFile,                  // initial filename
    };

#   define SW_SAMECLASS(o1, o2) SWAttrEqStr(o1,o2,SWA_ObjClassLib)
#   define SW_ISCLASS(o1, c)    SWAttrEqValStr(o1,SWA_ObjClassLib,c)

#   define SW_SAMECLASS_SAFE(o1, o2, t1, t2) \
     (t1 = SWGetStr(o1, SWA_ObjClassLib), \
      t2 = SWGetStr(o2, SWA_ObjClassLib), \
      (t1 && t2 && !strcmp(t1, t2)))

#   define SW_ISCLASS_SAFE(o1, c, t1) \
     (t1 = SWGetStr(o1, SWA_ObjClassLib), (t1 && !strcmp(t1, c)))

#   define READCNAME(fp,cn) (FGets(fp, cn, SW_MaxClassNameLen) == NULL)

    // -- defaults -----------------------------------------------------------

#   define SWF_DefObjDir      "PROGDIR:Objects"
#   define SWF_DefProjDir     "PROGDIR:Projects"
#   define SWF_DefObjPattern  "Object.#?"
#   define SWF_DefProjPattern "Project.#?"

#   ifdef SWSEMAPHOREFNS
#      define GetSemEx(o) SWObtainSemaphore(o);
#      define GetSem(o)   SWObtainSemaphoreShared(o);
#      define RelSem(o)   SWReleaseSemaphore(o)
#   else
#      define GetSemEx(o)
#      define GetSem(o)
#      define RelSem(o)
#   endif

#  define SWP_Root_Attrs \
   SWAM_FS(SWA_ObjClassName, SWAM_NoLab, SWAM_Get, SW_CNAME), \
   SWAM_FS(SWA_ObjClassLib,  SWAM_NoLab, SWAM_Get, SW_CLIB)

#  include "Libraries/L-Support.h"

#  undef SW_FIRSTTAG

# endif
