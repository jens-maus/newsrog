# include "S-FastStrStr.h"

/*****************************************************************************
 * Function: jump to list pos                                                *
 *****************************************************************************/

static void JumpPos(SW_IDATA *idata, ULONG pos, ULONG Wild, STRPTR Str)
{
   if (pos < idata->SWA_ListSearch_InitSrchPos)
      pos = MUIV_List_Active_Off;

   set(idata->SWA_ListSearch_List,      MUIA_List_Active, pos);

   if (pos != MUIV_List_Active_Off)
      DoMethod(idata->SWA_ListSearch_List, MUIM_List_Jump,   pos);

   if (idata->SWA_ListSearch_Flags & SWF_ListSearch_HLMethod)
      DoMethod(idata->SWA_ListSearch_List, SWM_ListSearch_HLMatch,
               pos, Str,
               idata->SWA_ListSearch_Case,
               idata->SWA_ListSearch_First,
               Wild);
}


/*****************************************************************************
 * Function: reset search position                                           *
 *****************************************************************************/

static __inline void ResetPos(SW_IDATA *idata, ULONG Pos)
{
   idata->NewOpts = TRUE;
   idata->SWA_ListSearch_PrevSrchPos = Pos;
}


/*****************************************************************************
 * Function: init search position to first selected item in list             *
 *****************************************************************************/

static void InitSrchPos(SW_IDATA *idata, ULONG Active)
{
   if (Active)
      idata->SWA_ListSearch_InitSrchPos = SWGetUL(idata->SWA_ListSearch_List,
                                                  MUIA_List_Active);
   else
      idata->SWA_ListSearch_InitSrchPos = 0;

   if (idata->SWA_ListSearch_InitSrchPos == MUIV_List_Active_Off)
      idata->SWA_ListSearch_InitSrchPos = 0;

   ResetPos(idata, idata->SWA_ListSearch_InitSrchPos);
}


