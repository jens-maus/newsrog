# include "C-NSF-SubjMatch-I.h"
# include "S-FastStrStr.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_SubjMatch

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NSF_SubjMatch_Str,        SWS_L_Str,    SWAM_UI,  NULL, 4096),
   SWAM_BO(SWA_NSF_SubjMatch_Case,       SWS_L_Case,   SWAM_UI,  TRUE),
   SWAM_BO(SWA_NSF_SubjMatch_SubStr,     SWS_L_SubStr, SWAM_UI,  FALSE),
   SWAM_BO(SWA_NSF_SubjMatch_MatchMsgID, SWAM_NoLab,   SWAM_SGI, FALSE),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "String:",
   "Match Case:",
   "SubString: ",

   "The contents of the subject field from the\n"
      "article is matched against this string.\n"
      "It must match exactly, modulo any " MUIX_I "Re: " MUIX_N "\n"
      "at the beginning.  This can be accomplished\n"
      "as well with the " MUIX_I " HeaderMatch " MUIX_N "filter, but\n"
      "this is faster and sometimes easier.",

   "If set, the match is case sensitive, else not.",

   "If set, the pattern given must match only\n"
      "a substring of the article subject.  Otherwise,\n"
      "it must match the entire subject.",

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NSF_SubjMatch_Str,     SWS_H_Str,
   SWA_NSF_SubjMatch_Case,    SWS_H_Case,
   SWA_NSF_SubjMatch_SubStr,  SWS_H_SubStr,
   
   NULL, NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SWF_FilterBegin idata->PatLen = strlen(idata->SWA_NSF_SubjMatch_Str)

# define SWF_FilterMatch                                                   \
   if (idata->SWA_NSF_SubjMatch_MatchMsgID) {                              \
      Match = !strcmp(idata->SWA_NSF_SubjMatch_Str,                        \
                      (STRPTR)DoMethod(msg->Art, SWM_NArticle_GetMsgID));  \
   } else {                                                                \
      STRPTR Hdr = (STRPTR)DoMethod(msg->Art, SWM_NArticle_GetSubj);       \
      if (!idata->SWA_NSF_SubjMatch_SubStr) {                              \
         Match = SWV_NArticle_SameSubj(Hdr, idata->SWA_NSF_SubjMatch_Str); \
      } else {                                                             \
         Match = faststrstr(Hdr, idata->SWA_NSF_SubjMatch_Str,             \
                            strlen(Hdr),                                   \
                            idata->PatLen, idata->SWA_NSF_SubjMatch_Case); \
      }                                                                    \
   }

# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
