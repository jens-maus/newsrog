# include "C-Support-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_LibFunc
# define SWDB_CLASSTYPE  SWDB_C_Root

SWLibraries     SWLibs;
APTR           *SWPool  = NULL;

# include "L-Debug.h"
# include "L-Attrmap.h"
# include "L-Subtask.h"

# define FASTSTRSTR_POS
# include "S-FastStrStr.h"


/***************************************************************************
 *   NEW FUNCTION                                                          *
 ***************************************************************************/

static void *OpenOneLib(UBYTE *name, ULONG ver)
{
   void *lib = (void *)OpenLibrary(name, ver);

   if (!lib) Printf("ERROR: unable to open %s rev %ld\n", name, ver);

   return lib;
}

__saveds __asm int SWOpenLibs(d0 ULONG Ver, a0 SWLibraries *SWLibs)
{
#  undef SysBase

   SWLibs->SW_CyberGfxBase  = (void *)OpenLibrary(SWCyberGfxName,   40L);
   SWLibs->SW_AslBase       = OpenOneLib(SWAslName,             __osver);
   SWLibs->SW_IFFParseBase  = OpenOneLib(SWIFFParseName,        __osver);
   SWLibs->SW_LocaleBase    = OpenOneLib(SWLocaleName,              38L);
   SWLibs->SW_DataTypesBase = OpenOneLib(SWDTName,              __osver);
   SWLibs->SW_RexxSysBase   = OpenOneLib(SWRexxSysName,               0);
   SWLibs->SW_CxBase        = OpenOneLib(SWCxName,              __osver);

   if (SWLibs->SW_AslBase       == NULL   ||
       SWLibs->SW_IFFParseBase  == NULL   ||
       SWLibs->SW_LocaleBase    == NULL   ||
       SWLibs->SW_DataTypesBase == NULL   ||
       SWLibs->SW_CxBase        == NULL   ||
       SWLibs->SW_RexxSysBase   == NULL)  return -1;

   return 0;
}

__saveds __asm void SWCloseLibs(d0 ULONG Ver, a0 SWLibraries *SWLibs)
{
   SW_CLOSELIB(SWLibs->SW_AslBase);
   SW_CLOSELIB(SWLibs->SW_CyberGfxBase);
   SW_CLOSELIB(SWLibs->SW_IFFParseBase);
   SW_CLOSELIB(SWLibs->SW_LocaleBase);
   SW_CLOSELIB(SWLibs->SW_DataTypesBase);
   SW_CLOSELIB(SWLibs->SW_RexxSysBase);
}


/*****************************************************************************
 *   User library init function - open all the libs we need.                 *
 *****************************************************************************/

Library *SWSupportBase              = NULL;
Library *UtilityBase                = NULL;
Library *MUIMasterBase              = NULL;
Library *IconBase                   = NULL;
struct GfxBase       *GfxBase       = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct DosLibrary    *DOSBase       = NULL;

__saveds __asm int __UserLibInit(a6 Library *libbase)
{
   SWSupportBase = libbase;
   return 0;
}


/*****************************************************************************
 *   Init/exit code for opening other libs etc.                              *
 *****************************************************************************/

static  ULONG   OpenCnt = 0;
static struct TextFont *Topaz8Font = NULL;
static struct TextAttr Topaz8Attr  = { "topaz.font",8,FS_NORMAL,FPF_ROMFONT };

