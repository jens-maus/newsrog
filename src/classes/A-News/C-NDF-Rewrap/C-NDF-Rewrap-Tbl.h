
/*****************************************************************************
 * Function: find column space extent, return # of space chars between data  *
 *****************************************************************************/

static ULONG FindColSpExtent(STRPTR L, ULONG Col, ULONG *s, ULONG *e)
{
   ULONG sc = Col, ec = Col;

   if (IsColSpChr(L[sc]))
      for (; sc && IsColSpChr(L[sc-1]); sc--);
   
   for (; IsColSpChr(L[ec]); ec++);

   if (s) *s = sc;
   if (e) *e = ec;

   return ec - sc;
}


/*****************************************************************************
 * Function: Find maximum extent of inter-column space                       *
 *****************************************************************************/

static ULONG ColExtents(STRPTR L, ULONG LLen,
                        ULONG *maxs, ULONG *maxe, ULONG *mins, ULONG *mine)
{
   ULONG x, sc, ec;
   
   if (!L[0] || *maxe >= LLen) return 0;

   for (x=*maxs; x<*maxe; x++)
      if (IsColSpChr(L[x]))
         break;

   if (x >= *maxe) return 0;  // no column space found
   
   FindColSpExtent(L, x, &sc, &ec);

   for (x=ec; x<*maxe; x++)
      if (IsColSpChr(L[x]))
         return 0;

   if (max(*mins, sc) == min(*mine, ec))  // oops - column went away.
      return 0;
   
   if (maxs) *maxs = min(*maxs, sc);
   if (maxe) *maxe = max(*maxe, ec);
   if (mins) *mins = max(*mins, sc);
   if (mine) *mine = min(*mine, ec);

   return ec - sc;
}


/*****************************************************************************
 * Function: See if a table can come after this line                         *
 *****************************************************************************/

static LONG TblIsIntroLine(STRPTR *s)
{
   return (s[1]-s[0]) < 10;
}


/*****************************************************************************
 * Function: Compute % data (non-sp) chars in the column                     *
 *****************************************************************************/

static ULONG ColDataPct(STRPTR Line, ULONG ColStart, ULONG ColEnd,
                        ULONG *DataChr, ULONG *BrkChr)
{
   ULONG x, DC = 0, BC = 0;

   // -- find first data (non-column-separator) char in the line -------------
   
   for (; Line[ColStart] && Line[ColStart] != '\n' && IsColSpChr(Line[ColStart]);
        ColStart++);

   for (; ColEnd > ColStart && IsColSpChr(Line[ColEnd]); ColEnd--);

   // -- count data & break chars --------------------------------------------
   
   for (x=ColStart; x <= ColEnd; x++)
      if (IsColSpChr(Line[x])) BC++;
      else DC++;

   if (DataChr) (*DataChr) += DC;
   if (BrkChr)  (*BrkChr)  += BC;

   return DC;
}


/*****************************************************************************
 * Function: See if there's a blank line N lines before this                 *
 *****************************************************************************/

static LONG HasPrevBlank(STRPTR *Lines, ULONG StartLine, ULONG Count)
{
   ULONG x;

   Count++;
   
   if (StartLine <= Count) return TRUE;

   for (x=1; x<=Count; x++)
      if (TblIsIntroLine(Lines + StartLine - x))
         return TRUE;

   return FALSE;      
}


/*****************************************************************************
 * Function: Check first few lines                                           *
 *****************************************************************************/

static LONG CheckIntro(STRPTR *Lines, ULONG QPLen)
{
   if (!Lines[0][0] || !Lines[1][0] || !Lines[2][0]) return FALSE;

   // -- need same quote prefix on all lines ---------------------------------

   if (strncmp(Lines[0], Lines[1], QPLen) ||
       strncmp(Lines[0], Lines[2], QPLen)) return FALSE;

   return TRUE;
}



/*****************************************************************************
 * Function: See if this line is the beginning of a table                    *
 *****************************************************************************/

