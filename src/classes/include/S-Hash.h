/*****************************************************************************
 * Function: Find string hash value                                          *
 *****************************************************************************/

static __inline ULONG StrHash(STRPTR ID)
{
   ULONG x=81241, h = 472141;
   while (*ID) h ^= (ULONG)(*ID++)*(x++)*92131;

   return h;
}


/*****************************************************************************
 * Macro: Loop through a hash table                                          *
 *   h  = variable for table position                                        *
 *   hv = tmp var to store string hash value                                 *
 *   s  = size of table array                                                *
 *   c  = var to store chain count                                           *
 *   id = string to hash                                                     *
 *   notdone = loop termination check                                        *
 *****************************************************************************/

# define StrHashPos(hv,c,s) (((hv)^((c)*107))%(s))

# define StrHashLoop(h,hv,s,c,notdone) \
         for ((c)=0; (h)=StrHashPos(hv,++c,s),(notdone); )
