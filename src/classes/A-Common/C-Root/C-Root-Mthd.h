/*****************************************************************************
 *   Inline funcs for calling Init/Exit change for MUI groups                *
 *****************************************************************************/

static __inline ULONG InitChange(Object *o, SW_IDATA *idata)
{
   if (idata->ObjRecCount++ == 0)
      if (!DoMethod(o, MUIM_Group_InitChange)) {
         idata->ObjRecCount--;
         return 0;
      }
   
   return 1;
}

static __inline void ExitChange(Object *o, SW_IDATA *idata)
{
   if (--idata->ObjRecCount == 0)
      DoMethod(o, MUIM_Group_ExitChange);
}


METHOD(SWM_Root_Null, Msg)
{
   SWDB_RET(NULL);
}


/*****************************************************************************
 *   Include code for other methods (so they can be inlined)                 *
 *****************************************************************************/

# include "C-Root-Assign.h"
# include "C-Root-CheckErr.h"
# include "C-Root-ClearObjE.h"
# include "C-Root-Clone.h"
# include "C-Root-Compare.h"
# include "C-Root-CopyFile.h"
# include "C-Root-Exchange.h"
# include "C-Root-Insert.h"
# include "C-Root-LendAttr.h"
# include "C-Root-ListDoMethod.h"
# include "C-Root-Load.h"
# include "C-Root-MethodForward.h"
# include "C-Root-NthChild.h"
# include "C-Root-PutIcon.h"
# include "C-Root-Read.h"
# include "C-Root-ReadInt.h"
# include "C-Root-RequestFN.h"
# include "C-Root-Reset.h"
# include "C-Root-Save.h"
# include "C-Root-SaveLoad.h"
# include "C-Root-Sem.h"
# include "C-Root-SendMsg.h"
# include "C-Root-SetObjE.h"
# include "C-Root-SetOnce.h"
# include "C-Root-Setup.h"
# include "C-Root-SysCmd.h"
# include "C-Root-Write.h"
