/* Prototypes for functions defined in
C-Decode.c
 */

LONG __asm __saveds SWUUIsBegin(register __a0 STRPTR );

STRPTR __asm __saveds SWUUFileName(register __a0 STRPTR );

LONG __asm __saveds SWUUIsEnd(register __a0 STRPTR );

LONG __asm __saveds SWUUSize(register __a0 STRPTR );

ULONG __asm __saveds SWUUDecodeLine(register __a0 STRPTR , register __a1 STRPTR , register __d0 ULONG , register __a2 ULONG * );

UBYTE * __asm __saveds SWUUDecode(register __a0 STRPTR , register __d0 ULONG , register __a1 ULONG * , register __a2 ULONG * , register __a3 ULONG * , register __a5 STRPTR );

ULONG __asm __saveds SWCountLines(register __a0 STRPTR );

UBYTE * __asm __saveds SWBase64Decode(register __a0 STRPTR , register __d0 ULONG , register __a1 ULONG * , register __a2 ULONG * , register __a3 ULONG * , register __a5 STRPTR );

UBYTE * __asm __saveds SWQPDecode(register __a0 STRPTR , register __d0 ULONG , register __a1 ULONG * , register __a2 ULONG * , register __a3 ULONG * , register __a5 STRPTR );

STRPTR __asm __saveds SWQPEncodeStr(register __a0 STRPTR , register __a1 STRPTR );

UBYTE * __asm __saveds SWMimeDecode(register __a0 STRPTR , register __d0 ULONG , register __a1 ULONG * , register __a2 ULONG * , register __a3 ULONG * , register __a5 STRPTR );

STRPTR __asm __saveds SWDecodeErrStr(register __d0 ULONG );

/* Prototypes for functions defined in
C-Timeslider.c
 */

struct IClass * __asm __saveds TimesliderClass(void);

struct MUI_CustomClass * TimesliderInit(Library * );

void TimesliderExit(void);

struct IClass * __asm __saveds KbSliderClass(void);

struct MUI_CustomClass * KbSliderInit(Library * );

void KbSliderExit(void);

ULONG __asm TimesliderDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

struct IClass * __asm __saveds SWTimesliderClass(void);

struct IClass * __asm __saveds SWKbsliderClass(void);

ULONG __asm KbSliderDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

/* Prototypes for functions defined in
C-ObjString.c
 */

ULONG __asm ObjStringDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

struct IClass * __asm __saveds SWObjStringClass(void);

struct MUI_CustomClass * ObjStringInit(Library * );

void ObjStringExit(void);

/* Prototypes for functions defined in
C-ObjList.c
 */

ULONG __asm ObjListDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

struct IClass * __asm __saveds SWObjListClass(void);

struct MUI_CustomClass * ObjListInit(Library * );

void ObjListExit(void);

/* Prototypes for functions defined in
L-Support.c
 */

extern SWLibraries SWLibs;

extern APTR * SWPool;

void __saveds SWDebugInit(void);

void __asm __saveds SWDebugSet(register __d0 int , register __d1 int );

void __asm __saveds SWDebugClose(void);

void __asm __saveds SWDebugOpen(register __a0 UBYTE * , register __d0 int , register __d1 int );

void __stdargs __saveds SWDebugPrint(LONG , LONG , UBYTE * , ...);

void __asm __saveds SWDebugEnter(register __d0 int , register __d1 int , register __a0 UBYTE * );

void __asm __saveds SWDebugExit(register __d0 int , register __d1 int , register __a0 UBYTE * );

void __asm __saveds SWDumpObj(register __a0 Object * , register __d0 ULONG );

ULONG __asm __saveds SWGetAttr(register __a0 SWAttrMapT const * , register __a1 UBYTE const * , register __d0 ULONG const , register __a2 ULONG * );

ULONG __asm __saveds SWSetAttr(register __a0 SWAttrMapT const * , register __a1 UBYTE const * , register __d0 ULONG const , register __d1 ULONG );

ULONG __asm __saveds SWInitAttrs(register __a0 SWAttrMapT const * , register __a1 UBYTE const * , register __a2 Object * );

ULONG * __asm __saveds SWGenTagList(register __a0 SWAttrMapT const * , register __a1 UBYTE const * , register __a2 ULONG * );

ULONG __asm __saveds SWCmpAttrs(register __a0 SWAttrMapT const * , register __a1 UBYTE const * , register __a2 UBYTE const * );

ULONG __asm __saveds SWWriteAttrs(register __a0 SWAttrMapT const * , register __a1 UBYTE const * , register __d1 BPTR const , register __a3 UBYTE * );

ULONG __asm __saveds SWReadAttrs(register __a0 SWAttrMapT const * , register __a1 UBYTE const * , register __d1 BPTR const , register __a2 Object const * );

ULONG __asm __saveds SWFreeAttrs(register __a0 SWAttrMapT const * , register __a1 UBYTE const * );

void __asm __saveds SWLocalizeNewMenu(register __a0 Catalog const * , register __a1 UBYTE const ** , register __a2 NewMenu * );

void __asm __saveds SWLocalizeStringArray(register __a0 Catalog const * , register __a1 STRPTR const * , register __a2 STRPTR * );

