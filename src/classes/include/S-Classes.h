
#ifndef S_CLASSES_INCLUDE
# define S_CLASSES_INCLUDE

# define __osver 39                    // minimum permitted OS lib versions
# define USE_BUILTIN_MATH              // use builtin min/max/etc
# define _USEOLDEXEC_

// -- include our header files -----------------------------------------------

# include <string.h>
# include <stdlib.h>
# include <stddef.h>
# include <limits.h>
# include <dos.h>

# include <proto/intuition.h>
# include <proto/graphics.h>
# include <proto/dos.h>
# include <proto/datatypes.h>
# include <proto/exec.h>
# include <proto/locale.h>
# include <proto/utility.h>
# include <proto/gadtools.h>
# include <proto/commodities.h>
# include <proto/layers.h>
# include <proto/icon.h>

# include <exec/execbase.h>
# include <exec/memory.h>
# include <exec/semaphores.h>

# include <workbench/workbench.h>
# include <workbench/startup.h>
# include <workbench/icon.h>

# include <intuition/classusr.h>
# include <intuition/classes.h>
# include <intuition/imageclass.h>
# include <intuition/gadgetclass.h>
# include <intuition/icclass.h>

# include <libraries/commodities.h>

# include <devices/timer.h>

# include <graphics/gfxmacros.h>
# include <graphics/gfx.h>

# include <rexx/rxslib.h>
# include <rexx/errors.h>

# include <clib/alib_stdio_protos.h>

# ifdef USEMUI
#   include <libraries/mui.h>
#   include <proto/muimaster.h>
#   define MUIA_Group_Forward               0x80421422 /* V11 .s. BOOL */
#   define MUIA_Prop_Delta                  0x8042c5f4 /* V4  is. LONG */
#   define MUIM_Application_KillPushMethod  0x80429954 /* V15 */
    struct MUIP_Application_KillPushMethod     { ULONG MethodID; Object *o; };
# endif

# ifdef USECYBERGFX
#   include <MUI/Listtree_mcc.h>
#   include <cybergraphics/cybergraphics.h>
#   include <proto/cybergraphics.h>
# endif

# include "S-AttrMaps.h"

// -- include some register definitions --------------------------------------

#  define a0   register __a0
#  define a1   register __a1
#  define a2   register __a2
#  define a3   register __a3
#  define a4   register __a4
#  define a5   register __a5
#  define a6   register __a6
#  define d0   register __d0
#  define d1   register __d1
#  define d2   register __d2
#  define d3   register __d3
#  define d4   register __d4
#  define d5   register __d5
#  define d6   register __d6
#  define d7   register __d7


// -- typedefs to save code space, improve readability -----------------------

typedef struct DosLibrary      DosLibrary;
typedef struct DrawInfo        DrawInfo;
typedef struct Gadget          Gadget;
typedef struct Image           Image;
typedef struct InputEvent      InputEvent;
typedef struct Library         Library;
typedef struct List            List;
typedef struct MinList         MinList;
typedef struct MinNode         MinNode;
typedef struct Node            Node;
typedef struct Task            Task;
typedef struct RastPort        RastPort;
typedef struct Screen          Screen;
typedef struct SignalSemaphore SignalSemaphore;
typedef struct TagItem         TagItem;
typedef struct TextFont        TextFont;
typedef struct Window          Window;
typedef struct opAddTail       opAddTail;
typedef struct opGet           opGet;
typedef struct opMember        opMember;
typedef struct opSet           opSet;
typedef struct BitMap          BitMap;
typedef struct ContextNode     ContextNode;
typedef struct StoredProperty  StoredProperty;
typedef struct IFFHandle       IFFHandle;
typedef struct MsgPort         MsgPort;
typedef struct Catalog         Catalog;
typedef struct NewMenu         NewMenu;
typedef struct MUIS_Listtree_TreeNode TreeNode;
typedef struct MUI_ImageSpec   MUI_ImageSpec;
typedef struct MUI_PenSpec     MUI_PenSpec;

# define SUCCESS 0
# define FAILURE 1

// -- function argument lists, for easy global modification ------------------

