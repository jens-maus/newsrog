# ifndef C_LISTSEARCH_INCLUDE_I
#  define C_LISTSEARCH_INCLUDE_I

#  include "Libraries/C-ListSearch.h"
#  include "Libraries/S-Lists.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_ListSearch
#  define SW_CNAME            "ListSearch"
#  define SW_SUPERPUB         MUIC_Notify
#  define SW_CATVER           1

#  include "Libraries/L-Support.h"
#  include "S-Params.h"
#  include "Proto/ListSearch_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_DefTitle = 1,
      SWS_L_Pattern,
      SWS_L_Search,
      SWS_L_SubString,
      SWS_L_Dynamic,
      SWS_L_Case,
      SWS_L_FirstOnly,
      SWS_L_SelAll,
      SWS_L_UnselAll,
      SWS_L_ToggleAll,
      SWS_L_FromTop,
         
      SWS_L_Item,
      SWS_L_Items,
      SWS_L_BadPatt,
      SWS_L_Searching,
      SWS_L_SearchingNum,

      SWS_H_Search,
      SWS_H_SelAll,
      SWS_H_UnselAll,
      SWS_H_ToggleAll,
      SWS_H_FromTop,

      SWS_H_SubString,
      SWS_H_Dynamic,
      SWS_H_Case,
      SWS_H_FirstOnly,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

#  define MaxLen   128
#  define PBLen  ((MaxLen+2)*2+2)

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG   SWA_ListSearch_Dynamic;
      ULONG   SWA_ListSearch_Case;
      ULONG   SWA_ListSearch_SubLine;
      ULONG   SWA_ListSearch_First;
      STRPTR  SWA_ListSearch_StrLabel;
      STRPTR  SWA_ListSearch_Title;
      Object *SWA_ListSearch_List;
      Object *SWA_ListSearch_SearchObj;
      ULONG   SWA_ListSearch_SearchMethod;
      ULONG   SWA_ListSearch_CmpMethod;
      ULONG   SWA_ListSearch_WindowID;
      ULONG   SWA_ListSearch_ObjAttr;
      STRPTR  SWA_ListSearch_ItemName;
      STRPTR  SWA_ListSearch_ItemNamePl;
      Object *SWA_ListSearch_UI;
      ULONG   SWA_ListSearch_Flags;
      LONG    SWA_ListSearch_PrevSrchPos;
      LONG    SWA_ListSearch_InitSrchPos;
      STRPTR  SWA_ListSearch_PrevPat;
      Object *SWA_ListSearch_StatusTxt;
      LONG    SWA_ListSearch_UpdSrchCnt;

      ULONG   PosStk[MaxLen+2];

      UBYTE   Buff[MaxLen+2];
      UBYTE   PBuff[PBLen];
      UBYTE   FTxt[64];
      Object  *App, *Win;
      Object  *s_Pattern;
      LONG    PrevPatLen;
      LONG    NewOpts;
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE

# endif // C_LISTSEARCH_INCLUDE_I
