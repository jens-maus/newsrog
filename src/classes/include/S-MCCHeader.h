
# define CLASS          SW_CLIB
# define SUPERCLASS     SW_SUPERPUB
# if !defined VERSION
#    define VERSION        1
#    define VERSTR        "1"
# endif
# if !defined REVISION
#    define REVISION       5
#    define REVSTR        "5"
# endif
# define UserLibID      "$VER: " SW_CLIB " " VERSTR "." REVSTR " " __AMIGADATE__
# define Data           SW_IDATA
# define _Dispatcher    SW_DISPFN
# define ClassInit      ClassInitFunc
# define ClassExit      ClassExitFunc
# define MASTERVERSION  MUIMASTER_VLATEST

# include "mccheader.c"

# undef CLASS
# undef SUPERCLASS
# undef VERSION
# undef REVISION
# undef VERSTR
# undef UserLibID
# undef Data
# undef _Dispatcher
# undef ClassInit
# undef ClassExit
# undef MASTERVERSION
