# include "../C-NArticle/S-LinePrefix.h"

/*****************************************************************************
 * Function: make checkmark                                                  *
 *****************************************************************************/

static Object *QCheck(ULONG ival, ULONG h)
{
   Object *b = CheckMark(ival);

   SetAttrs(b,
            MUIA_ShortHelp,  h ? GetS(h) : "",
            MUIA_CycleChain, TRUE,
            TAG_DONE);

   return b;
}


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NDFilter_UI, Msg)
{
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Make Toolbar UI for this object                                   *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBUI, SWP_NDFilter_TBUI *)
{
   Object *cm, *str;
   
   if (!msg->Grp) SWDB_RET(FAILURE);

   DoMethod(msg->Grp, OM_ADDMEMBER, Label(GetS(SWS_L_UseOnOffBt)));

   DoMethod(msg->Grp, OM_ADDMEMBER,
       (HGroup,
          MUIA_ShortHelp, GetS(SWS_H_UseOnOffBt),
          Child, cm = QCheck(idata->SWA_NDFilter_UseOnOffBt, SWS_H_UseOnOffBt),
          Child, PopaslObject,
             MUIA_Popstring_String, str = NewObject(SWObjStringClass(), NULL,
                StringFrame,
                MUIA_String_MaxLen,   FMSIZE,
                MUIA_String_Contents, idata->SWA_NDFilter_OnOffBtImg,
             TAG_DONE),
             MUIA_Popstring_Button, PopButton(MUII_PopDrawer),
             MUIA_Popasl_Type,      ASL_FileRequest,
          End,
       End));

   set(str, MUIA_Disabled, !idata->SWA_NDFilter_UseOnOffBt);

   DoMethod(cm, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NDFilter_UseOnOffBt, MUIV_TriggerValue);
  
   DoMethod(cm, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            str, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue);
  
   DoMethod(str, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NDFilter_OnOffBtImg, MUIV_TriggerValue);
  
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# include "S-TrackWinHash.h"


METHOD(SWM_NDFilter_ShowUI, SWP_NDFilter_ShowUI *)
{
   Object *ui, *globui, *eline, *b_print, *b_save, *b_reply;
   ULONG Hash = SWGetUL(o, SWA_ObjClassHash);
   static STRPTR Pages[4];

   Pages[0] = GetS(SWS_L_Edit),
   Pages[1] = GetS(SWS_L_Global),
   Pages[2] = GetS(SWS_L_AuthNote),
   Pages[3] = NULL;

   // just bring existing window to front --------

   if (!msg->App) SWDB_RET(0);

   idata->App                = msg->App;
   idata->SWA_AppBase_TopObj = (Object *)muiUserData(msg->App);
   
   if (idata->UIWin) {
      DoMethod(idata->UIWin, MUIM_Window_ToFront);
      SWDB_RET(SUCCESS);
   }

   if (!(ui = (Object *)DoMethod(o, SWM_NDFilter_UI)))
      SWDB_RET(FAILURE);

   globui = ColGroup(2),
      Child, Label2(GetS(SWS_L_Desc)),
      Child, idata->ui_s1 = NewObject(SWObjStringClass(), NULL,
                            StringFrame,
                            MUIA_ShortHelp,     GetS(SWS_H_Desc),
                            MUIA_CycleChain,    1,
                            MUIA_String_MaxLen, SW_ATTRMAP[0].Max-1,
                            TAG_DONE),
      
      Child, Label2(GetS(SWS_L_Enabled)),
      Child, eline = HGroup,
         MUIA_Weight, 1,
         Child, idata->ui_b1 = QCheck(idata->SWA_NDFilter_Enabled, SWS_H_Enabled),
      End,
   End;

   if (!globui) SWDB_RET(FAILURE);

   // -- add UI for text only mods for print/save/etc ------------------------
   
   if (SWGetUL(o, SWA_NDFilter_HasTxtMod)) {
      DoMethod(eline, OM_ADDMEMBER, HSpace(0));
      DoMethod(eline, OM_ADDMEMBER, Label2(GetS(SWS_L_ForPrint)));
      DoMethod(eline, OM_ADDMEMBER,
               b_print = QCheck(idata->SWA_NDFilter_T_Print,
                                          SWS_H_ForPrint));
      DoMethod(eline, OM_ADDMEMBER, HSpace(0));
      DoMethod(eline, OM_ADDMEMBER, Label2(GetS(SWS_L_ForSave)));
      DoMethod(eline, OM_ADDMEMBER,
               b_save = QCheck(idata->SWA_NDFilter_T_Save,
                               SWS_H_ForSave));
      DoMethod(eline, OM_ADDMEMBER, HSpace(0));
      DoMethod(eline, OM_ADDMEMBER, Label2(GetS(SWS_L_ForReply)));
      DoMethod(eline, OM_ADDMEMBER,
               b_reply = QCheck(idata->SWA_NDFilter_T_NetReply,
                                SWS_H_ForReply));

      DoMethod(b_print, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
               o, 3, MUIM_Set, SWA_NDFilter_T_Print, MUIV_TriggerValue);

      DoMethod(b_save, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
               o, 3, MUIM_Set, SWA_NDFilter_T_Save, MUIV_TriggerValue);

      DoMethod(b_reply, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
               o, 3, MUIM_Set, SWA_NDFilter_T_NetReply, MUIV_TriggerValue);
   }

   DoMethod(eline, OM_ADDMEMBER, HSpace(0));

   if (DoMethod(o, SWM_NDFilter_TBUI, globui) != SUCCESS)
      SWDB_RET(FAILURE);

   idata->UIWin = WindowObject,
      MUIA_Window_Title,       GetS(SWS_L_WinTitle),
      MUIA_Window_ID,          Hash + TrackWinHash(Hash, TWC_Open),
      MUIA_Window_IsSubWindow, TRUE,
      MUIA_Window_AppWindow,   TRUE,
      WindowContents, RegisterGroup(Pages),
         MUIA_Background, MUII_RegisterBack,
         MUIA_CycleChain, 1,
         Child, ui,
         Child, globui,
         Child, idata->Ed = NN_EditorObject,
            AppBaseDefTop(idata),
            SWA_NNews_Editor_InitStrAddr, &idata->SWA_NDFilter_AuthNote,
         End,
      End,
   End;

   // - set up notifications on various attributes ---------------------------

   if (!idata->UIWin) SWDB_RET(FAILURE);

   SetAttrs(idata->ui_b1, MUIA_HorizWeight, 1, MUIA_CycleChain, 1, TAG_DONE);

   set(idata->ui_s1, MUIA_String_Contents, idata->SWA_NDFilter_Desc);

   DoMethod(idata->ui_s1, MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NDFilter_Desc, MUIV_TriggerValue);

   DoMethod(idata->ui_b1, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 3, MUIM_Set, SWA_NDFilter_Enabled, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NDFilter_Desc, MUIV_EveryTime,
            idata->ui_s1, 3, MUIM_Set, MUIA_String_Contents, MUIV_TriggerValue);

   DoMethod(o, MUIM_Notify, SWA_NDFilter_Enabled, MUIV_EveryTime,
            idata->ui_b1, 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);

   DoMethod(msg->App, OM_ADDMEMBER, idata->UIWin);

   DoMethod(idata->UIWin, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
            msg->App, 4, MUIM_Application_PushMethod, o, 1,
            SWM_NDFilter_HideUI);
   
   set(idata->UIWin, MUIA_Window_Open, TRUE);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Hide UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NDFilter_HideUI, Msg)
{
   STRPTR Buff;
   
   if (!idata->UIWin) SWDB_RET(0);

   // -- save contents of editor ---------------------------------------------

   if (Buff = (STRPTR)DoMethod(idata->Ed, MUIM_TextEditor_ExportText)) {
      set(o, SWA_NDFilter_AuthNote, Buff);
      FreeVec(Buff);
   }

   // -- be sure to nuke ALL the notifications we set up! --------------------

   DoMethod(o, MUIM_KillNotifyObj, SWA_NDFilter_Desc,      idata->ui_s1);
   DoMethod(o, MUIM_KillNotifyObj, SWA_NDFilter_Enabled,   idata->ui_b1);

   TrackWinHash(SWGetUL(o, SWA_ObjClassHash), TWC_Close);

   set(idata->UIWin, MUIA_Window_Open, FALSE);
   DoMethod(idata->App, OM_REMMEMBER, idata->UIWin);
   MUI_DisposeObject(idata->UIWin);
   idata->UIWin = NULL;
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: get more space in buffer                                        *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBInit, SWP_NDFilter_TBInit *)
{
   memset(msg->Tx, 0, sizeof(TxtBuff));
   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Function: Determine if proper encapsulation                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBIsEncaps, SWP_NDFilter_TBIsEncaps *)
{
   STRPTR Buff = msg->s + msg->P1;
   ULONG x;
   LONG PenPos = 0, FontPos = 0;

   for (x=msg->P1; x<=msg->P2; x++, Buff++) {
      if (CT_IsPen(Buff))  PenPos  += CT_IsEmpty(Buff) ? -1 : 1;
      if (CT_IsFont(Buff)) FontPos += CT_IsEmpty(Buff) ? -1 : 1;

      if (PenPos < 0 || FontPos < 0)
         SWDB_RET(FALSE);
   }

   SWDB_RET(PenPos == 0 && FontPos == 0);
} SDFT


/*****************************************************************************
 * Function: Region has no ctrl codes?                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBNoCtrl, SWP_NDFilter_TBNoCtrl *)
{
   STRPTR Buff;
   ULONG x;

   if (!TxSize(msg->Tx))
      SWDB_RET(TRUE);

   Buff = TxBuff(msg->Tx) + msg->P1;

   for (x=msg->P1; x<=msg->P2; x++, Buff++)
      if (CT_IsCtrl(Buff)) SWDB_RET(FALSE);

   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Function: get more space in buffer                                        *
 *****************************************************************************/

static ULONG GetTBSpace(TxtBuff *Tx, ULONG Size, ULONG Preserve)
{
   STRPTR Buff;

   if (Size >= TxSize(Tx)) {
      Buff     = TxBuff(Tx);
      
      TxSize(Tx) = Size + max(Size>>2, 16);

      if (!(TxBuff(Tx) = AllocVec(TxSize(Tx), MEMF_ANY))) {
         FreeVec(Buff);
         TxSize(Tx) = TxLen(Tx) = 0;
         return FAILURE;
      }

      if (Buff) {
         if (Preserve)
            strcpy(TxBuff(Tx), Buff);
         FreeVec(Buff);
      }
   }

   return SUCCESS;
}


/*****************************************************************************
 * Method: get minimum text size                                             *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBGetSpace, SWP_NDFilter_TBGetSpace *)
{
   GetTBSpace(msg->Tx, max(TxLen(msg->Tx), msg->Size)+1, TRUE);
   
   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: append one character                                              *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBAppendChar, SWP_NDFilter_TBAppendChar *)
{
   GetTBSpace(msg->Tx, TxLen(msg->Tx)+2, TRUE);

   TxBuff(msg->Tx)[TxLen(msg->Tx)++] = (UBYTE)msg->Char;
   
   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Function: append substring                                                *
 *****************************************************************************/

static TxtBuff *AppendSub(TxtBuff *Tx1, STRPTR s, ULONG P1, ULONG P2)
{
   ULONG        l = P2 - P1;
   
   if (!Tx1)    return NULL;
   if (!s)      return Tx1;
   if (P1 > P2) return Tx1;

   if (s[P2]) l++;   // handle \0 or non-\0 last char

   GetTBSpace(Tx1, TxLen(Tx1) + l + 1, TRUE);

   strncpy(TxBuff(Tx1) + TxLen(Tx1), s + P1, l);
   
   TxBuff(Tx1)[TxLen(Tx1) += l] = '\0';

   return Tx1;
}


/*****************************************************************************
 * Method: Append text buffer                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBAppend, SWP_NDFilter_TBAppend *)
{
   SWDB_RET(AppendSub(msg->Tx1, TxBuff(msg->Tx2), 0, TxLen(msg->Tx2)));
} SDFT


/*****************************************************************************
 * Method: Append text buffer                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBAppendSub, SWP_NDFilter_TBAppendSub *)
{
   SWDB_RET(AppendSub(msg->Tx1, msg->s, msg->p1, msg->p2));
} SDFT


/*****************************************************************************
 * Method: Append text buffer                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBAppendTxt, SWP_NDFilter_TBAppendTxt *)
{
   SWDB_RET(msg->s ? AppendSub(msg->Tx, msg->s, 0, strlen(msg->s)) : msg->Tx);
} SDFT


/*****************************************************************************
 * Method: Prepend text buffer                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBPrependTxt, SWP_NDFilter_TBPrependTxt *)
{
   ULONG l;
   if (!msg->Tx || !msg->s) SWDB_RET(NULL);

   GetTBSpace(msg->Tx, TxLen(msg->Tx) + (l = strlen(msg->s)) + 1, TRUE);

   memmove(TxBuff(msg->Tx) + l, TxBuff(msg->Tx), TxLen(msg->Tx));
   memcpy(TxBuff(msg->Tx), msg->s, l);

   TxLen(msg->Tx) += l;

   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: Truncate buffer                                                   *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBTruncate, SWP_NDFilter_TBTruncate *)
{
   if (!msg->Tx) SWDB_RET(NULL);

   if (TxLen(msg->Tx) < msg->len) SWDB_RET(msg->Tx);

   TxBuff(msg->Tx)[TxLen(msg->Tx) = msg->len] = '\0';

   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: delete substring from P1 to P2-1 (leaving char at P2)             *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBDelSub, SWP_NDFilter_TBDelSub *)
{
   ULONG P1 = msg->P1,
         P2 = msg->P2,
         x;
   STRPTR Buff;
   
   if (!msg->Tx) SWDB_RET(NULL);

   Buff = TxBuff(msg->Tx);
   
   P1 = min(P1, TxLen(msg->Tx));
   P2 = min(P2, TxLen(msg->Tx));

   if (P1 == P2) SWDB_RET(msg->Tx);

   if (P2 < P1) x = P1, P1 = P2, P2 = x;

   for (x=0; Buff[x+P1] = Buff[x+P2]; x++);
   
   TxLen(msg->Tx) = x+P1;

   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: delete substring from P1 to P2-1 (leaving char at P2)             *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBCountChar, SWP_NDFilter_TBCountChar *)
{
   ULONG  Count = 0;
   STRPTR Buff;
   UBYTE  c = (UBYTE)msg->c;
   
   if (!msg->Tx) SWDB_RET(NULL);

   for (Buff = TxBuff(msg->Tx); *Buff; Buff++)
      if (*Buff == c)
         Count++;

   SWDB_RET(Count);
} SDFT


/*****************************************************************************
 * Method: Set text buffer                                                   *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBSet, SWP_NDFilter_TBSet *)
{
   if (!msg->Tx) SWDB_RET(NULL);

   if (!msg->s) SWDB_RET(DoMethod(o, SWM_NDFilter_TBClear, msg->Tx));

   GetTBSpace(msg->Tx, (TxLen(msg->Tx) = strlen(msg->s)) + 1, FALSE);
   
   strcpy(TxBuff(msg->Tx), msg->s);

   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: Clear text buffer                                                 *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBClear, SWP_NDFilter_TBClear *)
{
   if (!msg->Tx || !TxBuff(msg->Tx)) SWDB_RET(NULL);
   
   TxBuff(msg->Tx)[0] = '\0';
   TxLen(msg->Tx)     = 0;

   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: Dispose text buffer                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBDispose, SWP_NDFilter_TBDispose *)
{
   if (!msg->Tx) SWDB_RET(NULL);

   FreeVec(TxBuff(msg->Tx));

   SWDB_RET(DoMethod(o, SWM_NDFilter_TBInit, msg->Tx));
} SDFT


/*****************************************************************************
 * Method: Init multiple TxtBuffs, NULL terminated                           *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBMultiInit, ULONG *)
{
   ULONG x;

   for (x=1; msg[x]; x++)
      DoMethod(o, SWM_NDFilter_TBInit, msg[x]);

   SWDB_RET(0);
}


/*****************************************************************************
 * Method: Dispose multiple TxtBuffs, NULL terminated                        *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBMultiDispose, ULONG *)
{
   ULONG x;

   for (x=1; msg[x]; x++)
      DoMethod(o, SWM_NDFilter_TBDispose, msg[x]);

   SWDB_RET(0);
}


/*****************************************************************************
 * Method: Assign one TxtBuff to another, preserving first                   *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBAssign, SWP_NDFilter_TBAssign *)
{
   if (!msg->Tx1 || !msg->Tx2) SWDB_RET(NULL);

   GetTBSpace(msg->Tx1, TxLen(msg->Tx2)+4, FALSE);

   strcpy(TxBuff(msg->Tx1), TxBuff(msg->Tx2));
   msg->Tx1->TB_Len = msg->Tx2->TB_Len;

   SWDB_RET(msg->Tx1);
} SDFT


/*****************************************************************************
 * Method: Assign one TxtBuff to another, removing first                     *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBAssignDel, SWP_NDFilter_TBAssign *)
{
   if (!msg->Tx1 || !msg->Tx2) SWDB_RET(NULL);

   FreeVec(TxBuff(msg->Tx1));

   memcpy(msg->Tx1, msg->Tx2, sizeof(TxtBuff));
   memset(msg->Tx2, 0, sizeof(TxtBuff));

   SWDB_RET(msg->Tx1);
} SDFT


/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   SWDB_RET(SUCCESS);
} SDFT



/*****************************************************************************
 * Function: create styles                                                   *
 *****************************************************************************/

static LONG HasStyle(STRPTR Font, ULONG B, ULONG U, ULONG I)
{
   return !SW_STRISNULL(Font) || B || U || I;
}
   
static STRPTR StyleOn(STRPTR Style, STRPTR Font, ULONG B, ULONG U, ULONG I)
{
   if (!HasStyle(Font, B, U, I)) return "";

   strcpy(Style, CTxtFontSt);
   strcat(Style, SW_STRISNULL(Font) ? "/" : Font);

   if (B||U||I) strcat(Style, "/");
   if (B)       strcat(Style, "B");
   if (U)       strcat(Style, "U");
   if (I)       strcat(Style, "I");

   strcat(Style, CTxtCtrlEnd);

   return Style;
}

static STRPTR StyleOff(STRPTR Font, ULONG B, ULONG U, ULONG I)
{
   return HasStyle(Font, B, U, I) ? CTxtFont("") : "";
}

static STRPTR ColorOn(STRPTR Color, ULONG Use, MUI_PenSpec *ps)
{
   if (!Use) return "";
   
   sprintf(Color, "\033CV{%08lx}", ps);
   return Color;
}

static STRPTR ColorOff(ULONG Use)
{
   return Use ? CTxtPen("") : "";
}


/*****************************************************************************
 * Method: Style on                                                          *
 *****************************************************************************/

METHOD(SWM_NDFilter_StyleOn, SWP_NDFilter_Style *)
{
   TEXT Style[64];
   
   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx,
            StyleOn(Style, msg->Font, msg->B, msg->U, msg->I));
 
   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx,
            ColorOn(Style, msg->UseColor, msg->Color));
   
   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: Style off                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_StyleOff, SWP_NDFilter_Style *)
{
   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx,
            ColorOff(msg->UseColor));

   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx,
            StyleOff(msg->Font, msg->B, msg->U, msg->I));

   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: Add line prefix                                                   *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBAddPrefix, SWP_NDFilter_TBAddPrefix *)
{
   ULONG PrefLen, LineCount;
   STRPTR s, Buff, bp;
   
   if (!msg->Tx)                  SWDB_RET(NULL);
   if (SW_STRISNULL(msg->Prefix)) SWDB_RET(msg->Tx);

   PrefLen   = strlen(msg->Prefix);
   LineCount = SWCountLines(TxBuff(msg->Tx));

   if (!(Buff = AllocVec(TxLen(msg->Tx) + (LineCount+4) * PrefLen, MEMF_ANY)))
      SWDB_RET(msg->Tx);

   for (bp = Buff, s = TxBuff(msg->Tx); *s; *bp++ = *s++)
      if (bp == Buff || s[-1] == '\n')
         bp += strlen(strcpy(bp, LinePrefix(s+1, msg->Prefix, msg->QMode)));

   *bp = '\0';

   FreeVec(TxBuff(msg->Tx));
   TxBuff(msg->Tx) = Buff;
   TxLen(msg->Tx)  = 
   TxSize(msg->Tx) = bp - Buff;
   
   SWDB_RET(msg->Tx);
} SDFT


