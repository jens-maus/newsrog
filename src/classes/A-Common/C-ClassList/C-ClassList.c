# include "C-ClassList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_ClassList


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_B "Name",
   MUIX_B "Description",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# include "C-ClassList-Mthd.h"

/*****************************************************************************
 * Setup method                                                              *
 *****************************************************************************/

#define SW_SETUP
static __asm __inline ULONG SWSetup(SETUPFNPROTO)
   SDST(ULONG)
   SWP_XX(o, idata)
   SWDB_ASSERTNULL(o)
   SWDB_ASSERTNULL(idata)
{
   memset(idata, 0, sizeof(SW_IDATA));           // clear our data space

   SetAttrs((Object *)retval,
            MUIA_List_Format,      "MAW=25 W=25,MAW=75 W=75",
            MUIA_List_DisplayHook, &SW_CList_DspFnHook,
            MUIA_List_CompareHook, &SW_CList_CmpFnHook,
            MUIA_List_Title,       TRUE,
            TAG_DONE);

   strcpy(idata->Type, GetTagStr(SWA_ClassList_Type,    "#?Ext#?",         ti));
   strcpy(idata->Path, GetTagStr(SWA_ClassList_RegPath, "PROGDIR:RegInfo", ti));
      
   // -- so our display hook can find localized strings ----------------------

   SW_CList_DspFnHook.h_Data = (void *)getreg(REG_A4);

   if (GetTagUL(SWA_ClassList_Immediate, FALSE, ti))
      LoadRegDB((Object *)retval, idata);

   SWDB_RET(retval);
} SDFT


/*****************************************************************************
 *   Dispose the root object's instance data                                 *
 *****************************************************************************/

#define SW_DISPOSE
static __asm __inline void SWDispose(DISPOSEFNPROTO)
   SDSV
   SWP_XX(o, idata)
   SWDB_ASSERTNULL(o)
   SWDB_ASSERTNULL(idata)
{
   ClassInfo *ci;

   if (idata->Setup)
      while (ci = (ClassInfo *)RemHead(&idata->ClassList)) FreeVec(ci);
} SDFV


#  define SW_NOROOTMETHODS
#  define SW_Methods                    \
   SW_DO_MF(SWM_ClassList_MakeObj);     \
   SW_DO_MF(SWM_ClassList_MakeObjNew);  \
   SW_DO_MF(SWM_ClassList_GetAttr);     \
   SW_DO_MF(SWM_ClassList_InfoLib);     \
   SW_DO_MF(SWM_ClassList_InfoObj);     \
   SW_DO_MF(SWM_ClassList_GoodType);    \
   SW_DO_MF(SWM_ClassList_GoodTypeObj); \
   SW_DO_MF(MUIM_Setup);                \

# include "S-DefMethod.h"
# include "S-Dispatcher.h"
