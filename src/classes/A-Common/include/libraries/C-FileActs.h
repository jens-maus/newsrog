# ifndef C_FILEACTS_INCLUDE
#   define C_FILEACTS_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"
#   include "Libraries/C-FAct.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10100)

   enum {
      SWM_FileActs_DoAction = SW_FIRSTTAG,    // file type from file
      SWM_FileActs_GetActList,                // get list of actions for type
      SWM_FileActs_Ins,                       // ins from disk
      SWM_FileActs_Save,                      // save to disk
      SWM_FileActs_PostChange,                // post list change method
   };

   enum {
      SWT_FileActs_DoA_Type = SW_FIRSTTAG,
      SWT_FileActs_DoA_Action,
      SWT_FileActs_DoA_VarList,
      SWT_FileActs_DoA_VarObj,
      SWT_FileActs_GAL_Type,
   };

   SWM_P(SWP_FileActs_PostChange, ULONG Active);

#  undef SW_FIRSTTAG

# endif
