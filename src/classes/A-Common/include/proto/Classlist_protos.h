/* Prototypes for functions defined in
C-ClassList.c
 */

extern STRPTR SWCatStr[3];

extern STRPTR SWMsgStr[4];

extern Catalog * SWCatalog;

extern List ClassList;

extern ULONG ObjCount;

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

