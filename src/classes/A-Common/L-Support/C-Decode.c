#  include "C-Support-I.h"
#  include "S-GetEOL.h"

#  define IsOct(d)   ((d)>='0' && (d)<='7')
#  define IsPMode(s) (IsOct((s)[0]) && IsOct((s)[1]) && IsOct((s)[2]))
#  define DEC(c)     (((c) - 0x20) & 0x3F)
#  define SUMSIZE    64
#  define avtod(x)   (LONG)(((x)<='9')?(x)-'0':((x)>='a'?((x)-'a'):((x)-'A'))+10)
#  define IsWhite(c) ((c)==' '||(c)=='\t')

/*****************************************************************************
 * Function: check for begin line                                            *
 *****************************************************************************/

__saveds __asm
LONG SWUUIsBegin(a0 STRPTR s)
{
   // -- check for: begin mmm filename ---------------------------------------
   //               0123456789012

   return strncmp(s, "begin ", 6) == 0 &&
          IsPMode(s + 6)               &&
          strchr(s + 6, ' ')           &&
          s[10] != '\0';
}


/*****************************************************************************
 * Function: return UU filename                                              *
 *****************************************************************************/

__saveds __asm
STRPTR SWUUFileName(a0 STRPTR s)
{
   if (!SWUUIsBegin(s)) return NULL;

   return s+10 + strspn(s+10, " ");
}


/*****************************************************************************
 * Function: check for end line                                              *
 *****************************************************************************/

__saveds __asm
LONG SWUUIsEnd(a0 STRPTR s)
{
   return !strncmp(s, "end", 3);
}


/*****************************************************************************
 * Function: Get size                                                        *
 *****************************************************************************/

__saveds __asm
LONG SWUUSize(a0 STRPTR s)
{
   if (strncmp(s, "size ", 5)) return 0;

   return atol(s + 5);
}


/*****************************************************************************
 * Function: uudecode a single line to the output buffer                     *
 *****************************************************************************/

__saveds __asm
ULONG SWUUDecodeLine(a0 STRPTR bp, a1 STRPTR Data,
                     d0 ULONG OutPos, a2 ULONG *Err)
{
   LONG    n = DEC(bp[0]), j;
   LONG    CheckSum = 0, HasSum;

   bp++;

   // -- uudecode bulk of line -----------------------------------------
   while (n >= 4) {
      if (!bp[0] || !bp[1] || !bp[2] || !bp[3]) {
         if (Err) *Err = SWV_UUDecode_Err_BadLL;
         break;
      }

      Data[OutPos++] = j = (DEC(bp[0])<<2 | DEC(bp[1])>>4), CheckSum+=j;
      Data[OutPos++] = j = (DEC(bp[1])<<4 | DEC(bp[2])>>2), CheckSum+=j;
      Data[OutPos++] = j = (DEC(bp[2])<<6 | DEC(bp[3])),    CheckSum+=j;
      CheckSum = CheckSum % SUMSIZE;

      bp += 4;
      n -= 3;
   }

   // -- remainder of line ---------------------------------------------
   j = (DEC(bp[0])<<2 | DEC(bp[1])>>4), CheckSum += j;
   if (n >= 1) Data[OutPos++] = j;
      
   j = (DEC(bp[1])<<4 | DEC(bp[2])>>2), CheckSum += j;
   if (n >= 2) Data[OutPos++] = j;

   j = (DEC(bp[2])<<6 | DEC(bp[3])),    CheckSum += j;
   if (n >= 3) Data[OutPos++] = j;
   CheckSum = CheckSum % SUMSIZE;

   bp += 4;

   if (HasSum = *bp >= 0x20 && *bp <= 0x60)
      if (CheckSum != DEC(*bp))
         if (Err) *Err = SWV_UUDecode_Err_BadCS;
   
   return OutPos;
}


/*****************************************************************************
 * Function: uudecode the actual data and return a binary                    *
 *****************************************************************************/

__saveds __asm
UBYTE *SWUUDecode(a0 STRPTR Buff, d0 ULONG BuffLen,
                  a1 ULONG *Size, a2 ULONG *Lines, a3 ULONG *Err,
                  a5 STRPTR Out)
{
   UBYTE  *Data;
   UBYTE   c;
   ULONG   InEncode = FALSE, Done = FALSE, OutPos = 0, bol, eol;

   if (!Buff)    return NULL;

   if (!BuffLen) BuffLen = strlen(Buff);
   if (Size)     *Size    = NULL;
   if (Err)      *Err     = SWV_UUDecode_OK;
   if (Lines)    *Lines   = 0;

   // -- allocate a buffer for the output data -------------------------------
   
   if (!(Data = Out))
      if (!(Data = AllocVec(BuffLen * 3 / 4, MEMF_ANY)))
         return NULL;

   // -- loop through buffer, uudecoding it ----------------------------------

   for (bol=0; Buff[bol] && !Done; bol = eol + 1) {
      eol = GetEOL(Buff, bol);

      if ((eol - bol) < 2 || Buff[bol] == ' ' || Buff[bol] == '\t') continue;
      
      if (Lines) (*Lines)++;
      
      c   = Buff[eol], Buff[eol] = '\0';

      // -- look for end line ------------------------------------------------

      if (InEncode && (SWUUIsEnd(Buff + bol) || Buff[bol] == '\0'))
         InEncode = FALSE, Done = TRUE;

      // -- uudecode a single line -------------------------------------------
      
      if (InEncode)
         OutPos = SWUUDecodeLine(Buff + bol, Data, OutPos, Err);

      // -- look for begin line ----------------------------------------------
      
      if (!InEncode && SWUUIsBegin(Buff + bol))
         InEncode = TRUE;
      
      Buff[eol] = c;
   }

   if (Size) *Size = OutPos;

   return Data;
}


