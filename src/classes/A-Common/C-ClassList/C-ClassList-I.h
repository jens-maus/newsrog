# ifndef C_CLASSLIST_INCLUDE_I
#  define C_CLASSLIST_INCLUDE_I

#  include "Libraries/C-ClassList.h"


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_Classlist
#  define SW_CNAME            "Classlist"
#  define SW_SUPERPUB         MUIC_List
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/Classlist_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_Name = 1,
      SWS_Desc,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   typedef struct _cinfo {
      struct MinNode node;
      UBYTE ClassName[25];
      UBYTE ClassDesc[80];
      UBYTE ClassLib[50];
      UBYTE ClassType[32];
   } ClassInfo;

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA {
      List ClassList;
      ULONG Setup;
      UBYTE Path[64];
      UBYTE Type[64];
   } SW_IDATA;


# endif // C_CLASSLIST_INCLUDE_I
