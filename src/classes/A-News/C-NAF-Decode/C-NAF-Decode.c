# include "C-NAF-Decode-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

static STRPTR ModeLab[SWV_NAF_Decode_Mode_COUNT+1];

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NAF_Decode_FSpec,    SWS_L_FSpec,    SWAM_UI,
           "RAM:${File}", 1024),

   SWAM_CY(SWA_NAF_Decode_Mode,     SWS_L_Mode,     SWAM_UI,
           SWV_NAF_Decode_Mode_Overwrite, SWV_NAF_Decode_Mode_COUNT, ModeLab),

   SWAM_BO(SWA_NAF_Decode_MkDir,   SWS_L_MkDir,   SWAM_UI, FALSE),
      
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Filename:",

   "Save Mode:",
   "Overwrite",
   "Skip",
   "Suffix",

   "Create Dirs:",

   "The filename to which the decoded data\n"
      "should be saved.  The variable " SWX_B("${File}") " may\n"
      "be used and will be set to the filename\n"
      "found in the uuencoded or MIME data.",

   "The action to take if the target file exists:\n"
      SWX_B("  Overwrite: ") "Silently overwrites old file.\n"
      SWX_B("  Skip: ")      "Silently skips this file.\n"
      SWX_B("  Suffix: ")    "Appends a numeric suffix.",

   "If set, directories will be created if they do\n"
      "not already exist.",

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NAF_Decode_FSpec,    SWS_H_FSpec,
   SWA_NAF_Decode_Mode,     SWS_H_Mode,
   SWA_NAF_Decode_MkDir,    SWS_H_MkDir,
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods               \
  SW_DO_MF(SWM_NAFilter_Begin);   \
  SW_DO_MF(SWM_NAFilter_Action);  \
  SW_DO_MF(SWM_NAFilter_End);     \
  SW_DO_MF(SWM_NAFilter_UI);      \
  SW_DO_MF(SWM_NAFilter_HideUI);  \
  SW_DO_MF(SWM_VarList_Search);


# include "C-NAF-Decode-Mthd.h"
# include "S-DefAFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
