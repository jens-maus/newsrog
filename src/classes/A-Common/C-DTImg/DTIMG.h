
struct DTImgData {
   TEXT    FSpec[FMSIZE];
   Object *DTObj;
   ULONG   Width, Height;
};

SW_MkClass(DTImg, MUIC_Area)


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

Object *GetDT(STRPTR File, struct Screen *Scr)
{
   Object *DTObj = NULL;
   struct BitMapHeader *bmh;
   struct BitMap *bm;
   
   DTObj =NewDTObject(File,
                      DTA_SourceType       ,DTST_FILE,
                      DTA_GroupID          ,GID_PICTURE,
                      PDTA_Remap           ,TRUE,
                      PDTA_Screen          ,Scr,
                      PDTA_FreeSourceBitMap,TRUE,
                      OBP_Precision        ,PRECISION_IMAGE,
                      PDTA_DestMode,       PMODE_V43,
                      TAG_DONE);

   if (DTObj) {
      DoMethod(DTObj, DTM_PROCLAYOUT, NULL, 1);
   
      GetDTAttrs(DTObj,
                 PDTA_BitMapHeader,    &bmh,
                 PDTA_DestBitMap,      &bm,
                 TAG_DONE);
   }

   return DTObj;
}



/*****************************************************************************
 * Dispatcher for DTImg class                                                *
 *****************************************************************************/

__asm ULONG DTImgDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct DTImgData *idata = (struct DTImgData *)INST_DATA(cl, o);
   
   MkClassDispatchInit(cl);

#  define attrl ((opSet *)msg)->ops_AttrList

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *obj = (Object*)DoSuperMethodA(cl, o, msg);

        idata = INST_DATA(cl, obj);
        
        if (!obj) return NULL;

        set(obj, MUIA_FillArea, TRUE);
        
        idata->bm                 = (struct BitMap *)
                                    GetTagUL(SWA_NDF_DTImg_BM,       NULL, attrl);
        idata->Width              = GetTagUL(SWA_NDF_DTImg_Width,    NULL, attrl);
        idata->Height             = GetTagUL(SWA_NDF_DTImg_Height,   NULL, attrl);

        return (ULONG)obj;
     }

     case OM_DISPOSE: {
        FreeBitMap(idata->bm);
        break;
     }

     case MUIM_Draw: {
        struct MUIP_Draw *dr = (void *)msg;

        DoSuperMethodA(cl,o,msg);

        if (!(dr->flags & MADF_DRAWOBJECT)) return 0;

        SetAPen(_rp(o), 6);
        RectFill(_rp(o), _mleft(o), _mtop(o),
                 _mleft(o)+_mwidth(o),
                 _mtop(o)+_mheight(o));

        BltBitMapRastPort(idata->bm, 0, 0, _rp(o), _mleft(o), _mtop(o),
                          min(idata->Width,  _mwidth(o)),
                          min(idata->Height, _mheight(o)),
                          0xc0);
        return 0;
     }

     case MUIM_AskMinMax: {
        struct MUIP_AskMinMax *amm = (void *)msg;
        ULONG rc = DoSuperMethodA(cl, o, (Msg)msg);
 
        amm->MinMaxInfo->MinWidth  += idata->Width;
        amm->MinMaxInfo->DefWidth  += idata->Width;
        amm->MinMaxInfo->MaxWidth  += idata->Width;
   
        amm->MinMaxInfo->MinHeight += idata->Height;
        amm->MinMaxInfo->DefHeight += idata->Height;
        amm->MinMaxInfo->MaxHeight += idata->Height;
 
        return rc;
     }
   }

   return DoSuperMethodA(cl, o, msg);
}
