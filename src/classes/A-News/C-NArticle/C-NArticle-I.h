# ifndef C_NARTICLE_INCLUDE_I
#  define C_NARTICLE_INCLUDE_I

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/L-Support.h"
#  include "Libraries/C-NArticle.h"
#  include "Libraries/C-NArticleMgr.h"
#  include "Libraries/C-NArticleView.h"
#  include "Libraries/C-NDFilter.h"
#  include "Libraries/C-NServerList.h"
#  include "Libraries/C-NNews.h"
#  include "Libraries/C-VarList.h"
#  include <dos/datetime.h>
#  include <ctype.h>


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_NArticle
#  define SW_CNAME            "NArticle"
#  define SW_SUPERPUB         MUIC_Notify
#  define SW_CATVER           1

#  include "S-Params.h"
#  include "Proto/NArticle_protos.h"

   // -- catalog strings (must match with SW_CATSTR) -------------------------

   enum {
      SWS_Err_FileRead = 1,
      SWS_Err_FileFormat,
      SWS_Err_WriteTxt,
      SWS_Err_ReadTxt,

      SWS_LASTSTR,
   };

#  include "S-CatMsgs.h"

   // -- our instance specific data ------------------------------------------

   enum {
      SWF_NAF_IntFlags     = (0),
      SWF_NAF_ExtFlags     = (~(SWF_NAF_IntFlags)),
      SWF_NAF_NoFlagCache  = (1<<15),

      SWV_NAF_PrefsVer     = 1,

      SWV_NAF_EndLine      = -1,

      SWV_NAF_MaxLL        = 4092,
   };

   typedef struct SW_IDATA
   {
      LONG     SWA_NArticle_Followups;
      LONG     SWA_NArticle_Lines;
      STRPTR  *SWA_NArticle_Txt;
      APTR    *SWA_NArticle_MemPool;

      Object   *Srv;
      UWORD    ID;
      WORD     XFerState; // during fetch
      
      WORD     CachedGroupCount;
      UWORD    CachedFlags;
      LONG     CachedAge;
      ULONG    CachedIDHash;
      LONG     Cached_ds_Days;
      LONG     Cached_ds_Minute;
      STRPTR   CachedSubj;
      STRPTR   CachedMsgID;
      ULONG   *CachedRefs;
      LONG     CachedBytes;
      LONG     IncLines;
   } SW_IDATA;

#  define AChkFlg(o,x)    DoMethod(o,SWM_NArticle_FlagTest,(x))
#  define AChkFlgAll(o,x) (AChkFlg(o,x)==(x))
#  define AChkFlgNone(o,x)(AChkFlg(o,x)==(0))
#  define ASetFlg(o,x)    DoMethod(o,SWM_NArticle_FlagSet,(x))
#  define AResetFlg(o,x)  DoMethod(o,SWM_NArticle_FlagReset,(x))
#  define ATxt()          (idata->SWA_NArticle_Txt)
#  define ATxtLn(l)       (idata->SWA_NArticle_Txt[l])
#  define ALines()        (idata->SWA_NArticle_Lines)
#  define AIsOView(o)      AChkFlg(o, SWF_NArticle_OView)
#  define AHasBody(i)     (FirstBodyLine(idata) >= 0)
#  define AHasHead(i)     (ALines() > 0)
#  define AHasHdBd(i)     (AHasBody(i) && AHasHead(i))

#  define AAge(o,i)       ((i)->CachedAge ? (LONG)(i)->CachedAge : \
                           (LONG)DoMethod(o, SWM_NArticle_Age, NULL))

#  define AFlags(o,i)     (!FIsSet((i)->CachedFlags, SWF_NAF_NoFlagCache) ? \
                           (ULONG)(i)->CachedFlags : GetState(i))

#  define AFlagCmp(f1,f2,f) (FIsSet(f1,f)&&!FIsSet(f2,f) ? -1 : \
                             FIsSet(f2,f)&&!FIsSet(f1,f) ? 1 : 0)
  
#  define ASubj(o,i)      ((i)->CachedSubj ? (i)->CachedSubj : \
                           (STRPTR)DoMethod(o, SWM_NArticle_GetSubj))

#  define ABytes(o,i)     ((i)->CachedBytes >= 0 ? (i)->CachedBytes : \
                           (LONG)DoMethod(o, SWM_NArticle_GetBytes))

#  define AGrpCnt(o,i)    ((i)->CachedGroupCount>=0 ? (i)->CachedGroupCount : \
                           (LONG)DoMethod(o, SWM_NArticle_GroupCount))

#  define avtod(x) (LONG)(((x)<='9')?(x)-'0':((x)>='a'?((x)-'a'):((x)-'A'))+10)

#  define AV(s)   (SWPoolAV(idata->SWA_NArticle_MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->SWA_NArticle_MemPool, (STRPTR)(m))
#  define RV(s,m) (SWPoolRV(idata->SWA_NArticle_MemPool, (s), (STRPTR *)(m)))
#  define ASize(x) ((x)*sizeof(Object *))
#  define HMatch(c,h) (strncmp((c), (h), sizeof(h)-1) == 0)

#  define IsDigit(c)  ((c)>='0' && (c)<='9')

#  define ALineLoop(x, idata, Parts, StartLine) \
   for ((x) = FirstLine(idata,StartLine,Parts); (x) != SWV_NAF_EndLine; \
        (x) = NextLine(idata,x,Parts))

# endif // C_NARTICLE_INCLUDE_I
