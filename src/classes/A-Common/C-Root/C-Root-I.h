# ifndef C_ROOT_INCLUDE_I
#  define C_ROOT_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-AppBase.h"
#  include <dos/dostags.h>
#  include <dos/dosextens.h>


   // -- this classes names, etc ---------------------------------------------
  
#  define SW_CLIB             SWC_Root
#  define SW_CNAME            "Root"
#  define SW_SUPERPUB         MUIC_Group
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/Root_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_B_Ok = 1,
      SWS_B_Overwrite,
      SWS_L_SaveAs,
      SWS_L_Open,
      SWS_L_FileExists,
      SWS_L_WriteCanceled,
      
      SWS_Err_FileRead,
      SWS_Err_FileWrite,
      SWS_Err_FileOpenRead,
      SWS_Err_FileOpenWrite,
      SWS_Err_FileFormat,
      SWS_Err_ObjRead,
      SWS_Err_ObjAllocNew,
      SWS_Err_ObjAlloc,
      SWS_Err_TooManyChilden,
      SWS_Err_NullFileName,
      SWS_Err_Malloc,
      SWS_Err_WrongObjType,
      SWS_Err_OpenInput,
      SWS_Err_OpenOutput,
      SWS_Err_RunCmd,
      SWS_Err_Lockdir,
      SWS_Err_ReadDefIcon,
      SWS_Err_WriteIcon,
      SWS_Err_MakeDir,
      
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data -----------------------------------------

   typedef struct _LentAttr { ULONG Attr; ULONG Val; } LentAttr;
   
#  ifdef SW_IDATA
   typedef struct SW_IDATA
   {
#     ifdef SWSEMAPHOREFNS
         SignalSemaphore SWRootSemaphore;   // semaphore for object
#     endif
      UWORD     ObjRecCount;                // recursion count for changes
      UWORD     LentAttrCount;              // count of lent attrs allocated
      LentAttr *LentAttrs;                  // lent attributes
      ULONG     ChildCount;                 // count of children
   } SW_IDATA;
#  endif

#  define SAVE_COOKIE "SWMOBJFILE"
#  define MAXATTRS 128

   enum {
      LA_Unused  = 0,
      LA_Used    = -1,
      LA_Inc     = 2,
   };

# endif // C_ROOT_INCLUDE_I
