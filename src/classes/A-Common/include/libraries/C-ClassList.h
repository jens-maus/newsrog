# ifndef C_CLASSLIST_INCLUDE
#   define C_CLASSLIST_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"
#   include "Libraries/C-Root.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 2400)

   enum {
      SWA_ClassList_Type = SW_FIRSTTAG,      // [  I] type of classes to use
      SWA_ClassList_RegPath,                 // [  I] path to reginfo database
      SWA_ClassList_Immediate,               // [  I] immediate load of DB
   };

   enum {
      SWM_ClassList_MakeObj = SW_FIRSTTAG,   // make obj of selected class
      SWM_ClassList_GetAttr,                 // get attribute
      SWM_ClassList_MakeObjNew,              // new makeobj - can pass pos
      SWM_ClassList_InfoLib,                 // get info about class by lib name
      SWM_ClassList_InfoObj,                 // get info by object
      SWM_ClassList_GoodType,                // type is a match
      SWM_ClassList_GoodTypeObj,             // object's type is a match?
   };

   enum {
      SWV_ClassList_MakeObj_Active = -1,     // active
   };

   enum {
      SWT_ClassList_MakeObj_Pos = SW_FIRSTTAG, // position in list
   };

   enum {
      SWV_ClassList_Name,                    // for SWM_ClassList_GetAttr
      SWV_ClassList_Lib,
   };

   enum {
      SWV_ClassList_Info_Name = 0,           // for SWM_ClassList_GetInfo
      SWV_ClassList_Info_Desc,
      SWV_ClassList_Info_Lib,
      SWV_ClassList_Info_Type,
   };

   SWM_P(SWP_ClassList_GetAttr,    ULONG Pos; ULONG Attr);

   SWM_P(SWP_ClassList_InfoLib,
         STRPTR LibName;            // class to get info about
         ULONG  Info);              // what info to get

   SWM_P(SWP_ClassList_InfoObj,
         Object *Obj;               // object to get info about
         ULONG  Info);              // what info to get

   SWM_P(SWP_ClassList_GoodType,
         STRPTR Type);              // type to check

   SWM_P(SWP_ClassList_GoodTypeObj,
         Object *Obj);              // object to check

#  define ClasslistObject MUI_NewObject(SWC_Classlist

#   undef SW_FIRSTTAG

# endif
