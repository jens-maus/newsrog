# include "C-NSF-RepeatChar-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_RepeatChar

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IB(SWA_NSF_RepeatChar_MaxCount, SWS_L_MaxC,  SWAM_UI, 4, 1, 256),
   SWAM_SA(SWA_NSF_RepeatChar_Chars,    SWS_L_Chars, SWAM_UI, "?!-$", 256),
   SWAM_BO(SWA_NSF_RepeatChar_Set,      SWS_L_Set,   SWAM_UI, FALSE),

   SWP_Root_Attrs,
   SWAM_DONE
};

HelpMap SW_HELPMAP[] = {
   SWA_NSF_RepeatChar_MaxCount, SWS_H_MaxC,
   SWA_NSF_RepeatChar_Chars,    SWS_H_Chars,
   SWA_NSF_RepeatChar_Set,      SWS_H_Set,
   NULL, NULL,
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Max Count: ",
   "Characters: ",
   "Set: ",

   "This filter matches any article with more than\n"
      "a given number of repeated characters in the\n"
      "subject.  It is useful for matching posts such\n"
      "as " SWX_I("MAKE MONEY!!!!!") ", since repeated\n"
      "punctuation is rarely the mark of a useful\n"
      "article.",

   "These characters are checked for repetition.\n"
      "all others are ignored.",

   "If selected, the characters are treated as a\n"
      "set.  In this case, if " SWX_B("Max Count") " is 4 and\n"
      SWX_B("Characters") " contains " SWX_B("!?") ", then " SWX_B("FOO!?!?") " would\n"
      "match.  If unselected, " SWX_B("FOO!!!!") " or " SWX_B("FOO????\n")
      "would match but " SWX_B("FOO!?!?!") " would not.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


static void FltBegin(SW_IDATA *idata)
{
   STRPTR s = idata->SWA_NSF_RepeatChar_Chars;

   memset(idata->Chars, 0, sizeof(idata->Chars));

   while (*s) idata->Chars[*s++] = 1;
}


static __inline ULONG IsMatch(SW_IDATA *idata, Object *Art)
{
   STRPTR Subj    = (STRPTR)DoMethod(Art, SWM_NArticle_GetSubj);
   ULONG  Set     = idata->SWA_NSF_RepeatChar_Set;
   ULONG  Max     = idata->SWA_NSF_RepeatChar_MaxCount;
   UBYTE  LastChr = 0;
   ULONG  Count   = 0;

   for (; *Subj; Subj++) {
      if (!idata->Chars[*Subj] ||
          (!Set && Count>0 && LastChr != *Subj)) {
         Count=0;
         continue;
      }

      if (++Count >= Max) return TRUE;
      LastChr = *Subj;
   }
   
   return FALSE;
}

# define SWF_FilterBegin FltBegin(idata);
# define SWF_FilterMatch Match = IsMatch(idata, msg->Art);

# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
