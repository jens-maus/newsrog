#  include "C-NGroup-I.h"

#  define IObj(o) (ImageObject, MUIA_Image_Spec, (o), End)
#  define SWV_FAQList_ListFile    "NR.FAQList"
   
enum {
   SWV_NG_Img_Drawer = 0,
   SWV_NG_Img_Net,
   SWV_Img_MAX,
};

#  undef AV
#  undef SD
#  undef FV
#  define AV(s)   (SWPoolAV(idata->MemPool, (s)))
#  define SD(s)   (SWPoolStrDup(idata->MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->MemPool, (UBYTE *)(m))

typedef struct FAQListData {
   SWP_NNews_AttrDefs;
   SWP_Img_AttrDefs;

   TEXT FAQLocalDir[FMSIZE];
   TEXT FAQLocalFile[FMSIZE];
   
   struct Hook FAQDspHook,     // FAQ hooks
               FAQCstHook,
               FAQDstHook;

   ULONG       A4;
   Object     *FAQTitle;       // FAQ title line
   Object     *FListLV;        // FAQ list object
   Object     *FListL;         // FAQ list object
   Object     *b_Upd;          // FAQ update button (so we can en/disable it)
   Object     *b_Abt;          // FAQ abort button
   Object     *Group;          // group object for this FAQ
   Object     *ActPop;         // action popup
   Object     *AListV;         // action listview
   STRPTR      FAQFSpec;       // FAQ file to fetch
   ULONG       Displayed;      // displayed
   ULONG       BadFTPDir;      // FTP dir not found?
   Object     *FAQSrv;         // FAQ server
   LONG        FAQID;          // FAQ cmd ID
   APTR       *MemPool;
   STRPTR      Act;            // for multimedia actions
   STRPTR      FAQFile;        // for variable lookups
} FAQListData;

# undef  SW_IDATA
# define SW_IDATA FAQListData

SW_MkClass(FAQList, MUIC_Group)

static __stdargs Object *DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1,...)
{
   return (Object *)DoSuperMethod(cl,obj,OM_NEW,&tag1,NULL);
}


/*****************************************************************************
 * Display hook for FAQ list                                                 *
 *****************************************************************************/

