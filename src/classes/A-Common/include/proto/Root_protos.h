/* Prototypes for functions defined in
C-Root.c
 */

extern SWAttrMapT SWAttrs[6];

extern STRPTR SWCatStr[26];

extern STRPTR SWMsgStr[27];

extern Catalog * SWCatalog;

extern TagItem * atags;

extern SignalSemaphore MF_Sem;

extern MinList MF_List;

extern struct Hook LayoutHook;

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