# define INITFNPROTO    void
# define DISPFNPROTO    a0 Class *cl, a2 Object *o, a1 Msg msg
# define DISPFNPROTO_I  a0 IClass *cl, a2 Object *o, a1 Msg msg
# define DISPDECL_I(x)  __saveds __asm ULONG x##Dispatcher(DISPFNPROTO)
# define SETFNPROTO     d0 Tag Tag, d1 ULONG Data, \
                        a0 Class *cl, a2 Object *o, a1 Msg msg
# define GETFNPROTO     DISPFNPROTO
# define METHODFNPROTO  DISPFNPROTO
# define DRAWFNPROTO    a0 Class *cl,a2 Object *o,a1 struct MUIP_Draw *msg
# define MINMAXFNPROTO  a0 Class *cl,a2 Object *o,a1 \
                        struct MUIP_AskMinMax *msg
# define SETUPFNPROTO   a2 Object *o, d0 ULONG retval, a1 Class *cl, \
                        a0 SW_IDATA *idata, a3 TagItem *ti
# define DISPOSEFNPROTO a2 Object *o, a0 SW_IDATA *idata

# define REGCLASSPROTO  a0 const UBYTE  *ClassName,    \
                        a1 const ClassT *ClassPtr,     \
                        d0 const ULONG   ClassTag      \

# define MRT            static __inline ULONG
# define METHODMSGFN(x) Class *cl, Object *o, x msg, SW_IDATA *idata
# define METHOD(n,x)    MRT n##_F(Class *cl,Object *o, x msg,SW_IDATA *idata) \
                          SDST(ULONG)  \
                          SWDB_OBJMSG(__FUNC__, o)
# define METHOD_EXTERN(n,x) ULONG n##_F(Class *cl,Object *o, \
                                          x msg,SW_IDATA *idata)
# define METHOD_NOINLINE(n,x) ULONG n##_F(Class *cl,Object *o, \
                                          x msg,SW_IDATA *idata) \
                          SDST(ULONG)  \
                          SWDB_OBJMSG(__FUNC__, o)

# define M_NOTIMPL(fn)   static __inline ULONG fn(METHODMSGFN(Msg)) \
                        SDST(ULONG) SWDB_OBJMSG(__FUNC__, o) \
                        { SWDB_RET(FAILURE); } SDFT

# define SW_DO_M(m,f)   case m: SWDB_RET(f(cl, o, (void *)msg, idata))

# define SW_DO_MF(m)    case m: SWDB_RET(m##_F(cl, o, (void *)msg, idata))

# define SWM_P(n,c)     typedef struct n { ULONG MethodID; c; } n

# define SWCLASSSUFFIX  ".library"

# define SW_DOGETI(a,s) case a:SWDB_OBJMSGI(s,o,(ULONG)idata->a); \
                               SWDB_RET((ULONG)idata->a)

# define SW_DOGETS(a,s) case a:SWDB_OBJMSGS(s,o,(ULONG)idata->a); \
                               SWDB_RET((ULONG)idata->a)

# define SW_DOSETS(a,s,m) case a:SWDB_OBJMSGS(s,o,(ULONG)Data); \
                                 strncpy(idata->a, (UBYTE *)Data, (m)); \
                                 SWDB_RET(TRUE)

# define SW_DOGETX(a,s) case a:SWDB_OBJMSGX(s,o,(ULONG)idata->a); \
                               SWDB_RET((ULONG)idata->a)

# define SW_DOSETI(a,s) case a:SWDB_OBJMSGI(s,o,Data);idata->a=(LONG)Data; \
                               SWDB_RET(TRUE)

# define SW_DOSETX(a,s) case a:SWDB_OBJMSGX(s,o,Data);idata->a=(void *)Data; \
                               SWDB_RET(TRUE)

# define SW_DOGETA(a,s,p) case a:SWDB_OBJMSGI(s,o,(ULONG)idata->a[(ULONG)p]); \
                                 SWDB_RET((ULONG)idata->a[(ULONG)p])

# define SW_DOSETA(a,s,i) case a:SWDB_OBJMSGI(s,o,Data);         \
                                 idata->a[(ULONG)idata->i]=Data; \
                                 SWDB_RET(TRUE)

