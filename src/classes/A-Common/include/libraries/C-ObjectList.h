# ifndef C_OBJECTLIST_INCLUDE
#   define C_OBJECTLIST_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"
#   include "Libraries/C-Invocation.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 2500)

   enum {
      SWA_OList_ListObj  = SW_FIRSTTAG,           // [SG ] keep track of object &
      SWA_OList_TreeNode,                         // [SG ] tree node
   };

   enum {
      SWM_OList_Refresh  = SW_FIRSTTAG,           // refresh list from object
      SWM_OList_InsertClass,                      // insert obj of new class
      SWM_OList_InsertObject,                     // insert object
      SWM_OList_ActiveObj,                        // rtn active object or NULL
      SWM_OList_ObjClone,                         // clone active object
      SWM_OList_ObjEdit,                          // edit active object
      SWM_OList_ObjRem,                           // remove active object
      SWM_OList_ObjIns,                           // insert obj from disk
      SWM_OList_ObjSave,                          // save active obj to disk
      SWM_OList_ObjReset,                         // reset active obj to dflt
   };

#  undef SW_FIRSTTAG

   // -- method parameters ---------------------------------------------------

   enum { SWF_OList_SameLevel = (1<<0) };
   
   SWM_P(SWP_OList_Obj,      Object *o);
   SWM_P(SWP_OList_InsObj,   Object *o; ULONG Flags);
   SWM_P(SWP_OList_InsClass, Object *List);

# endif
