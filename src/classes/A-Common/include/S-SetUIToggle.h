/*****************************************************************************
 * Function: set up UI toggle                                                *
 *****************************************************************************/

static void SetUIToggle(Object *UI, ULONG a_gad, ULONG a_chk,
                        ULONG trg, ULONG ival)
{
   Object *gad =(Object *)DoMethod(UI, MUIM_FindUData, a_gad);
   Object *chk =(Object *)DoMethod(UI, MUIM_FindUData, a_chk);

   if (gad && chk) {
      DoMethod(chk, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
               gad, 3, MUIM_Set, MUIA_Disabled, trg);
      set(gad, MUIA_Disabled, ival);
   }
}
