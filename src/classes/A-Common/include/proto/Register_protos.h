/* Prototypes for functions defined in
C-Register.c
 */

Class * GetRegisterClass(void);

extern struct MUI_CustomClass * SWRegister;

ULONG __asm SWRegisterDispatch(register __a0 Class * , register __a2 Object * , register __a1 Msg );

int __asm __saveds _STI_4500_SWRegister(void);

void __saveds _STD_4500_SWRegister(void);

/* Prototypes for functions defined in
C-Register-UI.c
 */

extern struct RG_ptab RG_ParseTable[13];

ULONG __asm __saveds GROM_Generate(register __a2 Object * , register __a1 SWRegisterIData ** );

extern struct Hook RGOM_Hook_Generate;

ULONG RGOM_RegisterUI(SWRegisterIData * , SWP_RegUI * );

