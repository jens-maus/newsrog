# include "C-NSF-InGroup-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_InGroup

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_BO(SWA_NSF_InGroup_FirstGrp, SWS_L_FirstGroup, SWAM_UI,   FALSE),
   SWAM_BO(SWA_NSF_InGroup_Killed,   SWAM_NoLab,       SWAM_SGIN, FALSE),

   SWP_Root_Attrs,
   SWAM_DONE
};

HelpMap SW_HELPMAP[] = {
   SWA_NSF_InGroup_FirstGrp, SWS_H_FirstGroup,
   NULL, NULL,
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "First Group:",

   "If selected, a group must be the 1st in the\n"
      "group list for matching articles.  Otherwise,\n"
      "the group may appear anywhere.  For example:\n\n"
      "   " MUIX_I "Newsgroups: rec.pets.cats,rec.pets.dogs" MUIX_N "\n\n"
      "The article will not be matched when reading\n"
      "rec.pets.dogs unless this gadget is checked.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SWF_FilterBegin \
   idata->GroupName = msg->Grp ? SWGetStr(msg->Grp, SWA_NGroup_Name) : NULL;

# define SWF_FilterMatch \
   if (Match = DoMethod(msg->Art, SWM_NArticle_InGroup, idata->GroupName))  \
     if (!idata->SWA_NSF_InGroup_Killed)                                    \
        if (DoMethod(msg->Art, SWM_NArticle_FlagTest, SWF_NArticle_Killed)) \
           Match = FALSE;


# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
