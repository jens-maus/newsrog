
# include "Libraries/C-Root.h"
# include "Libraries/C-NSFList.h"

# include "S-Libsetup.h"

# define CNT 200
Object *Objs[CNT] = { 0 };

main (int argc, char *argv[])
{
   ULONG x;

   for (x=0; x<CNT; x++) {
      if (!(Objs[x] = MUI_NewObject(SWC_NSFList, TAG_DONE))) {
         Printf("Ooops\n");
         break;
      }
   }

   for (x=0; x<CNT; x++)
      MUI_DisposeObject(Objs[x]);      
}
