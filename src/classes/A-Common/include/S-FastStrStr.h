# include <ctype.h>

# ifdef FASTSTRSTR_NOINLINE
#   define FSTYPE
# else
#   define FSTYPE __inline
# endif

// define FASTSTRSTR_POS to get back 0 for no match, or pos+1 if a match.
// i.e, if match is at chr 0, you get back 1, etc.

/*****************************************************************************
 * Function: high performance strstr                                         *
 *****************************************************************************/

static FSTYPE LONG faststrstr(STRPTR s1, STRPTR s2,
                              ULONG len1, ULONG len2, ULONG Case)
{
#  ifdef FASTSTRSTR_POS
   STRPTR OrigS1 = s1;
#  endif
   
   if (len2 > len1) return FALSE;

   if (Case) {
      if (len2 >= 9) {
         ULONG l2p0 = ((ULONG *)(s2+1))[0];
         ULONG l2p1 = ((ULONG *)(s2+1))[1];
         
         while (s1[len2-1])
            if (*s1++ == s2[0])
               if (((ULONG *)s1)[0] == l2p0 && ((ULONG *)s1)[1] == l2p1)
                  if (!strncmp(s1+8, s2+9, len2-9))
#                    ifdef FASTSTRSTR_POS
                        return s1 - OrigS1;
#                    else
                        return TRUE;
#                    endif
      } else if (len2 >= 5) {
         ULONG l2p0 = ((ULONG *)(s2+1))[0];
         
         while (s1[len2-1])
            if (*s1++ == s2[0])
               if (*((ULONG *)(s1)) == l2p0)
                  if (!strncmp(s1+4, s2+5, len2-5))
#                    ifdef FASTSTRSTR_POS
                        return s1 - OrigS1;
#                    else
                        return TRUE;
#                    endif
      } else if (len2 >= 2) {
         UWORD u2p = *((UWORD *)s2);
         
         while (*s1)
            if (*((UWORD *)(s1++)) == u2p)
               if (!strncmp(s1+1, s2+2, len2-2))
#                 ifdef FASTSTRSTR_POS
                     return s1 - OrigS1;
#                 else
                     return TRUE;
#                 endif
         
      } else {
         while (*s1)
            if (*s1++ == s2[0])
#              ifdef FASTSTRSTR_POS
                  return s1 - OrigS1;
#              else
                  return TRUE;
#              endif
      }
   } else {
      ULONG s20 = toupper(s2[0]);

      while (s1[0]) {
         if (toupper(s1[0]) == s20)
            if (!Strnicmp(s1, s2, len2))
#              ifdef FASTSTRSTR_POS
                  return s1 - OrigS1 + 1;
#              else
                  return TRUE;
#              endif
         
         s1++;
      }
   }

#  ifdef FASTSTRSTR_POS
      return 0;
#  else
      return FALSE;
#  endif
}


/*****************************************************************************
 * Function: fast strcmp (optional case) if we know lengths                  *
 *****************************************************************************/

static FSTYPE LONG faststrcmp(STRPTR s1, STRPTR s2,
                              ULONG l1, ULONG l2, ULONG Case)
{
   if (l1 != l2) return FALSE;

   if (Case) {
      if (l2 >= 4 && *((ULONG *)s1) != *((ULONG *)s2))
         return FALSE;
      return !strcmp(s1, s2);
   } else {
      return !Stricmp(s1, s2);
   }
}


/*****************************************************************************
 * Function: strstr or strcmp                                                *
 *****************************************************************************/

static FSTYPE LONG findstr(STRPTR s1, STRPTR s2,
                           ULONG l1, ULONG l2, ULONG Case, ULONG Wild)
{
   return Wild ? faststrstr(s1, s2, l1, l2, Case) :
                 faststrcmp(s1, s2, l2, l2, Case);
}
