# ifndef C_APPBASE_INCLUDE_I
#  define C_APPBASE_INCLUDE_I

#  include "Libraries/C-Register.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-AppBase.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_AppBase
#  define SW_CNAME            "AppBase"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/AppBase_protos.h"


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_AB_Popkey = 1,
      SWS_AB_SavePI,
      SWS_ABH_PopKey,
      SWS_ABH_SaveProjIcons,
      SWS_ABErr_BadHotKey,
      SWS_ABErr_CxErr,
      SWS_ABErr_NoSaveFile,
      SWS_AB_AskOver,
      SWS_AB_AskOverBt,

      SWS_L_UnregMsg,
      SWS_B_Ok,

      SWS_L_ExitMode,
      SWS_L_EM_Ask,
      SWS_L_EM_Save,
      SWS_L_EM_NoSv,
      SWS_H_ExitMode,

      SWS_L_Clobber,
      SWS_H_Clobber,

      SWS_L_BackSfx,
      SWS_H_BackSfx,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

#  define DEFSAVEFILE "PROGDIR:Projects/Project.Default"
#  define DEFSAVEICON "PROGDIR:Icons/Project"

   // -- our instance specific data ------------------------------------------

   typedef struct SW_IDATA
   {
      SWP_AppBase_IntAttrDefs;

      ULONG     SWA_AppBase_Cost1;
      ULONG     SWA_AppBase_Cost2;
      STRPTR    SWA_AppBase_AppName;
      Object   *SWA_AppBase_App;
      Object   *SWA_AppBase_Win;
      Object   *SWA_AppBase_Reg;
      ULONG     SWA_AppBase_UnregSave;
      ULONG     SWA_AppBase_ProjMod;
      ULONG     SWA_AppBase_ExitMode;
      ULONG     SWA_AppBase_Clobber;
      STRPTR    SWA_AppBase_BackSuffix;
      STRPTR    SWA_AppBase_LastSaveFile;

      ULONG       AB_PopKey;
      ULONG       AB_CxNextID;
      AbCxCtrl   *AB_CxKeys;
      ULONG       AB_CxKeyCount;
      struct Hook AB_CxHook;
      struct Hook AB_RxHook;

      ULONG       A4;
      Object     *o;
   } SW_IDATA;

#  define SWV_AppBase_PrefsVer 1

   // -- Menu functions ------------------------------------------------------

#  define DoMI(a,b) case a: DoMethod(o, b, msg->Win); break

#  define SW_AppBase_MC_Proj                            \
     DoMI(SWV_ABM_Open,      SWM_AppBase_M_Open);       \
     DoMI(SWV_ABM_Save,      SWM_AppBase_M_Save);       \
     DoMI(SWV_ABM_SaveAs,    SWM_AppBase_M_SaveAs);     \
     DoMI(SWV_ABM_SaveAsDef, SWM_AppBase_M_SaveAsDef);  \
     DoMI(SWV_ABM_New,       SWM_AppBase_M_New);        \
     DoMI(SWV_ABM_Register,  SWM_AppBase_M_Register);   \
     DoMI(SWV_ABM_About,     SWM_AppBase_M_About);      \
     DoMI(SWV_ABM_AboutMUI,  SWM_AppBase_M_AboutMUI);   \
     DoMI(SWV_ABM_Hide,      SWM_AppBase_M_Hide);       \
     case SWV_ABM_Quit:      DoMethod(o, SWM_AppBase_M_Quit, msg->Win, FALSE); \
                             break;

#  define SW_AppBase_MC_Edit                            \
     DoMI(SWV_ABM_RestoreDf, SWM_AppBase_M_RestoreDf);  \
     DoMI(SWV_ABM_LastSv,    SWM_AppBase_M_LastSv);     \

#  define SW_AppBase_MC_Settings                        \
     DoMI( SWV_ABM_MUI,      SWM_AppBase_M_MUI);        \


#  define SW_AppBase_HelpMap \
   SWA_AppBase_SaveProjIcons, SWS_ABH_SaveProjIcons, \
   SWA_AppBase_PopKey,        SWS_ABH_PopKey
            
# endif // C_APPBASE_INCLUDE_I
