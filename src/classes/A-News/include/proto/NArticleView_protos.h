/* Prototypes for functions defined in
C-NArticleView.c
 */

extern SWAttrMapT SWAttrs[40];

extern STRPTR SWCatStr[44];

extern STRPTR SWMsgStr[45];

extern Catalog * SWCatalog;

struct IClass * __asm __saveds TextHideClass(void);

struct MUI_CustomClass * TextHideInit(Library * );

void TextHideExit(void);

ULONG __asm TextHideDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

struct IClass * __asm __saveds TextSelClass(void);

struct MUI_CustomClass * TextSelInit(Library * );

void TextSelExit(void);

ULONG __asm TextSelDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

struct IClass * __asm __saveds TextCmdClass(void);

struct MUI_CustomClass * TextCmdInit(Library * );

void TextCmdExit(void);

ULONG __asm TextCmdDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

struct IClass * __asm __saveds TextScrollClass(void);

struct MUI_CustomClass * TextScrollInit(Library * );

void TextScrollExit(void);

ULONG __asm TextScrollDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

struct IClass * __asm __saveds MimeBoxClass(void);

struct MUI_CustomClass * MimeBoxInit(Library * );

void MimeBoxExit(void);

ULONG __asm MimeBoxDispatcher(register __a0 struct IClass * , register __a2 Object * , register __a1 Msg );

STRPTR SUID_FNameQuery(Object * , Object * , STRPTR , STRPTR , STRPTR , STRPTR );

extern Catalog * SWCatalog;

ULONG __asm SWDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds STI_4500_SW(register __a6 Library * );

void __saveds STD_4500_SW(void);

