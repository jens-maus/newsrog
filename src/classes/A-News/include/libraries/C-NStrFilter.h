# ifndef C_NSTRFILTER_INCLUDE
#  define C_NSTRFILTER_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 12300)

   enum {
      SWA_NStrFilter_Header    = SW_FIRSTTAG,     // [SGI] attribute
      SWA_NStrFilter_Pattern,
      SWA_NStrFilter_Case,
      SWA_NStrFilter_Wild,
      SWA_NStrFilter_Anchor,
      SWA_NStrFilter_Total,
      SWA_NStrFilter_Found,
   };

#  define SWV_NStrFilter_HFields \
   SWF_NAHeader_Approved,        \
   SWF_NAHeader_Date,            \
   SWF_NAHeader_Distrib,         \
   SWF_NAHeader_Followup,        \
   SWF_NAHeader_From,            \
   SWF_NAHeader_Groups,          \
   SWF_NAHeader_Keywords,        \
   SWF_NAHeader_Lines,           \
   SWF_NAHeader_MsgID,           \
   SWF_NAHeader_Organiz,         \
   SWF_NAHeader_Path,            \
   SWF_NAHeader_Ref,             \
   SWF_NAHeader_Sender,          \
   SWF_NAHeader_Subject,         \
   SWF_NAHeader_X_NewsRd

#  undef SW_FIRSTTAG   

# endif
