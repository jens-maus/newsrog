# ifndef C_APPBASE_INCLUDE
#   define C_APPBASE_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"

   // -- attributes that this class understands ------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 7300)

   enum {
      SWA_AppBase_Reg = SW_FIRSTTAG,         // [SGI] register object
      SWA_AppBase_DefSaveFile,               // [ G ] default save file
      SWA_AppBase_LastFile,                  // [ G ] last requestor dir file
      SWA_AppBase_LastDir,                   // [SGI] last requestor dir path
      SWA_AppBase_LastPatt,                  // [SGI] last requestor pattern
      SWA_AppBase_SaveProjIcons,             // [SGI] save project icons?
      SWA_AppBase_PopKey,                    // [SGI] popkey for main editor
      SWA_AppBase_App,                       // [SGI] ptr to application obj
      SWA_AppBase_Win,                       // [SGI] ptr to application win
      SWA_AppBase_Cost1,                     // [SGI] prog cost
      SWA_AppBase_Cost2,                     // [SGI] prog cost
      SWA_AppBase_AppName,                   // [SGI] app name
      SWA_AppBase_TopObj,                    // [SGI] ptr to topmost object
      SWA_AppBase_UnregSave,                 // [SGI] T if unreg ver can save
      SWA_AppBase_ProjMod,                   // [SGI] project modified?
      SWA_AppBase_ExitMode,                  // [SGI] exit mode
      SWA_AppBase_Clobber,                   // [SGI] silent clobber
      SWA_AppBase_BackSuffix,                // [SGI] backup suffix
      SWA_AppBase_LastSaveFile,              // [SGI] proj last saved to this
   };


   // -- messages that this class understands --------------------------------

   enum {
      SWM_AppBase_MenuCall = SW_FIRSTTAG,    // call menu function
      SWM_AppBase_ShowAppUI,                 // show app ui
      SWM_AppBase_HideAppUI,                 // hide app ui
      SWM_AppBase_NewProj,                   // clear old project (from menu)
      SWM_AppBase_AppMessage,                // process app message
      SWM_AppBase_AttachHotKey,              // attach hotkey
      SWM_AppBase_RemoveHotKey,              // remove hotkey
      SWM_AppBase_UnIconify,                 // uniconify app
      SWM_AppBase_M_Open,                    // menu functions...
      SWM_AppBase_M_Save,
      SWM_AppBase_M_SaveAs,
      SWM_AppBase_M_SaveAsDef,
      SWM_AppBase_M_New,
      SWM_AppBase_M_Register,
      SWM_AppBase_M_About,
      SWM_AppBase_M_AboutMUI,
      SWM_AppBase_M_Hide,
      SWM_AppBase_M_Quit,
      SWM_AppBase_M_RestoreDf,
      SWM_AppBase_M_LastSv,
      SWM_AppBase_M_MUI,
      SWM_AppBase_UI,                        // return edit UI
      SWM_AppBase_UnregMsg,                  // unregistered message
      SWM_AppBase_LoadProj,                  // load a project file
      SWM_AppBase_PreNewProj,                // pre new project
      SWM_AppBase_PostNewProj,               // post new project hook
      SWM_AppBase_PostSaveProj,              // post save project hook
      SWM_AppBase_RexxCmd,                   // ARexx command
      SWM_AppBase_SaveProj,                  // save a project file
      SWM_AppBase_Rexx_ProjFileName,         // rexx cmd: last save file
      SWM_AppBase_Rexx_ProjIsModified,       // rexx cmd: proj is modified?
   };

   SWM_P(SWP_AppBase_RexxCmd,                // rexx command method
         struct RexxMsg *Rx;                 // the rexx message itself
         ULONG *rc;                          // return code from msg
         Object *App);                       // application object

   SWM_P(SWP_AppBase_MenuCall, ULONG Call; Object *Win);
   SWM_P(SWP_AppBase_MenuFn,   Object *Win; ULONG Mode);
   SWM_P(SWP_AppBase_LoadProj, STRPTR ProjFile; Object *Win);
   SWM_P(SWP_AppBase_SaveProj, STRPTR ProjFile; Object *Win);
   SWM_P(SWP_AppBase_PostSaveProj, STRPTR ProjFile);
   SWM_P(SWP_AppBase_UnregMsg, STRPTR Mesg; Object *Win);

   SWM_P(SWP_AppBase_AttachHotKey,
         STRPTR Key;         // hotkey spec
         ULONG OldID;        // old ID
         Object *o;          // object to signal
         ULONG Method);      // method to send to obj

   SWM_P(SWP_AppBase_RemoveHotKey,  LONG  ID);

   typedef struct _apcxctrl {
      ULONG    ID;           // hotkey ID
      Object   *o;           // object to send message to
      ULONG     Method;      // message to send
      CxObj     *cxobj;      // commodities obj
   } AbCxCtrl;

   enum {
      SWV_AppBase_RexxNotFound = 0,
      SWV_AppBase_RexxFound,
   };

   enum {
      SWV_AppBase_Mode_Menu = 0,   // normal menu invocation
      SWV_AppBase_Mode_Script,     // invocation from script
      SWV_AppBase_Mode_Force,      // FORCE=Yes (don`t ask questions)
   };

   enum {
      SWV_AppBase_EM_Ask = 0,
      SWV_AppBase_EM_Save,
      SWV_AppBase_EM_NoSave,

      SWV_AppBase_EM_MAX,
   };

