# include "S-FastStrStr.h"

/*****************************************************************************
 * Free filter buffers                                                       *
 *****************************************************************************/

static void FreeFltBuff(SW_IDATA *idata)
{
   ULONG x;

   if (idata->MData)
      for (x=0; x<idata->Entries; x++)
         FreeVec(idata->MData[x].PatBuff);
   
   FreeVec(idata->LCBuff);   idata->LCBuff   = NULL;

   idata->Entries = 0;
}


/*****************************************************************************
 * Method: Begin match tests                                                 *
 *****************************************************************************/

METHOD(SWM_NSFilter_Begin, SWP_NSFilter_Begin *)
{
   STRPTR toparse, tmp = NULL;
   ULONG  x, e, tlen = 0;
   Object *lo;
   
   idata->Total = idata->Found = 0;
   idata->Inv   = SWGetUL(o, SWA_NSFilter_Inv);
   idata->SWA_AppBase_TopObj = SWGetObj(msg->Grp, SWA_AppBase_TopObj);

   e = idata->Entries = SWGetUL(idata->SWA_NList_L, MUIA_List_Entries);

   if (!idata->Entries) SWDB_RET(SUCCESS);

   // -- Allocate buffer space -----------------------------------------------

   if (!(idata->MData = AllocVec(e * sizeof(struct _mdata), MEMF_ANY|MEMF_CLEAR)))
      SWDB_RET(FAILURE);
      
   if (!(idata->LCBuff  = AllocVec(SWV_MaxLCLen, MEMF_ANY|MEMF_CLEAR))) {
      FreeFltBuff(idata);
      SWDB_RET(FAILURE);
   }

   // -- loop through patterns, parsing each if it`s a wildcard --------------
   
   SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE) {
      STRPTR NVPat     = SWGetStr(lo, SWA_NStrFilter_Pattern), pb,
             Pat       = VL_Alloc(msg->Grp, idata, NVPat),
             OrigPat   = Pat;

      DoMethodA(lo, msg);
      
      idata->MData[x].Type =
         ((SWGetUL(lo, SWA_NStrFilter_Case)  ?SWV_HM_Case  :0) |
          (SWGetUL(lo, SWA_NStrFilter_Anchor)?SWV_HM_Anchor:0) |
          (SWGetUL(lo, SWA_NStrFilter_Wild)  ?SWV_HM_Wild  :0));
                             
      idata->MData[x].Header = (UBYTE)SWGetUL(lo, SWA_NStrFilter_Header);

      if (idata->MData[x].Header == SWV_HM_Subject)
         Pat = SWV_NArticle_SubjNoRe(Pat);

      idata->MData[x].PatLen = strlen(Pat);

      if (FIsSet(idata->MData[x].Type, SWV_HM_Wild)) {
         ULONG PBLen = idata->MData[x].PatLen*2+4;

         if (!(pb = idata->MData[x].PatBuff = AllocVec(PBLen, MEMF_ANY))) {
            FreeFltBuff(idata);
            FreeVec(tmp);
            SWDB_RET(FAILURE);
         }

         if (FIsSet(idata->MData[x].Type, SWV_HM_Anchor)) {
            toparse = Pat;
         } else {
            if ((strlen(Pat) + 3) > tlen) {
               tlen = strlen(Pat) + 3;
               FreeVec(tmp);
               tmp = AllocVec(tlen, MEMF_ANY);
            }

            toparse = strcat(strcat(strcpy(tmp, "*"), Pat), "*");
         }
         
         if ((FIsSet(idata->MData[x].Type, SWV_HM_Case)
              ? ParsePattern : ParsePatternNoCase)(toparse, pb, PBLen) < 0) {
            FreeVec(tmp);
            FreeFltBuff(idata);
            SWDB_RET(FAILURE);
         }
      } else {
         ULONG y;
         
         if (!(pb = idata->MData[x].PatBuff =
               AllocVec(idata->MData[x].PatLen+1, MEMF_ANY))) {
            FreeFltBuff(idata);
            FreeVec(tmp);
            SWDB_RET(FAILURE);
         }

         if (!FIsSet(idata->MData[x].Type, SWV_HM_Case)) {
            for (y=0; Pat[y]; y++)
               idata->MData[x].PatBuff[y] = toupper(Pat[y]);

            idata->MData[x].PatBuff[y] = '\0';
         } else
            strcpy(idata->MData[x].PatBuff, Pat);
      }

      VL_Free(msg->Grp, idata, OrigPat);
   }

   FreeVec(tmp);
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Determine if article is a match                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_Match, SWP_NSFilter_Match *)
{
   STRPTR HData[SWV_HM_HD_COUNT];
   ULONG  HLen[SWV_HM_HD_COUNT];
   LONG x, m, Match = (idata->Entries == 0) ? FALSE :
      (idata->SWA_NSF_HeaderMatch_Mode == SWV_HM_Mode_Or ? FALSE : TRUE);

   ULONG  Hdr, LCHdr=(ULONG)-1;
   
   if (!msg->Art) SWDB_RET(FALSE);

   memset(HData, 0, sizeof(HData));

   idata->Total++;

   // -- loop through each pattern -------------------------------------------
   
   for (x=0; x<idata->Entries; x++) {
      Hdr = idata->MData[x].Header;

      // -- only search if we look at a different hdr than last time ---------
      
      if (!HData[Hdr] ||
          Hdr!=LCHdr && !FIsSet(idata->MData[x].Type, SWV_HM_Case)) {
         
         switch (Hdr) {
           case SWV_HM_Subject:
               if (HData[Hdr] = (STRPTR)DoMethod(msg->Art, SWM_NArticle_GetSubj))
                  HData[Hdr] = SWV_NArticle_SubjNoRe(HData[Hdr]);
               break;
   
           case SWV_HM_MsgID:
               HData[Hdr] = (STRPTR)DoMethod(msg->Art, SWM_NArticle_GetMsgID);
               break;
   
           default:
               HData[Hdr] = (STRPTR)DoMethod(msg->Art, SWM_NArticle_FindHeader,
                                             Headers[Hdr]);
               break;
         }

         if (!HData[Hdr]) goto Done;
         
         if (!FIsSet(idata->MData[x].Type, SWV_HM_Case)) {
            for (m=0; HData[Hdr][m]; m++)
               idata->LCBuff[m] = toupper(HData[Hdr][m]);
            idata->LCBuff[m] = '\0';
            LCHdr = Hdr;
         }

         HLen[Hdr] = strlen(HData[Hdr]);
      }

#     define fc(f, c)   f(HData[Hdr],    idata->MData[x].PatBuff, HLen[Hdr], idata->MData[x].PatLen, c)
#     define fcnc(f, c) f(idata->LCBuff, idata->MData[x].PatBuff, HLen[Hdr], idata->MData[x].PatLen, c)
      
      switch (idata->MData[x].Type) {
        case SWV_HM_stristr: m = fcnc(faststrstr, TRUE); break;
        case SWV_HM_stricmp: m = fcnc(faststrcmp, TRUE); break;
        case SWV_HM_strstr:  m = fc(faststrstr,   TRUE); break;
        case SWV_HM_strcmp:  m = fc(faststrcmp,   TRUE); break;
        case SWV_HM_mpat:
        case SWV_HM_mpata:   m = MatchPatternNoCase(idata->MData[x].PatBuff, HData[Hdr]);
                             break;
        case SWV_HM_mpatc:
        case SWV_HM_mpatca:  m = MatchPattern(idata->MData[x].PatBuff, HData[Hdr]);
                             break;
      }

      idata->MData[x].MatchTry++;
      
      if (m) {
         idata->MData[x].MatchCnt++;
      
         if (idata->SWA_NSF_HeaderMatch_Mode  == SWV_HM_Mode_Or) {
            Match = TRUE;
            break;
         }
      }
   
      if (!m && idata->SWA_NSF_HeaderMatch_Mode == SWV_HM_Mode_And) {
         Match = FALSE;
         break;
      }
   }

  Done:
   if (idata->Inv) Match = !Match;
   if (Match)      idata->Found++;

   SWDB_RET(Match);
} SDFT
   

