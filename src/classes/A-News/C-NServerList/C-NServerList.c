# include "C-NServerList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NTCP

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_OB(SWA_NServerList_ListCol,    SWAM_NoLab,  SWAM_SGI | SWAM_ObjRead),

   SWP_NNews_Attrs,
   SWP_NList_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_I        "None",
   MUIX_I        "Unknown",
   MUIX_U MUIX_B "Default",
   MUIX_U MUIX_B "Server",
   MUIX_U MUIX_B "Success",
   MUIX_U MUIX_B "Fail",
   MUIX_U MUIX_B "Connect %",
   MUIX_U MUIX_B "# Groups",
   MUIX_U MUIX_B "Timeout",
   MUIX_U MUIX_B "Retry Delay",
   MUIX_U MUIX_B "Retries",
   MUIX_U MUIX_B "Hostname",
   MUIX_U MUIX_B "Posting",
   MUIX_U MUIX_B "Type",

   "Yes",
   "No",

   "Def",

   MUIX_C "News Servers (%ld)",
   "_Add Server",
   "_Delete",
   "_Sort",
   "_Reset Stats",
   "S_et Default",
   "_Get Env",
   "Cx Timeout: ",
   "Retry Delay: ",
   "Max Retries: ",
   "Auth User: ",
   "Password: ",
   "Type: ",
   "NNTP",
   "FTP",
   "Port: ",

   "Add a new server to the list.",
   "Delete the selected servers from the list.",
   "Sort the servers by hostname.",
   "Reset the connection statistics for\n"
      "the selected servers.",
   "Set the active entry as the default server.",
   "Creates a new server who's hostname is\n"
      "the contents of the NNTPSERVER\n"
      "environment variable.",
   "Connection timeout value.",
   "The amount of time between connect retries.",
   "The maximum number of connect retries.",
   "The user name used for NNTP authentication.",
   "The password used for NNTP authentication.",
   "Selects whether this is an NNTP\n"
      "server or an FTP server.",
   "The NNTP port number to connect to.",

   "Select which columns should be displayed\n"
      "in the server list.  Hotkey: '-'",
   "Copy the selected entries to the\n"
      "clipboard as text.  Hotkey: 'c'",

   "Use the default server.",

   SWX_B("Error: ") "No server specified.",
   SWX_B("Error: ") "No NNTPSERVER environment\nvariable defined.",

   NULL,
};

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NServer_Default,
   SWA_NServer_Host,
   SWA_NServer_Port,
   SWA_NServer_ConnectT,
   SWA_NServer_ConnectS,
   SWA_NServer_NGroups,
   SWA_NServer_CxTimeout,
   SWA_NServer_RetryDelay,
   SWA_NServer_MaxRetries,
   SWA_NServer_PostOK,
   SWA_NServer_Type,
   NULL
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


static STRPTR ColSpecs[] = {
   "COL=0 W=5 MAW=5 BAR",
   "COL=1 W=40 BAR",
   "COL=2 P=" MUIX_R " W=10 BAR",
   "COL=3 P=" MUIX_R " W=10 BAR",
   "COL=4 P=" MUIX_R " W=10 BAR",
   "COL=5 P=" MUIX_R " W=10 BAR",
   "COL=6 P=" MUIX_R " W=10 BAR",
   "COL=7 P=" MUIX_R " W=10 BAR",
   "COL=8 P=" MUIX_R " W=10 BAR",
   "COL=9 W=20 BAR",
   "COL=10 W=8 MAW=8 BAR",
   "COL=11 W=10 BAR",
   NULL,
};



/*****************************************************************************
 * Function: Generate an object to insert                                    *
 *****************************************************************************/

static Object *MkInsObj(SW_IDATA *idata, STRPTR Name)
{
   Object *no = MUI_NewObject(SWC_NServer,
                              AppBaseDefTop(idata),
                              SWA_NServer_Host, Name,
                              TAG_DONE);
   
   if (SWGetUL(idata->SWA_NList_L, MUIA_List_Entries) == 0)
      set(no, SWA_NServer_Default, TRUE);

   return no;
}


/*****************************************************************************
 * Function: Create a column list object                                     *
 *****************************************************************************/