# ifdef _GENPROTO
__saveds __asm int SWInit(void) { }
# else
__saveds __asm int SWInitA(a6 Library *libbase)
{
   if (OpenCnt++ > 0) return TRUE;
   
   if (!(IntuitionBase = (void  *)OpenLibrary(SWIntuitionName,  __osver))     ||
       !(MUIMasterBase = (void  *)OpenLibrary(MUIMASTER_NAME,
                                              MUIMASTER_VLATEST))             ||
       !(UtilityBase   = (void  *)OpenLibrary(SWUtilityName,    __osver))     ||
       !(DOSBase       = (void  *)OpenLibrary(SWDOSName,        __osver))     ||
       !(IconBase      = (void  *)OpenLibrary(ICONNAME,         __osver))     ||
       !(GfxBase       = (void  *)OpenLibrary(SWGfxName,        __osver))     ||
       !(SWPool        =          SWPoolCreate(MEMF_ANY, 1024*32, 512, NULL)) ||
       SWOpenLibs(SW_LIBSTRUCTVER, &SWLibs)                                   ||
       !TimesliderInit(libbase)                                               ||
       !KbSliderInit(libbase)                                                 ||
       !ObjListInit(libbase)                                                  ||
       !ObjStringInit(libbase))
      return FALSE;

   Topaz8Font = OpenFont(&Topaz8Attr);

   SWDebugInit();

   return TRUE;
}
# endif


__saveds __asm void SWExit(void)
{
   if (--OpenCnt > 0) return;

   SWDebugClose();

   if (Topaz8Font) CloseFont(Topaz8Font);
   Topaz8Font = NULL;

   TimesliderExit();
   KbSliderExit();
   ObjListExit();
   ObjStringExit();
   if (SWPool) SWPoolDelete(SWPool);
   SW_CLOSELIB(IntuitionBase);
   SW_CLOSELIB(UtilityBase);
   SW_CLOSELIB(MUIMasterBase);
   SW_CLOSELIB(DOSBase);
   SW_CLOSELIB(IconBase);
   SW_CLOSELIB(GfxBase);
   SWCloseLibs(SW_LIBSTRUCTVER, &SWLibs);
}


/*****************************************************************************
 * Function: get, return attribute                                           *
 *****************************************************************************/

__saveds __asm ULONG SWGetVal(a2 Object *o, d1 ULONG Attr)
{
   ULONG Res = 0;

   if (o && Attr) get(o, Attr, &Res);

   return Res;
}

/*****************************************************************************
 * Function: pooled AllocVec                                                 *
 *****************************************************************************/

static UBYTE *NullStr = "";

__saveds __asm UBYTE *SWPoolNullStr(void) { return NullStr; }

__saveds __asm UBYTE *SWPoolAV(a0 APTR *p, d0 ULONG s)
{
   ULONG *m = AllocPooled(p, s+sizeof(ULONG));
   if (!m) return NULL;

   m[0] = s;
   return (UBYTE *)(m+1);
}


/*****************************************************************************
 * Function: pooled FreeVec                                                  *
 *****************************************************************************/

__saveds __asm void SWPoolFV(a0 APTR *p, a1 UBYTE *m)
{
   ULONG *nm = (ULONG *)m;

   if (m && m!=NullStr)
      FreePooled(p, nm-1, nm[-1]+sizeof(ULONG));
}


/*****************************************************************************
 * Function: pooled Realloc                                                  *
 *****************************************************************************/

__saveds __asm LONG SWPoolRV(a0 APTR *p,d0 ULONG s, a1 UBYTE **m)
{
   UBYTE *new = SWPoolAV(p, s);   // new space

   if (!new) return -1;

   memset(new, 0, s);

   if (*m) {
      memcpy(new, *m, min(s, ((ULONG *)*m)[-1]));
      SWPoolFV(p, *m);
   }
   
   *m = new;

   return 0;
}


/*****************************************************************************
 * Function: pooled strdup                                                   *
 *****************************************************************************/

__saveds __asm UBYTE *SWPoolStrDup(a0 APTR *p, a1 UBYTE *str)
{
   UBYTE *mem;
   
   if (SW_STRISNULL(str)) return NullStr;

   if (!(mem = SWPoolAV(p, strlen(str)+1))) return NULL;

   return strcpy(mem, str);   
} 


/*****************************************************************************
 * Function: pool create                                                     *
 *****************************************************************************/

__saveds __asm APTR SWPoolCreate(d0 type, a0 ps, a1 ts, a2 tl)
{
   return CreatePool(type, ps, ts);
}


/*****************************************************************************
 * Function: pool create                                                     *
 *****************************************************************************/

