# include "C-NSF-LineRange-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_LineRange

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_NSF_LineRange_Min, SWAM_NoLab, SWAM_SGI,       5),
   SWAM_IN(SWA_NSF_LineRange_Max, SWAM_NoLab, SWAM_SGI, 1000000),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_C "Min Lines",
   MUIX_C "Max Lines",
   "<=",
   MUIX_C "Article\nLines",

   "An article must have at least the minimum\n"
   "number of lines to be selected, but not\n"
   "more than the maximum.",

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
                    MUIA_String_MaxLen, 8,
                    TAG_DONE);
}


# define SWF_FilterMatch {                                           \
  ULONG Lines;                                                       \
  STRPTR LHdr = (UBYTE *)DoMethod(msg->Art, SWM_NArticle_FindHeader, \
                                  SWF_NAHeader_Lines);               \
  if (!LHdr) Match = TRUE;                                           \
  else {                                                             \
    Lines = atol(LHdr);                                              \
    Match = (Lines >= idata->SWA_NSF_LineRange_Min &&                \
             Lines <= idata->SWA_NSF_LineRange_Max);                 \
  }                                                                  \
}

# define SWF_FilterUI                                                  \
   Object *n1, *n2, *ui = HGroup,                                      \
      Child, VGroup,                                                   \
         MUIA_ShortHelp, GetS(SWS_H_MinMax),                           \
         Child, n1 = MakeNumStr(),                                     \
         Child, Label2(GetS(SWS_L_MinLines)),                          \
      End,                                                             \
      Child, Label2(GetS(SWS_L_LE)),                                   \
      Child, Label2(GetS(SWS_L_ALines)),                               \
      Child, Label2(GetS(SWS_L_LE)),                                   \
      Child, VGroup,                                                   \
         MUIA_ShortHelp, GetS(SWS_H_MinMax),                           \
         Child, n2 = MakeNumStr(),                                     \
         Child, Label2(GetS(SWS_L_MaxLines)),                          \
      End,                                                             \
   End;                                                                \
                                                                       \
   set(n1, MUIA_String_Integer, idata->SWA_NSF_LineRange_Min);         \
   set(n2, MUIA_String_Integer, idata->SWA_NSF_LineRange_Max);         \
                                                                       \
   DoMethod(n1, MUIM_Notify, SWA_ObjString_NumVal, MUIV_EveryTime,     \
            o, 3, MUIM_Set, SWA_NSF_LineRange_Min, MUIV_TriggerValue); \
                                                                       \
   DoMethod(n2, MUIM_Notify, SWA_ObjString_NumVal, MUIV_EveryTime,     \
            o, 3, MUIM_Set, SWA_NSF_LineRange_Max, MUIV_TriggerValue); \
   SWDB_RET(ui);


# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
