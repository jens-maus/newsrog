
/*****************************************************************************
 * Method: Make & return an image cache list                                 *
 *****************************************************************************/

METHOD(SWM_DTImg_MakeImgCache, Msg)
{
   struct List *ImgCache = AllocVec(sizeof(struct List), MEMF_ANY);

   if (!ImgCache) SWDB_RET(NULL);
   
   NewList(ImgCache);
   
   SWDB_RET(ImgCache);
} SDFT


/*****************************************************************************
 * Function: Unlink & Free a PicNode                                         *
 *****************************************************************************/

static void DisposePicNode(PicNode *node)
{
   Remove(node);

   DisposeDTObject(node->pn_PictureObj);    // free node`s picture object...
   DisposeDTObject(node->pn_PictureObj_G);  // & ghosted version
   DisposeDTObject(node->pn_PictureObj_M);  // & ghosted version
      
   FreeVec(node->pn_Node.ln_Name);          // ... and its name ...
   FreeVec(node);                           // ... and finally the node itself.
}


/*****************************************************************************
 * Method: Free an image cache list                                          *
 *****************************************************************************/

METHOD(SWM_DTImg_FreeImgCache, SWP_DTImg_FreeImgCache *)
{
   Node *node, *next;

   if (!msg->ImgCache) SWDB_RET(SUCCESS);

   for (node = msg->ImgCache->lh_Head; next=node->ln_Succ; node=next)
      DisposePicNode((PicNode *)node);
   
   FreeVec(msg->ImgCache);
   
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Determine if a given mode is a LUT or truecolor mode                      *
 *****************************************************************************/

static LONG IsLUTMode(ULONG ModeID)
{
   if (!CyberGfxBase)          return TRUE;
   if (!IsCyberModeID(ModeID)) return TRUE;

   return (GetCyberIDAttr(CYBRIDATTR_PIXFMT, ModeID) == PIXFMT_LUT8);
}


/*****************************************************************************
 * Function: generate new DT object                                          *
 *****************************************************************************/

static Object *TryDT(SW_IDATA *idata, STRPTR File, struct Screen *Scr,
                     ULONG remap, ULONG fsb)
{
   Object *DT;

   if (SW_STRISNULL(File)) return NULL;
   
   if (DT = NewDTObject(File,
                        DTA_SourceType,        DTST_FILE,
                        DTA_GroupID,           GID_PICTURE,
                        PDTA_Remap,            remap,
                        PDTA_Screen,           Scr,
                        PDTA_FreeSourceBitMap, fsb,
                        OBP_Precision,         idata->SWA_DTImg_Precision,
                        PDTA_DestMode,         remap ? PMODE_V43 : PMODE_V42,
                        TAG_DONE))
      DoMethod(DT, DTM_PROCLAYOUT, NULL, 1);

   return DT;
}


/*****************************************************************************
 * Function: Try to load DT and ghost image                                  *
 *****************************************************************************/

static Object *LoadDTImgs(SW_IDATA *idata, STRPTR FSpec, struct Screen *Scr)
{
   if (idata->DTObj = TryDT(idata, FSpec, Scr, TRUE, TRUE)) {

      // -- try to load the ghosted version ----------------------------------
      
      if (idata->SWA_DTImg_HasGhost) {
         TEXT Ghost[FMSIZE];
         idata->DTObj_G = TryDT(idata, strcat(strcpy(Ghost, FSpec),
                                              GHOST_SUFFIX),
                                Scr, TRUE, TRUE);
      }

      // -- and the masked version -------------------------------------------
      
      if (!SW_STRISNULL(idata->SWA_DTImg_Mask))
         idata->DTObj_M = TryDT(idata, idata->SWA_DTImg_Mask, NULL, FALSE, FALSE);
   }

   return idata->DTObj;
}


/*****************************************************************************
 * Function: Try to load DT and ghost image, looking in path                 *
 *****************************************************************************/

static Object *LoadDTImgsPath(SW_IDATA *idata, STRPTR FBase, struct Screen *Scr)
{
   TEXT   FSpec[FMSIZE];
   UBYTE  c;
   LONG   x, Prev;
   STRPTR Path = idata->SWA_DTImg_Path;
   Object *DT = NULL;

   // -- try looking in search path ------------------------------------------
   
   if (!SW_STRISNULL(Path)) {
      for (x=Prev=0; ; x++) {
         if (Path[x] == ',' || !Path[x]) {
            c = Path[x], Path[x] = '\0';
            AddPart(strcpy(FSpec, Path + Prev), FilePart(FBase), FMSIZE);
            DT = LoadDTImgs(idata, FSpec, Scr);
            Path[x] = c;
            Prev    = x + 1;
         }

         if (DT || !Path[x]) break;
      }
   }

   // -- nothing found in path - try specified location ----------------------
        
   return DT ? DT : LoadDTImgs(idata, FBase, Scr);
}


/*****************************************************************************
 * Method: Load datatypes object                                             *
 *****************************************************************************/

static ULONG GetDT(Object *o, SW_IDATA *idata, struct Screen *Scr)
{
   struct DimensionInfo DI;
   ULONG  ModeID = GetVPModeID(&Scr->ViewPort);
   ULONG  IsLUT  = IsLUTMode(ModeID);
   ULONG  NomX   = 640;
   ULONG  IsBig, IsSh;
   TEXT   FSpec[FMSIZE];
   
   if (idata->SWA_DTImg_Sleep)
      set(_app(o), MUIA_Application_Sleep, TRUE);
   
   idata->SWA_DTImg_Width  = 0;
   idata->SWA_DTImg_Height = 0;
   idata->SWA_DTImg_Depth  = 0;
   idata->DTObj            = NULL;
   idata->DTObj_G          = NULL;
   idata->DTObj_M          = NULL;

   if (GetDisplayInfoData(FindDisplayInfo(ModeID), (UBYTE*)&DI, sizeof(DI),
                          DTAG_DIMS, NULL))
      NomX = DI.Nominal.MaxX - DI.Nominal.MinX + 1;

   IsBig = NomX >= idata->SWA_DTImg_MinXRes;
   IsSh  = IsLUT && Scr->RastPort.BitMap->Depth < 6;

   if (idata->SWA_DTImg_Adjust) {
      if (IsBig) {
         if (!IsLUT) {
            LoadDTImgsPath(idata,
                           strcat(strcpy(FSpec, idata->SWA_DTImg_FSpec), ".L.TC"),
                           Scr);
         }
         
         if (!idata->DTObj) {
            LoadDTImgsPath(idata,
                           strcat(strcpy(FSpec, idata->SWA_DTImg_FSpec), ".L.R8"),
                           Scr);
         }
      }
   
      if (!IsBig || !idata->DTObj) {
         if (!IsLUT) {
            LoadDTImgsPath(idata,
                           strcat(strcpy(FSpec, idata->SWA_DTImg_FSpec), ".S.TC"),
                           Scr);
         }
   
         if (!idata->DTObj) {
            LoadDTImgsPath(idata,
                           strcat(strcpy(FSpec, idata->SWA_DTImg_FSpec),
                                  IsSh?".S.R4":".S.R8"),
                           Scr);
         }
      }
   }

   if (!idata->DTObj)
      LoadDTImgsPath(idata, idata->SWA_DTImg_FSpec, Scr);

   if (idata->SWA_DTImg_Sleep)
      set(_app(o), MUIA_Application_Sleep, FALSE);
   
   if (!idata->DTObj) return FAILURE;

   return SUCCESS;
}


/*****************************************************************************
 * Function: Register a new bitmap in a cache list                           *
 *****************************************************************************/

static ULONG RegisterBitmap(Object *o, SW_IDATA *idata, Screen *s)
{
   PicNode *node;

   // -- no caching? ---------------------------------------------------------
   
   if (!idata->SWA_DTImg_ImgCache)
      return GetDT(o, idata, s);

   // -- search for an existing one on the list with same name ---------------

   for (node = (PicNode *)idata->SWA_DTImg_ImgCache;
        node = (PicNode *)FindName((List *)node, idata->SWA_DTImg_FSpec); )
      if (!s || node->pn_Screen == s) {
         node->pn_UseCount++;
         idata->DTObj   = node->pn_PictureObj;
         idata->DTObj_G = node->pn_PictureObj_G;
         idata->DTObj_M = node->pn_PictureObj_M;
         goto Done;
      }

   // -- couldn`t find on the list, so make a new one ------------------------

   if (!(node = AllocVec(sizeof(PicNode), MEMF_ANY | MEMF_CLEAR))) goto Error;

   if (!(node->pn_Node.ln_Name = AllocVec(strlen(idata->SWA_DTImg_FSpec)+1,
                                          MEMF_ANY)))
      goto Error;

   node->pn_Screen        = s;
   node->pn_UseCount      = 1;
   strcpy(node->pn_Node.ln_Name, idata->SWA_DTImg_FSpec);

   if (GetDT(o, idata, s) != SUCCESS)
      goto Error;

   node->pn_PictureObj   = idata->DTObj;
   node->pn_PictureObj_G = idata->DTObj_G;
   node->pn_PictureObj_M = idata->DTObj_M;
   
   AddHead(idata->SWA_DTImg_ImgCache, (Node *)node);

  Done:
   idata->ImgNode = (PicNode *)node;

   return SUCCESS;

  Error:
   if (node) FreeVec(node->pn_Node.ln_Name);
   FreeVec(node);

   return FAILURE;
}


/*****************************************************************************
 * Function: Unregister an old bitmap from a cache list                      *
 *****************************************************************************/

static void UnregisterBitmap(SW_IDATA *idata)
{
   if (!idata->SWA_DTImg_ImgCache || !idata->ImgNode) {
      DisposeDTObject(idata->DTObj);
      DisposeDTObject(idata->DTObj_G);
      DisposeDTObject(idata->DTObj_M);
   } else {
      --idata->ImgNode->pn_UseCount;
   }

   idata->bm      = NULL;
   idata->bmh     = NULL;
   idata->bm_g    = NULL;
   idata->bmh_g   = NULL;
   idata->bm_m    = NULL;
   idata->bmh_m   = NULL;

   idata->DTObj   = NULL;
   idata->DTObj_G = NULL;
   idata->DTObj_M = NULL;
   idata->ImgNode = NULL;
}


/*****************************************************************************
 * Method: MUI setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   if (!DoSuperMethodA(cl,o,msg)) SWDB_RET(FALSE);  // let parent setup self;

   RegisterBitmap(o, idata, _screen(o));

   GetDTAttrs(idata->DTObj,
              PDTA_BitMapHeader,    &idata->bmh,
              PDTA_DestBitMap,      &idata->bm,
              TAG_DONE);

   if (idata->DTObj_G)
      GetDTAttrs(idata->DTObj_G,
                 PDTA_BitMapHeader,    &idata->bmh_g,
                 PDTA_DestBitMap,      &idata->bm_g,
                 TAG_DONE);

   if (idata->DTObj_M)
      GetDTAttrs(idata->DTObj_M,
                 PDTA_BitMapHeader,    &idata->bmh_m,
                 PDTA_BitMap,          &idata->bm_m,
                 TAG_DONE);

   if (idata->bmh) {
      idata->SWA_DTImg_Width  = idata->bmh->bmh_Width;
      idata->SWA_DTImg_Height = idata->bmh->bmh_Height;
      idata->SWA_DTImg_Depth  = idata->bmh->bmh_Depth;
   }

   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(MUIM_Cleanup, Msg)
{
   UnregisterBitmap(idata);
   
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(MUIM_Draw, struct MUIP_Draw *)
{
   ULONG x0, y0, xs, ys;
   struct BitMapHeader *bmh;
   struct BitMap       *bm;
   
   DoSuperMethodA(cl,o,msg);

   if (!(msg->flags & MADF_DRAWOBJECT)) return 0;
   if (!idata->DTObj)                   return 0;

   if (idata->SWA_DTImg_Ghosted && idata->bm_g)
      bmh = idata->bmh_g,
      bm  = idata->bm_g;
   else
      bmh = idata->bmh,
      bm  = idata->bm;

   if (bmh) {
      xs = min(bmh->bmh_Width,  _mwidth(o));
      ys = min(bmh->bmh_Height, _mheight(o));
      x0 = (_mwidth(o)  - xs) / 2 + _mleft(o);
      y0 = (_mheight(o) - ys) / 2 + _mtop(o);

      if (idata->bm_m && idata->bmh_m &&
          idata->bmh_m->bmh_Width  == bmh->bmh_Width  &&
          idata->bmh_m->bmh_Height == bmh->bmh_Height &&
          idata->bmh_m->bmh_Depth  == 1)
         BltMaskBitMapRastPort(bm, 0, 0, _rp(o), x0, y0, xs, ys, 0xC0,
                               idata->bm_m->Planes[0]);
      else
         BltBitMapRastPort(bm, 0, 0, _rp(o), x0, y0, xs, ys, 0xc0);
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Get min/max sizes                                                 *
 *****************************************************************************/

METHOD(MUIM_AskMinMax, struct MUIP_AskMinMax *)
{
   ULONG rc = DoSuperMethodA(cl, o, (Msg)msg);
 
   msg->MinMaxInfo->MinWidth  += idata->SWA_DTImg_Width;
   msg->MinMaxInfo->DefWidth  += idata->SWA_DTImg_Width;
   msg->MinMaxInfo->MaxWidth  += idata->SWA_DTImg_Width;
   
   msg->MinMaxInfo->MinHeight += idata->SWA_DTImg_Height;
   msg->MinMaxInfo->DefHeight += idata->SWA_DTImg_Height;
   msg->MinMaxInfo->MaxHeight += idata->SWA_DTImg_Height;
 
   SWDB_RET(rc);
} SDFT
