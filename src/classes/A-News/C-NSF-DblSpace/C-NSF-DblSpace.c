# include "C-NSF-DblSpace-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_DblSpace

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IB(SWA_NSF_DblSpace_MinLen,   SWS_L_MinLen, SWAM_UI, 8,  1, 256),
   SWAM_IB(SWA_NSF_DblSpace_Pct,      SWS_L_Pct,    SWAM_UI, 80, 1, 100),
   SWAM_SA(SWA_NSF_DblSpace_Chars,    SWS_L_Chars,  SWAM_UI, " *", 80),

   SWP_Root_Attrs,
   SWAM_DONE
};

HelpMap SW_HELPMAP[] = {
   SWA_NSF_DblSpace_MinLen, SWS_H_MinLen,
   SWA_NSF_DblSpace_Pct,    SWS_H_Pct,
   SWA_NSF_DblSpace_Chars,  SWS_H_Chars,
   NULL, NULL,
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Min Length: ",
   "Min DblSpace %: ",
   "DblSpace Chars: ",

   "The minimum number of characters in the\n"
      "subject field.  Articles with subjects\n"
      "shorter than this are not matched.",

   "This filter matches articles whos subject contains\n"
      "at least this % of doublespaced characters.  For\n"
      "example: T h i s  P r o b a b l y  I s  S p a m !",

   "Characters in this set are counted as doublespace\n"
      "characters.  For example, if this set contains\n"
      "a space and an asterisk (*), then:\n\n"
      "  T*h*i*s I*s S*p*a*m*\n"
      "  A n d  t h i s  t o o",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SWF_FilterBegin {                                    \
    ULONG x;                                                  \
    memset(idata->DblChars, 0, 256);                          \
    idata->DblChars['\t'] = 1;                                \
    for (x=0; idata->SWA_NSF_DblSpace_Chars[x]; x++)          \
       idata->DblChars[idata->SWA_NSF_DblSpace_Chars[x]] = 1; \
  }

static __inline LONG IsMatch(SW_IDATA *idata, Object *Art)
{
   STRPTR Subj    = (STRPTR)DoMethod(Art, SWM_NArticle_GetSubj);
   ULONG  Len   = 0, Dbl = 0, Cnt;

#  define IsWhite(c) (idata->DblChars[c])

   Subj = SWV_NArticle_SubjNoRe(Subj);

   for (; *Subj; Subj++, Len++)
      if (!IsWhite(Subj[0]) && IsWhite(Subj[1]))
         for (Dbl++, Cnt=0; IsWhite(Subj[1]) && Cnt++ < 3;)
            Len++, Subj++, Dbl++;

   if (Len < idata->SWA_NSF_DblSpace_MinLen || !Len) return FALSE;

   return Dbl * 100 / Len >= idata->SWA_NSF_DblSpace_Pct;
}

# define SWF_FilterMatch Match = IsMatch(idata, msg->Art);

# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
