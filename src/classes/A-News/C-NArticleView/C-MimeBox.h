# include "S-MSBaseName.h"

struct MimeBoxData {
   SWP_NNews_AttrDefs;
   UBYTE  *MimeData;
   ULONG   MimeDataSize;
   ULONG   Prompt;
   ULONG   MakeDir;
   ULONG   DspSize;
   Object  *FNStr, *Grp, *Art, *AListV, *ActPop, *ArtView, *DTImg, *InfoObj;
   TEXT    SaveDir[FMSIZE];
   TEXT    SavedFile[FMSIZE];
   TEXT    InfoStr[128];
   STRPTR  FType, InfoAppend;
   TEXT    MType[64];
};

SW_MkClass(MimeBox, MUIC_Group)

static Object *TextObj(STRPTR s)
{
   return TextObject,
      TextFrame,
      MUIA_Background,    MUII_TextBack,
      MUIA_Text_Contents, s,
   End;
}

SWM_P(SWP_MimeBox_Save, ULONG AskFN);

/*****************************************************************************
 * Dispatcher for MimeBox class                                              *
 *****************************************************************************/

__asm ULONG MimeBoxDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct MimeBoxData *idata = (struct MimeBoxData *)INST_DATA(cl, o);
   
   MkClassDispatchInit(cl);

