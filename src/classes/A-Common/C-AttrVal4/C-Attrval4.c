# include "C-Attrval4-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_Attrval4

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_Attrval4_Attr,   SWAM_NoLab, SWAM_SGI, NULL, 16384),
   SWAM_SA(SWA_Attrval4_Val0,   SWAM_NoLab, SWAM_SGI, NULL, 16384),
   SWAM_SA(SWA_Attrval4_Val1,   SWAM_NoLab, SWAM_SGI, NULL, 16384),
   SWAM_SA(SWA_Attrval4_Val2,   SWAM_NoLab, SWAM_SGI, NULL, 16384),
   SWAM_SA(SWA_Attrval4_Val3,   SWAM_NoLab, SWAM_SGI, NULL, 16384),
   SWAM_IN(SWA_Attrval4_Num0,   SWAM_NoLab, SWAM_SGI, 0),
   SWAM_IN(SWA_Attrval4_Num1,   SWAM_NoLab, SWAM_SGI, 0),
   SWAM_IN(SWA_Attrval4_Num2,   SWAM_NoLab, SWAM_SGI, 0),
   SWAM_IN(SWA_Attrval4_Num3,   SWAM_NoLab, SWAM_SGI, 0),
   
   SWP_Root_Attrs,
   SWAM_DONE
};

# define SW_Methods ;

# include "S-DefAll.h"
# include "S-Dispatcher.h"
