
# ifndef CX_POPUP
#   define CX_POPUP TRUE
# endif

# ifdef SWNETLOOP
#   include <sys/types.h>
#   include <proto/socket.h>
#   include "Libraries/C-NTCPList.h"
#   define SWLPPROTO Object *App, Object *Win, Object *Top, Object *TCPList, \
                     ULONG Popup
# else
#   define SWLPPROTO Object *App, Object *Win, Object *Top, ULONG Popup
# endif

static void SWEventLoop(SWLPPROTO)
{
   set(Win, MUIA_Window_Open, Popup);

#  ifdef POSTOPEN
      POSTOPEN
#  endif

#  ifdef SWNETLOOP
      // -- networked event loop - gotta multiplex sockets too ---------------
      {
        ULONG     RetID, sigs        = 0;
        struct Library  *SocketBase = NULL;
        
        while ((RetID = DoMethod(App, MUIM_Application_NewInput, &sigs)) !=
               MUIV_Application_ReturnID_Quit) {

           switch (RetID) {
             case SWV_NTCPList_Online_On:
              if (!SocketBase)
                 SocketBase = OpenLibrary("bsdsocket.library", 4);
              break;

             case SWV_NTCPList_Online_Off:
              SW_CLOSELIB(SocketBase);
              break;
           }

           if (sigs) {
              fd_set rfds, wfds;
              ULONG  count;
              
              sigs |= SIGBREAKF_CTRL_C;
              if (SocketBase) {
                 ULONG maxsocket;

                 maxsocket = TCPList ?
                 DoMethod(TCPList, SWM_NTCPList_MaxSocket, &rfds, &wfds)+1 : 0;

                 count = WaitSelect(maxsocket, &rfds, &wfds, NULL, NULL, &sigs);
              } else {
                 sigs  = Wait(sigs);
                 count = 0;
              }
              
              if (count && TCPList)
                 DoMethod(TCPList, SWM_Root_ListDoMethod,
                          SWGetObj(TCPList, SWA_NList_L),
                          SWV_ListDoMethod_All, SWV_ListDoMethod_Entry, 3,
                          SWM_NTCP_RecvAsync, &rfds, &wfds);

              if (sigs & SIGBREAKF_CTRL_C) break;
          }
        }

        CloseLibrary(SocketBase);
      }

#  else
      {
        ULONG sigs = 0;
        while (DoMethod(App, MUIM_Application_NewInput, &sigs) !=
               MUIV_Application_ReturnID_Quit) {
          if (sigs) {
            sigs = Wait(sigs | SIGBREAKF_CTRL_C);
            if (sigs & SIGBREAKF_CTRL_C) break;
          }
        }
      }
#  endif


#  ifdef PRECLOSE
      PRECLOSE
#  endif

   set(Win, MUIA_Window_Open, FALSE);
}

# undef SWLPPROTO
