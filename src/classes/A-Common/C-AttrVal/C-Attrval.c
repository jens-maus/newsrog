# include "C-Attrval-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_Attrval

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_Attrval_Attr,   SWAM_NoLab, SWAM_SGI, NULL, 16384),
   SWAM_SA(SWA_Attrval_Val,    SWAM_NoLab, SWAM_SGI, NULL, 16384),
   SWAM_IN(SWA_Attrval_Num,    SWAM_NoLab, SWAM_SGI, 0),
   
   SWP_Root_Attrs,
   SWAM_DONE
};

# define SW_Methods ;

# include "S-DefAll.h"
# include "S-Dispatcher.h"
