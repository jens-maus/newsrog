# include "C-NAFilter-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NAFilter

// -- attribute map ----------------------------------------------------------

// keep Desc as 1st attr

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NAFilter_Desc,         SWAM_NoLab, SWAM_SGI, NULL, 256),
   SWAM_BO(SWA_NAFilter_Enabled,      SWAM_NoLab, SWAM_SGI, TRUE),
   SWAM_BO(SWA_NAFilter_MarkRead,     SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_SA(SWA_NAFilter_Groups,       SWAM_NoLab, SWAM_SGI, NULL, 4096),
   SWAM_SA(SWA_NAFilter_AuthNote,     SWAM_NoLab, SWAM_SGI, NULL, LONG_MAX),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Action Filter",
   "Global Info",
   "Filter Settings",
   "Author Note",

   "Description:",
   "Enabled:",
   "Mark Read: ",

   "A description string displayed in\n"
   "filter lists.  Use any meaningful\n"
   "string.",

   "If set, this filter is enabled.",

   "If set, each article will be marked\n"
   "as read after it  is processed.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static void SetupObj(Object *o, SW_IDATA *idata)
{
   setlent(o, SWA_ObjNoChildCopy, TRUE);
}

# define SETUPCUST   SetupObj((Object *)retval, idata)
# define DISPOSECUST DoMethod(o, SWM_NAFilter_HideUI)

# define SW_Methods                    \
  SW_DO_MF(SWM_NAFilter_UI);           \
  SW_DO_MF(SWM_NAFilter_ShowUI);       \
  SW_DO_MF(SWM_NAFilter_HideUI);       \
  SW_DO_MF(SWM_NAFilter_Begin);        \
  SW_DO_MF(SWM_NAFilter_Action);       \
  SW_DO_MF(SWM_NAFilter_End);          \

# include "C-NAFilter-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