__saveds __asm void SWPoolDelete(a0 APTR pool)
{
   DeletePool(pool);
}


/*****************************************************************************
 * Function: format byte/Kb/Mb Val                                           *
 *****************************************************************************/

__saveds __asm UBYTE *SWFmtSize(d0 ULONG val, a1 UBYTE *Str, d1 LONG d)
{
   static LONG pot[] = { 1000, 100, 10, 1 };
   STRPTR sfx, fmt;
   ULONG iv, fv;
   
   if (val < (1024*1000)) {
      sfx = "Kb";
      iv  = val / 1024;
      fv  = (((val - iv*1024) * 1000 / 1024) + pot[d] / 2) / pot[d];
   } else {
      sfx = "Mb";
      iv  = val / 1024 / 1000;
      fv  = ((val - (iv * 1024 * 1000)) / 1024 + pot[d] / 2) / pot[d];
   }

   if (fv == pot[3-d]) fv=0, iv++;    // handle rounding up in integer part
       
   switch (d) {
     case 0: fmt = "%ld %s"; sprintf(Str, fmt, iv, sfx); return Str;
     case 1: fmt = "%ld.%01ld %s"; break;
     case 2: fmt = "%ld.%02ld %s"; break;
     case 3: fmt = "%ld.%03ld %s"; break;
   }

   sprintf(Str, fmt, iv, fv, sfx);

   return Str;
}


/*****************************************************************************
 * Function: Create framed text                                              *
 *****************************************************************************/

__saveds __asm Object *SWUI_FrameText(a0 STRPTR s, d0 ULONG v, a1 Object *tb,
                                      a2 Object **title)
{
   Object *s1, *s2, *tobj, *t = HGroup,
      TextFrame,
      MUIA_Background, MUII_TextBack,
      MUIA_VertWeight, 0,
      Child, s1 = HSpace(0),
      Child, tobj = TextObject, MUIA_Text_SetMin, FALSE, End,
      Child, s2 = tb ? (HGroup, Child, HSpace(0), Child, tb, End) : HSpace(0),
   End;

   set(s1, MUIA_HorizWeight, 1);
   set(s2, MUIA_HorizWeight, 1);

   if (s) DoMethod(tobj, MUIM_SetAsString, MUIA_Text_Contents, s, v);

   if (title) *title = tobj;
   
   return t;
}


/*****************************************************************************
 * Function: Create text button                                              *
 *****************************************************************************/

__saveds __asm Object *SWUI_Button(a0 STRPTR l, a1 STRPTR h)
{
   Object *o = SimpleButton(l);

   if (o)
      SetAttrs(o,
               MUIA_CycleChain,  1,
               MUIA_ShortHelp,   h,
               TAG_DONE);
   return o;
}


/*****************************************************************************
 * Function: Create image button                                             *
 *****************************************************************************/

__saveds __asm Object *SWUI_ImageBt(a0 ULONG InpMode, d0 ULONG Ctrl,
                                    a1 STRPTR Image, a2 STRPTR Help,
                                    d1 ULONG Frame)
{
   return ImageObject,
      MUIA_Frame,           Frame   ? Frame   : MUIV_Frame_ImageButton,
      MUIA_InputMode,       InpMode ? InpMode : MUIV_InputMode_RelVerify,
      MUIA_ShortHelp,       Help,
      MUIA_CycleChain,      1,
      MUIA_ControlChar,     Ctrl,
      MUIA_Image_FreeHoriz, FALSE,
      MUIA_Image_FreeVert,  FALSE,
      MUIA_Image_FontMatch, TRUE,
      MUIA_Font,            Topaz8Font,
      MUIA_Image_Spec,      Image,
      MUIA_InnerTop,        0,
      MUIA_InnerBottom,     0,
      MUIA_InnerLeft,       0,
      MUIA_InnerRight,      0,
   End;
}


/*****************************************************************************
 * Function: Parse a boolean string                                          *
 *****************************************************************************/

__saveds __asm ULONG SWBoolVal(a0 STRPTR sv)
{
   if (MatchToolValue("yes|on|true", sv))  return TRUE;
   if (MatchToolValue("no|off|false", sv)) return FALSE;

   return FALSE;
} 


