
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

   
// ===========================================================================

void test_fileio(UBYTE *CName) SDSV
{
   Object *o, *inobj = NULL;

   if (!(o = MUI_NewObject(CName, TAG_DONE))) {
      ERR("err allocating object");
      goto Done;
   }

   if (DoMethod(o, SWM_Save, "t:mytest") != SUCCESS)   ERR("IO1");

   if (DoMethod(o, SWM_Save, NULL) != FAILURE)         ERR("IO2");
   if (DoMethod(o, SWM_Save, "") != FAILURE)           ERR("IO3");
   if (DoMethod(o, SWM_Load, "env:TMPDIR") != NULL)    ERR("IO4");
   if (DoMethod(o, SWM_Load, "env:TERMPATH") != NULL)  ERR("IO5");

   inobj = (Object *)DoMethod(o, SWM_Load, "t:mytest");
   if (!inobj)                                         ERR("IO6");

   if (inobj) {
      if (DoMethod(o, SWM_Compare, inobj))             ERR("IO7");
   }
   
   if (DoMethod(o, SWM_Load, NULL))                    ERR("IO9");
   if (DoMethod(o, SWM_Load, ""))                      ERR("IO10");

  Done:
   if (inobj) MUI_DisposeObject(inobj);
   if (o)     MUI_DisposeObject(o);
} SDFV
   

main (int argc, char *argv[])
{
   test_allocfree(SWC_Template);
   test_fileio(SWC_Template);

   exit(0);
}
