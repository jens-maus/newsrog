# ifndef C_NSF_HEADERMATCH_INCLUDE
#  define C_NSF_HEADERMATCH_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"
#  include "Libraries/C-NStrFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 11100)

   // -- attribute mapping into AttrVal attrs --------------------------------

   enum {
      SWA_NSF_HeaderMatch_Mode = SW_FIRSTTAG,
   };

   enum {
      SWV_HM_Mode_Or  = 0,
      SWV_HM_Mode_And = 1,
   };

   enum {
      SWV_HM_Approved = 0,
      SWV_HM_Date,
      SWV_HM_Distrib,
      SWV_HM_Followup,
      SWV_HM_From,
      SWV_HM_Groups,
      SWV_HM_Keywords,
      SWV_HM_Lines,
      SWV_HM_MsgID,
      SWV_HM_Organiz,
      SWV_HM_Path,
      SWV_HM_Ref,
      SWV_HM_Sender,
      SWV_HM_Subject,
      SWV_HM_X_NewsRd,
   };

#  undef SW_FIRSTTAG   

# endif
