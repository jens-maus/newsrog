
# include "Libraries/C-Root.h"

# include "S-LibSetup.h"

# define ERR(x) Printf("ERROR: %s %s\n", __FUNC__, x)

// ===========================================================================

int Func3(int x, int y)
   SDST(int) SWP_II(x, y)
{
   if (x < 4)  SWDB_RET(x+2);
   if (x < 7)  SWDB_RET(x+3);
   if (x < 10) SWDB_RET(x+4);

   SWDB_RET(x);
} SDFT

void Func2(int a, char *b) SDSV SWP_IS(a, b)
{
   if (Func3(3,1)  !=  5) ERR("ooops retval 1!");
   if (Func3(6,2)  !=  9) ERR("ooops retval 2!");
   if (Func3(12,3) != 12) ERR("ooops retval 3!");
} SDFV

void Func1(void) SDSV
{
   Func2(5, "zxy");
} SDFV

# define GN 12

// ===========================================================================

ULONG allocobjs(APTR o[]) SDSV
{
   ULONG r;

   for (r=0; r<GN; r++) o[r] = NULL;

   for (r=0; r<GN; r++)
      if (!(o[r] = MUI_NewObject(SWC_Root, TAG_DONE))) {
         ERR("object allocation failed");
         return FALSE;
      }

   return TRUE;
} SDFV

void freeobjs(APTR o[]) SDSV
{
   ULONG r;
   for (r=0; r<GN; r++)
      MUI_DisposeObject(o[r]);
} SDFV


void attachobjs(APTR o[], APTR oo[]) SDSV
{
   ULONG r;
   
   DoMethod(o[0], OM_ADDMEMBER, o[1]);
   DoMethod(o[0], OM_ADDMEMBER, o[2]);
   DoMethod(o[0], OM_ADDMEMBER, o[3]);
   DoMethod(o[1], OM_ADDMEMBER, o[4]);
   DoMethod(o[1], OM_ADDMEMBER, o[5]);
   DoMethod(o[2], OM_ADDMEMBER, o[6]);
   DoMethod(o[3], OM_ADDMEMBER, o[7]);
   DoMethod(o[3], OM_ADDMEMBER, o[8]);
   DoMethod(o[7], OM_ADDMEMBER, o[9]);
   DoMethod(o[7], OM_ADDMEMBER, o[10]);
   DoMethod(o[4], OM_ADDMEMBER, o[11]);

   for (r=0; r<GN; r++)
      oo[r] = o[r];

   for (r=1; r<GN; r++)
      o[r] = NULL;
} SDFV

// ===========================================================================
 
void test_allocfree(void) SDSV
{
   APTR o[GN];
   ULONG x;

   for (x=0; x<50; x++) {
      if (!allocobjs(o)) ERR("Alloc 1");
      freeobjs(o);
   }

} SDFV
  

// ===========================================================================

void test_member(void) SDSV
{
   ULONG r;
   APTR o[GN];

   if (!allocobjs(o)) return;

   DoMethod(o[0], OM_ADDMEMBER, o[1]);
   DoMethod(o[0], OM_ADDMEMBER, o[2]);

   if (SWGetUL(o[0], SWA_ObjChildCount) != 2)   ERR("count 1");
   if (SWGetUL(o[1], SWA_ObjChildCount) != 0)   ERR("count 2");

   DoMethod(o[1], OM_ADDMEMBER, o[3]);

   if (SWGetUL(o[1], SWA_ObjChildCount) != 1)   ERR("count 3");

   DoMethod(o[0], OM_REMMEMBER, o[1]);

   if (SWGetUL(o[0], SWA_ObjChildCount) != 1)   ERR("count 4");

   DoMethod(o[0], OM_ADDMEMBER, o[1]);

   if (SWGetUL(o[0], SWA_ObjChildCount) != 2)   ERR("count 5");

   o[1] = o[2] = o[3] = NULL; // don't free attached objects

 Done:

   freeobjs(o);
} SDFV


// ===========================================================================

