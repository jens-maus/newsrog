struct EditorData {
   SWP_NNews_AttrDefs;
   TEXT FSpec[FMSIZE];
   ULONG CursorDisp, AskClob, ReadOnly;
   Object *XTraObj1, *CursorGrp, *App, *Win, *CmdObj;
   Object *Filler, *TEDGroup, *T_Cursor, *TEditor;
   STRPTR *InitStrAttr;

   struct NotifyRequest          NR;
   struct MUI_InputHandlerNode   IHN;
   struct MsgPort                *Port;
};

SW_MkClass(Editor, MUIC_Group)

static void NewContents(struct EditorData *idata, STRPTR Buff, ULONG Size)
{
   ULONG Lines = 0;
   
   if (!Buff) Size = 0;
   if (Buff) {
      if (!Size) Size = strlen(Buff);
      Lines = SWCountLines(Buff);
   }

   SetAttrs(idata->TEditor,
            MUIA_TextEditor_CursorX, 0,
            MUIA_TextEditor_CursorY, 0,
            TAG_DONE);
   
   if (idata->CmdObj)
      DoMethod(idata->CmdObj, SWM_NNews_Editor_NewContents, Lines, Size);
}


/*****************************************************************************
 * Function: create an image button object                                   *
 *****************************************************************************/

static Object *MkEIB(struct EditorData *idata, ULONG h, STRPTR Img,
                     ULONG HasGhost, ULONG Toggle)
{
   Object *ib = DTImgObject,
      MUIA_InnerTop,        0,
      MUIA_InnerBottom,     0,
      MUIA_InnerLeft,       0,
      MUIA_InnerRight,      0,
      MUIA_Frame,           MUIV_Frame_ImageButton,
      MUIA_InputMode,       Toggle ? MUIV_InputMode_Toggle
                                   : MUIV_InputMode_RelVerify,
      MUIA_ShortHelp,       GetS(h),
      MUIA_CycleChain,      1,
      SWA_DTImg_FSpec,      Img,
      SWA_DTImg_HasGhost,   HasGhost,
      SWA_DTImg_ImgCache,   NN_DTImages(idata),
      SWA_DTImg_Path,       NN_ButtonPath(idata),
   End;

   if (ib && HasGhost)
      DoMethod(ib, MUIM_Notify, MUIA_Disabled, MUIV_EveryTime,
               ib, 3, MUIM_Set, SWA_DTImg_Ghosted, MUIV_TriggerValue);

   return ib;
}


/*****************************************************************************
 * Function: halt notifications                                              *
 *****************************************************************************/

static void HaltNotify(struct EditorData *idata)
{
   if (!SW_STRISNULL(idata->NR.nr_Name)) {
      EndNotify(&idata->NR);

      DoMethod(idata->App, MUIM_Application_RemInputHandler, &idata->IHN);

      idata->NR.nr_Name = NULL;

      DeleteMsgPort(idata->Port);
      idata->Port = NULL;
   }
}


/*****************************************************************************
 * Function: init notifications                                              *
 *****************************************************************************/

static ULONG InitNotify(Object *o, struct EditorData *idata, STRPTR FSpec)
{
   FSpec = SW_STRDEF(FSpec, idata->FSpec);

   if (SW_STRISNULL(FSpec))
      return SUCCESS;
   
   if (!SW_STRISNULL(idata->NR.nr_Name))
      return SUCCESS;

   if (!(idata->Port = CreateMsgPort()))
      return FAILURE;

   idata->NR.nr_Name      = FSpec;
   idata->NR.nr_UserData  = 0;
   idata->NR.nr_Flags     = NRF_SEND_MESSAGE;
   idata->NR.nr_Port      = idata->Port;
 
   idata->IHN.ihn_Object  = o;
   idata->IHN.ihn_Flags   = 0;
   idata->IHN.ihn_Method  = SWM_NNews_Editor_UpdateEF;
   idata->IHN.ihn_Signals = 1<<idata->Port->mp_SigBit;

   DoMethod(idata->App, MUIM_Application_AddInputHandler, &idata->IHN);

   if (!StartNotify(&idata->NR)) {
      DeleteMsgPort(idata->Port);
      idata->Port = NULL;
      return FAILURE;
   }

   return SUCCESS;   
}


