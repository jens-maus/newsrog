
# include "S-SetUIToggle.h"
# include "S-GetEOL.h"
# include "S-PrevEOL.h"
# include "S-CTSupport.h"
# include "S-IsEmptyLine.h"

/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   LONG x;

   memset(idata->BrkChars, 0, sizeof(idata->BrkChars));

   for (x=0; idata->SWA_NDF_HLQuotes_Chars[x]; x++)
      idata->BrkChars[idata->SWA_NDF_HLQuotes_Chars[x]] = 1;

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
 * Function: See if this is truely a break location in the string            *
 *****************************************************************************/


static __inline IsBrk(STRPTR qq, STRPTR s, ULONG x)
{
   if (!qq[s[x]]) return FALSE;

   if (s[x]==':' &&
       ((s[x+1]=='-' && (s[x+2]==')'||s[x+2]=='(')) ||
        s[x+1]==')'))
      return FALSE;

   return TRUE;
}


/*****************************************************************************
 * Function: Find indent level                                               *
 *****************************************************************************/

static LONG IndentLevel(SW_IDATA *idata, STRPTR s, LONG PrevIL,
                        STRPTR PrevLine, STRPTR NextLine)
{
   LONG  l = 0, found, x = 0, y, lpos, fpos, fpost = 0, inbc = TRUE,
         MaxPL = idata->SWA_NDF_HLQuotes_PrefixLen;
   STRPTR qq = idata->BrkChars;

   // -- if line is all whitespace, use last indent level --------------------

   if (IsEmptyLine(s)) return PrevIL;

   // -- find # of quote prefixes on this line -------------------------------
   
   do {
      y = found = 0;
      CT_Loop(s, x, x, s[x] != '\n' && y<=MaxPL && !found, y++)
         if (IsBrk(qq, s, x))
            l++, found=TRUE, lpos = x, fpos = fpost, inbc = TRUE;
         else if (inbc)
            fpost = x, inbc = FALSE;

      // -- ignore <html> tags -----------------------------------------------
      
      if (idata->SWA_NDF_HLQuotes_PrefixHeur && l && 
          s[fpos] == '<' && s[lpos] == '>')
         l--;
      
   } while (found);

   if (!l) return l;
   
   // -- heuristic to nuke some non-indent prefixes --------------------------
   
   if (idata->SWA_NDF_HLQuotes_PrefixHeur && (PrevLine || NextLine)) {
      // -- permit some types of prefixes iff next/prev line has one too -----
      
     if (!NextLine)
        NextLine = s + GetEOL(s, 0) + 1;

      while (lpos >= 0 && s[lpos] != '>') {

         // -- we need the next or previous line to have this prefix too -----

         if ((PrevLine && !strncmp(s, PrevLine, lpos+1)) ||
             (NextLine && !strncmp(s, NextLine, lpos+1))) break;

         // -- but let it pass if it`s between two blank lines or ------------
         // -- the first char on the line ------------------------------------

         if ((PrevLine && NextLine &&
              IsEmptyLine(PrevLine) && IsEmptyLine(NextLine)))
            break;
            
         for (l--, lpos--; lpos>=0 && !IsBrk(qq, s, lpos); lpos--);
      }
   }

   return l;
}


/*****************************************************************************
 * Function: Get quote start position                                        *
 *****************************************************************************/

static ULONG QStartPos(SW_IDATA *idata, STRPTR s, ULONG boq, ULONG eoq,
                       LONG MaxLines, LONG MaxLevel, ULONG *SkipLines)
{
   ULONG nboq = eoq, PrevL;
   STRPTR NextL = NULL;

   MaxLines--;
   *SkipLines = FALSE;
   
   while (MaxLines && nboq > boq) {
      PrevL = PrevBOL(s, nboq);
      
      if (IndentLevel(idata, s + nboq, 0, s + PrevL, NextL) <= MaxLevel)
         MaxLines--;
      else
         *SkipLines = TRUE;

      NextL = s + nboq;
      nboq  = PrevL;
   }

   return (ULONG)max(nboq, boq);
}


/*****************************************************************************
 * Function: Copy last N lines to output buffer, with formatting             *
 *****************************************************************************/