static ULONG CreateListCol(Object *o, SW_IDATA *idata)
{
   ULONG x;
   
   static STRPTR ColNames[16];

   for (x=0; ColSpecs[x]; x++)
      ColNames[x] = GetS(SWS_L_Default + x) + sizeof(MUIX_U MUIX_B) - 1;

   ColNames[x] = NULL;

   if (!(idata->SWA_NServerList_ListCol =
         MUI_NewObject(SWC_ListCol,
                       SWA_ListCol_ColSpecs, ColSpecs,
                       SWA_ListCol_ColNames, ColNames,
                       SWA_ListCol_Hook,     &idata->DspHook,
                       SWA_ListCol_List,     o,
                       TAG_DONE)))
      return FAILURE;

   DoMethod(idata->SWA_NServerList_ListCol, SWM_ListCol_HideCols,
            8, 9, 10, 11, -1);

   return SUCCESS;
}


/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

# include "C-NServerList-Hooks.h"
# include "S-Lists-Hooks.h"

static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l),GetS(h)); }

/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static ULONG CreateObj(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   Object *g, *to, *b_Add, *b_Del, *b_Sort, *b_Reset,
          *b_MkDef = NULL, *b_RdEnv;

   ULONG    Tiny = GetTagUL(SWA_NServerList_Tiny,        NULL, ti);
   Object  *SObj = GetTagObj(SWA_NServerList_HostStrObj, NULL, ti);

   if (Tiny && !SObj) SWDB_RET(FAILURE);
   
   set(o, MUIA_Group_Horiz, FALSE);

   if (!Tiny) {
      DoMethod(o, OM_ADDMEMBER,
               to = SWUI_FrameText(GetS(SWS_L_TopTitle), 0,
                                   (HGroup,
                                    Child, (Object *)
                                       DoMethod(idata->SWA_NServerList_ListCol,
                                                SWM_ListCol_Popup,
                                                '-', GetS(SWS_H_ListCol)),
                                    Child, (Object *)
                                       DoMethod(idata->SWA_NServerList_ListCol,
                                                SWM_ListCol_ClipBt,
                                                'c', GetS(SWS_H_Clip)),
                                    End), NULL));

      if (!to) return FAILURE;
   }

   DoMethod(o, OM_ADDMEMBER,
            idata->SWA_NList_LV = ListviewObject,
            MUIA_CycleChain,           1,
            MUIA_Listview_Input,       TRUE,
            MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
            MUIA_Listview_DragType,    MUIV_Listview_DragType_Immediate,
            MUIA_Listview_List, idata->SWA_NList_L = ObjListObject,
            InputListFrame,
            SWA_ObjList_Object,       o,
            MUIA_List_Format,         DoMethod(idata->SWA_NServerList_ListCol,
                                               SWM_ListCol_BuildFmt),
            MUIA_List_DragSortable,  TRUE,
            MUIA_List_Title,         TRUE,
            MUIA_List_DisplayHook,   &idata->DspHook,
            MUIA_List_DestructHook,  &idata->DstHook,
            MUIA_List_CompareHook,   &idata->CmpHook,
            End,
          End);

   if (!idata->SWA_NList_LV) return FAILURE;

   SWM_SetListColNotify(idata->SWA_NServerList_ListCol, idata->SWA_NList_L);

   if (!Tiny) {
      static STRPTR TypeLab[4];

      TypeLab[0] = GetS(SWS_L_TypeNNTP);
      TypeLab[1] = GetS(SWS_L_TypeFTP);
      TypeLab[2] = NULL;
      
      DoMethod(idata->SWA_NList_L, MUIM_Notify,
               MUIA_List_Entries, MUIV_EveryTime,
               to, 4, MUIM_SetAsString, MUIA_Text_Contents,
               GetS(SWS_L_TopTitle), MUIV_TriggerValue);

      DoMethod(o, OM_ADDMEMBER,
               g = VGroup,
                  MUIA_Background, MUII_GroupBack,
                  Child, ColGroup(2),
                     Child, b_Add  = MkBt(SWS_B_Add,    SWS_H_Add),
                     Child, HGroup,
                        Child, s_Host = StringObject,
                                           StringFrame,
                                           MUIA_CycleChain,     1,
                                           MUIA_HorizWeight, 5000,
                                        End,
                        Child, Label2(GetS(SWS_L_Type)),
                        Child, s_Type = CycleObject,
                           MUIA_Cycle_Entries, TypeLab,
                           MUIA_CycleChain,    1,
                           MUIA_ShortHelp,     GetS(SWS_H_Type),
                           MUIA_Disabled,      TRUE,
                        End,
                     End,
                     Child, Label2(GetS(SWS_L_Port)),
                     Child, s_Port = NewObject(SWObjStringClass(), NULL,
                        StringFrame,
                        MUIA_CycleChain,    1,
                        MUIA_String_Accept, "0123456879",
                        MUIA_String_MaxLen,      8,
                        MUIA_ShortHelp,     GetS(SWS_H_Port),
                        MUIA_Disabled,      TRUE,
                        TAG_DONE),
                     Child, Label2(GetS(SWS_L_CxTO)),
                     Child, s_CxTO = NewObject(SWTimesliderClass(), NULL,
                                               MUIA_Numeric_Min,   5*1000,
                                               MUIA_Numeric_Max,   10*60*1000,
                                               MUIA_Numeric_Value, 30*1000,
                                               MUIA_CycleChain,    1,
                                               MUIA_ShortHelp, GetS(SWS_H_CxTO),
                                               MUIA_Disabled,      TRUE,
                                               TAG_DONE),
                     Child, Label2(GetS(SWS_L_CxRT)),
                     Child, s_CxRT = NewObject(SWTimesliderClass(), NULL,
                                               MUIA_Numeric_Min,   5*1000,
                                               MUIA_Numeric_Max,   10*60*1000,
                                               MUIA_Numeric_Value, 30*1000,
                                               MUIA_CycleChain,    1,
                                               MUIA_ShortHelp, GetS(SWS_H_CxRT),
                                               MUIA_Disabled,      TRUE,
                                               TAG_DONE),
                     Child, Label2(GetS(SWS_L_CxMR)),
                     Child, s_CxMR = SliderObject,
                        MUIA_Numeric_Min,   0,
                        MUIA_Numeric_Max,   50,
                        MUIA_Numeric_Value, 5,
                        MUIA_CycleChain,    1,
                        MUIA_ShortHelp,     GetS(SWS_H_CxMR),
                        MUIA_Disabled,      TRUE,
                     End,
                     Child, Label2(GetS(SWS_L_AUser)),
                     Child, HGroup,
                        Child, s_User = StringObject,
                                           StringFrame,
                                           MUIA_CycleChain,  1,
                                           MUIA_ShortHelp, GetS(SWS_H_AuthUser),
                                           MUIA_String_MaxLen, 48,
                                           MUIA_Disabled,      TRUE,
                                        End,
                        Child, Label2(GetS(SWS_L_APass)),
                        Child, s_Pass = StringObject,
                                           StringFrame,
                                           MUIA_CycleChain,    1,
                                           MUIA_String_Secret, TRUE,
                                           MUIA_ShortHelp, GetS(SWS_H_AuthPass),
                                           MUIA_String_MaxLen, 48,
                                           MUIA_Disabled,      TRUE,
                                        End,
                     End,
                  End,
                  Child, HGroup,
                     MUIA_Group_SameSize, TRUE,
                     Child, b_Del   = MkBt(SWS_B_Del,   SWS_H_Del),
                     Child, b_Sort  = MkBt(SWS_B_Sort,  SWS_H_Sort),
                     Child, b_Reset = MkBt(SWS_B_Reset, SWS_H_Reset),
                     Child, b_MkDef = MkBt(SWS_B_MkDef, SWS_H_MkDef),
                     Child, b_RdEnv = MkBt(SWS_B_RdEnv, SWS_H_RdEnv),
                  End,
               End);
      s_CxTOAttr  = SWA_NServer_CxTimeout;
      s_CxTODef   = 30*1000;
      s_CxTOGAttr = MUIA_Numeric_Value;
      s_CxRTAttr  = SWA_NServer_RetryDelay;
      s_CxRTDef   = 30*1000;
      s_CxRTGAttr = MUIA_Numeric_Value;
      s_CxMRAttr  = SWA_NServer_MaxRetries;
      s_CxMRDef   = 0,
      s_CxMRGAttr = MUIA_Numeric_Value;
      s_UserAttr  = SWA_NServer_AuthUser;
      s_PassAttr  = SWA_NServer_AuthPass;
      s_TypeAttr  = SWA_NServer_Type;
      s_TypeDef   = SWV_NServer_Type_NNTP;
      s_TypeGAttr = MUIA_Cycle_Active;
      s_PortAttr  = SWA_NServer_Port;
      s_PortGAttr = SWA_ObjString_NumVal;
   } else {
      if (!(idata->FmtSlider = NewObject(SWTimesliderClass(), NULL, TAG_DONE)))
         return FAILURE;
      
      DoMethod(o, OM_ADDMEMBER,
               g = HGroup,
                  MUIA_Background,     MUII_GroupBack,
                  MUIA_Group_SameSize, TRUE,
                  Child, b_Add   = MkBt(SWS_B_Add,   SWS_H_Add),
                  Child, b_Del   = MkBt(SWS_B_Del,   SWS_H_Del),
                  Child, b_Sort  = MkBt(SWS_B_Sort,  SWS_H_Sort),
                  Child, b_Reset = MkBt(SWS_B_Reset, SWS_H_Reset),
                  Child, b_RdEnv = MkBt(SWS_B_RdEnv, SWS_H_RdEnv),
               End);
      s_Host = SObj;
   }

   if (!g) return FAILURE;

   SetAttrs(s_Host,
            MUIA_Disabled,              TRUE,
            MUIA_String_AttachedList,   idata->SWA_NList_LV,
            TAG_DONE);

   s_HostAttr  = SWA_NServer_Host;

   DoMethod(b_Sort,  NOTP, o, 1, MUIM_List_Sort);
   DoMethod(b_Del,   NOTP, o, 1, SWM_NList_RemoveSel);
   DoMethod(b_Add,   NOTP, o, 2, SWM_NList_Add, NULL);
   DoMethod(b_RdEnv, NOTP, o, 2, SWM_NServerList_NNTPSERVER, TRUE);

   if (b_MkDef) {
      DoMethod(b_MkDef, NOTP, o, 1, SWM_NServerList_ResetDefault);
      DoMethod(b_MkDef, NOTP, o, 8, SWM_Root_ListDoMethod,
               idata->SWA_NList_LV, SWV_ListDoMethod_Active,
               SWV_ListDoMethod_Entry, 3,
               MUIM_Set, SWA_NServer_Default, TRUE);
   }

   DoMethod(b_Add, NOTP, MUIV_Notify_Window, 3,
            MUIM_Set, MUIA_Window_ActiveObject, s_Host);

   DoMethod(b_Reset, NOTP, o, 6, SWM_Root_ListDoMethod,
            idata->SWA_NList_LV,
            SWV_ListDoMethod_Selected, SWV_ListDoMethod_Entry, 1,
            SWM_NServer_ResetStats);

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   return SUCCESS;
}

