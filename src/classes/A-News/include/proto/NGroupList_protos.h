/* Prototypes for functions defined in
C-NGroupList.c
 */

extern SWAttrMapT SWAttrs[80];

extern STRPTR SWCatStr[137];

extern STRPTR SWMsgStr[138];

extern Catalog * SWCatalog;

extern ULONG const Broken_colors[12];

extern UBYTE const Broken_body[59];

extern ULONG const WinOpen_colors[24];

extern UBYTE const WinOpen_body[99];

extern UBYTE const WinClose_body[98];

extern UBYTE const NEWGRP_body[98];

Object * MakeWinBitm(UBYTE * );

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

