
# include "Libraries/C-Root.h"
# include "Libraries/C-NServerList.h"


# define  SWNETLOOP
# include "S-Libsetup.h"
# include "S-EventLoop.h"

//# define CHOST "sol.sun.csd.unb.ca"
# define CHOST "news"

main (int argc, char *argv[])
{
   int rc   = 10;
   Object *App, *TCPList, *Srv = NULL,
   *SList, *SList1,
   *win1, *win2, *win3, *vg1, *vg2, *vg3;
   APTR *MemPool = CreatePool(MEMF_ANY, 4192, 256);

   App = ApplicationObject,
      MUIA_Application_Title      , "NTCPListTest",
      MUIA_Application_Version    , "$VER: NTCPListTest 01.1 (09.01.96)",
      MUIA_Application_Copyright  , "©1997 by Steve Koren",
      MUIA_Application_Author     , "Steve Koren",
      MUIA_Application_Description, "Test of NTCPList class",
      MUIA_Application_Base       , "TCPLISTTEST",
      SubWindow, win1 = WindowObject,
         MUIA_Window_Title, "Show the NTCPList class",
         MUIA_Window_ID   , MAKE_ID('T','s','i','v'),
         WindowContents, vg1 = VGroup,
         End,
      End,
      SubWindow, win2 = WindowObject,
         MUIA_Window_Title, "NServerList class",
         MUIA_Window_ID   , MAKE_ID('T','s','i','s'),
         WindowContents, vg2 = VGroup,
         End,
      End,
      SubWindow, win3 = WindowObject,
         MUIA_Window_Title, "NServerList class (T)",
         MUIA_Window_ID   , MAKE_ID('T','s','i','T'),
         WindowContents, vg3 = VGroup,
         End,
      End,
   End;

   if (!App) goto Done;

   if (!(TCPList = MUI_NewObject(SWC_NTCPList,    TAG_DONE))) goto Done;

   if (!(SList   = MUI_NewObject(SWC_NServerList,
                                 SWA_NServerList_TCPList, TCPList,
                                 SWA_NServerList_MemPool, MemPool,
                                 TAG_DONE))) goto Done;

   if (!(SList1  = MUI_NewObject(SWC_NServerList,
                                 SWA_NList_Master,        SList,
                                 SWA_NServerList_TCPList, TCPList,
                                 SWA_NServerList_MemPool, MemPool,
                                 TAG_DONE))) goto Done;

   if (!(Srv     = MUI_NewObject(SWC_NServer,
                                 SWA_NServer_Host,        CHOST,
                                 SWA_NServer_TCPList,     TCPList,
                                 SWA_NServer_MemPool,     MemPool,
                                 TAG_DONE))) goto Done;

   DoMethod(vg1, OM_ADDMEMBER, TCPList);
   DoMethod(vg2, OM_ADDMEMBER, SList);
   DoMethod(vg3, OM_ADDMEMBER, SList1);

   DoMethod(SList1, MUIM_List_InsertSingle, Srv, MUIV_List_Insert_Bottom);

   SW_NOTIFYQUIT(win1);
   SW_NOTIFYCLOSEWIN(win2);
   SW_NOTIFYCLOSEWIN(win3);

   set(win2, MUIA_Window_Open, TRUE);
   set(win3, MUIA_Window_Open, TRUE);

   SWEventLoop(App, win1, NULL, TCPList, TRUE);

   set(win2, MUIA_Window_Open, FALSE);
   set(win3, MUIA_Window_Open, FALSE);

   rc = 0;

 Done:
   MUI_DisposeObject(App);
   DeletePool(MemPool);

   exit (rc);
}
