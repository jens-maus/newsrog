
# include "Libraries/C-Root.h"
# include "Libraries/C-NArticleList.h"
# include "Libraries/C-NArticleMgr.h"

# define  SWNETLOOP
# include "S-Libsetup.h"
# include "S-EventLoop.h"

main (int argc, char *argv[])
{
   int rc   = 10;
   Object *App, *win, *vg, *ArtMgr, *ArtLst = NULL, *TCPList = NULL,
          *Art = NULL;
   APTR *MemPool = CreatePool(MEMF_ANY, 4192, 256);

   if (!(ArtMgr  = MUI_NewObject(SWC_NArticleMgr,
                                 SWA_NArticleMgr_MemPool,    MemPool,
                                 TAG_DONE))) goto Done;

   if (!(ArtLst  = MUI_NewObject(SWC_NArticleList,
                                 TAG_DONE))) goto Done;

   if (!(TCPList = MUI_NewObject(SWC_NTCPList,    TAG_DONE))) goto Done;

   if (!(Art     = MUI_NewObject(SWC_NArticle,
                                 SWA_NArticle_MemPool,    MemPool,
                                 TAG_DONE))) goto Done;

   DoMethod(Art, SWM_NArticle_Fetch,
            SWT_NArticle_MsgID, "<651rba$m96$1@kodak.rdcs.Kodak.COM>",
            TAG_DONE);

   DoMethod(ArtLst, MUIM_List_InsertSingle, Art, MUIV_List_Insert_Bottom);
   
   App = ApplicationObject,
      MUIA_Application_Title      , "NArticleListTest",
      MUIA_Application_Version    , "$VER: NArticleListTest 01.1 (09.01.96)",
      MUIA_Application_Copyright  , "©1997 by Steve Koren",
      MUIA_Application_Author     , "Steve Koren",
      MUIA_Application_Description, "Test of NArticleList class",
      SubWindow, win = WindowObject,
         MUIA_Window_Title, "Show the ArticleList class",
         MUIA_Window_ID   , MAKE_ID('T','s','a','l'),
         WindowContents, vg = VGroup,
            Child, ArtLst,
         End,
      End,
   End;

   if (!App) goto Done;

   SW_NOTIFYQUIT(win);

   SWEventLoop(App, win, NULL, TCPList, TRUE);

   rc = 0;

 Done:
   MUI_DisposeObject(ArtMgr);
   MUI_DisposeObject(TCPList);
   MUI_DisposeObject(Art);
   MUI_DisposeObject(App);

   DeletePool(MemPool);

   exit (rc);
}