#  define attrl ((opSet *)msg)->ops_AttrList

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *obj = (Object*)DoSuperMethodA(cl, o, msg),
               *b_Save, *b_SaveAs, *g, *bg, *AList;
        TEXT SizeStr[16];
        STRPTR s, FName, MType;

        idata = INST_DATA(cl, obj);
        
        if (!obj) return NULL;
        
        idata->DTImg              = GetTagObj(SWA_NDF_MimeBox_Img,     NULL, attrl);
        idata->Grp                = GetTagObj(SWA_NDF_MimeBox_Grp,     NULL, attrl);
        idata->Art                = GetTagObj(SWA_NDF_MimeBox_Art,     NULL, attrl);
        idata->ArtView            = GetTagObj(SWA_NDF_MimeBox_ArtView, NULL, attrl);
        idata->MimeData           = GetTagStr(SWA_NDF_MimeBox_Data,    NULL, attrl);
        idata->MimeDataSize       = GetTagUL(SWA_NDF_MimeBox_DataSize,    0, attrl);
        idata->Prompt             = GetTagUL(SWA_NDF_MimeBox_Prompt,   TRUE, attrl);
        idata->MakeDir            = GetTagUL(SWA_NDF_MimeBox_MakeDir,  TRUE, attrl);
        idata->DspSize            = GetTagUL(SWA_NDF_MimeBox_DispSize,FALSE, attrl);
        idata->SWA_AppBase_TopObj = GetTagObj(SWA_AppBase_TopObj,      NULL, attrl);
        s                         = GetTagStr(SWA_NDF_MimeBox_SaveDir, NULL, attrl);
        MType                     = GetTagStr(SWA_NDF_MimeBox_Type,    NULL, attrl);

        idata->MType[0] = '\0';
        idata->FType    = idata->MType;

        if (!SW_STRISNULL(MType)) {
           strncpy(idata->MType, MType, 63);
           idata->MType[63] = '\0';
        }

        FName = GetTagStr(SWA_NDF_MimeBox_FName, "Unnamed", attrl);

        if (SW_STRISNULL(idata->FType))
           idata->FType = (STRPTR)
              DoMethod(NN_MMTypes(idata), SWM_FileTypes_FindType,
                       SWT_FileTypes_FT_FSpec,   MSBaseName(FName),
                       SWT_FileTypes_FT_Mem,     idata->MimeData,
                       SWT_FileTypes_FT_MemSize, idata->MimeDataSize,
                       TAG_DONE);

        if (s) strcpy(idata->SaveDir, s);
        
        set(obj, MUIA_Group_Horiz, TRUE);

        sprintf(idata->InfoStr, GetS(SWS_L_InfoFmt),
                SWFmtSize(idata->MimeDataSize,SizeStr, 2),
                GetTagUL(SWA_NDF_MimeBox_Lines, 0, attrl));

        idata->InfoAppend = idata->InfoStr + strlen(idata->InfoStr);

        if (!SW_STRISNULL(idata->FType)) {
           AList = (Object *)
              DoMethod(NN_MMActs(idata), SWM_FileActs_GetActList,
                       SWT_FileActs_GAL_Type, idata->FType,
                       TAG_DONE);

           if (AList)
              idata->ActPop = PopobjectObject,
                 MUIA_CycleChain, 1,
                 MUIA_HorizWeight, 300,
                 MUIA_Popobject_Object, idata->AListV = ListviewObject,
                    InputListFrame,
                    MUIA_Listview_List, AList,
                 End,
                 MUIA_Popstring_Button, MkBt(SWS_B_Act, SWS_H_Act),
              End;
        }
        
        DoMethod(obj, OM_ADDMEMBER,
                 g = VGroup,
                    GroupFrameT("data block"),
                    MUIA_Background, MUII_GroupBack,
                    Child, ColGroup(4),
                       Child, Label2(GetS(SWS_L_FN)),
                       Child, idata->FNStr = StringObject,
                          StringFrame,
                          MUIA_CycleChain,      1,
                          MUIA_Background,      MUII_TextBack,
                          MUIA_String_Contents, FName,
                          MUIA_FixWidthTxt,     "MMWWmmwwmmwwmmww",
                       End,
                       Child, Label2(GetS(SWS_L_Info)),
                       Child, idata->InfoObj = TextObj(idata->InfoStr),
                       Child, Label2(GetS(SWS_L_Stat)),
                       Child, TextObj(GetTagStr(SWA_NDF_MimeBox_Status, "", attrl)),
                       Child, Label2(GetS(SWS_L_Type)),
                       Child, TextObj(SW_STRDEF(idata->FType, GetS(SWS_L_TUnknown))),
                    End,
                    Child, bg = HGroup,
                       MUIA_Group_SameSize, TRUE,
                       Child, b_Save   = MkBt(SWS_B_Save,   SWS_H_Save),
                       Child, b_SaveAs = MkBt(SWS_B_SaveAs, SWS_H_SaveAs),
                    End,
                 End);

        if (!g) return NULL;

        if (idata->ActPop)
           DoMethod(bg, OM_ADDMEMBER, idata->ActPop);

        DoMethod(bg, OM_ADDMEMBER, RectangleObject,
                   MUIA_HorizWeight, 500,
                 End);

        if (idata->DTImg)
           DoMethod(g, OM_ADDMEMBER, idata->DTImg);
                    
        DoMethod(obj, OM_ADDMEMBER, HSpace(0));

        DoMethod(b_Save,    NOTP, obj, 2, SWM_NDF_MimeBox_Save, FALSE);
        DoMethod(b_SaveAs,  NOTP, obj, 2, SWM_NDF_MimeBox_Save, TRUE);

        if (idata->ActPop)
           DoMethod(idata->AListV, DCLK, obj, 1, SWM_NDF_MimeBox_Act);

        if (!idata->MimeData)
           DoMethod(obj, MUIM_MultiSet, MUIA_Disabled, TRUE,
                    b_Save, b_SaveAs, idata->ActPop, NULL);
        
        return (ULONG)obj;
     }

     case OM_DISPOSE: {
        if (!SW_STRISNULL(idata->SavedFile))
           DeleteFile(idata->SavedFile);
        
        FreeVec(idata->MimeData);
        break;
     }

     case MUIM_Setup: {
        ULONG rc = DoSuperMethodA(cl, o, msg);

        if (idata->DTImg && idata->DspSize &&
            idata->FType && Strnicmp(idata->FType, "image", 5)==0) {
           strcat(idata->InfoAppend, " ");
           sprintf(idata->InfoAppend+1, GetS(SWS_L_SizeFmt),
                   SWGetUL(idata->DTImg, SWA_DTImg_Width),
                   SWGetUL(idata->DTImg, SWA_DTImg_Height),
                   SWGetUL(idata->DTImg, SWA_DTImg_Depth));

           set(idata->InfoObj, MUIA_Text_Contents, idata->InfoStr);
        }
        
        return rc;
     }
      
     case SWM_NDF_MimeBox_Save: {
        STRPTR IF = SWGetStr(idata->FNStr, MUIA_String_Contents),
               SDVar;

        SDVar = VL_Alloc(idata->ArtView, idata, idata->SaveDir);
        SDVar = SW_STRDEF(SDVar, "RAM:");

        DoMethod(idata->ArtView, SWM_NArticleView_SaveDataUI,
                 SWT_NArticleView_SD_App,         _app(o),
                 SWT_NArticleView_SD_Win,         _win(o),
                 SWT_NArticleView_SD_Title,       GetS(SWS_T_SaveUUD),
                 SWT_NArticleView_SD_InitDrawer,  SDVar,
                 SWT_NArticleView_SD_InitFile,    IF,
                 SWT_NArticleView_SD_OWPrompt,    TRUE,
                 SWT_NArticleView_SD_MSBase,      TRUE,
                 SWT_NArticleView_SD_SaveAs,      ((SWP_MimeBox_Save *)msg)->AskFN,
                 SWT_NArticleView_SD_CancPrompt,  idata->Prompt,
                 SWT_NArticleView_SD_MakePath,    idata->MakeDir,
                 SWT_NArticleView_SD_Data,        idata->MimeData,
                 SWT_NArticleView_SD_DataSize,    idata->MimeDataSize,
                 SWT_NArticleView_SD_ErrObj,      idata->Grp,
                 TAG_DONE);

        VL_Free(idata->ArtView, idata, SDVar);

        return NULL;
     }

     case SWM_NDF_MimeBox_Act: {
        STRPTR Act; 

        if (!idata->MimeData) return NULL;

        DoMethod(idata->AListV, MUIM_List_GetEntry, MUIV_List_GetEntry_Active,
                 &Act);
        DoMethod(idata->ActPop, MUIM_Popstring_Close, TRUE);

        if (SW_STRISNULL(Act)) return NULL;

        if (SW_STRISNULL(idata->SavedFile)) {
           sprintf(idata->SavedFile, "T:NR.UUB.%ld", GetUniqueID());
           
           if (DoMethod(idata->ArtView, SWM_NArticleView_SaveData,
                        idata->MimeData, idata->MimeDataSize, idata->Grp,
                        idata->SavedFile) == FAILURE) {
              idata->SavedFile[0] = '\0';
              DoMethod(idata->Grp, SWM_Root_CheckErr, _win(o));
              return NULL;
           }
        }

        DoMethod(NN_MMActs(idata), SWM_FileActs_DoAction,
                 SWT_FileActs_DoA_Type,    idata->FType,
                 SWT_FileActs_DoA_Action,  Act,
                 SWT_FileActs_DoA_VarList, NN_VarList(idata),
                 SWT_FileActs_DoA_VarObj,  o,
                 TAG_DONE);

        DoMethod(NN_MMActs(idata), SWM_Root_CheckErr, _win(o));
        
        return NULL;
     }

     case SWM_VarList_Search: {
        ULONG  rc;
        struct SWP_VarList_Search *VLS = (void *)msg;

        if (rc = DoMethodA(idata->Art, (Msg)msg))
           SWDB_RET(rc);

        if (rc = DoMethodA(idata->Grp, (Msg)msg))
           SWDB_RET(rc);

        if (!strcmp(VLS->Name, "File"))
           SWDB_RET(idata->SavedFile);

        SWDB_RET(NULL);
     }
   }      

   return DoSuperMethodA(cl, o, msg);
}
