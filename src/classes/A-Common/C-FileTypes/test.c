
# include "Libraries/C-Root.h"
# include "Libraries/C-FileTypes.h"

# include "S-Libsetup.h"
# include "S-EventLoop.h"

main (int argc, char *argv[])
{
   int rc   = 10;
   Object *App, *Win, *vl;
   STRPTR s;

   App = ApplicationObject,
      MUIA_Application_Title      , "FileTypeTest",
      SubWindow, Win = WindowObject,
         MUIA_Window_Title, "Show the FileType class",
         MUIA_Window_ID   , MAKE_ID('T','s','f','t'),
         WindowContents, VGroup,
            Child, vl = MUI_NewObject(SWC_FileTypes, TAG_DONE),
         End,
      End,
   End;

   if (!App) goto Done;

   SW_NOTIFYQUIT(Win);

   SWEventLoop(App, Win, NULL, TRUE);

   s = (STRPTR)DoMethod(vl, SWM_FileTypes_FindType,
                        SWT_FileTypes_FT_FSpec, "ram:test",
                        TAG_DONE);
                        
   Printf("Type: %s\n", s ? s : "Unknown");

   rc = 0;

 Done:
   MUI_DisposeObject(App);

   exit (rc);
}
