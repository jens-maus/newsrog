# ifndef C_OBJECTBOX_INCLUDE
#   define C_OBJECTBOX_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"
#   include "Libraries/C-Invocation.h"


   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 3100)

   enum {
      SWA_OBox_EditObj  = SW_FIRSTTAG,     // [  I] object to edit
      SWA_OBox_ContentObj,                 // [  I] object containing edit obj
      SWA_OBox_ContentAttr,                // [  I] attribute in content object
      SWA_OBox_VirtGroup,                  // [  I] use virtual group
   };

   enum {
      SWM_OBox_InsertClass = SW_FIRSTTAG,  // insert obj of new class
      SWM_OBox_InsertObject,               // insert object (del old)
      SWM_OBox_UpdateObject,               // update from new attr
      SWM_OBox_Editor,                     // return editor group
      SWM_OBox_Edit,                       // edit obj
      SWM_OBox_Save,                       // save obj
      SWM_OBox_Load,                       // load obj
      SWM_OBox_MethodForward,              // add method forwarding
   };

#  undef SW_FIRSTTAG

#  define OBAssign(x) if (x) DoMethod(x, SWM_OBox_InsertObject, NULL)

   SWM_P(SWP_OBox_InsObj,        Object *o);
   SWM_P(SWP_OBox_InsClass,      Object *List);
   SWM_P(SWP_OBox_Editor,        UBYTE *str);
   SWM_P(SWP_OBox_MethodForward, Object *Target; ULONG Method);

# endif