# define SW_DOSETV(a,s,v) case a:SWDB_OBJMSGI(s,o,Data); \
                                 idata->a=v; SWDB_RET(TRUE)


// -- basic attribute information for classes --------------------------------


typedef struct _sw_attrinfo {
   TagItem   Attribute;           // the attribute & default value
   ULONG     Offset;              // offset into the instance private data
   ULONG     Size;                // size of item
   char     *RequestClass;        // a class to input this attr's value
   Tag       RequestAttr;         // an attribute to request from the class
   BOOL     (*CopyMethod)();      // how to copy the attribute: NULL=default
   char     *Description;         // the textual attribute description
} SW_AttrInfo;

# define SW_MUISER  5696
# define SW_MUISERS "S5697_"
# define SW_MUIEXT  ".mcc"


// -- useful text macros -----------------------------------------------------

# define SWX_B(t) MUIX_B t MUIX_N
# define SWX_I(t) MUIX_I t MUIX_N
# define SWX_U(t) MUIX_U t MUIX_N


// -- macro to iterate through an object list --------------------------------

# define SW_GETLH(o,lh)      lh=NULL;                             \
                             if (!getlent(o, SWA_ObjNoChildCopy)) \
                                get(o, MUIA_Group_ChildList, &(lh))

# define SW_LISTLOOP(lo,s,o) if (lo) \
                               for((s)=(lo)->lh_Head;(o)=NextObject(&(s));)

# define SW_MUIListLoop(l, pos, obj, c) \
         for ((pos)=0; DoMethod((l), MUIM_List_GetEntry,(pos),&(obj)), \
              (obj)&&(c); (pos)++)

# define SW_MUIListLoopSel(l, pos, obj, c)                    \
         for ((pos)=MUIV_List_NextSelected_Start;             \
              (DoMethod((l), MUIM_List_NextSelected, &(pos)), \
               (pos) != MUIV_List_NextSelected_End) &&        \
              (DoMethod((l), MUIM_List_GetEntry, (pos), &(obj)), (c));)

# define SW_CLOSELIB(l) { CloseLibrary((void *)l); (l) = NULL; }

# define SW_STRNCPY(t,f,n) { strncpy((char *)(t),(char *)(f),(n)-1); \
                             ((char *)(t))[(n)-1]='\0'; }

# define SW_WRITE(f,i)    (FWrite(f,(STRPTR)&(i),sizeof(i),1) != 1)
# define SW_READ(f,i)     (FRead (f,(STRPTR)&(i),sizeof(i),1) != 1)

# define SW_WRITESTR(f,i,t) (t=strlen(i), SW_WRITE(f, t) || \
                             (FWrite(f,(STRPTR)(i),t+1,1)!=1))

# define SW_READSTR(f,i)    { ULONG t; \
                              SW_READ(f, t); \
                              if (i) FreeVec(i); i = AllocVec(t+1, MEMF_ANY);\ 
                              if (i) FRead(f,(STRPTR)(i),t+1,1); \
                            }

# define SW_NOTIFYSAME(o1,a1,o2,a2) \
     DoMethod(o1,MUIM_Notify,a1,MUIV_EveryTime,o2,3, \
              MUIM_NoNotifySet,a2,MUIV_TriggerValue)

# define SW_NOTIFYSAMEUPD(o1,a1,o2,a2) \
     DoMethod(o1,MUIM_Notify,a1,MUIV_EveryTime,o2,3, \
              MUIM_Set,a2,MUIV_TriggerValue)

# define SW_KILLNOTIFYSAMEUPD(o1,a1,o2,a2) \
         DoMethod(o1, MUIM_KillNotifyObj, a1, o2)

# define SW_NOTIFYUPD(o1,a1,o2,a2)        \
         { SW_NOTIFYSAMEUPD(o1,a1,o2,a2); \
           SW_NOTIFYSAMEUPD(o2,a2,o1,a1); }

# define SW_KILLNOTIFYUPD(o1,a1,o2,a2)        \
         { SW_KILLNOTIFYSAMEUPD(o1,a1,o2,a2); \
           SW_KILLNOTIFYSAMEUPD(o2,a2,o1,a1); }

