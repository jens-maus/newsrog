# include "C-NArticle-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NArticle

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_IN(SWA_NArticle_Followups,  SWAM_NoLab, SWAM_SGIN, 0),
   SWAM_IN(SWA_NArticle_Lines,      SWAM_NoLab, SWAM_SGIN, 0),
   SWAM_PT(SWA_NArticle_Txt,        SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_PT(SWA_NArticle_MemPool,    SWAM_NoLab, SWAM_SGIN, NULL),
   SWAM_FI(SWA_NArticle_NewState,   SWAM_NoLab, SWAM_Get,  0),
   SWAM_FI(SWA_NArticle_PctDL,      SWAM_NoLab, SWAM_Get,  0),
   SWAM_FI(SWA_NArticle_IDataSize,  SWAM_NoLab, SWAM_Get,
           sizeof(SW_IDATA) + 48),

   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Cache file read failed: %s",
   "Cache file is unknown format: %s",
   "Error writing text to file: %s",
   "Error reading text from file: %s",

   NULL,
};

# include "C-NArticle-Mthd.h"

struct Locale *Loc = NULL;

# define SW_USERSETUP   Loc = OpenLocale(NULL);
# define SW_USERCLEANUP CloseLocale(Loc); Loc = NULL;

# define DISPOSECUST    ClearBuffLines(o, idata); \
                        ResetCacheValues(idata);

# define SW_NOSETUPSET
# define SW_SETUP
static __asm __inline ULONG SWSetup(SETUPFNPROTO)
   SDST(ULONG)
{
   TagItem    *tstate = ti;
   memset(idata, 0, sizeof(SW_IDATA));           // clear our data space
   ResetCacheValues(idata);

   while (ti = NextTagItem(&tstate)) {
      switch (ti->ti_Tag) {
        case SWA_NArticle_Lines:
         idata->SWA_NArticle_Lines  = (LONG)ti->ti_Data; break;
        case SWA_NArticle_Txt:
         idata->SWA_NArticle_Txt    = (STRPTR *)ti->ti_Data; break;
        case SWA_NArticle_MemPool:
         idata->SWA_NArticle_MemPool= (APTR)ti->ti_Data;  break;
      }
   }
   
   SWDB_RET(retval);
} SDFT


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_METHOD
static __asm __inline ULONG SWMethod(METHODFNPROTO)
{
   SW_IDATA *idata = INST_DATA(cl, o);

   switch (msg->MethodID) {
      SW_DO_MF(SWM_NArticle_Cmp);
      SW_DO_MF(SWM_NArticle_Refs);
      SW_DO_MF(SWM_NArticle_GetMsgID);
      SW_DO_MF(SWM_NArticle_GetSubj);
      SW_DO_MF(SWM_NArticle_FindHeader);
      SW_DO_MF(SWM_NArticle_GetIDHash);
      SW_DO_MF(SWM_NArticle_OVInfo);
      SW_DO_MF(SWM_NArticle_GetBytes);
      SW_DO_MF(SWM_NArticle_IncFollowups);
      SW_DO_MF(SWM_NArticle_Search);
      SW_DO_MF(SWM_NArticle_Fetch);
      SW_DO_MF(SWM_NArticle_Fetch_P);
      SW_DO_MF(SWM_NArticle_FreeText);
      SW_DO_MF(SWM_NArticle_Cache);
      SW_DO_MF(SWM_NArticle_UnCache);
      SW_DO_MF(SWM_NArticle_GetCache);
      SW_DO_MF(SWM_NArticle_SaveText);
      SW_DO_MF(SWM_NArticle_LoadText);
      SW_DO_MF(SWM_NArticle_Clipboard);
      SW_DO_MF(SWM_NArticle_Abort);
      SW_DO_MF(SWM_NArticle_AddLines);
      SW_DO_MF(SWM_NArticle_GroupCount);
      SW_DO_MF(SWM_NArticle_InGroup);
      SW_DO_MF(SWM_NArticle_Age);
      SW_DO_MF(SWM_NArticle_FlagSet);
      SW_DO_MF(SWM_NArticle_FlagReset);
      SW_DO_MF(SWM_NArticle_FlagTest);
      SW_DO_MF(SWM_NArticle_SetPerm);
      SW_DO_MF(SWM_NArticle_ParseName);
      SW_DO_MF(SWM_NArticle_HashFName);
      SW_DO_MF(SWM_NArticle_Kill);
      SW_DO_MF(SWM_NArticle_FastUnCache);
      SW_DO_MF(SWM_NArticle_CanExpire);
      SW_DO_MF(SWM_NArticle_ByteSize);
      SW_DO_MF(SWM_NArticle_ParseComment);
      SW_DO_MF(SWM_NArticle_HasParts);
      SW_DO_MF(SWM_NArticle_ParseDate);
      SW_DO_MF(SWM_NArticle_AddSeparator);
      SW_DO_MF(SWM_NArticle_FixISOHdrs);
      SW_DO_MF(SWM_NArticle_FixHdrCase);
      SW_DO_MF(SWM_NArticle_ClipHeader);
      SW_DO_MF(SWM_NArticle_GetLines);
      SW_DO_MF(SWM_NArticle_ParseMPInfo);
      SW_DO_MF(SWM_NArticle_GetTxt);
   }

   switch (msg->MethodID) {
      SW_DO_MF(SWM_VarList_Search);
      SW_DO_MF(SWM_AppBase_RexxCmd);
   }

  SuperMethod:
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT

# define  SW_NOROOTMETHODS
# include "S-DefAll.h"
# include "S-Dispatcher.h"
