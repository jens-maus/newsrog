# ifndef CT_SUPPORT_INCLUDE
#   define CT_SUPPORT_INCLUDE

    /************************************************************************
     * Function: return next non-ctrl char                                  *
     ************************************************************************/
    
    static __inline ULONG CT_GetNextChr(const STRPTR s, ULONG pos)
    {
       if (s[pos] == '\033')
          while (s[pos] && s[pos++] != '}');
       
       return pos;
    }

    static __inline STRPTR CT_GetNextPtr(const STRPTR s, ULONG pos)
    {
       return s + CT_GetNextChr(s, pos);
    }


    /************************************************************************
     * Macro: Loop through string, ignoring control garbage                 *
     *    Buff:  string to loop through                                     *
     *    loop:  loop variable (ULONG)                                      *
     *    stpos: place to start looping                                     *
     *    cond:  must remain true or looping stops                          *
     *    eol:   end of loop action (i.e, myvar++)                          *
     ************************************************************************/
    
#   define CT_Loop(Buff, loop, stpos, cond, eol) \
    for ((loop) = CT_GetNextChr(Buff, stpos); \
         ((Buff)[loop]) && (cond); (loop)=CT_GetNextChr(Buff, ++(loop)),eol)


# endif
