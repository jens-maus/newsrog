
# include "Libraries/C-Root.h"
# include "Libraries/C-VarList.h"

# include "S-Libsetup.h"
# include "S-EventLoop.h"

main (int argc, char *argv[])
{
   int rc   = 10;
   Object *App, *Win, *vl;

   App = ApplicationObject,
      MUIA_Application_Title      , "VarListTest",
      SubWindow, Win = WindowObject,
         MUIA_Window_Title, "Show the VarList class",
         MUIA_Window_ID   , MAKE_ID('T','s','i','v'),
         WindowContents, VGroup,
            Child, vl = MUI_NewObject(SWC_VarList, TAG_DONE),
         End,
      End,
   End;

   if (!App) goto Done;

   SW_NOTIFYQUIT(Win);

   SWEventLoop(App, Win, NULL, TRUE);

   { STRPTR s = (STRPTR)DoMethod(vl, SWM_VarList_ReplaceValues,
                                 "${Test}", vl);

     if (s) PutStr(s);

     DoMethod(vl, SWM_VarList_FreeString, s, vl);
   }

   rc = 0;

 Done:
   MUI_DisposeObject(App);

   exit (rc);
}
