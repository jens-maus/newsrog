
# include "Libraries/C-Root.h"
# include "Libraries/C-NServer.h"


# define  SWNETLOOP
# include "S-Libsetup.h"
# include "S-EventLoop.h"

main (int argc, char *argv[])
{
   int rc   = 10;
   Object *App, *TCPList, *Srv = NULL, *win, *vg;

   App = ApplicationObject,
      MUIA_Application_Title      , "NTCPListTest",
      MUIA_Application_Version    , "$VER: NTCPListTest 01.1 (09.01.96)",
      MUIA_Application_Copyright  , "©1997 by Steve Koren",
      MUIA_Application_Author     , "Steve Koren",
      MUIA_Application_Description, "Test of NTCPList class",
      MUIA_Application_Base       , "TCPLISTTEST",
      SubWindow, win = WindowObject,
         MUIA_Window_Title, "Show the NTCPList class",
         MUIA_Window_ID   , MAKE_ID('T','s','i','v'),
         WindowContents, vg = VGroup,
         End,
      End,
   End;


   if (!App) goto Done;

   if (!(TCPList = MUI_NewObject(SWC_NTCPList, TAG_DONE))) goto Done;

   if (!(Srv     = MUI_NewObject(SWC_NServer,
                                 SWA_NServer_Host,    "news",
                                 SWA_NServer_TCPList, TCPList,
                                 TAG_DONE))) goto Done;
   
   DoMethod(vg, OM_ADDMEMBER, TCPList);

   DoMethod(Srv, SWM_NServer_SendCmd, SWT_NServer_Cmd, "HELP\n", TAG_DONE);

   SW_NOTIFYQUIT(win);

   SWEventLoop(App, win, NULL, TCPList, TRUE);

   rc = 0;

 Done:
   if (App) MUI_DisposeObject(App);
   if (Srv) MUI_DisposeObject(Srv);

   exit (rc);
}
