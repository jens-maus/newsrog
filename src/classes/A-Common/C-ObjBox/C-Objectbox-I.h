# ifndef C_OBJECTBOX_INCLUDE_I
#  define C_OBJECTBOX_INCLUDE_I

#  include "Libraries/C-Objectbox.h"
#  include "Libraries/C-Objectlist.h"
#  include "Libraries/C-Classlist.h"


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_Objectbox
#  define SW_CNAME            "Objectbox"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/Objectbox_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Edit = 1,
      SWS_L_Save,
      SWS_L_Load,

      SWS_H_OBox,
      SWS_H_Edit,
      SWS_H_Save,
      SWS_H_Load,
      
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

#  define MAXMTHF 4
   
   typedef struct SW_IDATA
   {
      Object     *SWA_OBox_EditObj;
      Object     *SWA_OBox_ContentObj;
      Object     *AddGroup, *AOGroup;
      ULONG       SWA_OBox_ContentAttr;
      ULONG       SWA_OBox_VirtGroup;

      Object     *Target[MAXMTHF];  // for method forwarding
      ULONG       Method[MAXMTHF];
   } SW_IDATA;


# endif // C_OBJECTBOX_INCLUDE_I
