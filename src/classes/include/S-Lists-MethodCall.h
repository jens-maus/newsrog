#     ifdef SW_NListTrackAttrNum
         SW_DO_MF(SWM_NList_Change);
#     endif
#     ifdef SW_NList_NewObj
        SW_DO_MF(SWM_NList_Add);
#     endif
#     if defined SW_NList_RemoveObj || defined SW_NList_DispHook
        SW_DO_MF(SWM_NList_RemoveObj);
#     endif
#     ifdef SWV_NList_TrackedMax
        SW_DO_MF(SWM_NList_AddTrackedList);
        SW_DO_MF(SWM_NList_RemTrackedList);
#     endif

#     ifdef SW_NList_Clone
      SW_DO_MF(SWM_NList_Clone);
#     endif

#     ifdef SW_NList_ClickCol
      SW_DO_MF(SWM_NList_ClickCol);
#     endif

#     ifdef SW_NList_SelChange
      SW_DO_MF(SWM_NList_SelChange);
#     endif

#     ifndef SW_NList_NoDragDrop
      SW_DO_M(SWM_ObjList_DragDrop,   SWM_NList_SetNotify_F);
#     endif
      SW_DO_MF(SWM_NList_SendCmd);
      SW_DO_MF(SWM_NList_SetNotify);
      SW_DO_MF(SWM_NList_KillNotify);
      SW_DO_MF(SWM_NList_FindObj);
      SW_DO_MF(SWM_NList_ObjMethod);
      SW_DO_MF(SWM_NList_ObjMethodA);
      SW_DO_MF(SWM_NList_ListSet);
      SW_DO_MF(MUIM_List_Remove);
      SW_DO_MF(SWM_NList_RemoveSel);
#     ifndef SW_NList_NoIns
      SW_DO_M(MUIM_List_InsertSingle, SWM_NList_DoTracked_F);
      SW_DO_M(MUIM_List_Insert,       SWM_NList_DoTracked_F);
#     endif
      SW_DO_M(MUIM_List_Redraw,       SWM_NList_ListMethod_F);
      SW_DO_M(MUIM_List_GetEntry,     SWM_NList_ListMethod_F);
      SW_DO_M(MUIM_List_Select,       SWM_NList_ListMethod_F);
      SW_DO_MF(MUIM_List_Clear);

      SW_DO_MF(MUIM_List_Sort);

#     ifdef SWListImg_Methods
         SWListImg_Methods;
#     endif
