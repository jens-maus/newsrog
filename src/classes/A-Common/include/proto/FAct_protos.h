/* Prototypes for functions defined in
C-FAct.c
 */

extern SWAttrMapT SWAttrs[20];

extern STRPTR SWCatStr[23];

extern HelpMap SW_HELPMAP[9];

extern STRPTR SWMsgStr[24];

extern Catalog * SWCatalog;

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);
