# ifndef C_NARTICLEVIEW_INCLUDE_I
#  define C_NARTICLEVIEW_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-CText.h"
#  include "Libraries/C-ListSearch.h"
#  include "Libraries/C-VarList.h"
#  include "Libraries/C-FileActs.h"
#  include "Libraries/C-DTImg.h"
#  include "Libraries/C-NGroup.h"
#  include "S-MkClass.h"
#  include <diskfont/diskfont.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NArticleView
#  define SW_CNAME            "NArticleView"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NArticleView_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_TopTitle = 1,
      SWS_L_Incomplete,

      SWS_L_QSTitle,
      SWS_L_Item,
      SWS_L_Items,

      SWS_L_BClip,
      SWS_L_BSel,
      SWS_L_BUnSel,
      SWS_L_BCanc,

      SWS_L_None,

      SWS_H_Clip,
      SWS_H_Search,
      SWS_H_BClip,
      SWS_H_BSel,
      SWS_H_BUnSel,
      SWS_H_BCanc,

      SWS_H_Up,
      SWS_H_Dn,

      SWS_L_InfoFmt,
      SWS_L_SizeFmt,
      SWS_L_FN,
      SWS_L_Info,
      SWS_L_Stat,
      SWS_L_Type,
      SWS_L_TUnknown,
      SWS_T_SaveUUD,
      SWS_L_OK,
      SWS_L_SavedOK,
      SWS_L_Canceled,
      
      SWS_B_Save,
      SWS_B_SaveAs,
      SWS_B_Act,

      SWS_H_Save,
      SWS_H_SaveAs,
      SWS_H_Act,

      SWS_L_WindozeFile,
      SWS_B_SvCanc,

      SWS_M_CpClip,

      SWS_L_OWrite,
      SWS_B_OWQuery,
      
      SWS_Err_Save,
      SWS_Err_Write,
      SWS_Err_NoSel,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   typedef struct OBJINFO {
      Object *NDF;     // filter to ask for objects from
      void   *Data;    // opaque data ptr from filter
   } ObjInfo;

   enum {
      SWV_NArticleView_MaxTitleLen = 40,
      SWV_NArticleView_MaxTxtLines = 250,
      SWV_VarBuffSize = 511,
   };

   enum {
      PEN_ArtCol = 0,
      PEN_Max
   };

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      Object     *SWA_NArticleView_DspList;
      Object     *SWA_NArticleView_ToolBar;
      Object     *SWA_NArticleView_DspGlob;
      ULONG       SWA_NArticleView_LineSpacing;
      STRPTR      SWA_NArticleView_TitleTxt;
      Object     *SWA_NArticleView_DspArt;
      Object     *SWA_NArticleView_DspGrp;
      ULONG       SWA_NArticleView_FirstBodyLine;
      STRPTR      SWA_NArticleView_ArtFont;
      ULONG       SWA_NArticleView_UseArtFont;
      ULONG       SWA_NArticleView_UseArtCol;
      MUI_PenSpec SWA_NArticleView_Pens[PEN_Max];
      ULONG       SWA_NArticleView_PendingDisp;

      Object  *o, *VGroupObj, *Empty, *QSearch, *b_Up, *b_Dn;
      Object  *NDF, *FltList;
      Object  *Title, *CtxMenu;
      APTR     MemPool;
      STRPTR  VarBuf;

      ULONG    RawFlag;   // if set, display raw (no filters)
      ULONG    SelCnt;

      ULONG    ObjListCnt;
      ULONG    ObjListMax;
      ObjInfo *ObjList;

      ULONG    PrevParseComplete;
      ULONG    PrevParseLine;

      ULONG    PrevObjCnt;
      ULONG    PrevObjMax;
      Object  **PrevObjs;

      Object  *FirstTxt;
      
      ULONG   *FltData;

      ULONG    LineHeight;

      struct Hook FltCmpHook;
      ULONG    A4;
   } SW_IDATA;

#  define AV(s)   (SWPoolAV(idata->MemPool, (s)))
#  define SD(s)   (SWPoolStrDup(idata->MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->MemPool, (UBYTE *)(m))
#  define RV(s,m) (SWPoolRV(idata->MemPool, (s), (UBYTE **)(m)))

#  define IsAO(s) (!strncmp(s, "\033AO{", 4))
#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

#  define IsWhite(c) ((c)==' '||(c)=='\t')

//#  define AV_CTextObject NewObject(OCLASS(CTextTemplate), NULL
#  define AV_CTextObject CTextObject

#  define TXT_MIME_TYPE "application/X-Text"
                                     
# endif // C_NARTICLEVIEW_INCLUDE_I
