# ifndef C_NSERVERLIST_INCLUDE_I
#  define C_NSERVERLIST_INCLUDE_I

#  define SWV_NList_TrackedMax SWV_NNews_MaxOpenGroups
#  define SW_NListTrackAttrNum 8

#  include "S-ListImages.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NServerList.h"
#  include "Libraries/C-NTCPList.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/S-Lists.h"
#  include "Libraries/C-ListCol.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NServerList
#  define SW_CNAME            "NServerList"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NServerList_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_None = 1,
      SWS_L_Unknown,
      SWS_L_Default,
      SWS_L_Server,
      SWS_L_CSuccess,
      SWS_L_CFail,
      SWS_L_CPercent,
      SWS_L_NGroups,
      SWS_L_CxTimeout,
      SWS_L_RetryDelay,
      SWS_L_MaxRetries,
      SWS_L_Node,
      SWS_L_PostOK,
      SWS_L_SType,

      SWS_L_Yes,
      SWS_L_No,

      SWS_B_Def,

      SWS_L_TopTitle,
      SWS_B_Add,
      SWS_B_Del,
      SWS_B_Sort,
      SWS_B_Reset,
      SWS_B_MkDef,
      SWS_B_RdEnv,
      SWS_L_CxTO,
      SWS_L_CxRT,
      SWS_L_CxMR,
      SWS_L_AUser,
      SWS_L_APass,
      SWS_L_Type,
      SWS_L_TypeNNTP,
      SWS_L_TypeFTP,
      SWS_L_Port,

      SWS_H_Add,
      SWS_H_Del,
      SWS_H_Sort,
      SWS_H_Reset,
      SWS_H_MkDef,
      SWS_H_RdEnv,
      SWS_H_CxTO,
      SWS_H_CxRT,
      SWS_H_CxMR,
      SWS_H_AuthUser,
      SWS_H_AuthPass,
      SWS_H_Type,
      SWS_H_Port,

      SWS_H_ListCol,
      SWS_H_Clip,

      SWS_H_Def,

      SWS_Err_NoServ,
      SWS_Err_NoEnv,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

#  define s_Host       idata->SW_NListTRA[0].Gad
#  define s_HostAttr   idata->SW_NListTRA[0].ObjAttr
#  define s_CxTO       idata->SW_NListTRA[1].Gad
#  define s_CxTOAttr   idata->SW_NListTRA[1].ObjAttr
#  define s_CxTODef    idata->SW_NListTRA[1].UnsetVal
#  define s_CxTOGAttr  idata->SW_NListTRA[1].GadAttr
#  define s_CxRT       idata->SW_NListTRA[2].Gad
#  define s_CxRTAttr   idata->SW_NListTRA[2].ObjAttr
#  define s_CxRTDef    idata->SW_NListTRA[2].UnsetVal
#  define s_CxRTGAttr  idata->SW_NListTRA[2].GadAttr
#  define s_CxMR       idata->SW_NListTRA[3].Gad
#  define s_CxMRAttr   idata->SW_NListTRA[3].ObjAttr
#  define s_CxMRDef    idata->SW_NListTRA[3].UnsetVal
#  define s_CxMRGAttr  idata->SW_NListTRA[3].GadAttr
#  define s_User       idata->SW_NListTRA[4].Gad
#  define s_UserAttr   idata->SW_NListTRA[4].ObjAttr
#  define s_Pass       idata->SW_NListTRA[5].Gad
#  define s_PassAttr   idata->SW_NListTRA[5].ObjAttr
#  define s_Type       idata->SW_NListTRA[6].Gad
#  define s_TypeAttr   idata->SW_NListTRA[6].ObjAttr
#  define s_TypeDef    idata->SW_NListTRA[6].UnsetVal
#  define s_TypeGAttr  idata->SW_NListTRA[6].GadAttr
#  define s_Port       idata->SW_NListTRA[7].Gad
#  define s_PortAttr   idata->SW_NListTRA[7].ObjAttr
#  define s_PortGAttr  idata->SW_NListTRA[7].GadAttr

#  define SW_NList_CmpAttr  SWA_NServer_Host
#  define SW_NList_DispHook MUI_DisposeObject(o)

#  define SW_NList_ClickCol_Pre if (Col == 0) \
                                   DoMethod(o, SWM_NServerList_ResetDefault)

#  define SW_NList_NewObj MkInsObj(idata, NULL)

   enum {
      SWV_Img_True = 0,
      SWV_Img_False,
      SWV_Img_MAX,
   };

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NList_AttrDefs;
      SWP_Img_AttrDefs;

      Object *FmtSlider;
      Object *SWA_NServerList_ListCol;

      UBYTE   v2[16], v3[16], v4[16], v5[16], v8[16], node[48];
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

# endif // C_NSERVERLIST_INCLUDE_I
