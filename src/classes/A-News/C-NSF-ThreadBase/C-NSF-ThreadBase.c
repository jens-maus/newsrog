# include "C-NSF-ThreadBase-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NSF_ThreadBase

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "This filter matches articles which are the base "
      "article in a thread in in the local view.  "
      "There are no options.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SWF_FilterBegin \
   idata->TM = msg->Grp ? SWGetObj(msg->Grp, SWA_NGroup_ThreadMgr) : NULL;

# define SWF_FilterMatch \
   Match = !idata->TM ? FALSE : \
      (DoMethod(idata->TM, SWM_NArticleMgr_ThreadParent, msg->Art) == NULL)

# define SWF_FilterHideUI
# define SWF_FilterUI                       \
  Object *ui = FloattextObject,             \
     TextFrame,                             \
     MUIA_Background,     MUII_TextBack,    \
     MUIA_Floattext_Text, GetS(SWS_H_TS),   \
  End;                                      \
  SWDB_RET(ui);

# include "S-DefSFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
