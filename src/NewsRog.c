# include "NewsRog.h"

# include "S-LibSetup.h"
# include "S-EventLoop.h"
# include "S-RegSetup.h"
# include "S-BetaCode.h"
# include "S-ParseCL.h"
# include "S-AppObject.h"

static LONG   CX_BrokerPri      = 0;
static TEXT   CX_StartProj[FMSIZE];
static TEXT   CX_ButtonPath[FMSIZE];
static ULONG  NoProj            = FALSE;
static ULONG  OnLine            = FALSE;
static ULONG  OffLine           = FALSE;
extern long __stack             = 80000;

# ifdef NR_BATCHMODE
#   define ServLen  63
#   define BCLen    127
#   define GrpLen   127
#   define HdrLen   31
    static TEXT   CX_BCData[BCLen+1];
    static TEXT   CX_Server[ServLen+1];
    static TEXT   CX_Group[GrpLen+1];
    static TEXT   CX_Header[HdrLen+1];
    static TEXT   CX_SRes[HdrLen+1];
    static ULONG  CX_GetMode    = SWV_Get_Art;
    static ULONG  CX_BatchCmd   = SWV_BC_None;
    static ULONG  CX_BanMode    = SWV_BanMode_Normal;
    static LONG   CX_MsgNum     = -1;
    static LONG   CX_SkipNums   = FALSE;
    static LONG   CX_Case       = FALSE;
    static LONG   CX_Invert     = FALSE;
    struct Library  *SocketBase = NULL;
    static STRPTR RawBuff       = NULL;
    static STRPTR LineBuff      = NULL;
    static TEXT   SendBuff[280];
    static LONG  *MsgNums       = NULL;
# endif


# ifdef NR_BATCHMODE

/*****************************************************************************
 * Function: Close networking lib if it`s open before we terminate           *
 *****************************************************************************/

void _STD_4200_Batch(void)
{
   CloseLibrary(SocketBase);
   FreeVec(RawBuff);
   FreeVec(LineBuff);
   FreeVec(MsgNums);
}


/*****************************************************************************
 * send a command to the server                                              *
 *****************************************************************************/

static void dosend(int sockfd, char *cmd)
{
   LONG Len = strlen(cmd);
   
   if (send(sockfd, cmd, Len, 0) != Len) {
      Printf("send error\n");
      exit(10);
   }
}


/****************************************************************************
 * read data the server sends to us and break into lines                     *
 *****************************************************************************/

static unsigned char *dorecv(int sockfd)
{
#  define RawBuffSize 8192
   static LONG RawPos = 0, InBytes = 0, LinePos = 0;

   if (!RawBuff)  RawBuff  = AllocVec(RawBuffSize, MEMF_ANY);
   if (!LineBuff) LineBuff = AllocVec(RawBuffSize, MEMF_ANY);
   
   for (;;) {
      if (RawPos == InBytes) {
         RawPos = 0;
         if ((InBytes = recv(sockfd, RawBuff, RawBuffSize, 0)) <= 0) {
            Printf("recv error on socket\n");
            exit(10);
         }
      }

      while (RawPos < InBytes) {
         unsigned char c;

         if (LinePos < RawBuffSize) {
            c = LineBuff[LinePos++] = RawBuff[RawPos++];
            if (c == '\r') {
               LinePos--;
               continue;
            }
         } else
            c = '\0', RawPos++;

         if (c == '\n') {
            LineBuff[--LinePos] = '\0';

            if (LinePos > 0 && LineBuff[LinePos-1] == 0x0D)
               LineBuff[--LinePos] = '\0';

            LinePos = 0;

            return LineBuff;
         }
      }
   }
}


/*****************************************************************************
 * Check return code against expected                                        *
 *****************************************************************************/

static LONG CheckRC(STRPTR s, LONG Min, LONG Max)
{
   LONG Got = atol(s);

   if (Got < Min || Got > Max) {
      Printf("Unexpected return code: %s\n", s);
      exit(10);
   }

   return Got;
}


/*****************************************************************************
 * Function: Dump results of command to stdout                               *
 *****************************************************************************/

