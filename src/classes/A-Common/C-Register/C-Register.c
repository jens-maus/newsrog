# define  NOLIBS

# include "Libraries/C-Register.h"
# include "C-Register-I.h"

extern ClassT *SW_CLASS;

# include "C-Register-Mthd.h"


/*****************************************************************************
 *   Set up the root object's instance data                                  *
 *****************************************************************************/


#define SW_SETUP
static __asm __inline ULONG SWSetup(SETUPFNPROTO)
{
   geta4();

   // - initialize instance data variables -----------------------------------

   memset(idata, 0, sizeof(SW_IDATA));

   idata->SWA_Registered    = REG_NotRegistered;

   // - normal return --------------------------------------------------------

   return retval;
}


/*****************************************************************************
 *   Handle the set methods for attributes this class understands.           *
 *****************************************************************************/

#define SW_SET

static __asm __inline ULONG SWSet(SETFNPROTO)
{
   SW_IDATA *idata = INST_DATA(cl, o);
   
   switch (Tag) {
     case SWA_RequiresKey:
      idata->SWA_RequiresKey = (ULONG)Data;
      idata->SWA_Registered  = REG_Registered;
      break;

     case SWA_KFSerOnly:
      idata->SWA_KFSerOnly   = (ULONG)Data;
      break;
   }

   return (ULONG)DoSuperMethodA(cl, o, msg);
}


/*****************************************************************************
 *   Handle the get methods for attributes this class understands.           *
 *****************************************************************************/

#define SW_GET
static __asm __inline ULONG SWGet(GETFNPROTO)
{
   ULONG Val, Found=FALSE;
   SW_IDATA *idata = INST_DATA(cl, o);

#  define SW_DOGET(a)    case a: Val = (ULONG)idata->a; Found=TRUE; break;
#  define SW_DOGETF(a,f) case a: Val = (ULONG) f;       Found=TRUE; break;

   switch (((opGet *)msg)->opg_AttrID) {
      SW_DOGET (SWA_Registered);
      SW_DOGET (SWA_RegID);
      SW_DOGET (SWA_RegLastName);
      SW_DOGET (SWA_RegFirstName);
      SW_DOGETF(SWA_RegStatus,    GenRegStatus(idata));
      SW_DOGETF(SWA_RegIsPirated, IsPirated(idata));
      SW_DOGETF(SWA_RegLibVer,    10);
   }

   if (Found) {
      if (((ULONG *)((opGet *)msg)->opg_Storage))
         *((ULONG *)((opGet *)msg)->opg_Storage) = Val;
      return 1;
   }

#  undef SW_DOGET
#  undef SW_DOGETF

   return (ULONG)DoSuperMethodA(cl, o, msg);
}



/*****************************************************************************
 *   Handle the methods this class understands.                              *
 *****************************************************************************/

#define SW_METHOD
static __asm __inline ULONG SWMethod(METHODFNPROTO)
{
   SW_IDATA *idata = INST_DATA(cl, o);

   putreg(REG_A6, (long)cl->cl_UserData);
   geta4();

   switch (msg->MethodID) {
      SW_DO_MF(SWM_RegCheck);
      SW_DO_MF(SWM_RegisterUI);
      SW_DO_MF(SWM_Reg_Banner);
   }

   return (ULONG)DoSuperMethodA(cl, o, msg);
}


/*****************************************************************************
 *   Register/Unregister class functions                                     *
 *****************************************************************************/

static ClassT *RegisterClassPtr;

# define SW_USERSETUP RegisterClassPtr = (ClassT *)SW_CLASS;

Class *GetRegisterClass(void) { return SW_GETICLASS(RegisterClassPtr); }

# include "S-Dispatcher.h"
