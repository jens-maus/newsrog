# ifndef C_LISTCOL_INCLUDE_I
#  define C_LISTCOL_INCLUDE_I

#  include "Libraries/C-ListCol.h"
#  include "Libraries/C-Root.h"
#  include <utility/hooks.h>

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_ListCol
#  define SW_CNAME            "ListCol"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "Libraries/L-Support.h"
#  include "S-Params.h"
#  include "Proto/ListCol_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Disp = 1,
      SWS_L_NonDisp,
      SWS_L_Reset,
      SWS_L_Close,
      SWS_L_Clip,
      SWS_L_Titles,
      SWS_L_Sep,

      SWS_H_Disp,
      SWS_H_Hidden,
      SWS_H_Reset,
      SWS_H_Close,
      SWS_H_Clip,
      SWS_H_Titles,
      SWS_H_Sep,

      SWS_LASTSTR,
   };

   enum {
      SWV_ListCol_MaxCol   = 24,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

#  define BF_ListCol "5:PROGDIR:Images/IFF.ListCol"
#  define BF_Clip    "5:PROGDIR:Images/IFF.Clip"

   typedef struct SW_IDATA
   {
      STRPTR *SWA_ListCol_ColSpecs;
      STRPTR *SWA_ListCol_ColNames;
      BYTE    SWA_ListCol_ShownCols[SWV_ListCol_MaxCol];
      BYTE    SWA_ListCol_HiddenCols[SWV_ListCol_MaxCol];
      STRPTR  SWA_ListCol_Format;
      ULONG   SWA_ListCol_FixWidth;
      Object *SWA_ListCol_List;
      STRPTR  SWA_ListCol_ClipColSep;
      ULONG   SWA_ListCol_ClipTitles;
      struct Hook *SWA_ListCol_Hook;
      ULONG   Setup;

      Object  *b_Title, *b_ColSep;
      
      TEXT    BCol[48];
      
      Object *L1, *L2, *LV1, *LV2;
   } SW_IDATA;

#  define Shown(x)  (idata->SWA_ListCol_ShownCols[x])
#  define Hidden(x) (idata->SWA_ListCol_HiddenCols[x])

#  define NOTP      MUIM_Notify, MUIA_Pressed, FALSE

# endif // C_LISTCOL_INCLUDE_I
