
extern ClassT *ThisClass;

# ifndef SW_MAKEUIPOST
#   define SW_MAKEUIPOST
# endif

METHOD(SWM_Invocation_MakeEditUI, SWP_MakeEditUI *)
{
   Object *rg = SWInv_ObjUI(o);

   if (!rg) SWDB_RET(NULL);
   
   DoMethod(rg, OM_ADDMEMBER, SWInv_ParentUI(o));

   SW_MAKEUIPOST;

   SWDB_RET((ULONG)rg);
} SDFT

