
# define SWP_SysCmdAttrVals(x) \
     x##_Command,              \
     x##_Args,                 \
     x##_Input,                \
     x##_Output,               \
     x##_Stack,                \
     x##_Pri,                  \
     x##_Dir,                  \
     x##_ASync

# define SWP_SysCmdAttrDefs(x) \
     STRPTR x##_Command;       \
     STRPTR x##_Args;          \
     STRPTR x##_Input;         \
     STRPTR x##_Output;        \
     ULONG  x##_Stack;         \
     LONG   x##_Pri;           \
     STRPTR x##_Dir;           \
     ULONG  x##_ASync

# define SWP_SysCmdAttrs(x,cmd,args,async)                               \
   SWAM_FR (x##_Command,  SWS_L_SCCmd,    SWAM_UI,  cmd,   FMSIZE),      \
   SWAM_DSS(x##_Args,     SWS_L_SCArgs,   SWAM_UI,  args,   256),        \
   SWAM_FR (x##_Input,    SWS_L_SCInput,  SWAM_UI,  "NIL:", FMSIZE),     \
   SWAM_FR (x##_Output,   SWS_L_SCOutput, SWAM_UI,  "NIL:", FMSIZE),     \
   SWAM_IB (x##_Stack,    SWS_L_SCStack,  SWAM_UI, 16000, 1000, 100000), \
   SWAM_IB (x##_Pri,      SWS_L_SCPri,    SWAM_UI,  0,    -127, 127),    \
   SWAM_DSS(x##_Dir,      SWS_L_SCDir,    SWAM_UI|SWAM_PopDir,  NULL,   FMSIZE), \
   SWAM_BO (x##_ASync,    SWS_L_SCASync,  SWAM_UI,  async)

# define SWP_SysCmdHelpLabs \
      SWS_H_SCCommand, \
      SWS_H_SCArgs, \
      SWS_H_SCInput, \
      SWS_H_SCOutput, \
      SWS_H_SCStack, \
      SWS_H_SCPri, \
      SWS_H_SCDir, \
      SWS_H_SCASync

# define SWP_SysCmdHelp(x)          \
      x##_Command, SWS_H_SCCommand, \
      x##_Args,    SWS_H_SCArgs,    \
      x##_Input,   SWS_H_SCInput,   \
      x##_Output,  SWS_H_SCOutput,  \
      x##_Stack,   SWS_H_SCStack,   \
      x##_Pri,     SWS_H_SCPri,     \
      x##_Dir,     SWS_H_SCDir,     \
      x##_ASync,   SWS_H_SCASync

# define SWP_SysCmdStrs \
   "Command:",          \
   "Args:",             \
   "Input File:",       \
   "Output File:",      \
   "Stack Size:",       \
   "Task Pri:",         \
   "Current Dir:",      \
   "ASync:"

# define SWP_SysCmdLabs \
      SWS_L_SCCmd, \
      SWS_L_SCArgs, \
      SWS_L_SCInput, \
      SWS_L_SCOutput, \
      SWS_L_SCStack, \
      SWS_L_SCPri, \
      SWS_L_SCDir, \
      SWS_L_SCASync
