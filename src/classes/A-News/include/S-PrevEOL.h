/*****************************************************************************
 * Function: return prev end of line                                         *
 *****************************************************************************/

static __inline ULONG PrevEOL(const STRPTR s, ULONG pos)
{
   for (; pos && s[pos] != '\n'; pos--);

   return pos;
}

static __inline ULONG PrevBOL(const STRPTR s, ULONG pos)
{
   if (pos>1 && s[pos-1]=='\n' && s[pos-2]=='\n')
      return pos - 1;
   
   pos = PrevEOL(s, pos>1 ? pos-2 : pos);

   return s[pos+1] == '\n' ? pos : pos + 1;
}
