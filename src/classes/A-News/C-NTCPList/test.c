
# include "Libraries/C-Root.h"
# include "Libraries/C-NTCPList.h"


# define  SWNETLOOP
# include "S-Libsetup.h"
# include "S-EventLoop.h"

#  define ntcp 4
#  define CHOST "news"
#  define CPORT 119

main (int argc, char *argv[])
{
   int rc   = 10, x;
   Object *App, *o, *win, *vg, *tcp[ntcp];

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

   if (!(o = MUI_NewObject(SWC_NTCPList, TAG_DONE))) goto Done;
   DoMethod(vg, OM_ADDMEMBER, o);

   for (x=0; x<ntcp; x++) {
      UBYTE Name[64];

      sprintf(Name, "Connect: %ld", x+1);
      
      tcp[x] = MUI_NewObject(SWC_NTCP,
                             SWA_NTCP_Host,    CHOST,
                             SWA_NTCP_Port,    CPORT,
                             SWA_NTCP_Timeout, 10,
                             SWA_NTCP_Title,   Name,
                             TAG_DONE);
      
      if (!tcp[x]) goto Done;
   
      DoMethod(o, MUIM_List_InsertSingle, tcp[x], MUIV_List_Insert_Bottom);
   }

   DoMethod(tcp[1], SWM_NTCP_Connect);
   DoMethod(tcp[2], SWM_NTCP_Connect);
   DoMethod(tcp[1], SWM_NTCP_SendStr, "HELP\n");

   SW_NOTIFYQUIT(win);

   SWEventLoop(App, win, NULL, o, TRUE);

   rc = 0;

 Done:
   if (App) MUI_DisposeObject(App);

   exit (rc);
}
