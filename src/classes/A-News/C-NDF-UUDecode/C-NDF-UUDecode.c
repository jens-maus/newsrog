# include "C-NDF-UUDecode-I.h"
# include "S-NDSetupCust.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA (SWA_NDF_UUDecode_SaveDir,   SWS_L_SaveD, SWAM_UI|SWAM_PopDir,
            "RAM:", FMSIZE),
   SWAM_BO (SWA_NDF_UUDecode_DispInline,SWS_L_DI,    SWAM_UI,  TRUE),
   SWAM_BO (SWA_NDF_UUDecode_MakeDir,   SWS_L_MD,    SWAM_UI,  TRUE),
   SWAM_BO (SWA_NDF_UUDecode_Prompt,    SWS_L_Pr,    SWAM_UI,  TRUE),
   SWAM_BO (SWA_NDF_UUDecode_DispSize,  SWS_L_DspS,  SWAM_UI,  TRUE),
   SWAM_SA (SWA_NDF_UUDecode_DeleteTxt, SWS_L_DTxt,  SWAM_UI, DefDTxt, 256),
   SWAM_FI (SWA_NDFilter_HasTxtMod,     SWAM_NoLab,  SWAM_Get, TRUE),

   SWP_NDFilter_Attrs(-8000),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Save Directory: ",
   "Display Inline: ",
   "Make Dirs: ",
   "Use Requestors: ",
   "Display Size: ",
   "Deletion Marker: ",

   "OK",
   SWX_B("Error"),
   SWX_B("Incomplete"),

   "The directory in which the uuencoded data should\n"
      "be saved when the save button is pressed",
   "If set, graphical images will be displayed\n"
      "inline in the article viewer.",
   "If set, directories for the 'Save' option\n"
      "will automatically be created.",
   "If set, 'OK' and 'Cancel' requestors will be\n"
      "displayed after saving data.  Otherwise,\n"
      "there will be no requestors used.",
   "If set, MIME objects which are of any image type\n"
      SWX_B("image/*") " will have the image size displayed\n"
      "in the information area of the data block.",

   "When operating in text mode for printing/saving/replying,\n"
   "uuencoded data blocks are replaced with this text.  In\n"
   "addition to the standard variables, these may be used:\n\n"
      SWX_B("  ${Lines}") " - The number of lines in the uuencoded region.\n"
      SWX_B("  ${Name}") " - The filename of the uuencoded data.\n"
      SWX_B("  ${Size}") " - The size of the uuencoded region in bytes.",

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWA_NDF_UUDecode_SaveDir,    SWS_H_SaveD,
   SWA_NDF_UUDecode_DispInline, SWS_H_DI,
   SWA_NDF_UUDecode_MakeDir,    SWS_H_MD,
   SWA_NDF_UUDecode_Prompt,     SWS_H_Pr,
   SWA_NDF_UUDecode_DispSize,   SWS_H_DspS,
   SWA_NDF_UUDecode_DeleteTxt,  SWS_H_DTxt,
   NULL, NULL,
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods                      \
    SW_DO_MF(SWM_NDFilter_ModTxt);       \
    SW_DO_MF(SWM_NDFilter_ModTxtNoObjs); \
    SW_DO_MF(SWM_NDFilter_StartMatch);   \
    SW_DO_MF(SWM_NDFilter_EndMatch);     \
    SW_DO_MF(SWM_NDFilter_UI);           \
    SW_DO_MF(SWM_NDFilter_HideUI);       \
    SW_DO_MF(SWM_NDFilter_GenAddObj);    \
    SW_DO_MF(SWM_NDFilter_RemAddObj);    \
    SW_DO_MF(SWM_NDFilter_LastParseLine);\
    SW_DO_MF(SWM_VarList_Search);        \

# include "C-NDF-UUDecode-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
