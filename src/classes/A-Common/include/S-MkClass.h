
# define SW_MkClassExterns(CName)                         \
   __saveds __asm struct IClass *CName##Class(void);      \
   struct MUI_CustomClass *CName##Init(Library *libbase); \
   void CName##Exit(void)

# define SW_MkClassInt(CName, super, supermcc)                             \
                                                                           \
__asm ULONG CName##Dispatcher(a0 struct IClass *cl,                        \
                              a2 Object *o, a1 Msg msg);                   \
                                                                           \
static struct MUI_CustomClass *CName##ClassPtr = NULL;                     \
                                                                           \
__saveds __asm struct IClass *CName##Class(void)                           \
{                                                                          \
   return SW_GETICLASS(CName##ClassPtr);                                   \
}                                                                          \
                                                                           \
struct MUI_CustomClass *CName##Init(Library *libbase)                      \
{                                                                          \
   if (CName##ClassPtr) return CName##ClassPtr;                            \
                                                                           \
   CName##ClassPtr =                                                       \
      MUI_CreateCustomClass(NULL, super, supermcc,                         \
                            sizeof(struct CName##Data), CName##Dispatcher);\
                                                                           \
   SW_GETICLASS(CName##ClassPtr)->cl_UserData = (ULONG)libbase;            \
                                                                           \
   return CName##ClassPtr;                                                 \
}                                                                          \
                                                                           \
void CName##Exit(void)                                                     \
{                                                                          \
   if (CName##ClassPtr) {                                                  \
      MUI_DeleteCustomClass(CName##ClassPtr);                              \
      CName##ClassPtr = NULL;                                              \
   }                                                                       \
}


# define MkClassDispatchInit(cl)         \
  putreg(REG_A6, (long)cl->cl_UserData); \
  geta4()


# define SW_MkClass(CName, super)     SW_MkClassInt(CName, super, NULL)
# define SW_MkClassPriv(CName, super) SW_MkClassInt(CName, NULL, super)
