# ifndef C_NDF_HIDEHEADER_INCLUDE_I
#  define C_NDF_HIDEHEADER_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-HideHeader.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NArticleMgr.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_HideHeader
#  define SW_CNAME            "NDF_HideHeader"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  define SW_NListTrackAttrNum 1

#  include "S-Params.h"
#  include "Proto/NDF_HideHeader_protos.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/S-Lists.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Pattern = 1,
      SWS_H_Pattern,

      SWS_T_Pattern,
      SWS_L_TopTitle,

      SWS_B_Add,
      SWS_B_Del,
      SWS_B_SelAll,

      SWS_L_Unset,

      SWS_H_Add,
      SWS_H_Del,
      SWS_H_SelAll,
      SWS_H_List,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

#  define SW_NList_DispHook MUI_DisposeObject(o)
#  define SW_NList_NewObj \
          MUI_NewObject(SWC_Attrval, SWA_NDF_HideHeader_Pat, msg->UData, TAG_DONE)
#  define NL_ReadCond (idata->SWA_NDF_HideHeader_SaveVer > 0)

#  define NL_ListPostRead \
   if (idata->SWA_NDF_HideHeader_SaveVer == 0) { \
      idata->SWA_NDF_HideHeader_SaveVer = 1; \
      DoMethod(o, SWM_NList_Add, NULL, idata->SWA_NDF_HideHeader_Pattern); \
      set(o, SWA_NDF_HideHeader_Pattern, NULL); \
   }

#  define NL_Write_Pre idata->SWA_NDF_HideHeader_SaveVer = 1

#  define MaxPL 256
#  define DefPt "(From|Subject|Date):"

#  define s_Patt       idata->SW_NListTRA[0].Gad
#  define s_PattAttr   idata->SW_NListTRA[0].ObjAttr

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NList_AttrDefs;
      SWP_NDFilter_AttrDefs;

      STRPTR  SWA_NDF_HideHeader_Pattern;
      ULONG   SWA_NDF_HideHeader_SaveVer;

      ULONG   Parsed;
      ULONG   PatCount;
      STRPTR *PPat;

      Object  *UI, *title;
   } SW_IDATA;

#  define NOTP      MUIM_Notify, MUIA_Pressed, FALSE

#  define SELALLON  MUIM_List_Select, MUIV_List_Select_All, \
                      MUIV_List_Select_On, NULL

# endif // C_NDF_HIDEHEADER_INCLUDE_I
