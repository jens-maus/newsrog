# include "C-NSF-PctCaps-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_PctCaps

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IB(SWA_NSF_PctCaps_MinLen,   SWS_L_MinLen, SWAM_UI, 8,  1, 256),
   SWAM_IB(SWA_NSF_PctCaps_Pct,      SWS_L_Pct,    SWAM_UI, 80, 1, 100),

   SWP_Root_Attrs,
   SWAM_DONE
};

HelpMap SW_HELPMAP[] = {
   SWA_NSF_PctCaps_MinLen, SWS_H_MinLen,
   SWA_NSF_PctCaps_Pct,    SWS_H_Pct,
   NULL, NULL,
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Min Length: ",
   "Cap %: ",

   "The minimum number of alphabetic characters in the\n"
      "subject field.",

   "This filter matches articles whos subject contains\n"
      "at least this percentage of capital letters.  Non\n"
      "alphabetic characters are not counted.  If set to\n"
      "50, then " SWX_I("MAKE MONEY") " and " SWX_I("MaKe MoNeY\n")
      "will both match, but " SWX_I("My Cat Sam") " will not.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


static __inline LONG IsMatch(SW_IDATA *idata, Object *Art)
{
   STRPTR Subj    = (STRPTR)DoMethod(Art, SWM_NArticle_GetSubj);
   ULONG  Alpha   = 0, Caps = 0;

   Subj = SWV_NArticle_SubjNoRe(Subj);


   for (; *Subj; Subj++) {
      if (isalpha(*Subj)) Alpha++;
      if (isupper(*Subj)) Caps++;
   }

   if (Alpha < idata->SWA_NSF_PctCaps_MinLen || !Alpha) return FALSE;

   return Caps * 100 / Alpha >= idata->SWA_NSF_PctCaps_Pct;
}

# define SWF_FilterMatch Match = IsMatch(idata, msg->Art);

# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