static __saveds __asm LONG FAQDspFn(a2 char **array, a1 FAQEntry *fe,
                                    a0 struct Hook *hook)
{
   FAQListData *idata = (FAQListData *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   if (fe) {
      *array++ = fe->Cached ? SWImg(idata, SWV_NG_Img_Drawer) : " ",
      *array   = fe->FAQName;
   } else {
      *array++ = " ",
      *array   = GetS(SWS_T_FAQName);
   }
   
   return 0;
}


/*****************************************************************************
 * Function: Generate FAQ filename                                           *
 *****************************************************************************/

static STRPTR FAQFName(SW_IDATA *idata, STRPTR FAQName, ULONG Hash)
{
   TEXT FAQHashName[SWV_NArticle_HashFNBuffLen];

   if (SW_STRISNULL(FAQName)) return NULL;

   strcpy(idata->FAQLocalFile, idata->FAQLocalDir);
   
   if (Hash) {
      if (!DoMethod(NN_ArtTpl(idata), SWM_NArticle_HashFName,
                    SWT_NArticle_HashStr,  FAQName,
                    SWT_NArticle_HashBuff, FAQHashName,
                    TAG_DONE))
         return NULL;
      AddPart(idata->FAQLocalFile, FAQHashName, FMSIZE);
   } else
      AddPart(idata->FAQLocalFile, FAQName, FMSIZE);
   
   return idata->FAQLocalFile;
}


/*****************************************************************************
 * Function: Determine if FAQ list exists on disk                            *
 *****************************************************************************/

static LONG FAQFileExists(SW_IDATA *idata, STRPTR Name, ULONG Hash)
{
   return SWFileExists(FAQFName(idata, Name, Hash));
}


/*****************************************************************************
 * Construct hook for FAQ list                                               *
 *****************************************************************************/

static __saveds __asm FAQEntry *FAQCstFn(a1 STRPTR s, a0 struct Hook *hook)
{
   FAQEntry *fe;
   FAQListData *idata = (FAQListData *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   if (!(fe = (FAQEntry *)AllocVec(sizeof(*fe), MEMF_ANY))) return NULL;

   strcpy(fe->FAQName, s);
   fe->Cached = FAQFileExists(idata, s, TRUE);

   return fe;
}


/*****************************************************************************
 * Destruct hook for FAQ list                                                *
 *****************************************************************************/

static __saveds __asm VOID FAQDstFn(a1 FAQEntry *fe, a0 struct Hook *hook)
{
   FAQListData *idata = (FAQListData *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   FreeVec(fe);
}


/*****************************************************************************
 * Method: Dispose object                                                    *
 *****************************************************************************/

METHOD(OM_DISPOSE, Msg)
{
   DoMethod(o, SWM_FAQList_AbortFAQ);

   FV(idata->Act);
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Make new object                                                   *
 *****************************************************************************/

METHOD(OM_NEW, opSet *)
{
   STRPTR FAQLocalDir;
   
#  define attrl msg->ops_AttrList

   Object *b_View, *b_Upd, *b_Del, *b_Sel, *b_Abt, *FAQTitle, *FListL, *FListLV,
          *TopObj = GetTagObj(SWA_AppBase_TopObj, NULL, attrl), *bg;

    o = DoSuperNew(cl, o,
      MUIA_Background,   MUII_GroupBack,
      MUIA_Group_Horiz,  FALSE,
      Child, SWUI_FrameText("", 0, NULL, &FAQTitle),
      Child, FListLV = ListviewObject,
         InputListFrame,
         MUIA_CycleChain,           1,
         MUIA_ShortHelp,            GetS(SWS_H_FAQList),
         MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
         MUIA_Listview_Input,       TRUE,
         MUIA_Listview_List,        FListL = ObjListObject,
            MUIA_Background,  MUII_ListBack,
            MUIA_List_Format, "COL=0,COL=1",
            MUIA_List_Title,  TRUE,
         End,
      End,
      Child, bg = ColGroup(3),
         MUIA_Group_SameSize, TRUE,
         Child, b_View = MkBt(SWS_B_FAQView,    SWS_H_FAQView),
         Child, b_Upd  = MkBt(SWS_B_FAQUpd,     SWS_H_FAQUpd),
         Child, b_Del  = MkBt(SWS_B_FAQDel,     SWS_H_FAQDel),
         Child, b_Sel  = MkBt(SWS_B_FAQSelAll,  SWS_H_FAQSelAll),
         Child, b_Abt  = MkBt(SWS_B_FAQAbort,   SWS_H_FAQAbort),
      End,
      TAG_MORE, attrl);

   if (!o) SWDB_RET(NULL);

   // -- initialize instance data --------------------------------------------

   idata                     = INST_DATA(cl, o);

   memset(idata, 0, sizeof(*idata));

   idata->b_Upd              = b_Upd;
   idata->b_Abt              = b_Abt;
   idata->FAQTitle           = FAQTitle;
   idata->FListLV            = FListLV;
   idata->FListL             = FListL;
   
   idata->FAQCstHook.h_Data  =
   idata->FAQDspHook.h_Data  =
   idata->FAQDstHook.h_Data  = (void *)idata;
   
   idata->FAQCstHook.h_Entry = (ULONG (*)())FAQCstFn;
   idata->FAQDspHook.h_Entry = (ULONG (*)())FAQDspFn;
   idata->FAQDstHook.h_Entry = (ULONG (*)())FAQDstFn;
   idata->A4                 = getreg(REG_A4);
   idata->SWA_AppBase_TopObj = TopObj;
   idata->Group              = GetTagObj(SWA_FAQList_Group, NULL, attrl);
   idata->MemPool            = NN_MemPool(idata);
   
   FAQLocalDir = GetTagUB(SWA_FAQList_LocalDir, NULL, attrl);

   if (!SW_STRISNULL(FAQLocalDir))
      SWMakePath(strcpy(idata->FAQLocalDir, FAQLocalDir));

   SetAttrs(FListL,
            MUIA_List_ConstructHook,  &idata->FAQCstHook,
            MUIA_List_DestructHook,   &idata->FAQDstHook,
            MUIA_List_DisplayHook,    &idata->FAQDspHook,
            TAG_DONE);

   set(idata->b_Abt,   MUIA_Disabled, TRUE);

   // -- set up action button ------------------------------------------------

   if (bg) {
      Object *AList = (Object *)
         DoMethod(NN_MMActs(idata), SWM_FileActs_GetActList,
                  SWT_FileActs_GAL_Type, SWV_FAQList_FAQType,
                  TAG_DONE);
      
      if (AList) {
         DoMethod(bg, OM_ADDMEMBER,
                  idata->ActPop = PopobjectObject,
                     MUIA_CycleChain, 1,
                     MUIA_Popobject_Object, idata->AListV = ListviewObject,
                        InputListFrame,
                        MUIA_Listview_List, AList,
                     End,
                     MUIA_Popstring_Button, MkBt(SWS_B_FAQAct, SWS_H_FAQAct),
                  End);

         if (idata->ActPop)
            DoMethod(idata->AListV, DCLK, o, 1, SWM_FAQList_Act);
      } else
         DoMethod(bg, OM_ADDMEMBER, HSpace(0));
   }

   // -- set up notifications ------------------------------------------------

   DoMethod(idata->b_Abt, NOTP, o, 1, SWM_FAQList_AbortFAQ);
   DoMethod(idata->b_Upd, NOTP, o, 1, SWM_FAQList_FetchFAQList);
   set(idata->b_Upd, MUIA_Disabled,
       !SWGetUL(NN_TCPList(idata), SWA_NTCPList_Online));

   DoMethod(b_View,  NOTP, o, 2, SWM_FAQList_FAQListClk, NULL);
   DoMethod(FListLV, DCLK, o, 2, SWM_FAQList_FAQListClk, NULL);
   DoMethod(b_Del,   NOTP, o, 1, SWM_FAQList_DelSelFAQ);

   SWDB_RET(o);
} SDFT


/*****************************************************************************
 * Method: MUI Setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   ULONG OpenMethod = 0;
   
   if (!DoSuperMethodA(cl, o, msg)) SWDB_RET(FALSE);

   SWMakeImg(idata, SWV_NG_Img_Drawer,  IObj(MUII_Disk));
   SWMakeImg(idata, SWV_NG_Img_Net,     IObj(MUII_Drawer));

   SWCreateImagesObj(idata->FListL, idata);

   idata->Displayed = TRUE;
   
   if (FAQFileExists(idata, SWV_FAQList_ListFile, FALSE))
      OpenMethod = SWM_FAQList_UpdList;
   else
      if (SWGetUL(NN_TCPList(idata), SWA_NTCPList_Online))
         OpenMethod = SWM_FAQList_FetchFAQList;

   if (OpenMethod)
      DoMethod(NN_App(idata), MUIM_Application_PushMethod, o, 1, OpenMethod);

   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: MUI Cleanup                                                       *
 *****************************************************************************/

METHOD(MUIM_Cleanup, Msg)
{
   ULONG rc = DoSuperMethodA(cl, o, msg);
   
   SWDeleteImagesObj(idata->FListL, idata);

   idata->Displayed = FALSE;

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: Insert FAQ list names into the actual FAQ list itself           *
 *****************************************************************************/

METHOD(SWM_FAQList_UpdList, Msg)
{
   TEXT FAQName[FMSIZE];
   BPTR fp = NULL;
   ULONG rc = FAILURE, Count = 0;
   STRPTR FAQList = FAQFName(idata, SWV_FAQList_ListFile, FALSE);

   if (SW_STRISNULL(FAQList))                    goto Done;
   if (!(fp = Open(FAQList, MODE_OLDFILE)))      goto Done;

   set(idata->FListL, MUIA_List_Quiet, TRUE);
   DoMethod(idata->FListL, MUIM_List_Clear);
   
   while (FGets(fp, FAQName, FMSIZE-1)) {
      SW_REMOVENL(FAQName);

      DoMethod(idata->FListL, MUIM_List_InsertSingle, FAQName,
               MUIV_List_Insert_Bottom);
      Count++;
   }

   set(idata->FListL, MUIA_List_Quiet, FALSE);

   DoMethod(idata->FAQTitle, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(Count ? SWS_T_FAQCount : SWS_T_NoFAQs), Count);
   
   rc = SUCCESS;

  Done:
   if (fp) Close(fp);
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Function: Get FAQ server                                                  *
 *****************************************************************************/

static Object *GetFAQSrv(SW_IDATA *idata)
{
   return (Object *)
      DoMethod(NN_ServList(idata), SWM_NServerList_FindCreate,
               NN_FAQServer(idata), SWV_NServer_Type_FTP);
}

   
/*****************************************************************************
 * Method: Fetch FAQ list for group, save to disk                            *
 *****************************************************************************/

METHOD(SWM_FAQList_FetchFAQList, Msg)
{
   Object *FAQSrv;
   STRPTR FAQDir;

   if (!(FAQSrv = GetFAQSrv(idata))) SWDB_RET(FAILURE);

   if (!(FAQDir = VL_Alloc(o, idata, NN_FAQDir(idata))))
      SWDB_RET(FAILURE);

   DoMethod(o, SWM_FAQList_AbortFAQ);

   idata->BadFTPDir = 0;
   
   if (DoMethod(FAQSrv, SWM_NServer_GetFTPDir,
                SWT_NServer_CmdObj,    o,
                SWT_NServer_CmdMethod, SWM_FAQList_FetchFAQList_P1,
                SWT_NServer_FTPDir,    FAQDir,
                TAG_DONE) == FAILURE) {
      if (idata->Displayed && idata->Group) {
         DoMethod(idata->Group, SWM_Root_AdoptObjError, FAQSrv, 0);
         DoMethod(idata->Group, SWM_Root_CheckErr, _win(o));
      }
   }

   VL_Free(o, idata, FAQDir);
   
   SWDB_RET(SUCCESS);
} SDFT

   
/*****************************************************************************
 * Function: FAQBegin                                                        *
 *****************************************************************************/

static void FAQBegin(Object *o, SW_IDATA *idata, SWP_NServer_FTPStatMsg *msg)
{
   if (!NSR_SBegin(msg)) return;

   idata->FAQSrv = NSR_Srv(msg),
   idata->FAQID  = NSR_ID(msg);

   set(idata->b_Abt, MUIA_Disabled, FALSE);
   set(idata->b_Upd, MUIA_Disabled, TRUE);
}


/*****************************************************************************
 * Function: FAQEnd                                                          *
 *****************************************************************************/

static void FAQEnd(Object *o, SW_IDATA *idata, SWP_NServer_FTPStatMsg *msg)
{
   if (NSR_SBad(msg) || NSR_SAbort(msg)) {
      if (idata->Displayed && idata->Group) {
         STRPTR FAQDir;
         TEXT   ErrMsg[FMSIZE + 128];
         
         DoMethod(idata->Group, SWM_Root_AdoptObjError, NSR_Srv(msg), 0);
         DoMethod(idata->Group, SWM_Root_CheckErr, _win(o));

         if (FAQDir = VL_Alloc(o, idata, NN_FAQDir(idata))) {
            sprintf(ErrMsg,
                    GetS(idata->BadFTPDir ? SWS_T_NoFAQDir : SWS_T_FAQAbort),
                    FAQDir);
            VL_Free(o, idata, FAQDir);
                                
            set(idata->FAQTitle, MUIA_Text_Contents, ErrMsg);
         }
      }
   }

   if (NSR_SAnyEnd(msg)) {
      idata->FAQSrv = NULL;

      FV(idata->FAQFSpec);
      idata->FAQFSpec = NULL;

      if (idata->Displayed) {
         set(idata->b_Abt, MUIA_Disabled, TRUE);
         set(idata->b_Upd, MUIA_Disabled, FALSE);
      }
   }
}

   
/*****************************************************************************
 * Function: Decode data from FTP server and build FAQ file                  *
 *****************************************************************************/

static ULONG DecodeFTPData(STRPTR Buff, ULONG BuffLen, STRPTR Dest)
{
   ULONG x, LastPos, rc= FAILURE;
   BPTR fp;
   UBYTE c;

   if (!Buff || SW_STRISNULL(Dest)) return FAILURE;

   if (!(fp = Open(Dest, MODE_NEWFILE))) goto Done;
   
   for (x=LastPos=0; x<=BuffLen; x++) {
      if (Buff[x] == '\n' || Buff[x] == '\r') {
         c = Buff[x], Buff[x] = '\0';
         if (FPuts(fp, Buff + LastPos) || FPuts(fp, "\n"))
            goto Done;

         Buff[x] = c;

         if (c=='\r' && Buff[x+1] == '\n') x++;
         LastPos = x+1;
      }
   }

   rc = SUCCESS;

 Done:
   if (fp) Close(fp);
   
   return rc;
}


/*****************************************************************************
 * Method: Fetch FAQ list for group, internal                                *
 *****************************************************************************/

METHOD(SWM_FAQList_FetchFAQList_P1, SWP_NServer_FTPStatMsg *)
{
   FAQBegin(o, idata, msg);

   if (NSR_SBadFTPDir(msg))
      idata->BadFTPDir = TRUE;

   if (NSR_SBegin(msg))
      if (idata->Displayed)
         set(idata->FAQTitle, MUIA_Text_Contents, GetS(SWS_T_FAQListFetch));
   
   if (NSR_SDone(msg)) {
      if (DecodeFTPData(NSR_Buff(msg), NSR_BuffSize(msg),
                        FAQFName(idata,SWV_FAQList_ListFile,FALSE)) == SUCCESS)
         DoMethod(o, SWM_FAQList_UpdList);
   }

   FAQEnd(o, idata, msg);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Abort any pending FAQ transfer                                    *
 *****************************************************************************/

METHOD(SWM_FAQList_AbortFAQ, Msg)
{
   if (idata->FAQSrv)
      DoMethod(idata->FAQSrv, SWM_NServer_AbortFTP, idata->FAQID);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: Display FAQ file                                                *
 *****************************************************************************/

static ULONG DisplayFAQFile(Object *o, SW_IDATA *idata, STRPTR Name)
{
   TEXT FAQFile[FMSIZE], tmp[FMSIZE];
   BPTR fp;

   if (!FAQFileExists(idata, Name, TRUE))     return FAILURE;

   strcpy(FAQFile, FAQFName(idata, Name, TRUE));

   if (strncmp(FAQFile, "PROGDIR:", sizeof("PROGDIR:")-1) == 0) {
      if (!(fp = Lock("PROGDIR:", ACCESS_READ))) return FAILURE;

      NameFromLock(fp, tmp, FMSIZE);
      UnLock(fp);

      AddPart(tmp, FAQFile + sizeof("PROGDIR:")-1, FMSIZE);
      strcpy(FAQFile, tmp);
   }

   if (!SW_STRISNULL(idata->Act)) {
      idata->FAQFile = FAQFile;
      
      DoMethod(NN_MMActs(idata), SWM_FileActs_DoAction,
               SWT_FileActs_DoA_Type,    SWV_FAQList_FAQType,
               SWT_FileActs_DoA_Action,  idata->Act,
               SWT_FileActs_DoA_VarList, NN_VarList(idata),
               SWT_FileActs_DoA_VarObj,  o,
               TAG_DONE);
   
      DoMethod(NN_MMActs(idata), SWM_Root_CheckErr, _win(o));

      idata->FAQFile = NULL;
   } else {
      DoMethod(idata->SWA_AppBase_TopObj, SWM_Root_SysCmd,
               SWT_Root_Cmd,      "multiview",
               SWT_Root_Arg,      FAQFile,
               TAG_DONE);
   }

   FV(idata->Act);
   idata->Act = NULL;

   return SUCCESS;
}


/*****************************************************************************
 * Method: FAQ name from list pointer                                        *
 *****************************************************************************/

static STRPTR FAQNameFromEntry(FAQEntry *fe)
{
   if (!fe) return NULL;
   
   return fe->FAQName;
}

   
/*****************************************************************************
 * Function: Find a FAQ entry given it's name                                *
 *****************************************************************************/

static FAQEntry *FindFAQEntry(Object *o, SW_IDATA *idata, STRPTR FAQName)
{
   LONG x;
   FAQEntry *fe;
   STRPTR EName;
   
   SW_MUIListLoop(idata->FListL, x, fe, TRUE)
      if (EName = FAQNameFromEntry(fe))
         if (!strcmp(EName, FAQName))
            return fe;

   return NULL;
}


/*****************************************************************************
 * Method: Fetch FAQ file, save to disk                                      *
 *****************************************************************************/

METHOD(SWM_FAQList_FetchFAQ, SWP_FAQList_FetchFAQ *)
{
   Object *FAQSrv;
   STRPTR FAQDir;

   if (SW_STRISNULL(msg->FAQName))            SWDB_RET(FAILURE);
   if (!(FAQSrv = GetFAQSrv(idata)))          SWDB_RET(FAILURE);
   if (!(idata->FAQFSpec = SD(msg->FAQName))) SWDB_RET(FAILURE);

   if (!(FAQDir = VL_Alloc(o, idata, NN_FAQDir(idata))))
      SWDB_RET(FAILURE);

   DoMethod(o, SWM_FAQList_AbortFAQ);

   if (DoMethod(FAQSrv, SWM_NServer_GetFTPFile,
                SWT_NServer_CmdObj,    o,
                SWT_NServer_CmdMethod, SWM_FAQList_FetchFAQ_P1,
                SWT_NServer_FTPDir,    FAQDir,
                SWT_NServer_FTPFile,   idata->FAQFSpec,
                TAG_DONE) == FAILURE) {
      if (idata->Displayed && idata->Group)
         DoMethod(idata->Group, SWM_Root_AdoptObjError, FAQSrv, 0);
         DoMethod(idata->Group, SWM_Root_CheckErr, _win(o));
   }

   VL_Free(o, idata, FAQDir);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Fetch FAQ file, internal                                          *
 *****************************************************************************/

METHOD(SWM_FAQList_FetchFAQ_P1, SWP_NServer_FTPStatMsg *)
{
   FAQBegin(o, idata, msg);
   
   if (NSR_SBegin(msg))
      if (idata->Displayed)
         set(idata->FAQTitle, MUIA_Text_Contents, GetS(SWS_T_FAQFetch));

   if (NSR_SDone(msg)) {
      FAQEntry *fe;

      if (idata->Displayed)
         set(idata->FAQTitle, MUIA_Text_Contents, GetS(SWS_T_GotFAQ));
      
      if (DecodeFTPData(NSR_Buff(msg), NSR_BuffSize(msg),
                        FAQFName(idata, idata->FAQFSpec, TRUE)) == SUCCESS) {

         if (fe = FindFAQEntry(o, idata, idata->FAQFSpec))
            fe->Cached = TRUE;
         
         DisplayFAQFile(o, idata, idata->FAQFSpec);

         DoMethod(idata->FListL, MUIM_List_Redraw, MUIV_List_Redraw_All);
      }
   }

   FAQEnd(o, idata, msg);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Process FAQ list click                                            *
 *****************************************************************************/

METHOD(SWM_FAQList_FAQListClk, SWP_FAQList_FAQListClk *)
{
   FAQEntry *fe = NULL;
   STRPTR FAQName;
   
   DoMethod(idata->FListL, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &fe);

   FAQName = FAQNameFromEntry(fe);
   
   if (SW_STRISNULL(FAQName)) SWDB_RET(FAILURE);

   // -- remember action for later -------------------------------------------

   FV(idata->Act);
   idata->Act = SD(msg->Act);
   
   // -- display or fetch FAQ ------------------------------------------------

   if (FAQFileExists(idata, FAQName, TRUE))
      DisplayFAQFile(o, idata, FAQName);
   else
      DoMethod(o, SWM_FAQList_FetchFAQ, FAQName);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Delete selected FAQs                                              *
 *****************************************************************************/

METHOD(SWM_FAQList_DelSelFAQ, Msg)
{
   LONG      x;
   FAQEntry *fe;
   STRPTR    FAQName, FAQFile;

   SW_MUIListLoopSel(idata->FListL, x, fe, TRUE)
      if (FAQName = FAQNameFromEntry(fe))
         if (FAQFile = FAQFName(idata, FAQName, TRUE)) {
            DeleteFile(FAQFile);

            if (!FAQFileExists(idata, FAQName, TRUE))
               fe->Cached = FALSE;
         }

   DoMethod(idata->FListL, MUIM_List_Redraw, MUIV_List_Redraw_All);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Go on or offline                                                  *
 *****************************************************************************/

METHOD(SWM_FAQList_OnOffLine, SWP_FAQList_OnOffLine *)
{
   DoMethod(o, SWM_FAQList_AbortFAQ);
   set(idata->b_Upd, MUIA_Disabled, !msg->State);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: multimedia action                                                 *
 *****************************************************************************/

METHOD(SWM_FAQList_Act, Msg)
{
   STRPTR Act; 
   
   DoMethod(idata->AListV, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &Act);
   DoMethod(idata->ActPop, MUIM_Popstring_Close, TRUE);
   
   DoMethod(o, SWM_FAQList_FAQListClk, Act);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   STRPTR s;

   VL_FindRet("FAQFile", idata->FAQFile);
   
   if (idata->Group && (s=(STRPTR)DoMethodA(idata->Group, msg)))
      SWDB_RET(s);
   
   SWDB_RET(NULL);
} SDFT


/*****************************************************************************
 * Dispatcher for FAQList class                                              *
 *****************************************************************************/

__asm ULONG FAQListDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct FAQListData *idata = INST_DATA(cl, o);

   MkClassDispatchInit(cl);

   switch (msg->MethodID) {
      SW_DO_MF(OM_NEW);
      SW_DO_MF(OM_DISPOSE);
      SW_DO_MF(MUIM_Setup);
      SW_DO_MF(MUIM_Cleanup);
      SW_DO_MF(SWM_FAQList_UpdList);
      SW_DO_MF(SWM_FAQList_FetchFAQList);
      SW_DO_MF(SWM_FAQList_FetchFAQList_P1);
      SW_DO_MF(SWM_FAQList_FetchFAQ);
      SW_DO_MF(SWM_FAQList_FetchFAQ_P1);
      SW_DO_MF(SWM_FAQList_AbortFAQ);
      SW_DO_MF(SWM_FAQList_FAQListClk);
      SW_DO_MF(SWM_FAQList_DelSelFAQ);
      SW_DO_MF(SWM_FAQList_OnOffLine);
      SW_DO_MF(SWM_FAQList_Act);
      SW_DO_MF(SWM_VarList_Search);
   }

   return DoSuperMethodA(cl, o, msg);
}
