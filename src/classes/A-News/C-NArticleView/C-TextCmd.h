struct TextCmdData {
   SWP_NNews_AttrDefs;
   Object *Art, *Grp;
   TEXT SavedFile[64];
   STRPTR Type, Action, Var, Val;
   ULONG  DClick, UseMenu;
};

SW_MkClassPriv(TextCmd, TextSelClassPtr)

/*****************************************************************************
 * Function: copy a string from tags                                         *
 *****************************************************************************/

static STRPTR TCCpyStr(ULONG Tag, STRPTR def, struct TagItem *ti)
{
   STRPTR s = GetTagStr(Tag, NULL, ti), a;

   if (SW_STRISNULL(s)) return NULL;

   if (!(a = AllocVec(strlen(s)+1, MEMF_ANY))) return NULL;

   return strcpy(a, s);
}


/*****************************************************************************
 * Dispatcher for TextCmd class                                              *
 *****************************************************************************/

__asm ULONG TextCmdDispatcher(a0 struct IClass *cl, a2 Object *o, a1 Msg msg)
{
   struct TextCmdData *idata = INST_DATA(cl, o);

   MkClassDispatchInit(cl);

#  define attrl ((opSet *)msg)->ops_AttrList

   switch (msg->MethodID) {
     case OM_NEW: {
        Object *obj = (Object*)DoSuperMethodA(cl, o, msg);
        idata = INST_DATA(cl, obj);
        if (!obj) return NULL;
        
        idata->SWA_AppBase_TopObj = GetTagObj(SWA_AppBase_TopObj, NULL, attrl);
        idata->Art                = GetTagObj(SWA_TextCmd_Art,    NULL, attrl);
        idata->Grp                = GetTagObj(SWA_TextCmd_Grp,    NULL, attrl);
        idata->Type               = TCCpyStr(SWA_TextCmd_Type,    NULL, attrl);
        idata->Action             = TCCpyStr(SWA_TextCmd_Action,  NULL, attrl);
        idata->Var                = TCCpyStr(SWA_TextCmd_Var,     NULL, attrl);
        idata->Val                = TCCpyStr(SWA_TextCmd_Val,     NULL, attrl);
        idata->DClick             = GetTagUL(SWA_TextCmd_NeedDouble, FALSE, attrl);
        idata->UseMenu            = GetTagUL(SWA_TextCmd_UsePopMenu,FALSE, attrl);
        
        return (ULONG)obj;
     }

     case OM_DISPOSE: {
        if (!SW_STRISNULL(idata->SavedFile))
           DeleteFile(idata->SavedFile);
        
        FreeVec(idata->Type);
        FreeVec(idata->Action);
        FreeVec(idata->Var);
        FreeVec(idata->Val);
        break;
     }

     case OM_GET: {
        switch (((opGet *)msg)->opg_AttrID) {
          case SWA_TextCmd_Activated:
           return 1;

          case SWA_TextCmd_Val:
           *((ULONG *)(((opGet *)msg)->opg_Storage)) = (ULONG)idata->Val;
           return 1;
        }
        break;
     }
      
     case OM_SET: {
        if (!(idata->DClick  &&
              GetTagUL(SWA_TextSel_DoubleClick, -99, attrl) == TRUE||
              !idata->DClick &&
              GetTagUL(SWA_TextSel_Pressed, -99, attrl) == TRUE))
           break;
        
        SetSuperAttrs(cl, o, SWA_TextCmd_Activated, TRUE, TAG_DONE);
        
        if (!SW_STRISNULL(idata->Action)) {
           DoMethod(NN_MMActs(idata), SWM_FileActs_DoAction,
                    SWT_FileActs_DoA_Type,    idata->Type,
                    SWT_FileActs_DoA_Action,  idata->Action,
                    SWT_FileActs_DoA_VarList, NN_VarList(idata),
                    SWT_FileActs_DoA_VarObj,  o,
                    TAG_DONE);
   
           DoMethod(NN_MMActs(idata), SWM_Root_CheckErr, _win(o));
        }

        break;
     }

     case MUIM_Setup: {
        if (!DoSuperMethodA(cl, o, msg))
           return FALSE;

        set(o, MUIA_ContextMenu,
            idata->UseMenu
                ? BuildCtxMenu(o, idata->SWA_AppBase_TopObj, idata->Type)
                : NULL);
        
        return TRUE;
     }

     case MUIM_Cleanup: {
        FreeCtxMenu(o, idata->Type);
        break;
     }

     case MUIM_ContextMenuChoice: {
        struct MUIP_ContextMenuChoice *cmc = (void *)msg;
        STRPTR Choice = SWGetStr(cmc->item, MUIA_Menuitem_Title);

        if (strcmp(Choice, GetS(SWS_M_CpClip)) == 0) {
           // -- copy text to clipboard --------------------------------------
           struct IFFHandle *iff = SWOpenClipWrite(0);

           if (iff) {
              SWTextToClip(iff, idata->Val);
              SWCloseClipWrite(iff);
           }
        } else {
           // -- otherwise, perform given action -----------------------------
           DoMethod(NN_MMActs(idata), SWM_FileActs_DoAction,
                    SWT_FileActs_DoA_Type,    idata->Type,
                    SWT_FileActs_DoA_Action,  Choice,
                    SWT_FileActs_DoA_VarList, NN_VarList(idata),
                    SWT_FileActs_DoA_VarObj,  o,
                    TAG_DONE);
        }

        break;
     }
         
     case SWM_VarList_Search: {
        ULONG  rc;
        struct SWP_VarList_Search *VLS = (void *)msg;

        if (rc = DoMethodA(idata->Art, (Msg)msg))
           SWDB_RET(rc);

        if (rc = DoMethodA(idata->Grp, (Msg)msg))
           SWDB_RET(rc);

        if (!strcmp(VLS->Name, idata->Var))
           SWDB_RET(idata->Val);

        if (!strncmp(VLS->Name, "File", 4) && idata->Art) {
           STRPTR QP    = NULL;
           ULONG  Parts = SWF_NArticle_HdBd;

           if (!SW_STRISNULL(idata->SavedFile))
              SWDB_RET(idata->SavedFile);

           sprintf(idata->SavedFile, "t:NR.O.%08lx", idata->Art);

           if (strstr(VLS->Name, ":Head"))  Parts = SWF_NArticle_Head;
           if (strstr(VLS->Name, ":Body"))  Parts = SWF_NArticle_Body;
           if (strstr(VLS->Name, ":HdBd"))  Parts = SWF_NArticle_HdBd;
           if (strstr(VLS->Name, ":Q"))
              QP = VL_Alloc(o, idata, NN_QuoteStr(idata));

           DoMethod(idata->Art, SWM_NArticle_SaveText,
                    SWT_NArticle_FSpec,   idata->SavedFile,
                    SWT_NArticle_Parts,   Parts,
                    SWT_NArticle_Prefix,  QP,
                    SWT_NArticle_QMode,   NN_QuoteMode(idata),
                    SWT_NArticle_Grp,     idata->Grp,
                    SWT_NArticle_ArtView, SWGetObj(idata->Grp,
                                                   SWA_NGroup_ArtView),
                    SWT_NArticle_SvAttr,  SWA_NDFilter_T_NetReply,
                    TAG_DONE);

           VL_Free(o, idata, QP);
           
           SWDB_RET(idata->SavedFile);
        }
     }
   }

   return DoSuperMethodA(cl, o, msg);
}
