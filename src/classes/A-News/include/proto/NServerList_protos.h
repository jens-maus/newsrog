/* Prototypes for functions defined in
C-NServerList.c
 */

extern SWAttrMapT SWAttrs[10];

extern STRPTR SWCatStr[52];

extern STRPTR SWMsgStr[53];

extern Catalog * SWCatalog;

struct IClass * __asm __saveds PopSrvListClass(void);

struct MUI_CustomClass * PopSrvListInit(Library * );

void PopSrvListExit(void);

ULONG __asm PopSrvListDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

