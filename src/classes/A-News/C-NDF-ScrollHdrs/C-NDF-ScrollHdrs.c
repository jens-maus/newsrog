# include "C-NDF-ScrollHdrs-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_ScrollHdrs

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IB(SWA_NDF_ScrollHdrs_Lines,     SWS_L_Lines, SWAM_UI, 3, 1, 40),
   
   SWP_NDFilter_Attrs(20),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Lines: ",

   "The size of the scrollable header block in lines.",

   NULL,
};


HelpMap SW_HELPMAP[] = {
   SWA_NDF_ScrollHdrs_Lines,     SWS_H_Lines,

   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

Library *DiskfontBase = NULL;

# define SW_USERSETUP if (!(DiskfontBase = OpenLibrary("diskfont.library", 37))) \
                         return -1;

# define SW_USERCLEANUP CloseLibrary(DiskfontBase);

# define  NDF_AddObj
# include "C-NDF-ScrollHdrs-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
