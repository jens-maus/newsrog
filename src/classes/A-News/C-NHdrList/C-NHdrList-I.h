# ifndef C_NHDRLIST_INCLUDE_I
#  define C_NHDRLIST_INCLUDE_I

#  define SW_NListTrackAttrNum 4

#  include "Libraries/C-Root.h"
#  include "Libraries/S-Lists.h"
#  include "Libraries/C-Nhdrlist.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NHdrList
#  define SW_CNAME            "NHdrList"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NHdrList_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Hdr= 1,
      SWS_L_Val,
      SWS_L_Grp,
      SWS_L_Mode,

      SWS_L_UnsetHdr,
      SWS_L_UnsetVal,
      SWS_L_UnsetGrp,

      SWS_L_HdrLab,
      SWS_L_ValLab,
      SWS_L_GrpLab,
      SWS_L_ModeLab,
      SWS_L_ModeAny,
      SWS_L_ModeAll,
      SWS_B_Add,
      SWS_B_Del,

      SWS_H_List,
      SWS_H_Add,
      SWS_H_Del,
      SWS_H_Hdr,
      SWS_H_Val,
      SWS_H_Grp,
      SWS_H_Mode,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

#  define s_Hdr       idata->SW_NListTRA[0].Gad
#  define s_HdrAttr   idata->SW_NListTRA[0].ObjAttr
#  define s_Val       idata->SW_NListTRA[1].Gad
#  define s_ValAttr   idata->SW_NListTRA[1].ObjAttr
#  define s_Grp       idata->SW_NListTRA[2].Gad
#  define s_GrpAttr   idata->SW_NListTRA[2].ObjAttr
#  define c_Mode      idata->SW_NListTRA[3].Gad
#  define c_ModeAttr  idata->SW_NListTRA[3].ObjAttr
#  define c_ModeGAttr idata->SW_NListTRA[3].GadAttr
#  define c_ModeDef   idata->SW_NListTRA[3].UnsetVal

#  define SW_NList_DispHook MUI_DisposeObject(o)
#  define SW_NList_NewObj   MUI_NewObject(SWC_Attrval4, TAG_DONE);

   typedef struct SW_IDATA
   {
      SWP_NList_AttrDefs;
      TEXT ModeBuff[20];
   } SW_IDATA;

#  define NOTP                MUIM_Notify, MUIA_Pressed, FALSE

# endif // C_HDRLIST_INCLUDE_I
