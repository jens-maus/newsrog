# include "C-NPost-I.h"


# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NPost

// -- attribute map ----------------------------------------------------------

# define UI SWAM_UI | SWAM_Notify

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_NPost_Subj,       SWS_L_Subj, UI, NULL, 8192),
   SWAM_SA(SWA_NPost_Grp,        SWS_L_Grp,  UI, NULL, 8192),
   SWAM_SA(SWA_NPost_FUps,       SWS_L_FUps, UI, NULL, 8192),
   SWAM_SA(SWA_NPost_Distrib,    SWS_L_Dist, UI, NULL, 8192),

   SWAM_TX(SWS_L_SigF, NULL),
   SWAM_SA(SWA_NPost_SigFile,    SWAM_NoLab, UI|SWAM_NewHG|SWAM_PopFile, NULL, FMSIZE),
   SWAM_BO(SWA_NPost_UseCustHdrs,SWS_L_CstH, SWAM_UI|SWAM_EndG, TRUE),
   SWAM_SA(SWA_NPost_Refs,       SWAM_NoLab, SWAM_SGI,      NULL, 8192),
   SWAM_SA(SWA_NPost_MsgID,      SWAM_NoLab, SWAM_SGI,      NULL, SWV_MsgIDLen),
   SWAM_SA(SWA_NPost_Server,     SWAM_NoLab, SWAM_SGI,      NULL, 256),
   SWAM_IN(SWA_NPost_Lines,      SWAM_NoLab, SWAM_SGI,      0),
   SWAM_IN(SWA_NPost_Bytes,      SWAM_NoLab, SWAM_SGI,      0),
   SWAM_IN(SWA_NPost_Mod,        SWAM_NoLab, SWAM_SGIN,     0),
   SWAM_FI(SWA_NPost_Pct,        SWAM_NoLab, SWAM_SGIN,     0),
   SWAM_IN(SWA_NPost_PostedOK,   SWAM_NoLab, SWAM_SGIN,     FALSE),
   SWAM_PT(SWA_NPost_PostDir,    SWAM_NoLab, SWAM_SGIN,     "PostQueue"),
   SWAM_BO(SWA_NPost_NoDel,      SWAM_NoLab, SWAM_SGIN,     FALSE),
   SWAM_SA(SWA_NPost_TagLine,    SWAM_NoLab, SWAM_SGI,      NULL, 4096),
   SWAM_IN(SWA_NPost_TagLinePos, SWAM_NoLab, SWAM_SGI,      0),
   SWAM_BO(SWA_NPost_NewPost,    SWAM_NoLab, SWAM_SGIN,     0),
   
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- help map ---------------------------------------------------------------

HelpMap SW_HELPMAP[] = {
   SWA_NPost_Subj,        SWS_H_Subj,
   SWA_NPost_Grp,         SWS_H_Grp,
   SWA_NPost_FUps,        SWS_H_FUps,
   SWA_NPost_Distrib,     SWS_H_Dist,
   SWA_NPost_SigFile,     SWS_H_SigF,
   SWA_NPost_UseCustHdrs, SWS_H_CstH,
   NULL, NULL,
};


// -- edit menu --------------------------------------------------------------

struct NewMenu EditMenu[] =
{
  { NM_TITLE, (STRPTR)SWS_M_Text,         0, 0,0, (APTR)0                 },
  { NM_ITEM,  (STRPTR)SWS_M_T_Save,       0, 0,0, (APTR)SWS_M_T_Save      },
  { NM_ITEM,  (STRPTR)SWS_M_T_Ins,        0, 0,0, (APTR)SWS_M_T_Ins       },
  { NM_ITEM,  (STRPTR)SWS_M_T_InsQuoted,  0, 0,0, (APTR)SWS_M_T_InsQuoted },
  { NM_ITEM,  (STRPTR)SWS_M_T_Clear,      0, 0,0, (APTR)SWS_M_T_Clear     },
  { NM_TITLE, (STRPTR)SWS_M_Edit,         0, 0,0, (APTR)0                 },
  { NM_ITEM,  (STRPTR)SWS_M_E_Undo,       0, 0,0, (APTR)SWS_M_E_Undo      },
  { NM_ITEM,  (STRPTR)SWS_M_E_Redo,       0, 0,0, (APTR)SWS_M_E_Redo      },
  { NM_ITEM,  (STRPTR)SWS_M_E_Cut,        0, 0,0, (APTR)SWS_M_E_Cut       },
  { NM_ITEM,  (STRPTR)SWS_M_E_Copy,       0, 0,0, (APTR)SWS_M_E_Copy      },
  { NM_ITEM,  (STRPTR)SWS_M_E_Paste,      0, 0,0, (APTR)SWS_M_E_Paste     },
  { NM_ITEM,  (STRPTR)SWS_M_E_Rewrap,     0, 0,0, (APTR)SWS_M_E_Rewrap    },
  { NM_END,   (STRPTR)NULL,               0, 0,0, (APTR)0                 },
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Subject: ",
   "Group(s): ",
   "Followups-To: ",
   "Distribution: ",
   "Sig File: ",
   "Custom Headers: ",

