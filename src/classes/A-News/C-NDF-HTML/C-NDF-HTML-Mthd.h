
# include "S-SetUIToggle.h"
# include "S-GetEOL.h"
# include "S-FastStrStr.h"

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
 * Function: see if there's a <HTML> on this line                            *
 *****************************************************************************/

static LONG HasHTML(SW_IDATA *idata, STRPTR Buff, LONG bol, LONG eol,
                    LONG Type, LONG HTag, LONG *SPos, LONG *EPos)
{
   LONG spos;
   
   for (; bol < eol-5; bol++) {
      if (Buff[bol] != '<')  continue;

      spos = bol++;

      while (Buff[bol] == ' ' || Buff[bol] == '\t' || Buff[bol] == '\n')
         bol++;

      if (Type == EndTag && Buff[bol++] != '/') continue;

      if (HTag == HTAG_BODY) {
         if (!((Buff[bol+0] == 'B' || Buff[bol+0] == 'b') &&
               (Buff[bol+1] == 'O' || Buff[bol+1] == 'o') &&
               (Buff[bol+2] == 'D' || Buff[bol+2] == 'd') &&
               (Buff[bol+3] == 'Y' || Buff[bol+3] == 'y')))
            continue;
      } else if (HTag == HTAG_HTML) {
         if (!((Buff[bol+0] == 'H' || Buff[bol+0] == 'h') &&
               (Buff[bol+1] == 'T' || Buff[bol+1] == 't') &&
               (Buff[bol+2] == 'M' || Buff[bol+2] == 'm') &&
               (Buff[bol+3] == 'L' || Buff[bol+3] == 'l')))
            continue;
      } else continue;

      bol += 4;

      while (Buff[bol] == ' ' || Buff[bol] == '\t' || Buff[bol] == '\n')
         bol++;

      if (Buff[bol] == '>') {
         if (SPos) *SPos = spos;
         if (EPos) *EPos = bol;
      
         return Type == EndTag ? spos : bol;
      }
   }
   
   return 0;
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  Buff = TxBuff(msg->Tx);
   LONG    bol, eol, BeginPos = 0, EndPos, InHTML = FALSE, BodyBegin, BodyEnd,
           SPos, EPos;
   TEXT    AddObj[AV_AddOSize];
   AddData *AD;

   if (!msg->Tx) SWDB_RET(SWV_NDFilter_Incomplete);

   bol = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   // -- loop through buffer looking for URL pattern -------------------------

   for (; Buff[bol]; bol = eol + 1) {
      eol = GetEOL(Buff, bol);

      if (!InHTML && (BeginPos = HasHTML(idata, Buff, bol, eol, BeginTag,
                                         HTAG_HTML, &SPos, NULL)))
         InHTML = TRUE, BodyBegin = BodyEnd = 0;

      if (InHTML) {
         if (!BodyBegin)
            BodyBegin = HasHTML(idata, Buff, bol, eol, BeginTag,
                                HTAG_BODY, NULL, NULL);
         
         if (BodyBegin && !BodyEnd)
            BodyEnd = HasHTML(idata, Buff, bol, eol, EndTag,
                              HTAG_BODY, NULL, NULL);
      
         if (EndPos = HasHTML(idata, Buff, bol, eol, EndTag,
                              HTAG_HTML, NULL, &EPos)) {
            InHTML = FALSE;

            if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
               SWDB_RET(SWV_NDFilter_Incomplete);

            AD->Grp = msg->Grp;
            AD->Art = msg->Art;

            DoMethod(o, SWM_NDFilter_TBInit, &AD->HTML);
            DoMethod(o, SWM_NDFilter_TBGetSpace, &AD->HTML,
                     EndPos - BeginPos + 20);

            if (!BodyBegin)
               DoMethod(o, SWM_NDFilter_TBAppendTxt, &AD->HTML, "<BODY>");
            
            DoMethod(o, SWM_NDFilter_TBAppendSub, &AD->HTML, Buff,
                     BeginPos+1, EndPos-1);
            
            if (!BodyEnd)
               DoMethod(o, SWM_NDFilter_TBAppendTxt, &AD->HTML, "</BODY>");

            strcpy(TxBuff(msg->Tx) + SPos, 
                   (STRPTR)DoMethod(msg->ArtView, SWM_NArticleView_AddObj,
                                    AddObj, AD, o));

            DoMethod(o, SWM_NDFilter_TBDelSub, msg->Tx,
                     SPos + strlen(AddObj), EPos+1);
            
            Buff = TxBuff(msg->Tx);

            eol = BeginPos;
         }
      }
   }

   SWDB_RET(InHTML ? SWV_NDFilter_Incomplete : SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object  *Obj;
   AddData *AD = (AddData *)msg->Data;

   Obj = MUI_NewObject(MUIC_HTMLview,
                       MUIA_HTMLview_Contents,   TxBuff(&AD->HTML),
                       MUIA_HTMLview_Scrollbars, MUIV_HTMLview_Scrollbars_Auto,
                       TAG_DONE);

   SWDB_RET(Obj);
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemAddObj, SWP_NDFilter_RemAddObj *)
{
   AddData *AD = (AddData *)msg->Data;

   if (!AD) SWDB_RET(0);

   DoMethod(o, SWM_NDFilter_TBDispose, &AD->HTML);

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

//   SWP_NDF_FltToggle(idata, SWA_NDF_URL);

   SWDB_RET(idata->UI);
} SDFT
