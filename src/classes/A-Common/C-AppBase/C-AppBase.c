# include "C-AppBase-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_Barnone

// -- attribute map ----------------------------------------------------------

static STRPTR EMLab[SWV_AppBase_EM_MAX+1];

static SWAttrMapT SW_ATTRMAP[] = {
   SWAM_PT(SWA_AppBase_Reg,          SWAM_NoLab,  SWAM_SGIN, NULL),
   SWAM_PT(SWA_AppBase_App,          SWAM_NoLab,  SWAM_SGIN, NULL),
   SWAM_PT(SWA_AppBase_Win,          SWAM_NoLab,  SWAM_SGIN, NULL),
   SWAM_IN(SWA_AppBase_Cost1,        SWAM_NoLab,  SWAM_SGIN, 20),
   SWAM_IN(SWA_AppBase_Cost2,        SWAM_NoLab,  SWAM_SGIN, 26),
   SWAM_PT(SWA_AppBase_AppName,      SWAM_NoLab,  SWAM_SGIN, "TestApp"),
   SWAM_BO(SWA_AppBase_UnregSave,    SWAM_NoLab,  SWAM_SGIN, TRUE),
   SWAM_BO(SWA_AppBase_ProjMod,      SWAM_NoLab,  SWAM_SGIN, FALSE),
   SWAM_SA(SWA_AppBase_LastSaveFile, SWAM_NoLab,  SWAM_SGIN, NULL, FMSIZE),

   SWP_AppBase_IntAttrs,
   SWAM_BO(SWA_AppBase_Clobber,      SWS_L_Clobber,  SWAM_UI, FALSE),
   SWAM_CY(SWA_AppBase_ExitMode,     SWS_L_ExitMode, SWAM_UI,
           SWV_AppBase_EM_Ask,       SWV_AppBase_EM_MAX, EMLab),
   SWAM_SA(SWA_AppBase_BackSuffix,   SWS_L_BackSfx,  SWAM_UI, ".bak", 16),

   SWP_Root_Attrs,
   SWAM_DONE
};


HelpMap SW_HELPMAP[] = {
   SW_AppBase_HelpMap,

   SWA_AppBase_ExitMode,   SWS_H_ExitMode,
   SWA_AppBase_Clobber,    SWS_H_Clobber,
   SWA_AppBase_BackSuffix, SWS_H_BackSfx,
   NULL, NULL,
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Popkey: ",
   "Save Project Icons: ",
   "Popkey to make the main editor window appear.",
   "If set, project icons will be created.",
   "Invalid hotkey: '%s'",
   "Error creating CxObj",
   "No previous filename.",
   "The current project has been modified.\n"
      "Any changes will be lost.",
   "_Ok|*_Cancel|_Save",

   "The unregistered demo version of this program\n"
      "does not permit saving or loading.  Please\n"
      "consider buying the commercial version for\n"
      "the complete functionality.",
#  ifndef DEMOVERSION
   "_OK|*_Register",
#  else
   "*_OK",
#  endif

   "Exit Mode: ",
      "Ask",
      "Save",
      "Don't Save",
   "This controls the exit behavior when there\n"
      "are unsaved changes.  " SWX_I("Save") " saves\n"
      "to the current project.  " SWX_I ("Don't Save") " quits\n"
      "without saving the changes.  " SWX_I("Ask") " prompts\n"
      "at exit time.",

    "Clobber: ",
      "If set, saved projects will silently overwrite\n"
      "files when the project is written to a new\n"
      "file.",

    "Backup Suffix: ",
      "If non-null, this string is appended to\n"
      "filenames to create backup versions when\n"
      "overwriting projects.",

   NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SETUPCUST    SW_AppBase_Setup((Object *)retval, idata);
# define DISPOSECUST  SW_AppBase_Dispose(idata);
# define SW_USERSET   SW_AppBase_Set(o, idata, Tag, Data)

# define SW_Methods                             \
     SW_DO_MF(SWM_AppBase_MenuCall);            \
     SW_DO_MF(SWM_AppBase_ShowAppUI);           \
     SW_DO_MF(SWM_AppBase_HideAppUI);           \
     SW_DO_MF(SWM_AppBase_AppMessage);          \
     SW_DO_MF(SWM_AppBase_AttachHotKey);        \
     SW_DO_MF(SWM_AppBase_RemoveHotKey);        \
     SW_DO_MF(SWM_AppBase_UnIconify);           \
     SW_DO_MF(SWM_AppBase_NewProj);             \
     SW_DO_MF(SWM_AppBase_M_Open);              \
     SW_DO_MF(SWM_AppBase_M_Save);              \
     SW_DO_MF(SWM_AppBase_M_SaveAs);            \
     SW_DO_MF(SWM_AppBase_M_SaveAsDef);         \
     SW_DO_MF(SWM_AppBase_M_New);               \
     SW_DO_MF(SWM_AppBase_M_Register);          \
     SW_DO_MF(SWM_AppBase_M_AboutMUI);          \
     SW_DO_MF(SWM_AppBase_M_Hide);              \
     SW_DO_MF(SWM_AppBase_M_Quit);              \
     SW_DO_MF(SWM_AppBase_M_RestoreDf);         \
     SW_DO_MF(SWM_AppBase_M_LastSv);            \
     SW_DO_MF(SWM_AppBase_M_MUI);               \
     SW_DO_MF(SWM_AppBase_UI);                  \
     SW_DO_MF(SWM_AppBase_UnregMsg);            \
     SW_DO_MF(SWM_AppBase_LoadProj);            \
     SW_DO_MF(SWM_AppBase_SaveProj);            \
     SW_DO_MF(SWM_AppBase_PreNewProj);          \
     SW_DO_MF(SWM_AppBase_PostNewProj);         \
     SW_DO_MF(SWM_AppBase_PostSaveProj);        \
     SW_DO_MF(SWM_AppBase_RexxCmd);             \
     SW_DO_MF(SWM_AppBase_Rexx_ProjFileName);   \
     SW_DO_MF(SWM_AppBase_Rexx_ProjIsModified); \
     SW_DO_MF(SWM_Root_RequestFNData);          \
     SW_DO_MF(SWM_Root_AssignSafe);             \
     SW_DO_MF(SWM_Save);                        \
     SW_DO_MF(SWM_Load);                        \

# include "S-PutIcon.h"
# include "C-AppBase-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
