# include "C-NArticleView-I.h"
# include "S-Profile.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NArticleView

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_PT(SWA_NArticleView_DspList,    SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_PT(SWA_NArticleView_ToolBar,    SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_PT(SWA_NArticleView_DspGlob,    SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_SA(SWA_NArticleView_TitleTxt,   SWAM_NoLab,   SWAM_SGIN, NULL, 1024),
   SWAM_PT(SWA_NArticleView_DspArt,     SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_PT(SWA_NArticleView_DspGrp,     SWAM_NoLab,   SWAM_SGIN, NULL),
   SWAM_IN(SWA_NArticleView_FirstBodyLine, SWAM_NoLab,SWAM_SGIN, 0),
   SWAM_BO(SWA_NArticleView_UseArtFont, SWAM_NoLab,   SWAM_SGIN, FALSE),   
   SWAM_PF(SWA_NArticleView_ArtFont,    SWAM_NoLab,   SWAM_SGIN, NULL,
            MAXFONTNAME),
   SWAM_BO(SWA_NArticleView_UseArtCol,  SWAM_NoLab,   SWAM_SGIN, FALSE),   
   SWAM_PA(SWA_NArticleView_ArtCol,     SWAM_NoLab,   SWA_NArticleView_Pens,
           PEN_ArtCol, MPEN_TEXT, SWAM_SGIN),
   SWAM_BO(SWA_NArticleView_PendingDisp,SWAM_NoLab,   SWAM_SGIN, FALSE),   
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Article Text",
   MUIX_C "Article Incomplete",

   "Search Article Body",
   "line",
   "lines",

   "_Clip",
   "_Select All",
   "_Unselect All",
   "Cance_l",

   SWX_I("None"),

   "Copy the selected text to\n"
      "the clipboard.  Hotkey: 'c'",

   "Quick Search: search through\n"
      " article body.  Hotkey: '?'",

   "Copy the selected text to the clipboard.",
   "Select all article text.",
   "Unselect all article text.",
   "Cancel.",

   "Move to the beginning of\n"
      "the displayed article.",

   "Move to the end of the\n"
      "displayed article.",

   "%s / %ld Lines",
   "(%ld x %ld x %ld)",
   "Filename: ",
   "Info: ",
   "Status: ",
   "File Type: ",
   SWX_I("Unknown"),
   "Save data",
   "*_OK",
   "The enclosed data was saved\n"
      "successfully to the file:\n"
      "'%s'.",
   "Save Canceled.",
   
   "Save...",
   "Save As...",
   "Action...",

   "Save the enclosed data to the default file.",
   "Save the enclosed data to any file.",
   "Perform a defined multimedia action\n"
      "upon the enclosed data.",

   "The target filename:\n\n"
   "  " SWX_I("%s") "\n\n"
   "appears to contain MS Windows path components inserted\n"
   "by a broken Windows application.  Do you wish to cancel\n"
   "the save operation, or proceed and save the file using\n"
   "the Windows basename, which is:\n\n"
   "  " SWX_I("%s") " ?",
   "*_Save|_Cancel",

   "Copy to Clipboard",

   "File %s exists.\n"
   "You may overwrite this file, chose a\n"
   "new filename, or cancel this save.",
   "_Overwrite|*_New Filename|_Cancel",
      
   SWX_B("Error: ") "Unable to open file for writing:\n'%s'",
   SWX_B("Error: ") "File write failed: '%s'",
   SWX_B("Error: ") "No text selected.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

static Object *MkIB(SW_IDATA *idata, ULONG h, STRPTR Image, LONG Cmd)
{
   return DTImgObject,
      MUIA_InnerTop,        0,
      MUIA_InnerBottom,     0,
      MUIA_InnerLeft,       0,
      MUIA_InnerRight,      0,
      MUIA_Frame,           MUIV_Frame_ImageButton,
      MUIA_InputMode,       MUIV_InputMode_RelVerify,
      MUIA_ControlChar,     Cmd,
      MUIA_ShortHelp,       GetS(h),
      MUIA_CycleChain,      1,
      SWA_DTImg_FSpec,      Image,
      SWA_DTImg_ImgCache,   NN_DTImages(idata),
      SWA_DTImg_Path,       NN_ButtonPath(idata),
   End;
}


static Object *ImageB(ULONG i, LONG Cmd, LONG h)
{
   return ImageObject,
      ImageButtonFrame,
      MUIA_Image_Spec,  i,
      MUIA_CycleChain,  1,
      MUIA_InputMode,   MUIV_InputMode_RelVerify,
      MUIA_ControlChar, Cmd,
      MUIA_ShortHelp,   GetS(h),
   End;
}

static Object *CreateQSrch(Object *o, SW_IDATA *idata)
{
   return
      MUI_NewObject(SWC_ListSearch,
                    SWA_ListSearch_Title,        GetS(SWS_L_QSTitle),
                    SWA_ListSearch_List,         idata->VGroupObj,
                    SWA_ListSearch_CmpMethod,    SWV_ListSearch_SimpleStr,
                    SWA_ListSearch_Dynamic,      TRUE,
                    SWA_ListSearch_First,        TRUE,
                    SWA_ListSearch_WindowID,     MAKE_ID('A','v','S','r'),
                    SWA_ListSearch_ItemName,     GetS(SWS_L_Item),
                    SWA_ListSearch_ItemNamePl,   GetS(SWS_L_Items),
                    SWA_ListSearch_Flags,        SWF_ListSearch_HLMethod,
                    TAG_DONE);
}

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

static ULONG Create(Object *o, SW_IDATA *idata)
{
   Object *g, *b_Clip, *b_Srch, *b_Sel, *b_UnSel, *b_Canc, *to, *PopClip;

   if (!(idata->NDF = MUI_NewObject(SWC_NDFilter, TAG_DONE)))
      return FAILURE;
            
   DoMethod(o, OM_ADDMEMBER,
            to = SWUI_FrameText("", 0,
               (HGroup,
                   MUIA_Group_HorizSpacing, 3,
                   Child, idata->b_Up = ImageB(MUII_ArrowUp,   '<', SWS_H_Up),
                   Child, idata->b_Dn = ImageB(MUII_ArrowDown, '>', SWS_H_Dn),
                   Child, idata->SWA_NArticleView_ToolBar ?
                         idata->SWA_NArticleView_ToolBar : HSpace(0),
                   Child, b_Srch = MkIB(idata, SWS_H_Search, BF_Search, '?'),
                   Child, PopClip = PopobjectObject,
                      MUIA_Popstring_Button, MkIB(idata,SWS_H_Clip,BF_Clip,'c'),
                      MUIA_Popobject_Object, VGroup,
                         GroupFrame,
                         MUIA_Background, MUII_GroupBack,
                         Child, b_Clip  = MkBt(SWS_L_BClip,  SWS_H_BClip),
                         Child, b_Sel   = MkBt(SWS_L_BSel,   SWS_H_BSel),
                         Child, b_UnSel = MkBt(SWS_L_BUnSel, SWS_H_BUnSel),
                         Child, b_Canc  = MkBt(SWS_L_BCanc,  SWS_H_BCanc),
                      End,
                   End,
                End), &idata->Title));

   if (!to) {
      MUI_DisposeObject(idata->NDF);
      idata->NDF = NULL;
      return FAILURE;
   }

   DoMethod(o, OM_ADDMEMBER, g = ScrollgroupObject,
               MUIA_CycleChain, 1,
               MUIA_UserData,   SWV_NArticleView_SGroup,
               MUIA_Scrollgroup_Contents, idata->VGroupObj = VirtgroupObject,
                  VirtualFrame,
                  MUIA_UserData,       SWV_CText_ClickGrpRoot,
                  MUIA_Background,     MUII_TextBack,
                  MUIA_Group_Horiz,    FALSE,
                  MUIA_Group_Spacing,  0,
                  Child, idata->Empty = HVSpace,
               End,
            End);
   
   if (!g) {
      MUI_DisposeObject(idata->NDF);
      idata->NDF = NULL;
      return FAILURE;
   }

   DoMethod(idata->VGroupObj, MUIM_Notify, MUIA_ContextMenuTrigger, MUIV_EveryTime,
            o, 2, SWM_NArticleView_CtxMenuChoice, MUIV_TriggerValue);
   
   if (!(idata->QSearch = CreateQSrch(o, idata))) {
      MUI_DisposeObject(idata->NDF);
      idata->NDF = NULL;
      return FAILURE;
   }

   DoMethod(b_Srch,  NOTP, o, 1, SWM_NArticleView_SWin);
   DoMethod(b_Clip,  NOTP, o, 1, SWM_NArticleView_Clip);
   DoMethod(b_Sel,   NOTP, o, 1, SWM_NArticleView_SelectAll);
   DoMethod(b_UnSel, NOTP, o, 1, SWM_NArticleView_UnselectAll);

   DoMethod(b_Clip,  NOTP, PopClip, 2, MUIM_Popstring_Close, TRUE);
   DoMethod(b_Canc,  NOTP, PopClip, 2, MUIM_Popstring_Close, TRUE);

   DoMethod(idata->b_Up, NOTP, o, 1, SWM_NArticleView_Top);
   DoMethod(idata->b_Dn, NOTP, o, 1, SWM_NArticleView_Bottom);

   idata->MemPool = NN_MemPool(idata);
   idata->A4      = getreg(REG_A4);
   
   return SUCCESS;
}


# define SW_SETUP_POSTSET \
            if (Create((Object *)retval, idata) != SUCCESS)      \
               SWDB_RET(NULL);

# define DISPOSECUST DoMethod(o, SWM_NArticleView_Clear, FALSE); \
                     MUI_DisposeObject(idata->NDF);              \
                     MUI_DisposeObject(idata->QSearch);          \
                     MUI_DisposeObject(idata->FltList);          \
                     FV(idata->PrevObjs);                        \
                     FV(idata->VarBuf);                          \


static Object *CTextTemplate = NULL;

# define SW_USERSETUP    TextHideInit(libbase);   \
                         TextSelInit(libbase);    \
                         TextCmdInit(libbase);    \
                         TextScrollInit(libbase); \
                         MimeBoxInit(libbase);    \
                         CTextTemplate = CTextObject, End; \
                         _ProfInit();

# define SW_USERCLEANUP  TextCmdExit();                   \
                         TextSelExit();                   \
                         TextHideExit();                  \
                         TextScrollExit();                \
                         MimeBoxExit();                   \
                         MUI_DisposeObject(CTextTemplate);\
                         CTextTemplate = NULL;            \
                         _ProfTerm();

# define SW_Methods                           \   
  SW_DO_MF(SWM_NArticleView_Clear);           \
  SW_DO_MF(SWM_NArticleView_Begin);           \
  SW_DO_MF(SWM_NArticleView_NewLines);        \
  SW_DO_MF(SWM_NArticleView_End);             \
  SW_DO_MF(SWM_NArticleView_Abort);           \
  SW_DO_MF(SWM_NArticleView_Clip);            \
  SW_DO_MF(SWM_NArticleView_SWin);            \
  SW_DO_MF(SWM_NArticleView_Browse);          \
  SW_DO_MF(SWM_NArticleView_Scroll);          \
  SW_DO_MF(SWM_NArticleView_GenObj);          \
  SW_DO_MF(SWM_NArticleView_GenTxt);          \
  SW_DO_MF(SWM_NArticleView_AddObj);          \
  SW_DO_MF(SWM_NArticleView_TextHideClass);   \
  SW_DO_MF(SWM_NArticleView_TextSelClass);    \
  SW_DO_MF(SWM_NArticleView_TextCmdClass);    \
  SW_DO_MF(SWM_NArticleView_TextScrollClass); \
  SW_DO_MF(SWM_NArticleView_MimeBoxClass);    \
  SW_DO_MF(SWM_NArticleView_SelectAll);       \
  SW_DO_MF(SWM_NArticleView_UnselectAll);     \
  SW_DO_MF(SWM_NArticleView_Top);             \
  SW_DO_MF(SWM_NArticleView_Bottom);          \
  SW_DO_MF(SWM_NArticleView_SaveData);        \
  SW_DO_MF(SWM_NArticleView_MakeObjVis);      \
  SW_DO_MF(SWM_NArticleView_Search);          \
  SW_DO_MF(SWM_NArticleView_DispRaw);         \
  SW_DO_MF(SWM_NArticleView_DispCooked);      \
  SW_DO_MF(SWM_NArticleView_GetDefFont);      \
  SW_DO_MF(SWM_NArticleView_GetDefColor);     \
  SW_DO_MF(SWM_NArticleView_ModTxt);          \
  SW_DO_MF(SWM_NArticleView_ArtFwd);          \
  SW_DO_MF(SWM_NArticleView_DFwd);            \
  SW_DO_MF(SWM_NArticleView_SelChange);       \
  SW_DO_MF(SWM_NArticleView_SaveDataUI);      \
  SW_DO_MF(SWM_VarList_Search);               \
  SW_DO_MF(MUIM_Setup);                       \
  SW_DO_MF(MUIM_Cleanup);                     \
  SW_DO_MF(SWM_NArticleView_CtxMenuChoice);

static Object *BuildCtxMenu(Object *o, Object *TopObj, STRPTR Type);
static void FreeCtxMenu(Object *o, STRPTR Type);

# include "C-TextHide.h"
# include "C-TextSel.h"
# include "C-TextCmd.h"
# include "C-TextScroll.h"
# include "C-MimeBox.h"
# include "C-NArticleView-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