static void DumpCmdOutput(int sockfd, LONG Min, LONG Max,
                          STRPTR Header, ULONG SkipSp)
{
   STRPTR s;
   ULONG  HLen = Header && Header[0] ? strlen(Header) : 0;

   // -- check command results -----------------------------------------------
   
   CheckRC(dorecv(sockfd), Min, Max);

   for (;;) {
      s = dorecv(sockfd);
      if (s[0] == '.') {
         if (s[1] == '\0')                return;
         if (s[1] == '.' && s[2] == '\0') s++;
      }

      if (HLen) {
         if (Strnicmp(s, Header, HLen) == 0) {
            Printf("%s\n", s + HLen + 1);
            return;
         }
      } else {
         if (SkipSp)
            if (s = strchr(s, ' '))
               s++;

         if (s)
            Printf("%s\n", s);
      }
   }
}


/*****************************************************************************
 * Function: Send group command                                              *
 *****************************************************************************/

static void SendGrpCmd(int sockfd, ULONG Check)
{
   if (CX_Group[0]) {
      sprintf(SendBuff, "GROUP %s\r\n", CX_Group);
      dosend(sockfd, SendBuff);

      if (Check)
         CheckRC(dorecv(sockfd), 200, 299);
   }
}


/*****************************************************************************
 * Add colon to header if not present                                        *
 *****************************************************************************/

static LONG AddHeaderColon(STRPTR Hdr)
{
   LONG len = strlen(Hdr);
   // -- fix up header to search for -----------------------------------------
   
   if (len)
      if (Hdr[len-1] != ':')
         strcat(Hdr, ":"), len++;

   return len;
}


/*****************************************************************************
 * Function: Fetch by message ID                                             *
 *****************************************************************************/

static ULONG GC_FetchMsgID(int sockfd, STRPTR MsgID, ULONG GetMode,
                           LONG MsgNum)
{
   TEXT  MID[BCLen+4];
   ULONG rc = 0;

   if (MsgNum < 0) {
      // -- normalize MsgID to angle bracket mode -------------------------------
      
      if (MsgID[0] != '<') {
         MID[0] = '<';
         strcat(strcpy(MID+1, MsgID), ">");
         MsgID = MID;
      }
   } else
      SendGrpCmd(sockfd, TRUE);

   // -- fix up header to search for -----------------------------------------
   
   AddHeaderColon(CX_Header);

   // -- formulate command ---------------------------------------------------
   
   sprintf(SendBuff, MsgNum < 0 ? "%s %s\r\n" : "%s %ld\r\n",
           (GetMode == SWV_Get_Head || CX_Header[0]) ? "HEAD" :
           GetMode == SWV_Get_Body                   ? "BODY" :
           (GetMode == SWV_Get_Stat || GetMode == SWV_Get_Check) ? "STAT" :
           "ARTICLE",
           MsgNum < 0 ? (ULONG)MsgID : (ULONG)MsgNum);

   dosend(sockfd, SendBuff);

   if (GetMode == SWV_Get_Check) {
      rc = atol(dorecv(sockfd));
      rc = (rc >= 200 && rc <= 299) ? 0 : 1;
   } else
      DumpCmdOutput(sockfd, 200, 299, CX_Header, FALSE);

   return rc;
}


/*****************************************************************************
 * Function: list groups matching pattern                                    *
 *****************************************************************************/

static ULONG GC_ListGroups(int sockfd, STRPTR Patt)
{
   sprintf(SendBuff, "LIST ACTIVE %s\r\n", SW_STRDEF(Patt, ""));

   dosend(sockfd, SendBuff);

   DumpCmdOutput(sockfd, 200, 299, NULL, FALSE);

   return 0;
}


/*****************************************************************************
 * Function: get list of all msg numbers in this group, term with -1         *
 *****************************************************************************/

static void GetMsgNums(int sockfd)
{
   LONG MaxCount = 0,
        Pos      = 0;
   STRPTR s;
   
   dosend(sockfd, "LISTGROUP\r\n");
   CheckRC(dorecv(sockfd), 200, 299);

   for (;;) {
      s = dorecv(sockfd);

      if (s[0] == '.' && s[1] == '\0')
         break;

      if ((Pos+1) >= MaxCount) {
         ULONG *OldMsgNums = MsgNums;
         
         MaxCount += 1000;

         if (!(MsgNums = AllocVec(MaxCount * sizeof(LONG), MEMF_ANY))) {
            Printf("Malloc error.\n");
            exit(15);
         }

         if (OldMsgNums) {
            memcpy(MsgNums, OldMsgNums, (MaxCount-1000) * sizeof(LONG));
            FreeVec(OldMsgNums);
         }
      }

      MsgNums[Pos++] = atol(s);
   }

   if (MsgNums)
      MsgNums[Pos] = -1;
}