/*****************************************************************************
 * Method: Open ListSearch window                                            *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h)
{
   return SWUI_Button(GetS(l), GetS(h));
}

static Object *MChk(ULONG Label, ULONG Help, ULONG Init)
{
   Object *c, *g = HGroup,
      MUIA_ShortHelp, GetS(Help),
      Child, HSpace(0),
      Child, Label2(GetS(Label)),
      Child, c = CheckMark(Init),
   End;

   if (c) set(c, MUIA_CycleChain, 1);

   return g;
}

METHOD(SWM_ListSearch_OpenWin, SWP_ListSearch_OpenWin *)
{
   Object *b_Search, *b_Unsel, *b_Toggle, *b_Sel,
          *b_Case, *b_Sub, *b_1st, *b_Dyn, *b_Top, *ui;

   if (idata->Win) {
      SetAttrs(idata->Win,
               MUIA_Window_Open,         TRUE,
               MUIA_Window_ActiveObject, idata->s_Pattern,
               TAG_DONE);
      DoMethod(idata->Win, MUIM_Window_ToFront);
      InitSrchPos(idata, TRUE);
      SWDB_RET(SUCCESS);
   }

   ui = (idata->SWA_ListSearch_UI) ? idata->SWA_ListSearch_UI :
      RectangleObject, MUIA_VertWeight, 1, End;

   idata->App = msg->App;

   idata->Win = WindowObject,
      MUIA_Window_Title, SW_STRDEF(idata->SWA_ListSearch_Title,
                                   GetS(SWS_L_DefTitle)),
      MUIA_Window_ID,    idata->SWA_ListSearch_WindowID,
      MUIA_Window_IsSubWindow, TRUE,
      WindowContents, HGroup,
         MUIA_Background, MUII_GroupBack,
         GroupFrame,
         Child, VGroup,
            GroupFrame,
            Child, VSpace(0),
            Child, ColGroup(2),
               Child, Label2(SW_STRDEF(idata->SWA_ListSearch_StrLabel,
                                       GetS(SWS_L_Pattern))),
               Child, idata->s_Pattern = StringObject,
                  StringFrame,
                  MUIA_CycleChain,    1,
                  MUIA_String_MaxLen, MaxLen-1,
               End,
               Child, b_Search       = MkBt(SWS_L_Search, SWS_H_Search),
               Child, idata->SWA_ListSearch_StatusTxt = SWUI_FrameText(0, 0, 0, 0),
            End,
            Child, VSpace(0),
            Child, ui,
         End,
         Child, VGroup,
            GroupFrame,
            MUIA_HorizWeight, 1,
            Child, VSpace(0),
            Child, b_Top    = MkBt(SWS_L_FromTop,   SWS_H_FromTop),
            Child, b_Sel    = MkBt(SWS_L_SelAll,    SWS_H_SelAll),
            Child, b_Unsel  = MkBt(SWS_L_UnselAll,  SWS_H_UnselAll),
            Child, b_Toggle = MkBt(SWS_L_ToggleAll, SWS_H_ToggleAll),
            Child, VSpace(0),
         End,
         Child, VGroup,
            GroupFrame,
            MUIA_HorizWeight, 1,
            Child, VSpace(0),
            Child, b_Case = MChk(SWS_L_Case,     SWS_H_Case,
                                 idata->SWA_ListSearch_Case),
            Child, b_Sub  = MChk(SWS_L_SubString,SWS_H_SubString,
                                 idata->SWA_ListSearch_SubLine),
            Child, b_1st  = MChk(SWS_L_FirstOnly,SWS_H_FirstOnly,
                                 idata->SWA_ListSearch_First),
            Child, b_Dyn  = MChk(SWS_L_Dynamic,  SWS_H_Dynamic,
                                 idata->SWA_ListSearch_Dynamic),
            Child, VSpace(0),
         End,
      End,
   End;
   
   if (!idata->Win) SWDB_RET(FAILURE);

#  define CkNt(b,a) \
   DoMethod(b, MUIM_Notify, MUIA_Selected, MUIV_EveryTime, o, 3, \
            MUIM_Set, a, MUIV_TriggerValue)

   set(b_Search, MUIA_HorizWeight, 1);

   CkNt(b_Case, SWA_ListSearch_Case);
   CkNt(b_Sub,  SWA_ListSearch_SubLine);
   CkNt(b_1st,  SWA_ListSearch_First);
   CkNt(b_Dyn,  SWA_ListSearch_Dynamic);

   DoMethod(b_Sel, MUIM_Notify, MUIA_Pressed, FALSE,
            idata->SWA_ListSearch_List, 4,
            MUIM_List_Select, MUIV_List_Select_All,
            MUIV_List_Select_On, NULL);

   DoMethod(b_Unsel, NOTP, idata->SWA_ListSearch_List, 4,
            MUIM_List_Select, MUIV_List_Select_All,
            MUIV_List_Select_Off, NULL);

   DoMethod(b_Toggle, NOTP, idata->SWA_ListSearch_List, 4,
            MUIM_List_Select, MUIV_List_Select_All,
            MUIV_List_Select_Toggle, NULL);

   DoMethod(b_Search, NOTP, o, 3, SWM_ListSearch_Search, NULL, FALSE);

   DoMethod(b_Top, NOTP, o, 1, SWM_ListSearch_FromTop);
   
   DoMethod(idata->s_Pattern, MUIM_Notify,
            MUIA_String_Contents, MUIV_EveryTime,
            o, 3, SWM_ListSearch_Search, MUIV_TriggerValue, TRUE);

   DoMethod(idata->s_Pattern, MUIM_Notify,
            MUIA_String_Acknowledge, MUIV_EveryTime,
            o, 3, SWM_ListSearch_Search, MUIV_TriggerValue, FALSE);

   DoMethod(idata->s_Pattern, MUIM_Notify,
            MUIA_String_Acknowledge, MUIV_EveryTime,
            MUIV_Notify_Window, 3, MUIM_Set,
            MUIA_Window_ActiveObject, idata->s_Pattern);
   
   SW_NOTIFYCLOSEWIN(idata->Win);
   DoMethod(idata->App, OM_ADDMEMBER, idata->Win);

   SetAttrs(idata->Win,
            MUIA_Window_DefaultObject, idata->s_Pattern,
            MUIA_Window_ActiveObject,  idata->s_Pattern,
            MUIA_Window_Open,          TRUE,
            TAG_DONE);

   InitSrchPos(idata, TRUE);
   ResetPos(idata, idata->SWA_ListSearch_InitSrchPos);
   
   if (idata->SWA_ListSearch_List)
      DoMethod(idata->SWA_ListSearch_List, MUIM_Notify, SWA_NList_Changed, TRUE,
               o, 1, SWM_ListSearch_ListChg);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Close ListSearch window                                           *
 *****************************************************************************/

