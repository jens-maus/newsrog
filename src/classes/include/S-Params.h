
#  ifndef SW_CLASS
#    define SW_CLASS SW
#  endif

#  define _SW_G_IDATA(x)       x##IData
#  define _SW_G_DISPFN(x)      x##Dispatch
#  define _SW_G_INITFN(x)      x##InitClass
#  define _SW_G_FREEFN(x)      x##FreeClass
#  ifdef NOLIBS
#     define _SW_G_STARTUP(x)  _STI_4500_##x
#     define _SW_G_SHUTDOWN(x) _STD_4500_##x
#  else
#     define _SW_G_STARTUP(x)  STI_4500_##x
#     define _SW_G_SHUTDOWN(x) STD_4500_##x
#  endif
#  define _SW_G_ATTRMAP(x)     x##Attrs
#  define _SW_G_CATALOG(x)     x##Catalog
#  define _SW_G_CATSTR(x)      x##CatStr
#  define _SW_G_MSGSTR(x)      x##MsgStr

   
#  define SW_G_IDATA(x)       _SW_G_IDATA(x)
#  define SW_G_DISPFN(x)      _SW_G_DISPFN(x)
#  define SW_G_INITFN(x)      _SW_G_INITFN(x)
#  define SW_G_FREEFN(x)      _SW_G_FREEFN(x)
#  define SW_G_STARTUP(x)     _SW_G_STARTUP(x)
#  define SW_G_SHUTDOWN(x)    _SW_G_SHUTDOWN(x)
#  define SW_G_ATTRMAP(x)     _SW_G_ATTRMAP(x)
#  define SW_G_CATALOG(x)     _SW_G_CATALOG(x)
#  define SW_G_CATSTR(x)      _SW_G_CATSTR(x)
#  define SW_G_MSGSTR(x)      _SW_G_MSGSTR(x)

#  define SW_IDATA            SW_G_IDATA    (SW_CLASS)
#  define SW_DISPFN           SW_G_DISPFN   (SW_CLASS)
#  define SW_INITFN           SW_G_INITFN   (SW_CLASS)
#  define SW_FREEFN           SW_G_FREEFN   (SW_CLASS)
#  define SW_STARTUP          SW_G_STARTUP  (SW_CLASS)
#  define SW_SHUTDOWN         SW_G_SHUTDOWN (SW_CLASS)
#  define SW_ATTRMAP          SW_G_ATTRMAP  (SW_CLASS)

#  ifdef  SW_CATVER
#     define SW_CATALOG          SW_G_CATALOG  (SW_CLASS)
#     define SW_CATSTR           SW_G_CATSTR   (SW_CLASS)
#     define SW_MSGSTR           SW_G_MSGSTR   (SW_CLASS)
#  endif

#  ifndef SW_SUPERPUB
#     define SW_SUPERPUB      NULL
#  endif

#  ifndef SW_SUPERPRI
#     define SW_SUPERPRI      NULL
#  endif
