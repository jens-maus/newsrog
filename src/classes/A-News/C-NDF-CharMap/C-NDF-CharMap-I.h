# ifndef C_NDF_CHARMAP_INCLUDE_I
#  define C_NDF_CHARMAP_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NDF-CharMap.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/S-Lists.h"
#  include "MUI/TextEditor_mcc.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NDF_CharMap
#  define SW_CNAME            "NDF_CharMap"
#  define SW_SUPERPUB         SWC_NDFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NDF_CharMap_protos.h"
#  include "Libraries/C-NDFilter.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_TopTitle = 1,

      SWS_H_MapHelp,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;
      SWP_NDFilter_AttrDefs;

      STRPTR SWA_NDF_CharMap_MapInfo;

      UBYTE Mapping[256];
      UBYTE Deletion[256];
      ULONG HaveMap;
 
      Object  *UI, *Ed;
   } SW_IDATA;

#  define IsWhite(d) ((d)==' ' || (d)=='\t')
#  define IsOD(d)    ((d)>='0' && (d)<='7')
#  define IsDD(d)    ((d)>='0' && (d)<='9')
#  define IsHD(d)    (IsDD(d) || (d)>='a' && (d)<='f' || (d)>='A' && (d)<='A')
#  define hdtod(x)   (LONG)(((x)<='9')?(x)-'0':((x)>='a'?((x)-'a'):((x)-'A'))+10)
#  define ddtod(x)   (LONG)((x)-'0')

#  define NOTP  MUIM_Notify, MUIA_Pressed, FALSE

# endif // C_NDF_CHARMAP_INCLUDE_I