Object * __asm __saveds SWAttrUI(register __a0 SWAttrMapT const * , register __a1 STRPTR const * , register __a2 HelpMap const * , register __a3 Object * );

void __asm __saveds SWKillAttrUI(register __a0 SWAttrMapT const * , register __a1 Object * , register __a2 Object * );

LONG __asm __saveds SWSendSubTaskMsg(register __a0 struct SubTask * , register __d0 WORD , register __d1 APTR );

struct MsgPort * SWCreateMsgPort(void);

void SWDeleteMsgPort(struct MsgPort * );

struct SubTask * __asm __saveds SWSpawnSubTask(register __a0 UBYTE * , register __a1 void (* )(void), register __d0 APTR , register __d1 ULONG );

void __asm __saveds SWKillSubTask(register __a0 struct SubTask * );

void __asm __saveds SWExitSubTask(register __a0 struct SubTask * , register __a1 struct SubTaskMsg * , register __a2 struct Library * );

struct SubTask * __asm __saveds SWInitSubTask(void);

int __asm __saveds SWOpenLibs(register __d0 ULONG , register __a0 SWLibraries * );

void __asm __saveds SWCloseLibs(register __d0 ULONG , register __a0 SWLibraries * );

extern Library * SWSupportBase;

extern Library * UtilityBase;

extern Library * MUIMasterBase;

extern Library * IconBase;

extern struct GfxBase * GfxBase;

extern struct IntuitionBase * IntuitionBase;

extern struct DosLibrary * DOSBase;

int __asm __saveds __UserLibInit(register __a6 Library * );

int __asm __saveds SWInit(void);

void __asm __saveds SWExit(void);

ULONG __asm __saveds SWGetVal(register __a2 Object * , register __d1 ULONG );

UBYTE * __asm __saveds SWPoolNullStr(void);

UBYTE * __asm __saveds SWPoolAV(register __a0 APTR * , register __d0 ULONG );

void __asm __saveds SWPoolFV(register __a0 APTR * , register __a1 UBYTE * );

LONG __asm __saveds SWPoolRV(register __a0 APTR * , register __d0 ULONG , register __a1 UBYTE ** );

UBYTE * __asm __saveds SWPoolStrDup(register __a0 APTR * , register __a1 UBYTE * );

APTR __asm __saveds SWPoolCreate(register __d0 int , register __a0 int , register __a1 int , register __a2 int );

void __asm __saveds SWPoolDelete(register __a0 APTR );

UBYTE * __asm __saveds SWFmtSize(register __d0 ULONG , register __a1 UBYTE * , register __d1 LONG );

Object * __asm __saveds SWUI_FrameText(register __a0 STRPTR , register __d0 ULONG , register __a1 Object * , register __a2 Object ** );

Object * __asm __saveds SWUI_Button(register __a0 STRPTR , register __a1 STRPTR );

Object * __asm __saveds SWUI_ImageBt(register __a0 ULONG , register __d0 ULONG , register __a1 STRPTR , register __a2 STRPTR , register __d1 ULONG );

ULONG __asm __saveds SWBoolVal(register __a0 STRPTR );

STRPTR __asm __saveds SWGetCmdLine(register __a0 STRPTR * );

void __asm __saveds SWFreeCmdLine(register __a0 STRPTR );

LONG __asm __saveds SWMakePath(register __a0 STRPTR );

LONG __asm __saveds SWDirExists(register __a0 STRPTR );

LONG __asm __saveds SWFileExists(register __a0 STRPTR );

void __asm __saveds SWCloseClipWrite(register __a0 struct IFFHandle * );

struct IFFHandle * __asm __saveds SWOpenClipWrite(register __d0 ULONG );

LONG __asm __saveds SWTextToClip(register __a0 struct IFFHandle * , register __a1 STRPTR );

struct IFFHandle * __asm __saveds SWOpenClipRead(register __d0 ULONG );

LONG __asm __saveds SWClipToText(register __a0 struct IFFHandle * , register __a1 STRPTR );

void __asm __saveds SWCloseClipRead(register __a0 struct IFFHandle * );

void __asm __saveds SWPB_End(register __a0 struct PatBounds * );

ULONG __asm __saveds SWPB_Start(register __a0 struct PatBounds * , register __a1 STRPTR , register __d0 LONG , register __d1 LONG , register __a2 void * );

STRPTR __asm __saveds SWPB_Find(register __a0 STRPTR , register __a1 struct PatBounds * , register __a2 ULONG * , register __a3 ULONG * , register __d1 ULONG );

UBYTE * __asm __saveds SWReadFileContents(register __a0 STRPTR , register __a1 ULONG * , register __a2 ULONG * );

ULONG __asm __saveds SWWriteFileContents(register __a0 STRPTR , register __a1 UBYTE * , register __d0 ULONG );

ULONG __asm __saveds SWWriteFileString(register __a0 STRPTR , register __a1 UBYTE * , register __d0 ULONG );

ULONG __asm __saveds SWDeleteFile(register __a0 STRPTR );

BPTR __asm __saveds SWOpenAppend(register __a0 STRPTR );

LONG __asm __saveds SWSendRexxCmd(register __a0 STRPTR , register __a1 STRPTR , register __a2 struct MsgPort * , register __a3 STRPTR , register __d0 ULONG );