METHOD(SWM_ListSearch_CloseWin, Msg)
{
   if (!idata->Win) SWDB_RET(0);

   if (idata->SWA_ListSearch_List)
      DoMethod(idata->SWA_ListSearch_List, MUIM_KillNotifyObj,
               SWA_NList_Changed, o);
   
   set(idata->Win, MUIA_Window_Open, FALSE);
   DoMethod(idata->App, OM_REMMEMBER, idata->Win);
   MUI_DisposeObject(idata->Win);
   idata->Win                      = NULL;
   idata->SWA_ListSearch_StatusTxt = NULL;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: search by calling cmd fn on list elements                       *
 *****************************************************************************/

static __inline ULONG SearchCmp(SW_IDATA *idata, ULONG len, STRPTR origstr,
                                LONG Wild, LONG Case)
{
   STRPTR SrchStr;
   ULONG x, c = 0, found = 0, Match;
   LONG first=-1;

   for (x=idata->SWA_ListSearch_PrevSrchPos; ; x++) {
      Object *Entry = NULL;

      if (idata->SWA_ListSearch_UpdSrchCnt &&
          !(x % idata->SWA_ListSearch_UpdSrchCnt)) {
         sprintf(idata->FTxt, GetS(SWS_L_SearchingNum), x);
         set(idata->SWA_ListSearch_StatusTxt, MUIA_Text_Contents, idata->FTxt);
      }

      DoMethod(idata->SWA_ListSearch_List, MUIM_List_GetEntry, x, &Entry);
      if (!Entry) break;

      switch (idata->SWA_ListSearch_CmpMethod) {
        case SWV_ListSearch_SimpleStr:
           SrchStr = (STRPTR)Entry;
           break;
           
        case SWV_ListSearch_ObjAttr:
           SrchStr = SWGetStr(Entry, idata->SWA_ListSearch_ObjAttr);
           break;
      
        default:
           SrchStr = (STRPTR)DoMethod(idata->SWA_ListSearch_SearchObj,
                                      idata->SWA_ListSearch_CmpMethod,
                                      Entry, Wild ? idata->PBuff : origstr,
                                      Case, len, idata->SWA_ListSearch_SubLine,
                                      Wild, origstr);
           break;
        }

      if (!SrchStr) continue;

      // -- see if we got a match --------------------------------------------
      
      switch ((ULONG)SrchStr) {
        case SWV_ListSearch_CmpMethod_Found:
         Match = TRUE;
         break;

        case SWV_ListSearch_CmpMethod_NotFound:
         Match = FALSE;
         break;

        default:
          if (Wild) {
             Match = (Case?MatchPattern:MatchPatternNoCase)(idata->PBuff, SrchStr);
          } else {
             Match = findstr(SrchStr, origstr, LONG_MAX, len, Case,
                             idata->SWA_ListSearch_SubLine);
          }
      }

      if (Match) {
         found++;
         if (first < 0) first = x;
         if (idata->SWA_ListSearch_First) break;

         DoMethod(idata->SWA_ListSearch_List, MUIM_List_Select, x,
                  MUIV_List_Select_On, NULL);
      }

      if (++c >= 500) {
         c = 0;
         DoMethod(_app(idata->Win), MUIM_Application_CheckRefresh);
      }
   }

   if (first >= 0) {
      JumpPos(idata, idata->SWA_ListSearch_PrevSrchPos = first, Wild, origstr);

      if (idata->SWA_ListSearch_Dynamic) {
         for (x=idata->PrevPatLen; x<len; x++)
            idata->PosStk[x] = idata->PosStk[idata->PrevPatLen];
         
         idata->PosStk[len]  = first;
      }
   }

   return found;
}


/*****************************************************************************
 * Function: search by calling search fn on list                             *
 *****************************************************************************/

static ULONG SearchAll(SW_IDATA *idata)
{
   if (!idata->SWA_ListSearch_SearchObj)
      return 0;

   return DoMethod(idata->SWA_ListSearch_SearchObj,
                   idata->SWA_ListSearch_SearchMethod,
                   idata->PBuff,
                   idata->SWA_ListSearch_Case,
                   idata->SWA_ListSearch_First);
}

   
/*****************************************************************************
 * Function: see if this pattern is same as previous one to N chars          *
 *****************************************************************************/

static LONG SamePat(SW_IDATA *idata, STRPTR origstr, ULONG n)
{
   if (SW_STRISNULL(idata->SWA_ListSearch_PrevPat) &&
       !SW_STRISNULL(origstr))
      return FALSE;

   if (SW_STRISNULL(idata->SWA_ListSearch_PrevPat))
      return TRUE;

   return strncmp(idata->SWA_ListSearch_PrevPat, origstr, n) == 0;
}


/*****************************************************************************
 * Method: Search                                                            *
 *****************************************************************************/

METHOD(SWM_ListSearch_Search, SWP_ListSearch_Search_I *)
{
   LONG   Wild, State = 0;
   ULONG  Found, len, olen;
   STRPTR ss = SW_STRDEF(msg->Str, SWGetStr(idata->s_Pattern,
                                            MUIA_String_Contents)),
          origstr = ss;
      
   if (msg->Dyn && !idata->SWA_ListSearch_Dynamic)
      SWDB_RET(0);

   olen = len = strlen(ss);
   
   if (idata->PrevPatLen == olen &&
       SamePat(idata, origstr, olen) &&
       msg->Dyn && !idata->NewOpts)
      SWDB_RET(0);

   DoMethod(idata->SWA_ListSearch_List, SWM_ListSearch_Setup, &State);


   if (!idata->SWA_ListSearch_First)
      DoMethod(idata->SWA_ListSearch_List, MUIM_List_Select, MUIV_List_Select_All,
               MUIV_List_Select_Off, NULL);

   JumpPos(idata, MUIV_List_Active_Off, FALSE, "");

   if (!len) {
      ResetPos(idata, idata->SWA_ListSearch_InitSrchPos = 0);
      JumpPos(idata, 0, FALSE, "");
      set(o, SWA_ListSearch_PrevPat, "");
      idata->PrevPatLen = 0;
      SWDB_RET(0);
   }

   Wild = strpbrk(ss, "?#(|)~[]%*\\") ? TRUE : FALSE;

   if (idata->SWA_ListSearch_SubLine && Wild) {
      idata->Buff[0] = '*';
      idata->Buff[1] = '\0';
      ss = strcat(strcat(idata->Buff, ss), "*");
   }

   if (olen < idata->PrevPatLen &&
       SamePat(idata, origstr, olen)) {
      idata->NewOpts = TRUE;
      if (idata->SWA_ListSearch_Dynamic)
         JumpPos(idata, idata->SWA_ListSearch_PrevSrchPos=idata->PosStk[olen],
                 Wild, origstr);
      else
         ResetPos(idata, idata->SWA_ListSearch_InitSrchPos);
   }

   // if searching again, start at next element
   if (olen == idata->PrevPatLen &&
       SamePat(idata, origstr, olen) && !idata->NewOpts)
      idata->SWA_ListSearch_PrevSrchPos++;
   idata->NewOpts = FALSE;

   if ((Wild = (idata->SWA_ListSearch_Case ? ParsePattern : ParsePatternNoCase)
               (ss, idata->PBuff, PBLen)) < 0) {
      set(idata->SWA_ListSearch_StatusTxt, MUIA_Text_Contents, GetS(SWS_L_BadPatt));
      idata->PosStk[idata->PrevPatLen = olen] = idata->SWA_ListSearch_InitSrchPos;
      SWDB_RET(0);
   }

   set(idata->SWA_ListSearch_StatusTxt, MUIA_Text_Contents, GetS(SWS_L_Searching));
   
   if (!msg->Dyn)
      set(_app(idata->Win), MUIA_Application_Sleep, TRUE);

   Found=(idata->SWA_ListSearch_CmpMethod      ?
          SearchCmp(idata, olen, origstr, Wild, idata->SWA_ListSearch_Case) :
          SearchAll(idata));

   idata->PrevPatLen = olen;
   set(o, SWA_ListSearch_PrevPat, origstr);
   
   if (!msg->Dyn)
      set(_app(idata->Win), MUIA_Application_Sleep, FALSE);

   sprintf(idata->FTxt, "Matched: %ld %s", Found,
           Found == 1 ?
           SW_STRDEF(idata->SWA_ListSearch_ItemName,   GetS(SWS_L_Item)) :
           SW_STRDEF(idata->SWA_ListSearch_ItemNamePl, GetS(SWS_L_Items)));

   set(idata->SWA_ListSearch_StatusTxt, MUIA_Text_Contents, idata->FTxt);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: from top                                                          *
 *****************************************************************************/

METHOD(SWM_ListSearch_FromTop, Msg)
{
   InitSrchPos(idata, FALSE);
   ResetPos(idata, 0);
   SWDB_RET(DoMethod(o, SWM_ListSearch_Search, NULL, FALSE));
}


/*****************************************************************************
 * Method: the list changed                                                  *
 *****************************************************************************/

METHOD(SWM_ListSearch_ListChg, Msg)
{
   idata->NewOpts = TRUE;
   SetAttrs(o,
            SWA_ListSearch_PrevSrchPos, 0,
            SWA_ListSearch_InitSrchPos, 0,
            TAG_DONE);

   SWDB_RET(0);
}
