   

// -- DEBUG stuff - turn on SW_USEDEBUG to use -------------------------------

  typedef struct SWDebugInfo
  {
     BPTR   fp;
     ULONG  Indent;
     ULONG  Flags;
     ULONG  Classes;
  } SWDebugInfo;

# define SWDB_EnvLevel   "ShadowWorks/DebugLevel"
# define SWDB_EnvClasses "ShadowWorks/DebugClasses"
# define SWDB_EnvName    "ShadowWorks/DebugFile"

  enum {
     SWDB_Assert         = (1<< 0),       // assertion
     SWDB_Error          = (1<< 1),       // error
     SWDB_FuncCall       = (1<< 2),       // plain function call
     SWDB_ParamList      = (1<< 3),       // parameter list of functions
     SWDB_Logic          = (1<< 4),       // logic
     SWDB_ObjFunc        = (1<< 5),       // object function call
     SWDB_ObjMsg         = (1<< 6),       // object message
     SWDB_LibFunc        = (1<< 7),       // library function
  };

  enum {
     SWDB_C_Root         = (1<< 0),       // root            class
     SWDB_C_Template     = (1<< 0),       // template        class
     SWDB_C_Class        = (1<< 1),       // class           class
     SWDB_C_Invocation   = (1<< 2),       // invocation      class
     SWDB_C_Timer        = (1<< 3),       // timer           class
     SWDB_C_Clock        = (1<< 4),       // clock           class
     SWDB_C_Layout       = (1<< 5),       // layout          class
     SWDB_C_ClassList    = (1<< 6),       // classlist       class
     SWDB_C_ObjectList   = (1<< 7),       // objectlist      class
     SWDB_C_Text         = (1<< 8),       // text            class
     SWDB_C_Bitmap       = (1<< 9),       // bitmap          class
     SWDB_C_Icon         = (1<< 9),       // icon            class
     SWDB_C_Syscmd       = (1<<10),       // syscmd          class
     SWDB_C_Dropcmd      = (1<<11),       // dropcmd         class
     SWDB_C_Space        = (1<<11),       // space           class
     SWDB_C_Boolstatus   = (1<<12),       // boolean monitor class
     SWDB_C_Objectbox    = (1<<13),       // object box edit class
     SWDB_C_Boolfile     = (1<<14),       // boolean file    class
     SWDB_C_Boolsrch     = (1<<15),       // boolean search  class
     SWDB_C_Boolser      = (1<<15),       // boolean serial  class
     SWDB_C_Screenview   = (1<<16),       // screen monitor  class
     SWDB_C_Window       = (1<<17),       // window          class
     SWDB_C_Barnone      = (1<<18),       // barnone         class
     SWDB_C_News         = (1<<18),       // news            class
     SWDB_C_Meter        = (1<<19),       // meter           class
     SWDB_C_Loadmeter    = (1<<20),       // load meter      class
     SWDB_C_Memmeter     = (1<<21),       // mem meter       class
     SWDB_C_Sysmeter     = (1<<22),       // sys meter       class
     SWDB_C_Booltask     = (1<<23),       // boolean task    class
     SWDB_C_E_Winlist    = (1<<24),       // winlist         class
     SWDB_C_E_Tpllist    = (1<<24),       // tpllist         class
     SWDB_C_E_Varlist    = (1<<24),       // varlist         class
     SWDB_C_E_Filetypes  = (1<<24),       // filetypes       class
     SWDB_C_Attrval      = (1<<25),       // attrval         class
     SWDB_C_Screenswitch = (1<<26),       // screen switch   class
     SWDB_C_PopWin       = (1<<27),       // pop window      class
     SWDB_C_Iterator     = (1<<28),       // iterator        class
     SWDB_C_DirIterator  = (1<<29),       // dir iterator    class
     SWDB_C_ScrIterator  = (1<<29),       // dir iterator    class
     SWDB_C_Updatetext   = (1<<30),       // updated text    class
  };

# define SWDB_Class_All (SWDB_C_Root        | \
                         SWDB_C_Class       | \
                         SWDB_C_Template    | \
                         SWDB_C_Invocation  | \
                         SWDB_C_Timer       | \
                         SWDB_C_Clock       | \
                         SWDB_C_Layout      | \
                         SWDB_C_ClassList   | \
                         SWDB_C_ObjectList  | \
                         SWDB_C_Text        | \
                         SWDB_C_Bitmap      | \
                         SWDB_C_Syscmd      | \
                         SWDB_C_Space       | \
                         SWDB_C_Boolstatus  | \
                         SWDB_C_Objectbox   | \
                         SWDB_C_Boolfile    | \
                         SWDB_C_Boolsrch    | \
                         SWDB_C_Screenview  | \
                         SWDB_C_Window      | \
                         SWDB_C_Barnone     | \
                         SWDB_C_Meter       | \
                         SWDB_C_Loadmeter   | \
                         SWDB_C_Memmeter    | \
                         SWDB_C_Sysmeter    | \
                         SWDB_C_Booltask    | \
                         SWDB_C_E_Winlist   | \
                         SWDB_C_E_Tpllist   | \
                         SWDB_C_E_Varlist   | \
                         SWDB_C_Attrval     | \
                         SWDB_C_PopWin      | \
                         SWDB_C_Iterator    | \
                         SWDB_C_DirIterator | \
                         SWDB_C_ScrIterator | \
                         SWDB_C_Updatetext  | \
                         0)

