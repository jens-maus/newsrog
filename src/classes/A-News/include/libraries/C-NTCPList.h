# ifndef C_NTCPLIST_INCLUDE
#  define C_NTCPLIST_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "C-NTCP.h"
#  include "Libraries/S-Lists.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10300)

   enum {
      SWA_NTCPList_ListCol    = SW_FIRSTTAG,     // [SGI] list columns
      SWA_NTCPList_Online,                       // [SGI] TRUE if online
   };

   enum {
      SWM_NTCPList_MaxSocket  = SW_FIRSTTAG,     // find max socket number
      SWM_NTCPList_Online,                       // check/set online
   };

   enum {
      SWV_NTCPList_Online_On = SW_FIRSTTAG,      // for SWM_NTCPList_Online
      SWV_NTCPList_Online_Off,
      SWV_NTCPList_Online_Test,
   };

   SWM_P(SWP_NTCPList_MaxSocket, fd_set *rfds; fd_set *wfds);
   SWM_P(SWP_NTCPList_Online,    ULONG Mode);

#  undef SW_FIRSTTAG

# endif
