
# include "S-SetUIToggle.h"

/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   idata->Art = msg->Art;
   idata->Grp = msg->Grp;
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   idata->Art =
   idata->Grp = NULL;
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG x, bol, PageCnt, LastPos = 0;
   TEXT    AddObj[AV_AddOSize];
   AddData *AD;

   // -- skip headers --------------------------------------------------------

   bol = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   // -- look for page markers -----------------------------------------------

   for (x=bol, PageCnt = 0; Buff[x]; x++) {
      if (Buff[x] == 0x0c) {
         idata->BrkPos[PageCnt++] = x;
         if (PageCnt >= (SWV_MaxPgBrk-2))
            break;
      }

      // -- stop at .sig marker ----------------------------------------------
      
      if (Buff[x]   == '\n' &&
          Buff[x+1] == '-'  &&
          Buff[x+2] == '-'  &&
          (Buff[x+3] == ' ' && Buff[x+4] == '\n' || Buff[x+3] == '\n')) {
         LastPos = x;
         break;
      }
   }

   idata->BrkPos[PageCnt] = LastPos ? LastPos : x;

   // -- if none found, just return without doing anything -------------------
   
   if (!PageCnt)
      SWDB_RET(SWV_NDFilter_Complete);

   // -- otherwise, allocate data to store other strings ---------------------

   if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
      SWDB_RET(SWV_NDFilter_Incomplete);

   AD->PageCount = PageCnt;

   if (!(AD->Pages = (TxtBuff *)NDF_AV(sizeof(TxtBuff) * PageCnt))) {
      NDF_FV(AD);
      SWDB_RET(SWV_NDFilter_Incomplete);
   }

   for (x=0; x<PageCnt; x++)
      DoMethod(o, SWM_NDFilter_TBInit, &AD->Pages[x]);

   // -- copy ^L-bounded strings to our own area -----------------------------

   for (x=0; x<PageCnt; x++)
      DoMethod(o, SWM_NDFilter_TBAppendSub,
               &AD->Pages[x], Buff, idata->BrkPos[x]+1, idata->BrkPos[x+1]-1);

   // -- save trailing section of buffer -------------------------------------

   DoMethod(o, SWM_NDFilter_TBSet, msg->Scratch, Buff + idata->BrkPos[PageCnt]);

   // -- truncate input buffer at first ^L -----------------------------------
   
   DoMethod(o, SWM_NDFilter_TBTruncate, msg->Tx, idata->BrkPos[0]);

   // -- append clickable objects --------------------------------------------
   
   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Tx,
            DoMethod(msg->ArtView, SWM_NArticleView_AddObj, AddObj, AD, o));

   // -- append last section that we saved before ----------------------------

   DoMethod(o, SWM_NDFilter_TBAppend, msg->Tx, msg->Scratch);

   SWDB_RET(SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object *BGrp, *Obj;
   AddData *AD = (AddData *)msg->Data;
   TxtBuff Tx1, Tx2;
   ULONG x;
   STRPTR t;

   BGrp = VGroup, End;

   if (!BGrp) SWDB_RET(NULL);
   
   DoMethod(o, SWM_NDFilter_TBMultiInit, &Tx1, &Tx2, NULL);

   for (x=0; x<AD->PageCount; x++) {
      idata->TmpAD       = AD;
      idata->CurrentPage = x;
      t = VL_Alloc(o, idata, idata->SWA_NDF_Spoilers_FmtStr);
      idata->TmpAD       = NULL;
      
      Obj = AV_TextHideClass,
               SWA_TextHide_DispObj, FALSE,
               SWA_TextHide_Button,  SimpleButton(t),
               SWA_TextHide_VLayout, TRUE,
               SWA_TextHide_Obj0, RectangleObject,
                  MUIA_FixHeight, 2,
               End,
               SWA_TextHide_Obj1,
                  SWP_NDF_TSObj(msg, o, idata, SWA_NDF_Spoilers,
                                &Tx2, SWA_NDF_Spoilers, EHPEN_SPText,
                                TxBuff(&AD->Pages[x])),
            End;

      VL_Free(o, idata, t);

      DoMethod(BGrp, OM_ADDMEMBER, Obj);
      DoMethod(o, SWM_NDFilter_TBClear, &Tx1);
      DoMethod(o, SWM_NDFilter_TBClear, &Tx2);
   }

   DoMethod(o, SWM_NDFilter_TBMultiDispose, &Tx1, &Tx2, NULL);

   SWDB_RET(BGrp);
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemAddObj, SWP_NDFilter_RemAddObj *)
{
   ULONG x;
   AddData *AD = (AddData *)msg->Data;

   if (!AD) SWDB_RET(0);

   for (x=0; x<AD->PageCount;  x++)
      DoMethod(o, SWM_NDFilter_TBDispose, &AD->Pages[x]);

   NDF_FV(AD->Pages);
   NDF_FV(AD);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   static TEXT VarBuf[16];
   STRPTR s;

   if (!idata->TmpAD) SWDB_RET(NULL);

   VL_FindRetNum("MaxPage", idata->TmpAD->PageCount, VarBuf,  "%ld");
   VL_FindRetNum("Page",    idata->CurrentPage + 1,  VarBuf,  "%ld");
   VL_FindRetNum("Size", TxLen(&idata->TmpAD->Pages[idata->CurrentPage]),
                 VarBuf,  "%ld");

   VL_FindRetNum("Lines",
                 DoMethod(o, SWM_NDFilter_TBCountChar, 
                          &idata->TmpAD->Pages[idata->CurrentPage],
                          (ULONG)'\n') + 1,
                 VarBuf,  "%ld");

   if (s = (STRPTR)DoMethodA(idata->Art, (Msg)msg)) SWDB_RET(s);

   SWDB_RET(DoMethodA(idata->Grp, (Msg)msg));
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o);

   SWP_NDF_FltToggle(idata, SWA_NDF_Spoilers);

   SWDB_RET(idata->UI);
} SDFT

