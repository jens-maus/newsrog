# ifndef C_NAF_SAVETXT_INCLUDE
#  define C_NAF_SAVETXT_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NAFilter.h"
#  include "S-SysCmdVars.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 15100)

   enum {
      SWA_NAF_SaveTxt_FSpec = SW_FIRSTTAG,      // [SGI] File to save to
      SWA_NAF_SaveTxt_HdrMode,                  // [SGI] hdr/body/both
      SWA_NAF_SaveTxt_Mode,                     // [SGI] save mode
      SWA_NAF_SaveTxt_MkDir,                    // [SGI] create directories

      SWP_SysCmdAttrVals(SWA_NAF_SaveTxt_Cmd),  // [SGI] system cmd
      SWA_NAF_SaveTxt_DelFile,                  // [SGI] delete file after cmd?
   };

   enum {
      SWM_NAF_SaveTxt_Save,                     // [SGI] save the file
      SWM_NAF_SaveTxt_Exec,                     // [SGI] execute command
   };

   enum {
      SWV_NAF_SaveTxt_Mode_Overwrite = 0,       // overwrite old files
      SWV_NAF_SaveTxt_Mode_Append,              // append to old files
      SWV_NAF_SaveTxt_Mode_Skip,                // skip existing files
      SWV_NAF_SaveTxt_Mode_Suffix,              // append numeric suffix
      SWV_NAF_SaveTxt_Mode_COUNT,
   };

   enum {
      SWV_NAF_SaveTxt_HM_All = 0,               // save head+body
      SWV_NAF_SaveTxt_HM_Head,                  // save head
      SWV_NAF_SaveTxt_HM_Body,                  // save body
      SWV_NAF_SaveTxt_HM_COUNT,
   };

   SWM_P(SWP_NAF_SaveTxt_Save,
         Object *Grp; Object *Art; STRPTR FSpec; ULONG Parts; ULONG Append);

   SWM_P(SWP_NAF_SaveTxt_Exec, Object *Grp; Object *Art);


#  undef SW_FIRSTTAG   

# endif
