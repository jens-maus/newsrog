# ifndef C_NSFLIST_INCLUDE
#  define C_NSFLIST_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NSFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 10900)

   enum {
      SWA_NSFList_Mode   = SW_FIRSTTAG,     // [SGI] T for AND, else OR
      SWA_NSFList_UIMode,                   // [  I] UI mode
      SWA_NSFList_ItemChange,               // [SGI] enable state chg
      SWA_NSFList_NoDispose,                // [SGI] don`t dispose children
      SWA_NSFList_ShowGroups,               // [  I] show group column
      SWA_NSFList_Group,                    // [  I] group ptr
      SWA_NSFList_FltType,                  // [  I] filter type
      SWA_NSFList_DefMatch,                 // [SGI] if no entries.
      SWA_NSFList_UseCtxMenu,               // [SGI] use Ctx menu?
      SWA_NSFList_FFToolBar,                // [  I] filter function toolbar
      SWA_NSFList_Use,                      // [  I] what these are used for
   };

   enum {
      SWM_NSFList_DoSelected = SW_FIRSTTAG, // do mthd for selected entries
      SWM_NSFList_AddSel,                   // add selected classes
      SWM_NSFList_Save,                     // save filters to disk
      SWM_NSFList_Load,                     // load filters from disk
      SWM_NSFList_CtxMenuChoice,            // context menu choice
      SWM_NSFList_InsEdClose,               // close insert editor
      SWM_NSFList_InsEdLoad,                // load marked filters
      SWM_NSFList_InsEdDspInfo,             // display user info
      SWM_NSFList_InsEdMkAll,               // mark all
      SWM_NSFList_AddFFIcon,                // add filter function icons
      SWM_NSFList_RemFFIcons,               // remove filter function icons
      SWM_NSFList_BldFltList,               // build composite filter list
   };

   enum {
      SWV_NSFList_Use_Display = 0,
      SWV_NSFList_Use_Action  = 0,
      SWV_NSFList_Use_Select  = 0,
      SWV_NSFList_Use_Kill    = 1,
      SWV_NSFList_Use_Hlight  = 2,
   };

   enum {
      SWV_NSFList_UI_Normal = 0,
      SWV_NSFList_UI_Tiny,
      SWV_NSFList_UI_Insert,
   };

   enum {
      SWV_NSFList_Or  = 0,
      SWV_NSFList_And = 1,                  // modes
   };

   SWM_P(SWP_NSFList_AddSel,                // add selected
         Object *CList;
         Object *CBox);

   SWM_P(SWP_NSFList_CtxMenuChoice,         // context menu choice
         Object *Item);

   SWM_P(SWP_NSFList_InsEdDspInfo,          // display info for filter
         ULONG Pos);

   SWM_P(SWP_NSFList_AddFFIcon,             // add filter function icons
	 Object *FilterObj;                 // filter object this belongs to
         Object *Icon);                     // icon to add

   SWM_P(SWP_NSFList_RemFFIcons,            // remove filter function icons
	 Object *FilterObj);                // filter object this belongs to
                                            // NULL to remove all

#  define NSFListObject MUI_NewObject(SWC_NSFList

#  undef SW_FIRSTTAG   

# endif
