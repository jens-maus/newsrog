
# include <libraries/asl.h>

/*****************************************************************************
 * request filename & return it (or NULL if user canceled)                   *
 *****************************************************************************/

static __saveds __asm void IntuiMsgFunc(a1 struct IntuiMessage *imsg,
                                        a2 struct FileRequester *req)
{
   if (imsg->Class==IDCMP_REFRESHWINDOW)
      DoMethod(req->fr_UserData,MUIM_Application_CheckRefresh);
}


METHOD(SWM_Root_RequestFileName, ULONG *)
{
   struct FileRequester *Req;
   struct Window *w;
   static LONG left=-1,top=-1,width=-1,height=-1;
   Object *App;
   UBYTE *Res = NULL;
   static const struct Hook IntuiMsgHook =
      { { 0,0 },(VOID *)IntuiMsgFunc,NULL,NULL };

   Object *wp    = SWGetObj(o,MUIA_WindowObject),
          *Win   = GetTagObj(SWT_Root_SL_Win,         wp,         msg+1);
   STRPTR Title  = GetTagStr(SWT_Root_SL_Title,       "File",     msg+1),
      InitDrawer = GetTagStr(SWT_Root_SL_InitDrawer,  "PROGDIR:", msg+1),
      InitFile   = GetTagStr(SWT_Root_SL_InitFile,    "",         msg+1),
      InitPattern= GetTagStr(SWT_Root_SL_InitPattern, "#?",       msg+1),
      Buffer     = GetTagStr(SWT_Root_SL_Buffer,      NULL,       msg+1);
   ULONG SaveMode= GetTagUL (SWT_Root_SL_SaveMode,    FALSE,      msg+1);
   

   w   = SWGetType(Win, MUIA_Window_Window, struct Window *);
   App = SWGetObj (Win, MUIA_ApplicationObject);

   if (left==-1) {
      left   = w->LeftEdge+w->BorderLeft+2;
      top    = w->TopEdge+w->BorderTop+2;
      width  = w->Width-w->BorderLeft-w->BorderRight-4;
      height = w->Height-w->BorderTop-w->BorderBottom-4;
   }

   if (Req=MUI_AllocAslRequestTags(ASL_FileRequest,
                                   ASLFR_Window         , w,
                                   ASLFR_TitleText      , Title,
                                   ASLFR_InitialLeftEdge, left,
                                   ASLFR_InitialTopEdge , top,
                                   ASLFR_InitialWidth   , width,
                                   ASLFR_InitialHeight  , height,
                                   ASLFR_InitialDrawer  , InitDrawer,
                                   ASLFR_InitialPattern , InitPattern,
                                   ASLFR_InitialFile    , InitFile,
                                   ASLFR_DoSaveMode     , SaveMode,
                                   ASLFR_DoPatterns     , TRUE,
                                   ASLFR_RejectIcons    , TRUE,
                                   ASLFR_UserData       , App,
                                   ASLFR_IntuiMsgFunc   , &IntuiMsgHook,
                                   TAG_DONE))
      {
         set(App,MUIA_Application_Sleep,TRUE);
         if (MUI_AslRequestTags(Req,TAG_DONE)) {
            if (*Req->fr_File) {
               strncpy(Res = Buffer, Req->fr_Drawer, FMSIZE);
               AddPart(Res, Req->fr_File, FMSIZE);

               DoMethod(o, SWM_Root_RequestFNData,
                        Req->fr_Drawer, Req->fr_File, Req->fr_Pattern, Res);
            }
            left   = Req->fr_LeftEdge;
            top    = Req->fr_TopEdge;
            width  = Req->fr_Width;
            height = Req->fr_Height;
         }
         MUI_FreeAslRequest(Req);
         set(App,MUIA_Application_Sleep,FALSE);
      }

   SWDB_RET((ULONG)Res);
} SDFT
