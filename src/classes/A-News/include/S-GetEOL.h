/*****************************************************************************
 * Function: return next end of line                                         *
 *****************************************************************************/

static __inline ULONG GetEOL(const STRPTR s, ULONG pos)
{
   for (; s[pos] && s[pos] != '\n'; pos++);

   return pos;
}
