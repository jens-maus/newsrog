
static Object *ObjAddBox(Object *o, Object **box, ULONG attr,
                         ULONG attrptr, UBYTE *str, ULONG virt)
{
   Object   *eo = SWInv_GetEO(o);

   *box=MUI_NewObject(SWC_Objectbox,
                      SWA_OBox_EditObj,      SWGetObj(eo, attr),
                      SWA_OBox_ContentObj,   eo,
                      SWA_OBox_ContentAttr,  attrptr,
                      SWA_OBox_VirtGroup,    virt,
                      TAG_DONE);

   if (!*box) return NULL;

#  ifdef C_E_VARLIST_INCLUDE
      DoMethod(*box, SWM_OBox_MethodForward, eo, SWM_E_Varlist_Search);
#  endif

   DoMethod(eo, MUIM_Notify, attr, MUIV_EveryTime, *box, 1,
            SWM_OBox_UpdateObject);

   return (Object *)DoMethod(*box, SWM_OBox_Editor, str);
}
