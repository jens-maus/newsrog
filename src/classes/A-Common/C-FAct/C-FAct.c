# include "C-FAct-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_FAct

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_FAct_Type,   SWAM_NoLab, SWAM_SGI, NULL, 256),
   SWAM_SA(SWA_FAct_Action, SWAM_NoLab, SWAM_SGI, NULL, 256),
   SWP_SysCmdAttrs(SWA_FAct, "","", TRUE),
   SWAM_IN(SWA_FAct_CmdType, SWAM_NoLab, SWAM_SGI, SWV_FAct_CLI),

   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Media Type: ",
   "Action: ",

   "The media type for this action.  Patterns\n"
      "may be used, e.g, " SWX_B("image/*"),

   "The name of this action.  Any user\n"
      "defined string is permitted.",
   
   SWP_SysCmdStrs,

   "Command to run",
   "Arguments to command",
   "Input file",
   "Output file",
   "Stack Size",
   "Priority",
   "Directory",
   "If set, run async.",

   SWX_B("Error: ") "Action `%s' for type\n`%s'has no command.",
   SWX_B("Error: ") "Unable to send ARexx command for\naction `%s', type `%s'.",
   
   NULL,
};


HelpMap SW_HELPMAP[] = {
   SWP_SysCmdHelp(SWA_FAct),
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods \
   SW_DO_MF(SWM_FAct_Execute); \


# include "C-FAct-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