/*****************************************************************************
 * Function: Build cmd line from tooltypes or CL args                        *
 *****************************************************************************/

__saveds __asm STRPTR SWGetCmdLine(a0 STRPTR *ttypes)
{
   STRPTR CmdBuff;
   LONG  x, totsize;

   if (!ttypes) return NULL;
   
   for (x=totsize=0; ttypes[x]; x++)
      totsize += strlen(ttypes[x])+3;

   if (!(CmdBuff = AllocVec(totsize+1, MEMF_ANY))) return NULL;

   CmdBuff[0] = '\0';

   for (x=totsize=0; ttypes[x]; x++)
      if (strchr(ttypes[x], ' '))
         strcat(strcat(strcat(CmdBuff, "\""), ttypes[x]), "\" ");
      else
         strcat(strcat(CmdBuff, ttypes[x]), " ");

   return CmdBuff;
} 


/*****************************************************************************
 * Function: Free cmd line from tooltypes or CL args                         *
 *****************************************************************************/

__saveds __asm void SWFreeCmdLine(a0 STRPTR cl)
{
   FreeVec(cl);
}


/*****************************************************************************
 * Function: make path (recursive)                                           *
 *****************************************************************************/

__saveds __asm LONG SWMakePath(a0 STRPTR Path)
{
   BPTR l; UBYTE c, *Pos;

   if (SW_STRISNULL(Path)) SWDB_RET(FAILURE);

   if (l=Lock(Path, ACCESS_READ)) {
      UnLock(l);
      return SUCCESS;
   }

   if ((Pos = PathPart(Path)) == Path) return FAILURE;
   if (SW_STRISNULL(Pos))              return FAILURE;

   c = *Pos, *Pos = '\0';

   if (SWMakePath(Path) == FAILURE) {
      *Pos = c;
      return FAILURE;
   }

   *Pos = c;

   if (l = CreateDir(Path))
      UnLock(l);

   return l ? SUCCESS : FAILURE;
}


/*****************************************************************************
 * Function: get file type                                                   *
 *****************************************************************************/

__asm static LONG GetFType(a0 STRPTR fspec)
{
   LONG Type = 0;

   struct FileInfoBlock __aligned fib;
   BPTR lock = NULL;

   if (SW_STRISNULL(fspec)) goto Done;

   if (!(lock = Lock(fspec, ACCESS_READ)) ||
       (Examine((BPTR)lock, &fib)) == 0)
      goto Done;

   Type = fib.fib_DirEntryType;

  Done:
   if (lock) UnLock((BPTR)lock);

   return Type;
}


/*****************************************************************************
 * Function: test dir                                                        *
 *****************************************************************************/

__saveds __asm LONG SWDirExists(a0 STRPTR Path)
{
   return GetFType(Path) > 0;
}


/*****************************************************************************
 * Function: test file                                                       *
 *****************************************************************************/

__saveds __asm LONG SWFileExists(a0 STRPTR Path)
{
   return GetFType(Path) < 0;
}


/*****************************************************************************
 * Function: Close clipboard after writing                                   *
 *****************************************************************************/

__saveds __asm void SWCloseClipWrite(a0 struct IFFHandle *iff)
{
   if (!iff) return;

   PopChunk(iff);
   PopChunk(iff);

   CloseIFF(iff);
   CloseClipboard((struct ClipboardHandle *) iff->iff_Stream);
   FreeIFF(iff);
}

/*****************************************************************************
 * Function: Open clipboard for writing                                      *
 *****************************************************************************/