/*****************************************************************************
 * Method: Style text                                                        *
 *****************************************************************************/

METHOD(SWM_NDFilter_TBSkipHdrs, SWP_NDFilter_TBSkipHdrs *)
{
   STRPTR Buff = TxBuff(msg->Tx);
   ULONG  bol;
      
   if (!msg->Tx) SWDB_RET(NULL);

   if (!msg->HasHdrs) SWDB_RET(0);

   // -- skip headers --------------------------------------------------------

   for (bol=0; Buff[bol]; bol++)
      if (Buff[bol] == '\n' && Buff[bol+1] == '\n')
         SWDB_RET(bol+2);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Style text                                                        *
 *****************************************************************************/

METHOD(SWM_NDFilter_StyleTxt, SWP_NDFilter_StyleTxt *)
{
   LONG  pos;
   UBYTE c;
   
   DoMethod(o, SWM_NDFilter_StyleOn, msg->Tx, msg->Font,
            msg->B, msg->U, msg->I, msg->UseColor, msg->Color);

   if ((c = msg->Text[pos = (strlen(msg->Text)-1)]) == '\n')
      msg->Text[pos] = '\0';

   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx, msg->Text);

   DoMethod(o, SWM_NDFilter_StyleOff, msg->Tx, msg->Font,
            msg->B, msg->U, msg->I, msg->UseColor, msg->Color);

   if (c == '\n') {
      DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx, "\n");
      msg->Text[pos] = '\n';
   }

   SWDB_RET(msg->Tx);
} SDFT



