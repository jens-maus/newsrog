# ifndef C_NDF_UUDECODE_INCLUDE
#  define C_NDF_UUDECODE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15400)

   enum {
      SWA_NDF_UUDecode_SaveDir = SW_FIRSTTAG,     // [SGI] save directory
      SWA_NDF_UUDecode_DispInline,                // [SGI] display inline gfx
      SWA_NDF_UUDecode_MakeDir,                   // [SGI] make dirs for save
      SWA_NDF_UUDecode_Prompt,                    // [SGI] OK/Cancel prompts
      SWA_NDF_UUDecode_DispSize,                  // [SGI] display image size
      SWA_NDF_UUDecode_DeleteTxt,                 // [SGI] marks deletion
   };

#  undef SW_FIRSTTAG   

# endif
