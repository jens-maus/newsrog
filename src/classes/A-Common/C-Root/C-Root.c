# include "C-Root-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_Root

// -- root attribute map -----------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_FI(SWA_ObjClassVer, SWAM_NoLab, SWAM_Get, 1),
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "*_Ok",
   "_Ok|*_Cancel",
   "Save File",
   "Open File",
   "OK to overwrite '%s'?",
   "Write canceled.",
   
   "File read failed: %s",
   "File write failed: %s",
   "Cannot open file for reading: %s",
   "Cannot open file for writing: %s",
   "File is unknown format: %s",
   "Unable to read object contents for class: %s",
   "Unable to allocate object of class: %s",
   "Object allocation failed",
   "Too many sub-objects: %ld",
   "Null filename given.",
   "Memory allocation failed.",
   "Object type incorrect:\nFound '%s' (expected '%s')",
   "Error opening input file: '%s'",
   "Error opening output file: '%s'",
   "Error running command",
   "Unable to lock directory: '%s'",
   "Unable to read default icon: '%s'",
   "Unable to write icon: '%s'",
   "Error creating directory: '%s'",
   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

TagItem *atags = NULL;

# define  SW_NONOTIFY

# define SW_USERSETUP     atags = AllocVec(sizeof(TagItem)*MAXATTRS,MEMF_ANY);
# define SW_USERCLEANUP   FreeVec(atags);
# define SETUPCUST   {static ULONG I=0;set((Object *)retval,MUIA_UserData,I++);}
# define DISPOSECUST FreeVec(idata->LentAttrs);


# include "S-DefSet.h"
# include "S-DefSetup.h"
# include "S-DefDispose.h"
# include "C-Root-Mthd.h"


/*****************************************************************************
 * Handle the get methods for attributes this class understands.             *
 *****************************************************************************/

#define SW_GET
static __asm __inline ULONG SWGet(GETFNPROTO)
   SDST(ULONG)
   SWP_XXX(cl, o, msg)
   SWDB_ASSERTCLFN()
{
   LONG x;
   ULONG Res       = 0;
   ULONG Attr      = ((opGet *)msg)->opg_AttrID;
   ULONG *Stor     = (ULONG *)((opGet *)msg)->opg_Storage;
   SW_IDATA *idata = (SW_IDATA *)INST_DATA(cl, o);

   switch (Attr) {
     case SWA_ObjChildCount: {
        Object *obj; APTR state; List *l;

        SW_GETLH(o, l);
        SW_LISTLOOP(l, state, obj) Res++;

        goto Found;
     }

     case SWA_ObjClassHash: {
        UBYTE *c = SWGetStr(o, SWA_ObjClassName);

        for (Res=47, x=0; c[x]; x++) Res += (x+7)*c[x];

        goto Found;
     }

     case SWA_ObjPointer: Res = (ULONG)o; goto Found;
   }

   if (SWGetAttr(SW_ATTRMAP, (UBYTE *)idata, Attr, &Res))
      SWDB_RET(1);

   SWDB_RET(DoSuperMethodA(cl, o, msg));

  Found:
   if (Stor) *Stor = Res;
   SWDB_RET(1);
} SDFT

# define SW_NOROOTMETHODS
# define SW_Methods                                  \
  SW_DO_MF(SWM_ReadInt);                             \
  SW_DO_MF(SWM_Read);                                \
  SW_DO_MF(SWM_Write);                               \
  SW_DO_MF(SWM_Save);                                \
  SW_DO_MF(SWM_Load);                                \
  SW_DO_MF(SWM_SendMsg);                             \
  SW_DO_MF(SWM_Clone);                               \
  SW_DO_MF(SWM_Assign);                              \
  SW_DO_MF(SWM_Reset);                               \
  SW_DO_MF(SWM_Compare);                             \
  SW_DO_MF(SWM_NthChild);                            \
  SW_DO_MF(SWM_Insert);                              \
  SW_DO_MF(SWM_InsertSingle);                        \
  SW_DO_MF(SWM_SetObjError);                         \
  SW_DO_MF(SWM_Root_AdoptObjError);                  \
  SW_DO_MF(SWM_ClearObjError);                       \
  SW_DO_MF(SWM_FindChildPos);                        \
  SW_DO_MF(SWM_NoForwardSet);                        \
  SW_DO_MF(SWM_Root_RequestFileName);                \
  SW_DO_MF(SWM_Root_CheckErr);                       \
  SW_DO_MF(SWM_Root_CheckErrAll);                    \
  SW_DO_MF(SWM_Root_GetTagList);                     \
  SW_DO_MF(SWM_Root_M_Save);                         \
  SW_DO_MF(SWM_Root_M_SaveAs);                       \
  SW_DO_MF(SWM_Root_M_Open);                         \
  SW_DO_MF(SWM_Root_M_OpenAssign);                   \
  SW_DO_MF(SWM_Root_ListDoMethod);                   \
  SW_DO_MF(SWM_Root_SetLentAttr);                    \
  SW_DO_MF(SWM_Root_FreeLentAttr);                   \
  SW_DO_MF(SWM_Root_GetLentAttr);                    \
  SW_DO_MF(SWM_Root_AssignSafe);                     \
  SW_DO_MF(SWM_Root_MFForward);                      \
  SW_DO_MF(SWM_Root_MFObjReg);                       \
  SW_DO_MF(SWM_Root_MFObjUnReg);                     \
  SW_DO_MF(SWM_Root_SysCmd);                         \
  SW_DO_MF(SWM_Root_PutIcon);                        \
  SW_DO_MF(SWM_Root_CopyFile);                       \
  SW_DO_M (SWM_Root_GetDragObj,    SWM_Root_Null_F); \
  SW_DO_M (SWM_Root_RequestFNData, SWM_Root_Null_F); \


# include "S-DefMethod.h"
# include "S-Dispatcher.h"
