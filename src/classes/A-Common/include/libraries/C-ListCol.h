# ifndef C_LISTCOL_INCLUDE
#  define C_LISTCOL_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 90100)

   enum {
      SWA_ListCol_ColSpecs = SW_FIRSTTAG,  // [  I] all possible fields
      SWA_ListCol_ColNames,                // [  I] field names
      SWA_ListCol_ShownCols,               // [   ] shown column
      SWA_ListCol_HiddenCols,              // [   ] hidden column
      SWA_ListCol_Format,                  // [ G ] list format string
      SWA_ListCol_FixWidth,                // [SGI] size to entries
      SWA_ListCol_List,                    // [  I] ptr to list
      SWA_ListCol_Hook,                    // [  I] display hook for list
                                           //       (required for cp to clip)
      SWA_ListCol_ClipColSep,              // [SGI] column sp for clip
      SWA_ListCol_ClipTitles,              // [SGI] clip titles?
   };

   enum {
      SWM_ListCol_BuildFmt = SW_FIRSTTAG, // build FMT string
      SWM_ListCol_UI,                     // return UI
      SWM_ListCol_TrueCol,                // find true click column
      SWM_ListCol_Reset,                  // reset to default cols
      SWM_ListCol_Popup,                  // popup for listcol
      SWM_ListCol_HideCols,               // hide columns
      SWM_ListCol_Clip,                   // copy data to clipboard
      SWM_ListCol_ClipBt,                 // clip button
      SWM_ListCol_RemNotify,              // remove notifies
   };

   SWM_P(SWP_ListCol_TrueCol,   LONG Col);
   SWM_P(SWP_ListCol_Popup,     LONG Ctrl; STRPTR Help);
   SWM_P(SWP_ListCol_ClipBt,    LONG Ctrl; STRPTR Help);
   SWM_P(SWP_ListCol_Clip,      Object *ClipBt);

   // -- macros to faciliate setting up notifies -----------------------------
   
#  define SWM_SetListColNotify(listcolobj, listobj)                      \
   DoMethod(listcolobj, MUIM_Notify, SWA_ListCol_Format, MUIV_EveryTime, \
            listobj, 3, MUIM_Set, MUIA_List_Format, MUIV_TriggerValue)

#  undef SW_FIRSTTAG   

# endif
