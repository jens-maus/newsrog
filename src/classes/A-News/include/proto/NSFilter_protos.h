/* Prototypes for functions defined in
C-NSFilter.c
 */

extern SWAttrMapT SWAttrs[30];

extern STRPTR SWCatStr[36];

extern STRPTR SWMsgStr[37];

extern Catalog * SWCatalog;

struct IClass * __asm __saveds ExpireSliderClass(void);

struct MUI_CustomClass * ExpireSliderInit(Library * );

void ExpireSliderExit(void);

struct IClass * __asm __saveds DisuseSliderClass(void);

struct MUI_CustomClass * DisuseSliderInit(Library * );

void DisuseSliderExit(void);

ULONG __asm ExpireSliderDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

ULONG __asm DisuseSliderDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

