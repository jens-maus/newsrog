# ifndef C_REGISTER_INCLUDE_I
#   define C_REGISTER_INCLUDE_I

    // -- this classes names, etc --------------------------------------------
  
#   define SW_CLASS            SWRegister
#   define SW_CLIB             SWC_Register
#   define SW_CNAME            "Register"
#   define SW_SUPERPUB         MUIC_Area
#   define SW_SUPERPRI         NULL

#   include "S-Params.h"
#   include "S-Debug.h"

#   undef  SW_ATTRMAP
#   undef  SW_CATALOG
#   undef  SW_CATSTR
#   undef  SW_MSGSTR

    // -- our instance specific data -----------------------------------------
  
#   ifdef SW_IDATA
    typedef struct SW_IDATA
    {
       struct {
          ULONG   SWA_Registered;                       // registration state
          LONG    SWA_RegID;                            // registration ID #
          UBYTE   SWA_RegLastName[SWF_CryptLineSize];   // registration last
          UBYTE   SWA_RegFirstName[SWF_CryptLineSize];  // registration first
          UBYTE   SWA_RegLine[SWF_CryptLineSize*3];     // reg description
          ULONG   SWA_RequiresKey;                      // needs keyfile?
          ULONG   SWA_KFSerOnly;                        // only ser number

          Object  *RegWin,
                  *Gads[16],
                  *B_Generate,
                  *B_Cancel;

       } InternalInfo;

    } SW_IDATA;
#   endif

#   define PsRand(val,p1,p2,p3)  (val = (val * 73 + p1*3 + p2*5 + p3*7))

#   include "Proto/Register_protos.h"

# endif
