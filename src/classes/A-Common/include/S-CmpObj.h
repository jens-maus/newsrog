# ifndef SW_CMPOBJ_INCLUDE
#   define SW_CMPOBJ_INCLUDE

   enum {
      SWV_CmpMaxAttr = 5,
   };

   struct  SWS_CmpInfo {
      ULONG    Count;                          // count of attrs
      ULONG    Attrs[SWV_CmpMaxAttr];          // compare attributes
      ULONG    Dirs[SWV_CmpMaxAttr];           // compare directions
   };

   SWM_P(SWP_Cmp,                              // compare 2 entries
         Object *Entry;                        // entry to compare with
         struct SWS_CmpInfo *CmpInfo);         // compare info

   enum {
      SWV_CmpForwardSort = 0,                  // forward sort
      SWV_CmpReverseSort,                      // reverse sort
   };

# define SWAM_SRT(m, ad, num, t, s, d)                      \
   SWAM(m, offsetof(SW_IDATA, CmpInfo.ad[num]),             \
        sizeof(ULONG), t, s, d, LONG_MIN, LONG_MAX, SWAM_NoLab)

# endif
