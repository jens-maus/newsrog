# ifndef C_FILEACTS_INCLUDE_I
#  define C_FILEACTS_INCLUDE_I

#  define SW_NListTrackAttrNum 11

#  include "Libraries/C-Root.h"
#  include "Libraries/C-AppBase.h"
#  include "Libraries/S-Lists.h"
#  include "Libraries/C-FileActs.h"
#  include "libraries/asl.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_FileActs
#  define SW_CNAME            "FileActs"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/FileActs_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_T_Type = 1,
      SWS_T_Action,
      SWS_T_Cmd,
      SWS_T_Args,
      SWS_T_Input,
      SWS_T_Output,
      SWS_T_Dir,
      SWS_T_Stack,
      SWS_T_Pri,

      SWS_L_L1,
      SWS_L_L2,
      
      SWS_H_List,

      SWS_L_Unset,

      SWS_L_Type,
      SWS_L_Action,
      SWS_L_Cmd,
      SWS_L_Args,
      SWS_L_Input,
      SWS_L_Output,
      SWS_L_Stack,
      SWS_L_Pri,
      SWS_L_Dir,
      SWS_L_ASync,
      SWS_L_CLI,
      SWS_L_ARexx,
      SWS_L_WB,
      SWS_T_RexxPort,
      SWS_T_RexxCmd,

      SWS_B_New,
      SWS_B_Del,
      SWS_B_Clone,
      SWS_B_Save,
      SWS_B_Ins,
      SWS_B_Sort,

      SWS_H_New,
      SWS_H_Del,
      SWS_H_Clone,
      SWS_H_Save,
      SWS_H_Ins,
      SWS_H_Sort,

      SWS_T_CmdAsl,

      SWS_Err_ActNotFound,
      SWS_Err_EmptyAct,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   enum {
      FT_MatchFNameNoCase = 0,
      FT_MatchFNameCase,
      FT_MatchFCBin,
      FT_MatchFCPattern,
      FT_MatchDNameNoCase,
      FT_MatchDNameCase,
      FT_AskExternal,
      FT_FTMAX,
   };

#  define s_Type       idata->SW_NListTRA[0].Gad
#  define s_TypeAttr   idata->SW_NListTRA[0].ObjAttr
#  define s_Action     idata->SW_NListTRA[1].Gad
#  define s_ActionAttr idata->SW_NListTRA[1].ObjAttr
#  define s_Cmd        idata->SW_NListTRA[2].Gad
#  define s_CmdAttr    idata->SW_NListTRA[2].ObjAttr
#  define s_Args       idata->SW_NListTRA[3].Gad
#  define s_ArgsAttr   idata->SW_NListTRA[3].ObjAttr
#  define s_In         idata->SW_NListTRA[4].Gad
#  define s_InAttr     idata->SW_NListTRA[4].ObjAttr
#  define s_Out        idata->SW_NListTRA[5].Gad
#  define s_OutAttr    idata->SW_NListTRA[5].ObjAttr
#  define s_Dir        idata->SW_NListTRA[6].Gad
#  define s_DirAttr    idata->SW_NListTRA[6].ObjAttr
#  define s_Pri        idata->SW_NListTRA[7].Gad
#  define s_PriAttr    idata->SW_NListTRA[7].ObjAttr
#  define s_PriDef     idata->SW_NListTRA[7].UnsetVal
#  define s_PriGAttr   idata->SW_NListTRA[7].GadAttr
#  define s_Stack      idata->SW_NListTRA[8].Gad
#  define s_StackAttr  idata->SW_NListTRA[8].ObjAttr
#  define s_StackDef   idata->SW_NListTRA[8].UnsetVal
#  define s_StackGAttr idata->SW_NListTRA[8].GadAttr
#  define s_ASync      idata->SW_NListTRA[9].Gad
#  define s_ASyncAttr  idata->SW_NListTRA[9].ObjAttr
#  define s_ASyncDef   idata->SW_NListTRA[9].UnsetVal
#  define s_ASyncGAttr idata->SW_NListTRA[9].GadAttr
#  define s_CmdT       idata->SW_NListTRA[10].Gad
#  define s_CmdTAttr   idata->SW_NListTRA[10].ObjAttr
#  define s_CmdTDef    idata->SW_NListTRA[10].UnsetVal
#  define s_CmdTGAttr  idata->SW_NListTRA[10].GadAttr

#  define SW_NList_Clone
#  define SW_NList_CmpCode  return CmpVars(o1, o2)
#  define SW_NList_DispHook MUI_DisposeObject(o)
#  define SW_NList_NewObj MUI_NewObject(SWC_FAct, TAG_DONE)
#  define SW_NList_Change_Post DoMethod(o, SWM_FileActs_PostChange, msg->Active);

   typedef struct SW_IDATA
   {
      SWP_NList_AttrDefs;
      SWP_AppBase_AttrDefs;
      STRPTR PatBuf;
      Object *Lab1, *Lab2;

      TEXT Stack[12], Pri[8];
   } SW_IDATA;

#  define  MaxPSize 1024

#  define NOTP        MUIM_Notify, MUIA_Pressed, FALSE

#  undef SWF_DefObjPattern
#  define SWF_DefObjPattern "MMAction.*"

# endif // C_FILEACTS_INCLUDE_I
