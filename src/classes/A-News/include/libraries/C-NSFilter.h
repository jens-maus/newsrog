# ifndef C_NSFILTER_INCLUDE
#  define C_NSFILTER_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NArticle.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NSFList.h"

#  define SW_FIRSTTAG         (int)(TAG_USER + 20100)

   enum {
      SWA_NSFilter_TotHist  = (LONG)SW_FIRSTTAG,   // [ G ] total tries ever
      SWA_NSFilter_FoundHist,                      // [ G ] total found ever
      SWA_NSFilter_TotNow,                         // [ G ] total this time
      SWA_NSFilter_FoundNow,                       // [ G ] found this time
      SWA_NSFilter_Desc,                           // [SGI] user description
      SWA_NSFilter_Inv,                            // [SGI] match or not
      SWA_NSFilter_Enabled,                        // [SGI] turn on or off
      SWA_NSFilter_Groups,                         // [SGI] group match pattern
      SWA_NSFilter_ExpireDate,                     // [SGI] expiration ds_Day
      SWA_NSFilter_ExpireDateOK,                   // [SGI] use ExpireDay
      SWA_NSFilter_LastMatchDay,                   // [ G ] last match day
      SWA_NSFilter_ExpDisuse,                      // [SGI] N days aft lst mtchr
      SWA_NSFilter_ExpDisuseOK,                    // [SGI] N days aft lst mtchr
      SWA_NSFilter_LastMatchDayDsp,                // [SGI] last match day
      SWA_NSFilter_AuthNote,                       // [SGI] author note
      SWA_NSFilter_ThreadMode,                     // [SGI] filter thread mode
      SWA_NSFilter_UseOnOffBt,                     // [SGI] use on/off button
      SWA_NSFilter_OnOffBtImg,                     // [SGI] button image
   };
   
   enum {
      SWM_NSFilter_Begin    = (LONG)SW_FIRSTTAG,   // begin matching
      SWM_NSFilter_Match,                          // T if matches
      SWM_NSFilter_End,                            // end matching
      SWM_NSFilter_TrackStats,                     // keep stats
      SWM_NSFilter_UI,                             // get UI window
      SWM_NSFilter_ShowUI,                         // bring up edit UI
      SWM_NSFilter_HideUI,                         // hide UI
      SWM_NSFilter_Expire,                         // expire children
      SWM_NSFilter_ExpireList,                     // expire flts in list
      SWM_NSFilter_UpdStatWin,                     // update stat window
      SWM_NSFilter_UpdLastMatch,                   // update last match date
      SWM_NSFilter_ResetStats,                     // reset statistics
      SWM_NSFilter_ExpireSliderClass,              // get object class
      SWM_NSFilter_DisuseSliderClass,              // get object class
      SWM_NSFilter_NewImbUI,                       // new imbedded UI
      SWM_NSFilter_ImbUI,                          // imbedded UI
      SWM_NSFilter_ChildUI,                        // child`s UI
      SWM_NSFilter_AddFFIcons,                     // add filter function icons
      SWM_NSFilter_RemFFIcon,                      // called once for each
      SWM_NSFilter_FF_OnOff,                       // turn filter on/off
      SWM_NSFilter_TBUI,                           // toolbar UI
   };

   enum {
      SWV_NSFilter_ExpireNever = 0,
   };

   enum {
      SWV_NSFilter_TM_Base = 0,                    // thread modes
      SWV_NSFilter_TM_Any,
      SWV_NSFilter_TM_All,
   };

   enum {
      SWV_NSFilter_Off = 0,                        // turn filter off
      SWV_NSFilter_On,                             // turn filter on
      SWV_NSFilter_Toggle,                         // toggle filter
   };

   SWM_P(SWP_NSFilter_TBUI,        Object *Grp);
   SWM_P(SWP_NSFilter_Match,       Object *Art; Object *Grp);
   SWM_P(SWP_NSFilter_Begin,       Object *Grp);
   SWM_P(SWP_NSFilter_TrackStats,  ULONG *Total; ULONG *Found);
   SWM_P(SWP_NSFilter_ShowUI,      Object *App);
   SWM_P(SWP_NSFilter_ExpireList,  Object *L);
   SWM_P(SWP_NSFilter_NewImbUI,    Object **Filter);
   SWM_P(SWP_NSFilter_ChildUI,     Object *Obj);
   SWM_P(SWP_NSFilter_ImbUI,       Object **Filter; Object *UI; STRPTR Help);
   SWM_P(SWP_NSFilter_FF_OnOff,    Object *Grp; ULONG State; Object *NSFList);

   SWM_P(SWP_NSFilter_AddFFIcons,
         Object *Grp;
         Object *NSFList);

   SWM_P(SWP_NSFilter_RemFFIcon,
         Object *Grp;
         Object *NSFList;
         Object *Icon);

#  undef SW_FIRSTTAG

# endif
