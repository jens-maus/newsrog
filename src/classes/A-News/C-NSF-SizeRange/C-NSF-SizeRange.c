# include "C-NSF-SizeRange-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_SizeRange

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_NSF_SizeRange_Min, SWAM_NoLab, SWAM_SGI,   0),
   SWAM_IN(SWA_NSF_SizeRange_Max, SWAM_NoLab, SWAM_SGI,   10*1024),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_C "Min Size",
   MUIX_C "Max Size",
   "<=",
   MUIX_C "Article\nSize",

   "An article must be at least the minimum\n"
   "size to be selected, but not more than\n"
   "the maximum size.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static Object *MakeNumStr(void)
{
   return NewObject(SWKbsliderClass(), NULL,
                    MUIA_CycleChain,         1,
                    MUIA_Numeric_Min,        0,
                    MUIA_Numeric_Max,        5*1024*1024,
                    SWA_KbSlider_Bytes,      TRUE,
                    SWA_KbSlider_NumIncMult, 256,
                    TAG_DONE);
}


# define SWF_FilterMatch {                                \
  ULONG Size = DoMethod(msg->Art, SWM_NArticle_GetBytes); \
                                                          \
  Match = Size >= idata->SWA_NSF_SizeRange_Min &&         \
          Size <= idata->SWA_NSF_SizeRange_Max;           \
}

# define SWF_FilterUI                                                  \
   Object *n1, *n2, *ui = HGroup,                                      \
      Child, VGroup,                                                   \
         MUIA_ShortHelp, GetS(SWS_H_MinMax),                           \
         Child, n1 = MakeNumStr(),                                     \
         Child, Label2(GetS(SWS_L_MinSize)),                           \
      End,                                                             \
      Child, Label2(GetS(SWS_L_LE)),                                   \
      Child, Label2(GetS(SWS_L_ASize)),                                \
      Child, Label2(GetS(SWS_L_LE)),                                   \
      Child, VGroup,                                                   \
         MUIA_ShortHelp, GetS(SWS_H_MinMax),                           \
         Child, n2 = MakeNumStr(),                                     \
         Child, Label2(GetS(SWS_L_MaxSize)),                           \
      End,                                                             \
   End;                                                                \
                                                                       \
   set(n1, MUIA_Numeric_Value, idata->SWA_NSF_SizeRange_Min);          \
   set(n2, MUIA_Numeric_Value, idata->SWA_NSF_SizeRange_Max);          \
                                                                       \
   DoMethod(n1, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,       \
            o, 3, MUIM_Set, SWA_NSF_SizeRange_Min, MUIV_TriggerValue); \
                                                                       \
   DoMethod(n2, MUIM_Notify, MUIA_Numeric_Value, MUIV_EveryTime,       \
            o, 3, MUIM_Set, SWA_NSF_SizeRange_Max, MUIV_TriggerValue); \
   SWDB_RET(ui);


# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
