
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


void test_newfree(void)
{
   ULONG x;
   Object *r[10];

   for (x=0; x<10; x++) r[x] = NULL;

   for (x=0; x<10; x++)
      if (!(r[x] = NewObject(GetRegisterClass(),   NULL, TAG_DONE))) {
         Printf("ERROR: %s %s\n", __FUNC__,  ": alloc failed\n");
         goto Done;
      }

  Done:
   for (x=0; x<10; x++)
      if (r[x]) DisposeObject(r[x]);
}


void test_good(void)
{
   Object *RegObj = NewObject(GetRegisterClass(),   NULL, TAG_DONE);

   if (!RegObj) {
      Printf("ERROR: %s %s\n", __FUNC__, ": alloc failed");
      return;
   }

   if (DoMethod(RegObj, SWM_RegCheck, "key.good", "OpaqueMove") !=
       REG_Registered)
         Printf("ERROR: %s %s\n", __FUNC__,  ": reg check failed\n");

   if (SWGetUL(RegObj, SWA_Registered) != REG_Registered)
      Printf("ERROR: %s %s\n", __FUNC__, ": reg check failed 2");

   if (SWGetUL(RegObj, SWA_RegID) != 100500)
      Printf("ERROR: %s %s\n", __FUNC__, ": reg ID wrong");

   if (strcmp(SWGetStr(RegObj, SWA_RegLastName), "Koren"))
      Printf("ERROR: %s %s\n", __FUNC__, ": reg last name wrong");

   if (strcmp(SWGetStr(RegObj, SWA_RegFirstName), "Steve"))
      Printf("ERROR: %s %s\n", __FUNC__, ": reg first name wrong");

   if (strcmp(SWGetStr(RegObj, SWA_RegStatus), "Steve Koren (#100500)"))
      Printf("ERROR: %s %s\n", __FUNC__, ": status incorrect");
         
   DisposeObject(RegObj);
}

void test_bad(void)
{
   Object *RegObj = NewObject(GetRegisterClass(),   NULL, TAG_DONE);

   if (!RegObj) {
      Printf("ERROR: %s %s\n", __FUNC__, ": alloc failed");
      return;
   }

   if (DoMethod(RegObj, SWM_RegCheck, "key.bad", "OpaqueMove") !=
       REG_InvalidKey)
      Printf("ERROR: %s %s\n", __FUNC__, ": reg check failed");

   if (SWGetUL(RegObj, SWA_Registered) != REG_InvalidKey)
      Printf("ERROR: %s %s\n", __FUNC__, ": reg check failed 2");

   if (strcmp(SWGetStr(RegObj, SWA_RegStatus), "Invalid Key"))
      Printf("ERROR: %s %s\n", __FUNC__, ": status incorrect");

   if (DoMethod(RegObj, SWM_RegCheck, "key.bad", "OpaqueMovZ") !=
       REG_InvalidKey)
      Printf("ERROR: %s %s\n", __FUNC__, ": reg check failed");

   if (strcmp(SWGetStr(RegObj, SWA_RegStatus), "Invalid Key"))
      Printf("ERROR: %s %s\n", __FUNC__, ": status incorrect");

   DisposeObject(RegObj);
}

void test_unreg(void)
{
   Object *RegObj = NewObject(GetRegisterClass(),   NULL, TAG_DONE);

   if (!RegObj) {
      Printf("ERROR: %s %s\n", __FUNC__, ": alloc failed");
      return;
   }

   if (DoMethod(RegObj, SWM_RegCheck, "key.nothere", "OpaqueMove") !=
       REG_NotRegistered)
      Printf("ERROR: %s %s\n", __FUNC__, ": reg check failed");
   
   if (strcmp(SWGetStr(RegObj, SWA_RegStatus), "Unregistered"))
      Printf("ERROR: %s %s\n", __FUNC__, ": status incorrect");

   DisposeObject(RegObj);
}


main (int argc, char *argv[])
{
   test_newfree();
   test_good();
   test_bad();
   test_unreg();

   exit(0);
}
