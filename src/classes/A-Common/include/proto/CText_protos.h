/* Prototypes for functions defined in
C-CText.c
 */

extern SWAttrMapT SWAttrs[20];

extern Library * DiskfontBase;

extern APTR CTPool;

extern LONG FontTabLast;

extern struct TextFont * Topaz8;

void FreeFont(struct TextFont * );

void InitFontTab(void);

void CloseFontTab(void);

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