/*****************************************************************************
 * Function: search for a given pattern, list matching MsgIDs                *
 *****************************************************************************/

# define PreFetch 2

static ULONG GC_Search(int sockfd, STRPTR Patt, ULONG GetMode)
{
   TEXT   PattBuff[BCLen*2+2], tp[BCLen+4], Result[256];
   LONG x, rc, Found, InHdr, HLen, SRLen;
   STRPTR s, s1;

   tp[0] = '*';
   strcat(strcpy(tp+1, Patt), "*");

   if (CX_SRes[0] == '\0')
      strcpy(CX_SRes, "Message-ID:");

   SRLen=AddHeaderColon(CX_SRes);

   if ((CX_Case ? ParsePattern : ParsePatternNoCase)(tp, PattBuff, BCLen*2+2)
       < 0) {
      Printf("Pattern error.\n");
      exit(5);
   }

   if (HLen = AddHeaderColon(CX_Header))
     CX_GetMode = SWV_Get_Head;
   
   SendGrpCmd(sockfd, TRUE);
   GetMsgNums(sockfd);

   if (!MsgNums)
      exit(0);

   for (x=0; MsgNums[x] >= 0; x++) {
      sprintf(SendBuff, "%s %ld\r\n",
              CX_GetMode == SWV_Get_Head ? "HEAD" : "ARTICLE", MsgNums[x]);

      dosend(sockfd, SendBuff);
      rc = atol(dorecv(sockfd));

      if (rc < 200 || rc > 299) continue;

      for (Found=FALSE, InHdr=TRUE, Result[0]='\0';;) {
         s = dorecv(sockfd);

         if (s[0] == '.') {
            if (s[1] == '\0')                break;
            if (s[1] == '.' && s[2] == '\0') s++;
         }

         if (s[0] == '\0') InHdr = FALSE;

         if (InHdr) {
            if (s1 = strchr(s,':'))
               if ((++s1)[0] = ' ') s1++;
         } else s1 = s;

         if (!s1) continue;
         
         if ((CX_Case ? MatchPattern : MatchPatternNoCase)(PattBuff, s1) &&
             (!CX_Header[0] || InHdr && !Strnicmp(s, CX_Header, HLen))) {
            Found = TRUE;
            if (!Strnicmp(CX_SRes, "MATCH:", 6))
               Printf("%s\n", s);
         }

         if (InHdr && Strnicmp(s, CX_SRes, SRLen) == 0)
            strncpy(Result, s1, sizeof(Result)-1)[sizeof(Result)-1] = '\0';
      }

      if (Found == !CX_Invert && Result[0])
         Printf("%s\n", Result);
   }

   return 0;
}


/*****************************************************************************
 * Function: list headers pattern                                            *
 *****************************************************************************/

static ULONG GC_ListHdrs(int sockfd)
{
   LONG Len;
   
   SendGrpCmd(sockfd, TRUE);

   if (CX_Header[Len = (strlen(CX_Header)-1)] == ':')
      CX_Header[Len] = '\0';

   if (CX_MsgNum > 0)
      sprintf(SendBuff, "XHDR %s %ld-%ld\r\n", CX_Header, CX_MsgNum, CX_MsgNum);
   else
      sprintf(SendBuff, "XHDR %s 1-\r\n", CX_Header);

   dosend(sockfd, SendBuff);

   DumpCmdOutput(sockfd, 200, 299, NULL, CX_SkipNums);

   return 0;
}


/*****************************************************************************
 * Function: list message IDs                                                *
 *****************************************************************************/

static ULONG GC_ListMsgIDs(int sockfd)
{
   SendGrpCmd(sockfd, TRUE);
   
   dosend(sockfd, "XHDR Message-ID 1-\r\n");

   DumpCmdOutput(sockfd, 200, 299, NULL, TRUE);

   return 0;
}


/*****************************************************************************
 * Function: return server help message                                      *
 *****************************************************************************/

static ULONG GC_SrvHelp(int sockfd)
{
   dosend(sockfd, "HELP\r\n");

   DumpCmdOutput(sockfd, 100, 100, NULL, TRUE);

   return 0;
}


