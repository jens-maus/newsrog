# ifndef C_NPOSTLIST_INCLUDE_I
#  define C_NPOSTLIST_INCLUDE_I

#  define SW_NListTrackAttrNum 2

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NPostList.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NTCPList.h"
#  include "Libraries/C-NArticleList.h"
#  include "Libraries/S-Lists.h"
#  include "Libraries/C-ListCol.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NPostList
#  define SW_CNAME            "NPostList"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NPostlist_protos.h"
#  include "Libraries/C-NGroupList.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Unset = 1,
      SWS_L_Empty,
      SWS_L_TopTitle,

      SWS_L_Subj,
      SWS_L_Grp,

      SWS_T_PostOf,
      SWS_T_Sending,
      SWS_T_PostWin,
      SWS_T_Lines,
      SWS_T_Bytes,
      SWS_T_Subj,
      SWS_T_Grp,

      SWS_B_Cancel,
      SWS_B_Post,
      SWS_B_Edit,
      SWS_B_Del,
      SWS_B_Add,
      SWS_B_SelAll,
      SWS_B_MoveToSent,
      SWS_B_MoveToOut,

      SWS_H_List,
      SWS_H_Post,
      SWS_H_Edit,
      SWS_H_Del,
      SWS_H_Add,
      SWS_H_SelAll,
      SWS_H_MoveToSent,
      SWS_H_MoveToOut,

      SWS_H_ListCol,
      SWS_H_Clip,

      SWS_NG_H_Left,
      SWS_NG_H_Right,

      SWS_NG_L_Del,
      SWS_NG_L_Close,
      SWS_NG_L_Rest,
      SWS_NG_H_Del,
      SWS_NG_H_Close,
      SWS_NG_H_Rest,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

#  define s_Subj       idata->SW_NListTRA[0].Gad
#  define s_SubjAttr   idata->SW_NListTRA[0].ObjAttr
#  define s_SubjGAttr  idata->SW_NListTRA[0].GadAttr
#  define s_Grp        idata->SW_NListTRA[1].Gad
#  define s_GrpAttr    idata->SW_NListTRA[1].ObjAttr
#  define s_GrpGAttr   idata->SW_NListTRA[1].GadAttr

#  define SW_NList_DispHook DelListObj(o)
#  define SW_NList_NewObj   MkInsObj(idata)

#  define SW_NList_ReadObjPost(o) \
   set(o, SWA_NPost_PostDir, idata->SWA_NPostList_PostDir);

#  define SWV_NPostList_DefPostDir "PostQueue"
#  define SWV_NPostList_PostSvDir  "PostArch"

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NList_AttrDefs;

      ULONG  SWA_NPostList_Saved;
      STRPTR SWA_NPostList_PostDir;

      Object *b_Post, *PostWin, *G1, *G2, *PostObj, *GLPop;
      Object *SWA_NPostList_ListCol;
      ULONG   PostPos, PostCurr, PostCount;

      TEXT   v0[12], v1[12], PostOf[64];
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

#  define SW_NList_Change_Post DoMethod(idata->GLPop, SWM_NGList_StrToGroupsRO) \


# endif // C_NPOSTLIST_INCLUDE_I
