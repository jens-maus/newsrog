/*****************************************************************************
 * Function: make image bitmap                                               *
 *****************************************************************************/

static Object *MakeBrokenBitm(UBYTE *body)
{
   Object *obj = BodychunkObject,
      MUIA_FixWidth              , Broken_WIDTH,
      MUIA_FixHeight             , Broken_HEIGHT,
      MUIA_Bitmap_Width          , Broken_WIDTH,
      MUIA_Bitmap_Height         , Broken_HEIGHT,
      MUIA_Bodychunk_Depth       , Broken_DEPTH,
      MUIA_Bodychunk_Body        , (UBYTE *)body,
      MUIA_Bodychunk_Compression , Broken_COMPRESSION,
      MUIA_Bodychunk_Masking     , Broken_MASKING,
      MUIA_Bitmap_SourceColors   , (ULONG *)Broken_colors,
      MUIA_Bitmap_Transparent    , 0,
   End;

   return obj;
}
