
# ifndef C_REGISTER_INCLUDE
#   define C_REGISTER_INCLUDE

#   include "S-Classes.h"
#   include "S-ClassNames.h"


    // -- messages that this class understands -------------------------------


#   define FIRSTTAG         (LONG)(TAG_USER + 1000)
    
    enum {
       SWA_Registered = FIRSTTAG,               // [ G ] registration status
       SWA_RegID,                               // [ G ] registration ID #
       SWA_RegLastName,                         // [ G ] registration lastname
       SWA_RegFirstName,                        // [ G ] registration firstname
       SWA_RegStatus,                           // [ G ] full reg status
       SWA_RequiresKey,                         // [ G ] requires key
       SWA_KFSerOnly,                           // [ G ] KF has ser # only
       SWA_RegIsPirated,                        // [ G ] Is pirated?
       SWA_RegLibVer,                           // [ G ] reg lib version
    };
    
    enum {
       SWM_RegCheck = FIRSTTAG,                 // check for registration
       SWM_RegisterUI,                          // show registration UI
       SWM_Reg_Banner,                          // show unregistered banner
    };

    SWM_P(SWP_CheckReg,  UBYTE *Name; UBYTE *Prog);
    SWM_P(SWP_RegUI,     Object *App; UBYTE *Prog; ULONG Cost1; ULONG Cost2);
    SWM_P(SWP_RegBanner, Object *App; Object *Win; UBYTE *Msg);

    enum {
       SWF_CryptLineSize       = 48,            // crypt line size
       SWF_CryptInitVal        = 198762181,     // initial crypt value
       SWF_MinRegID            = 100000,        // min registration ID
       SWF_MaxRegID            = 10000000,      // max registration ID
       SWV_IsPirate            = 0x0BAD1937,    // for pirated copies
       SWV_KCOffset            = 0xbef,         // for checking pirated keys
    };

#   define SWF_KeyArray1   "iqmzorjbt291*^!%"
#   define SWF_KeyArray2   "ayn483u6wvf57+?$"
#   define SWF_KeyArray3   "&dkg<hl@:>#.esxp"
#   define SWF_KeyArrayNum 3
#   define SWF_KeyV1       129121316
#   define SWF_KeyV2       813219211
#   define SWF_KeyV3       132509518

    enum {
       REG_NotRegistered = 1,
       REG_Registered,
       REG_InvalidKey,
    };

#   undef FIRSTTAG

    Class * GetRegisterClass(void);

# endif