/*****************************************************************************
 * Function: Count lines in region                                           *
 *****************************************************************************/

__saveds __asm
ULONG SWCountLines(a0 STRPTR Buff)
{
   ULONG Lines = 0;
   // -- count text size & # of lines ----------------------------------------

   for (; *Buff; Buff++)
      if (*Buff == '\n')
         Lines++;

   return Lines;
}


/*****************************************************************************
 * Function: base64 decode data and return binary                            *
 *****************************************************************************/

__saveds __asm
UBYTE *SWBase64Decode(a0 STRPTR Buff, d0 ULONG BuffLen,
                      a1 ULONG *Size, a2 ULONG *Lines, a3 ULONG *Err,
                      a5 STRPTR Out)
{
   static BYTE Base64Table[] = {
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  62,  -1,  -1,  -1,  63,  52,  53,
      54,  55,  56,  57,  58,  59,  60,  61,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,   0,   1,   2,   3,   4,
       5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
      25,  -1,  -1,  -1,  -1,  -1,  -1,  26,  27,  28,
      29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
      39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
      49,  50,  51,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      -1,  -1,  -1,  -1,  -1,  -1
   };

   UBYTE  *Data, c;
   ULONG   OutPos = 0, x, Val = 0;
   LONG    QuadCount = 0, PadCount = 0;
   BYTE    v;

   if (!Buff) return NULL;

   if (!BuffLen) BuffLen = strlen(Buff);
   if (Size)    *Size    = NULL;
   if (Err)     *Err     = 0;
   if (Lines)   *Lines   = 0;

   // -- allocate a buffer for the output data -------------------------------

   if (!(Data = Out))   
      if (!(Data = AllocVec(BuffLen * 3 / 4 + 32, MEMF_ANY)))
         return NULL;

   // -- loop through buffer, base64decoding it ------------------------------

   for (x=0; c = Buff[x]; x++) {
      if (IsWhite(c))
         continue;

      if (c == '\n') {
         if (Lines) (*Lines)++;
         continue;
      }

      if (c != '=') {
         if ((v = Base64Table[c]) < 0) {
            *Err = SWV_Base64_Err_BadChar;
            v    = 0;
         }
      } else v=0, PadCount++;

      Val = (Val << 6) | v;

      if (++QuadCount == 4) {
         Data[OutPos++] = ((UBYTE *)&Val)[1];
         Data[OutPos++] = ((UBYTE *)&Val)[2];
         Data[OutPos++] = ((UBYTE *)&Val)[3];

         QuadCount      = 0;
         Val            = 0;
      }
   }

   if (PadCount == 1) OutPos -= 1;
   if (PadCount == 2) OutPos -= 2;

   if (Size) *Size = OutPos;

   return Data;
}


/*****************************************************************************
 * Function: decode quoted printable data                                    *
 *****************************************************************************/

__saveds __asm
UBYTE *SWQPDecode(a0 STRPTR Buff, d0 ULONG BuffLen,
                  a1 ULONG *Size, a2 ULONG *Lines, a3 ULONG *Err,
                  a5 STRPTR Out)
{
   UBYTE  *Data, c;
   ULONG   OutPos = 0, x;

   if (!Buff) return NULL;
   
   if (!BuffLen) BuffLen  = strlen(Buff);
   if (Size)     *Size    = NULL;
   if (Err)      *Err     = 0;
   if (Lines)    *Lines   = 0;

   // -- allocate a buffer for the output data -------------------------------

   if (!(Data = Out))
      if (!(Data = AllocVec(BuffLen + 32, MEMF_ANY)))
         return NULL;

   // -- loop through buffer, base64decoding it ------------------------------

   for (x=0; c = Buff[x]; x++) {
      if (c == '=') {
         if (Buff[x+1] == '\n') {  // handle soft EOL marker;
            x++;
            if (Lines) (*Lines)++;
            continue;
         } else {
            if (Buff[x+1] && Buff[x+2])
               Data[OutPos++] = avtod(Buff[x+1])*16 + avtod(Buff[x+2]);
            x += 2;
         }
      } else {
         if (c == '\n')
            if (Lines) (*Lines)++;
         
         Data[OutPos++] = Buff[x];
      }
   }

   if (Size) *Size = OutPos ? OutPos - 1 : 0;

   return Data;
}