__saveds __asm struct IFFHandle *SWOpenClipWrite(d0 ULONG unit)
{
   struct IFFHandle *iff;

   if (!(iff = AllocIFF())) return NULL;

   InitIFFasClip(iff);

   if (!(iff->iff_Stream = (ULONG) OpenClipboard(unit))) {
      FreeIFF(iff);
      return NULL;
   }

   if (OpenIFF(iff,IFFF_WRITE) != 0) {
      CloseClipboard((struct ClipboardHandle *) iff->iff_Stream);
      FreeIFF(iff);
      return NULL;
   }

   if (PushChunk(iff,ID_FTXT,ID_FORM,IFFSIZE_UNKNOWN)) {
      SWCloseClipWrite(iff);
      return NULL;
   }

   if (PushChunk(iff,ID_FTXT,ID_CHRS,IFFSIZE_UNKNOWN)) {
      SWCloseClipWrite(iff);
      return NULL;
   }
   
   return iff;
}


/*****************************************************************************
 * Function: write text to clipboard                                         *
 *****************************************************************************/

__saveds __asm LONG SWTextToClip(a0 struct IFFHandle *iff, a1 STRPTR buf)
{
   ULONG len;

   if (!buf || !iff) return FAILURE;
   
   len = strlen(buf);
   
   return (WriteChunkBytes(iff, buf, len) == len) ? SUCCESS : FAILURE;
}


/*****************************************************************************
 * Function: Open clipboard for reading                                      *
 *****************************************************************************/

__saveds __asm struct IFFHandle *SWOpenClipRead(d0 ULONG unit)
{
   return 0; // NOT IMPLEMETNED
}


/*****************************************************************************
 * Function: write text to clipboard                                         *
 *****************************************************************************/

__saveds __asm LONG SWClipToText(a0 struct IFFHandle *iff, a1 STRPTR buf)
{
   return 0; // NOT IMPLEMETNED
}



/*****************************************************************************
 * Function: Close clipboard after reading                                   *
 *****************************************************************************/

__saveds __asm void SWCloseClipRead(a0 struct IFFHandle *iff)
{
}


/*****************************************************************************
 * Function: End for pattern in string; return bounds                        *
 *****************************************************************************/

__saveds __asm void SWPB_End(a0 struct PatBounds *pb)
{
   if (!pb) return;

   if (pb->PatBuf0 != pb->B0) FV(pb->PatBuf0);
   if (pb->PatBuf1 != pb->B1) FV(pb->PatBuf1);

   pb->PatBuf0 = pb->PatBuf1 = NULL;
}


/*****************************************************************************
 * Function: Init pattern bounds finding                                     *
 *****************************************************************************/

__saveds __asm ULONG SWPB_Start(a0 struct PatBounds *pb,
                                a1 STRPTR Pat, d0 LONG Case, d1 LONG Wild,
                                a2 void *ee)
{
   ULONG  bl = strlen(Pat)*2+4;
   STRPTR ts = AV(strlen(Pat)+2);

   if (!ts || !pb) goto Error;

   strcat(strcpy(ts, Pat), "*");
   
   pb->Case = Case;

   if (bl < PB_MaxS) {
      pb->PatBuf0 = pb->B0;
      pb->PatBuf1 = pb->B1;
   } else
      if (!(pb->PatBuf0 = AV(bl)) || !(pb->PatBuf1 = AV(bl)))
         goto Error;

   if (Wild) {
      if (Case) pb->Wild = ParsePattern(Pat, pb->PatBuf1, bl);
      else      pb->Wild = ParsePatternNoCase(Pat, pb->PatBuf1, bl);
   } else pb->Wild = 0;

   if (pb->Wild < 0) goto Error;

   if (pb->Wild) {
      if (Case) ParsePattern(ts, pb->PatBuf0, bl);
      else      ParsePatternNoCase(ts, pb->PatBuf0, bl);
   } else {
      strcpy(pb->PatBuf0, Pat);
      pb->PatLen = strlen(Pat);
      if (pb->PatBuf1 != pb->B1) {
         FV(pb->PatBuf1);
         pb->PatBuf1 = NULL;
      }
   };

   FV(ts);
   return SUCCESS;
   
  Error:
   FV(ts);
   SWPB_End(pb);
   return FAILURE;
}


/*****************************************************************************
 * Function: Check for pattern in string; return bounds                      *
 *****************************************************************************/

