# include "C-NGroup-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NGroup

// -- attribute map ----------------------------------------------------------

# define IntStrUI SWAM_UI|SWAM_IntStr
# define BEG SWAM_NewHG|SWAM_EndG

static STRPTR CELab[SWV_NGroup_CE_MAX+1];
static STRPTR NALab[SWV_NGroup_NA_MAX+1];
static STRPTR CfgLab[5];
   
# define BG SWAM_NewCG2
# define EG SWAM_EndG

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_UICTL(SWAM_NewVG,  0, 0, 0),
   SWAM_UICTL(SWAM_NewRG,  SWS_Tab_Misc, 3, CfgLab),
   SWAM_SA(SWA_NGroup_Desc,            SWS_L_Desc, SWAM_UI|BG,   NULL, 256),
   SWAM_TX(SWS_L_Srv, NULL),
   SWAM_SA(SWA_NGroup_Server1,         SWAM_NoLab,  SWAM_UI|BEG,NULL, 256),
   SWAM_SA(SWA_NGroup_SigFile,         SWS_L_SigF, SWAM_UI | SWAM_PopFile,
           NULL, FMSIZE),
   SWAM_SA(SWA_NGroup_ArtViewTitle,    SWS_L_AVT,  SWAM_UI,      NULL, 1024),
   SWAM_IB(SWA_NGroup_MaxArticles,     SWS_L_MaxA, IntStrUI,  200,  10, 10000),
   SWAM_IB(SWA_NGroup_MaxKeep,         SWS_L_MaxK, IntStrUI, 10001,  0, 10001),
   SWAM_TX(SWS_L_ArtFont, NULL),
   SWAM_BO(SWA_NGroup_UseArtFont,      SWAM_NoLab, SWAM_UI|SWAM_Notify|SWAM_NewHG, FALSE),   
   SWAM_PF (SWA_NGroup_ArtFont,        SWAM_NoLab, SWAM_UI|SWAM_Notify|SWAM_EndG, "helvetica/13",
            MAXFONTNAME),
   SWAM_TX(SWS_L_ArtCol, NULL),
   SWAM_BO(SWA_NGroup_UseArtCol,       SWAM_NoLab, SWAM_UI|SWAM_Notify|SWAM_NewHG, FALSE),   
   SWAM_PA (SWA_NGroup_ArtCol,         SWAM_NoLab, SWA_NGroup_Pens,
            PEN_ArtCol, MPEN_TEXT, SWAM_UI|SWAM_EndG),
   SWAM_CY(SWA_NGroup_NewArtMd,        SWS_L_NaMd, SWAM_UI,
           SWV_NGroup_NA_Exit, SWV_NGroup_NA_MAX, NALab),
   SWAM_BO(SWA_NGroup_IncrDisp,        SWS_L_IncD, SWAM_UI|EG, TRUE),
   SWAM_CY(SWA_NGroup_CacheExit,       SWS_L_CatE, SWAM_UI|BG,
           SWV_NGroup_CE_Ask, SWV_NGroup_CE_MAX, CELab),
   SWAM_IB(SWA_NGroup_MemCacheNum,     SWS_L_MmCn, SWAM_UI,   5, 1, 10000),
   SWAM_IB(SWA_NGroup_MemCacheKb,      SWS_L_MmCs, SWAM_UI|SWAM_KbSld, 256, 1, 10*1000),
   SWAM_IB(SWA_NGroup_PrefetchNum,     SWS_L_PreF, SWAM_UI,      1, 0, 100),
   SWAM_BO(SWA_NGroup_ReadnCache,      SWS_L_RCach,SWAM_UI|EG,   FALSE),

   SWAM_BO(SWA_NGroup_DLHdrStart,      SWS_L_DLHd, SWAM_UI|BG,TRUE),
   SWAM_BO(SWA_NGroup_FullHeaders,     SWS_L_FlHd, SWAM_UI,   FALSE),
   SWAM_BO(SWA_NGroup_ReadOnExit,      SWS_L_RonE, SWAM_UI|EG,FALSE),
   SWAM_UICTL(SWAM_EndG, 0, 0, 0),
   SWAM_HVS(1, 500),
   SWAM_SA(SWA_NGroup_Name,            SWAM_NoLab, SWAM_SGI,  NULL, 200),
   SWAM_IB(SWA_NGroup_ExpireDays,      SWAM_NoLab, SWAM_SGI,  365,  1,  1000),
   SWAM_IN(SWA_NGroup_RdDays,          SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_RdMin,           SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_RdTick,          SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_HistArtCount,    SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_HistArtKill,     SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_HistArtRead,     SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_PrevArtNum,      SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_FirstArtNum,     SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_LastArtNum,      SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_AvailArtNum,     SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_NewArtNum,       SWAM_NoLab, SWAM_SGI,  0),
   SWAM_PT(SWA_NGroup_Status,          SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_PT(SWA_NGroup_MemPool,         SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_PT(SWA_NGroup_ArtMgr,          SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_PT(SWA_NGroup_ArtView,         SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_BO(SWA_NGroup_ReaderOpen,      SWAM_NoLab, SWAM_SGIN, FALSE),
   SWAM_PT(SWA_NGroup_SrvList,         SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_OB(SWA_NGroup_ThreadMgr,       SWAM_NoLab, SWAM_SGIN),
   SWAM_IN(SWA_NGroup_ThreadFT,        SWAM_NoLab, SWAM_SGI,  SWF_NGroup_ThreadFlt_Base),
   SWAM_IN(SWA_NGroup_ReadFT,          SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_AuthFT,          SWAM_NoLab, SWAM_SGI,  0),
   SWAM_SA(SWA_NGroup_AuthFTName,      SWAM_NoLab, SWAM_SGI,  NULL, 256),
   SWAM_OB(SWA_NGroup_SelFlt,          SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB(SWA_NGroup_KillFlt,         SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB(SWA_NGroup_GrpDefFlt,       SWAM_NoLab, SWAM_SGI),
   SWAM_OB(SWA_NGroup_DispFlt,         SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_OB(SWA_NGroup_ActFlt,          SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_IN(SWA_NGroup_Sort1,           SWAM_NoLab, SWAM_SGI,  SWV_NArticle_SortRef),
   SWAM_IN(SWA_NGroup_Dir1,            SWAM_NoLab, SWAM_SGI,  SWV_NArticle_ForwardSort),
   SWAM_IN(SWA_NGroup_Sort2,           SWAM_NoLab, SWAM_SGI,  SWV_NArticle_Off),
   SWAM_IN(SWA_NGroup_Dir2,            SWAM_NoLab, SWAM_SGI,  SWV_NArticle_ForwardSort),
   SWAM_IN(SWA_NGroup_Sort3,           SWAM_NoLab, SWAM_SGI,  SWV_NArticle_Off),
   SWAM_IN(SWA_NGroup_Dir3,            SWAM_NoLab, SWAM_SGI,  SWV_NArticle_ForwardSort),
   SWAM_IN(SWA_NGroup_ReadArtCnt,      SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_TotArtCnt,       SWAM_NoLab, SWAM_SGI,  0),
   SWAM_BO(SWA_NGroup_BadGroup,        SWAM_NoLab, SWAM_SGI,  FALSE),
   SWAM_IN(SWA_NGroup_Flags,           SWAM_NoLab, SWAM_SGIN, 0),
   SWAM_FI(SWA_NGroup_HdrPctDone,      SWAM_NoLab, SWAM_SGIN, 0),
   SWAM_FI(SWA_NGroup_DLQPctDone,      SWAM_NoLab, SWAM_SGIN, 0),
   SWAM_OB(SWA_NGroup_ArtListCol,      SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_SA(SWA_NGroup_LastFile,        SWAM_NoLab, SWAM_SGI,  "",     FMSIZE),
   SWAM_SA(SWA_NGroup_LastDir,         SWAM_NoLab, SWAM_SGI,  "RAM:", FMSIZE),
   SWAM_SA(SWA_NGroup_LastPatt,        SWAM_NoLab, SWAM_SGI,  "#?",   FMSIZE),
   SWAM_IN(SWA_NGroup_PermFlags,       SWAM_NoLab, SWAM_SGI,  0),
   SWAM_SA(SWA_NGroup_LastMsgID,       SWAM_NoLab, SWAM_SGI,  NULL,   1024),
   SWAM_SA(SWA_NGroup_LastRdSrv,       SWAM_NoLab, SWAM_SGI,  NULL,   80),
   SWAM_IN(SWA_NGroup_LastRdMsgE,      SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_LastRdMsgC,      SWAM_NoLab, SWAM_SGI,  0),
   SWAM_IN(SWA_NGroup_Moderated,       SWAM_NoLab, SWAM_SGI,  0),
   
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


/*****************************************************************************
 * Function: new global selection filter                                     *
 *****************************************************************************/

static Object *NewGlobSel(Object *o, SW_IDATA *idata, ULONG Use)
{
   return MUI_NewObject(SWC_NSFList,
                        SWA_NSFList_Use,        Use,
                        AppBaseTopObj(o),
                        TAG_DONE);
}


static ULONG DoSetup(Object *o, SW_IDATA *idata)
{
   ULONG rc = FAILURE;

   idata->SWA_NGroup_GrpDefFlt  = MUI_NewObject(SWC_NSF_InGroup, TAG_DONE);
   
   idata->SWA_NGroup_SelFlt     = NewGlobSel(o, idata, SWV_NSFList_Use_Select);
   idata->SWA_NGroup_KillFlt    = NewGlobSel(o, idata, SWV_NSFList_Use_Kill);
   idata->SWA_NGroup_DispFlt    = MUI_NewObject(SWC_NSFList, TAG_DONE);
   idata->SWA_NGroup_ActFlt     = MUI_NewObject(SWC_NSFList, TAG_DONE);

   if (!idata->SWA_NGroup_SelFlt     ||
       !idata->SWA_NGroup_KillFlt    ||
       !idata->SWA_NGroup_DispFlt    ||
       !idata->SWA_NGroup_ActFlt     ||
       !idata->SWA_NGroup_GrpDefFlt) goto Done;

   idata->SWA_NGroup_Status = GetS(SWS_L_Status_Idle);

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   rc = SUCCESS;

  Done:
   if (rc != SUCCESS) {
      MUI_DisposeObject(idata->SWA_NGroup_SelFlt);
      MUI_DisposeObject(idata->SWA_NGroup_KillFlt);
      MUI_DisposeObject(idata->SWA_NGroup_GrpDefFlt);
      MUI_DisposeObject(idata->SWA_NGroup_DispFlt);
      MUI_DisposeObject(idata->SWA_NGroup_ActFlt);
   }
   
   SWDB_RET(rc);
}

static void SetTopObj(SW_IDATA *idata, Object *NewsObj)
{
   Object *rlc;

   idata->SWA_AppBase_TopObj = NewsObj;
   
   idata->SWA_NGroup_MemPool = NN_MemPool(idata);
   idata->SWA_NGroup_ArtMgr  = NN_ArtMgr(idata);
   idata->SWA_NGroup_SrvList = NN_ServList(idata);

   DoMethod(NewsObj, MUIM_MultiSet,
            SWA_AppBase_TopObj, NewsObj,
            idata->SWA_NGroup_SelFlt,
            idata->SWA_NGroup_KillFlt,
            idata->SWA_NGroup_DispFlt,
            idata->SWA_NGroup_ActFlt,
            NULL);

   // -- setup group`s listcol -----------------------------------------------
   
   if (!(rlc = SWGetObj(NewsObj, SWA_NNews_ArtListCol)))
      return;

   if (!idata->SWA_NGroup_ArtListCol) {
      idata->SWA_NGroup_ArtListCol = (Object *)
         DoMethod(rlc, SWM_Clone);

      if (!idata->SWA_NGroup_ArtListCol)
         return;
   }

   SetAttrs(idata->SWA_NGroup_ArtListCol,
            SWA_ListCol_ColSpecs, SWGetStr(rlc, SWA_ListCol_ColSpecs),
            SWA_ListCol_ColNames, SWGetStr(rlc, SWA_ListCol_ColNames),
            TAG_DONE);
}

   
# define SW_SETUP_POSTSET   if (DoSetup((Object *)retval, idata) != SUCCESS) \
                              SWDB_RET(NULL);

static void NukeObj(Object *o, SW_IDATA *idata)
{
   DoMethod(o, SWM_NGroup_CloseFAQList);

   DoMethod(o, SWM_NGroup_StopRead,
            !(idata->SWA_NGroup_Flags & SWF_NGroup_NoSaveOV), TRUE);

   FreeVec(idata->VarBuf);

   NukeArtArrays(idata);
   FreeFetchData(idata);

   if (idata->DBOut) Close(idata->DBOut);
}

# define DISPOSECUST NukeObj(o, idata);

# define SW_USERSET \
     if (Tag == SWA_NGroup_Name)    InitNameFields(o, idata, (STRPTR)Data); \
     if (Tag == SWA_AppBase_TopObj) SetTopObj(idata, (Object *)Data)

# define SW_USERSETUP if (FirstTime) \
                         SWLocalizeNewMenu(SW_CATALOG, SW_CATSTR, MainMenu); \
                      FAQListInit(libbase);

# define SW_USERCLEANUP  FAQListExit();


# define SW_Methods                      \
  SW_DO_MF(SWM_NGroup_CheckStats);       \
  SW_DO_MF(SWM_NGroup_CheckStats_P);     \
  SW_DO_MF(SWM_NGroup_ReadDesc);         \
  SW_DO_MF(SWM_NGroup_ReadDesc_P);       \
  SW_DO_MF(SWM_NGroup_Fetch);            \
  SW_DO_MF(SWM_NGroup_Fetch_P1);         \
  SW_DO_MF(SWM_NGroup_Fetch_P2);         \
  SW_DO_MF(SWM_NGroup_Abort);            \
  SW_DO_MF(SWM_NGroup_Abbrev);           \
  SW_DO_MF(SWM_NGroup_FetchAdd);         \
  SW_DO_MF(SWM_NGroup_OpenReaderWin);    \
  SW_DO_MF(SWM_NGroup_CloseReaderWin);   \
  SW_DO_MF(SWM_NGroup_StartRead);        \
  SW_DO_MF(SWM_NGroup_StopRead);         \
  SW_DO_MF(SWM_NGroup_OVName);           \
  SW_DO_MF(SWM_NGroup_SaveOV);           \
  SW_DO_MF(SWM_NGroup_LoadOV);           \
  SW_DO_MF(SWM_NGroup_MenuCall);         \
  SW_DO_MF(SWM_NGroup_ArtListClick);     \
  SW_DO_MF(SWM_NGroup_ArtListDClick);    \
  SW_DO_MF(SWM_NGroup_ChgSelFlt);        \
  SW_DO_MF(SWM_NGroup_ChgDspFlt);        \
  SW_DO_MF(SWM_NGroup_RunActFlt);        \
  SW_DO_MF(SWM_NGroup_ChgUIFlt);         \
  SW_DO_MF(SWM_NGroup_ChgThreadFlt);     \
  SW_DO_MF(SWM_NGroup_ChgReadFlt);       \
  SW_DO_MF(SWM_NGroup_ChgAuthFlt);       \
  SW_DO_MF(SWM_NGroup_DeleteOVDir);      \
  SW_DO_MF(SWM_NGroup_Mark);             \
  SW_DO_MF(SWM_NGroup_CacheSel);         \
  SW_DO_MF(SWM_NGroup_DLFetchNext);      \
  SW_DO_MF(SWM_NGroup_TestOVDir);        \
  SW_DO_MF(SWM_NGroup_CleanCache);       \
  SW_DO_MF(SWM_NGroup_Cmp);              \
  SW_DO_MF(SWM_NGroup_GL_Hdrs);          \
  SW_DO_MF(SWM_NGroup_GL_Hdrs_P);        \
  SW_DO_MF(SWM_NGroup_GL_Arts);          \
  SW_DO_MF(SWM_NGroup_GL_Arts_P1);       \
  SW_DO_MF(SWM_NGroup_GL_Arts_P2);       \
  SW_DO_MF(SWM_NGroup_NoSaveOV);         \
  SW_DO_MF(SWM_NGroup_GL_Mark);          \
  SW_DO_MF(SWM_NGroup_OnOffLine);        \
  SW_DO_MF(SWM_NGroup_KillSel);          \
  SW_DO_MF(SWM_NGroup_Browse);           \
  SW_DO_MF(SWM_NGroup_MarkRead);         \
  SW_DO_MF(SWM_NGroup_SaveArt);          \
  SW_DO_MF(SWM_NGroup_ExpireOld);        \
  SW_DO_MF(SWM_NGroup_ExpireOld_P1);     \
  SW_DO_MF(SWM_NGroup_Post);             \
  SW_DO_MF(SWM_NGroup_PostNew);          \
  SW_DO_MF(SWM_NGroup_PostFup);          \
  SW_DO_MF(SWM_NGroup_ALQSWin);          \
  SW_DO_MF(SWM_NGroup_OVDir);            \
  SW_DO_MF(SWM_NGroup_DLQAdd);           \
  SW_DO_MF(SWM_NGroup_ALSearchUIUpd);    \
  SW_DO_MF(SWM_NGroup_LimitToSel);       \
  SW_DO_MF(SWM_NGroup_OpenFAQList);      \
  SW_DO_MF(SWM_NGroup_CloseFAQList);     \
  SW_DO_MF(SWM_NGroup_ChgHLFlt);         \
  SW_DO_MF(SWM_NGroup_UpdArtList);       \
  SW_DO_MF(SWM_NGroup_Fetch_XOV_P1);     \
  SW_DO_MF(SWM_NGroup_Fetch_XOV_P2);     \
  SW_DO_MF(SWM_NGroup_ToggleWins) ;      \
  SW_DO_MF(SWM_NGroup_GroupStats) ;      \
  SW_DO_MF(SWM_NGroup_CloseStatWin) ;    \
  SW_DO_MF(SWM_NGroup_MPSelect);         \
  SW_DO_MF(SWM_NGroup_MPCheck);          \
  SW_DO_MF(SWM_NGroup_MPOne);            \
  SW_DO_MF(SWM_NGroup_MPAll);            \
  SW_DO_MF(SWM_NGroup_WinCtrl);          \
  SW_DO_MF(SWM_NGroup_Redisp);           \
  SW_DO_MF(SWM_NGroup_Select);           \
  SW_DO_MF(SWM_NGroup_Remove);           \
  SW_DO_MF(SWM_NGroup_UnRemove);         \
  SW_DO_MF(SWM_NGroup_ViewMsgID);        \
  SW_DO_MF(SWM_NArticle_StatMsg);        \
  SW_DO_MF(SWM_NArticleMgr_NewArticles); \
  SW_DO_MF(SWM_NArticleMgr_DelArticles); \
  SW_DO_MF(SWM_VarList_Search);          \
  SW_DO_MF(SWM_Root_RequestFNData);      \
  SW_DO_MF(SWM_NNews_NewProj);           \
  SW_DO_MF(SWM_AppBase_RexxCmd);         \

# include "C-NGroup-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
