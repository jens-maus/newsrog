# ifndef C_NMPMGR_INCLUDE
#  define C_NMPMGR_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-AttrVal4.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 16000)

   // -- attribute mapping into AttrVal attrs --------------------------------

   enum {
      SWA_NSF_NMPMgr_ArtMgr = SW_FIRSTTAG,
   };

   enum {
      SWA_NSF_NMPMgr_ArtMgr = SW_FIRSTTAG,
   };

   enum {
      SWA_NMPMgr_Subj    = SWA_Attrval4_Attr,
      SWA_NMPMgr_Grp     = SWA_Attrval4_Val0,
      SWA_NMPMgr_MaxP    = SWA_Attrval4_Num0,
      SWA_NMPMgr_Missing = SWA_Attrval4_Num1,
      SWA_NMPMgr_Mode    = SWA_Attrval4_Num2,
   };

#  undef SW_FIRSTTAG   

# endif // C_NMPMGR_INCLUDE
