# ifndef C_NDF_REWRAP_INCLUDE
#  define C_NDF_REWRAP_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NDFilter.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG          (int)(TAG_USER + 12600)

   enum {
      SWA_NDF_Rewrap_QuoteChars = SW_FIRSTTAG,    // [SGI] Quote chars
      SWA_NDF_Rewrap_PrefixLen,                   // [SGI] prefix len
      SWA_NDF_Rewrap_WidthPct,                    // [SGI] % of display width
      SWA_NDF_Rewrap_BadWrap,                     // [SGI] try to fix bad wrap
      SWA_NDF_Rewrap_MinWrapLen,                  // [SGI] min len for wrap
      SWA_NDF_Rewrap_BreakChars,                  // [SGI] break characters
      SWA_NDF_Rewrap_NoFlow,                      // [SGI] noflow expression
      SWA_NDF_Rewrap_UseNoFlow,                   // [SGI] T to use above
      SWA_NDF_Rewrap_BadWrapMax,                  // [SGI] max badwrap len
      SWA_NDF_Rewrap_Cols,                        // [SGI] rewrap to col pos
      SWA_NDF_Rewrap_PrefixHeur,                  // [SGI] prefix heuristics
      SWA_NDF_Rewrap_Tbl_Use,                     // [SGI] use table heuristics?
      SWA_NDF_Rewrap_Tbl_MinRows,                 // [SGI] min table rows
      SWA_NDF_Rewrap_Tbl_MinCols,                 // [SGI] min table columns
      SWA_NDF_Rewrap_Tbl_MinTblProb,              // [SGI] min table probability
      SWA_NDF_Rewrap_Tbl_MaxILines,               // [SGI] max intro lines
      SWP_NDF_StyleAttrVals(SWA_NDF_Rewrap_Tbl_), // [SGI] table font
   };

   enum {
      SWM_NDF_Rewrap_RewrapTx = SW_FIRSTTAG,      // rewrap text
   };

   SWM_P(SWP_NDF_Rewrap_RewrapTx, TxtBuff *Tx; ULONG *HasHeaders);

#  undef SW_FIRSTTAG   

# endif
