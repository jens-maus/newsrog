/* Prototypes for functions defined in
C-AppBase.c
 */

extern HelpMap SW_HELPMAP[10];

extern STRPTR SWCatStr[21];

extern STRPTR SWMsgStr[22];

extern Catalog * SWCatalog;

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

