# include "C-NDF-EMail-I.h"
# include "S-Profile.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NServer

// -- attribute map ----------------------------------------------------------

# define BEG SWAM_NewCG2|SWAM_EndG
   
SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewVG,  0, 0, 0),

   SWAM_DSS(SWA_NDF_EMail_Action,     SWS_L_EMailAct,
            SWAM_UI|SWAM_NewCG2, "Send-email", 256),

   SWAM_TX(SWS_L_ReqDot, 0),
   SWAM_BO(SWA_NDF_EMail_RequireDot,  SWAM_NoLab, SWAM_UI|SWAM_NewHG, TRUE),
   SWAM_BO(SWA_NDF_EMail_DClick,      SWS_L_DClk,   SWAM_UI,   FALSE),
   SWAM_BO(SWA_NDF_EMail_PopMenus,    SWS_L_PopM,   SWAM_UI|SWAM_EndG, TRUE),

   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
     
   SWAM_UICTL(SWAM_NewCG2 | SWAM_GrpLab, SWS_L_EMailTxt, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_EMail, FALSE, TRUE, FALSE, MPEN_SHINE,
                      SWA_NDF_EMail_Pens, PEN_EMail, TRUE),

   SWAM_PA(SWA_NDF_EMail_SelColor, SWS_L_SelCol, SWA_NDF_EMail_Pens,
           PEN_SelEMail, MPEN_TEXT, SWAM_UI|SWAM_EndG),
   
   SWAM_UICTL(SWAM_NewCG2 | SWAM_GrpLab, SWS_L_MsgIDTxt, 0, 0),
   SWP_NDF_StyleAttrs(SWA_NDF_EMail_MsgID, FALSE, TRUE, FALSE, MPEN_SHINE,
                      SWA_NDF_EMail_Pens, PEN_MsgID, TRUE),

   SWAM_PA(SWA_NDF_EMail_MsgID_SelColor, SWS_L_SelCol, SWA_NDF_EMail_Pens,
           PEN_SelMsgID, MPEN_TEXT, SWAM_UI|SWAM_EndG),

   SWP_NDFilter_Attrs(-1000),
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Text Style for email addresses.",
   "Text Style for Message-ID's.",
   "email Text Style",
   "Message-ID Text Style",

   "Multimedia Action: ",
   "Require Dots: ",
   "Double Click: ",
   "Use PopMenus: ",
   
   "An action in the Multimedia Action list.  The type\n"
     "is hardcoded at " SWX_B("application/X-email") ", for email\n"
     "addresses and " SWX_B("application/X-MsgID") " for message ID's, but\n"
     "any number of actions on this type may be defined to\n"
     "send email addresses or message ID's to various programs.",

   "If selected, then a string is detected as an email\n"
     "address only if the hostname component contains\n"
     "a dot (.).  For example, \"user@myhost.com\" would be\n"
     "matched, but not \"user@myhost\".  This can prevent\n"
     "false positives when parsing certain types of data.",

   "If selected, then a doubleclick is needed to\n"
      "trigger the action for this email address or\n"
      "MsgID.  Otherwise, a singleclick is sufficient.",

   "If selected, a popup menu is created for each\n"
      "address.  The address may be copied to the\n"
      "clipboard, or any Multimedia Action defined for the\n"
      "type " SWX_B("application/X-email") " (or " SWX_B("application/X-MsgID") "\n"
      "for Message ID's) may be executed.",

   "Selected Col:",
   "The color for a selected email address.",
   "The color for a selected Message-ID.",
   
   SWP_NDF_StyleLabStr,

   NULL,
};

HelpMap SW_HELPMAP[] = {
   SWP_NDF_StyleHelp(SWA_NDF_EMail,       SWS_H_EMail),
   SWP_NDF_StyleHelp(SWA_NDF_EMail_MsgID, SWS_H_MsgID),
   SWA_NDF_EMail_SelColor,   SWS_H_SelCol,
   SWA_NDF_EMail_MsgID_SelColor, SWS_H_SelColMsgID,
   SWA_NDF_EMail_Action,     SWS_H_EMailAct,
   SWA_NDF_EMail_RequireDot, SWS_H_ReqDot,
   SWA_NDF_EMail_DClick,     SWS_H_DClk,
   SWA_NDF_EMail_PopMenus,   SWS_H_PopM,
   NULL, NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# ifdef SW_PROFILE
#    define SW_USERSETUP    _ProfInit();
#    define SW_USERCLEANUP  _ProfTerm();
# endif

# define SW_Methods                  \
  SW_DO_MF(SWM_NDF_EMail_ViewMsgID); \
  SW_DO_MF(SWM_NDFilter_ModTxt);     \
  SW_DO_MF(SWM_NDFilter_StartMatch); \
  SW_DO_MF(SWM_NDFilter_EndMatch);   \
  SW_DO_MF(SWM_NDFilter_UI);         \
  SW_DO_MF(SWM_NDFilter_HideUI);     \
  SW_DO_MF(SWM_NDFilter_GenAddObj);  \
  SW_DO_MF(SWM_NDFilter_RemAddObj);  \

# include "C-NDF-EMail-Mthd.h"
# include "S-DefDFilter.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
