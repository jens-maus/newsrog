
# include "S-SetUIToggle.h"
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
 * Function: Format header data line, maybe with newlines                    *
 *****************************************************************************/

static void FmtHdrData(Object *o, SW_IDATA *idata,
                       TxtBuff *Out, STRPTR Hdr, STRPTR Buff)
{
   UBYTE ec = '\0';
   ULONG Pos, LastPos;

   if (idata->SWA_NDF_EmpHeader_FoldLines)
      if (SWP_HasHeaderNoCase(Hdr, SWF_NAHeader_MsgID))         ec=' ';
      else if (SWP_HasHeaderNoCase(Hdr, SWF_NAHeader_Xref))     ec=' ';
      else if (SWP_HasHeaderNoCase(Hdr, SWF_NAHeader_Ref))      ec=' ';
      else if (SWP_HasHeaderNoCase(Hdr, SWF_NAHeader_Groups))   ec=',';
      else if (SWP_HasHeaderNoCase(Hdr, SWF_NAHeader_Followup)) ec=',';
      else if (SWP_HasHeaderNoCase(Hdr, SWF_NAHeader_Path))     ec='!';

   if (ec == '\0') {
      DoMethod(o, SWM_NDFilter_TBAppendTxt, Out, Buff);
      return;
   }

   for (Pos = LastPos = 0; ; Pos++) {
      if (Pos && Buff[Pos] == ec || !Buff[Pos]) {
         DoMethod(o, SWM_NDFilter_TBAppendSub, Out, Buff, LastPos, Pos-1);

         if (Buff[Pos])
            DoMethod(o, SWM_NDFilter_TBAppendTxt, Out, "\n   ");

         LastPos = Pos+1;
      }

      if (!Buff[Pos]) break;
   }
}

   
/*****************************************************************************
 * Function: Append header with styling                                      *
 *****************************************************************************/

static void AppendHdrLine(Object *o, SW_IDATA *idata, TxtBuff *Out,
                          STRPTR Buff, ULONG bol, ULONG eol)
{
   ULONG col;
   UBYTE c;

   for (col=bol; col < eol && Buff[col] != ':'; col++);
   
   if (eol == bol || col == eol) return;

   if (!DoMethod(o, SWM_NDFilter_TBIsEncaps, Buff, bol, eol)) {
      DoMethod(o, SWM_NDFilter_TBAppendSub, Out, Buff, bol, eol);
      return;
   }

   c=Buff[col+1], Buff[col+1] = '\0';
   
   DoMethod(o, SWM_NDFilter_StyleOn, Out, 
            idata->SWA_NDF_EmpHeader_FieldFont,
            idata->SWA_NDF_EmpHeader_FieldB,
            idata->SWA_NDF_EmpHeader_FieldU,
            idata->SWA_NDF_EmpHeader_FieldI,
            idata->SWA_NDF_EmpHeader_FieldUseColor,
            &idata->SWA_NDF_EmpHeader_Pens[EHPEN_Field]);

   DoMethod(o, SWM_NDFilter_TBAppendTxt, Out, Buff+bol);

   DoMethod(o, SWM_NDFilter_StyleOff, Out, 
            idata->SWA_NDF_EmpHeader_FieldFont,
            idata->SWA_NDF_EmpHeader_FieldB,
            idata->SWA_NDF_EmpHeader_FieldU,
            idata->SWA_NDF_EmpHeader_FieldI,
            idata->SWA_NDF_EmpHeader_FieldUseColor,
            &idata->SWA_NDF_EmpHeader_Pens[EHPEN_Field]);

   Buff[col+1] = c;
   c = Buff[eol], Buff[eol] = '\0';

   DoMethod(o, SWM_NDFilter_StyleOn, Out, 
            idata->SWA_NDF_EmpHeader_DataFont,
            idata->SWA_NDF_EmpHeader_DataB,
            idata->SWA_NDF_EmpHeader_DataU,
            idata->SWA_NDF_EmpHeader_DataI,
            idata->SWA_NDF_EmpHeader_DataUseColor,
            &idata->SWA_NDF_EmpHeader_Pens[EHPEN_Data]);

   FmtHdrData(o, idata, Out, Buff+bol, Buff+col+1);

   DoMethod(o, SWM_NDFilter_StyleOff, Out, 
            idata->SWA_NDF_EmpHeader_DataFont,
            idata->SWA_NDF_EmpHeader_DataB,
            idata->SWA_NDF_EmpHeader_DataU,
            idata->SWA_NDF_EmpHeader_DataI,
            idata->SWA_NDF_EmpHeader_DataUseColor,
            &idata->SWA_NDF_EmpHeader_Pens[EHPEN_Data]);

   Buff[eol] = c;

   DoMethod(o, SWM_NDFilter_TBAppendTxt, Out, "\n");
}

   
/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG   bol, eol;

   if (!*msg->HasHeaders) SWDB_RET(SWV_NDFilter_Complete);

   // -- add headers, inserting ctrl codes -----------------------------------

   for (bol=0; Buff[bol] && Buff[bol] != '\n'; bol = eol + 1) {
      eol = GetEOL(Buff, bol);

      AppendHdrLine(o, idata, msg->Scratch, Buff, bol, eol);
   }

   // -- add rest of buffer --------------------------------------------------
   
   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx,
            DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch,
                     Buff, bol, TxLen(msg->Tx)));
   
   SWDB_RET(SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o);

   SWP_NDF_FltToggle(idata, SWA_NDF_EmpHeader_Field);
   SWP_NDF_FltToggle(idata, SWA_NDF_EmpHeader_Data);

   SWDB_RET(idata->UI);
} SDFT