/*****************************************************************************
 * Function: Test for group existing                                         *
 *****************************************************************************/

static ULONG GC_TestGrp(int sockfd)
{
   LONG rc = atol(dorecv(sockfd));

   return (ULONG)((rc >= 200 && rc <= 299) ? 0 : 1);
}


/*****************************************************************************
 * Function: Test for group existing                                         *
 *****************************************************************************/

static ULONG GC_ArtCount(int sockfd)
{
   STRPTR s;
   
   CheckRC(s = dorecv(sockfd), 200, 299);

   if (s = strchr(s, ' '))
      Printf("%ld\n", atol(s));
   else {
      Printf("Malformed group result.\n");
      exit(10);
   }

   return 0;
}


/*****************************************************************************
 * Function: handle batch mode stuff                                         *
 *****************************************************************************/

static void BatchMode(void)
{
   ULONG rc = 10;
   struct hostent *he;
   struct sockaddr_in sin;
   int sockfd = -1;

   if (CX_BatchCmd == SWV_BC_None) return;

   if (CX_BatchCmd == SWV_BC_GetArt && CX_MsgNum >= 0 && CX_BCData[0]) {
      Printf("Specify only one of the MSGNUM and MSGID options.\n");
      goto Done;
   }

   if (CX_BatchCmd == SWV_BC_ListHdrs && !CX_Header[0]) {
      Printf("Please specify a header to list using the HEADER option.\n");
      goto Done;
   }

   if (!CX_Group[0] &&
       (CX_BatchCmd == SWV_BC_ListMsgIDs ||
        CX_BatchCmd == SWV_BC_ListHdrs   ||
        CX_BatchCmd == SWV_BC_Search     ||
        (CX_BatchCmd == SWV_BC_GetArt && CX_MsgNum >= 0))) {
      Printf("No group specified.\n");
      goto Done;
   }

   // -- open network library ------------------------------------------------
   
   if (!(SocketBase = OpenLibrary("bsdsocket.library", 4))) {
      Printf("Unable to open bsdsocket.library\n");
      goto Done;
   }

   // -- get server name -----------------------------------------------------

   if (SW_STRISNULL(CX_Server))
      if (GetVar("NNTPSERVER", CX_Server, ServLen, 0) <= 0)
         strcpy(CX_Server, "news");

   // -- initialize connection to NNTP server --------------------------------

   if (!(he = gethostbyname(CX_Server))) {
      Printf("Host lookup failure: %s\n", CX_Server);
      goto Done;
   }

   if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      Printf("Unable to create socket.\n");
      goto Done;
   }

   sin.sin_family = AF_INET;
   sin.sin_len    = sizeof(sin);
   sin.sin_addr   = *(struct in_addr *)he->h_addr;
   sin.sin_port   = NNTPPort;
      
   if (connect(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
      Printf("Unable to connect to host %s.\n", CX_Server);
      goto Done;
   }

   /* -- look for startup message from server ----------------------------- */

   CheckRC(dorecv(sockfd), 200, 201);

   // -- run command ---------------------------------------------------------

   switch (CX_BatchCmd) {
     case SWV_BC_Search:
      rc = GC_Search(sockfd, CX_BCData, CX_GetMode);
      break;

     case SWV_BC_GetArt:
      rc = GC_FetchMsgID(sockfd, CX_BCData, CX_GetMode, CX_MsgNum);
      break;

     case SWV_BC_ListGrp:
      rc = GC_ListGroups(sockfd, CX_BCData);
      break;

     case SWV_BC_ListHdrs:
      rc = GC_ListHdrs(sockfd);
      break;

     case SWV_BC_ListMsgIDs:
      rc = GC_ListMsgIDs(sockfd);
      break;

     case SWV_BC_TestGrp:
      SendGrpCmd(sockfd, FALSE);
      rc = GC_TestGrp(sockfd);
      break;

     case SWV_BC_ArtCount:
      SendGrpCmd(sockfd, FALSE);
      rc = GC_ArtCount(sockfd);
      break;

     case SWV_BC_SrvHelp:
      rc = GC_SrvHelp(sockfd);
      break;
   }
   
  Done:

   if (sockfd >= 0) {
      shutdown(sockfd, 2);
      CloseSocket(sockfd);
   }
   
   exit(rc);
}

# endif


