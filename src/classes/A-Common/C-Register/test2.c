
# include "Libraries/C-Register.h"

struct Library *MUIMasterBase = NULL;

int _STI_4000_Libs(void)
{
  MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN);

  if (!MUIMasterBase) PutStr("ERROR: Unable to open " MUIMASTER_NAME "\n");

  return (MUIMasterBase == NULL) ? -1 : 0;
}

void _STD_4000_Libs(void)
{
   SW_CLOSELIB(MUIMasterBase);
}

#   define SWGetType(o,a,t)      ((t)(SWGetVal(o,a)))
#   define SWGetUL(o,a)          SWGetType(o,a,ULONG)
#   define SWGetLong(o,a)        SWGetType(o,a,LONG)
#   define SWGetStr(o,a)         SWGetType(o,a,UBYTE *)
#   define SWGetObj(o,a)         SWGetType(o,a,Object *)


/*****************************************************************************
 * Function: get, return attribute                                           *
 *****************************************************************************/

__saveds __asm ULONG SWGetVal(a2 Object *o, d1 ULONG Attr)
{
   ULONG Res = 0;

   get(o, Attr, &Res);

   return Res;
}


void test_good(void)
{
   Object *RegObj = NewObject(GetRegisterClass(),   NULL, TAG_DONE);

   if (!RegObj) {
      Printf("ERROR: %s %s\n", __FUNC__, ": alloc failed");
      return;
   }

   if (DoMethod(RegObj, SWM_RegCheck, "ram:test.key", "NewsRog") ==
       REG_Registered)
      Printf("Registered\n");
   else
      Printf("UN-Registered\n");

   Printf("ID: %ld\n", SWGetUL(RegObj, SWA_RegID));
   Printf("RS: %s\n", SWGetStr(RegObj, SWA_RegStatus));
         
   DisposeObject(RegObj);
}

main (int argc, char *argv[])
{
   test_good();

   exit(0);
}
