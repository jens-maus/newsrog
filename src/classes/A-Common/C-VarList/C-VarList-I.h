# ifndef C_VarList_INCLUDE_I
#  define C_VarList_INCLUDE_I

#  define SW_NListTrackAttrNum 2

#  include "Libraries/C-Root.h"
#  include "Libraries/S-Lists.h"
#  include "Libraries/C-VarList.h"

   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_VarList
#  define SW_CNAME            "VarList"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/VarList_protos.h"
#  include <ctype.h>


   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_L_Var= 1,
      SWS_L_Val,

      SWS_L_ValLab,
      SWS_B_Add,
      SWS_B_Del,
      SWS_B_Sort,

      SWS_H_List,
      SWS_H_Add,
      SWS_H_Del,
      SWS_H_Sort,

      SWS_L_Error,

      SWS_L_UnsetVar,
      SWS_L_UnsetVal,

      SWS_L_EVar,
      SWS_L_EVal,

      SWS_L_WeekdayShort,
      SWS_L_Weekday,
      SWS_L_MonthShort,
      SWS_L_Month,
      SWS_L_DayNum,
      SWS_L_DayNumSp,
      SWS_L_Hour24,
      SWS_L_Hour12,
      SWS_L_MonthNum,
      SWS_L_Min,
      SWS_L_AmPm,
      SWS_L_Hour24Sp,
      SWS_L_Hour12Sp,
      SWS_L_Sec,
      SWS_L_Year2,
      SWS_L_Year4,

      SWS_L_ChipKb,
      SWS_L_FastKb,
      SWS_L_AllKb,
      SWS_L_LargestKb,
      SWS_L_ChipMb,
      SWS_L_FastMb,
      SWS_L_AllMb,
      SWS_L_LargestMb,
      
      SWS_Err_UnterminatedVar,
      SWS_Err_TooManyReplacements,
      SWS_Err_VarNameLength,
      SWS_Err_NoExpansion,
      SWS_Err_Malloc,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"


   // -- our instance specific data ------------------------------------------

   enum {
      SWV_VarList_MaxVarLen    = 255,              // max variable name length
      SWV_VarList_MaxIter      = 256,              // max replacement iterations
      SWV_VarList_IncSize      = 24,               // increment size for var repl
      SWV_VarList_FnBuffSize   = 512,              // fn   buffer size
      SWV_VarList_Key1         = (LONG)0xFB14B371, // alloc key 1
      SWV_VarList_Key2         = (LONG)0x713BA946, // alloc key 2
   };

   enum {
      SWV_VL_Add,
      SWV_VL_Sub,
      SWV_VL_Mul,
      SWV_VL_Div,
      SWV_VL_Min,
      SWV_VL_Max,
      SWV_VL_And,
      SWV_VL_Or,
      SWV_VL_Not,
      SWV_VL_GT,
      SWV_VL_LT,
      SWV_VL_GE,
      SWV_VL_LE,
      SWV_VL_EQ,
      SWV_VL_NE,
      SWV_VL_Mod,
      SWV_VL_Xor,
   };

   typedef struct SWS_VarList_RS {
      UBYTE *Buff;
      ULONG Alloc;
      ULONG Size;
   } SWS_VarList_RS;

#  define s_Var       idata->SW_NListTRA[0].Gad
#  define s_VarAttr   idata->SW_NListTRA[0].ObjAttr
#  define s_Val       idata->SW_NListTRA[1].Gad
#  define s_ValAttr   idata->SW_NListTRA[1].ObjAttr

#  define SW_NList_CmpCode  return CmpVars(o1, o2)
#  define SW_NList_DispHook MUI_DisposeObject(o)

#  define SW_NList_NewObj MUI_NewObject(SWC_Attrval, TAG_DONE);

   typedef struct SW_IDATA
   {
      SWP_NList_AttrDefs;

      ULONG  ReplaceBuffSize;
      UBYTE *ReplaceBuff;                      // temporary replacement buffer
      UBYTE  BadVar[64];
      ULONG  FnBuffSize;
      UBYTE *FnBuff;
      ULONG  BuffPos;
      struct Hook DateHook;
      struct Locale *loc;

      STRPTR FuncParams;

      TEXT   VarBuf[SWV_VarList_MaxVarLen+sizeof(MUIX_PH)+1];
   } SW_IDATA;

#  define NOTP                MUIM_Notify, MUIA_Pressed, FALSE
#  define BEGINMARK(str, pos) ((str)[pos] == '$' && (str)[pos+1] == '{')
#  define ENDMARK(str, pos)   ((str)[pos] == '}')
#  define ESCMARK(str, pos)   ((str)[pos] == '\\')
#  define ISDIGSGN(c)         ((c) >= '0' && (c) <= '9' || (c) == '-')
#  define ISDIG(c)            ((c) >= '0' && (c) <= '9')

# endif // C_VarList_INCLUDE_I
