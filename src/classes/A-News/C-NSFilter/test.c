
# include "Libraries/C-Root.h"
# include "Libraries/C-NSF-LineRange.h"

# include "S-Libsetup.h"

//# define CHOST "sol.sun.csd.unb.ca"
# define CHOST "news"

main (int argc, char *argv[])
{
   int rc   = 10;
   Object *f = MUI_NewObject(SWC_NSF_LineRange, TAG_DONE);

   if (f) rc = 0;

   Printf("%08lx\n", f);

 Done:
   MUI_DisposeObject(f);

   exit (rc);
}
