
static LONG IsEmptyLine(STRPTR s)
{
   for (; *s && *s != '\n'; s++)
      if (*s != ' ' && *s != '\t')
         return FALSE;

   return TRUE;
}
