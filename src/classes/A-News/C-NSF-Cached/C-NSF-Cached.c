# include "C-NSF-Cached-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_Cached

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "This filter matches articles that have been cached, "
      "but are not marked permanent.  There are no options.",
   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SWF_FilterMatch                                                \
   Match = (DoMethod(msg->Art, SWM_NArticle_FlagTest,                   \
                     SWF_NArticle_CachedBody|SWF_NArticle_Permanent) == \
                SWF_NArticle_CachedBody)

# define SWF_FilterHideUI
# define SWF_FilterUI                         \
  Object *ui = FloattextObject,               \
     TextFrame,                               \
     MUIA_Background,     MUII_TextBack,      \
     MUIA_Floattext_Text, GetS(SWS_H_Cached), \
  End;                                        \
  SWDB_RET(ui);

# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
