/*****************************************************************************
 * Beta code - undefine to turn off                                          *
 *****************************************************************************/

# ifdef BETACODE
#   define BetaCodeInit(App, Win) BetaCodeMsg(App, Win, "BETA");

ULONG BetaCodeMsg(Object *App, Object *Win, UBYTE *type)
{
   static const char Text[] =
      "This is \33b%s\33n code.  Use is subject to the following conditions:\n\n"

      "  This code is property of ShadowWorks Software and may not\n"
      "  be distributed in any form.  Permission is granted for use\n"
      "  only by beta testers directly chosen by ShadowWorks Software.\n\n"

      "  This software may contain catestrophic bugs.  ShadowWorks\n"
      "  Software makes no warranties expressed or implied about\n"
      "  the suitability of this software for any particular purpose.\n"
      "  The user bears the full responsibility for any damages\n"
      "  caused by this software.\n\n"

      "  File formats are subject to change before the release version.\n"
      "  Saved data may be incompatible with the releae version.\n\n"

      "Your use of this software constitutes your agreement to the above\n"
      "terms.";

   set(Win, MUIA_Window_Open, TRUE);
   set(App, MUIA_Application_Iconified, FALSE);

   MUI_Request(App, Win, 0, "NOTE!", "Ok", (char *)Text, type);

   return 0;
}
# else
#   define BetaCodeInit(App, Win)
# endif
