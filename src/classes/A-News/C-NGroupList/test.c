
# include "Libraries/C-Root.h"
# include "Libraries/C-NGroupList.h"
# include "Libraries/C-NServerList.h"
# include "Libraries/C-NArticleMgr.h"


# include "S-Libsetup.h"

# define CNT 100

Object *o[CNT];

main (int argc, char *argv[])
{
   ULONG x;
   Object *App = NULL;

   APTR *MemPool = CreatePool(MEMF_ANY, 4192, 256);

   if (!MemPool) goto Done;

   if (!(App = ApplicationObject, End)) goto Done;

   Printf("Start\n");

   for (x=0; x<CNT; x++)
      o[x] = MUI_NewObject(SWC_NGroup, TAG_DONE);

   Printf("End\n");

   for (x=0; x<CNT; x++)
      MUI_DisposeObject(o[x]);

 Done:
   MUI_DisposeObject(App);
   DeletePool(MemPool);
   exit (0);
}
