# ifndef C_NARTICLELIST_INCLUDE_I
#  define C_NARTICLELIST_INCLUDE_I

#  include "S-ListImages.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NArticleList.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-ListCol.h"
#  include "Libraries/C-DTImg.h"
#  include "Libraries/S-Lists.h"
#  include "Libraries/C-ListSearch.h"
#  include "S-CmpObj.h"
#  include <dos/datetime.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NArticleList
#  define SW_CNAME            "NArticleList"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NArticleList_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Title   = 1,
      SWS_L_Unknown,
      SWS_L_QMark,
      SWS_L_Blank,
      SWS_L_Lines,
      SWS_L_NumGroups,
      SWS_L_Auth,
      SWS_L_Date,
      SWS_L_Time,
      SWS_L_Repl,
      SWS_L_Subject,
      SWS_L_Size,
      SWS_L_Age,

      SWS_L_LongTitle0,
      SWS_L_LongTitle1,
      SWS_L_LongTitle2,
      SWS_L_LongTitle3,
      SWS_L_LongTitle4,
      SWS_L_LongTitle5,
      SWS_L_LongTitle6,
      SWS_L_LongTitle7,
      SWS_L_LongTitle8,
      SWS_L_LongTitleLast,

      SWS_L_Sort1,
      SWS_L_Sort2,
      SWS_L_Sort3,
      SWS_L_SortRev,

      SWS_H_Clip,
      SWS_H_ListCol,
            
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   enum {
      SWV_NArticleList_MaxSubLen     = 256,
      SWV_NArticleList_MaxIndent     = 24,
      SWV_NArticleList_IndentPer     = 2,
      SWV_NArticleList_NameBuffLen   = 18,
      SWV_NArticleList_DateBuffLen   = 32,
      SWV_NArticleList_TimeBuffLen   = 16,
      SWV_NArticleList_SizeBuffLen   = 20,
      SWV_NArticleList_AgeBuffLen    = 32,
   
      SWV_NArticleList_SubjBuffLen   = SWV_NArticleList_MaxSubLen +
                                       (LONG)sizeof(MUIX_B)  +
                                       (LONG)sizeof(MUIX_I)  +
                                       (LONG)sizeof(MUIX_PH) +
                                       SWV_NArticleList_MaxIndent *
                                          SWV_NArticleList_IndentPer + 64,
   };

   enum {
      SWV_NAL_Img_Disk = 0,
      SWV_NAL_Img_Drawer,
      SWV_NAL_Img_Net,
      SWV_NAL_Img_Broken,
      SWV_NAL_Img_NewArt,
      SWV_Img_MAX,
   };

#  define SW_NList_CmpCode \
   return (LONG)DoMethod(o1, SWM_NArticle_Cmp, o2, &idata->CmpInfo);

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NList_AttrDefs;
      SWP_Img_AttrDefs;

      STRPTR  SWA_NArticleList_Title;
      ULONG   SWA_NArticleList_QSH;
      ULONG   SWA_NArticleList_QSUpdDisp;
      ULONG   SWA_NArticleList_QSFromRam;
      Object *SWA_NArticleList_ToolBar;
      ULONG   SWA_NArticleList_ClickAttr;
      Object *SWA_NArticleList_ClickItem;
      APTR   *SWA_NArticleList_MemPool;
      Object *SWA_NArticleList_ThreadMgr;  // private artmgr for threads
      ULONG   SWA_NArticleList_Threaded;
      Object *SWA_NArticleList_ListCol;
      Object *SWA_NArticleList_Group;
      ULONG   SWA_NArticleList_NoUI;
      Object *SWA_NArticleList_SortBt;

      Object  *LastSrchArt;

      struct SWS_NArticle_CmpInfo CmpInfo;

      ULONG   Modified, Threaded;
      
      UBYTE GrpBuff[12], FBuff[12];
      UBYTE SubjBuff[SWV_NArticleList_SubjBuffLen];
      UBYTE SizeBuff[SWV_NArticleList_SizeBuffLen];
      UBYTE NameBuff[SWV_NArticleList_NameBuffLen];
      UBYTE DateBuff[SWV_NArticleList_DateBuffLen];
      UBYTE TimeBuff[SWV_NArticleList_TimeBuffLen];
      UBYTE AgeBuff[SWV_NArticleList_AgeBuffLen];
      UBYTE PrevDateBuff[SWV_NArticleList_DateBuffLen];
      ULONG  Prev_ds_Days;
      struct DateStamp ArtDS;

      ULONG    OldMemPool;       // re-use this field for hash tbl pos

      Object *Title;
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE

# endif // C_NARTICLELIST_INCLUDE_I