   "_Proceed|*_Cancel",
   SWX_B("Warning: ") "Followups have been redirected from\n"
      "the original group list to these groups:\n"
      "%s",
   "*_Use\nFollowups|_Ignore\nFollowups|This group\n_only|_Cancel\n",
   "Do you wish to delete this post, or keep it\n"
   "and return it to the post queue?",
   "_Delete|*_Keep",
   SWX_B("Note: ") "Posting succeeded.",
   "*_Ok",
   "Canceled",
   "Edit Post",
   "View Wrapped",
   "View All",
   "View Encoding",
   "View Post",
   "Cursor: ",

   "_Post now",
   "Invoke _Editor",
   "To _Queue",
   "_Cancel",
   "_Delete",

   "Posting article...",
   "Ending post...",
   "Edit Post",
   MUIX_C SWX_B("Article Text"),
   "Save Post to File...",
   "Load Post from File...",

   "The subject for the article.",
   "The group list to which the\n"
      "article will be posted.",
   "If non-null, followups will be\n"
      "sent to this list of groups.",
   "If non-null, the article distribution\n"
      "will be set to this value.",
   "If non-null, the contents of this\n"
      "file will be appended to the post\n"
      "when it is sent to the server.",
   "If checked, custom headers from the " SWX_I("Headers") "\n"
      "area of the main preferences window will\n"
      "be include with this post.  Otherwise,\n"
      "custom headers are ignored.",

   "Post this article.",
   "Edit article text.",
   "Close this window and save the\n"
      "edited text in the post queue.",
   "Close this window and cancel any changes\n"
      "to the edited text.  The article is not\n"
      "deleted from the post queue.",
   "Delete the article from the post queue.",

   "The article text may be edited with and external\n"
      "editor.  Also, workbench icons or articles from\n"
      "an article list may be dropped on this object\n"
      "to compose a reply.",

   "This area shows the post body after line wrapping.",

   "This is the list of groups this article\n"
      "will be posted to.  You may drag groups\n"
      "and drop them on this list to crosspost\n"
      "the article to those groups.",
   "You may drag these groups into the list on\n"
      "the left to add them to the group list\n"
      "for this article.",
   "This is the list of groups the followups to\n"
      "this article will be directed to.  You may\n"
      "drag groups and drop them on this list to\n"
      "add the group to the followup list.",
         
   "Text",
   "Save...",
   "Insert...",
   "Insert Quoted...",
   "Clear",
   "Edit",
   "Undo",
   "Redo",
   "Cut",
   "Copy",
   "Paste",
   "Rewrap",
         
