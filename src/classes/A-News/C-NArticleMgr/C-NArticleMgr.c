# include "C-NArticleMgr-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NArticleMgr

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_NArticleMgr_Count,        SWAM_NoLab, SWAM_SGIN, 0),
   SWAM_PT(SWA_NArticleMgr_MemPool,      SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_FI(SWA_NArticleMgr_OVPct,        SWAM_NoLab, SWAM_SGIN, 0),
   SWAM_BO(SWA_NArticleMgr_Threads,      SWAM_NoLab, SWAM_SGIN, FALSE),
   SWAM_BO(SWA_NArticleMgr_Master,       SWAM_NoLab, SWAM_SGIN, TRUE),

   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Error saving overview: no path.",
   "Error saving overview: file open error\nDir=`%s'\nFile=`%s'",
   "Error saving overview: file write error\nDir=`%s'\nFile=`%s'",
   "Error saving overview: directory create error\nDir=`%s'",
   "Error loading overview: no path.",
   "Error loading overview: file open error\nDir=`%s'\nFile=`%s'",
   "Error loading overview: file read error\nDir=`%s'\nFile=`%s'",
   "Error loading overview: article create failed.",
   "Error loading overview: too many header fields.",
   "Error loading overview: no msg ID header.",
   "Error scanning cache directory:\n%s",
   "Error scanning cache directory:\n%s",
   "Error scanning cache directory: article create failed.",
   "Error removing overview directory:\n%s",
   "Error scanning overview directory",
   "Error in Article Manager: out of memory.",
   "Error in Article Manager: hash table full (yikes!).",
   "Unknown Message-ID.",
   "Not a multipart subject.",

   NULL,
};

static STRPTR OVFields[SWV_NArticleMgr_MaxOV+1] = {
   SWF_NAHeader_MsgID,
   SWF_NAHeader_Subject,
   SWF_NAHeader_Lines,
   SWF_NAHeader_Date,
   SWF_NAHeader_Groups,
   SWF_NAHeader_Ref,
   SWF_NAHeader_From,
   SWF_NAHeader_State,
   SWF_NAHeader_X_NewsRd,
   SWF_NAHeader_MimeVer,
   SWF_NAHeader_MimeType,
   SWF_NAHeader_MimeCTE,
   SWF_NAHeader_Followup,
   SWF_NAHeader_Keywords,
   SWF_NAHeader_Organiz,
   SWF_NAHeader_Bytes,
   NULL,
};

static void FreeArticleArray(SW_IDATA *idata);

static void NukeObj(SW_IDATA *idata)
{
   ULONG x;
   
   FreeArticleArray(idata);

   for (x=0; x<SWV_NArticleMgr_FltStk; x++)
      MUI_DisposeObject(idata->FltList[x]);

   for (x=0; x<idata->MTMax; x++)
      MUI_DisposeObject(idata->MTFlt[x]);

   FV(idata->MTFlt);
   FV(idata->MTUse);
   
   FreeVec(idata->ReadList);
   FreeVec(idata->ReadSet);
}

# define SW_SETUP_POSTSET                                              \
  idata->Obj = (Object *)retval;                                       \
  idata->SWA_NArticleMgr_MemPool = (APTR *)NN_MemPool(idata);

# define DISPOSECUST NukeObj(idata);

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static LONG HTSizes[] = {
  2003, 5003, 9007, 14009, 20011, 26003, 35023, 50021, 70001, 100003, 140009,
  180001, 220009, 250007, 0,
};

# define SW_Methods                          \
  SW_DO_MF(SWM_NArticleMgr_Find);            \
  SW_DO_MF(SWM_NArticleMgr_ThreadInfo);      \
  SW_DO_MF(SWM_NArticleMgr_ThreadBase);      \
  SW_DO_MF(SWM_NArticleMgr_ThreadParent);    \
  SW_DO_MF(SWM_NArticleMgr_ThreadRefs);      \
  SW_DO_MF(SWM_NArticleMgr_SameThread);      \
  SW_DO_MF(SWM_NArticleMgr_GetThreadArts);   \
  SW_DO_MF(SWM_NArticleMgr_KillArts);        \
  SW_DO_MF(SWM_NArticleMgr_DelArts);         \
  SW_DO_MF(SWM_NArticleMgr_Cache);           \
  SW_DO_MF(SWM_NArticleMgr_UnCache);         \
  SW_DO_MF(SWM_NArticleMgr_GetCache);        \
  SW_DO_MF(SWM_NArticleMgr_Match);           \
  SW_DO_MF(SWM_NArticleMgr_HashStats);       \
  SW_DO_MF(SWM_NArticleMgr_Add);             \
  SW_DO_MF(SWM_NArticleMgr_SaveOverview);    \
  SW_DO_MF(SWM_NArticleMgr_LoadOverview);    \
  SW_DO_MF(SWM_NArticleMgr_AddGroup);        \
  SW_DO_MF(SWM_NArticleMgr_RemGroup);        \
  SW_DO_MF(SWM_NArticleMgr_MakeThreads);     \
  SW_DO_MF(SWM_NArticleMgr_FilterList);      \
  SW_DO_MF(SWM_NArticleMgr_RemoveOV);        \
  SW_DO_MF(SWM_NArticleMgr_CleanCache);      \
  SW_DO_MF(SWM_NArticleMgr_Clear);           \
  SW_DO_MF(SWM_NArticleMgr_CDGroupName);     \
  SW_DO_MF(SWM_NArticleMgr_ReadListAdd);     \
  SW_DO_MF(SWM_NArticleMgr_ReadListRem);     \
  SW_DO_MF(SWM_NArticleMgr_ReadListLoad);    \
  SW_DO_MF(SWM_NArticleMgr_ReadListSave);    \
  SW_DO_MF(SWM_NArticleMgr_ReadListRemBl);   \
  SW_DO_MF(SWM_NArticleMgr_ReadListUpd);     \
  SW_DO_MF(SWM_NArticleMgr_MultiPart_Find);  \
  SW_DO_MF(SWM_NArticleMgr_MultiPart_Free);  \
  SW_DO_MF(SWM_NArticleMgr_MultiPart_Check); \


# include "C-NArticleMgr-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
