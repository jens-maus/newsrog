
# include "S-GetEOL.h"

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
 * Function: Is line blank?                                                  *
 *****************************************************************************/

static ULONG IsBlank(SW_IDATA *idata, STRPTR s, ULONG bol, ULONG eol)
{
   ULONG Blank = FALSE;
   UBYTE c;

   c   = s[eol], s[eol] = '\0';

   if (strlen(s + bol) == strspn(s + bol, " \t"))
      Blank = TRUE;
   
   s[eol] = c;

   return Blank;
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG   bol, eol, bst, ben, lbl, LastPos = 0, BlankCount, LastBlank = FALSE;

   DoMethod(o, SWM_NDFilter_TBGetSpace, msg->Scratch, TxSize(msg->Tx));

   // -- skip headers --------------------------------------------------------

   bol = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   // -- look for signature line pattern -------------------------------------

   for (; Buff[bol]; bol = eol + 1) {
      eol = GetEOL(Buff, bol);

      if (!IsBlank(idata, Buff, bol, eol)) {
         LastBlank = FALSE;
         continue;
      }

      LastBlank = TRUE;

      // -- count consecutive blanks -----------------------------------------
      
      for (BlankCount = 0, bst = bol; Buff[bst]; bst = ben + 1) {
         ben = GetEOL(Buff, bst);

         if (!IsBlank(idata, Buff, bst, ben)) {
            LastBlank = FALSE;
            break;
         }
         
         if (BlankCount++ == idata->SWA_NDF_TrimSpace_MaxLines)
            lbl = ben;
      }

      if (BlankCount <= idata->SWA_NDF_TrimSpace_MaxLines)
         continue;

      if (lbl)
         DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff, LastPos, lbl-1);

      LastPos = bst;
      eol     = bst - 1;
   }

   if (!LastPos)
      SWDB_RET(SWV_NDFilter_Complete);

   DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff, LastPos, TxLen(msg->Tx));
   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   SWDB_RET(LastBlank ? SWV_NDFilter_Incomplete : SWV_NDFilter_Complete);
} SDFT
