
# define FASTSTRSTR_POS
# include "S-GetEOL.h"

/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   FData **FD = (FData **)msg->FltData;
   Object *lo;
   LONG x, Pos = 0, PatCount;

   if (!(*FD = (FData *)NDF_AV(sizeof(FData))))
      SWDB_RET(FAILURE);

   PatCount = SWGetUL(idata->SWA_NList_L, MUIA_List_Entries);

   if (!((*FD)->PatInfo = (struct PatInfo *)
         NDF_AV((PatCount + 1) * sizeof(struct PatInfo))))
      SWDB_RET(FAILURE);

   SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE) {
      STRPTR Pat  = SWGetStr(lo, SWA_NDF_Substitute_Pattern);
      ULONG  Wild = SWGetUL(lo, SWA_NDF_Substitute_Wild),
             Case = SWGetUL(lo, SWA_NDF_Substitute_Case);

      (*FD)->PatInfo[Pos].Longest = SWGetUL(lo, SWA_NDF_Substitute_Longest);
      (*FD)->PatInfo[Pos].Repl    = SWGetStr(lo, SWA_NDF_Substitute_Replace);
      (*FD)->PatInfo[Pos].Img     = SWGetStr(lo, SWA_NDF_Substitute_Image);
      (*FD)->PatInfo[Pos].Mask    = SWGetStr(lo, SWA_NDF_Substitute_Mask);

      if (SW_STRISNULL((*FD)->PatInfo[Pos].Img)  &&
          SW_STRISNULL((*FD)->PatInfo[Pos].Repl))
         continue;

      if (SWPB_Start(&((*FD)->PatInfo[Pos].pb), Pat, Case, Wild, NULL) != SUCCESS)
         continue;

      Pos++;
   }

   (*FD)->PatInfo[Pos].EndPat = TRUE;
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   LONG x;
   FData **FD = (FData **)msg->FltData;

   if (*FD) {
      if ((*FD)->PatInfo)
         for (x=0; !(*FD)->PatInfo[x].EndPat; x++)
            SWPB_End(&((*FD)->PatInfo[x].pb));
      
      NDF_FV((*FD)->PatInfo);
   }

   NDF_FV(*FD);
   *FD = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: see if there's a pattern in this line                           *
 *****************************************************************************/

static LONG HasSubstPatt(SW_IDATA *idata, FData *FD, STRPTR s, ULONG Len,
                         ULONG *StCol, ULONG *EndCol, ULONG *Found)
{
   LONG  x;
   ULONG S, E;

   *StCol = LONG_MAX;

   for (x=0; !FD->PatInfo[x].EndPat; x++) {
      S = 0, E = Len;
      if (SWPB_Find(s, &FD->PatInfo[x].pb, &S, &E, FD->PatInfo[x].Longest))
         if (S < *StCol)
            *StCol  = Len = S, *EndCol = E, *Found = x;
   }
   
   return *StCol != LONG_MAX ? TRUE : FALSE;
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   FData  *FD = (FData *)msg->FltData;
   STRPTR  Buff = TxBuff(msg->Tx), tmp;
   ULONG   StCol, EndCol, Found, bol, eol, FirstPos = 0, LastPos = 0;
   LONG    ImgVarPos;
   UBYTE   c;
   AddData *AD;
   TEXT    AddObj[AV_AddOSize];

   if (!msg->Tx) SWDB_RET(SWV_NDFilter_Incomplete);

   DoMethod(o, SWM_NDFilter_TBGetSpace, msg->Scratch, TxSize(msg->Tx)); 

   // -- loop through and replace stuff --------------------------------------

   bol    = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   for (; Buff[bol]; bol = eol + 1) {
      eol = GetEOL(Buff, bol);
      c   = Buff[eol], Buff[eol] = '\0';

      // -- replace patterns in this line ------------------------------------

      while (HasSubstPatt(idata, FD, Buff+bol, eol - bol,
                          &StCol, &EndCol, &Found)) {
         if ((bol+StCol) > 0)
            DoMethod(o,SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
                     LastPos, bol+StCol-1);

         if (tmp = strstr(FD->PatInfo[Found].Repl, ImgVar))
            ImgVarPos = tmp - FD->PatInfo[Found].Repl;
         else
            ImgVarPos = -1;

         // -- if no image, or no ${Img} var, just add replacement text ------
         
         if (SW_STRISNULL(FD->PatInfo[Found].Img) ||
             !SW_STRISNULL(FD->PatInfo[Found].Repl) &&
             ImgVarPos < 0) {
            DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
                     FD->PatInfo[Found].Repl);
         } else {
            if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
               SWDB_RET(SWV_NDFilter_Incomplete);

            DoMethod(o, SWM_NDFilter_TBMultiInit, &AD->Img, &AD->Mask, NULL);
            DoMethod(o, SWM_NDFilter_TBAppendTxt, &AD->Img,
                     FD->PatInfo[Found].Img);
            DoMethod(o, SWM_NDFilter_TBAppendTxt, &AD->Mask,
                     FD->PatInfo[Found].Mask);
            
            if (ImgVarPos > 0)
               DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch,
                        FD->PatInfo[Found].Repl, 0, ImgVarPos - 1);

            DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
                     (STRPTR)DoMethod(msg->ArtView, SWM_NArticleView_AddObj,
                                      AddObj, AD, o));

            if (ImgVarPos >= 0)
               DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
                        FD->PatInfo[Found].Repl +
                        ImgVarPos + sizeof(ImgVar) - 1);
         }

         bol = LastPos = bol + EndCol + 1;
      }
      
      Buff[eol] = c;
   }


   // -- if nothing done, just return ----------------------------------------
   
   if (LastPos == FirstPos)
      SWDB_RET(SWV_NDFilter_Complete);

   // -- otherwise insert rest of buffer and return --------------------------
   
   DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
            LastPos, TxLen(msg->Tx));
   
   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   SWDB_RET(SWV_NDFilter_Complete);
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
 * Method: Hide UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterHIDEUIF
