# include "S-GetEOL.h"
# include "S-CTSupport.h"

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
 * Function: Find end of emphasis region                                     *
 *****************************************************************************/

static LONG FindEmpEnd(SW_IDATA *idata, STRPTR Buff, LONG pos, UBYTE c)
{
   LONG x;
   UBYTE c1;
   
   for (x=2; x < idata->SWA_NDF_TextStyles_MaxLen && Buff[pos+x]; x++)
      if (Buff[pos+x] == c) {
         if (!idata->SWA_NDF_TextStyles_WordBoundary)
            return pos+x;
            
         c1 = Buff[pos+x+1];
         if (c1==' '  || c1=='.'  || c1=='\n' || c1=='\t' ||
             c1==';'  || c1==','  || c1=='!'  || c1=='?'  || c1=='\0')
            return pos+x;
      }

   return -1;
}


/*****************************************************************************
 * Function: See if this is the beginning of en emphasis region              *
 *****************************************************************************/

static __inline LONG IsEmpBegin(SW_IDATA *idata, STRPTR Buff, LONG pos, UBYTE c)
{
   // -- this has to be one of the style characters --------------------------
   
   if (!(c == TSC_B || c == TSC_U || c == TSC_I))
      return FALSE;

   // -- we have to be looking for this style --------------------------------

   if (c == TSC_B && !idata->SWA_NDF_TextStyles_Bold      ||
       c == TSC_U && !idata->SWA_NDF_TextStyles_Underline ||
       c == TSC_I && !idata->SWA_NDF_TextStyles_Italic)
      return FALSE;

   // -- check for word boundaries -------------------------------------------
   
   if (idata->SWA_NDF_TextStyles_WordBoundary &&
       !(pos == 0          ||
         Buff[pos-1]==' '  ||
         Buff[pos-1]=='\t' ||
         Buff[pos-1]=='\n'))
      return FALSE;

   return TRUE;
}


/*****************************************************************************
 * Function: Return string for turning on emphasis                           *
 *****************************************************************************/

static STRPTR EmpOnStr(UBYTE c)
{
   switch (c) {
     case TSC_B: return CTxtB();
     case TSC_I: return CTxtI();
     case TSC_U: return CTxtU();
     default:  return "";
   }
}


/*****************************************************************************
 * Function: Return string for turning off emphasis                          *
 *****************************************************************************/

static STRPTR EmpOffStr(UBYTE c)
{
   switch (c) {
     case TSC_B:
     case TSC_I:
     case TSC_U: return CTxtFont("");
     default:  return "";
   }
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG   pos, LastPos, ModTxt = FALSE;
   LONG    EmpEnd;
   UBYTE   c;

   // -- skip headers --------------------------------------------------------

   pos = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   if (pos > 0)
      DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff, 0, pos-1);

   // -- loop through buffer, looking for emphasis marks ---------------------

   for (LastPos = pos; c=Buff[pos]; pos++) {
      if (!IsEmpBegin(idata, Buff, pos, c))
         continue;

      // -- find the end of this emphasis region -----------------------------

      if ((EmpEnd = FindEmpEnd(idata, Buff, pos, c)) < 0)
         continue;

      ModTxt = TRUE;

      // -- insert everything up to but not including the emp character ------
      
      if (LastPos != pos)
         DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
                  LastPos, pos - 1);

      // -- insert emphasis on string ----------------------------------------

      DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch, EmpOnStr(c));

      // -- insert the text we`re supposed to emphasize ----------------------
      
      DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
                  pos+1, EmpEnd-1);
      
      // -- insert emphasis off string ---------------------------------------

      DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch, EmpOffStr(c));

      pos = LastPos = EmpEnd + 1;
   }

   // -- if nothing done, return original buffer -----------------------------

   if (!ModTxt)
      SWDB_RET(SWV_NDFilter_Complete);

   // -- append rest of buffer -----------------------------------------------
   
   DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff,
            LastPos, TxLen(msg->Tx));
   
   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   SWDB_RET(SWV_NDFilter_Complete);
} SDFT