/*****************************************************************************
 * Create initial object                                                     *
 *****************************************************************************/

static Object *LoadTopObj(int argc, char *argv[], UBYTE *fspec, Object *App)
{
   UBYTE ProjFile[FMSIZE];
   Object *Top = NULL, *Root = NULL, *TmpAppBase;
   UBYTE  *SaveFile, *DefSave;

   if (!App) return NULL;
   
   if (!(TmpAppBase = MUI_NewObject(SWC_AppBase, TAG_DONE)))
      goto Done;

   DefSave = SaveFile = SWGetStr(TmpAppBase, SWA_AppBase_DefSaveFile);

   if (!(Root = MUI_NewObject(SWC_Root, TAG_DONE)))
      goto Done;
   
   // -- started from workbench - use project icon, if given one -------------

   if (argc == 0) {
      BPTR lock;
      struct WBStartup *wb = (struct WBStartup *)argv;

      if (wb->sm_NumArgs > 1) {

         NameFromLock(wb->sm_ArgList[1].wa_Lock, ProjFile, FMSIZE);
         AddPart(ProjFile, wb->sm_ArgList[1].wa_Name,      FMSIZE);

         UnLock(lock = Lock(ProjFile, ACCESS_READ));

         if (lock) SaveFile = ProjFile;
      }
   }
   
   SaveFile = SW_STRDEF(fspec, SaveFile);

   // -- load default saved object from disk if possible ---------------------

   if (!NoProj && (Top = (Object  *)DoMethod(Root, SWM_Load, SaveFile))) {
      // -- clear error if no default save file ------------------------------
      if (SaveFile == DefSave)
         DoMethod(Top, SWM_ClearObjError);

      set(Top, SWA_AppBase_LastSaveFile, SaveFile);
   }

   if (!Top)
      Top = MUI_NewObject(SWC_NNews, TAG_DONE);

   set(RegObj, SWA_KFSerOnly, TRUE);
   
   SetAttrs(Top,
            SWA_AppBase_Reg,      RegObj,
            SWA_AppBase_App,      App,
            SWA_NNews_ButtonPath, CX_ButtonPath,
            TAG_DONE);

   if (OnLine || OffLine)
      set(Top, SWA_NNews_StartNWMode,
          OnLine ? SWV_NNews_StartNW_On : SWV_NNews_StartNW_Off);
   
   DoMethod(Top, SWM_AppBase_PostNewProj);

  Done:
   MUI_DisposeObject(Root);
   MUI_DisposeObject(TmpAppBase);

   return Top;
}


/*****************************************************************************
 * Function: process command line arguments                                  *
 *****************************************************************************/

static void getstrop(LONG *opts, LONG pos, STRPTR s, LONG len)
{
   if (opts[pos]) strncpy(s, (UBYTE *)opts[pos], len)[len] = '\0';
}