/*****************************************************************************
 * Function: build text editor group                                         *
 *****************************************************************************/


static Object *MakeTEDGroup(Object *o, struct EditorData *idata)
{
   Object *b_Insert, *b_Save, *b_Cut, *b_Copy, *b_Paste, *b_Undo, *b_Redo,
          *b_Bold, *b_Italic, *b_Uline, *TEDGroup, *te_Slider, *BGrp;

   te_Slider = ScrollbarObject, End;

   if (!te_Slider) return NULL;

   idata->CursorGrp = NULL;
   
   if (idata->CursorDisp || idata->XTraObj1)
      idata->CursorGrp = VGroup,
         idata->XTraObj1 ? Child : TAG_IGNORE, idata->XTraObj1,
         Child, HGroup,
            Child, Label2(GetS(SWS_LE_Cursor)),
            Child, idata->T_Cursor = TextObject,
               TextFrame,
               MUIA_Background,  MUII_TextBack,
               MUIA_FixWidthTxt, "X=00 Y=00",
            End,
         End,
      End;

   if (idata->ReadOnly) {
      BGrp = NULL;
   } else {
      BGrp = HGroup,
         MUIA_HorizWeight, 1,
         Child, b_Insert = MkEIB(idata, SWS_HE_Insert, ebi_Insert, FALSE, FALSE),
         Child, b_Save   = MkEIB(idata, SWS_HE_Save,   ebi_Save,   FALSE, FALSE),
         Child, HSpace(0),
         Child, b_Cut    = MkEIB(idata, SWS_HE_Cut,    ebi_Cut,    TRUE,  FALSE),
         Child, b_Copy   = MkEIB(idata, SWS_HE_Copy,   ebi_Copy,   TRUE,  FALSE),
         Child, b_Paste  = MkEIB(idata, SWS_HE_Paste,  ebi_Paste,  FALSE, FALSE),
         Child, b_Undo   = MkEIB(idata, SWS_HE_Undo,   ebi_Undo,   TRUE,  FALSE),
         Child, b_Redo   = MkEIB(idata, SWS_HE_Redo,   ebi_Redo,   TRUE,  FALSE),
         Child, HSpace(0),
         Child, b_Bold   = MkEIB(idata, SWS_HE_Bold,   ebi_Bold,   FALSE, TRUE),
         Child, b_Italic = MkEIB(idata, SWS_HE_Italic, ebi_Italic, FALSE, TRUE),
         Child, b_Uline  = MkEIB(idata, SWS_HE_Uline,  ebi_Uline,  FALSE, TRUE),
         Child, HSpace(0),
         idata->CursorGrp ? Child : TAG_IGNORE, idata->CursorGrp,
         Child, RectangleObject,
            MUIA_HorizWeight, 1000,
         End,
      End;
   }
   
   TEDGroup = VGroup,
      MUIA_InnerTop,    0,
      MUIA_InnerBottom, 0,
      MUIA_InnerLeft,   0,
      MUIA_InnerRight,  0,
      BGrp ? Child : TAG_IGNORE, BGrp,
      Child, VGroup,
         MUIA_Group_Spacing, 0,
         Child, HGroup,
            MUIA_Group_Spacing, 0,
            Child, idata->TEditor = TextEditorObject,
               MUIA_CycleChain,            TRUE,
               MUIA_TextEditor_Slider,     te_Slider,
               MUIA_TextEditor_ExportHook, MUIV_TextEditor_ExportHook_EMail,
               MUIA_TextEditor_ImportHook, MUIV_TextEditor_ImportHook_EMail,
               MUIA_TextEditor_ExportWrap, 0,
               MUIA_TextEditor_ReadOnly,   idata->ReadOnly,
               MUIA_TextEditor_Contents,   (idata->InitStrAttr &&
                                            *idata->InitStrAttr) ? *idata->InitStrAttr : "",
               NN_PostWrapType(idata) == SWV_NNews_PR_Type ? 
                   MUIA_TextEditor_WrapBorder : TAG_IGNORE, NN_PostWrap(idata),
            End,
            Child, te_Slider,
         End,
      End,
   End;

   if (!TEDGroup) return NULL;

   if (BGrp) {
      DoMethod(b_Bold, MUIM_Notify, MUIA_Pressed, MUIV_EveryTime,
               idata->TEditor, 3,
               MUIM_NoNotifySet, MUIA_TextEditor_StyleBold, MUIV_TriggerValue);
   
      DoMethod(b_Italic, MUIM_Notify, MUIA_Pressed, MUIV_EveryTime,
               idata->TEditor, 3,
               MUIM_NoNotifySet, MUIA_TextEditor_StyleItalic, MUIV_TriggerValue);
   
      DoMethod(b_Uline, MUIM_Notify, MUIA_Pressed, MUIV_EveryTime,
               idata->TEditor, 3,
               MUIM_NoNotifySet, MUIA_TextEditor_StyleUnderline, MUIV_TriggerValue);
   
      DoMethod(idata->TEditor, MUIM_Notify, MUIA_TextEditor_StyleBold,
               MUIV_EveryTime, b_Bold, 3, MUIM_NoNotifySet,
               MUIA_Selected, MUIV_TriggerValue);
   
      DoMethod(idata->TEditor, MUIM_Notify, MUIA_TextEditor_StyleUnderline,
               MUIV_EveryTime, b_Uline, 3, MUIM_NoNotifySet,
               MUIA_Selected, MUIV_TriggerValue);
   
      DoMethod(idata->TEditor, MUIM_Notify, MUIA_TextEditor_StyleItalic,
               MUIV_EveryTime, b_Italic, 3, MUIM_NoNotifySet,
               MUIA_Selected, MUIV_TriggerValue);
   
      DoMethod(idata->TEditor, MUIM_Notify,
               MUIA_TextEditor_UndoAvailable, MUIV_EveryTime,
               b_Undo, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
   
      set(b_Undo, MUIA_Disabled, TRUE);
      set(b_Redo, MUIA_Disabled, TRUE);
   
      DoMethod(idata->TEditor, MUIM_Notify,
               MUIA_TextEditor_RedoAvailable, MUIV_EveryTime,
               b_Redo, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
   
      DoMethod(idata->TEditor, MUIM_Notify,
               MUIA_TextEditor_AreaMarked, MUIV_EveryTime,
               MUIV_Notify_Self, 6, MUIM_MultiSet, MUIA_Disabled, MUIV_NotTriggerValue,
               b_Cut, b_Copy, NULL);
   
      set(b_Cut,  MUIA_Disabled, TRUE);
      set(b_Copy, MUIA_Disabled, TRUE);
   
      DoMethod(b_Undo,  NOTP, idata->TEditor, 2, MUIM_TextEditor_ARexxCmd, "UNDO");
      DoMethod(b_Redo,  NOTP, idata->TEditor, 2, MUIM_TextEditor_ARexxCmd, "REDO");
      DoMethod(b_Cut,   NOTP, idata->TEditor, 2, MUIM_TextEditor_ARexxCmd, "CUT");
      DoMethod(b_Copy,  NOTP, idata->TEditor, 2, MUIM_TextEditor_ARexxCmd, "COPY");
      DoMethod(b_Paste, NOTP, idata->TEditor, 2, MUIM_TextEditor_ARexxCmd, "PASTE");
   
      DoMethod(b_Insert,NOTP, o, 2, SWM_NNews_Editor_EdIns, NULL);
      DoMethod(b_Save,  NOTP, o, 1, SWM_NNews_Editor_EdSave);
   
      DoMethod(idata->TEditor, MUIM_Notify, MUIA_TextEditor_CursorX,
               MUIV_EveryTime, o, 1, SWM_NNews_Editor_UpdCursPos);
   
      DoMethod(idata->TEditor, MUIM_Notify, MUIA_TextEditor_CursorY,
               MUIV_EveryTime, o, 1, SWM_NNews_Editor_UpdCursPos);

      DoMethod(o, MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
               o, 2, SWM_NNews_Editor_AppMessage, MUIV_TriggerValue);
   }

   return TEDGroup;
}


/*****************************************************************************
 * Function: quote text                                                      *
 *****************************************************************************/

static STRPTR QuoteText(STRPTR Text, ULONG *Size, ULONG Lines,
                        STRPTR QPrefix)
{
   STRPTR NewText, NTPos, TPos = Text;
   ULONG  QPLen;
   UBYTE  c;

   if (SW_STRISNULL(Text) || SW_STRISNULL(QPrefix)) return Text;

   if (!Lines) Lines = SWCountLines(Text);
   
   QPLen = strlen(QPrefix);

   if (!(NewText = AllocVec(*Size + 64 + QPLen * (Lines+4), MEMF_ANY)))
      return Text;

   strcpy(NTPos = NewText, QPrefix);
   NTPos += QPLen;
   
   while (c=*TPos) {
      *NTPos++ = *TPos++;
      
      if (c=='\n' && *TPos) {
         strcpy(NTPos, QPrefix);
         NTPos += QPLen;
      }
   }

   *NTPos = '\0';
   
   FreeVec(Text);
   *Size += QPLen * Lines;
   return NewText;
}


/*****************************************************************************
 * Dispatcher for Editor class                                               *
 *****************************************************************************/

static ULONG __stdargs DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1,...)
{
   return DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
}

__asm ULONG EditorDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct EditorData *idata = INST_DATA(cl, o);
   TEXT   FSpec[FMSIZE];
   ULONG  Err, Size, rc;
   STRPTR FData;

   MkClassDispatchInit(cl);

#  define attrl ((opSet *)msg)->ops_AttrList
#  define SvED  ((SWP_NNews_Editor_SaveEditData *)msg)
#  define EDIns ((SWP_NNews_Editor_EdIns *)msg)
#  define InsFl ((SWP_NNews_Editor_InsFile *)msg)
#  define ApMsg ((SWP_NNews_Editor_AppMessage *)msg)
#  define INot  ((SWP_NNews_Editor_InitNotify *)msg)
#  define AClob ((SWP_NNews_Editor_AskClob *)msg)

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *obj, *InitContents;
        int NoDelayInit = GetTagUL(SWA_NNews_Editor_NoDelayInit, FALSE, attrl);

        obj = (Object*)DoSuperNew(cl, o,
           MUIA_Group_Horiz, FALSE,
           MUIA_InnerTop,    0,
           MUIA_InnerBottom, 0,
           MUIA_InnerLeft,   0,
           MUIA_InnerRight,  0,
           Child, InitContents = TextObject,
              TextFrame,
              MUIA_Background,    MUII_TextBack,
              MUIA_Font,          MUIV_Font_Big,
              MUIA_Text_Contents, GetS(SWS_TE_Loading),
              MUIA_Text_PreParse, MUIX_C,
              MUIA_Text_SetMax,   FALSE,
              MUIA_Text_SetMin,   TRUE,
              MUIA_Text_SetVMax,  FALSE,
           End,
           TAG_MORE, attrl);

        if (!obj) return NULL;

        idata = INST_DATA(cl, obj);
        idata->SWA_AppBase_TopObj = GetTagObj(SWA_AppBase_TopObj, NULL, attrl);
        idata->Filler             = InitContents;
        idata->InitStrAttr        = (STRPTR *)
                                    GetTagUL(SWA_NNews_Editor_InitStrAddr, NULL, attrl);

        idata->CursorDisp         = GetTagUL(SWA_NNews_Editor_CursorDisp, FALSE, attrl);
        idata->XTraObj1           = GetTagObj(SWA_NNews_Editor_XTraObj1,   NULL,  attrl);
        idata->CmdObj             = GetTagObj(SWA_NNews_Editor_CmdObj,     NULL,  attrl);
        idata->AskClob            = GetTagUL(SWA_NNews_Editor_AskClob,   FALSE,  attrl);
        idata->ReadOnly           = GetTagUL(SWA_NNews_Editor_ReadOnly,  FALSE,  attrl);
        idata->App                = NN_App(idata);

        strcpy(idata->FSpec,
               GetTagStr(SWA_NNews_Editor_FSpec, "",  attrl));

        if (NoDelayInit)
           DoMethod(obj, SWM_NNews_Editor_Create);

        return (ULONG)obj;
     }

     case OM_DISPOSE: {
        DoMethod(o, SWM_NNews_Editor_Destroy);
        MUI_DisposeObject(idata->XTraObj1);
        break;
     }

     case OM_SET: {
        break;
     }

     case SWM_NNews_Editor_Create: {
        if (idata->TEDGroup)
           return SUCCESS;

        set(idata->App, MUIA_Application_Sleep, TRUE);
        
        if (!(idata->TEDGroup = MakeTEDGroup(o, idata))) {
           set(idata->App, MUIA_Application_Sleep, FALSE);
           return FAILURE;
        }

        set(idata->App, MUIA_Application_Sleep, FALSE);

        DoMethod(o, MUIM_Group_InitChange);
        DoMethod(o, OM_ADDMEMBER, idata->TEDGroup);
        DoMethod(o, OM_REMMEMBER, idata->Filler);
        DoMethod(o, MUIM_Group_ExitChange);

        if (idata->Win)
           set(idata->Win, MUIA_Window_ActiveObject, idata->TEditor);
        
        if (idata->CmdObj)
           DoMethod(idata->App, MUIM_Application_PushMethod,
                    idata->CmdObj, 1, SWM_NNews_Editor_SetupDone);
        
        return SUCCESS;
     }

     case SWM_NNews_Editor_Destroy: {
        if (idata->TEDGroup) {
           HaltNotify(idata);
           
           DoMethod(o, MUIM_Group_InitChange);

           DoMethod(o, OM_ADDMEMBER, idata->Filler);
           DoMethod(o, OM_REMMEMBER, idata->TEDGroup);

           DoMethod(o, MUIM_Group_ExitChange);

           if (idata->XTraObj1)
              DoMethod(idata->CursorGrp, OM_REMMEMBER, idata->XTraObj1);
   
           MUI_DisposeObject(idata->TEDGroup);
           idata->TEDGroup  = NULL;
           idata->T_Cursor  = NULL;
           idata->CursorGrp = NULL;
           idata->TEditor   = NULL;
        }

        return SUCCESS;
     }

     case MUIM_Show: {
        rc = DoSuperMethodA(cl, o, msg);
        
        if (!idata->TEDGroup) {
           idata->Win = _win(o);
           DoMethod(idata->App, MUIM_Application_PushMethod, o, 1,
                    SWM_NNews_Editor_Create);
        }

        return rc;
     }

     case MUIM_Hide: {
        break;
     }

     case SWM_NNews_Editor_UpdCursPos: {
        TEXT CurPos[32];

        if (!idata->TEDGroup || !idata->T_Cursor)
           return FAILURE;

        sprintf(CurPos, "X=%ld Y=%ld",
                SWGetUL(idata->TEditor, MUIA_TextEditor_CursorX),
                SWGetUL(idata->TEditor, MUIA_TextEditor_CursorY));

        set(idata->T_Cursor, MUIA_Text_Contents, CurPos);
        return SUCCESS;
     }


     case MUIM_TextEditor_InsertText:
      NewContents(idata, ((struct MUIP_TextEditor_InsertText *)msg)->text, 0);
      goto PassThrough;

     case MUIM_TextEditor_ClearText:
      NewContents(idata, NULL, 0);
      goto PassThrough;
           
     case MUIM_TextEditor_ARexxCmd:
     case MUIM_TextEditor_ExportText:
      goto PassThrough;

     case SWM_NNews_Editor_EdSave: {
        STRPTR SF;
   
        if (!idata->TEDGroup || !idata->TEditor)
           return FAILURE;

        SF = (STRPTR)DoMethod(idata->SWA_AppBase_TopObj,
                              SWM_Root_RequestFileName,
                              SWT_Root_SL_Win,         idata->Win,
                              SWT_Root_SL_Title,       GetS(SWS_TE_SaveBuff),
                              SWT_Root_SL_InitDrawer,  "RAM:",
                              SWT_Root_SL_Buffer,      FSpec,
                              SWT_Root_SL_SaveMode,    TRUE,
                              TAG_DONE);

        if (!SF) {
           MUI_Request(idata->App, idata->Win, 0, NULL,
                       GetS(SWS_B_OK), GetS(SWS_L_Canceled));
           return FAILURE;
        }

        DoMethod(o, SWM_NNews_Editor_SaveEditData, SF);
        
        return SUCCESS;
     }

     case SWM_NNews_Editor_InsFile: {
        FData = SWReadFileContents(InsFl->FSpec, &Size, &Err);

        if (Err || !FData) {
           SWERR_NM(idata->SWA_AppBase_TopObj, NULL, SWDecodeErrStr(Err),
                    InsFl->FSpec, 0);
           DoMethod(idata->SWA_AppBase_TopObj, SWM_Root_CheckErr, idata->Win);
           return FAILURE;
        }

        if (!(FData = QuoteText(FData, &Size, 0, InsFl->QPrefix)))
           return FAILURE;

        if (InsFl->Clear)
           DoMethod(idata->TEditor, MUIM_TextEditor_ClearText);
        
        DoMethod(idata->TEditor, MUIM_TextEditor_InsertText, FData,
                 MUIV_TextEditor_InsertText_Cursor);

        NewContents(idata, FData, Size);

        set(idata->TEditor, MUIA_TextEditor_HasChanged, !InsFl->Clear);
        
        FreeVec(FData);

        return SUCCESS;
     };
      
     case SWM_NNews_Editor_EdIns: {
        STRPTR IF;
   
        if (!idata->TEDGroup || !idata->TEditor)
           return FAILURE;

        IF = (STRPTR)DoMethod(idata->SWA_AppBase_TopObj,
                              SWM_Root_RequestFileName,
                              SWT_Root_SL_Win,         idata->Win,
                              SWT_Root_SL_Title,       GetS(SWS_TE_InsBuff),
                              SWT_Root_SL_InitDrawer,  "RAM:",
                              SWT_Root_SL_Buffer,      FSpec,
                              SWT_Root_SL_SaveMode,    FALSE,
                              TAG_DONE);

        if (!IF) {
           MUI_Request(idata->App, idata->Win, 0, NULL,
                       GetS(SWS_B_OK), GetS(SWS_L_Canceled));
           return SUCCESS;
        }

        return DoMethod(o, SWM_NNews_Editor_InsFile, IF, EDIns->QPrefix, FALSE);
     }

     case SWM_NNews_Editor_SaveEditData: {
        STRPTR Buff;

        if (SW_STRISNULL(SvED->FSpec)) return FAILURE;

        if (Buff = (STRPTR)DoMethod(idata->TEditor, MUIM_TextEditor_ExportText)) {
           if (Err = SWWriteFileString(SvED->FSpec, Buff, 0)) {
              SWERR_NM(idata->SWA_AppBase_TopObj, NULL, SWDecodeErrStr(Err),
                       SvED->FSpec, 0);
              DoMethod(idata->SWA_AppBase_TopObj, SWM_Root_CheckErr, idata->Win);
           }
           FreeVec(Buff);
        }
        
        return (ULONG)(Err ? FAILURE : SUCCESS);
     }

     case SWM_NNews_Editor_AppMessage: {
        if (DoMethod(o, SWM_NNews_Editor_AskClob) != SUCCESS)
           return FAILURE;

        if (ApMsg->AMsg->am_NumArgs == 0) return SUCCESS;

        NameFromLock(ApMsg->AMsg->am_ArgList[0].wa_Lock, FSpec, FMSIZE);
        AddPart(FSpec, ApMsg->AMsg->am_ArgList[0].wa_Name,      FMSIZE);

        return DoMethod(o, SWM_NNews_Editor_InsFile, FSpec, NULL, TRUE);
     }

     case SWM_NNews_Editor_UpdateEF: {
        // -- make sure we really got the signal -----------------------------

        if (!idata->Port || !GetMsg(idata->Port))
           return FAILURE;

        return DoMethod(o, SWM_NNews_Editor_InsFile, idata->FSpec, NULL, TRUE);
     }

     case SWM_NNews_Editor_InitNotify: {
        InitNotify(o, idata, INot->FSpec);
        return SUCCESS;
     }

     case SWM_NNews_Editor_HaltNotify: {
        HaltNotify(idata);
        return SUCCESS;
     }

     case SWM_NNews_Editor_AskClob: {
        if (!idata->AskClob || !idata->TEditor ||
            !SWGetUL(idata->TEditor, MUIA_TextEditor_HasChanged))
           return SUCCESS;

        rc = MUI_Request(idata->App, idata->Win,
                         0, NULL,
                         GetS(SWS_LE_YN), GetS(SWS_LE_AskMod), NULL);

        return (ULONG)(rc == 1 ? SUCCESS : FAILURE);
     }
   }

   return DoSuperMethodA(cl, o, msg);

  PassThrough:
   return idata->TEditor ? DoMethodA(idata->TEditor, msg) : NULL;
}
