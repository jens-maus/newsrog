# ifndef C_NDF_SUBSTITUTE_INCLUDE_I
#  define C_NDF_SUBSTITUTE_INCLUDE_I

#  define SW_NListTrackAttrNum 4

#  include "S-ListImages.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-Substitute.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-DTImg.h"
#  include "Libraries/S-Lists.h"
#  include <libraries/ASL.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_Substitute
#  define SW_CNAME            "NDF_Substitute"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_Substitute_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Case = 1,
      SWS_L_Wild,
      SWS_L_Longest,
      SWS_L_Pattern,
      SWS_L_Replace,
      SWS_L_Image,
      SWS_L_Mask,

      SWS_L_TopTitle,

      SWS_B_Add,
      SWS_B_Del,
      SWS_B_SelAll,

      SWS_L_Unset,

      SWS_BL_Replace,
      SWS_BL_Img,
      SWS_BL_Mask,
      
      SWS_H_Add,
      SWS_H_Del,
      SWS_H_SelAll,

      SWS_H_List,
      SWS_H_Pattern,
      SWS_H_Replace,
      SWS_H_Image,
      SWS_H_Mask,
      
      SWS_LASTSTR,
   };

   enum {
      SWV_Img_True = 0,
      SWV_Img_False,
      SWV_Img_MAX,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      PEN_Substitute = 0,
      PEN_Max
   };

#  define SW_NList_SelChange
#  define SW_NList_ClickCol
#  define SW_NList_DispHook MUI_DisposeObject(o)

#  define SW_NList_NewObj MUI_NewObject(SWC_Attrval4, \
                                        SWA_NDF_Substitute_Case, TRUE, \
                                        TAG_DONE)

#  define s_Patt       idata->SW_NListTRA[0].Gad
#  define s_PattAttr   idata->SW_NListTRA[0].ObjAttr
#  define s_Repl       idata->SW_NListTRA[1].Gad
#  define s_ReplAttr   idata->SW_NListTRA[1].ObjAttr
#  define s_Img        idata->SW_NListTRA[2].Gad
#  define s_ImgAttr    idata->SW_NListTRA[2].ObjAttr
#  define s_Mask       idata->SW_NListTRA[3].Gad
#  define s_MaskAttr   idata->SW_NListTRA[3].ObjAttr

   typedef struct FData
   {
      struct PatInfo {
         ULONG  EndPat;
         ULONG  Longest;
         STRPTR Repl;
         STRPTR Img;
         STRPTR Mask;
         struct  PatBounds pb;
      } *PatInfo;
   } FData;

   typedef struct ADDDATA {
      TxtBuff Img, Mask;
   } AddData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;
      SWP_NList_AttrDefs;
      SWP_Img_AttrDefs;
 
      Object  *UI, *title;
   } SW_IDATA;

#  define ImgVar    "${Img}"

#  define NOTP      MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE
#  define SELALLON  MUIM_List_Select, MUIV_List_Select_All, \
                      MUIV_List_Select_On, NULL

# endif // C_NDF_SUBSTITUTE_INCLUDE_I
