# ifndef C_VARLIST_INCLUDE
#   define C_VARLIST_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"
#   include "Libraries/C-AttrVal.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10000)

   enum {
      SWA_VarList_Parent = SW_FIRSTTAG,      // lent - overrides MUIA_Parent

      SWA_VarList_Var    = SWA_Attrval_Attr,
      SWA_VarList_Val    = SWA_Attrval_Val,
      SWA_VarList_RO     = SWA_Attrval_Num,
   };

   enum {
      SWM_VarList_Search = SW_FIRSTTAG,      // search for a variable object
      SWM_VarList_ReplaceValues,             // replace variable values
      SWM_VarList_FreeString,                // free a SWS_Varlist_RS
      SWM_VarList_CpParam,                   // copy (maybe quoted) string
      SWM_VarList_FindParam,                 // find parameter, cp to buffer
      SWM_VarList_FindParamNum,              // find numeric parameter
      SWM_VarList_FindParamChr,              // find char parameter
      SWM_VarList_FindRest,                  // find part after params
   };

   SWM_P(SWP_VarList_Search,        STRPTR Name; Object *obj);
   SWM_P(SWP_VarList_ReplaceValues, STRPTR Str;  Object *obj);
   SWM_P(SWP_VarList_FreeString,    STRPTR Str;  Object *obj);

   SWM_P(SWP_VarList_FindParam,
         STRPTR Name; STRPTR Param; STRPTR Buff; ULONG BuffLen; STRPTR Def);

   SWM_P(SWP_VarList_FindParamNum,
         STRPTR Name; STRPTR Param; LONG Min; LONG Def; LONG Max);

   SWM_P(SWP_VarList_FindParamChr, STRPTR Name; STRPTR Param; LONG Def);

   SWM_P(SWP_VarList_FindRest,      STRPTR Name; ULONG *Quoted);

   SWM_P(SWP_VarList_CpParam, STRPTR Name; STRPTR Buff; ULONG BuffLen);

   // -- macros to make easier to use ----------------------------------------

#  define VL_FindRet(a, v) if (!strcmp(msg->Name, (a))) SWDB_RET((ULONG)(v));

#  define VL_FindRetNum(a, v, b, f) \
   if (!strcmp(msg->Name, a)) {     \
      sprintf((b), f, (v));         \
      SWDB_RET((ULONG)(b));         \
   }

#  undef SW_FIRSTTAG

# endif
