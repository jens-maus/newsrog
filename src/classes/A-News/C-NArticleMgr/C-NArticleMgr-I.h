# ifndef C_NARTICLEMGR_INCLUDE_I
#  define C_NARTICLEMGR_INCLUDE_I

#  include "Libraries/C-Root.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-NServerList.h"
#  include "Libraries/C-NGroup.h"
#  include "Libraries/C-NSFList.h"
#  include "Libraries/C-NSFilter.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-NSF-MatchThread.h"
#  include <dos/exall.h>
#  include <dos/stdio.h>


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NArticleMgr
#  define SW_CNAME            "NArticleMgr"
#  define SW_SUPERPUB         SWC_Root
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NArticleMgr_protos.h"
#  include "Libraries/C-NTCPList.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_Err_SaveOV_NoPath = 1,
      SWS_Err_SaveOV_Open,
      SWS_Err_SaveOV_Write,
      SWS_Err_SaveOV_Mkdir,
      SWS_Err_LoadOV_NoPath,
      SWS_Err_LoadOV_Open,
      SWS_Err_LoadOV_Read,
      SWS_Err_LoadOV_Art,
      SWS_Err_LoadOV_HdrCnt,
      SWS_Err_LoadOV_NoMsgID,
      SWS_Err_CleanCache_Lock,
      SWS_Err_CleanCache_Scan,
      SWS_Err_CleanCache_Art,
      SWS_Err_RemOVDir_Rem,
      SWS_Err_RemOVDir_Scan,
      SWS_Err_Alloc,
      SWS_Err_TableFull,
      SWS_Err_NoMsgID,
      SWS_Err_MP_BadSubj,
      
      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      SWV_NArticleMgr_MaxChain      = 256,
      SWV_NArticleMgr_MaxOV         = 20,
      SWV_NArticleMgr_FltStk        = 8,
      SWV_NArticleMgr_MaxLocRef     = 65000,
      SWV_NArticleMgr_MaxGrps       = SWV_NNews_MaxOpenGroups,
      SWV_NArticleMgr_OVLoadInt     = 8192,   // notify UI interval for load OV
      SWV_NArticleMgr_OVLineSize    = 8*1024, // max line size for overview file
      SWV_NArticleMgr_IOBuffSize    = 256*1024,
      SWV_NArticleMgr_ReadListSize  = 8192,
      SWV_NArticleMgr_ReadListBytes = (int)(SWV_NArticleMgr_ReadListSize*sizeof(ULONG)),
      SWV_NArticleMgr_Added         = 0,
      SWV_NArticleMgr_Freed         = 1,
   };
   
#  define RC_SETSIZE 70001

   typedef struct rc_set {
      fd_mask fds_bits[howmany(RC_SETSIZE, NFDBITS)];
   } rc_set;

   typedef struct SW_IDATA
   {
      SWP_NNews_AttrDefs;

      ULONG     SWA_NArticleMgr_Count;

      Object  **SWA_NArticleMgr_Array;
      LONG     *SWA_NArticleMgr_PrevPos;
      UWORD    *SWA_NArticleMgr_LocRefs;
      Object ***SWA_NArticleMgr_Responses;
      ULONG     SWA_NArticleMgr_ArraySize;
      APTR     *SWA_NArticleMgr_MemPool;
      ULONG     SWA_NArticleMgr_Threads;
      ULONG     SWA_NArticleMgr_Master;
      ULONG     SWA_NArticleMgr_SPos;

      ULONG     MaxChain;
      ULONG     ThreadMod;  // threadinfo modified flag

      Object   *FltList[SWV_NArticleMgr_FltStk];

      ULONG     MTMax;  // count of array below
      Object  **MTFlt;  // headermatch filters
      UBYTE    *MTUse;  // in use markers

      ULONG     FltPos;
      ULONG     FltOK;

      Object   *Group[SWV_NArticleMgr_MaxGrps];
      ULONG     GrpCnt[SWV_NArticleMgr_MaxGrps];

      ULONG     ReadListSet;
      LONG      ReadListFirst;
      LONG      ReadListLast;
      ULONG    *ReadList;
      rc_set   *ReadSet;
      
      Object   *Obj;
   } SW_IDATA;

#  define SWV_DefOVFile    "OV.Headers"
#  define SWV_DefOVData    "OV.Data"
#  define SWV_OVD_Records  "Records: "
#  define SWV_OVD_Group    "Group: "
#  define SWV_RL_FSpec     "ReadCache"

#  define AV(s)    (SWPoolAV(idata->SWA_NArticleMgr_MemPool, (s)))
#  define FV(m)     SWPoolFV(idata->SWA_NArticleMgr_MemPool, (STRPTR)(m))
#  define RV(s,m)  (SWPoolRV(idata->SWA_NArticleMgr_MemPool, (s), (STRPTR *)(m)))
#  define SDup(s)  (SWPoolStrDup(idata->SWA_NArticleMgr_MemPool, (s)))

   // -- just some shorthand macros ------------------------------------------

#  define AArray()    (idata->SWA_NArticleMgr_Array)
#  define ASize()     (idata->SWA_NArticleMgr_ArraySize)
#  define ACount()    (idata->SWA_NArticleMgr_Count)
#  define Art(x)      (idata->SWA_NArticleMgr_Array[x])
#  define PrevPos(x)  (idata->SWA_NArticleMgr_PrevPos[x])
#  define LocRef(x)   (idata->SWA_NArticleMgr_LocRefs[x])
#  define RespA(x)    (idata->SWA_NArticleMgr_Responses[x])
#  define Resp(x,y)   (idata->SWA_NArticleMgr_Responses[x][y])

   // -- loop through all articles with refresh ---------------------------

#  define ALoopAll(idata, x, c, chk, size, array)                          \
     for((x)=(c)=StartBusy(idata); (x)<size ? 1 : EndBusy(idata); (x)++)   \
        if (array[x] ? (((c)=((c)>=(chk)?CheckRefresh(idata):(c)+1)),1):0)
   
   // -- this ugly-ass macro loops through the a table, selecting
   // -- articles that match a set of NSFilter conditions.  you`re
   // -- better off not knowing.  really.

#  define ALoop(idata, x, msg, c, chk, size, array, thrdmode)             \
     for(StartEndMatch(idata, msg, SWM_NSFilter_Begin, thrdmode),         \
         (x)=(c)=StartBusy(idata);                                        \
         (x)<size ? 1 : (EndBusy(idata),                                  \
                         StartEndMatch(idata, msg, SWM_NSFilter_End,      \
                                       thrdmode));                        \
         (x)++)                                                           \
        if (array[x] &&                                                   \
            (IsMatch(idata, array[x], msg) ?                              \
             (((c)=((c)>=(chk)?CheckRefresh(idata):(c)+1)), 1) : 0))

#  define ACheckR(count, maxc)  if (count>=maxc)  {count=0;CheckRefresh(idata);}
#  define ACheckRI(count, maxc) if (count++>=maxc){count=0;CheckRefresh(idata);}

# endif // C_NARTICLEMGR_INCLUDE_I