static TxtBuff *CpLines(Object *o, SW_IDATA *idata, TxtBuff *Tx,
                        STRPTR s, ULONG MaxLevel)
{
   LONG    l = 0, prevl = 0;
   STRPTR  spos, PrevLine = NULL;
   TxtBuff Off;
   UBYTE   c;

   DoMethod(o, SWM_NDFilter_TBInit, &Off);

   for (; !SW_STRISNULL(s); s = (spos ? spos + 1 : NULL)) {
      spos     = strchr(s, '\n');
      l        = IndentLevel(idata, s, l, PrevLine, spos ? spos + 1 : NULL);
      l        = min(l, 5);
      PrevLine = s;

      if (l > MaxLevel) continue;

      if (spos) c = spos[1], spos[1] = '\0';

      if (l == prevl) {
         DoMethod(o, SWM_NDFilter_TBAppendTxt, Tx, s);
      } else {
         DoMethod(o, SWM_NDFilter_TBAppend, Tx, &Off);
         DoMethod(o, SWM_NDFilter_TBClear, &Off);

         switch (prevl = l) {
           case 0: DoMethod(o, SWM_NDFilter_TBAppendTxt, Tx, s);
                   DoMethod(o, SWM_NDFilter_TBClear, &Off);
                   break;

           ILCase(1, SWA_NDF_HLQuotes_1, PEN_SQ1);
           ILCase(2, SWA_NDF_HLQuotes_2, PEN_SQ2);
           ILCase(3, SWA_NDF_HLQuotes_3, PEN_SQ3);
           ILCase(4, SWA_NDF_HLQuotes_4, PEN_SQ4);
           default:
           ILCase(5, SWA_NDF_HLQuotes_5, PEN_SQ5);
         }
      }

      if (spos) spos[1] = c;
   }

   DoMethod(o, SWM_NDFilter_TBAppend, Tx, &Off);
   DoMethod(o, SWM_NDFilter_TBClear, &Off);

   return Tx;
}


/*****************************************************************************
 * Function: find end of quoted region                                       *
 *****************************************************************************/

