# include <proto/dos.h>
# include <proto/exec.h>
# include <proto/intuition.h>
# include <proto/utility.h>
# include <stdlib.h>
# include <string.h>
#  include <sys/socket.h>
#  include <sys/time.h>
#  include <sys/ioctl.h>
#  include <sys/errno.h>
#  include <proto/socket.h>
#  include <proto/timer.h>
#  include <devices/timer.h>
#  include <netdb.h>
#  include <netinet/ip.h>

struct Library *SocketBase = NULL;

main (int argc, char *argv[])
{
   struct sockaddr_in locaddr, srvaddr;
   LONG   ID;
   const LONG ONE = 1;
   
   if (!(SocketBase = OpenLibrary("bsdsocket.library", 4)))
      exit(5);

   memset(&locaddr, 0, sizeof(locaddr));
   memset(&srvaddr, 0, sizeof(srvaddr));

   if ((ID = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
      setsockopt(ID, SOL_SOCKET, SO_KEEPALIVE, &ONE, sizeof(ONE));
      setsockopt(ID, SOL_SOCKET, SO_REUSEADDR, &ONE, sizeof(ONE));

      srvaddr.sin_port        = htons(119);
      srvaddr.sin_addr.s_addr = htons((208<<24)|(146<<16)|(240<<8)|4);
      srvaddr.sin_family      = AF_INET;

      if (connect(ID, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) >= 0) {
         long len = sizeof(locaddr);
         if (getsockname(ID, (struct sockaddr *)&locaddr, &len) >= 0) {
            Printf("%ld, %08lx\n", locaddr.sin_port, locaddr.sin_addr.s_addr);
         }
      }

# ifdef wfewafw
      if (bind(ID, (struct sockaddr *)&addr, sizeof(addr)) >= 0) {
         if (listen(ID, 1) >= 0) {
            long len = sizeof(addr);

            if (getsockname(ID, (struct sockaddr *)&addr, &len) >= 0) {
               Printf("%ld, %ld\n", addr.sin_port, addr.sin_addr.s_addr);
            }
         }
      }
# endif
      
      shutdown(ID, 2);
      CloseSocket(ID);
   }
   
   CloseLibrary(SocketBase);
}
