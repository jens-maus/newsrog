
# include "S-SetUIToggle.h"
# include "S-GetEOL.h"
# include "S-CTSupport.h"

/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   FData **FD = (FData **)msg->FltData;

   if (!(*FD = (FData *)NDF_AV(sizeof(FData))))
      SWDB_RET(FAILURE);

   if (!((*FD)->PPat = NDF_AV(MaxPL * 2 + 2)))
      SWDB_RET(FAILURE);

   if (ParsePattern(idata->SWA_NDF_HideSig_Separator,(*FD)->PPat,MaxPL*2+2)<0)
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

   NDF_FV(*FD);
   *FD = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   FData  *FD = (FData *)msg->FltData;
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG x, bol, eol, foundpos = 0, SigLines;
   UBYTE   c;
   TEXT    AddObj[AV_AddOSize];
   AddData *AD;

   // -- skip headers --------------------------------------------------------

   bol = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   // -- look for signature line pattern -------------------------------------

   for (; !foundpos && Buff[bol]; bol = eol + 1) {
      eol = GetEOL(Buff, bol);
      c   = Buff[eol], Buff[eol] = '\0';

      // -- this looks like a signature line - is it near the EOB? -----------
      
      if ((eol-CT_GetNextChr(Buff,bol)) <= idata->SWA_NDF_HideSig_MaxLineLen &&
          MatchPattern(FD->PPat, CT_GetNextPtr(Buff, bol))) {
         foundpos = bol;

         if (c)
            for (SigLines=0, x=eol+1; Buff[x] &&
                 SigLines <= idata->SWA_NDF_HideSig_MaxSigLines; x++)
               if (Buff[x] == '\n')
                  SigLines++;

         // -- if the sig block would be too big, keep looking ---------------

         if (SigLines > idata->SWA_NDF_HideSig_MaxSigLines || !msg->EndOfData)
            foundpos = 0;
      }
      
      
      Buff[eol] = c;
   }

   // -- none found, or not able to modify? just return ----------------------

   if (!foundpos ||
       !DoMethod(o, SWM_NDFilter_TBIsEncaps, Buff, foundpos, TxLen(msg->Tx)))
      SWDB_RET(SWV_NDFilter_Complete);

   // -- text only mode?  no replacement marker? just nuke text --------------
      
   if (idata->TextOnly || SW_STRISNULL(idata->SWA_NDF_HideSig_Marker)) {
      DoMethod(o, SWM_NDFilter_TBTruncate, msg->Tx, foundpos);
      SWDB_RET(SWV_NDFilter_Complete);
   }


   // -- otherwise, add toggle object ----------------------------------------

   if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
      SWDB_RET(SWV_NDFilter_Incomplete);

   DoMethod(o, SWM_NDFilter_TBInit,      &AD->Sig);
   DoMethod(o, SWM_NDFilter_TBAppendTxt, &AD->Sig, Buff + foundpos);

   DoMethod(o, SWM_NDFilter_TBTruncate,  msg->Tx, foundpos);

   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx,
            DoMethod(msg->ArtView, SWM_NArticleView_AddObj, AddObj, AD, o));

   SWDB_RET(SWV_NDFilter_Incomplete);
} SDFT


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object *Obj;
   AddData *AD = (AddData *)msg->Data;
   TxtBuff Tx1, Tx2;

   DoMethod(o, SWM_NDFilter_TBMultiInit, &Tx1, &Tx2, NULL);
   
   Obj = AV_TextHideClass,
            SWA_TextHide_DispObj, idata->SWA_NDF_HideSig_StartMode ? 1 : 0,
            SWA_TextHide_Obj0,
               SWP_NDF_TSObj(msg, o, idata, SWA_NDF_HideSig_Hidden,
                             &Tx1, SWA_NDF_HideSig_Pens, EHPEN_Hidden,
                             idata->SWA_NDF_HideSig_Marker),
            SWA_TextHide_Obj1,
               SWP_NDF_TSObj(msg, o, idata, SWA_NDF_HideSig_Shown,
                             &Tx2, SWA_NDF_HideSig_Pens, EHPEN_Shown,
                             TxBuff(&AD->Sig)),
         End;

   DoMethod(o, SWM_NDFilter_TBMultiDispose, &Tx1, &Tx2, NULL);

   SWDB_RET(Obj);
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemAddObj, SWP_NDFilter_RemAddObj *)
{
   AddData *AD = (AddData *)msg->Data;

   if (!AD) SWDB_RET(0);

   DoMethod(o, SWM_NDFilter_TBDispose, &AD->Sig);

   NDF_FV(AD);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Modify buffer, no objects (text only)                             *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxtNoObjs, ULONG *)
{
   ULONG rc;

   // -- fake call to SWM_NDFilter_ModTxt, and set TextOnly flag -------------
   
   idata->TextOnly = TRUE;
   msg[0]          = SWM_NDFilter_ModTxt;
   
   rc = DoMethodA(o, (Msg)msg);

   msg[0]          = SWM_NDFilter_ModTxtNoObjs;
   idata->TextOnly = FALSE;
   
   SWDB_RET(rc);
} SDFT



/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o);

   SWP_NDF_FltToggle(idata, SWA_NDF_HideSig_Shown);
   SWP_NDF_FltToggle(idata, SWA_NDF_HideSig_Hidden);

   SWDB_RET(idata->UI);
} SDFT