static ULONG FindEOQ(SW_IDATA *idata, STRPTR Buff, ULONG eoq, ULONG l)
{
   STRPTR LPos = NULL;
   ULONG  PrevPos;
   
   // -- find end of quoted region -------------------------------------------
      
   for (; Buff[eoq] && (l=IndentLevel(idata, Buff+eoq, l, LPos, NULL)); )
      LPos  = Buff + eoq,
      eoq   = GetEOL(Buff, eoq) + 1;

   // -- traverse backwards to last quoted line ------------------------------

   LPos = NULL;
   while (eoq > 2) {
      PrevPos = PrevBOL(Buff, eoq);
      
      if (IndentLevel(idata, Buff+eoq, 0, Buff + PrevPos, LPos))
         break;

      LPos = Buff + eoq;
      eoq  = PrevPos;
   }

   return eoq;
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  Buff = TxBuff(msg->Tx), PrevL = NULL;
   ULONG   l, bol, eol, eoq, boq, LastPos, LastQ, SkipLines, ModTxt = FALSE;
   UBYTE   c;
   TEXT    AddObj[AV_AddOSize];
   AddData *AD;

   // -- skip headers --------------------------------------------------------

   bol    = DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);
   
   // -- loop through buffer, checking indent level of lines -----------------
   
   for (LastPos=0; Buff[bol]; PrevL = Buff + bol, bol = eol + 1) {
      eol = GetEOL(Buff, bol);
      
      if (!(l = IndentLevel(idata, Buff+bol, 0, PrevL, Buff + eol + 1))) {
         LastQ = FALSE;
         continue;
      }

      LastQ = ModTxt = TRUE;
      eoq   = FindEOQ(idata, Buff, bol, l);

      // -- copy unquoted text into buffer -----------------------------------
      
      if (bol > 0)
         DoMethod(o, SWM_NDFilter_TBAppendSub,msg->Scratch,Buff,LastPos,bol-1);
      LastPos = GetEOL(Buff, eoq) + 1;
      eol     = LastPos - 1;

      // -- Get start positions ----------------------------------------------

      if (idata->SWA_NDF_HLQuotes_MaxLines)
         boq = QStartPos(idata, Buff, bol, eoq,
                         idata->SWA_NDF_HLQuotes_MaxLines,
                         idata->SWA_NDF_HLQuotes_MaxLevel,
                         &SkipLines);
      else
         boq = LastPos-1;

      // -- see if we can modify this region ---------------------------------

      if (!DoMethod(o, SWM_NDFilter_TBIsEncaps, Buff, bol, LastPos-1) ||
          !DoMethod(o, SWM_NDFilter_TBIsEncaps, Buff, boq, LastPos-1)) {
         if (bol > 0)
            DoMethod(o, SWM_NDFilter_TBAppendSub,msg->Scratch,Buff,LastPos,bol-1);
         LastPos = bol;

         continue;
      }

      if (boq == bol) {
         c = Buff[LastPos-1], Buff[LastPos-1] = '\0';
         CpLines(o, idata, msg->Scratch, Buff + bol,
                 SkipLines ? idata->SWA_NDF_HLQuotes_MaxLevel : LONG_MAX);
         Buff[LastPos-1] = c;
         DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch, "\n");

         continue;
      }

      // -- substitute clickable object --------------------------------------

      if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
         SWDB_RET(SWV_NDFilter_Incomplete);

      DoMethod(o, SWM_NDFilter_TBMultiInit, &AD->Full, &AD->Part, NULL);

      AD->Mode = idata->SWA_NDF_HLQuotes_UIMode;

      c = Buff[LastPos-1], Buff[LastPos-1] = '\0';
      if (boq != (LastPos - 1))
         CpLines(o, idata, &AD->Part, Buff + boq, idata->SWA_NDF_HLQuotes_MaxLevel);
      else
         DoMethod(o, SWM_NDFilter_TBSet, &AD->Part, "");
      
      CpLines(o, idata, &AD->Full, Buff + bol,
              AD->Mode == SWV_NDF_HLQuotes_UIM_Toggle ? LONG_MAX :
              idata->SWA_NDF_HLQuotes_MaxLevel);
      Buff[LastPos-1] = c;

      if (AD->Mode == SWV_NDF_HLQuotes_UIM_Toggle ||
          AD->Mode == SWV_NDF_HLQuotes_UIM_Scroll) {
         DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
                  DoMethod(msg->ArtView, SWM_NArticleView_AddObj, AddObj, AD, o));
      } else {
         DoMethod(o, SWM_NDFilter_TBAppend, msg->Scratch, &AD->Part);
         DoMethod(o, SWM_NDFilter_RemAddObj, AD, NULL);
      }

      DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch, "\n");
   }

   // -- if nothing done, return original buffer -----------------------------

   if (!ModTxt)
      SWDB_RET(SWV_NDFilter_Complete);

   // -- else, put back end of buffer ----------------------------------------

   DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff, LastPos, TxLen(msg->Tx));
   DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);

   SWDB_RET(LastQ ? SWV_NDFilter_Incomplete : SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object  *Obj = NULL;
   AddData *AD = (AddData *)msg->Data;

   // -- insert toggle object ------------------------------------------------
   
   if (AD->Mode == SWV_NDF_HLQuotes_UIM_Toggle)
      Obj = AV_TextHideClass,
         SWA_TextHide_Obj0,
            DoMethod(msg->ArtView, SWM_NArticleView_GenObj, &AD->Part, NULL),
         SWA_TextHide_Obj1,
            DoMethod(msg->ArtView, SWM_NArticleView_GenObj, &AD->Full, NULL),
      End;

   // -- insert scroll object ------------------------------------------------

   if (AD->Mode == SWV_NDF_HLQuotes_UIM_Scroll)
      Obj = AV_TextScrollClass,
         SWA_TextScroll_Obj, DoMethod(msg->ArtView, SWM_NArticleView_GenObj,
                                      &AD->Full, NULL),
         SWA_TextScroll_IsGrp,     TRUE,
         SWA_TextScroll_TxtHeight, max(idata->SWA_NDF_HLQuotes_MaxLines, 1),
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

   DoMethod(o, SWM_NDFilter_TBMultiDispose, &AD->Full, &AD->Part, NULL);

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

   SWP_NDF_FltToggle(idata, SWA_NDF_HLQuotes_1);
   SWP_NDF_FltToggle(idata, SWA_NDF_HLQuotes_2);
   SWP_NDF_FltToggle(idata, SWA_NDF_HLQuotes_3);
   SWP_NDF_FltToggle(idata, SWA_NDF_HLQuotes_4);
   SWP_NDF_FltToggle(idata, SWA_NDF_HLQuotes_5);

   SWDB_RET(idata->UI);
} SDFT
