# ifndef C_NAFILTER_INCLUDE
#  define C_NAFILTER_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NArticle.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NSFilter.h"

#  define SW_FIRSTTAG         (int)(TAG_USER + 20200)

   enum {
      SWA_NAFilter_Enabled    = SWA_NSFilter_Enabled,
      SWA_NAFilter_Desc       = SWA_NSFilter_Desc,
      SWA_NAFilter_Groups     = SWA_NSFilter_Groups,
      SWA_NAFilter_AuthNote   = SWA_NSFilter_AuthNote,
      SWA_NAFilter_MarkRead   = (LONG)SW_FIRSTTAG,  // [SGI] 
   }; 

   enum {
      SWM_NAFilter_UI       = SWM_NSFilter_UI,
      SWM_NAFilter_ShowUI   = SWM_NSFilter_ShowUI,
      SWM_NAFilter_HideUI   = SWM_NSFilter_HideUI,
      SWM_NAFilter_Begin    = (LONG)SW_FIRSTTAG,   // begin action for N files
      SWM_NAFilter_Action,                         // perform action on 1 file
      SWM_NAFilter_End,                            // end action
   };

   enum {
      SWV_NAFilter_OK = 0,                         // return codes: OK
      SWV_NAFilter_Error,                          // non-fatal error
      SWV_NAFitler_Abort,                          // abort whole shebang

      SWV_NAFilter_NeedHdr,                        // only need headers
      SWV_NAFilter_NeedBody,                       // also need article body
   };

   SWM_P(SWP_NAFilter_ShowIU,     Object *App);
   SWM_P(SWP_NAFilter_Begin,      Object *Grp);
   SWM_P(SWP_NAFilter_Action,     Object *Grp; Object *Art);
   SWM_P(SWP_NAFilter_End,        Object *Grp);

#  undef SW_FIRSTTAG

# endif
