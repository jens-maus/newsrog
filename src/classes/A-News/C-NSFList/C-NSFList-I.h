# ifndef C_NSFLIST_INCLUDE_I
#  define C_NSFLIST_INCLUDE_I

#  define SW_NListTrackAttrNum 2
#  define SWV_NList_TrackedMax 24

#  include "S-ListImages.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NSFList.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-ClassList.h"
#  include "Libraries/S-Lists.h"
#  include <MUI/TextEditor_mcc.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSFList
#  define SW_CNAME            "NSF_And-Or"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSFList_protos.h"
#  include "Libraries/C-NSFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_B_Add = 1,
      SWS_B_Del,
      SWS_B_Edit,
      SWS_B_Clone,

      SWS_LB_Add,
      SWS_LB_Close,

      SWS_L_Desc,
      SWS_L_Grp,

      SWS_L_Any,
      SWS_L_All,
      SWS_L_None,
      SWS_L_Unset,
      SWS_L_AllGrps,

      SWS_T_Active,
      SWS_T_Inv,
      SWS_T_Desc,
      SWS_T_Type,
      SWS_T_Match,
      SWS_T_Pct,
      SWS_T_Groups,

      SWS_H_Add,
      SWS_H_Del,
      SWS_H_Edit,
      SWS_H_Clone,
      SWS_LBH_Add,
      SWS_LBH_Close,

      SWS_H_Desc,
      SWS_H_Grp,

      SWS_M_Filter,
      SWS_M_SelAll,
      SWS_M_Save,
      SWS_M_Load,
      SWS_M_Clear,

      SWS_L_OK,
      SWS_L_OkCanc,
      SWS_L_Canc,
      SWS_L_SaveMsg,
      SWS_L_LoadMsg,
      SWS_L_ClearMsg,

      SWS_T_InsEdit,
      SWS_T_AuthNote,
      
      SWS_B_InsLoad,
      SWS_B_InsMkAll,
      SWS_B_InsClose,
  
      SWS_H_InsLoad,
      SWS_H_InsMkAll,
      SWS_H_InsClose,
      
      SWS_H_AnyAll,
         
      SWS_H_InsertHelp,
      
      SWS_Note_BadFltType,
      SWS_Err_NoFilters,
      SWS_Err_WrongObjClass,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      SWV_Img_True = 0,
      SWV_Img_False,
      SWV_Img_MAX,
   };


#  define SW_NList_ClickCol
#  define SW_NList_SelChange
#  define SW_NList_Clone

#  define SW_NList_CstHook return AddIfMatch(o, idata)

#  define SW_NList_ListDispHookPreObj(o, flt) \
      DoMethod(o, SWM_NSFList_RemFFIcons, flt)

#  define SW_NList_RemoveObjHook(o, flt) \
      DoMethod(o, SWM_NSFList_RemFFIcons, flt)

#  define SW_NList_DispHook \
      if (!idata->SWA_NSFList_NoDispose) MUI_DisposeObject(o)

#  define SW_NList_NoDragDrop
#  define SW_NList_NewObj MkInsObj((Object *)msg->UData)

#  define s_Desc       idata->SW_NListTRA[0].Gad
#  define s_DescAttr   idata->SW_NListTRA[0].ObjAttr
#  define s_Grp        idata->SW_NListTRA[1].Gad
#  define s_GrpAttr    idata->SW_NListTRA[1].ObjAttr

#  define SWV_NSFList_SaveLoadDir "PROGDIR:Filters"
   
   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NList_AttrDefs;
      SWP_Img_AttrDefs;

      Object *UI, *App, *o;
      ULONG   SWA_NSFList_Mode;
      ULONG   SWA_NSFList_UIMode;
      ULONG   SWA_NSFList_ShowGroups;
      ULONG   SWA_NSFList_NoDispose;
      Object *SWA_NSFList_Group;
      STRPTR  SWA_NSFList_FltType;
      ULONG   SWA_NSFList_DefMatch;
      ULONG   SWA_NSFList_UseCtxMenu;
      Object *SWA_NSFList_FFToolBar;
      ULONG   SWA_NSFList_Use;

      ULONG    FFArrayMax;
      Object **FFIcons;
      Object **FFObjs;
      ULONG    FFLabs;

      APTR   *MemPool;
     
      ULONG   EnableNew;
      ULONG   Total, Found, Inv;

      STRPTR GrpPatt;
      ULONG  GrpPattLen;
      
      Object **Flt, *CList,
              *InsertEditorWin, *InsertEditorList, *InsertEditorInfo;
      ULONG  Entries;

      UBYTE   DescBuff[256+sizeof(MUIX_B)];
      UBYTE   matchbuff[16], pctbuff[8];

      ULONG   ClickAttrs[5];
      ULONG   ClickCols[5];
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

#  define AV(s)   (SWPoolAV(idata->MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->MemPool, (UBYTE *)(m))
#  define RV(s,m) (SWPoolRV(idata->MemPool, (s), (UBYTE **)(m)))

#  define FL_PPEnabled  MUIX_C MUIX_B
#  define FL_PPDisabled MUIX_C

# endif // C_NSFLIST_INCLUDE_I
