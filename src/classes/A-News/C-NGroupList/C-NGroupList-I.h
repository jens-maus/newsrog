# ifndef C_NGROUPLIST_INCLUDE_I
#  define C_NGROUPLIST_INCLUDE_I

#  define SW_NListTrackAttrNum 1

#  include "S-ListImages.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NGroupList.h"
#  include "Libraries/C-ListSearch.h"
#  include "Libraries/C-NTCPList.h"
#  include "Libraries/C-NSFList.h"
#  include "Libraries/C-NServerList.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-ListCol.h"
#  include "Libraries/C-Timer.h"
#  include "Libraries/C-VarList.h"
#  include "Libraries/C-DTImg.h"
#  include "Libraries/S-Lists.h"
#  include "S-CmpObj.h"
#  include <dos/datetime.h>
#  include <dos/exall.h>
#  include <ctype.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NGroupList
#  define SW_CNAME            "NGroupList"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NGroupList_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Unnamed = 1,
      SWS_L_Unset,
      SWS_L_Never,
      SWS_L_Default,
      SWS_L_Name,
      SWS_L_Server,
      SWS_L_NewCount,
      SWS_L_AvailCount,
      SWS_L_TotArtCnt,
      SWS_L_UnReadArtCnt,
      SWS_L_ReadArtCnt,
      SWS_L_Description,
      SWS_L_Moderated,
      SWS_L_LastRead1,
      SWS_L_LastRead2,
      SWS_L_Icon,
      SWS_L_LastRead,
      SWS_L_TopTitle,
      SWS_L_OnLine,
      SWS_L_OffLine,
      SWS_L_GroupCmds,
      SWS_L_PM_GroupOf,
      SWS_L_GroupSrch,
      SWS_L_SGUpdInt,
      SWS_L_SGPatt,

      SWS_L_ModY,
      SWS_L_ModN,
      
      SWS_B_Cancel,
      SWS_B_Add,
      SWS_B_Del,
      SWS_B_Sort,
      SWS_B_SelAll,
      SWS_B_ViewUnsub,
      SWS_B_Read,
      SWS_B_MarkRead,
      SWS_B_MarkUnread,
      SWS_B_UpdateCnt,
      SWS_B_ReadDesc,
      SWS_B_BatchOView,
      SWS_B_BatchArts,
      SWS_B_Fetch,
      SWS_B_GLClr,
      SWS_B_GLSrch,
      SWS_B_GLAbt,
      SWS_B_GLSort,
      SWS_B_GLSave,
      SWS_B_GLLoad,
      SWS_B_GLSub,
      SWS_B_GLSelAll,
      SWS_B_GLNew,
      SWS_B_GLBrowse,
      SWS_B_GoOnLine,
      SWS_B_GoOffLine,

      SWS_H_SubGrpList,
      SWS_H_SrvGrpList,
      SWS_H_Add,
      SWS_H_Del,
      SWS_H_Sort,
      SWS_H_SelAll,
      SWS_H_ViewUnsub,
      SWS_H_Read,
      SWS_H_MarkRead,
      SWS_H_MarkUnread,
      SWS_H_Search,
      SWS_H_UpdateCnt,
      SWS_H_ReadDesc,
      SWS_H_BatchOView,
      SWS_H_BatchArts,
      SWS_H_Fetch,
      SWS_H_StrFetch,
      SWS_H_GLClr,
      SWS_H_GLSrch,
      SWS_H_GLAbt,
      SWS_H_GLSort,
      SWS_H_GLSave,
      SWS_H_GLLoad,
      SWS_H_GLSub,
      SWS_H_GLSelAll,
      SWS_H_GLNew,
      SWS_H_GLBrowse,
      SWS_H_SortP,
      SWS_H_GoOnLine,
      SWS_H_GoOffLine,
      SWS_H_FAQb,
      SWS_H_SGUpdInt,
      SWS_H_SGPatt,

      SWS_L_UnregMsg,
      SWS_B_DelOVBt,
      SWS_L_DelOVMsg,
      SWS_L_Group,
      SWS_L_Groups,
      SWS_L_Warn,

      SWS_T_UnSub,
      SWS_T_DLSG,
      SWS_T_GL_Srch,
      SWS_T_GSTitle,
      SWS_T_GSSave,
      SWS_T_GSLoad,
      SWS_T_GSCmds,
      SWS_T_GL_Hdrs,
      SWS_T_GL_Arts,

      SWS_B_GSSaveCanc,
      SWS_B_GSLoadCanc,
      SWS_B_GSOK,

      SWS_L_Cancel,
      SWS_L_Note,
      SWS_L_GSSubWarn,
      SWS_B_GSSubWarn,
      SWS_B_OK,

      SWS_L_FNames,
      SWS_H_FNames,
      SWS_L_UpdInt,
      SWS_H_UpdInt,
      SWS_L_UOpen,
      SWS_H_UOpen,

      SWS_L_SubGrps,
      SWS_L_GrpOpts,
      SWS_L_BatchFlt,

      SWS_H_BatchFlt,

      SWS_L_Sort1,
      SWS_L_Sort2,
      SWS_L_Sort3,
      SWS_L_SortRev,

      SWS_M_Groups,
         SWS_M_ImportNRC,
         SWS_M_AllCache,

      SWS_H_ListCol,
      SWS_H_Clip,

      SWS_Err_GSSaveOpen,
      SWS_Err_GSLoadOpen,
      SWS_Err_GSLoadFmt,
      SWS_Err_Alloc,
      SWS_Err_NoHome,
      SWS_Err_ImportCD_Scan,
      SWS_Note_DupGroups,
      SWS_Note_NoneSelected,

      SWS_LASTSTR,
   };

