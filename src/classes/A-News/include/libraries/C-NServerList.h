# ifndef C_NSERVERLIST_INCLUDE
#  define C_NSERVERLIST_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-NServer.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10400)

   enum {
      SWA_NServerList_UNUSED = SW_FIRSTTAG,     // UNUSED
      SWA_NServerList_HostStrObj,               // [  I] use for host string
      SWA_NServerList_Tiny,                     // [  I] tiny mode
      SWA_NServerList_ListCol,                  // [SGI] list columns
      SWA_PopSrvList_AttrUI,                    // Attr UI to attach to
      SWA_PopSrvList_SrvAttr,                   // Server attribute in AttrUI
      SWA_PopSrvList_SrvObj,                    // object that owns SrvAttr
      SWA_PopSrvList_CopyOnDisp,                // T to copy on dispose
      SWA_PopSrvList_DefBt,                     // T to use default button
   };

   enum {
      SWM_NServerList_ResetDefault = SW_FIRSTTAG, // set default server
      SWM_NServerList_GetDefault,               // get default server
      SWM_NServerList_NNTPSERVER,               // make server for env var
      SWM_NServerList_PopSrvListClass,          // get PopSrvList class
      SWM_PopSrvList_FixHS,                     // fix server stuff
      SWM_PopSrvList_SetDef,                    // set default server
      SWM_NServerList_FindCreate,               // find serv, else create
   };

   SWM_P(SWP_NServerList_FindCreate,            // find/create server
         STRPTR ServName;                       // server name
         ULONG  ServType);                      // server type (NNTP/FTP)

   SWM_P(SWP_NServerList_NNTPSERVER,            // make server from on var
         ULONG CheckErr);                       // T to report errors

   enum {
      SWT_NServerList_ServerName = SW_FIRSTTAG, // server name for sending
   };

#  define NServerListObject MUI_NewObject(SWC_NServerList

#  define PopSrvListObject \
   NewObject((struct IClass *)DoMethod(NN_ServList(idata), \
                                       SWM_NServerList_PopSrvListClass), NULL

#  undef SW_FIRSTTAG

# endif
