/* Prototypes for functions defined in
C-NArticleList.c
 */

extern SWAttrMapT SWAttrs[30];

extern STRPTR SWCatStr[30];

extern STRPTR SWMsgStr[31];

extern Catalog * SWCatalog;

extern ULONG const Broken_colors[12];

extern UBYTE const Broken_body[59];

extern UBYTE const NewArt_body[59];

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

