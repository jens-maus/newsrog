
// -- support SW class library name and version ---------------------------------

# ifndef L_SUPPORT_INCLUDE
#   define L_SUPPORT_INCLUDE

#   ifndef SWSUPPORTNAME
#      define SWSUPPORTNAME SW_MUISERS "Support.library"
#      define SWSUPPORTVER  6
#   endif
  
    extern Library *SWSupportBase;
  
#   define TAG_STD 1000
  
#   include "S-Libs.h"
  
#   ifdef SWSEMAPHOREFNS
#      define GetSemEx(o) SWObtainSemaphore(o);
#      define GetSem(o)   SWObtainSemaphoreShared(o);
#      define RelSem(o)   SWReleaseSemaphore(o)
#   else
#      define GetSemEx(o)
#      define GetSem(o)
#      define RelSem(o)
#   endif

#   define SWSetupUINotify(o, e, Attr, EditA, SetA) \
      SW_NOTIFYSAME(o, Attr, e, EditA); \
      DoMethod(e, MUIM_Notify, SetA, MUIV_EveryTime, \
               o, 6, SWM_NoForwardSet, SWA_ObjNoNotify, TRUE, \
               Attr, MUIV_TriggerValue, TAG_DONE);

    // -- subtask support ----------------------------------------------------
    struct SubTaskMsg
    {
       struct Message stm_Message;
       WORD           stm_Command;
       APTR           stm_Parameter;
       LONG           stm_Result;
    };

    struct SubTask
    {
       struct Task      *st_Task;    // sub task pointer
       struct MsgPort   *st_Port;    // allocated by sub task
       struct MsgPort   *st_Reply;   // allocated by main task
       APTR              st_Data;    // more initial data to pass to the sub task
       struct SubTaskMsg st_Message; // Message buffer
    };

    enum {
       STC_Startup  = -2,
       STC_Shutdown = -1,
    };

    typedef struct _HelpMap { ULONG Attr, Label; } HelpMap;

    enum {
       SWA_ObjString_NumVal    = (LONG)TAG_USER,// numeric value
       SWA_ObjString_StrAttr,
       SWA_TimeSlider_NoDiv1000,               // [  I] seconds, not millis
       SWA_ObjString_NumValRet,                // like numval, but only on rtn
       SWA_TimeSlider_NumIncMult,              // [  I] numeric increase multiple
       SWA_KbSlider_NumIncMult,                // [  I] numeric increase multiple
       SWA_KbSlider_Bytes,                     // [  I] bytes instead of Kb
    };

    enum {
       SWM_ObjString_AppMsg    = (LONG)TAG_USER,

       SWM_ObjList_DragQuery,
       SWM_ObjList_DragDrop,
    };

    SWM_P(SWP_AppMsg, struct AppMessage *AMsg);
    SWM_P(SWP_ObjList_DragQuery, Object *obj; Object *target);
    SWM_P(SWP_ObjList_DragDrop,  Object *obj; LONG x; LONG y; Object *target);

#   define ObjListObject   NewObject(SWObjListClass(), NULL
#   define ObjStringObject NewObject(SWObjStringClass(), NULL

    enum {
       SWA_ObjList_Object      = (LONG)TAG_USER,
    };

    enum {
       SWV_UUDecode_OK           = 0,
       SWV_UUDecode_Err_BadCS,
       SWV_UUDecode_Err_BadSize,
       SWV_UUDecode_Err_BadLL,
       SWV_Base64_Err_BadChar,
       SWV_Err_Malloc,
       SWV_Err_FOpen,
       SWV_Err_Read,
       SWV_Err_Write,
    };

#   include "S-Debug.h"

#   define SWGetType(o,a,t)      ((t)(SWGetVal(o,a)))
#   define SWGetUL(o,a)          SWGetType(o,a,ULONG)
#   define SWGetL(o,a)           SWGetType(o,a,LONG)
#   define SWGetLong(o,a)        SWGetType(o,a,LONG)
#   define SWGetStr(o,a)         SWGetType(o,a,UBYTE *)
#   define SWGetObj(o,a)         SWGetType(o,a,Object *)

#   define SWAttrEqUL(o1,o2,a)      (SWGetUL(o1,a)==SWGetUL(a2,a))
#   define SWAttrEqValUL(o,a,v)     (SWGetUL(o,a)==(v))
#   define SWAttrStrcmp(o1,o2,a)    (strcmp(SWGetStr(o1,a),SWGetStr(o2,a)))
#   define SWAttrStrcmpVal(o,a,v)   (strcmp(SWGetStr(o,a),v))
#   define SWAttrEqStr(o1,o2,a)     (!SWAttrStrcmp(o1,o2,a))
#   define SWAttrEqValStr(o,a,v)    (!SWAttrStrcmpVal(o,a,v))

    // -- pattern bounds support ---------------------------------------------

#   define PB_MaxS 64
                                   
    struct PatBounds
    {
       LONG Case;
       LONG Wild;
       LONG PatLen;

       STRPTR PatBuf0;     // for pattern with    trailing *
       STRPTR PatBuf1;     // for pattern without trailing *

       TEXT   B0[PB_MaxS], B1[PB_MaxS];
    };

    // -- return TRUE if this char needs QP encoding -------------------------

    enum {
       SWV_QPEncStr_BuffLen = 78
    };

    // -- can this char be represented as itself in QP? ----------------------

#   define QPTransparent(c)   ((c) == 9 || (c)==32    || \
                               (c) >= 33 && (c) <= 60 || \
                               (c) >= 62 && (c) <= 126)

#   define QPNeedsEncoding(c) (!QPTransparent(c))
                                     
#   ifndef SW_NO_PROTOS
#      include "Proto/Support_protos.h"
#   endif

#   ifndef SW_NO_PRAGMAS
#      include "Pragmas/Support_pragmas.h"
#   endif

# endif
