
/*****************************************************************************
 *   Debug functions                                                         *
 *****************************************************************************/

# define SWDEBUGFNS
# ifdef SWDEBUGFNS

   static SWDebugInfo SWDBInfo;

   __saveds void SWDebugInit(void)
   {
      SWDBInfo.fp       = NULL;
      SWDBInfo.Indent   = 0;
      SWDBInfo.Flags    = 0;
      SWDBInfo.Classes  = 0;
   }
   
   __saveds __asm void SWDebugSet(d0 Flags, d1 Classes)
   {
      SWDBInfo.Flags    = Flags;
      SWDBInfo.Classes  = Classes;
   }

   __saveds __asm void SWDebugClose(void)
   {
      if (SWDBInfo.fp) Close(SWDBInfo.fp);
      SWDebugInit();
   }
   
   __saveds __asm void SWDebugOpen(a0 UBYTE *fspec, d0 Flags, d1 Classes)
   {
      UBYTE envbuf[80];

      SWDebugClose();

      if (!fspec && GetVar(SWDB_EnvName, envbuf, sizeof(envbuf), 0) > 0)
         fspec = envbuf;

      if (fspec) SWDBInfo.fp = Open(fspec, MODE_NEWFILE);

      SWDBInfo.Flags    = Flags;
      SWDBInfo.Classes  = Classes;

      if (GetVar(SWDB_EnvLevel,   envbuf, sizeof(envbuf), 0) > 0)
         SWDBInfo.Flags   |= atol(envbuf);

      if (GetVar(SWDB_EnvClasses, envbuf, sizeof(envbuf), 0) > 0)
         SWDBInfo.Classes |= atol(envbuf);
   }
   
   __saveds __asm static void SWDebugPutC(d0 LONG putchar, a3 LONG rega6)
   {
      putreg(REG_A6, rega6);
      geta4();

      if (putchar)
         FPutC(SWDBInfo.fp, putchar);
   }

   static __inline int SWDebugCheck(ULONG Flags, ULONG Classes)
   {
      return (!SWDBInfo.fp                          ||
              (Flags   & SWDBInfo.Flags)   != Flags ||
              (Classes & SWDBInfo.Classes) != Classes);
   }

#  ifdef _GENPROTO
     __saveds void SWDebugPrint(LONG Flags, LONG Classes, UBYTE *fmat, ...)
     {
     } 
#  else
     __saveds __asm void SWDebugPrintA(d0 Flags, d1 Classes,
                                       a0 UBYTE *fmat, a1 APTR data)
     {
        LONG x;

        if (SWDebugCheck(Flags, Classes)) return;

        for (x = SWDBInfo.Indent; x>0; --x)
           FPutC(SWDBInfo.fp, (LONG)' ');

        RawDoFmt(fmat, data, SWDebugPutC, (APTR)SWSupportBase);
        FPutC(SWDBInfo.fp, (LONG)'\n');
     }
#  endif

   __saveds __asm void SWDebugEnter(d0 Flags, d1 Classes, a0 UBYTE *name)
   {
      UBYTE *n = name;

      if (SWDebugCheck(Flags, Classes)) return;

      SWDebugPrintA(Flags, Classes, "Enter: %s", &n);
      SWDBInfo.Indent += 2;
   }

   __saveds __asm void SWDebugExit(d0 Flags, d1 Classes, a0 UBYTE *name)
   {
      UBYTE *n = name;

      if (SWDebugCheck(Flags, Classes)) return;

      SWDBInfo.Indent -= 2;
      SWDebugPrintA(Flags, Classes, "Exit:  %s", &n);
   }

# endif


/***************************************************************************
 *   NEW FUNCTION                                                          *
 ***************************************************************************/
   
# ifdef SWDEBUGFNS

__saveds __asm void SWDumpObj(a0 Object *o, d0 ULONG attr1)
   SDSV
   SWDB_ASSERTNULL(o)
{
   Object *obj; APTR state; List *l;
   LONG x;
   static LONG Level = 0;

   for (x=0; x<Level; x++) FPutC(Output(), (LONG)' ');
   
   Printf("Obj: %s %s\n", SWGetStr(o, SWA_ObjClassName), SWGetStr(o, attr1));
   
   Level+=2;
   SW_GETLH(o, l);
   SW_LISTLOOP(l, state, obj)
      SWDumpObj(obj, attr1);
   Level-=2;

} SDFV

# endif
