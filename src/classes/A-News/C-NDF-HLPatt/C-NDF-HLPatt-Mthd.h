
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

   strcat(strcat(strcpy(Pat, "*"), idata->SWA_NDF_HLPatt_Pattern), "*");
   
   if ((idata->SWA_NDF_HLPatt_Case ? ParsePattern : ParsePatternNoCase)
       (Pat, (*FD)->PPat, MaxPL*2+2) < 0)
      SWDB_RET(FAILURE);

   if (SWPB_Start(&((*FD)->pb), idata->SWA_NDF_HLPatt_Pattern,
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

   if (*FD) {
      NDF_FV((*FD)->PPat);
      SWPB_End(&((*FD)->pb));
   }

   NDF_FV(*FD);
   *FD = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: see if there's a pattern in this line                           *
 *****************************************************************************/

static ULONG HasHLPatt(SW_IDATA *idata, FData *FD, STRPTR s, ULONG Len)
{
   UBYTE  c;
   ULONG  Found;
   STRPTR fcc = idata->SWA_NDF_HLPatt_FastCheck, bar;

   // -- check for fastcheck strings -----------------------------------------
   
   for (Found = FALSE; !SW_STRISNULL(fcc) && !Found; fcc = bar?bar+1:NULL) {
      if (bar = strchr(fcc, '|'))
         c = *bar, *bar = '\0';

      if (!SW_STRISNULL(fcc))
         Found = faststrstr(s, fcc, Len, strlen(fcc), FALSE);
      
      if (bar) *bar = c;
   }

   if (!SW_STRISNULL(idata->SWA_NDF_HLPatt_FastCheck) && !Found)
      return FALSE;

   // -- if no FC, or we found a string, check for HLPatt -----------------------

   return (idata->SWA_NDF_HLPatt_Case ? MatchPattern : MatchPatternNoCase)
          (FD->PPat, s);
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   FData  *FD = (FData *)msg->FltData;
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG   StCol, EndCol, bol, eol, eol2, FirstPos = 0, LastPos = 0;
   UBYTE   c;

   if (!msg->Tx) SWDB_RET(SWV_NDFilter_Incomplete);

   DoMethod(o, SWM_NDFilter_TBGetSpace, msg->Scratch, TxSize(msg->Tx)); 

   // -- loop through buffer looking for HLPatt pattern -------------------------

   bol    = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   for (; Buff[bol]; bol = eol + 1) {
      eol2 = eol = GetEOL(Buff, bol);
      c    = Buff[eol2], Buff[eol2] = '\0';

      StCol = EndCol = 0;

      if (HasHLPatt(idata, FD, Buff+bol, eol - bol) &&
          SWPB_Find(Buff+bol, &FD->pb, &StCol, &EndCol,
                    idata->SWA_NDF_HLPatt_MatchMode == SWV_NDF_HLPatt_MatchLontest) &&
          DoMethod(o, SWM_NDFilter_TBNoCtrl, msg->Tx,bol+StCol,bol+EndCol)) {
         
         if ((bol+StCol) > 0)
            DoMethod(o,SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
                     LastPos,bol+StCol-1);

         SWP_NDF_TStyleOn(o, idata, SWA_NDF_HLPatt, msg->Scratch,
                          SWA_NDF_HLPatt_Pens, PEN_HLPatt);

         DoMethod(o,SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
                  bol+StCol, bol+EndCol);

         SWP_NDF_TStyleOff(o, idata, SWA_NDF_HLPatt, msg->Scratch,
                           SWA_NDF_HLPatt_Pens, PEN_HLPatt);
         
         LastPos = (eol = bol + EndCol) + 1;
      }

      Buff[eol2] = c;
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
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o);

   SWP_NDF_FltToggle(idata, SWA_NDF_HLPatt);

   SWDB_RET(idata->UI);
} SDFT