static ULONG ProcessArgs(int argc, char *argv[])
{
   ULONG rc = FAILURE;
   struct RDArgs *rdargs = NULL;
   LONG opts[RA_Count];
   STRPTR CmdLine    = SWParseCmdLine(argc, argv);
   struct RDArgs *ra = AllocDosObject(DOS_RDARGS, TAG_DONE);

   if (!ra) goto Done;
   if (!CmdLine) { rc = SUCCESS; goto Done; }
   
   CX_StartProj[0] = '\0';
   
#  ifdef NR_BATCHMODE
   CX_Server[0] = CX_BCData[0] = CX_Group[0] = CX_Header[0] = CX_SRes[0] = '\0';
#  endif
   
   memset(opts, 0, sizeof(opts));

   ra->RDA_Source.CS_Buffer = CmdLine;
   ra->RDA_Source.CS_Length = strlen(CmdLine);
   ra->RDA_Source.CS_CurChr = 0;
   
   if (!(rdargs = ReadArgs(TEMPLATE, opts, ra))) goto Done;

   if (opts[RA_HELP])      goto Done;
   if (opts[RA_CX_PRI])    CX_BrokerPri = *(LONG *)opts[RA_CX_PRI];
   if (opts[RA_TASKPRI])   SetTaskPri(FindTask(0), *(LONG *)opts[RA_TASKPRI]);
   if (opts[RA_NOPROJ])    NoProj       = TRUE;
   if (opts[RA_ONLINE])    OnLine       = TRUE;
   if (opts[RA_OFFLINE])   OffLine      = TRUE;
   if (opts[RA_NOBANIMG])  CX_BanMode   = SWV_BanMode_NoImg;
   if (opts[RA_NOBAN])     CX_BanMode   = SWV_BanMode_NoBan;
   if (opts[RA_STARTPROJ]) {
       strcpy(CX_StartProj, ((STRPTR *)opts[RA_STARTPROJ])[0]);

       // -- handle '?' as help ----------------------------------------------

       if (CX_StartProj[0] == '?' && CX_StartProj[1] == '\0')
          goto Done;

       if (((STRPTR *)(opts[RA_STARTPROJ]))[1]) {
          Printf("Please specify only one project file.\n");
          goto Done;
       }
    }

   getstrop(opts, RA_BPATH,      CX_ButtonPath, FMSIZE-1);

#  ifdef NR_BATCHMODE
   getstrop(opts, RA_SERVER,     CX_Server,  ServLen);
   getstrop(opts, RA_GROUP,      CX_Group,   GrpLen);
   getstrop(opts, RA_HEADER,     CX_Header,  HdrLen);
   getstrop(opts, RA_MSGID,      CX_BCData,  BCLen);
   getstrop(opts, RA_LISTGROUPS, CX_BCData,  BCLen);
   getstrop(opts, RA_SEARCH,     CX_BCData,  BCLen);
   getstrop(opts, RA_SEARCHRES,  CX_SRes,    HdrLen);
   if (opts[RA_MSGID])       CX_BatchCmd   = SWV_BC_GetArt;
   if (opts[RA_LISTGROUPS])  CX_BatchCmd   = SWV_BC_ListGrp;
   if (opts[RA_LISTMSGIDS])  CX_BatchCmd   = SWV_BC_ListMsgIDs;
   if (opts[RA_GROUP]) { 
      if (opts[RA_CHECK])    CX_BatchCmd   = SWV_BC_TestGrp;
      if (opts[RA_ARTCOUNT]) CX_BatchCmd   = SWV_BC_ArtCount;
   }
   if (opts[RA_SRVHELP])     CX_BatchCmd   = SWV_BC_SrvHelp;
   if (opts[RA_HEAD])        CX_GetMode    = SWV_Get_Head;
   if (opts[RA_BODY])        CX_GetMode    = SWV_Get_Body;
   if (opts[RA_ART])         CX_GetMode    = SWV_Get_Art;
   if (opts[RA_CHECK])       CX_GetMode    = SWV_Get_Check;
   if (opts[RA_MSGNUM])      CX_MsgNum     = *(LONG *)opts[RA_MSGNUM],
                             CX_BatchCmd   = SWV_BC_GetArt;
   if (opts[RA_LISTHDRS])    CX_BatchCmd   = SWV_BC_ListHdrs;
   if (opts[RA_SKIPNUMS])    CX_SkipNums   = TRUE;
   if (opts[RA_SEARCH])      CX_BatchCmd   = SWV_BC_Search;
   if (opts[RA_CASE])        CX_Case       = TRUE;
   if (opts[RA_INVERT])      CX_Invert     = TRUE;

#  endif

   rc = SUCCESS;

  Done:
   if (rdargs) FreeArgs(rdargs);
   FreeDosObject(DOS_RDARGS, ra);
   SWFreeCmdLine(CmdLine);
   return rc;
}


/*****************************************************************************
 * close init window                                                         *
 *****************************************************************************/

static void CloseBannerWin(Object *App, Object **InitWin, Object **StatusTxt)
{
   if (!App || !InitWin || !*InitWin) return;
   
   set(*InitWin, MUIA_Window_Open, FALSE);
   DoMethod(App, OM_REMMEMBER, *InitWin);

   MUI_DisposeObject(*InitWin);

   *InitWin   = NULL;
   *StatusTxt = NULL;
}


/*****************************************************************************
 * Function: set status window text                                          *
 *****************************************************************************/

static void SetStatusTxt(Object *StatusTxt, STRPTR Status)
{
   set(StatusTxt, MUIA_Text_Contents, Status);
}


/*****************************************************************************
 * open init window                                                          *
 *****************************************************************************/