# ifdef SW_USEDEBUG

#    define SWDB_FUNCTYPE    SWDB_FuncCall
#    define SWDB_CLASSTYPE   -1
#    define SWDB_FUNCPREFIX  __FILE__ ": "

#    define SWDB_ENTER(f,c)  SWDebugEnter(f,c,SWDB_FUNCPREFIX __FUNC__)
#    define SWDB_EXIT(f,c)   SWDebugExit(f,c,SWDB_FUNCPREFIX __FUNC__)
#    define SWDB_OPEN(f,l,c) SWDebugOpen(f, l, c)
#    define SWDB_CLOSE()     SWDebugClose()
#    define SWDB_RET(x)      {_sdbrc=((ULONG)(x)); goto SDFDone;}
#    define SWDB_RETV        goto SDFDone
#    define SWDB_ERROR(s)    SWDebugPrint(SWDB_Error,0, \
                                          "Error: " __FUNC__ ": %s",s)
#    define SWDB_ASSERT(c,s) if (!(c)) \
                               SWDebugPrint(SWDB_Assert,0, \
                                            "Assert: " __FUNC__ ": %s",s);
#    define SWDB_LOGIC(s)    SWDebugPrint(SWDB_Logic, SWDB_CLASSTYPE, \
                                          "Logic: %s", s)
#    define SWDB_LOGICI(s,n) SWDebugPrint(SWDB_Logic, SWDB_CLASSTYPE, \
                                          "Logic: %s %ld",s,n)
#    define SWDB_LOGICII(a,b,c,d) SWDebugPrint(SWDB_Logic, SWDB_CLASSTYPE, \
                                               "Logic: %s %ld %s %ld", a,b,c,d)
#    define SWDB_OBJMSG(s,o) SWDebugPrint(SWDB_ObjMsg, \
                                          SWDB_CLASSTYPE,\
                                          "ObjMsg: 0x%lx %s",o,s);
#    define SWDB_OBJMSGX(s,o,a) SWDebugPrint(SWDB_ObjMsg, \
                                             SWDB_CLASSTYPE, \
                                             "ObjMsg: 0x%lx %s 0x%lx",o,s,a);
#    define SWDB_OBJMSGI(s,o,a) SWDebugPrint(SWDB_ObjMsg, \
                                             SWDB_CLASSTYPE, \
                                             "ObjMsg: 0x%lx %s: %ld",o,s,a);
#    define SWDB_OBJMSGS(s,o,a) SWDebugPrint(SWDB_ObjMsg, \
                                             SWDB_CLASSTYPE, \
                                             "ObjMsg: 0x%lx %s: \"%s\"",o,s,a);

#    define SDSV     { SWDB_ENTER(SWDB_FUNCTYPE, SWDB_CLASSTYPE);
#    define SDST(t)  { t _sdbrc; SWDB_ENTER(SWDB_FUNCTYPE, SWDB_CLASSTYPE);
#    define SDFV     SDFDone: SWDB_EXIT(SWDB_FUNCTYPE, SWDB_CLASSTYPE); }
#    define SDFT     SDFDone: SWDB_EXIT(SWDB_FUNCTYPE, SWDB_CLASSTYPE); \
                          return(_sdbrc); }

#    define _SWF(s)  (SWDB_FUNCTYPE|SWDB_ParamList),SWDB_CLASSTYPE,"Param: " s
#    define _SWI0    "%s=%ld, "
#    define _SWS0    "%s=\"%s\", "
#    define _SWX0    "%s=0x%lx, "
#    define _SWI1    "%s=%ld"
#    define _SWS1    "%s=\"%s\""
#    define _SWX1    "%s=0x%lx"

#    define _SWI     _SWI1
#    define _SWS     _SWS1
#    define _SWX     _SWX1
#    define _SWII    _SWI0 _SWI1
#    define _SWSS    _SWS0 _SWS1
#    define _SWIS    _SWI0 _SWS1
#    define _SWSI    _SWS0 _SWI1
#    define _SWXX    _SWX0 _SWX1

