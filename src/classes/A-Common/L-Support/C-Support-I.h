# ifndef C_SUPPORT_INCLUDE_I
#  define C_SUPPORT_INCLUDE_I

#  include <proto/iffparse.h>
#  include "S-Classes.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-Invocation.h"
#  include "S-Params.h"
#  include <stdarg.h>
#  include <libraries/asl.h>
#  include <libraries/iffparse.h>
#  include <clib/rexxsyslib_protos.h>
#  include <pragmas/rexxsyslib_pragmas.h>

#  define ID_FTXT                 MAKE_ID('F','T','X','T')
#  define ID_CHRS                 MAKE_ID('C','H','R','S')

   // -- other internal only types, defines etc -----------------------------

extern   APTR   *SWPool;

# define AV(s)  SWPoolAV(SWPool, s)
# define FV(m)  SWPoolFV(SWPool, m)

#  define SWV_Support_PrefsVer 1

# endif