static ULONG OpenBannerWin(Object *App, Object **InitWin, Object **StatusTxt,
                           STRPTR Str)
{
   Object *Logo = NULL;

   if (CX_BanMode == SWV_BanMode_NoBan) return SUCCESS;

   if (!App || !InitWin) return FAILURE;

   if (CX_BanMode != SWV_BanMode_NoImg)
      Logo = DTImgObject,
         SWA_DTImg_FSpec,      LogoImage,
         SWA_DTImg_Adjust,     TRUE,
         SWA_DTImg_Path,       CX_ButtonPath,
      End;

   if (Logo)
      Logo = HGroup,
         ReadListFrame,
         MUIA_Background, MUII_SHADOW,
         Child, HSpace(0),
         Child, Logo,
         Child, HSpace(0),
      End;
   
   *InitWin = WindowObject,
      MUIA_Window_DragBar,     FALSE,
      MUIA_Window_CloseGadget, FALSE,
      MUIA_Window_DepthGadget, FALSE,
      MUIA_Window_SizeGadget,  FALSE,
      MUIA_Window_Activate,    FALSE,
      WindowContents, VGroup,
         GroupFrame,
         MUIA_Background,      MUII_TextBack,
         MUIA_Group_SameWidth, TRUE,
         Child, TextObject,
            MUIA_Text_Contents, CopyrightTxt,
            MUIA_Text_PreParse, MUIX_C,
         End,
         Logo ? Child : TAG_IGNORE, Logo,
         Child, *StatusTxt = TextObject,
            TextFrame,
            MUIA_Font,          MUIV_Font_Big,
            MUIA_Text_PreParse, MUIX_C,
            MUIA_Text_Contents, Str,
         End,
      End,
   End;

   if (!*InitWin) {
      Printf("Unable to create initialization window\n");
      return FAILURE;
   }

   DoMethod(App, OM_ADDMEMBER, *InitWin);
   set(*InitWin, MUIA_Window_Open, TRUE);

   ((struct Process *)FindTask(0))->pr_WindowPtr =
      SWGetObj(*InitWin, MUIA_Window_Window);

   DoMethod(App, MUIM_Application_CheckRefresh);
   
   return SUCCESS;
}


/*****************************************************************************
 * Main code                                                                 *
 *****************************************************************************/

main (int argc, char *argv[])
{
   int rc = 10;
   Object *App = NULL, *Win, *TCPList, *News = NULL, *InitWin = NULL,
          *StatusTxt = NULL;
   SWDiskObjVars;

   // -- process command line options ----------------------------------------

   if (ProcessArgs(argc, argv) == FAILURE) {
      rc = 5;
      Printf("Usage: %s\n", TEMPLATE);
      goto Done;
   }

   // -- handle batch mode stuff ---------------------------------------------

   BatchMode();
      
   // -- build application object --------------------------------------------

   App  = SWAppObj(APPNAME, APPNAMECAP, APPVERSTR,
                   "Amiga News Reader",CX_BrokerPri, TRUE);

   if (!App)  {
      Printf("Unable to create application\n");
      goto Done;
   }

   if (OpenBannerWin(App, &InitWin, &StatusTxt, InitWinTxt) != SUCCESS)
      goto Done;

   SetStatusTxt(StatusTxt, "Checking Registration...");

   SWRegCheck(App, NULL);
   BetaCodeInit(App, NULL);
   DoMethod(App, MUIM_Application_CheckRefresh);

   SetStatusTxt(StatusTxt, "Loading Project...");
   
   if (!(News = LoadTopObj(argc, argv, CX_StartProj, App))) {
      Printf("Unable to create NNews object\n");
      goto Done;
   }

   SetStatusTxt(StatusTxt, "Creating UI...");

   if (!(Win     = (Object *)DoMethod(News, SWM_AppBase_ShowAppUI))) {
      rc = 11;
      goto Done;
   }
   
   if (!(TCPList = SWGetObj(News, SWA_NNews_TCPList))) {
      rc = 12;
      goto Done;
   }

   CloseBannerWin(App, &InitWin, &StatusTxt);

   SWEventLoop(App, Win, NULL, TCPList, TRUE);
   
   rc = 0;

 Done:
   CloseBannerWin(App, &InitWin, &StatusTxt);

   OpenBannerWin(App, &InitWin, &StatusTxt, TermWinTxt);
   
   MUI_DisposeObject(News);

   CloseBannerWin(App, &InitWin, &StatusTxt);

   SWFreeAppObj(App);

   exit (rc);
}