#  define SWV_AppBase_DefSaveFile     "PROGDIR:Projects/Project.Default"

   // -- attribute definitions -----------------------------------------------

#  define SWP_AppBase_Attrs \
   SWAM_PT (SWA_AppBase_TopObj,       SWAM_NoLab,  SWAM_SGIN, NULL)
   
#  define SWP_AppBase_IntAttrs                                                      \
   SWAM_FS (SWA_AppBase_DefSaveFile,  SWAM_NoLab,  SWAM_Get,                        \
            SWV_AppBase_DefSaveFile),                                               \
   SWAM_SA (SWA_AppBase_LastFile,     SWAM_NoLab,  SWAM_SGIN, NULL, FMSIZE),        \
   SWAM_SA (SWA_AppBase_LastDir,      SWAM_NoLab,  SWAM_SGI,  SWF_DefProjDir,       \
            FMSIZE),                                                                \
   SWAM_SA (SWA_AppBase_LastPatt,     SWAM_NoLab,  SWAM_SGI,  SWF_DefProjPattern,   \
            256),                                                                   \
   SWAM_DSS(SWA_AppBase_PopKey,       SWS_AB_Popkey,SWAM_UI|SWAM_StrRet, NULL, 256),\
   SWAM_BO (SWA_AppBase_SaveProjIcons,SWS_AB_SavePI,SWAM_UI,   TRUE)


   // -- attribute instance data defs ----------------------------------------

#  define SWP_AppBase_AttrDefs             \
      Object   *SWA_AppBase_TopObj

#  define SWP_AppBase_IntAttrDefs          \
      STRPTR    SWA_AppBase_LastFile;      \
      STRPTR    SWA_AppBase_LastDir;       \
      STRPTR    SWA_AppBase_LastPatt;      \
      ULONG     SWA_AppBase_SaveProjIcons; \
      STRPTR    SWA_AppBase_PopKey

   // -- catalog strings -----------------------------------------------------

#  define SW_AppBase_CatStrDef \
   SWS_ABM_Project,            \
   SWS_ABM_Open,               \
   SWS_ABM_Save,               \
   SWS_ABM_SaveAs,             \
   SWS_ABM_SaveAsDef,          \
   SWS_ABM_New,                \
   SWS_ABM_Register,           \
   SWS_ABM_About,              \
   SWS_ABM_AboutMUI,           \
   SWS_ABM_Hide,               \
   SWS_ABM_Quit,               \
   SWS_ABM_Edit,               \
   SWS_ABM_RestoreDf,          \
   SWS_ABM_LastSv,             \
   SWS_ABM_Settings,           \
   SWS_ABM_MUI

