
# include "S-GetEOL.h"
# include "S-PrevEOL.h"
# include "S-CTSupport.h"

/*****************************************************************************
 * Function: open topaz 8                                                    *
 *****************************************************************************/

static struct TextFont *GetFont(struct TextAttr *fa)
{
   struct TextAttr ta = { "topaz.font", 8, FS_NORMAL, FPF_ROMFONT };
   struct TextFont *tf;

   if (tf = OpenDiskFont(fa))
      return tf;

   return OpenFont(&ta);
}


/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   FData **FD = (FData **)msg->FltData;
   ULONG x, YSize = 8;
   struct RastPort rp;
   struct TextAttr ta;
   struct TextFont *tf = NULL;
   STRPTR sp, VFont = SWGetStr(SWGetObj(msg->Grp, SWA_AppBase_TopObj),
                               SWA_NNews_VFont);
   UBYTE  c;
   ULONG  rc = SUCCESS;
   TEXT   Txt[2], Font[64];

   if (!(*FD = (FData *)NDF_AV(sizeof(FData))))
      SWDB_RET(FAILURE);

   if (SW_STRISNULL(VFont)) {
      rc = FAILURE;
      goto Done;
   }
   
   // -- parse font size, etc ------------------------------------------------

   if (sp = strchr(VFont, '/')) {
      c = sp[0], sp[0] = '\0';
      YSize = atol(sp+1);
      YSize = max(YSize, 5);
   }

   strcat(strcpy(Font, VFont), ".font");
   if (sp) sp[0] = c;

   ta.ta_Name  = Font;
   ta.ta_Flags = 0;
   ta.ta_Style = FS_NORMAL;
   ta.ta_YSize = YSize;
   
   // -- open the font -------------------------------------------------------

   if (!(tf = GetFont(&ta))) {
      rc = FAILURE;
      goto Done;
   }

   // -- get table of character sizes ----------------------------------------
   
   InitRastPort(&rp);
   SetFont(&rp, tf);

   for (x=0; x<CNUM; x++)
      (*FD)->CharWidths[x] = tf->tf_XSize;

   for (x=tf->tf_LoChar, Txt[1] = '\0'; x<=tf->tf_HiChar; x++) {
      Txt[0] = x;
      (*FD)->CharWidths[x] = TextLength(&rp, Txt, 1);
   }

   // -- get maximum wrap width in pixels ------------------------------------

   (*FD)->MaxWidth = SWGetUL(msg->ArtView, MUIA_Width) * 90 / 100;
   (*FD)->MaxWidth = max((*FD)->MaxWidth, SWV_NDF_ScrollHdrs_MinWidth);

  Done:
   if (tf) CloseFont(tf);
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   FData **FD = (FData **)msg->FltData;

   NDF_FV(*FD);
   *FD = NULL;

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Function: copy headers into buffer                                        *
 *****************************************************************************/

static void ReflowRegion(Object *o, SW_IDATA *idata, FData *FD, TxtBuff *Tx,
                         STRPTR Buff, ULONG p1, ULONG p2)
{
   LONG   x, LineW = 0, LastPos = p1, LastBrk = p1;

   for (x=p1; x<=p2; x++) {
      if (Buff[x] == ' ' || Buff[x] == ',' || Buff[x] == '!') LastBrk = x;

      if ((LineW += FD->CharWidths[Buff[x]]) > FD->MaxWidth) {
         DoMethod(o, SWM_NDFilter_TBAppendSub, Tx, Buff, LastPos, LastBrk);
         DoMethod(o, SWM_NDFilter_TBAppendTxt, Tx, "\n  ");
         LastPos = LastBrk+1;
         LineW   = 2*FD->CharWidths[' '];
      }

      if (Buff[x] == '\n') {
         LineW = 0;
      }
   }

   DoMethod(o, SWM_NDFilter_TBAppendSub, Tx, Buff, LastPos, p2);
}

   
/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   FData  *FD = (FData *)msg->FltData;
   STRPTR  Buff = TxBuff(msg->Tx);
   ULONG   bol;
   TEXT    AddObj[AV_AddOSize];
   AddData *AD;

   if (!*msg->HasHeaders) SWDB_RET(SWV_NDFilter_Complete);

   // -- skip headers --------------------------------------------------------

   bol    = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   if (bol < 2) SWDB_RET(SWV_NDFilter_Complete);
   
   // -- substitute clickable object for headers --------------------------

   if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
      SWDB_RET(SWV_NDFilter_Incomplete);

   DoMethod(o, SWM_NDFilter_TBMultiInit, &AD->Headers, NULL);
   ReflowRegion(o, idata, FD, &AD->Headers, Buff, 0, bol-3);
   DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
            DoMethod(msg->ArtView, SWM_NArticleView_AddObj, AddObj, AD, o));

   DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff, bol-2,
            TxLen(msg->Tx));

   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   *msg->HasHeaders = FALSE;

   SWDB_RET(SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   AddData *AD = (AddData *)msg->Data;

   // -- insert scroll object ------------------------------------------------

   SWDB_RET((AV_TextScrollClass,
      SWA_TextScroll_Obj, DoMethod(msg->ArtView, SWM_NArticleView_GenObj,
                                   &AD->Headers, NULL),
      SWA_TextScroll_IsGrp,     TRUE,
      SWA_TextScroll_TxtHeight, idata->SWA_NDF_ScrollHdrs_Lines,
      SWA_TextScroll_InitLine,  0,
   End));
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemAddObj, SWP_NDFilter_RemAddObj *)
{
   AddData *AD = (AddData *)msg->Data;

   if (!AD) SWDB_RET(0);

   DoMethod(o, SWM_NDFilter_TBMultiDispose, &AD->Headers, NULL);

   NDF_FV(AD);
   
   SWDB_RET(0);
} SDFT
