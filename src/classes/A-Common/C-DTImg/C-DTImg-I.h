# ifndef C_DTIMG_INCLUDE_I
#  define C_DTIMG_INCLUDE_I

#  include "Libraries/C-DTImg.h"
#  include "Libraries/C-ListSearch.h"
#  include <datatypes/PictureClass.h>
#  include <datatypes/PictureClassExt.h>
#  include <graphics/displayinfo.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_DTImg
#  define SW_CNAME            "DTImg"
#  define SW_SUPERPUB         MUIC_Area

#  include "S-Params.h"
#  include "Proto/DTImg_protos.h"

   // -- registration node for pictures, so reusing one doesn`t load it again.

   typedef struct _PicNode
   {
      Node    pn_Node;
      Object  *pn_PictureObj;
      Object  *pn_PictureObj_G;
      Object  *pn_PictureObj_M;
      Screen  *pn_Screen;      // it`s only remapped for one screen!
      ULONG   pn_UseCount;     // current usecount
   } PicNode;


   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      STRPTR       SWA_DTImg_FSpec;
      STRPTR       SWA_DTImg_Mask;
      STRPTR       SWA_DTImg_Path;
      UBYTE       *SWA_DTImg_Data;
      ULONG        SWA_DTImg_Del;
      ULONG        SWA_DTImg_Width;
      ULONG        SWA_DTImg_Height;
      ULONG        SWA_DTImg_Depth;
      ULONG        SWA_DTImg_Precision;
      ULONG        SWA_DTImg_MinXRes;
      ULONG        SWA_DTImg_Adjust;
      ULONG        SWA_DTImg_HasGhost;
      ULONG        SWA_DTImg_Ghosted;
      ULONG        SWA_DTImg_Sleep;
      struct List *SWA_DTImg_ImgCache;

      Object *DTObj, *DTObj_G, *DTObj_M;
      struct BitMapHeader *bmh, *bmh_g, *bmh_m;
      struct BitMap       *bm,  *bm_g, *bm_m;
      PicNode *ImgNode;
   } SW_IDATA;

#  define GHOST_SUFFIX "_G"

# endif // C_DTIMG_INCLUDE_I
