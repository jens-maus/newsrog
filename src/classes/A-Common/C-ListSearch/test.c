
# include "Libraries/C-Root.h"
# include "Libraries/C-ListSearch.h"

# include "S-Libsetup.h"
# include "S-EventLoop.h"

main (int argc, char *argv[])
{
   int rc   = 10;
   Object *App, *win, *b, *ls;

   App = ApplicationObject,
      MUIA_Application_Title      , "ListSearchTest",
      MUIA_Application_Version    , "$VER: ListSearchTest 01.1 (09.01.96)",
      MUIA_Application_Copyright  , "©1997 by Steve Koren",
      MUIA_Application_Author     , "Steve Koren",
      MUIA_Application_Description, "Test of ListSearch class",
      SubWindow, win = WindowObject,
         MUIA_Window_Title, "Show the Listsearch class",
         MUIA_Window_ID   , MAKE_ID('T','s','l','s'),
         WindowContents, VGroup,
            Child, b = SimpleButton("SearchWin"),
         End,
      End,
   End;

   if (!App) goto Done;

   ls = MUI_NewObject(SWC_ListSearch,
                      TAG_DONE);
   if (!ls) goto Done;

   DoMethod(b, MUIM_Notify, MUIA_Pressed, FALSE,
            ls, 2, SWM_ListSearch_OpenWin, App);
   
   SW_NOTIFYQUIT(win);

   SWEventLoop(App, win, NULL, TRUE);

   rc = 0;

 Done:
   MUI_DisposeObject(ls);
   MUI_DisposeObject(App);

   exit (rc);
}
