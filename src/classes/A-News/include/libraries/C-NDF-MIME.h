# ifndef C_NDF_MIME_INCLUDE
#  define C_NDF_MIME_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15500)

   enum {
      SWA_NDF_MIME_SaveDir = SW_FIRSTTAG,       // [SGI] save directory
      SWA_NDF_MIME_DispInline,                  // [SGI] display inline gfx
      SWA_NDF_MIME_MakeDir,                     // [SGI] make dirs for save
      SWA_NDF_MIME_Prompt,                      // [SGI] OK/Cancel prompts
      SWA_NDF_MIME_DispSize,                    // [SGI] display image size
      SWA_NDF_MIME_DelTxtBin,                   // [SGI] text for bin delete
      SWA_NDF_MIME_NoDispTypes,                 // [SGI] types not to display
   };

#  undef SW_FIRSTTAG   

# endif
