/* Prototypes for functions defined in
C-NGroup.c
 */

extern SWAttrMapT SWAttrs[82];

extern STRPTR SWMsgStr[293];

extern Catalog * SWCatalog;

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

/* Prototypes for functions defined in
C-NGroup-Data.c
 */

/* Prototypes for functions defined in
C-NGroup-Util.c
 */

/* Prototypes for functions defined in
C-FAQList.c
 */

struct IClass * __asm __saveds FAQListClass(void);

struct MUI_CustomClass * FAQListInit(Library * );

void FAQListExit(void);

ULONG __asm FAQListDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

