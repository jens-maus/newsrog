
/*****************************************************************************
 * Function: Compare two variables for sorting                               *
 *****************************************************************************/

static LONG CmpVars(Object *o1, Object *o2)
{
   return 
      strcmp(SWGetStr(o1, SWA_FAct_Type), SWGetStr(o2, SWA_FAct_Type));
}


/*****************************************************************************
 * Method: insert object from disk                                           *
 *****************************************************************************/
   
METHOD(SWM_FileActs_Ins, Msg)
{
   Object *new;

   if (new = (Object *)DoMethod(o, SWM_Root_M_Open,
                                SWT_Root_SL_Win,         _win(o),
                                SWT_Root_SL_InitDrawer,  SWF_DefObjDir,
                                SWT_Root_SL_InitPattern, SWF_DefObjPattern,
                                TAG_DONE))
      if (SW_ISCLASS(new, SWC_FAct))
         DoMethod(o, MUIM_List_InsertSingle, new, MUIV_List_Insert_Bottom);
      else
         MUI_DisposeObject(new);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: insert object from disk                                           *
 *****************************************************************************/
   
METHOD(SWM_FileActs_Save, Msg)
{
   Object *Item;
   
   DoMethod(o, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &Item);
   
   if (Item)
      DoMethod(Item, SWM_Root_M_SaveAs,
               SWT_Root_SL_Win,         _win(o),
               SWT_Root_SL_InitDrawer,  SWF_DefObjDir,
               SWT_Root_SL_InitPattern, SWF_DefObjPattern,
               SWT_Root_SL_SaveMode,    TRUE,
               TAG_DONE);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: see if action/type match                                        *
 *****************************************************************************/

static LONG IsMatch(SW_IDATA *idata, STRPTR IType, STRPTR IAction, Object *a,
                    ULONG MatchAct)
{
   STRPTR Type   = SWGetStr(a, SWA_FAct_Type),
          Action = SWGetStr(a, SWA_FAct_Action);

   if (!idata->PatBuf)
      if (!(idata->PatBuf = AllocVec(MaxPSize, MEMF_ANY)))
         return FALSE;

   return ParsePatternNoCase(Type,   idata->PatBuf, MaxPSize) >= 0 &&
          MatchPatternNoCase(idata->PatBuf, IType)                 &&
           (!MatchAct || !strcmp(Action, IAction));
}

   
/*****************************************************************************
 * Method: invoke action                                                     *
 *****************************************************************************/
   
METHOD(SWM_FileActs_DoAction, Msg)
{
   STRPTR  IType   = GetTagStr(SWT_FileActs_DoA_Type,    NULL, msg+1),
           IAction = GetTagStr(SWT_FileActs_DoA_Action,  "",   msg+1);
   Object *VarObj  = GetTagObj(SWT_FileActs_DoA_VarObj,  NULL, msg+1),
          *VarList = GetTagObj(SWT_FileActs_DoA_VarList, NULL, msg+1);
   Object  *a;
   ULONG   x, Found = FALSE;

   if (SW_STRISNULL(IType)) SWDB_RET(SUCCESS);
   
   if (SW_STRISNULL(IAction))
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_EmptyAct, 0, 0));

   SW_MUIListLoop(idata->SWA_NList_L, x, a, TRUE)
      if (IsMatch(idata, IType, IAction, a, TRUE)) {
         DoMethod(a, SWM_FAct_Execute, VarList, VarObj);
         DoMethod(o, SWM_Root_AdoptObjError, a, 0);
         Found = TRUE;
         break;
      }

   if (!Found)
      SWDB_RET(SWERR(o, FAILURE, SWS_Err_ActNotFound, IAction, IType));
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: get list of possible actions                                      *
 *****************************************************************************/
   
METHOD(SWM_FileActs_GetActList, Msg)
{
   STRPTR  IType = GetTagStr(SWT_FileActs_GAL_Type, NULL, msg+1);
   Object *AList = ListObject,
                      MUIA_List_ConstructHook, MUIV_List_ConstructHook_String,
                      MUIA_List_DestructHook,  MUIV_List_DestructHook_String,
                   End;
   Object  *a;
   ULONG   x;

   if (!AList) SWDB_RET(NULL);

   if (!IType) {
      MUI_DisposeObject(AList);
      SWDB_RET(NULL);
   }
   
   SW_MUIListLoop(idata->SWA_NList_L, x, a, TRUE)
      if (IsMatch(idata, IType, NULL, a, FALSE))
         DoMethod(AList, MUIM_List_InsertSingle,
                  SWGetStr(a, SWA_FAct_Action),
                  MUIV_List_Insert_Bottom);
   
   SWDB_RET(AList);
} SDFT


/*****************************************************************************
 * Method: Fiddle with things after new list item has been selected          *
 *****************************************************************************/
   
METHOD(SWM_FileActs_PostChange, SWP_FileActs_PostChange *)
{
   ULONG Disabled;
   STRPTR L1, L2;
   
   if (!s_CmdT)
      SWDB_RET(FAILURE);

   L1 = GetS(SWS_L_Cmd);
   L2 = GetS(SWS_L_Args);
   
   if (msg->Active == MUIV_List_Active_Off) {
      Disabled = TRUE;
   } else {
      switch (SWGetUL(s_CmdT, MUIA_Cycle_Active)) {
        case SWV_FAct_CLI:
         Disabled = FALSE;
         break;
        case SWV_FAct_ARexx:
         Disabled = TRUE;
         L1 = GetS(SWS_T_RexxPort);
         L2 = GetS(SWS_T_RexxCmd);
         break;
        default: SWDB_RET(FAILURE);
      }
   }

   set(idata->Lab1, MUIA_Text_Contents, L1);
   set(idata->Lab2, MUIA_Text_Contents, L2);
   
   DoMethod(s_CmdT, MUIM_MultiSet, MUIA_Disabled, Disabled,
            s_In, s_Out, s_Stack, s_Pri, s_Dir, s_ASync, NULL);

   SWDB_RET(SUCCESS);
} SDFT