/*****************************************************************************
 * Method: End match tests                                                   *
 *****************************************************************************/

METHOD(SWM_NSFilter_End, Msg)
{
   ULONG x;
   Object *lo;
   
   SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE) {
      SetAttrs(lo,
               SWA_NStrFilter_Total, idata->MData[x].MatchTry,
               SWA_NStrFilter_Found, idata->MData[x].MatchCnt,
               TAG_DONE);
      
      DoMethodA(lo, msg);
   }
   
   FreeFltBuff(idata);
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Update statistics after xfer                                      *
 *****************************************************************************/

METHOD(SWM_NSFilter_TrackStats, SWP_NSFilter_TrackStats *)
{
   *msg->Total = idata->Total;
   *msg->Found = idata->Found;

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

METHOD(SWM_NSFilter_UI, Msg)
{
   Object *b_Add, *b_Del, *b_SelAll, *b_Edit;

   idata->UI = VGroup,
      GroupFrame,
      MUIA_Background,  MUII_GroupBack,
      MUIA_Group_Horiz, FALSE,
      Child, idata->title = SWUI_FrameText(GetS(SWS_L_TopTitle), 0, NULL, NULL),
      Child, o,
      Child, HGroup,
         Child, b_Edit    = MkBt(SWS_B_Edit,   SWS_H_Edit),
         Child, b_Del     = MkBt(SWS_B_Del,    SWS_H_Del),
         Child, b_SelAll  = MkBt(SWS_B_SelAll, SWS_H_SelAll),
      End,
      Child, ColGroup(2),
         Child, b_Add      = MkBt(SWS_B_Add,    SWS_H_Add),
         Child, s_Patt = StringObject,
            StringFrame,
            MUIA_CycleChain,          1,
            MUIA_Disabled,            TRUE,
            MUIA_String_AttachedList, idata->SWA_NList_LV,
            MUIA_ShortHelp,           GetS(SWS_H_Pattern),
         End,
         Child, Label(GetS(SWS_GL_Header)),
         Child, s_Hdr = CycleObject,
            MUIA_Cycle_Entries,       Headers,
            MUIA_CycleChain,          1,
            MUIA_Disabled,            TRUE,
            MUIA_ShortHelp,           GetS(SWS_H_Header),
         End,
         Child, Label(GetS(SWS_L_Mode)),
         Child, SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o),
      End,
   End;
   
   if (!idata->UI) SWDB_RET(NULL);

   DoMethod(idata->SWA_NList_L, MUIM_Notify, MUIA_List_Entries, MUIV_EveryTime,
            idata->title, 4, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_L_TopTitle), MUIV_TriggerValue);

   DoMethod(idata->title, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_L_TopTitle), SWGetUL(idata->SWA_NList_L, MUIA_List_Entries));
   
   s_PattAttr = SWA_NStrFilter_Pattern;
   s_HdrAttr  = SWA_NStrFilter_Header;
   s_HdrDef   = SWV_HM_Subject;
   s_HdrGAttr = MUIA_Cycle_Active;
   
   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);

   DoMethod(b_Add,    NOTP, o, 2, SWM_NList_Add, NULL);
   DoMethod(b_SelAll, NOTP, o, 4, SELALLON);
   DoMethod(b_Del,    NOTP, o, 1, SWM_NList_RemoveSel);

   DoMethod(b_Edit,    NOTP, o, 7,
            SWM_Root_ListDoMethod, idata->SWA_NList_L,
            SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 2,
            SWM_NSFilter_ShowUI, SWV_ListDoMethod_App);
            
   DoMethod(b_Add,    NOTP, MUIV_Notify_Window, 3,
            MUIM_Set, MUIA_Window_ActiveObject, s_Patt);

   DoMethod(idata->SWA_NList_LV, DCLK,
            o, 7, SWM_Root_ListDoMethod, idata->SWA_NList_L,
            SWV_ListDoMethod_Active, SWV_ListDoMethod_Entry, 2,
            SWM_NSFilter_ShowUI, SWV_ListDoMethod_App);

   SWDB_RET(idata->UI);
} SDFT


