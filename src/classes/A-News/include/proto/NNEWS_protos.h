/* Prototypes for functions defined in
C-NNews.c
 */

extern SWAttrMapT SWAttrs[150];

extern STRPTR SWCatStr[261];

extern STRPTR SWMsgStr[262];

extern Catalog * SWCatalog;

struct IClass * __asm __saveds EditorClass(void);

struct MUI_CustomClass * EditorInit(Library * );

void EditorExit(void);

ULONG __asm EditorDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

LONG WinNumFromRexxStr(STRPTR );

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