/*****************************************************************************
 * Function: Find last non-white character in a string                       *
 *****************************************************************************/

static STRPTR LastNonWhite(STRPTR s)
{
   STRPTR lnw = s;

   for (; *s; s++)
      if (!IsWhite(*s))
         lnw = s;

   return lnw;
}


/*****************************************************************************
 * Function: encode quoted printable data                                    *
 *   In buffer must be at least 78 bytes long!!!                             *
 *                                                                           *
 *   Returns NULL for error, or a pointer to the last encoded character.     *
 *   If this isn't the end of the string, you must call this function again  *
 *   on the rest!!  This is because QP lines cannot be more than 76          *
 *   characters.                                                             *
 *****************************************************************************/

__saveds __asm
STRPTR SWQPEncodeStr(a0 STRPTR InStr, a1 STRPTR OutBuff)
{
   static TEXT QPTab[] = "0123456789ABCDEF";
   LONG   OBPos, EndOfWordPos;
   STRPTR lnw, EndOfWord;
   UBYTE  c;
   
   if (!InStr || !OutBuff) return NULL;

   lnw = LastNonWhite(InStr);
   
   for (OBPos = EndOfWordPos = 0; (c=*InStr) && OBPos < 73; InStr++) {
      if (QPNeedsEncoding(c) || (IsWhite(c) && InStr >= lnw)) {
         OutBuff[OBPos++] = '=';
         OutBuff[OBPos++] = QPTab[((c)&0xf0) >> 4];
         OutBuff[OBPos++] = QPTab[((c)&0x0f)];
      } else
         OutBuff[OBPos++] = c;

      if (!IsWhite(c) && IsWhite(InStr[1]))
         EndOfWord = InStr+1, EndOfWordPos = OBPos;
   }

   // -- if not enough room, back up to end of last word ---------------------
   
   if (c) {
      if (EndOfWordPos)
         OBPos = EndOfWordPos,
         InStr = EndOfWord;
      
      OutBuff[OBPos++] = '=';  // soft line break if we didn`t finish
   }

   OutBuff[OBPos] = '\0';
   
   return InStr;
}


/*****************************************************************************
 * Function: decode mime data                                                *
 *****************************************************************************/

__saveds __asm
UBYTE *SWMimeDecode(a0 STRPTR Buff, d0 ULONG BuffLen,
                    a1 ULONG *Size, a2 ULONG *Lines, a3 ULONG *Err,
                    a5 STRPTR MimeEnc)
{
   UBYTE *Data;
   
   if (Err)      *Err      = 0;

   if (!Buff) return NULL;
   
#  define IsMimeEncoding(m, t) (Strnicmp(m, t, strlen(t)) == 0)

   // -- decode data ---------------------------------------------------------

   if (!SW_STRISNULL(MimeEnc)) {
      if (IsMimeEncoding(MimeEnc, "base64"))
         return SWBase64Decode(Buff, 0, Size, Lines, Err, NULL);
   
      if (IsMimeEncoding(MimeEnc, "quoted-printable"))
         return SWQPDecode(Buff, 0, Size, Lines, Err, NULL);
   
      if (IsMimeEncoding(MimeEnc, "x-uue") ||
          IsMimeEncoding(MimeEnc, "x-uuencode"))
         return SWUUDecode(Buff, 0, Size, Lines, Err, NULL);
   }

   // -- otherwise, just copy it ---------------------------------------------

   if (!BuffLen) BuffLen = strlen(Buff);
   if (Size) *Size = BuffLen;

   if (!(Data = AllocVec(BuffLen+32, MEMF_ANY)))
      return NULL;

   memcpy(Data, Buff, BuffLen);

   return Data;
}


/*****************************************************************************
 * Function: decode error string                                             *
 *****************************************************************************/

__saveds __asm
STRPTR SWDecodeErrStr(d0 ULONG Err)
{
   switch (Err) {
     case SWV_UUDecode_OK:
      return "OK";
     case SWV_UUDecode_Err_BadCS:
      return "Bad Checksum";
     case SWV_UUDecode_Err_BadSize:
      return "Bad Size";
     case SWV_UUDecode_Err_BadLL:
      return "Bad Line Len";
     case SWV_Base64_Err_BadChar:
      return "Bad Base64 Character";
     case SWV_Err_Malloc:
      return "Malloc Error";
     case SWV_Err_FOpen:
      return "Error opening file `%s'";
     case SWV_Err_Read:
      return "Error read file `%s'";
     case SWV_Err_Write:
      return "Error writing file `%s'";
   }

   return "Unknown";
}
