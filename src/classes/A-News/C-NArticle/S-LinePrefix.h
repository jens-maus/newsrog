/*****************************************************************************
 * Function: Return prefix if we need to include it, or NULL                 *
 *****************************************************************************/

static STRPTR LinePrefix(STRPTR Line, STRPTR Prefix, ULONG QMode)
{
   if (SW_STRISNULL(Prefix)) return NULL;
   
   if (QMode == SWV_NNews_QM_PrefixQB ||
       QMode == SWV_NNews_QM_PrefixSB && !SW_STRISNULL(Line))
      return Prefix;

   return NULL;
}


