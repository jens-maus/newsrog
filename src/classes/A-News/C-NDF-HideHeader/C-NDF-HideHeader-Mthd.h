# include "S-GetEOL.h"
# include "S-CTSupport.h"

/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   Object *Hdr;
   STRPTR Pat;
   LONG x, PLen;
   
   if (idata->Parsed && !idata->UI) SWDB_RET(SUCCESS);

   FreePPats(idata);
   
   SW_MUIListLoop(idata->SWA_NList_L, idata->PatCount, Hdr, TRUE);

   if (!(idata->PPat = (STRPTR *)NDF_AV(idata->PatCount * sizeof(STRPTR))))
      goto Err;

   SW_MUIListLoop(idata->SWA_NList_L, x, Hdr, TRUE) {
      Pat = SWGetStr(Hdr, SWA_NDF_HideHeader_Pat);
      idata->PPat[x] = NULL;
      
      if (!SW_STRISNULL(Pat)) {
         if (!(idata->PPat[x] = NDF_AV(PLen = (strlen(Pat)*2+2))))
            goto Err;
      
         if (ParsePatternNoCase(Pat, idata->PPat[x], PLen) < 0)
            goto Err;
      }
   }

   idata->Parsed = idata->UI ? FALSE : TRUE;
   
   SWDB_RET(SUCCESS);

  Err:
   FreePPats(idata);
   SWDB_RET(FAILURE);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: Append header lines matching pattern                            *
 *****************************************************************************/

static void AppendHdrLines(Object *o, STRPTR Line[], STRPTR ColPos[],
                           UBYTE Used[], STRPTR PPat, TxtBuff *Out)
{
   LONG x, Match;
   UBYTE c;

   for (x=0; Line[x+1]; x++)
      if (!Used[x]) {

         c = ColPos[x][0], ColPos[x][0] = '\0';
         Match = MatchPatternNoCase(PPat, Line[x]);
         ColPos[x][0] = c;
         
         if (Match) {
            Used[x] = TRUE;

            c = Line[x+1][0], Line[x+1][0] = '\0';
            DoMethod(o, SWM_NDFilter_TBAppendTxt, Out, Line[x]);
            Line[x+1][0] = c;
         }
      }
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
#  define MaxLn 50
   STRPTR Buff, Line[MaxLn+2], ColPos[MaxLn+2], Body;
   LONG   x, bol;
   UBYTE  Used[MaxLn];

   if (!*msg->HasHeaders || idata->PatCount == 0)
      SWDB_RET(SWV_NDFilter_Complete);
   
   Buff = TxBuff(msg->Tx);
   DoMethod(o, SWM_NDFilter_TBGetSpace, msg->Scratch, TxSize(msg->Tx));

   memset(Used, 0, MaxLn);
   
   for (x=bol=0; x<MaxLn && Buff[bol] != '\n'; x++) {
      Line[x]   = Buff+bol,
      ColPos[x] = NULL;

      for (bol; Buff[bol]!='\n' && Buff[bol] != ':'; bol++);
      ColPos[x] = Buff+bol+(Buff[bol]==':' ? 1 : 0);

      bol       = GetEOL(Buff, bol) + 1;
   }

   Line[x]   = Buff + bol;
   Line[x+1] = NULL;
   Body      = Buff + bol + 1;

   for (x=0; x<idata->PatCount; x++)
      AppendHdrLines(o, Line, ColPos, Used, idata->PPat[x], msg->Scratch);

   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch, "\n");
   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch, Body);
   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   SWDB_RET(SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   Object *b_Add, *b_Del, *b_SelAll;

   idata->UI = VGroup,
      GroupFrame,
      MUIA_Background,  MUII_GroupBack,
      MUIA_Group_Horiz, FALSE,
      Child, idata->title = SWUI_FrameText(GetS(SWS_L_TopTitle), 0, NULL, NULL),
      Child, o,
      Child, HGroup,
         MUIA_Group_SameSize, TRUE,
         Child, b_Del     = MkBt(SWS_B_Del,    SWS_H_Del),
         Child, b_SelAll  = MkBt(SWS_B_SelAll, SWS_H_SelAll),
      End,
      Child, HGroup,
         Child, b_Add      = MkBt(SWS_B_Add,    SWS_H_Add),
         Child, s_Patt = StringObject,
            StringFrame,
            MUIA_HorizWeight,         10000,
            MUIA_CycleChain,          1,
            MUIA_Disabled,            TRUE,
            MUIA_String_AttachedList, idata->SWA_NList_LV,
            MUIA_ShortHelp,           GetS(SWS_H_Pattern),
         End,
      End,
   End;
   
   if (!idata->UI) SWDB_RET(NULL);

   DoMethod(idata->SWA_NList_L, MUIM_Notify, MUIA_List_Entries, MUIV_EveryTime,
            idata->title, 4, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_L_TopTitle), MUIV_TriggerValue);

   DoMethod(idata->title, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_L_TopTitle), SWGetUL(idata->SWA_NList_L, MUIA_List_Entries));
   
   s_PattAttr = SWA_NDF_HideHeader_Pat;
   
   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);

   DoMethod(b_Add,    NOTP, o, 2, SWM_NList_Add, NULL);
   DoMethod(b_SelAll, NOTP, o, 4, SELALLON);
   DoMethod(b_Del,    NOTP, o, 1, SWM_NList_RemoveSel);

   DoMethod(b_Add,    NOTP, MUIV_Notify_Window, 3,
            MUIM_Set, MUIA_Window_ActiveObject, s_Patt);

   SWDB_RET(idata->UI);
} SDFT


/*****************************************************************************
 * Method: Hide UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterHIDEUIF
METHOD(SWM_NDFilter_HideUI, Msg)
{
   if (!idata->UI) SWDB_RET(FAILURE);

   SWKillAttrUI(SW_ATTRMAP, idata->UI, o);
   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);
   DoMethod(idata->UI, OM_REMMEMBER, o);
   idata->UI = s_Patt = NULL;
   
   DoMethod(idata->SWA_NList_L, MUIM_KillNotify, MUIA_List_Entries,
            idata->title);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