__saveds __asm
STRPTR SWPB_Find(a0 STRPTR s, a1 struct PatBounds *pb,
                 a2 ULONG *StCol, a3 ULONG *EndCol,
                 d1 ULONG Longest)
{
   LONG  C0, C1, Found = FALSE;
   UBYTE c;
   ULONG ECol = *EndCol;

   if (!ECol) ECol = strlen(s);

   if (s[0] == '\0') return NULL;

   if (pb->Wild) {
      // -- find beginning of match ------------------------------------------
   
      if (pb->Case) {
         for (C0=*StCol; s[C0] && C0<ECol; C0++)
            if (Found = MatchPattern(pb->PatBuf0, s+C0))
               break;
      } else
         for (C0=*StCol; s[C0] && C0<ECol; C0++)
            if (Found = MatchPatternNoCase(pb->PatBuf0, s+C0))
               break;

      if (!Found) return NULL;

      // -- find end of match ---------------------------------------------------

      Found = FALSE;

      if (Longest) {
         for (C1=ECol; C1>C0; C1--) {
            c = s[C1], s[C1] = '\0';

            if (pb->Case)
               Found = MatchPattern(pb->PatBuf1, s+C0);
            else
               Found = MatchPatternNoCase(pb->PatBuf1, s+C0);
            
            s[C1] = c;
   
            if (Found) break;
         }
      } else {
         for (C1=C0+1; s[C1] && C1<ECol; C1++) {
            c = s[C1], s[C1] = '\0';

            if (pb->Case)
               Found = MatchPattern(pb->PatBuf1, s+C0);
            else
               Found = MatchPatternNoCase(pb->PatBuf1, s+C0);

            s[C1] = c;
   
            if (Found) break;
         }
      }
   } else if (C0 = faststrstr(s + *StCol, pb->PatBuf0,
                              ECol - *StCol, pb->PatLen, pb->Case))
      C1 = --C0 + pb->PatLen, Found = TRUE;

   if (Found)
      *StCol  = C0, *EndCol = C1-1;

   return Found ? s+C0 : NULL;
}


/*****************************************************************************
 * Function: read file contents                                              *
 *    allocates a few extra bytes so you can \n terminate it if you want.    *
 *****************************************************************************/

__saveds __asm
UBYTE *SWReadFileContents(a0 STRPTR FSpec, a1 ULONG *Size, a2 ULONG *Err)
{
   BPTR  fp;
   struct FileInfoBlock __aligned fib;
   STRPTR FData;
   ULONG  DSize;

   if (SW_STRISNULL(FSpec)) return NULL;

   if (Err)  *Err  = 0;
   if (Size) *Size = 0;
   
   // -- find the file size --------------------------------------------------

   if (!(fp = Lock(FSpec, ACCESS_READ)))
      return NULL;

   if (Examine(fp, &fib))
      if (fib.fib_DirEntryType < 0) {
         DSize = fib.fib_Size;
         if (Size) *Size = DSize;
      }

   UnLock(fp);
   fp = NULL;

   // -- allocate space for data ---------------------------------------------
   
   if (!(FData = AllocVec(DSize+8, MEMF_ANY))) {
      if (Err) *Err = SWV_Err_Malloc;
      goto Done;
   }

   FData[0] = '\0';

   if (DSize == 0) goto Done;

   if (!(fp = Open(FSpec, MODE_OLDFILE))) {
      if (Err) *Err = SWV_Err_FOpen;
      FreeVec(FData);
      FData = NULL;
      goto Done;
   }

   // -- read the file -------------------------------------------------------

   if (Read(fp, FData, DSize) != DSize) {
      if (Err) *Err = SWV_Err_Read;
      FreeVec(FData);
      FData = NULL;
      goto Done;
   }

   FData[DSize] = '\0';

  Done:
   if (fp) Close(fp);

   return FData;
}


/*****************************************************************************
 * Function: save file contents (bin)                                        *
 *****************************************************************************/

