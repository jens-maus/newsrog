
# include "/Include/Libraries/C-Root.h"
# include "/Include/Libraries/C-NTCP.h"

# include "S-LibSetup.h"

# define ERR(x) Printf("ERROR: %s %s\n", __FUNC__, x)

# define GN 10

void test_connect(UBYTE *CName) SDSV
{
#  define CHOST "rtfm.mit.edu"
#  define DPORT 20
#  define CPORT 21

   Object *dtcp, *ctcp = NULL;

   if (!(dtcp = MUI_NewObject(CName,
                           SWA_NTCP_Host,    CHOST,
                           SWA_NTCP_Port,    DPORT,
                           SWA_NTCP_Timeout, 10,
                           TAG_DONE))) {
         ERR("failed allocating object");
         goto Done;
      }

   if (!(ctcp = MUI_NewObject(CName,
                           SWA_NTCP_Host,    CHOST,
                           SWA_NTCP_Port,    CPORT,
                           SWA_NTCP_Timeout, 10,
                           TAG_DONE))) {
         ERR("failed allocating object");
         goto Done;
      }

   if ((LONG)DoMethod(dtcp, SWM_NTCP_Connect) < 0) {
      ERR("Failed to connect");
      Printf((STRPTR)getlent(dtcp, SWA_ObjErrorText), CHOST, DPORT);
      goto Done;
   }

   if ((LONG)DoMethod(ctcp, SWM_NTCP_Connect) < 0) {
      ERR("Failed to connect");
      Printf((STRPTR)getlent(ctcp, SWA_ObjErrorText), CHOST, CPORT);
      goto Done;
   }

   Delay(60);
   
   if (DoMethod(ctcp, SWM_NTCP_SendStr, "CWD pub/usenet-by-group\r\n") != 0)
      ERR("send failed");
   Delay(120);
   
   if (DoMethod(ctcp, SWM_NTCP_RecvAsync, NULL) != 0) ERR("recv failed");
   if (DoMethod(dtcp, SWM_NTCP_RecvAsync, NULL) != 0) ERR("recv failed");

//   Printf("%ld %ld %ld\n",
//          SWGetUL(o, SWA_NTCP_SentBytes),
//          SWGetUL(o, SWA_NTCP_RecvBytes),
//          SWGetUL(o, SWA_NTCP_RecvLines));

 Done:
   MUI_DisposeObject(dtcp);
   MUI_DisposeObject(ctcp);
} SDFV

main (int argc, char *argv[])
{
   UBYTE b[32];
   
   test_connect(SWC_NTCP);

   exit(0);
}