void test_nthchild(void) SDSV
{
   APTR o[GN], oo[GN];
   allocobjs(o);
   attachobjs(o, oo);

   if (DoMethod(oo[0], SWM_NthChild, 0) != (ULONG)oo[1])  ERR("nth 1");
   if (DoMethod(oo[0], SWM_NthChild, 1) != (ULONG)oo[2])  ERR("nth 2");
   if (DoMethod(oo[0], SWM_NthChild, 2) != (ULONG)oo[3])  ERR("nth 3");
   if (DoMethod(oo[0], SWM_NthChild, 4) != (ULONG)NULL)   ERR("nth 4");
   if (DoMethod(oo[0], SWM_NthChild, 9) != (ULONG)NULL)   ERR("nth 5");
   if (DoMethod(oo[1], SWM_NthChild, 0) != (ULONG)oo[4])  ERR("nth 6");
   if (DoMethod(oo[1], SWM_NthChild, 1) != (ULONG)oo[5])  ERR("nth 7");
   if (DoMethod(oo[2], SWM_NthChild, 0) != (ULONG)oo[6])  ERR("nth 8");
   if (DoMethod(oo[3], SWM_NthChild, 0) != (ULONG)oo[7])  ERR("nth 9");
   if (DoMethod(oo[3], SWM_NthChild, 1) != (ULONG)oo[8])  ERR("nth 10");
   if (DoMethod(oo[7], SWM_NthChild, 0) != (ULONG)oo[9])  ERR("nth 11");
   if (DoMethod(oo[7], SWM_NthChild, 1) != (ULONG)oo[10]) ERR("nth 12");
   if (DoMethod(oo[4], SWM_NthChild, 0) != (ULONG)oo[11]) ERR("nth 13");

   DoMethod(o[0], SWM_Reset);

   if (DoMethod(oo[0], SWM_NthChild, 0) != (ULONG)oo[1])  ERR("nth 20");
   if (DoMethod(oo[0], SWM_NthChild, 1) != (ULONG)oo[2])  ERR("nth 21");
   
   freeobjs(o);
} SDFV


// ===========================================================================

void test_fileio(void) SDSV
{
   APTR o[GN], oo[GN], inobj;
   allocobjs(o);
   attachobjs(o, oo);

   if (DoMethod(o[0], SWM_Save, "t:mytest") != SUCCESS)   ERR("IO1");

   if (DoMethod(o[0], SWM_Save, NULL)   != FAILURE)       ERR("IO2");
   if (DoMethod(o[0], SWM_Save, "")     != FAILURE)       ERR("IO3");
   if (DoMethod(o[0], SWM_Save, "ram:") != FAILURE)       ERR("IO3A");
   if (DoMethod(o[0], SWM_Load, "env:TMPDIR"))            ERR("IO4");
   if (DoMethod(o[0], SWM_Load, "env:TERMPATH"))          ERR("IO5");
   if (DoMethod(o[0], SWM_Load, "env:BLORGUFH"))          ERR("IO5A");
   if (DoMethod(o[0], SWM_Load, NULL))                    ERR("IO5B");
   if (DoMethod(o[0], SWM_Load, ""))                      ERR("IO5C");
   if (DoMethod(o[0], SWM_Load, "ram:"))                  ERR("IO5d");

   inobj = (APTR)DoMethod(o[0], SWM_Load, "t:mytest");
   if (!inobj)                                            ERR("IO6");

   if (inobj) {
      if (DoMethod(o[0], SWM_Compare, inobj))             ERR("IO7");
      if (!DoMethod(oo[2], SWM_Compare, inobj))           ERR("IO8");
   }

   freeobjs(o);
   if (inobj) MUI_DisposeObject(inobj);
} SDFV



// ===========================================================================

void test_clone(void) SDSV
{
   APTR o[GN], o1, oo[GN];
   allocobjs(o);
   attachobjs(o, oo);

   if (!(o1 = (APTR)DoMethod(o[0], SWM_Clone))) {
      ERR("Clonealloc");
      goto Done;
   }

   if (DoMethod(oo[0],  SWM_Compare, o1))        ERR("cmp 1");
   if (DoMethod(o1,     SWM_Compare, oo[0]))     ERR("cmp 2");
   if (!DoMethod(o1,    SWM_Compare, oo[1]))     ERR("cmp 3");
   if (!DoMethod(o1,    SWM_Compare, oo[2]))     ERR("cmp 4");
   if (!DoMethod(o1,    SWM_Compare, oo[3]))     ERR("cmp 5");
   if (!DoMethod(oo[2], SWM_Compare, oo[3]))     ERR("cmp 6");
   if (DoMethod(o[4],   SWM_Compare, oo[4]))     ERR("cmp 7");

 Done:
   freeobjs(o);
   if (o1) MUI_DisposeObject(o1);
   
} SDFV


// ===========================================================================

void test_assign(void) SDSV
{
   APTR o[GN], o2;
   allocobjs(o);

   o2 = MUI_NewObject(SWC_Root, TAG_DONE);

   DoMethod(o2, SWM_Assign, o[0]);

   if (DoMethod(o2,    SWM_Compare, o[0]))     ERR("as 0");

   DoMethod(o2, SWM_Assign, o[1]);

   if (DoMethod(o2,    SWM_Compare, o[1]))     ERR("as 1");

   DoMethod(o2, SWM_Assign, o[2]);

   if (DoMethod(o2,    SWM_Compare, o[2]))     ERR("as 2");
   
 Done:
   freeobjs(o);
   MUI_DisposeObject(o2);
} SDFV
   

// ===========================================================================