# define SW_NOTIFYSINGLE(o1,a1,v1,o2,a2,v2) \
     DoMethod(o1,MUIM_Notify,a1,v1,o2,3,MUIM_NoNotifySet,a2,v2)

# define SW_NOTIFYCLOSEWIN(w) \
     DoMethod((w),  MUIM_Notify, MUIA_Window_CloseRequest, TRUE, \
             MUIV_Notify_Self, 3, MUIM_Set, MUIA_Window_Open, FALSE);

# define SW_NOTIFYQUIT(w) \
     DoMethod(w, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, App, 2, \
              MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

# define SW_STRISNULL(s) (!(s) || !((s)[0]))
# define SW_REMOVENL(s) if ((s)[0]) (s)[strlen(s)-1]='\0'

# define SW_STRDEF(s, def) (SW_STRISNULL(s) ? (def) : (s))

# define SRK_EMAIL     "srk@frii.com"

# define nfset(o, a, v) SetAttrs(o, a, v, MUIA_Group_Forward, FALSE, TAG_DONE)

# define SW_MaxClassNameLen 48

// -- library bases for our library ------------------------------------------

# define SW_LIBSTRUCTVER 0

  typedef struct _swlibs {
     Library *SW_AslBase;
     Library *SW_CyberGfxBase;
     Library *SW_IFFParseBase;
     Library *SW_LocaleBase;
     Library *SW_RexxSysBase;
     Library *SW_DataTypesBase;
     Library *SW_CxBase;
  } SWLibraries;
 
# define SWAslName       "asl.library"
# define SWCyberGfxName  "cybergraphics.library"
# define SWDOSName       "dos.library"
# define SWGfxName       "graphics.library"
# define SWIFFParseName  "iffparse.library"
# define SWIntuitionName "intuition.library"
# define SWLocaleName    "locale.library"
# define SWRexxSysName   "rexxsyslib.library"
# define SWSysName       "exec.library"
# define SWUtilityName   "utility.library"
# define SWCxName        "commodities.library"
# define SWLayersName    "layers.library"
# define SWCxName        "commodities.library"
# define SWDTName        "datatypes.library"


// -- macros to manipulate flags ---------------------------------------------

# define FIsSet(flgs,f)    ((flgs)&(f))           // flags are set?(some)
# define FHaveAll(flgs,f)  (FIsSet(flgs,f)==(f))  // flags are set?(all)
# define FHaveNone(flgs,f) (FIsSet(flgs,f)==0)    // flags are not set?
# define FSet(flgs,f)      ((flgs)|=(f))          // set flags
# define FReset(flgs,f)    ((flgs)&=~(f))         // reset flags


// -- type return macros for GetTagData --------------------------------------

# define GetTagTyp(x,y,z,t) ((t)GetTagData(x,(ULONG)y,(struct TagItem *)(z)))
# define GetTagL(x,y,z)   GetTagTyp(x, y, z, LONG)
# define GetTagUL(x,y,z)  GetTagTyp(x, y, z, ULONG)
# define GetTagUB(x,y,z)  GetTagTyp(x, y, z, UBYTE *)
# define GetTagStr(x,y,z) GetTagTyp(x, y, z, STRPTR)
# define GetTagOB(x,y,z)  GetTagTyp(x, y, z, Object *)
# define GetTagObj(x,y,z) GetTagTyp(x, y, z, Object *)


// -- define some MUI stuff --------------------------------------------------

#  define _between(a,x,b) ((x)>=(a) && (x)<=(b))
#  define _isinobject(o,x,y) (_between(_mleft(o),(x),_mright(o)) && \
                              _between(_mtop(o),(y),_mbottom(o)))

#  ifdef MUIMASTER_NAME
#    define ClassT struct   MUI_CustomClass
#    define SW_GETICLASS(c) (((ClassT *)c)->mcc_Class)
#  else
#    define ClassT Class
#    define SW_GETICLASS(c) (c)
#  endif

# endif // S_CLASSES_INCLUDE