#  define SW_AppBase_CatStr                         \
   "Project",                                       \
     "Open...",                                     \
     "Save",                                        \
     "Save As...",                                  \
     "Save As Default",                             \
     "New",                                         \
     "Register...",                                 \
     "About...",                                    \
     "About MUI...",                                \
     "Hide Window",                                 \
     "Quit",                                        \
   "Edit",                                          \
     "Restore Default",                             \
     "Last Saved",                                  \
   "Settings",                                      \
     "MUI..."

   // -- menu UserData values ------------------------------------------------

   enum {
      SWV_ABM_Open = 1,
      SWV_ABM_Save,
      SWV_ABM_SaveAs,
      SWV_ABM_SaveAsDef,
      SWV_ABM_New,
      SWV_ABM_Register,
      SWV_ABM_About,
      SWV_ABM_AboutMUI,
      SWV_ABM_Hide,
      SWV_ABM_Quit,
      SWV_ABM_RestoreDf,
      SWV_ABM_LastSv,
      SWV_ABM_MUI,
   };

   // -- NewMenus for AppBase ------------------------------------------------

#  define SW_AppBase_Menu_Proj_NoReg                                        \
  { NM_TITLE, (STRPTR)SWS_ABM_Project,    0 ,0,0,(APTR)0                 }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Open,      "O",0,0,(APTR)SWV_ABM_Open      }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Save,      "S",0,0,(APTR)SWV_ABM_Save      }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_SaveAs,    "A",0,0,(APTR)SWV_ABM_SaveAs    }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_SaveAsDef,  0, 0,0,(APTR)SWV_ABM_SaveAsDef }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_New,       "N",0,0,(APTR)SWV_ABM_New       }, \
  { NM_ITEM,  (STRPTR)NM_BARLABEL,      0 ,0,0,(APTR)0                   }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_About,     "?",0,0,(APTR)SWV_ABM_About     }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_AboutMUI,   0 ,0,0,(APTR)SWV_ABM_AboutMUI  }, \
  { NM_ITEM,  (STRPTR)NM_BARLABEL,      0 ,0,0,(APTR)0                   }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Hide,      "H",0,0,(APTR)SWV_ABM_Hide      }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Quit,      "Q",0,0,(APTR)SWV_ABM_Quit      }

#  define SW_AppBase_Menu_Proj                                              \
  { NM_TITLE, (STRPTR)SWS_ABM_Project,    0 ,0,0,(APTR)0                 }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Open,      "O",0,0,(APTR)SWV_ABM_Open      }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Save,      "S",0,0,(APTR)SWV_ABM_Save      }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_SaveAs,    "A",0,0,(APTR)SWV_ABM_SaveAs    }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_SaveAsDef,  0, 0,0,(APTR)SWV_ABM_SaveAsDef }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_New,       "N",0,0,(APTR)SWV_ABM_New       }, \
  { NM_ITEM,  (STRPTR)NM_BARLABEL,      0 ,0,0,(APTR)0                   }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_About,     "?",0,0,(APTR)SWV_ABM_About     }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_AboutMUI,   0 ,0,0,(APTR)SWV_ABM_AboutMUI  }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Register,  "R",0,0,(APTR)SWV_ABM_Register  }, \
  { NM_ITEM,  (STRPTR)NM_BARLABEL,      0 ,0,0,(APTR)0                   }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Hide,      "H",0,0,(APTR)SWV_ABM_Hide      }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_Quit,      "Q",0,0,(APTR)SWV_ABM_Quit      }

#  define SW_AppBase_Menu_Edit                                              \
  { NM_TITLE, (STRPTR)SWS_ABM_Edit,       0 ,0,0,(APTR)0                 }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_RestoreDf, "D",0,0,(APTR)SWV_ABM_RestoreDf }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_LastSv,    "L",0,0,(APTR)SWV_ABM_LastSv    }

#  define SW_AppBase_Menu_Settings                                          \
  { NM_TITLE, (STRPTR)SWS_ABM_Settings,   0 ,0,0,(APTR)0                 }, \
  { NM_ITEM,  (STRPTR)SWS_ABM_MUI,       "M",0,0,(APTR)SWV_ABM_MUI       }

#  define AH_None (0)

   // -- methods -------------------------------------------------------------

#  define SW_AppBase_Methods             \
     SW_DO_MF(SWM_AppBase_NewProj);      \
     SW_DO_MF(SWM_AppBase_M_About);      \

#  define AppBaseTopObj(o) SWA_AppBase_TopObj, o
#  define AppBaseDefTop(i) AppBaseTopObj((i)->SWA_AppBase_TopObj)

#  define AB_SetMod(i) set((i)->SWA_AppBase_TopObj, SWA_AppBase_ProjMod, TRUE)

#  undef SW_FIRSTTAG

# endif
