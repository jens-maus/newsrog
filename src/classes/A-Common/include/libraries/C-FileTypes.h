# ifndef C_FILETYPES_INCLUDE
#   define C_FILETYPES_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"
#   include "Libraries/C-AttrVal.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10100)

   enum {
      SWA_FileTypes_FT     = SWA_Attrval_Attr,
      SWA_FileTypes_Patt   = SWA_Attrval_Val,
      SWA_FileTypes_Method = SWA_Attrval_Num
   };

   enum {
      SWM_FileTypes_FindType = SW_FIRSTTAG,    // file type from file
      SWM_FileTypes_Ins,                       // prompt, insert obj
      SWM_FileTypes_Save,                      // prompt, save obj
   };

   enum {
      SWT_FileTypes_FT_FSpec = SW_FIRSTTAG,    // file type from file
      SWT_FileTypes_FT_Mem,                    // file type from RAM
      SWT_FileTypes_FT_MemSize,                //   size of RAM buffer
   };

#  undef SW_FIRSTTAG

# endif