static LONG IsTblBegin(SW_IDATA *idata,
                       STRPTR *Lines, ULONG StartLine,
                       ULONG PrevTblEnd, LONG *Cols, ULONG QPLen)
{
   UBYTE c;
   ULONG x, l, t0, t1, Columns, MaxS, MaxE, MinS, MinE, CSpace,
         ColMaxWidth, NextPos, ColDepth, ColStartPos,
         ColProb, DataChr, BrkChr, DCTmp, BCTmp, MinRows, MinColumns,
         MinTblProb;

   if (!idata->SWA_NDF_Rewrap_Tbl_Use)
      return FALSE;
   
   // -- need a blank line before the table ----------------------------------

   if (!(StartLine == PrevTblEnd+1 ||
         HasPrevBlank(Lines, StartLine, idata->SWA_NDF_Rewrap_Tbl_MaxILines)))
      return FALSE;

   // -- need a few lines left -----------------------------------------------
   
   Lines += StartLine;

   if (!CheckIntro(Lines, QPLen)) return FALSE;
   
   Columns = ColMaxWidth = ColDepth = CSpace = ColStartPos = ColProb =
      DataChr = BrkChr = 0;

   MinRows    = max(idata->SWA_NDF_Rewrap_Tbl_MinRows,     2);
   MinColumns = max(idata->SWA_NDF_Rewrap_Tbl_MinCols,     2);
   MinTblProb = max(idata->SWA_NDF_Rewrap_Tbl_MinTblProb, 10);
   
   if (Cols) Cols[0] = -1;
   
   // -- find table data -----------------------------------------------------
   
   for (x=QPLen; (c=Lines[0][x]) &&
        c != '\n' && Lines[1][x] != '\n' && Lines[2][x] != '\n'; x++)
      if (IsColSpChr(c) && IsColSpChr(Lines[1][x]) && IsColSpChr(Lines[2][x])) {
         if (t1 = FindColSpExtent(Lines[0], x, &MaxS, &MaxE)) {
            NextPos = MaxE;
            MinS    = MaxS;
            MinE    = MaxE;
            DCTmp   = 0;
            BCTmp   = 0;
            
            for (l=1; l<ColSrchDepth; l++, t1 += t0) {
               if (!(t0 = ColExtents(Lines[l], Lines[l+1]-Lines[l],
                                     &MaxS, &MaxE, &MinS, &MinE)))
                  break;

               if (ColDataPct(Lines[l], ColStartPos, MinS, &DCTmp, &BCTmp) == 0)
                  break;

               if (MaxE > NextPos + 5)  // only small hanging indents.
                  break;
            }

            if (l >= MinRows) {
               ColDepth += l;
               CSpace += t1;
               Columns = Columns ? Columns+1 : 2;
               if (Cols && (Columns-1 < MaxColArray))
                  Cols[Columns-2] = MinE-1,
                  Cols[Columns-1] = -1;
               ColMaxWidth += MaxE - MaxS;
               DataChr     += DCTmp;
               BrkChr      += BCTmp;
               ColStartPos = NextPos;
            }
            
            x = NextPos - 1;
         }
      }

   if (Columns >= MinColumns) {
      ULONG DataPct    = DataChr * 100 / (DataChr + BrkChr),
            DepthPct   = ColDepth / (Columns-1) * (100 / ColSrchDepth),
            ColumnsPct = min((Columns-1), 6) * 16 + 4,
            CSpacePct  = min(CSpace * 20 / ColDepth, 100),
            ColWidPct  = min(ColMaxWidth * 20 / (Columns-1), 100);

      DataPct = (max(DataPct, 75) - 75) * 4;
      
      ColProb =
         (DepthPct    * 30 +
          ColumnsPct  * 30 +
          CSpacePct   * 10 +
          DataPct     * 25 +
          ColWidPct   *  5) / 100;
   }
   
   return ColProb > MinTblProb;
}


/*****************************************************************************
 * Function: Find the end of a table that begins on this line                *
 *****************************************************************************/

static ULONG FindTblEnd(STRPTR *Lines, ULONG StartLine, LONG *Cols)
{
   ULONG x, y, NumCols, Found;

   if (Cols[0] < 0) return StartLine;
   
   for (NumCols=0; Cols[NumCols] >= 0; NumCols++);
   
   for (x=StartLine; Lines[x][0]; x++) {
      if (IsEmptyLine(Lines[x])) break;
      
      for (y=Found=0; Cols[y] >= 0 && Cols[y] < LLen(Lines, x); y++)
         if (IsColSpChr(Lines[x][Cols[y]]))
            Found++;

      if (Found < (NumCols/2+1))
         break;
   }
   
   return x - 1;
}
