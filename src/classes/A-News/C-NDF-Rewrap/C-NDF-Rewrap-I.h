# ifndef C_NDF_REWRAP_INCLUDE_I
#  define C_NDF_REWRAP_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-Rewrap.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NArticleView.h"
#  include <graphics/text.h>
#  include <diskfont/diskfont.h>
#  include <proto/diskfont.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_Rewrap
#  define SW_CNAME            "NSF_Rewrap"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_Rewrap_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

#  define TCount 4
   
   enum {
      SWS_L_QChars = 1,
      SWS_L_PLen,
      SWS_L_WPct,
      SWS_L_BadW,
      SWS_L_BChars,
      SWS_L_MRLen,
      SWS_L_NoFl,
      SWS_L_PreH,
      SWS_L_Cols,
      SWS_L_UseTbl,
      SWS_L_MinTR,
      SWS_L_MinTC,
      SWS_L_MinTP,
      SWS_L_MaxIL,
      SWS_L_TFont,

      SWS_L_L0,
      SWS_L_L1,

      SWS_H_QChars,
      SWS_H_PLen,
      SWS_H_WPct,
      SWS_H_BadW,
      SWS_H_BChars,
      SWS_H_MRLen,
      SWS_H_NoFl,
      SWS_H_PreH,
      SWS_H_Cols,
      SWS_H_UseTbl,
      SWS_H_MinTR,
      SWS_H_MinTC,
      SWS_H_MinTP,
      SWS_H_MaxIL,
      SWS_H_TFont,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      CNUM                    = 256,
      QPrefixLen              = 40,
      MaxWP                   = 95,
      MinWP                   = 10,
      SWV_NDF_Rewrap_MinWidth = 80,    // minimum wrap width
   };

   typedef struct FData
   {
      STRPTR   PPat;
      UWORD    CharWidths[CNUM];
      ULONG    MaxWidth;

      STRPTR  *Lines;
      ULONG    LineMax;
   } FData;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;
      
      STRPTR  SWA_NDF_Rewrap_QuoteChars;
      STRPTR  SWA_NDF_Rewrap_BreakChars;
      STRPTR  SWA_NDF_Rewrap_NoFlow;
      ULONG   SWA_NDF_Rewrap_PrefixLen;
      ULONG   SWA_NDF_Rewrap_WidthPct;
      ULONG   SWA_NDF_Rewrap_BadWrap;
      ULONG   SWA_NDF_Rewrap_MinWrapLen;
      ULONG   SWA_NDF_Rewrap_UseNoFlow;
      ULONG   SWA_NDF_Rewrap_BadWrapMax;
      ULONG   SWA_NDF_Rewrap_Cols;
      ULONG   SWA_NDF_Rewrap_PrefixHeur;
      ULONG   SWA_NDF_Rewrap_Tbl_Use;
      ULONG   SWA_NDF_Rewrap_Tbl_MinRows;
      ULONG   SWA_NDF_Rewrap_Tbl_MinCols;
      ULONG   SWA_NDF_Rewrap_Tbl_MinTblProb;
      ULONG   SWA_NDF_Rewrap_Tbl_MaxILines;
      STRPTR  SWA_NDF_Rewrap_Tbl_Font;

      ULONG   TextOnly;

      UBYTE   QChars[256], BChars[256];

      Object  *UI;
   } SW_IDATA;

#  define DefNoFl "#[ ](-#[-] |o |[0-9]#[0-9].)"

#  define DefTblFont "courier/13"
#  define MaxColArray  10
#  define ColSrchDepth 8
#  define IsColSpChr(c)    ((c)==' ' || (c)=='\t')
#  define IsColDataChr(c)  (!IsColSpChr(c))
#  define LLen(l, x)       ((l)[x+1] - (l)[x])

# endif // C_NDF_REWRAP_INCLUDE_I