__saveds __asm
ULONG SWWriteFileContents(a0 STRPTR FSpec, a1 UBYTE *Buff, d0 ULONG Size)
{
   ULONG  Err = 0;
   BPTR   fp = NULL;
   
   if (!Buff || SW_STRISNULL(FSpec)) goto Done;

   if (!(fp = Open(FSpec, MODE_NEWFILE))) {
      Err = SWV_Err_FOpen;
      goto Done;
   }

   if (Write(fp, Buff, Size) != Size) {
      Err = SWV_Err_Write;
      goto Done;
   }

  Done:
   if (fp) Close(fp);

   return Err;
}


/*****************************************************************************
 * Function: save file contents (string)                                     *
 *****************************************************************************/

__saveds __asm
ULONG SWWriteFileString(a0 STRPTR FSpec, a1 UBYTE *Buff, d0 ULONG Strlen)
{
   if (!Buff) return 0;
   
   if (!Strlen) Strlen = strlen(Buff);

   return SWWriteFileContents(FSpec, Buff, Strlen);
}


/*****************************************************************************
 * Function: Delete file & make sure it's gone                               *
 *****************************************************************************/

__saveds __asm
ULONG SWDeleteFile(a0 STRPTR FSpec)
{
   BPTR l;

   if (SW_STRISNULL(FSpec))
      return SUCCESS;
   
   if (!DeleteFile(FSpec))
      if (l = Lock(FSpec, ACCESS_READ)) {
         UnLock(l);
         return FAILURE;
      }

   return SUCCESS;
}

/*****************************************************************************
 * Function: Open a file in append mode                                      *
 *****************************************************************************/

__saveds __asm
BPTR SWOpenAppend(a0 STRPTR FSpec)
{
   BPTR fp;

   if (SW_STRISNULL(FSpec)) return NULL;

   if (fp = Open(FSpec, MODE_OLDFILE)) {
      Seek(fp, 0, OFFSET_END);
      return fp;
   }

   return Open(FSpec, MODE_NEWFILE);
}


/*****************************************************************************
 * Function: Send an ARexx command                                           *
 *****************************************************************************/

__saveds __asm
LONG SWSendRexxCmd(a0 STRPTR Port, a1 STRPTR Cmd, a2 struct MsgPort *ReplyPort,
                   a3 STRPTR Result, d0 ULONG MaxResLen)
{
   LONG Error;
   LONG CreatedReplyPort = FALSE;

   struct MsgPort *RexxPort;

   if (!ReplyPort) {
      if (!(ReplyPort = CreateMsgPort()))
         return RC_FATAL;
      CreatedReplyPort = TRUE;
   }
   
   if (Result && MaxResLen) *Result = '\0';

   Forbid();

   if (RexxPort = FindPort(Port))
   {
      struct RexxMsg *rexxMsg, *Answer;

      if (rexxMsg = CreateRexxMsg(ReplyPort, NULL, NULL))
      {
         if (rexxMsg->rm_Args[0] = CreateArgstring(Cmd, strlen(Cmd)))
         {
            rexxMsg->rm_Action = RXCOMM | RXFF_RESULT;

            PutMsg(RexxPort, &rexxMsg->rm_Node);

            do
            {
               WaitPort(ReplyPort);
               Answer = (struct RexxMsg *)GetMsg(ReplyPort);
            } while (Answer == NULL);

            Permit();

            if ((Error = Answer->rm_Result1) == RC_OK)
            {
               if (Answer->rm_Result2 && Result && MaxResLen)
               {
                  strncpy(Result, (STRPTR)Answer->rm_Result2, MaxResLen-1)
                     [MaxResLen-1] = '\0';
                  DeleteArgstring((UBYTE *)Answer->rm_Result2);
               }
            }

            DeleteArgstring((UBYTE *)ARG0(Answer));
            DeleteRexxMsg(Answer);

            if (CreatedReplyPort)
               DeleteMsgPort(ReplyPort);
            
            return Error;
         }
         else
         {
            DeleteRexxMsg(rexxMsg);
         }
      }
   }

   if (CreatedReplyPort)
      DeleteMsgPort(ReplyPort);

   Permit();

   return RC_FATAL;
}
