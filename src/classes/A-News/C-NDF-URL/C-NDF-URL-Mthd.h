
# include "S-SetUIToggle.h"
# include "S-GetEOL.h"
# include "S-FastStrStr.h"

/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   FData **FD = (FData **)msg->FltData;
   TEXT Pat[MaxPL+2];

   if (!(*FD = (FData *)NDF_AV(sizeof(FData))))
      SWDB_RET(FAILURE);

   if (!((*FD)->PPat = NDF_AV(MaxPL * 2 + 2)))
      SWDB_RET(FAILURE);

   strcat(strcat(strcpy(Pat, "*"), idata->SWA_NDF_URL_Pattern), "*");
   
   if ((idata->SWA_NDF_URL_Case ? ParsePattern : ParsePatternNoCase)
       (Pat, (*FD)->PPat, MaxPL*2+2) < 0)
      SWDB_RET(FAILURE);

   if (SWPB_Start(&((*FD)->pb), idata->SWA_NDF_URL_Pattern,
                  FALSE, TRUE, NULL)!=SUCCESS)
      SWDB_RET(FAILURE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   FData **FD = (FData **)msg->FltData;

   if (*FD)
      NDF_FV((*FD)->PPat);

   SWPB_End(&((*FD)->pb));

   NDF_FV(*FD);
   *FD = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: see if there's a pattern in this line                           *
 *****************************************************************************/

static ULONG HasURL(SW_IDATA *idata, FData *FD, STRPTR s, ULONG Len)
{
   UBYTE  c;
   ULONG  Found;
   STRPTR fcc = idata->SWA_NDF_URL_FastCheck, bar;

   // -- check for fastcheck strings -----------------------------------------
   
   for (Found = FALSE; !SW_STRISNULL(fcc) && !Found; fcc = bar?bar+1:NULL) {
      if (bar = strchr(fcc, '|'))
         c = *bar, *bar = '\0';

      if (!SW_STRISNULL(fcc))
         Found = faststrstr(s, fcc, Len, strlen(fcc), FALSE);
      
      if (bar) *bar = c;
   }

   if (!SW_STRISNULL(idata->SWA_NDF_URL_FastCheck) && !Found)
      return FALSE;

   // -- if no FC, or we found a string, check for URL -----------------------

   return (idata->SWA_NDF_URL_Case ? MatchPattern : MatchPatternNoCase)
          (FD->PPat, s);
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   FData  *FD = (FData *)msg->FltData;
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG   StCol, EndCol, bol, eol, eol2, HdrPos, LastPos = 0;
   UBYTE   c, c1;
   TEXT    AddObj[AV_AddOSize];
   AddData *AD;

   if (!msg->Tx) SWDB_RET(SWV_NDFilter_Incomplete);

   DoMethod(o, SWM_NDFilter_TBGetSpace, msg->Scratch, TxSize(msg->Tx)); 
   HdrPos = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   // -- loop through buffer looking for URL pattern -------------------------

   for (bol=0; Buff[bol]; bol = eol + 1) {

      // -- don`t parse crap like X-HHTP: in headers please. -----------------
      if (bol < HdrPos && Buff[bol] != '\n') {
         while (Buff[bol] != ':') bol++;
         bol += 2;
      }
      
      eol2 = eol = GetEOL(Buff, bol);
      c    = Buff[eol2], Buff[eol2] = '\0';

      StCol = EndCol = 0;

      if (HasURL(idata, FD, Buff+bol, eol - bol) &&
          SWPB_Find(Buff+bol, &FD->pb, &StCol, &EndCol, TRUE) &&
          DoMethod(o, SWM_NDFilter_TBNoCtrl, msg->Tx,bol+StCol,bol+EndCol)) {
         
         if ((bol+StCol) > 0)
            DoMethod(o,SWM_NDFilter_TBAppendSub,msg->Scratch, Buff,
                     LastPos,bol+StCol-1);

         if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
            SWDB_RET(SWV_NDFilter_Incomplete);

         if (Buff[bol+StCol-1] == '(' && Buff[bol+EndCol]  == ')')
            EndCol--;

         AD->Grp = msg->Grp;
         AD->Art = msg->Art;
         DoMethod(o, SWM_NDFilter_TBInit, &AD->URL);

         c1 = Buff[bol+EndCol+1], Buff[bol+EndCol+1] = '\0';
         DoMethod(o, SWM_NDFilter_TBAppendTxt, &AD->URL, Buff + bol + StCol);
         DoMethod(o, SWM_NDFilter_TBTruncate,  &AD->URL, MaxURLLen);
         Buff[bol+EndCol+1] = c1;

         DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
                  DoMethod(msg->ArtView, SWM_NArticleView_AddObj, AddObj, AD, o));


         LastPos = (eol = bol + EndCol) + 1;
      }

      Buff[eol2] = c;
   }

   if (LastPos) {
      DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
               LastPos, TxLen(msg->Tx));
   
      DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);
   }

   SWDB_RET(SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Generate Text Object                                              *
 *****************************************************************************/

static Object *TxtObj(Object *o, SW_IDATA *idata, STRPTR URL,
                      TxtBuff *Tx, ULONG UseColor, ULONG Pen,
                      STRPTR DefFont, void *DefColor)
{
   DoMethod(o, SWM_NDFilter_StyleTxt, Tx,
            idata->SWA_NDF_URLFont,
            idata->SWA_NDF_URLB,
            idata->SWA_NDF_URLU,
            idata->SWA_NDF_URLI,
            UseColor, &idata->SWA_NDF_URL_Pens[Pen], URL);

   return CTextObject,
      MUIA_HorizWeight,      1,
      SWA_CText_MemPool,     idata->SWA_NDFilter_MemPool,
      SWA_CText_Contents,    TxBuff(Tx),
      SWA_CText_AutoClip,    NN_AutoClip(idata),
      SWA_CText_DefFont,     DefFont,
      SWA_CText_DefPen,      DefColor,
      SWA_CText_Select,      SWV_CText_Select_NoMulti,
   End;
}


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object *Obj, *Obj0, *Obj1;
   AddData *AD = (AddData *)msg->Data;
   STRPTR URL, DefFont;
   TxtBuff Tx;
   TEXT   URLB[MaxURLLen+80];
   void *DefColor;

   DefFont  = (STRPTR)DoMethod(msg->ArtView, SWM_NArticleView_GetDefFont);
   DefColor = (void *)DoMethod(msg->ArtView, SWM_NArticleView_GetDefColor);

   URL         = TxBuff(&AD->URL);
   URLB[0]     = '\0';
   TxSize(&Tx) = sizeof(URLB);
   TxLen(&Tx)  = 0;
   TxBuff(&Tx) = URLB;

   Obj0 = TxtObj(o, idata, URL, &Tx, idata->SWA_NDF_URLUseColor, PEN_URL,
                 DefFont, DefColor);

   DoMethod(o, SWM_NDFilter_TBClear, &Tx);
   Obj1 = TxtObj(o, idata, URL, &Tx, TRUE, PEN_SelURL, DefFont, DefColor);

   Obj = AV_TextCmdClass,
            AppBaseDefTop(idata),
            SWA_TextHide_UI,        FALSE,
            SWA_TextHide_Obj0,      Obj0,
            SWA_TextHide_Obj1,      Obj1,
            SWA_TextCmd_Art,        AD->Art,
            SWA_TextCmd_Grp,        AD->Grp,
            SWA_TextCmd_Type,       "application/X-URL",
            SWA_TextCmd_Action,     idata->SWA_NDF_URL_Action,
            SWA_TextCmd_Var,        "URL",
            SWA_TextCmd_Val,        URL,
            SWA_TextCmd_NeedDouble, idata->SWA_NDF_URL_DClick,
            SWA_TextCmd_UsePopMenu, idata->SWA_NDF_URL_PopMenus,
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

   DoMethod(o, SWM_NDFilter_TBDispose, &AD->URL);

   NDF_FV(AD);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o);

   SWP_NDF_FltToggle(idata, SWA_NDF_URL);

   SWDB_RET(idata->UI);
} SDFT
