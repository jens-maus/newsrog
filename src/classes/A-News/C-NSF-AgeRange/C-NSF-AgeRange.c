# include "C-NSF-AgeRange-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_AgeRange

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_NSF_AgeRange_Min, SWAM_NoLab, SWAM_SGI,   0),
   SWAM_IN(SWA_NSF_AgeRange_Max, SWAM_NoLab, SWAM_SGI,   5*24*60*60),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_C "Min Age",
   MUIX_C "Max Age",
   "<=",
   MUIX_C "Article\nAge",

   "An article must be at least the minimum\n"
   "age (d hh:mm:ss) to be selected, but not\n"
   "more than the maximum age.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static Object *MakeNumStr(void)
{
   return NewObject(SWTimesliderClass(), NULL,
                    MUIA_CycleChain,    1,
                    SWA_TimeSlider_NoDiv1000,  TRUE,
                    SWA_TimeSlider_NumIncMult, 60*60,
                    MUIA_Numeric_Min,   0,
                    MUIA_Numeric_Max,   30*24*60*60,
                    TAG_DONE);
}


# define SWF_FilterMatch {                                                       \
  ULONG Age = DoMethod(msg->Art, SWM_NArticle_Age, NULL);                        \
                                                                                 \
  Match = (Age >= (idata->SWA_NSF_AgeRange_Min / SWV_NArticle_SecsPerAgeTick) && \
           Age <= (idata->SWA_NSF_AgeRange_Max / SWV_NArticle_SecsPerAgeTick));  \
}

# define SWF_FilterUI                                                 \
   Object *n1, *n2, *ui = HGroup,                                     \
      Child, VGroup,                                                  \
         MUIA_ShortHelp, GetS(SWS_H_MinMax),                          \
         Child, n1 = MakeNumStr(),                                    \
         Child, Label2(GetS(SWS_L_MinAge)),                           \
      End,                                                            \
      Child, Label2(GetS(SWS_L_LE)),                                  \
      Child, Label2(GetS(SWS_L_AAge)),                                \
      Child, Label2(GetS(SWS_L_LE)),                                  \
      Child, VGroup,                                                  \
         MUIA_ShortHelp, GetS(SWS_H_MinMax),                          \
         Child, n2 = MakeNumStr(),                                    \
         Child, Label2(GetS(SWS_L_MaxAge)),                           \
      End,                                                            \
   End;                                                               \
                                                                      \
   set(n1, MUIA_Numeric_Value, idata->SWA_NSF_AgeRange_Min);          \
   set(n2, MUIA_Numeric_Value, idata->SWA_NSF_AgeRange_Max);          \
                                                                      \
   DoMethod(n1, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,      \
            o, 3, MUIM_Set, SWA_NSF_AgeRange_Min, MUIV_TriggerValue); \
                                                                      \
   DoMethod(n2, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,      \
            o, 3, MUIM_Set, SWA_NSF_AgeRange_Max, MUIV_TriggerValue); \
   SWDB_RET(ui);


# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