#  define MOff   1000
#  define GSDir  "PROGDIR:ServerGroups"
#  define GSPat  "(.newsrc|SGrp.*)"

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   enum {
      SWV_Img_Broken  = 0,
      SWV_Img_WinOpen,
      SWV_Img_WinClose,
      SWV_Img_NewBatch,
      SWV_Img_MAX,
   };

#  define s_Name     idata->SW_NListTRA[0].Gad
#  define s_NameAttr idata->SW_NListTRA[0].ObjAttr

#  define SW_NList_NoDragDrop
#  define SW_NList_CmpCode \
   return (LONG)DoMethod(o1, SWM_NGroup_Cmp, o2, &idata->CmpInfo);

#  define SW_NList_DispHook MUI_DisposeObject(o)
#  define SW_NList_NewObj   MkNewGrp(o, idata, NULL)

   enum {
      SWV_NGroupList_SG_MaxLineLen = 1024,
      SWV_NGroupList_SG_MaxSGPatt  = 512,
      SWV_NGroupList_SG_Inc        = 256,
      SWV_NGroupList_SG_SubWarn    = 100,

      SWV_NGroupList_AbbrevLen     = 80,

      SWV_NGroupList_MaxUnreg      = 2,
   };
   
   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NList_AttrDefs;
      SWP_Img_AttrDefs;

      ULONG Online, UIAdded;

      APTR   *SWA_NGroupList_MemPool;
      ULONG   SWA_NGroupList_FullNames;
      Object *SWA_NGroupList_ListCol;
      ULONG   SWA_NGroupList_UpdInt;
      ULONG   SWA_NGroupList_Upd;
      ULONG   SWA_NGroupList_UpdOpen;
      STRPTR  SWA_NGroupList_LastFetchPat;
      Object *SWA_NGroupList_BatchDLFlt;
      STRPTR  SWA_NGroupList_LastSGDate;
      STRPTR  SWA_NGroupList_SGPatt;
      ULONG   SWA_NGroupList_SGUpdateInt;
      Object *SWA_NGroupList_SG_L;
      Object *SWA_NGroupList_SG_LV;

      ULONG   WinOpen;
      Object  *App, *Mode, *SGWin, *s_FetchPat, *b_GLAbt, *b_Fetch,
              *b_GoOnLine, *b_GoOffLine, *b_ModeBt, *ModeGroup, *UpdTimer, *RegGrp;

      Object *GL_Srv;      // for server groups;
      ULONG   GL_Id;
      STRPTR *GL_Lines;
      Object *GL_Search;
      ULONG   GL_All;

      ULONG   SGInt;
      STRPTR  SGPatt;
      
      Object *GroupSearch;
      Object *BrowseGroup;   // browse this group without subscribing to it

      UBYTE  v3[16], v4[16], v5[16], v6[16], v9[16],
             abbrev[SWV_NGroupList_AbbrevLen], date[32], time[16];

      struct SWS_CmpInfo CmpInfo;

      struct {
         LONG    Pos, PrevPos, Count;
         ULONG   Method;
         STRPTR *List;
         STRPTR  WinTitle;
         STRPTR  GrpMsg;
         ULONG   ShowProgWin;
         ULONG   PcAttr;
         Object *ProgWin, *G1, *G2;
         Object *Button;
      } Cmds[SWV_GC_MAX];
   } SW_IDATA;

#  define b_RdDesc idata->Cmds[SWV_GC_ReadDesc].Button
#  define b_UpdCnt idata->Cmds[SWV_GC_CheckStats].Button
#  define b_BatchO idata->Cmds[SWV_GC_GL_Hdrs].Button
#  define b_BatchA idata->Cmds[SWV_GC_GL_Arts].Button
#  define b_Fetch  idata->b_Fetch

#  define NOTP      MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK      MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

#  define SELALLON  MUIM_List_Select, MUIV_List_Select_All, \
                      MUIV_List_Select_On, NULL

#  define SELALLOFF MUIM_List_Select, MUIV_List_Select_All, \
                      MUIV_List_Select_Off, NULL

#  define SELALLTOG MUIM_List_Select, MUIV_List_Select_All, \
                      MUIV_List_Select_Toggle, NULL

#  define AV(s)   (SWPoolAV(idata->SWA_NGroupList_MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->SWA_NGroupList_MemPool, (UBYTE *)(m))
#  define RV(s,m) (SWPoolRV(idata->SWA_NGroupList_MemPool, (s), (STRPTR *)(m)))
#  define SDup(s) (SWPoolStrDup(idata->SWA_NGroupList_MemPool, (s)))

#  define ASize(x) ((x)*sizeof(ULONG))

#  define InitSGDate "00000000 000000"

Object *FindGroup(SW_IDATA *idata, STRPTR Name);
Object *MkNewGrp(Object *o, SW_IDATA *idata, STRPTR Name);

   METHOD_EXTERN(SWM_AppBase_RexxCmd, SWP_AppBase_RexxCmd *);

# endif // C_NGROUPLIST_INCLUDE_I
