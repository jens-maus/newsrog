# ifdef SW_PROFILE

# include <proto/timer.h>
# include <time.h>

# define TimerBase _profTimerBase

struct timerequest _ProfTimerIO;
struct Library *TimerBase;

# define _PROFINIT(v) struct EClockVal _profbaseline, _proft0; \
                      LONG  _profsum, _profint; \
                      static LONG _profrecur = v;

# define _PROFTERM    _profrecur--

void _ProfInit(void)
{
   if (!TimerBase) {
      if (OpenDevice(TIMERNAME, UNIT_ECLOCK, (struct IORequest *)&_ProfTimerIO, 37L))
         return;
   
      TimerBase = (struct Library *)_ProfTimerIO.tr_node.io_Device;
   }
}


void _ProfTerm(void)
{
   CloseDevice((struct IORequest *)&_ProfTimerIO);
   TimerBase = NULL;
}

# define _ProfDB(s) s

# define _ProfBegin(f) \
   { if (!_profrecur++) { \
        if (f) Printf("PROF: %s\n", f); \
        _profsum = 0; \
        ReadEClock(&_profbaseline); } }

# define _ProfCheck(f)                                     \
   {                                                       \
      if (_profrecur==1) {                                 \
         ReadEClock(&_proft0);                                \
                                                              \
         _profint = _proft0.ev_lo - _profbaseline.ev_lo - 12; \
         _profint = _profint < 0 ? 0 : _profint;              \
                                                              \
         _profsum += _profint;                                \
                                                              \
         if (f) Printf("PROF:   Tot: %5ld, Int: %5ld, %s\n",  \
                       _profsum, _profint, f);                \
                                                              \
         ReadEClock(&_profbaseline);                          \
      } \
   }

# else
#   define _ProfTerm()
#   define _ProfInit()
#   define _ProfBegin(f)
#   define _ProfCheck(f)
#   define _PROFINIT(v)
#   define _PROFTERM
#   define _ProfDB(s)
# endif
