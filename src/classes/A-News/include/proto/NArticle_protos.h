/* Prototypes for functions defined in
C-NArticle.c
 */

extern SWAttrMapT SWAttrs[30];

extern STRPTR SWCatStr[5];

extern struct Locale * Loc;

extern STRPTR SWMsgStr[6];

extern Catalog * SWCatalog;

extern STRPTR Months[13];

extern ULONG MDays[13];

extern STRPTR TZ[10];

extern LONG TZOff[9];

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

