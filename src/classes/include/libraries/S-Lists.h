# ifndef C_NLIST_INCLUDE
#  define C_NLIST_INCLUDE

#  define SW_FIRSTTAG         (int)(TAG_USER + 20000)

   enum {
      SWA_NList_L          = (LONG)SW_FIRSTTAG,  // [ G ] list object attribute
      SWA_NList_LV,                              // [ G ] listview object
      SWA_NList_Obj,                             // [ G ] ptr back to object
      SWA_NList_Master,                          // [ G ] ptr to master list, or NULL
      SWA_NList_Tracked,                         // [ G ] tracked lists
      SWA_NList_Changed,                         // [ G ] for notifcations.
      SWA_NList_NoNotifies,                      // [  I] don't do notifies
   };
   
   enum {
      SWM_NList_ListMethod = (LONG)SW_FIRSTTAG,  // listmethod
      SWM_NList_ListMethodN,                     // listmethod w/ notify
      SWM_NList_SetNotify,                       // reset all notifications
      SWM_NList_KillNotify,                      // kill all notifications
      SWM_NList_FindObj,                         // find an object by name
      SWM_NList_SendCmd,                         // send command to obj
      SWM_NList_Change,                          // change active entry
      SWM_NList_Add,                             // add new entry
      SWM_NList_RemoveObj,                       // remove by object
      SWM_NList_AddTrackedList,                  // add tracked list
      SWM_NList_RemTrackedList,                  // remove tracked list
      SWM_NList_DoTracked,                       // for for all tracked lists
      SWM_NList_AddObj,                          // add obj
      SWM_NList_ObjMethod,                       // send method to all objs
      SWM_NList_Clone,                           // insert clone of active obj
      SWM_NList_ObjMethodA,                      // non-stack ObjMethod
      SWM_NList_ClickCol,                        // click in a column
      SWM_NList_SelChange,                       // select change
      SWM_NList_RemoveSel,                       // remove selected
      SWM_NList_ListSet,                         // set for all in list
   };

   enum {
      SWT_NList_Attr = (LONG)SW_FIRSTTAG,  // tags for SendCmd
      SWT_NList_Val,
   };

   enum {
      SWV_NList_PrefsVer   = 1,
      SWV_NPost_ObjStrInt  = (SW_FIRSTTAG),
   };

   SWM_P(SWP_NList_ClickCol,       ULONG Column);
   SWM_P(SWP_NList_Change,         LONG Active);
   SWM_P(SWP_NList_FindObj,        ULONG Attr; UBYTE *Val);
   SWM_P(SWP_NList_RemoveObj,      Object *o);
   SWM_P(SWP_NList_Add,            Object *o; ULONG UData);
   SWM_P(SWP_NList_AddTrackedList, Object *list);
   SWM_P(SWP_NList_RemTrackedList, Object *list);
   SWM_P(SWP_NList_ObjMethodA,     Msg msg);
   SWM_P(SWP_NList_ListSet,        ULONG Attr; ULONG Val);

   enum {
      SWF_NList_RecurFlag    = (1<<0),
      SWF_NList_DelRecurFlag = (1<<1),
      SWF_NList_SetRecurFlag = (1<<2),
      SWF_NList_MasterFlag   = (1<<3),
      SWF_NList_NoSetNotify  = (1<<4),
      SWF_NList_NoNotifies   = (1<<5),
   };

   typedef struct _sw_nlisttrackattrs {
      Object *Gad;
      ULONG  ObjAttr;
      ULONG  GadAttr;
      ULONG  UnsetVal;
   } SW_NListTrackA;

#  ifdef SW_NListTrackAttrNum
#     define SWP_NList_AttrTrackAttrs                       \
         SW_NListTrackA  SW_NListTRA[SW_NListTrackAttrNum]; \
         Object         *SWA_NListLastTLA;
#  else
#     define SWP_NList_AttrTrackAttrs 
#  endif

#  ifdef SWV_NList_TrackedMax
#    define SWP_NList_AttrDefs  SWP_NList_AttrTrackAttrs                     \
                                Object *SWA_NList_LV, *SWA_NList_L;          \
                                Object *SWA_NList_Master;                    \
                                Object *SWA_NList_Obj;                       \
                                ULONG  NLFlags;                              \
                                ULONG  CCLastVal, CCLastCol;                 \
                                ULONG  A4;                                   \
                                struct Hook DspHook,CstHook,DstHook,CmpHook; \
                                Object *SWA_NList_Tracked[SWV_NList_TrackedMax]

#    define SWP_NList_Attrs \
        SWAM_PT(SWA_NList_L,       SWAM_NoLab, SWAM_SGIN,  NULL), \
        SWAM_PT(SWA_NList_LV,      SWAM_NoLab, SWAM_SGIN,  NULL), \
        SWAM_PT(SWA_NList_Tracked, SWAM_NoLab, SWAM_SGIN,  NULL), \
        SWAM_PT(SWA_NList_Master,  SWAM_NoLab, SWAM_SGIN,  NULL), \
        SWAM_FI(SWA_NList_Changed, SWAM_NoLab, SWAM_SGIN, FALSE)
# else
#    define SWP_NList_AttrDefs  SWP_NList_AttrTrackAttrs            \
                                Object *SWA_NList_LV, *SWA_NList_L; \
                                Object *SWA_NList_Obj;              \
                                ULONG  NLFlags;                     \
                                ULONG  CCLastVal, CCLastCol;        \
                                ULONG  A4;                          \
                                struct Hook DspHook,CstHook,DstHook,CmpHook

#    define SWP_NList_Attrs \
        SWAM_PT(SWA_NList_L,       SWAM_NoLab, SWAM_SGIN, NULL), \
        SWAM_PT(SWA_NList_LV,      SWAM_NoLab, SWAM_SGIN, NULL), \
        SWAM_FI(SWA_NList_Changed, SWAM_NoLab, SWAM_SGIN, FALSE)

# endif

# undef SW_FIRSTTAG
