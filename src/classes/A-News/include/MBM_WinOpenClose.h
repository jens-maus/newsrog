Object *MakeWinBitm(UBYTE *body)
{
   Object *obj = BodychunkObject,
      MUIA_FixWidth              , WINOPEN_WIDTH,
      MUIA_FixHeight             , WINOPEN_HEIGHT,
      MUIA_Bitmap_Width          , WINOPEN_WIDTH,
      MUIA_Bitmap_Height         , WINOPEN_HEIGHT,
      MUIA_Bodychunk_Depth       , WINOPEN_DEPTH,
      MUIA_Bodychunk_Body        , (UBYTE *)body,
      MUIA_Bodychunk_Compression , WINOPEN_COMPRESSION,
      MUIA_Bodychunk_Masking     , WINOPEN_MASKING,
      MUIA_Bitmap_SourceColors   , (ULONG *)WinOpen_colors,
      MUIA_Bitmap_Transparent    , 0,
   End;
   
   return obj;
}
