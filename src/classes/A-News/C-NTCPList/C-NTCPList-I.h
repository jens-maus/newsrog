# ifndef C_NTCPLIST_INCLUDE_I
#  define C_NTCPLIST_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NTCPList.h"
#  include "Libraries/C-ListCol.h"
#  include "Libraries/C-NNews.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NTCPList
#  define SW_CNAME            "NTCPList"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NTCPList_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Untitled = 1,
      SWS_L_None,
      SWS_L_Node,
      SWS_L_Title,
      SWS_L_Status,
      SWS_L_BPS,
      SWS_L_InBytes,
      SWS_L_InLines,
      SWS_L_Host,

      SWS_L_TopTitle,

      SWS_B_Dis,
      SWS_B_DisAll,

      SWS_H_List,
      SWS_H_Dis,
      SWS_H_DisAll,

      SWS_H_ListCol,
      SWS_H_Clip,
      
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

#  define SW_NList_DispHook   MUI_DisposeObject(o)
#  define SW_NList_RemoveObj  TRUE

   typedef struct SW_IDATA
   {
      SWP_NList_AttrDefs;
      SWP_NNews_AttrDefs;

      Object *SWA_NTCPList_ListCol;
      ULONG   SWA_NTCPList_Online;
      Library  *SocketBase;

      TEXT v4[20], v5[20], v6[20], node[48];
   } SW_IDATA;

#  define NOTP MUIM_Notify, MUIA_Pressed, FALSE

#  undef FD_ZERO
#  define FD_ZERO(p) memset((char *)(p), 0, sizeof(*(p)))

# endif // C_NTCPLIST_INCLUDE_I