   SWX_B("Error: ") "Operation cannot be\nperformed in offline mode.",
   SWX_B("Error: ") "Unable to invoke editor.",
   SWX_B("Error: ") "Out of memory.",
   SWX_B("Error: ") "Read error on file: '%s'",
   SWX_B("Error: ") "Write error on file: '%s'",
   SWX_B("Error: ") "The file is not ASCII: '%s'",
   SWX_B("Error: ") "The server %s does\nnot allow posting.",
   SWX_B("Error: ") "Article body missing or invalid.",
   SWX_B("Error: ") "Newsgroup list is empty.",
   SWX_B("Error: ") "Subject field is empty.",
   SWX_B("Error: ") "Unable to open sig file: '%s'",
   SWX_B("Error: ") "Posting failed.\n(%s)",
   SWX_B("Error: ") "Posting aborted.",
   SWX_B("Error: ") "Unable to create `%s' directory in '%s'.",
   SWX_B("Error: ") "Unable to save text: '%s'.",
   SWX_B("Error: ") "Body line length too long:\n   Line %ld (%ld chars)",
   SWX_B("Error: ") "User EMail is set improperly.  It should be\n"
      "   of the form: user@your.domain\n"
      "   but is now set to: `%s'",
   SWX_B("Error: ") "Unable to move file `%s'\nto `%s'.",
   SWX_B("Error: ") "Improper newsgroups line.",
   SWX_B("Error: ") "This post body contains non-ASCII data and\n"
      "cannot be sent using 7-bit encoding.",
   SWX_B("Warning: ") "Body line length exceeds the\n"
      "   recommended limit of 80 characters.",
   SWX_B("Warning: ") "A large fraction of this post\n"
      "   is quoted text.  It is polite to trim\n"
      "   excessive quoted text before posting.",
   SWX_B("Warning: ") "Test posts should not be sent to\n"
      "   discussion groups.  If this is a test post\n"
      "   please send it to a group such as alt.test.",
   "_Proceed|Send to alt._test|*_Cancel",
   SWX_B("Warning: ") "This post will be sent to %ld groups.\n"
      "   Please consider trimming the newsgroup list\n"
      "   to include only the most appropriate groups.",

   NULL,
};


static struct { STRPTR Header; ULONG  Attr; } PHeaders[] = {
   SWF_NAHeader_Subject,  SWA_NPost_Subj,
   SWF_NAHeader_MsgID,    SWA_NPost_PostMsgID,
   SWF_NAHeader_From,     SWA_NPost_From,
   SWF_NAHeader_Groups,   SWA_NPost_Grp,
   SWF_NAHeader_Followup, SWA_NPost_FUps,
   SWF_NAHeader_Ref,      SWA_NPost_Refs,
   SWF_NAHeader_X_NewsRd, SWA_NPost_XNewsRdr,
   SWF_NAHeader_Organiz,  SWA_NPost_Org,
   SWF_NAHeader_Distrib,  SWA_NPost_Distrib,
   SWF_NAHeader_MimeVer,  SWA_NPost_MimeVer,
   SWF_NAHeader_MimeCTE,  SWA_NPost_MimeCTE,
   SWF_NAHeader_MimeType, SWA_NPost_MimeType,
   NULL,                  NULL,
};


STRPTR GetStr(STRPTR s, ULONG v)
{
   ULONG  x = 0, y = 0;

   while (v || y < 2) {
      s[x] = v % 62;
      v /= 62;
      s[x] = s[x] < 10 ? s[x] + '0' :
             s[x] < 36 ? s[x] + 'A' - 10 :
             s[x] + 'a' - 36;
      x++, y++;
   }

   s[x] = '\0';

   return s;
}


static ULONG CreateObj(Object *o, SW_IDATA *idata)
{
   TEXT    MsgID[SWV_MsgIDLen];
   STRPTR  host = NN_UserEmail(idata);
   struct  DateStamp ds;
   TEXT    b1[6], b2[6], b3[6], b4[6], b5[6], b6[6];

   // -- generate a message ID for this message ------------------------------

   DateStamp(&ds);

   host = SW_STRDEF(host, "@nospam.nowhere");
   host = strchr(host, '@') ? (strchr(host, '@')+1) : host;

   sprintf(MsgID, "%s.%s%s%s%s%s%s.NR@%s",
           GetStr(b1, GetUniqueID()),
           GetStr(b2, ds.ds_Days),
           GetStr(b3, ds.ds_Minute),
           GetStr(b4, ds.ds_Tick),
           GetStr(b5, ((ULONG)FindTask(0))>>4),
           GetStr(b6, ((ULONG)o)>>4),
           NN_IsPirate(idata) ? "Z" : "",
           host);

   SetAttrs(o,
            SWA_NPost_MsgID,  MsgID,
            TAG_DONE);

   setlent(o, SWA_ObjNoChildCopy, TRUE);
   return SUCCESS;
}

# define SW_USERSETUP if (FirstTime) \
                         SWLocalizeNewMenu(SW_CATALOG, SW_CATSTR, EditMenu);

# define SW_SETUP_POSTSET \
         if (CreateObj((Object *)retval, idata) != SUCCESS) \
            SWDB_RET(FAILURE);

# define DISPOSECUST \
  DoMethod(o, SWM_NPost_CloseEdit, TRUE, !idata->IDel, FALSE); \
  FV(idata->QPLineBuff);  

# define SW_USERSET                                 \
     if (Tag == SWA_AppBase_TopObj)                 \
       idata->SWA_AppBase_TopObj  = (Object *)Data; \

