
# include "/Include/Libraries/C-Root.h"

# include "S-LibSetup.h"

# define ERR(x) Printf("ERROR: %s %s\n", __FUNC__, x)

# define GN 10

void test_allocfree(UBYTE *CName) SDSV
{
   ULONG r;
   APTR o[GN];

   for (r=0; r<GN; r++) o[r] = NULL;

   for (r=0; r<GN; r++)
      if (!(o[r] = MUI_NewObject(CName, TAG_DONE))) {
         ERR("failed allocating object");
         goto Done;
      }

 Done:

   for (r=0; r<GN; r++) if (o[r]) MUI_DisposeObject(o[r]);
} SDFV


main (int argc, char *argv[])
{
   test_allocfree(SWC_Objectlist);

   exit(0);
}
