
# include <libraries/mui.h>
# include <proto/commodities.h>
# include <proto/muimaster.h>
# include <proto/icon.h>
# include <workbench/startup.h>

# include "Libraries/C-Root.h"
# include "Libraries/L-Support.h"
# include "Libraries/C-Register.h"
# include "Libraries/C-NNews.h"
# include "Libraries/C-DTImg.h"

# define  SWNETLOOP

# ifdef NR_BATCHMODE
#    include <netdb.h>
#    include <sys/socket.h>
#    include <netinet/tcp.h>
#    include <netinet/in.h>
#    include <fcntl.h>
# endif

# define LogoImage  "PROGDIR:Images/NRLogo"

# define BASETEMPLATE "SP=STARTPROJ/M,CX_PRI/N,TP=TASKPRI/N," \
                      "NP=NOPROJ/S,ON=ONLINEMODE/S,OFF=OFFLINEMODE/S," \
                      "NBI=NOBANNERIMG/S,NB=NOBANNER/S,BP=BUTTONPATH/K,USAGE=HELP/S"

# ifdef NR_BATCHMODE
#   define TEMPLATE BASETEMPLATE \
           ",SRV=SERVER/K,G=GROUP/K,MID=MSGID/K,MNUM=MSGNUM/N,H=HEAD/S,B=BODY/S,A=ART/S,CHK=CHECK/S," \
           "HDR=HEADER/K,LG=LISTGROUPS/K,LMSGID=LISTMSGIDS/S,ARTCOUNT/S,SRVHELP/S," \
           "LH=LISTHDRS/S,SN=SKIPNUMS/S,SEARCH/K,SR=SEARCHRES/K,MC=MATCHCASE/S,INVERT/S"

    enum {
       SWV_BanMode_Normal = 0,
       SWV_BanMode_NoImg,
       SWV_BanMode_NoBan,
    };

#   define NNTPPort 119

    // -- batch commands ------------------------------------------------------
   
    enum {
       SWV_BC_None = 0,
       SWV_BC_GetArt,
       SWV_BC_ListGrp,
       SWV_BC_ListMsgIDs,
       SWV_BC_TestGrp,
       SWV_BC_ArtCount,
       SWV_BC_SrvHelp,
       SWV_BC_ListHdrs,
       SWV_BC_Search,
    };

    // -- article fetch modes ------------------------------------------------

    enum {
       SWV_Get_Art = 0,
       SWV_Get_Head,
       SWV_Get_Body,
       SWV_Get_Stat,
       SWV_Get_Check,
    };
# else
#   define TEMPLATE BASETEMPLATE
# endif

# define CopyrightTxt APPNAME " " APPVERSTR "\n"           \
                      "© 1998-1999 ShadowWorks Software\n" \
                      "All Rights Reserved"                \

# define InitWinTxt "Initializing..."
# define TermWinTxt "Cleaning Up..."

enum {
   RA_STARTPROJ,
   RA_CX_PRI,
   RA_TASKPRI,
   RA_NOPROJ,
   RA_ONLINE,
   RA_OFFLINE,
   RA_NOBANIMG,
   RA_NOBAN,
   RA_BPATH,
   RA_HELP,
#  ifdef NR_BATCHMODE
      RA_SERVER,
      RA_GROUP,
      RA_MSGID,
      RA_MSGNUM,
      RA_HEAD,
      RA_BODY,
      RA_ART,
      RA_CHECK,
      RA_HEADER,
      RA_LISTGROUPS,
      RA_LISTMSGIDS,
      RA_ARTCOUNT,
      RA_SRVHELP,
      RA_LISTHDRS,
      RA_SKIPNUMS,
      RA_SEARCH,
      RA_SEARCHRES,
      RA_CASE,
      RA_INVERT,
#  endif
   RA_Count,
};