static ULONG NGGet(SW_IDATA *idata, ULONG Attr, STRPTR *Val)
{
   TEXT b1[6];
   
   switch (Attr) {
     case SWA_NPost_XNewsRdr:
      sprintf(idata->Tmp, "Amiga %s", 
              SWGetStr(NN_App(idata), MUIA_Application_Version)+6);
      *Val = idata->Tmp;
      return 1;

     case SWA_NPost_MimeVer:
      *Val = "1.0";
      return 1;

     case SWA_NPost_MimeCTE: {
        if (!idata->Post_Encoding) {
           *Val = NULL;
           return 1;
        }
        
        strcpy(idata->Tmp, idata->Post_Encoding);
        *Val = idata->Tmp;
        return 1;
     }

     case SWA_NPost_MimeType: {
        if (!idata->Post_ContentType) {
           *Val = NULL;
           return 1;
        }
        
        strcpy(idata->Tmp, idata->Post_ContentType);

        if (idata->Post_CharSet)
           strcat(strcat(strcat(idata->Tmp, "; charset=\""),
                         idata->Post_CharSet),
                  "\"");

        *Val = idata->Tmp;
        return 1;
     }
      
     case SWA_NPost_Org:
      *Val = NN_UserOrg(idata);
      return 1;

     case SWA_NPost_From:
      sprintf(idata->Tmp, "\"%s\" <%s>", NN_UserName(idata), NN_UserEmail(idata));
      *Val = idata->Tmp;
      return 1;

     case SWA_NPost_PostMsgID:
      sprintf(idata->Tmp, "<%s.%s>",
              GetStr(b1, GetUniqueID()),
              idata->SWA_NPost_MsgID);
      *Val = idata->Tmp;
      return 1;
   }

   return 0;
}

# define SW_USERGET if (NGGet((SW_IDATA *)INST_DATA(cl, o),           \
                              ((opGet *)msg)->opg_AttrID,             \
                              (STRPTR *)((opGet *)msg)->opg_Storage)) \
                        SWDB_RET(1);

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# define SW_Methods                       \
  SW_DO_MF(SWM_NPost_Edit);               \
  SW_DO_MF(SWM_NPost_CloseEdit);          \
  SW_DO_MF(SWM_NPost_IPost);              \
  SW_DO_MF(SWM_NPost_IPost_End);          \
  SW_DO_MF(SWM_NPost_Post);               \
  SW_DO_MF(SWM_NPost_Post_P1);            \
  SW_DO_MF(SWM_NPost_Post_P2);            \
  SW_DO_MF(SWM_NPost_Editor);             \
  SW_DO_MF(SWM_NPost_FSpec);              \
  SW_DO_MF(SWM_NPost_Delete);             \
  SW_DO_MF(SWM_NPost_CopyFile);           \
  SW_DO_MF(SWM_NPost_UpdDisp);            \
  SW_DO_MF(SWM_NPost_UpdStats);           \
  SW_DO_MF(SWM_NPost_AddText);            \
  SW_DO_MF(SWM_NPost_Abort);              \
  SW_DO_MF(SWM_NPost_IDel);               \
  SW_DO_MF(SWM_NPost_GenReply);           \
  SW_DO_MF(SWM_NPost_SaveEditData);       \
  SW_DO_MF(SWM_NPost_MenuCall);           \
  SW_DO_MF(SWM_NPost_EditMode);           \
  SW_DO_MF(SWM_NPost_Rewrap);             \
  SW_DO_MF(SWM_NPost_WrapEditTxt);        \
  SW_DO_MF(SWM_NPost_MoveFiles);          \
  SW_DO_MF(SWM_NPost_Archive);            \
  SW_DO_MF(SWM_NPost_GetTagLine);         \
  SW_DO_MF(SWM_NPost_InsFile);            \
  SW_DO_MF(SWM_NNews_Editor_SetupDone);   \
  SW_DO_MF(SWM_NNews_Editor_NewContents); \
  SW_DO_MF(SWM_NPostList_OnOffLine);      \
  SW_DO_MF(SWM_ObjList_DragQuery);        \
  SW_DO_MF(SWM_ObjList_DragDrop);         \
  SW_DO_MF(SWM_VarList_Search);           \
  SW_DO_MF(SWM_AppBase_RexxCmd);          \


# include "C-NPost-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
