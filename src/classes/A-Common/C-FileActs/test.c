
# include "Libraries/C-Root.h"
# include "Libraries/C-FileActs.h"

# include "S-Libsetup.h"
# include "S-EventLoop.h"

main (int argc, char *argv[])
{
   int rc   = 10;
   Object *App, *Win, *vl, *AList;

   App = ApplicationObject,
      MUIA_Application_Title      , "FileActsTest",
      SubWindow, Win = WindowObject,
         MUIA_Window_Title, "Show the FileActs class",
         MUIA_Window_ID   , MAKE_ID('T','s','f','t'),
         WindowContents, VGroup,
            Child, vl = MUI_NewObject(SWC_FileActs, TAG_DONE),
         End,
      End,
   End;

   if (!App) goto Done;

   SW_NOTIFYQUIT(Win);

   SWEventLoop(App, Win, NULL, TRUE);

   AList = (Object *)DoMethod(vl, SWM_FileActs_GetActList,
            SWT_FileActs_GAL_Type,   "image/jpeg",
            TAG_DONE);

   if (AList)
      MUI_DisposeObject(AList);

   rc = 0;

 Done:
   MUI_DisposeObject(App);

   exit (rc);
}
