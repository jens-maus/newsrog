# ifndef C_NDF_EMAIL_INCLUDE
#  define C_NDF_EMAIL_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "S-SysCmdVars.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 13200)

   enum {
      SWA_NDF_EMail_SelColor  = SW_FIRSTTAG,       // [SGI] selected color

      SWP_NDF_StyleAttrVals(SWA_NDF_EMail),        // [SGI] email text style

      SWA_NDF_EMail_Action,                        // [SGI] multimedia action
      SWA_NDF_EMail_RequireDot,                    // [SGI] require . in hostn
      SWA_NDF_EMail_DClick,                        // [SGI] need doubleclick
      SWA_NDF_EMail_PopMenus,                      // [SGI] use popup menus

      SWP_NDF_StyleAttrVals(SWA_NDF_EMail_MsgID),  // [SGI] MsgID text style
      SWA_NDF_EMail_MsgID_SelColor,                // [SGI] selected color
   };

   enum {
      SWM_NDF_EMail_ViewMsgID,                     // view this message ID
   };

   SWM_P(SWP_NDF_EMail_ViewMsgID, Object *Bt; Object *Grp);

#  undef SW_FIRSTTAG

# endif
