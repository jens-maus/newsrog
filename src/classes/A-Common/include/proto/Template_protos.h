/* Prototypes for functions defined in
C-Template.c
 */

extern SWAttrMapT SWAttrs[4];

extern STRPTR SWCatStr[2];

extern STRPTR SWMsgStr[3];

extern Catalog * SWCatalog;

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

/* Prototypes for functions defined in
L-Template.c
 */

extern struct Library * MUIClassBase;

extern struct Library * MUIMasterBase;

extern struct Library * SysBase;

extern struct Library * UtilityBase;

extern struct DosLibrary * DOSBase;

extern struct GfxBase * GfxBase;

extern struct IntuitionBase * IntuitionBase;

extern struct MUI_CustomClass * ThisClass;

LONG __asm LibNull(void);

struct LibraryHeader * __asm __saveds LibInit(register __a0 BPTR );

BPTR __asm __saveds LibExpunge(register __a6 struct LibraryHeader * );

struct LibraryHeader * __asm __saveds LibOpen(register __a6 struct LibraryHeader * );

BPTR __asm __saveds LibClose(register __a6 struct LibraryHeader * );

BOOL __asm __saveds UserLibOpen(register __a6 struct Library * );

BOOL __asm __saveds UserLibClose(register __a6 struct Library * );

ULONG __asm __saveds MCC_Query(register __d0 LONG );

extern Library * SWSupportBase;

extern SWLibraries SWLibs;

void ClassExitFunc(Library * );

BOOL ClassInitFunc(Library * );