void test_error(void) SDSV
{
   Object *o;

   o = MUI_NewObject(SWC_Root, TAG_DONE);
   if (!o) goto Done;

   if (SWGetUL(o, SWA_ObjError))           ERR("has error on init");

   DoMethod(o, SWM_SetObjError, NULL, 1, "Bob %s", "foo");

   if (SWGetUL(o, SWA_ObjError) != 1)      ERR("object error wrong");

   if (strcmp(SWGetStr(o, SWA_ObjErrorText), "Bob %s"))
      ERR("objerr txt");

   if (strcmp(SWGetStr(o, SWA_ObjErrorParam1), "foo"))
      ERR("objerr p1");

   DoMethod(o, SWM_ClearObjError);

   if (SWGetUL(o, SWA_ObjError))           ERR("has error after clear");

   if (DoMethod(o, SWM_SetObjError, 5, 1, "Bob %s", "foo") != 5)
      ERR("return after error wrong");

   if (DoMethod(o, SWM_SetObjError, 99, 1, "Bob %s", "foo") != 99)
      ERR("return after error wrong");

 Done:
   MUI_DisposeObject(o);
} SDFV


// ===========================================================================

void test_extclass(void) SDSV
{
   LONG x;
   Object *inv;

   for (x=0; x<10; x++) {
      inv = MUI_NewObject(SWC_Timer, TAG_DONE);
      if (!inv)                                  ERR("EXTCL 1");
      MUI_DisposeObject(inv);
   }

} SDFV


// ===========================================================================

void test_insert(void) SDSV
{
   Object *o[GN];
#  define O (Object *)

   allocobjs((APTR *)o);
   DoMethod(o[0], SWM_Insert, &o[2], 4, 3);
   if (SWGetUL(o[0], SWA_ObjChildCount) != 4)       ERR("INS 1");
   if (O DoMethod(o[0], SWM_NthChild, 0) != o[2])   ERR("INS 2");
   if (O DoMethod(o[0], SWM_NthChild, 1) != o[3])   ERR("INS 3");
   if (O DoMethod(o[0], SWM_NthChild, 3) != o[5])   ERR("INS 4");
   if (O DoMethod(o[0], SWM_NthChild, 4) != NULL)   ERR("INS 5");

   DoMethod(o[0], SWM_Insert, &o[7], 2, 0);
   if (SWGetUL(o[0], SWA_ObjChildCount) != 6)       ERR("INS 10");
   if (O DoMethod(o[0], SWM_NthChild, 0) != o[7])   ERR("INS 11");
   if (O DoMethod(o[0], SWM_NthChild, 1) != o[8])   ERR("INS 12");
   if (O DoMethod(o[0], SWM_NthChild, 2) != o[2])   ERR("INS 13");
   if (O DoMethod(o[0], SWM_NthChild, 3) != o[3])   ERR("INS 14");

   DoMethod(o[0], SWM_InsertSingle, o[9], MUIV_List_Insert_Bottom);
   if (SWGetUL(o[0], SWA_ObjChildCount) != 7)       ERR("INS 20");
   if (O DoMethod(o[0], SWM_NthChild, 6) != o[9])   ERR("INS 21");

   DoMethod(o[0], SWM_InsertSingle, o[10], 2);
   if (SWGetUL(o[0], SWA_ObjChildCount) != 8)       ERR("INS 20");
   if (O DoMethod(o[0], SWM_NthChild, 2) != o[10])  ERR("INS 31");
   
   o[2]=o[3]=o[4]=o[5]=NULL;
   o[7]=o[8]=NULL;
   o[9]=NULL;
   o[10]=NULL;

   freeobjs((APTR *)o);
} SDFV


# ifdef FOOB

void test_notify(void) SDSV
{
   Object *o1, *o2;

   o1 = MUI_NewObject(SWC_Clock, NULL, TAG_DONE);
   if (!o1)                                ERR("EXTCL 1");

   o2 = MUI_NewObject(SWC_Layout, NULL, TAG_DONE);
   if (!o2)                                ERR("EXTCL 2");

   if (!o1 || !o2) goto Done;

   DoMethod(o1, MUIM_Notify, SWA_Clock_HasSec, MUIV_EveryTime,
            o2, 3, MUIM_Set, SWA_Layout_HorizSpacing, MUIV_TriggerValue);

   set(o1, SWA_Clock_HasSec, 1);
   if (SWGetUL(o2, SWA_Layout_HorizSpacing) != 1) ERR("NOTIFY 1");
   
  Done:
   MUI_DisposeObject(o1);
   MUI_DisposeObject(o2);

} SDFV
# endif

main (int argc, char *argv[])
{
//   Func1();
   test_allocfree();
   test_member();
   test_nthchild();
// test_clone();
   test_fileio();
   test_assign();
//   test_error();
   test_insert();
// test_notify();
}
