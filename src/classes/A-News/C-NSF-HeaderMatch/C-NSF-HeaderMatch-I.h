# ifndef C_NSF_HEADERMATCH_INCLUDE_I
#  define C_NSF_HEADERMATCH_INCLUDE_I

#  define SW_NListTrackAttrNum 2

#  include "S-ListImages.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-VarList.h"
#  include "Libraries/C-NSF-HeaderMatch.h"
#  include "Libraries/S-Lists.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NSF_HeaderMatch
#  define SW_CNAME            "NSF_HeaderMatch"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NSF_HeaderMatch_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Case   = 1,
      SWS_L_Wild,
      SWS_L_Anchor,
      SWS_L_Header,
      SWS_L_Pattern,

      SWS_L_Mode,
      SWS_L_MLab,
      SWS_L_Any,
      SWS_L_All,

      SWS_L_TopTitle,
      
      SWS_B_Add,
      SWS_B_Del,
      SWS_B_SelAll,
      SWS_B_Edit,
      SWS_GL_Header,

      SWS_L_Unset,

      SWS_H_Mode,
      SWS_H_Add,
      SWS_H_Del,
      SWS_H_SelAll,
      SWS_H_Edit,

      SWS_H_HMList,
      SWS_H_Header,
      SWS_H_Pattern,

      SWS_LASTSTR,
   };

   enum {
      SWV_Img_True = 0,
      SWV_Img_False,
      SWV_Img_MAX,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   enum {
      SWV_HM_HD_COUNT   = 15,
      SWV_MH_MaxHdrLen  = 48,
   };

#  define SW_NList_SelChange
#  define SW_NList_ClickCol
#  define SW_NList_DispHook MUI_DisposeObject(o)

#  define SW_NList_NewObj MUI_NewObject(SWC_NStrFilter, TAG_DONE)

#  define s_Patt       idata->SW_NListTRA[0].Gad
#  define s_PattAttr   idata->SW_NListTRA[0].ObjAttr
#  define s_Hdr        idata->SW_NListTRA[1].Gad
#  define s_HdrAttr    idata->SW_NListTRA[1].ObjAttr
#  define s_HdrDef     idata->SW_NListTRA[1].UnsetVal
#  define s_HdrGAttr   idata->SW_NListTRA[1].GadAttr

   enum {
      SWV_HM_Anchor   = (1<<0),
      SWV_HM_Case     = (1<<1),
      SWV_HM_Wild     = (1<<2),

      SWV_HM_stristr  = 0,
      SWV_HM_stricmp  = SWV_HM_Anchor,
      SWV_HM_strstr   = SWV_HM_Case,
      SWV_HM_strcmp   = (SWV_HM_Case   | SWV_HM_Anchor),
      SWV_HM_mpat     = (SWV_HM_Wild),
      SWV_HM_mpata    = (SWV_HM_Wild   | SWV_HM_Anchor),
      SWV_HM_mpatc    = (SWV_HM_Wild   | SWV_HM_Case),
      SWV_HM_mpatca   = (SWV_HM_Wild   | SWV_HM_Anchor| SWV_HM_Case),

      SWV_MaxLCLen    = 4090
   };
   
   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NList_AttrDefs;
      SWP_Img_AttrDefs;

      ULONG   SWA_NSF_HeaderMatch_Mode;

      TEXT    Hdr[SWV_MH_MaxHdrLen];

      struct _mdata {
         STRPTR PatBuff;
         ULONG  PatLen;
         ULONG  MatchCnt;
         ULONG  MatchTry;
         UBYTE  Header;
         UBYTE  Type;
      } *MData;
      
      ULONG   Entries;
      STRPTR  LCBuff;

      Object *UI, *title, *Grp, *Art;
      ULONG Total, Found, Inv;
   } SW_IDATA;

#  define SELALLON  MUIM_List_Select, MUIV_List_Select_All, \
                      MUIV_List_Select_On, NULL

#  define NOTP      MUIM_Notify, MUIA_Pressed, FALSE
#  define DCLK MUIM_Notify, MUIA_Listview_DoubleClick, TRUE

# endif // C_NSF_HEADERMATCH_INCLUDE_I
