/*****************************************************************************
 * Make an edit UI component for this classes instance data.                 *
 *****************************************************************************/

extern ClassT *ThisClass;

# ifndef SW_MAKEUIPOST
#   define SW_MAKEUIPOST
# endif

METHOD(SWM_Invocation_MakeEditUI, SWP_MakeEditUI *)
{
   Object *uis[1];
   UBYTE *labs[1];

   uis[0] = SWInv_ObjUI(o); labs[0] = GetS(SWS_L_RegLab);

   SW_MAKEUIPOST;

   SWDB_RET(DoSuperMethod(cl, o, SWM_Invocation_MakeEditUI, 1, uis, labs));
} SDFT