#    define _SWIII   _SWI0 _SWI0 _SWI1
#    define _SWIIS   _SWI0 _SWI0 _SWS1
#    define _SWISI   _SWI0 _SWS0 _SWI1
#    define _SWSII   _SWS0 _SWI0 _SWI1
#    define _SWSSI   _SWS0 _SWS0 _SWI1
#    define _SWSIS   _SWS0 _SWI0 _SWS1
#    define _SWISS   _SWI0 _SWS0 _SWS1
#    define _SWSSS   _SWS0 _SWS0 _SWS1
#    define _SWXXX   _SWX0 _SWX0 _SWX1
#    define _SWXIX   _SWX0 _SWI0 _SWX1
#    define _SWXXI   _SWX0 _SWX0 _SWI1

#    define SWP_I(a)       SWDebugPrint(_SWF(_SWI),  #a,a);
#    define SWP_S(a)       SWDebugPrint(_SWF(_SWS),  #a,a);
#    define SWP_X(a)       SWDebugPrint(_SWF(_SWX),  #a,a);
 
#    define SWP_II(a,b)    SWDebugPrint(_SWF(_SWII), #a,a,#b,b);
#    define SWP_SS(a,b)    SWDebugPrint(_SWF(_SWSS), #a,a,#b,b);
#    define SWP_IS(a,b)    SWDebugPrint(_SWF(_SWIS), #a,a,#b,b);
#    define SWP_SI(a,b)    SWDebugPrint(_SWF(_SWSI), #a,a,#b,b);
#    define SWP_XX(a,b)    SWDebugPrint(_SWF(_SWXX), #a,a,#b,b);

#    define SWP_III(a,b,c) SWDebugPrint(_SWF(_SWIII), #a,a,#b,b,#c,c);
#    define SWP_IIS(a,b,c) SWDebugPrint(_SWF(_SWIIS), #a,a,#b,b,#c,c);
#    define SWP_ISI(a,b,c) SWDebugPrint(_SWF(_SWISI), #a,a,#b,b,#c,c);
#    define SWP_SII(a,b,c) SWDebugPrint(_SWF(_SWSII), #a,a,#b,b,#c,c);
#    define SWP_SSI(a,b,c) SWDebugPrint(_SWF(_SWSSI), #a,a,#b,b,#c,c);
#    define SWP_SIS(a,b,c) SWDebugPrint(_SWF(_SWSIS), #a,a,#b,b,#c,c);
#    define SWP_ISS(a,b,c) SWDebugPrint(_SWF(_SWISS), #a,a,#b,b,#c,c);
#    define SWP_SSS(a,b,c) SWDebugPrint(_SWF(_SWSSS), #a,a,#b,b,#c,c);
#    define SWP_XXX(a,b,c) SWDebugPrint(_SWF(_SWXXX), #a,a,#b,b,#c,c);
#    define SWP_XIX(a,b,c) SWDebugPrint(_SWF(_SWXIX), #a,a,#b,b,#c,c);
#    define SWP_XXI(a,b,c) SWDebugPrint(_SWF(_SWXXI), #a,a,#b,b,#c,c);

# else
#    define SWDB_ENTER()
#    define SWDB_EXIT()
#    define SWDB_OPEN(f,l,c)
#    define SWDB_CLOSE()
#    define SWDB_RET(x)     return((ULONG)(x))
#    define SWDB_RETV       return
#    define SWDB_ERROR(s)
#    define SWDB_ASSERT(c,s)
#    define SWDB_LOGIC(s)
#    define SWDB_LOGICI(s,n)
#    define SWDB_LOGICII(a,b,c,d)
#    define SWDB_OBJMSG(s, o)
#    define SWDB_OBJMSGI(s,o,a)
#    define SWDB_OBJMSGS(s,o,a)
#    define SWDB_OBJMSGX(s,o,p)
   
#    define SDSV
#    define SDST(t)
#    define SDFV
#    define SDFT

#    define SWP_I(a)
#    define SWP_S(a)
#    define SWP_X(a)

#    define SWP_II(a,b)
#    define SWP_SS(a,b)
#    define SWP_IS(a,b)
#    define SWP_SI(a,b)
#    define SWP_XX(a,b)

#    define SWP_III(a,b,c)
#    define SWP_IIS(a,b,c)
#    define SWP_ISI(a,b,c)
#    define SWP_SII(a,b,c)
#    define SWP_SSI(a,b,c)
#    define SWP_SIS(a,b,c)
#    define SWP_ISS(a,b,c)
#    define SWP_SSS(a,b,c)
#    define SWP_XXX(a,b,c)
#    define SWP_XIX(a,b,c)
#    define SWP_XXI(a,b,c)

# endif

# define SWDB_ASSERTNULL(x) SWDB_ASSERT(x,#x "is NULL")

# define SWDB_ASSERTCLFN()  SWDB_ASSERTNULL(cl) \
                            SWDB_ASSERTNULL(o) \
                            SWDB_ASSERTNULL(msg)
