# ifndef C_NAF_DECODE_INCLUDE
#  define C_NAF_DECODE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NAFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15800)

   enum {
      SWA_NAF_Decode_FSpec = SW_FIRSTTAG,      // [SGI] File to save to
      SWA_NAF_Decode_Mode,                     // [SGI] save mode
      SWA_NAF_Decode_MkDir,                    // [SGI] create directories
      SWA_NDF_Decode_DelFile,                  // [SGI] delete file afterwords
   };

   enum {
      SWM_NAF_Decode_Save,                     // [SGI] save decoded pieces
   };

   enum {
      SWV_NAF_Decode_Mode_Overwrite = 0,       // overwrite old files
      SWV_NAF_Decode_Mode_Skip,                // skip existing files
      SWV_NAF_Decode_Mode_Suffix,              // append numeric suffix
      SWV_NAF_Decode_Mode_COUNT,
   };

   SWM_P(SWP_NAF_Decode_Save,
         Object *Grp; Object *Art; ULONG Append);

#  undef SW_FIRSTTAG   

# endif