/*****************************************************************************
 * Method: Hide UI for this object                                           *
 *****************************************************************************/

METHOD(SWM_NSFilter_HideUI, Msg)
{
   if (!idata->UI) SWDB_RET(FAILURE);

   SWKillAttrUI(SW_ATTRMAP, idata->UI, o),
   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);
   DoMethod(idata->UI, OM_REMMEMBER, o);
   idata->UI = s_Patt = s_Hdr  = NULL;
   
   DoMethod(idata->SWA_NList_LV,MUIM_KillNotifyObj,MUIA_Listview_DoubleClick,o);

   DoMethod(idata->SWA_NList_L, MUIM_KillNotify, MUIA_List_Entries,
            idata->title);
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: MUI Setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   if (!DoSuperMethodA(cl, o, msg)) SWDB_RET(FALSE);

   SWMakeImg(idata, SWV_Img_True,  CheckMark(TRUE));
   SWMakeImg(idata, SWV_Img_False, CheckMark(FALSE));

   SWCreateImages(idata);
   
   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: MUI Cleanup                                                       *
 *****************************************************************************/
   
METHOD(MUIM_Cleanup, Msg)
{
   SWDeleteImages(idata);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Expire children if possible                                       *
 *****************************************************************************/

METHOD(SWM_NSFilter_Expire, Msg)
{
   SWDB_RET(DoMethod(o, SWM_NSFilter_ExpireList, o));
} SDFT
