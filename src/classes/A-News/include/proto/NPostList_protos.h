/* Prototypes for functions defined in
C-NPostList.c
 */

extern SWAttrMapT SWAttrs[20];

extern STRPTR SWCatStr[39];

extern STRPTR SWMsgStr[40];

extern Catalog * SWCatalog;

struct IClass * __asm __saveds NGListClass(void);

struct MUI_CustomClass * NGListInit(Library * );

void NGListExit(void);

ULONG __asm NGListDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

