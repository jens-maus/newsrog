# include "C-NSF-ThreadCount-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_ThreadCount

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_NSF_ThreadCount_Min, SWAM_NoLab, SWAM_SGI,   1),
   SWAM_IN(SWA_NSF_ThreadCount_Max, SWAM_NoLab, SWAM_SGI,  50),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_C "Min Articles",
   MUIX_C "Max Articles",
   "<=",
   MUIX_C "Thread\nArticle Count",

   "An article will be selected if it is in\n"
   "a thread containing at least the minimum,\n"
   "but not more than the maximum number of\n"
   "total articles.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static Object *MakeNumStr(void)
{
   return NewObject(SWObjStringClass(), NULL,
                    StringFrame,
                    MUIA_CycleChain,    1,
                    MUIA_String_Accept, "0123456789",
                    MUIA_String_MaxLen, 4,
                    TAG_DONE);
}


# define SWF_FilterMatch {                                   \
  ULONG Count = DoMethod(msg->Art, SWM_NArticle_GroupCount); \
                                                             \
  Match = (Count >= idata->SWA_NSF_ThreadCount_Min &&         \
           Count <= idata->SWA_NSF_ThreadCount_Max);          \
}

# define SWF_FilterUI                                                   \
   Object *n1, *n2, *ui = HGroup,                                       \
      Child, VGroup,                                                    \
         MUIA_ShortHelp, GetS(SWS_H_MinMax),                            \
         Child, n1 = MakeNumStr(),                                      \
         Child, Label2(GetS(SWS_L_Min)),                                \
      End,                                                              \
      Child, Label2(GetS(SWS_L_LE)),                                    \
      Child, Label2(GetS(SWS_L_AGroups)),                               \
      Child, Label2(GetS(SWS_L_LE)),                                    \
      Child, VGroup,                                                    \
         MUIA_ShortHelp, GetS(SWS_H_MinMax),                            \
         Child, n2 = MakeNumStr(),                                      \
         Child, Label2(GetS(SWS_L_Max)),                                \
      End,                                                              \
   End;                                                                 \
                                                                        \
   set(n1, MUIA_String_Integer, idata->SWA_NSF_ThreadCount_Min);         \
   set(n2, MUIA_String_Integer, idata->SWA_NSF_ThreadCount_Max);         \
                                                                        \
   DoMethod(n1, MUIM_Notify, SWA_ObjString_NumVal, MUIV_EveryTime,      \
            o, 3, MUIM_Set, SWA_NSF_ThreadCount_Min, MUIV_TriggerValue); \
                                                                        \
   DoMethod(n2, MUIM_Notify, SWA_ObjString_NumVal, MUIV_EveryTime,      \
            o, 3, MUIM_Set, SWA_NSF_ThreadCount_Max, MUIV_TriggerValue); \
   SWDB_RET(ui);


# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