METHOD(SWM_NDFilter_HideUI, Msg)
{
   if (!idata->UI) SWDB_RET(FAILURE);

// SWKillAttrUI(SW_ATTRMAP, idata->UI, o),
   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);
   DoMethod(idata->UI, OM_REMMEMBER, o);
   idata->UI = s_Patt = s_Repl = s_Img  = s_Mask = NULL;
   
   DoMethod(idata->SWA_NList_L, MUIM_KillNotify, MUIA_List_Entries,
            idata->title);
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT

/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

static Object *StrO(SW_IDATA *idata, ULONG h)
{
   return StringObject,
      StringFrame,
      MUIA_CycleChain,          1,
      MUIA_Disabled,            TRUE,
      MUIA_String_AttachedList, idata->SWA_NList_LV,
      MUIA_ShortHelp,           GetS(h),
   End;
}
   
static Object *PFO(SW_IDATA *idata, ULONG h)
{
   return PopaslObject,
      MUIA_Popstring_String, StringObject,
         StringFrame,
         MUIA_CycleChain,          1,
         MUIA_String_AttachedList, idata->SWA_NList_LV,
      End,
      MUIA_Disabled,            TRUE,
      MUIA_Popasl_Type,         ASL_FileRequest,
      MUIA_Popstring_Button,    PopButton(MUII_PopFile),
      MUIA_ShortHelp,           GetS(h),
   End;
}
   
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
         Child, b_Del     = MkBt(SWS_B_Del,    SWS_H_Del),
         Child, b_SelAll  = MkBt(SWS_B_SelAll, SWS_H_SelAll),
      End,
      Child, ColGroup(2),
         Child, b_Add      = MkBt(SWS_B_Add,    SWS_H_Add),
         Child, s_Patt = StrO(idata, SWS_H_Pattern),
         Child, Label(GetS(SWS_BL_Replace)),
         Child, s_Repl = StrO(idata, SWS_H_Replace),
         Child, Label(GetS(SWS_BL_Img)),
         Child, s_Img = PFO(idata, SWS_H_Image),
         Child, Label(GetS(SWS_BL_Mask)),
         Child, s_Mask = PFO(idata, SWS_H_Mask),
      End,
   End;
   
   if (!idata->UI) SWDB_RET(NULL);

   DoMethod(idata->SWA_NList_L, MUIM_Notify, MUIA_List_Entries, MUIV_EveryTime,
            idata->title, 4, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_L_TopTitle), MUIV_TriggerValue);

   DoMethod(idata->title, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_L_TopTitle), SWGetUL(idata->SWA_NList_L, MUIA_List_Entries));
   
   s_PattAttr = SWA_NDF_Substitute_Pattern;
   s_ReplAttr = SWA_NDF_Substitute_Replace;
   s_ImgAttr  = SWA_NDF_Substitute_Image;
   s_MaskAttr = SWA_NDF_Substitute_Mask;
   
   set(idata->SWA_NList_L, MUIA_List_Active, MUIV_List_Active_Off);

   DoMethod(b_Add,    NOTP, o, 2, SWM_NList_Add, NULL);
   DoMethod(b_SelAll, NOTP, o, 4, SELALLON);
   DoMethod(b_Del,    NOTP, o, 1, SWM_NList_RemoveSel);

   DoMethod(b_Add,    NOTP, MUIV_Notify_Window, 3,
            MUIM_Set, MUIA_Window_ActiveObject, s_Patt);

   SWDB_RET(idata->UI);
} SDFT


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object *Obj;
   AddData *AD  = (AddData *)msg->Data;

   Obj = DTImgObject,
      MUIA_InnerTop,        0,
      MUIA_InnerBottom,     0,
      MUIA_InnerLeft,       0,
      MUIA_InnerRight,      0,
      SWA_DTImg_FSpec,      TxBuff(&AD->Img),
      SWA_DTImg_Mask,       TxBuff(&AD->Mask),
      SWA_DTImg_ImgCache,   NN_DTImages(idata),
   End;

   SWDB_RET(Obj);
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemAddObj, SWP_NDFilter_RemAddObj *)
{
   AddData *AD = (AddData *)msg->Data;

   if (!AD) SWDB_RET(0);

   DoMethod(o, SWM_NDFilter_TBMultiDispose, &AD->Img, &AD->Mask, NULL);

   NDF_FV(AD);
   
   SWDB_RET(0);
} SDFT
