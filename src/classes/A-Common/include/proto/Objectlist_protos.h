/* Prototypes for functions defined in
C-ObjectList.c
 */

extern STRPTR SWCatStr[7];

extern STRPTR SWMsgStr[8];

extern Catalog * SWCatalog;

LONG __asm __saveds ocfunc(register __a1 struct MUIS_Listtree_TreeNode * );

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

