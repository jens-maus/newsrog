
/*****************************************************************************
 * Function: See if line has C brace by itself                               *
 *****************************************************************************/

static LONG IsCBrace(STRPTR s, UBYTE c)
{
   // -- check line 1 for opening curleybrace/comment ------------------------
   
   for (s=Lines[0]; *s==' ' || *s=='\t'; s++);  // skip whitespace

   if (*s != '{') return FALSE;

   for (s++; *s==' ' || *s=='\t'; s++);         // skip more whitespace

   if (!(s[0]=='\n'               ||
         s[0]=='/' && s[1] == '*' ||
         s[0]=='/' && s[1] == '/')) return FALSE;

   return TRUE;
}

/*****************************************************************************
 * Function: See if line looks like type fn(params)                          *
 *****************************************************************************/

static LONG IsCFn(STRPTR s)
{
   
}


/*****************************************************************************
 * Function: See if this is the beginning of some C code                     *
 *****************************************************************************/

static LONG IsCCodeBegin(SW_IDATA *idata,
                         STRPTR *Lines, ULONG StartLine, ULONG QPLen)

{
   if (!idata->SWA_NDF_Rewrap_Code_Use || !StartLine)
      return FALSE;

   // -- need a few lines left -----------------------------------------------
   
   Lines += StartLine;

   if (!CheckIntro(Lines, QPLen)) return FALSE;

   // -- check line 0 for function defn --------------------------------------

   if (!IsCBrace(Lines[1], '{'))  return FALSE;

   if (!IsCFn(Lines[0]))          return FALSE;

   return TRUE;
}
