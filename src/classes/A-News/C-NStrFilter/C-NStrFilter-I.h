# ifndef C_NSTRFILTER_INCLUDE_I
#  define C_NSTRFILTER_INCLUDE_I

#  include "Libraries/C-NStrFilter.h"


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NStrFilter
#  define SW_CNAME            "NStrFilter"
#  define SW_SUPERPUB         SWC_NSFilter
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NStrFilter_protos.h"
#  include "Libraries/C-NSF-HeaderMatch.h"

   enum {
      SWS_L_Header = 1,
      SWS_LH_Approved,
      SWS_LH_Date,
      SWS_LH_Distrib,
      SWS_LH_Followup,
      SWS_LH_From,
      SWS_LH_Groups,
      SWS_LH_Keywords,
      SWS_LH_Lines,
      SWS_LH_MsgID,
      SWS_LH_Organiz,
      SWS_LH_Path,
      SWS_LH_Ref,
      SWS_LH_Sender,
      SWS_LH_Subject,
      SWS_LH_X_NewsRd,

      SWS_L_Pattern,
      SWS_L_Case,
      SWS_L_Wild,
      SWS_L_Anchor,

      SWS_H_Header,
      SWS_H_Pattern,
      SWS_H_Case,
      SWS_H_Wild,
      SWS_H_Anchor,

      SWS_LASTSTR,
   };

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      ULONG  SWA_NStrFilter_Header;
      STRPTR SWA_NStrFilter_Pattern;
      ULONG  SWA_NStrFilter_Case;
      ULONG  SWA_NStrFilter_Wild;
      ULONG  SWA_NStrFilter_Anchor;

      ULONG  SWA_NStrFilter_Total;
      ULONG  SWA_NStrFilter_Found;

      Object *UI;
   } SW_IDATA;

# endif // C_NSTRFILTER_INCLUDE_I
