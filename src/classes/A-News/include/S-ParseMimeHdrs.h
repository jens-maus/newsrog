
# ifndef IsWhite
#    define IsWhite(c) ((c)==' '||(c)=='\t')
# endif

/***************************************************************************
 * Function: Copy mime header data into a buffer                           *
 ***************************************************************************/

static STRPTR CopyMimeHdr(STRPTR Data, STRPTR Buff, ULONG BuffSize,
                          LONG *NextPos)
{
   ULONG DPos = 0, BPos = 0, InQuote = FALSE, InComment = 0, BeginSp = TRUE;

   if (!Data || !Data[0] || Data[0] == '\n') {
      *NextPos = -1;
      return NULL;
   }
   
   if (Data[DPos] == '"') DPos++, InQuote = TRUE, BeginSp = FALSE;

   // -- copy header data, ignoring comments ---------------------------------

   while (TRUE) {
      if (!Data[DPos]                     ||
          (Data[DPos] == '"' && InQuote)  ||
          (Data[DPos] == ';' && !InQuote) ||
          (Data[DPos] == '\n' && !IsWhite(Data[DPos+1])))
         break;

      // -- next line has continuation of this line --------------------------
      if (Data[DPos] == '\n' && IsWhite(Data[DPos+1])) {
         BeginSp = TRUE, DPos++;
         continue;
      }

      // -- look for start of comment ----------------------------------------
      if (Data[DPos] == '(' && !InQuote) {
         InComment = TRUE, DPos++;
         continue;
      }

      // -- in comment, ignore comment`s contents ----------------------------
      if (InComment) {
         if (Data[DPos++] == ')') {
            InComment = FALSE;
            continue;
         } else continue;
      }

      // -- ignore white space at beginning of line --------------------------
      if (IsWhite(Data[DPos]) && BeginSp) {
         DPos++;
         continue;
      } else BeginSp = FALSE;

      // -- if we got this far, just copy characters to output buffer --------
      if (Buff && BPos < (BuffSize - 1))
         Buff[BPos++] = Data[DPos];

      DPos++;
   }

   // -- remove trailing spaces if not quoted --------------------------------

   if (!InQuote && Buff)
      while (BPos && IsWhite(Buff[BPos-1])) BPos--;

   // -- return last position parsed -----------------------------------------

   if (InQuote && Data[DPos] == '"') DPos++;

   while (Data[DPos] && IsWhite(Data[DPos])) DPos++;
   if (Data[DPos] == ';') DPos++;

   if (NextPos) *NextPos = DPos;
   
   // -- terminate da buffer SVP ----------------------------------------------

   if (Buff)
      Buff[BPos] = '\0';

   return Buff;
}


/*****************************************************************************
 * Function: Parse mime header                                               *
 *    Data:     the header string                                            *
 *    Header:   header to look for                                           *
 *    Field:    subfield, or NULL for none                                   *
 *    Buff:     where to put the parsed data                                 *
 *    BuffSize: size of Buff in bytes                                        *
 *****************************************************************************/

static STRPTR ParseMimeHdr(STRPTR Data, STRPTR Header, STRPTR Field,
                           STRPTR Buff, ULONG BuffSize)
{
   LONG   NextPos, FieldLen;
   
   if (Header && Strnicmp(Data, Header, strlen(Header)) != 0)
      return NULL;

   // -- find field separator : ----------------------------------------------

   while (*Data && *Data != ':') Data++;

   if (!*Data) return NULL;

   Data++;  // skip ':' char

   // -- if not looking for a particular field, return header value ----------

   if (!Field)
      return CopyMimeHdr(Data, Buff, BuffSize, NULL);

   // -- otherwise, look for given field -------------------------------------

   FieldLen = strlen(Field);

   while (TRUE) {
      CopyMimeHdr(Data, NULL, 0, &NextPos);
      if (NextPos < 0) break;
      
      Data += NextPos;

      while (*Data && IsWhite(*Data)) Data++;

      if (Strnicmp(Data, Field, FieldLen) == 0) {
         Data += FieldLen;
         while (*Data && IsWhite(*Data)) Data++;
         if (*Data != '=') {
            Data -= NextPos;
         } else
            return CopyMimeHdr(Data+1, Buff, BuffSize, NULL);
      }
   }

   return NULL;
}