/*****************************************************************************
 * Method: Last candidate line for parsing                                   *
 *****************************************************************************/

METHOD(SWM_NDFilter_LastParseLine, SWP_NDFilter_LastParseLine *)
{
   SWDB_RET(msg->Last);
} SDFT


/*****************************************************************************
 * Method: Add filter function icons                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_AddFFIcons, SWP_NDFilter_AddFFIcons *)
{
   Object *b;
   ULONG NewLen;

   if (!idata->SWA_NDFilter_UseOnOffBt ||
       SW_STRISNULL(idata->SWA_NDFilter_OnOffBtImg))
      SWDB_RET(SUCCESS);

   if (!idata->MemPool) {
      idata->SWA_AppBase_TopObj = SWGetObj(msg->Grp, SWA_AppBase_TopObj);
      idata->MemPool            = NN_MemPool(idata);
   }

   NewLen = strlen(idata->SWA_NDFilter_Desc) + strlen(GetS(SWS_H_TBOnOff)) + 8;

   if (!idata->TBDesc || NewLen >= idata->TBDescLen)
      RV(NewLen, &idata->TBDesc);

   if (!idata->TBDesc) SWDB_RET(FAILURE);

   idata->TBDescLen = NewLen;

   sprintf(idata->TBDesc, GetS(SWS_H_TBOnOff), idata->SWA_NDFilter_Desc);

   b = DTImgObject,
      MUIA_ShortHelp,       idata->TBDesc,
      MUIA_Selected,        idata->SWA_NDFilter_Enabled,
      MUIA_InnerTop,        0,
      MUIA_InnerBottom,     0,
      MUIA_InnerLeft,       0,
      MUIA_InnerRight,      0,
      MUIA_Frame,           MUIV_Frame_ImageButton,
      MUIA_InputMode,       MUIV_InputMode_Toggle,
      MUIA_CycleChain,      1,
      SWA_DTImg_FSpec,      idata->SWA_NDFilter_OnOffBtImg,
      SWA_DTImg_ImgCache,   NN_DTImages(idata),
   End;

   if (!b) SWDB_RET(FAILURE);

   DoMethod(b, MUIM_Notify, MUIA_Selected, MUIV_EveryTime,
            o, 4, SWM_NSFilter_FF_OnOff, msg->Grp, MUIV_TriggerValue,
            msg->NSFList);

   DoMethod(o, MUIM_Notify, SWA_NDFilter_Enabled, MUIV_EveryTime,
            b, 3, MUIM_Set, MUIA_Selected, MUIV_TriggerValue);
   
   SWDB_RET(DoMethod(msg->NSFList, SWM_NSFList_AddFFIcon, o, b));
} SDFT


/*****************************************************************************
 * Method: Remove filter function icons                                      *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemFFIcon, SWP_NDFilter_RemFFIcon *)
{
   DoMethod(o, MUIM_KillNotifyObj, SWA_NDFilter_Enabled, msg->Icon);
   
   MUI_DisposeObject(msg->Icon);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Turn filter on/off (for filter function button)                   *
 *****************************************************************************/

METHOD(SWM_NDFilter_FF_OnOff, SWP_NDFilter_FF_OnOff *)
{
   switch (msg->State) {
      case SWV_NDFilter_Off:    set(o, SWA_NDFilter_Enabled, FALSE); break;
      case SWV_NDFilter_On:     set(o, SWA_NDFilter_Enabled, TRUE);  break;
      case SWV_NDFilter_Toggle: set(o, SWA_NDFilter_Enabled,
                                    !idata->SWA_NDFilter_Enabled);   break;
      default: SWDB_RET(FAILURE);
   }

   DoMethod(msg->Grp, SWM_NGroup_Redisp, SWV_NGroup_Redisp_Cooked);
  
   SWDB_RET(SUCCESS);
} SDFT
