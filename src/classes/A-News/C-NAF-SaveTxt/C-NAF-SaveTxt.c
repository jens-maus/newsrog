# include "C-NAF-SaveTxt-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

static STRPTR ModeLab[SWV_NAF_SaveTxt_Mode_COUNT+1];
static STRPTR HMLab[SWV_NAF_SaveTxt_HM_COUNT+1];
static STRPTR RegLab[3];

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewRG,  SWS_L_L0, 2, RegLab),

   SWAM_UICTL(SWAM_NewCG2, 0, 0, 0),

   SWAM_SA(SWA_NAF_SaveTxt_FSpec,    SWS_L_FSpec,    SWAM_UI,
           "RAM:Art_${Num}.txt", 1024),
   
   SWAM_CY(SWA_NAF_SaveTxt_HdrMode,  SWS_L_HdrMode,  SWAM_UI,
           SWV_NAF_SaveTxt_HM_All, SWV_NAF_SaveTxt_HM_COUNT, HMLab),

   SWAM_CY(SWA_NAF_SaveTxt_Mode,     SWS_L_Mode,     SWAM_UI,
           SWV_NAF_SaveTxt_Mode_Overwrite, SWV_NAF_SaveTxt_Mode_COUNT, ModeLab),

   SWAM_BO(SWA_NAF_SaveTxt_MkDir,   SWS_L_MkDir,   SWAM_UI, FALSE),

   SWAM_UICTL(SWAM_EndG, 0, 0, 0),

   SWAM_UICTL(SWAM_NewCG2, 0, 0, 0),

   SWP_SysCmdAttrs(SWA_NAF_SaveTxt_Cmd, "", "", TRUE),
   SWAM_BO(SWA_NAF_SaveTxt_DelFile, SWS_L_DelFile, SWAM_UI, FALSE),

   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
      
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Filename:",

   "Save Parts:",
   "Header+Body",
   "Header",
   "Body",

   "Save Mode:",
   "Overwrite",
   "Append",
   "Skip",
   "Suffix",

   SWP_SysCmdStrs,
   "Create Dirs:",
   "Delete File:",

   "The filename to which the article will be saved.\n"
      "The variable " SWX_I("${Num}") " will be set to 0001\n"
      "for the first file saved, 0002 for the second,\n"
      "and so on.  Group and article variables may also\n"
      "be used (e.g. " SWX_I("RAM:${GroupAbbrev}/Art_${Num}.txt") "\n"
      "might generate the file 'RAM:c.s.a.misc/Art_0001.txt' if\n"
      "Create Dirs is set.",

   "Which parts of the article to save:\n"
      SWX_B("  Header+Body: ") "Save both parts.\n"
      SWX_B("  Header: ")      "Save only the headers.\n"
      SWX_B("  Body: ")        "Save only the body.\n",

   "The action to take if the target file exists:\n"
      SWX_B("  Overwrite: ") "Silently overwrites old file.\n"
      SWX_B("  Append: ")    "Appends new contents to old file.\n"
      SWX_B("  Skip: ")      "Silently skips this file.\n"
      SWX_B("  Suffix: ")    "Appends a numeric suffix.",

   "Command to invoke on saved\nfile, or none if blank.",
   "Command arguments.  Variable " SWX_B("${File}") " will be\n"
     "set to the most recent saved filename.",
   "Input filename (none if null).",
   "Output filename (none if null).",
   "Stack size",
   "Task priority.",
   "Current directory.",
   "If set, the command is run in the background, and\n"
      "execution cannot proceed until the command\n"
      "terminates.",

   "If set, directories will be created if they do\n"
      "not already exist.",

   "If set, the saved file will be deleted after\n"
      "the command is run.  This only makes sense\n"
      "if ASync is turned off.",

   "Save Prefs",
   "Cmd Prefs",

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NAF_SaveTxt_FSpec,    SWS_H_FSpec,
   SWA_NAF_SaveTxt_Mode,     SWS_H_Mode,
   SWA_NAF_SaveTxt_HdrMode,  SWS_H_HdrMode,
   SWA_NAF_SaveTxt_MkDir,    SWS_H_MkDir,
   SWA_NAF_SaveTxt_DelFile,  SWS_H_DelFile,
   SWP_SysCmdHelp(SWA_NAF_SaveTxt_Cmd),
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
  SW_DO_MF(SWM_NAF_SaveTxt_Save); \
  SW_DO_MF(SWM_NAF_SaveTxt_Exec); \
  SW_DO_MF(SWM_VarList_Search);


# include "C-NAF-SaveTxt-Mthd.h"
# include "S-DefAFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
