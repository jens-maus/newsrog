# include "C-NTCPList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_NTCP

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_OB(SWA_NTCPList_ListCol, SWAM_NoLab, SWAM_SGI | SWAM_ObjRead),
   SWAM_IN(SWA_NTCPList_Online,  SWAM_NoLab, SWAM_SGIN, FALSE),

   SWP_NList_Attrs,
   SWP_NNews_Attrs,
   SWP_Root_Attrs,
   SWAM_DONE
};

// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_I "Untitled",
   MUIX_I "None",
   MUIX_U MUIX_B "Nodename",
   MUIX_U MUIX_B "Description",
   MUIX_U MUIX_B "TCP Status",
   MUIX_U MUIX_B "Speed",
   MUIX_U MUIX_B "In Bytes",
   MUIX_U MUIX_B "In Lines",
   MUIX_U MUIX_B "Hostname",

   MUIX_C "Network Connections (%ld)",

   "Disconnect",
   "Disconnect All",

   "This is a list of all TCP connections in use.\n"
      "Connections are reused for sequential commands\n"
      "but parallel connections need multiple TCP\n"
      "connections open at once.  If a connection gets\n"
      "\"stuck\" it can be disconnected here, which\n"
      "will abort the operation that was using it.",

   "Disconnect the select connections.",
   "Disconnect all connections.",

   "Select which columns should be displayed\n"
      "in the TCP connection list.  Hotkey: '-'",

   "Copy the selected entries to the\n"
      "clipboard as text.  Hotkey: 'c'",
   
   NULL,
};

static ULONG SWA_NList_NotifyAttrs[] = {
   SWA_NTCP_Title,
   SWA_NTCP_Status,
   SWA_NTCP_Host,
   SWA_NTCP_BPS,
   SWA_NTCP_RecvBytes,
   SWA_NTCP_RecvLines,
   NULL
};


STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


static STRPTR ColSpecs[] = {
   "COL=0 W=50 MAW=50 BAR",
   "COL=1 W=50 MAW=50 BAR",
   "COL=2 W=50 MAW=50 BAR",
   "COL=3 P=" MUIX_R " W=10 MAW=10 BAR",
   "COL=4 P=" MUIX_R " W=10 MAW=10 BAR",
   "COL=5 P=" MUIX_R " W=10 MAW=10 BAR",
   "COL=6 W=50 MAW=50 BAR",
   NULL,
};

/*****************************************************************************
 *   Set up the hook data                                                    *
 *****************************************************************************/

# include "C-NTCPList-Hooks.h"
# include "S-Lists-Hooks.h"


static Object *MkBt(ULONG l, ULONG h) { return SWUI_Button(GetS(l), GetS(h)); }


/*****************************************************************************
 * Function: create the list subobject                                       *
 *****************************************************************************/

static ULONG CreateListmgr(Object *o, SW_IDATA *idata)
{
   Object *g, *B_Dis, *B_DisAll, *to;

   set(o, MUIA_Group_Horiz, FALSE);

   DoMethod(o, OM_ADDMEMBER,
            to = SWUI_FrameText(GetS(SWS_L_TopTitle), 0,
                                (HGroup,
                                 Child, (Object *)
                                    DoMethod(idata->SWA_NTCPList_ListCol,
                                             SWM_ListCol_Popup,
                                             '-', GetS(SWS_H_ListCol)),
                                 Child, (Object *)
                                        DoMethod(idata->SWA_NTCPList_ListCol,
                                                 SWM_ListCol_ClipBt,
                                                 'c', GetS(SWS_H_Clip)),
                                 End), NULL));

   if (!to) return FAILURE;
   
   DoMethod(o, OM_ADDMEMBER,
            idata->SWA_NList_LV = ListviewObject,
            MUIA_CycleChain,           1,
            MUIA_ShortHelp,            GetS(SWS_H_List),
            MUIA_Listview_Input,       TRUE,
            MUIA_Listview_MultiSelect, MUIV_Listview_MultiSelect_Default,
            MUIA_Listview_List, idata->SWA_NList_L = ListObject,
            InputListFrame,
            MUIA_List_Format,        DoMethod(idata->SWA_NTCPList_ListCol,
                                           SWM_ListCol_BuildFmt),
            MUIA_List_DragSortable,  FALSE,
            MUIA_List_Title,         TRUE,
            MUIA_List_DisplayHook,   &idata->DspHook,
            MUIA_List_DestructHook,  &idata->DstHook,
            End,
          End);

   if (!idata->SWA_NList_LV) return FAILURE;

   SWM_SetListColNotify(idata->SWA_NTCPList_ListCol, idata->SWA_NList_L);

   DoMethod(idata->SWA_NList_L, MUIM_Notify,
            MUIA_List_Entries, MUIV_EveryTime,
            to, 4, MUIM_SetAsString, MUIA_Text_Contents,
            GetS(SWS_L_TopTitle), MUIV_TriggerValue);

   DoMethod(o, OM_ADDMEMBER,
            g = HGroup,
               Child, B_Dis    = MkBt(SWS_B_Dis,    SWS_H_Dis),
               Child, B_DisAll = MkBt(SWS_B_DisAll, SWS_H_DisAll),
            End);

   if (!g) return FAILURE;

   DoMethod(B_Dis,    NOTP, o, 7, SWM_Root_ListDoMethod,
            idata->SWA_NList_LV,
            SWV_ListDoMethod_Selected, SWV_ListDoMethod_Entry, 2,
            SWM_NTCP_Disconnect, FALSE);

   DoMethod(B_DisAll, NOTP, o, 7, SWM_Root_ListDoMethod,
            idata->SWA_NList_LV,
            SWV_ListDoMethod_All,      SWV_ListDoMethod_Entry, 2,
            SWM_NTCP_Disconnect, FALSE);

   setlent(o, SWA_ObjNoChildCopy, TRUE);

   return SUCCESS;
}

static ULONG CreateListCol(Object *o, SW_IDATA *idata)
{
   ULONG x;
   
   static STRPTR ColNames[8];

   for (x=0; ColSpecs[x]; x++)
      ColNames[x] = GetS(SWS_L_Node + x) + sizeof(MUIX_U MUIX_B) - 1;

   ColNames[x] = NULL;

   if (!(idata->SWA_NTCPList_ListCol =
         MUI_NewObject(SWC_ListCol,
                       SWA_ListCol_ColSpecs, ColSpecs,
                       SWA_ListCol_ColNames, ColNames,
                       SWA_ListCol_Hook,     &idata->DspHook,
                       SWA_ListCol_List,     o,
                       TAG_DONE)))
      return FAILURE;

   DoMethod(idata->SWA_NTCPList_ListCol, SWM_ListCol_HideCols, 5, 6, -1);

   return SUCCESS;
}

# define SETUPCUST                                                   \
            if (CreateListCol((Object *)retval, idata) == FAILURE || \
                CreateListmgr((Object *)retval, idata) == FAILURE)   \
               SWDB_RET(NULL);

# define DISPOSECUST \
         DoMethod(o, SWM_NTCPList_Online, SWV_NTCPList_Online_Off)

# define SW_Methods                     \
      SW_DO_MF(SWM_NTCPList_MaxSocket); \
      SW_DO_MF(SWM_NTCPList_Online);    \
      SW_DO_MF(SWM_AppBase_MenuCall);   \

# include "C-NTCPList-Mthd.h"
# include "S-Lists-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