static void SWNListUserSet(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data);

static void SetTop(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (Tag == SWA_AppBase_TopObj) {
      idata->SWA_AppBase_TopObj     = (Object *)Data, 

      DoMethod(o, SWM_NList_ListSet, Tag, Data);
   }

   SWNListUserSet(o, idata, Tag, Data);
}

# define SW_USERSET SetTop(o, idata, Tag, Data)

# define SETUPCUST                                                \
         if (CreateListCol((Object *)retval, idata) != SUCCESS || \
             CreateObj((Object *)retval, idata, ti) != SUCCESS)   \
            SWDB_RET(NULL);

# define SW_USERSETUP    PopSrvListInit(libbase);
# define SW_USERCLEANUP  PopSrvListExit();
   
// -- info for list clicking to set attrs ------------------------------------

# define SW_NList_ClickCol
# define SNLT_ListCol (idata->SWA_NServerList_ListCol)
static ULONG SNLT_Attr[] = { SWA_NServer_Default, 0 };
static ULONG SNLT_Cols[] = { 0 };

#  define DISPOSECUST MUI_DisposeObject(idata->FmtSlider)

#  define SW_Methods                          \
   SW_DO_MF(SWM_AppBase_MenuCall);            \
   SW_DO_MF(SWM_AppBase_RexxCmd);             \
   SW_DO_MF(SWM_NServerList_ResetDefault);    \
   SW_DO_MF(SWM_NServerList_GetDefault);      \
   SW_DO_MF(SWM_NServerList_NNTPSERVER);      \
   SW_DO_MF(SWM_NServerList_PopSrvListClass); \
   SW_DO_MF(SWM_NServerList_FindCreate);      \
   SW_DO_MF(MUIM_Setup);                      \
   SW_DO_MF(MUIM_Cleanup);

# include "C-PopSrvList.h"
# include "S-Lists-Mthd.h"
# include "C-NServerList-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
