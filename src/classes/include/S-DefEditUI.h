
extern ClassT *ThisClass;

METHOD(SWM_Invocation_MakeEditUI, SWP_MakeEditUI *)
{
   Object *ui = VGroup,
      Child, SWInv_ParentUI(o),
      Child, VSpace(0),
      Child, SWInv_ObjUI(o),
   End;

   SWDB_RET((ULONG)ui);
} SDFT

