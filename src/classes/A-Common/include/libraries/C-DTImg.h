# ifndef C_DTIMG_INCLUDE
#  define C_DTIMG_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-Root.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 2900)

   enum {
      SWA_DTImg_Data     = SW_FIRSTTAG,       // not yet implemented
      SWA_DTImg_Del,                          // [S I] T to del file on dispose
      SWA_DTImg_Width,                        // [ G ] image width
      SWA_DTImg_Height,                       // [ G ] image height
      SWA_DTImg_Depth,                        // [ G ] image depth
      SWA_DTImg_Precision,                    // [  I] mapping precision
      SWA_DTImg_FSpec,                        // [  I] image filespec
      SWA_DTImg_MinXRes,                      // [  I] min x res for large img
      SWA_DTImg_Adjust,                       // [  I] adjust sizes etc
      SWA_DTImg_HasGhost,                     // [  I] has ghost image
      SWA_DTImg_Ghosted,                      // [SGI] display ghosted image
      SWA_DTImg_ImgCache,                     // [  I] image cache list
      SWA_DTImg_Sleep,                        // [SGI] app sleep during Setup?
      SWA_DTImg_Mask,                         // [  I] mask filespec
      SWA_DTImg_Path,                         // [  I] search path for image
   };

   enum {
      SWM_DTImg_MakeImgCache = SW_FIRSTTAG,   // make image cache list
      SWM_DTImg_FreeImgCache,                 // free image cache list
   };

   SWM_P(SWP_DTImg_FreeImgCache, List *ImgCache);

   typedef struct List DTImgCache;

#  define DTImgObject MUI_NewObject(SWC_DTImg

#  undef SW_FIRSTTAG   

# endif
