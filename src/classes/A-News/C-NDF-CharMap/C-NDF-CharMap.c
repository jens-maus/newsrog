# include "C-NDF-CharMap-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NDF_CharMap_MapInfo,  SWAM_NoLab, SWAM_SGI,  NULL, 8192),

   SWP_NDFilter_Attrs(-970),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_C MUIX_B "Character Translation Table",

   "Define zero or more mappings of the form c1=c2 or c1-c2=c3-c4\n"
   "or DEL=c1.  Each character may be defined by two hex digits\n"
   "preceded by 0x (e.g, " SWX_B("0xA2") "), or 3 octal digits preceded by 0 (e.g.\n"
   SWX_B("0067") "), or a character by itself (e.g. " SWX_B("Z") ").  Whitespace and\n"
   "line breaks are ignored, so " SWX_B("a=A") " is the same as " SWX_B("a = A") ".\n\n"
   "For example, to translate lower case to upper case, use:\n"
   "   a-z = A-Z\n"
   "To reverse the case of all ASCII letters, use:\n"
   "   a-z = A-Z   A-Z = a-z\n"
   "To implement ROT-13 and change underscores to spaces:\n"
   "   a-m=n-z  n-z=a-m  A-M=N-Z  N-Z=A-M  _=0x20\n\n"
   SWX_B("##") " defines a comment through the end of the line.",
   
   NULL,
};

HelpMap SW_HELPMAP[] = {
   NULL, NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static ULONG CreateObj(Object *o, SW_IDATA *idata)
{
   setlent(o, SWA_ObjNoChildCopy, TRUE);

// DoMethod(o, OM_ADDMEMBER, idata->SWA_NList_LV);

   return SUCCESS;
}

# define SETUPCUST \
            if (CreateObj((Object *)retval, idata) != SUCCESS) \
               SWDB_RET(NULL);

# define SW_Methods                  \
  SW_DO_MF(SWM_NDFilter_ModTxt);     \
  SW_DO_MF(SWM_NDFilter_StartMatch); \
  SW_DO_MF(SWM_NDFilter_EndMatch);   \
  SW_DO_MF(SWM_NDFilter_UI);         \
  SW_DO_MF(SWM_NDFilter_HideUI);     \

# include "C-NDF-CharMap-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
