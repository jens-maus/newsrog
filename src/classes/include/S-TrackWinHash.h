
/*****************************************************************************
 * Track # of windows open with a particular hash                            *
 *****************************************************************************/

#  define TWC_Max 16
   enum { TWC_Open, TWC_Close };
   struct WinIDs { ULONG Hash, Count; };

static ULONG TrackWinHash(ULONG Hash, ULONG Cmd)
{
   ULONG  x;
   static struct WinIDs WinID[TWC_Max] = {0};

   if (Cmd == TWC_Open) {
      for (x=0; x<TWC_Max; x++)
         if (WinID[x].Count && WinID[x].Hash == Hash) break;

      if (x >= TWC_Max)
         for (x=0; x<TWC_Max; x++)
            if (WinID[x].Count == 0) break;

      return x >= TWC_Max ? 0 : (WinID[x].Hash=Hash, WinID[x].Count++);
   }

   for (x=0; x<TWC_Max; x++)
      if (WinID[x].Count && WinID[x].Hash == Hash)
         WinID[x].Count--;
   
   return 0;
}
