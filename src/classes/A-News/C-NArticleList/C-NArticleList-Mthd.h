# include "S-Hash.h"

# define USE_Broken_COLORS
# define USE_Broken_BODY
# define USE_NewArt_BODY
# include "BM_Broken.h"
# include "BM_NewArt.h"
# include "MBM_Broken.h"

/*****************************************************************************
 * Method: MUI Setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   if (!DoSuperMethodA(cl, o, msg)) SWDB_RET(FALSE);

#  define IObj(o) (ImageObject, MUIA_Image_Spec, (o), End)
   
   SWMakeImg(idata, SWV_NAL_Img_Disk,   IObj(MUII_Disk));
   SWMakeImg(idata, SWV_NAL_Img_Drawer, IObj(MUII_Drawer));
   SWMakeImg(idata, SWV_NAL_Img_Net,    IObj(MUII_Network));
   SWMakeImg(idata, SWV_NAL_Img_Broken, MakeBrokenBitm((UBYTE *)Broken_body));
   SWMakeImg(idata, SWV_NAL_Img_NewArt, MakeBrokenBitm((UBYTE *)NewArt_body));

   SWCreateImages(idata);
   
   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: MUI Cleanup                                                       *
 *****************************************************************************/
   
METHOD(MUIM_Cleanup, Msg)
{
   SWDeleteImages(idata);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: MUI Cleanup                                                       *
 *****************************************************************************/
   
METHOD(SWM_NArticleList_Click, SWP_NArticleList_Click *)
{
   LONG  Col = msg->Column;
   Object *Active;

   DoMethod(o, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &Active);
   if (!Active) SWDB_RET(0);

   idata->SWA_NArticleList_ClickItem = Active;

   if (Col != SWV_NArticleList_KeyCC)
      Col = DoMethod(idata->SWA_NArticleList_ListCol, SWM_ListCol_TrueCol, Col);

   if (Col >= 0)
      set(o, SWA_NArticleList_ClickAttr, Col);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: search through article head/body                                *
 *****************************************************************************/

static ULONG AL_ArtSearch(SWP_ListSearch_Cmp *msg, ULONG Parts)
{
   ULONG Found;
   
   Found = DoMethod(msg->Entry, SWM_NArticle_Search,
                    msg->Wild ? SWT_NArticle_ParsedPattern :
                                SWT_NArticle_Pattern, msg->Pat,
                    SWT_NArticle_Wild,     msg->Wild,
                    SWT_NArticle_Parts,    Parts,
                    SWT_NArticle_CaseSens, msg->Case,
                    TAG_DONE) ?
        SWV_ListSearch_CmpMethod_Found :
        SWV_ListSearch_CmpMethod_NotFound;

   return Found;
}

/*****************************************************************************
 * Function: search through article head/body, display art, highlight match  *
 *****************************************************************************/

static ULONG AL_ArtSearchDisp(SW_IDATA *idata, SWP_ListSearch_Cmp *msg)
{
   ULONG rc = FALSE, First = 0;

   if (idata->LastSrchArt && idata->LastSrchArt != msg->Entry)
      if (!idata->SWA_NArticleList_QSFromRam ||
          AvailMem(MEMF_FAST) < SWV_NGroup_QS_MinFree)
         DoMethod(idata->LastSrchArt, SWM_NArticle_FreeText, SWF_NArticle_Body);

   if (DoMethod(msg->Entry, SWM_NArticle_FlagTest, SWF_NArticle_CachedHdBd) &&
       DoMethod(msg->Entry, SWM_NArticle_GetCache,
                SWT_NArticle_MsgID,    DoMethod(msg->Entry,
                                                SWM_NArticle_GetMsgID),
                SWT_NArticle_Parts,    SWF_NArticle_HdBd,
                SWT_NArticle_CacheDir, DoMethod(idata->SWA_NArticleList_Group,
                                                SWM_NGroup_OVDir),
                TAG_DONE) == SUCCESS) {

      rc = AL_ArtSearch(msg, idata->SWA_NArticleList_QSH == 14 ?
                        SWF_NArticle_Body : SWF_NArticle_HdBd);

      if (rc == SWV_ListSearch_CmpMethod_Found &&
          idata->SWA_NArticleList_QSUpdDisp) {
         Object *ArtView = SWGetObj(idata->SWA_NArticleList_Group,
                                    SWA_NGroup_ArtView);
         
         if (idata->LastSrchArt != msg->Entry)
            DoMethod(idata->SWA_NArticleList_Group, SWM_NGroup_DLQAdd,
                     msg->Entry, SWV_NGroup_DLQ_Read, TRUE);

         if (idata->SWA_NArticleList_QSH == 14)
            First = SWGetUL(ArtView, SWA_NArticleView_FirstBodyLine);
         
         DoMethod(ArtView, SWM_NArticleView_Search,
                  msg->Pat, msg->Case, msg->Len, msg->SubLine, msg->Wild,
                  First, msg->OrigStr);
      }

      idata->LastSrchArt = msg->Entry;
   }

   return rc;
}


/*****************************************************************************
 * Method: Comparison for searching/selecting articles                       *
 *****************************************************************************/
   
METHOD(SWM_NArticleList_SrchCmp, SWP_ListSearch_Cmp *)
{
   // -- look for some special cases -----------------------------------------
   
   switch (idata->SWA_NArticleList_QSH) {
     case 0:
      SWDB_RET(DoMethod(msg->Entry, SWM_NArticle_GetSubj));
      
     case 13: // search through all headers
      SWDB_RET(AL_ArtSearch(msg, SWF_NArticle_Head));

     case 14:
     case 15:
      SWDB_RET(AL_ArtSearchDisp(idata, msg));
   }

   // -- if not a special case, just return that particular header -----------
   
   SWDB_RET(DoMethod(msg->Entry, SWM_NArticle_FindHeader,
                     HEntries[idata->SWA_NArticleList_QSH]));
} SDFT


/*****************************************************************************
 * Method: Change title                                                      *
 *****************************************************************************/

METHOD(SWM_NArticleList_ChangeTitle, SWP_NArticleList_ChangeTitle *)
{
   ULONG Entries = msg->Count ? msg->Count :
      SWGetUL(idata->SWA_NList_L, MUIA_List_Entries);
   
   DoMethod(idata->Title,
            MUIM_SetAsString, MUIA_Text_Contents,
            SW_STRDEF(idata->SWA_NArticleList_Title, GetS(SWS_L_Title)),
            Entries);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Generate context lines around an article                          *
 *****************************************************************************/

METHOD(SWM_NArticleList_CtxCenter, SWP_NArticleList_CtxCenter *)
{
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: set up CmpInfo                                                  *
 *****************************************************************************/

static void InitCmpInfo(SW_IDATA *idata)
{
   idata->CmpInfo.Count    = 3;
   idata->ArtMgr           = idata->SWA_NArticleList_ThreadMgr;
}


/*****************************************************************************
 * Method: Delete articles from our list of displayed ones                   *
 *****************************************************************************/
   
METHOD(SWM_NArticleMgr_DelArts, Msg *)
{
   idata->LastSrchArt = NULL;
   
   SWDB_RET(0);
} SDFT
