# ifndef C_FILETYPES_INCLUDE_I
#  define C_FILETYPES_INCLUDE_I

#  define SW_NListTrackAttrNum 3

#  include "Libraries/C-Root.h"
#  include "Libraries/C-AppBase.h"
#  include "Libraries/S-Lists.h"
#  include "Libraries/C-FileTypes.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_FileTypes
#  define SW_CNAME            "FileTypes"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/FileTypes_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_FType = 1,
      SWS_L_FMeth,
      SWS_L_FPatt,

      SWS_H_List,

      SWS_L_UnsetType,
      SWS_L_UnsetPatt,

      SWS_L_EType,
      SWS_L_EPatt,
      SWS_L_EMethod,

      SWS_L_MatchFNameNoCase,
      SWS_L_MatchFNameCase,
      SWS_L_MatchFCBin,
      SWS_L_MatchFCPattern,
      SWS_L_MatchDNameNoCase,
      SWS_L_MatchDNameCase,
      SWS_L_AskExternal,

      SWS_FT_0,
      SWS_FT_1,
      SWS_FT_2,
      SWS_FT_3,
      SWS_FT_4,
      SWS_FT_5,
      SWS_FT_6,
      
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

#  define s_FT        idata->SW_NListTRA[0].Gad
#  define s_FTAttr    idata->SW_NListTRA[0].ObjAttr
#  define s_Pat       idata->SW_NListTRA[1].Gad
#  define s_PatAttr   idata->SW_NListTRA[1].ObjAttr
#  define s_Meth      idata->SW_NListTRA[2].Gad
#  define s_MethAttr  idata->SW_NListTRA[2].ObjAttr
#  define s_MethDef   idata->SW_NListTRA[2].UnsetVal
#  define s_MethGAttr idata->SW_NListTRA[2].GadAttr


#  define SW_NList_Clone
#  define SW_NList_CmpCode  return CmpVars(o1, o2)
#  define SW_NList_DispHook MUI_DisposeObject(o)

#  define SW_NList_NewObj MUI_NewObject(SWC_Attrval, TAG_DONE)

   enum {
      SWV_Filetypes_LineMax = 512,
      FT_PatLen             = (512*2+2),
   };

#  define KW_Pos     "Offset:"
#  define KWS_Pos    (sizeof(KW_Pos)-1)

   enum {
      SWV_Filetypes_Unknown = 0,
      SWV_Filetypes_File,
      SWV_Filetypes_Dir,
   };

   typedef struct SW_IDATA
   {
      SWP_NList_AttrDefs;
      SWP_AppBase_AttrDefs;

      TEXT    Buff[FT_PatLen];

      STRPTR  CycGads[FT_FTMAX+1];
   } SW_IDATA;

#  define NOTP        MUIM_Notify, MUIA_Pressed, FALSE

# endif // C_FILETYPES_INCLUDE_I
