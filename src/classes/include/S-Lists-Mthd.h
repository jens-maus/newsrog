
/*****************************************************************************
 * Functions & defns used by list classes                                    *
 *****************************************************************************/

# ifndef AB_SetMod
#   define AB_SetMod(i)
# endif

# define SW_LISTMETHODS TRUE


/*****************************************************************************
 * Method: assign one list to another                                        *
 *****************************************************************************/

# ifndef SW_ListAsgn
#   define SW_ListAsgn(o) ((Object *)DoMethod(o, SWM_Clone))
# endif

static void SW_ListAssign(Object *o1, Object *o2,
                          SW_IDATA *idata1, SW_IDATA *idata2)
{
   Object *g;
   ULONG  x;

   DoMethod(o1, MUIM_List_Clear);
   FSet(idata1->NLFlags, SWF_NList_NoSetNotify);
   set(idata1->SWA_NList_L, MUIA_List_Quiet, TRUE);
   
   SW_MUIListLoop(idata2->SWA_NList_L, x, g, TRUE) {
      DoMethod(o1, MUIM_List_InsertSingle, SW_ListAsgn(g),
               MUIV_List_Insert_Bottom);
   }

   FReset(idata1->NLFlags, SWF_NList_NoSetNotify);
   DoMethod(o1, SWM_NList_SetNotify);

   set(idata1->SWA_NList_L, MUIA_List_Quiet, FALSE);
}


# define SWM_NOASSIGNAPP
# define SWM_USERASSIGN SW_ListAssign(o, msg->o, idata, idata2);

/*****************************************************************************
 * Method: pass method to sub list                                           *
 *****************************************************************************/
   
METHOD(SWM_NList_ListMethod, Msg)
{
   SWDB_RET(DoMethodA(idata->SWA_NList_L, msg));
} SDFT


/*****************************************************************************
 * Method: pass method to sub list, reset notify                             *
 *****************************************************************************/
   
METHOD(SWM_NList_ListMethodN, Msg)
{
   ULONG rc = DoMethodA(idata->SWA_NList_L, msg);

   DoMethod(o, SWM_NList_SetNotify);
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: pass method to each entry in the list                             *
 *****************************************************************************/
   
METHOD(SWM_NList_ObjMethod, ULONG *)
{
   ULONG x;
   Object *lo;

   if (idata->SWA_NList_L) 
      SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
         DoMethodA(lo, (Msg)(msg+1));

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: pass method to each entry in the list                             *
 *****************************************************************************/
   
METHOD(SWM_NList_ObjMethodA, SWP_NList_ObjMethodA *)
{
   ULONG x;
   Object *lo;

   if (idata->SWA_NList_L)
      SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
         DoMethodA(lo, msg->msg);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: set up one notification                                         *
 *****************************************************************************/
   
static void SetN1(SW_IDATA *idata, Object *lo, ULONG attr, ULONG x)
{
   if (attr) {
      DoMethod(lo, MUIM_KillNotifyObj, attr, idata->SWA_NList_L);
      DoMethod(lo, MUIM_Notify, attr, MUIV_EveryTime,
               idata->SWA_NList_L, 2, MUIM_List_Redraw, x);
   }
}


/*****************************************************************************
 * Method: set up all notifications                                          *
 *****************************************************************************/

METHOD(SWM_NList_SetNotify, Msg)
{
   ULONG x, y;
   Object *lo;

   if (FIsSet(idata->NLFlags, SWF_NList_NoSetNotify|SWF_NList_NoNotifies))
      SWDB_RET(0);

   // - reset notifications on all list objects ------------------------------

   if (idata->SWA_NList_L)
      SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
         for (y=0; SWA_NList_NotifyAttrs[y]; y++)
            SetN1(idata, lo, SWA_NList_NotifyAttrs[y], x);

   set(o, SWA_NList_Changed, TRUE);

   DoMethod(o, SWM_NList_Change, SWGetUL(idata->SWA_NList_L, MUIA_List_Active));

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: kill notify on one object                                         *
 *****************************************************************************/

static void Kill1N(SW_IDATA *idata, Object *lo)
{
   ULONG y;

   for (y=0; lo && SWA_NList_NotifyAttrs[y]; y++)
      DoMethod(lo, MUIM_KillNotifyObj, SWA_NList_NotifyAttrs[y],
               idata->SWA_NList_L);
}

   
/*****************************************************************************
 * Method: kill up all notifications                                         *
 *****************************************************************************/

METHOD(SWM_NList_KillNotify, Msg)
{
   ULONG x;
   Object *lo;

   if (FIsSet(idata->NLFlags, SWF_NList_NoNotifies))
      SWDB_RET(0);

   if (idata->SWA_NList_L)
      SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
         Kill1N(idata, lo);

   SWDB_RET(0);
}


/*****************************************************************************
 * Method: pass method to sub list, reset notify                             *
 *****************************************************************************/
   
METHOD(MUIM_List_Remove, Msg)
{
   ULONG rc;

   set(o, MUIA_List_Quiet, TRUE);
   rc = DoMethodA(idata->SWA_NList_L, msg);
   DoMethod(o, SWM_NList_SetNotify);
   set(o, MUIA_List_Quiet, FALSE);

#  ifdef SW_NList_Remove
     SW_NList_Remove;
#  endif

   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Find object by name                                               *
 *****************************************************************************/

METHOD(SWM_NList_FindObj, SWP_NList_FindObj *)
{
   LONG x;
   Object *lo;
   
   if (SW_STRISNULL(msg->Val)) SWDB_RET(NULL);

   if (idata->SWA_NList_L)
      SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
         if (!strcmp(msg->Val, SWGetStr(lo, msg->Attr)))
            SWDB_RET(lo);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: send command to particular obj by name                            *
 *****************************************************************************/

METHOD(SWM_NList_SendCmd, ULONG *)
{
   Object *lo;
   ULONG  Attr = GetTagUL(SWT_NList_Attr, NULL, msg+1);
   UBYTE  *Val = GetTagUB(SWT_NList_Val,  NULL, msg+1);

   DoMethod(o, MUIM_List_GetEntry, 0, &lo);

   lo = Val ? (Object *)DoMethod(o, SWM_NList_FindObj, Attr, Val) : lo;
   
   if (!lo) SWDB_RET(NULL);  // no server found

   SWDB_RET(DoMethodA(lo, (Msg)msg));
} SDFT


/*****************************************************************************
 * Method: Change active entry                                               *
 *****************************************************************************/

#ifdef SW_NListTrackAttrNum

METHOD(SWM_NList_Change, SWP_NList_Change *)
{
   ULONG x;
   Object *Item = NULL;

   if (msg->Active != MUIV_List_Active_Off)
      DoMethod(idata->SWA_NList_L, MUIM_List_GetEntry, msg->Active, &Item);

#  ifdef SW_NList_Change_Pre
      SW_NList_Change_Pre;
#  endif
   
   for (x=0; x<SW_NListTrackAttrNum; x++) {
      Object *Gad    = idata->SW_NListTRA[x].Gad;
      ULONG  GadAttr = idata->SW_NListTRA[x].GadAttr,
             SetV    = idata->SW_NListTRA[x].UnsetVal,
             ObjAttr = idata->SW_NListTRA[x].ObjAttr;

      if (!Gad) continue;

      if (!GadAttr) GadAttr = MUIA_String_Contents;

      if (!SetV && GadAttr == MUIA_String_Contents)
         SetV    = (ULONG)"";

      if (idata->SWA_NListLastTLA)
         SW_KILLNOTIFYUPD(Gad, GadAttr, idata->SWA_NListLastTLA, ObjAttr);

      if (Item) {
         SetV = SWGetUL(Item, ObjAttr);
         SW_NOTIFYUPD(Gad, GadAttr, Item, ObjAttr);
      }

      SetAttrs(Gad,
               MUIA_NoNotify,      TRUE,
               MUIA_Disabled,      msg->Active == MUIV_List_Active_Off,
               GadAttr,            SetV,
               TAG_DONE);
   }

   idata->SWA_NListLastTLA = Item;

#  ifdef SW_NList_Change_Post
      SW_NList_Change_Post;
#  endif

   SWDB_RET(0);
} SDFT
#endif


/*****************************************************************************
 * Method: Add new entry                                                     *
 *****************************************************************************/

# ifdef SW_NList_NewObj
METHOD(SWM_NList_Add, SWP_NList_Add *)
{
   Object *Obj = msg->o ? msg->o : SW_NList_NewObj;

   if (!Obj) SWDB_RET(0);

   AB_SetMod(idata);

   DoMethod(o, MUIM_List_InsertSingle, Obj, MUIV_List_Insert_Bottom);
   set(idata->SWA_NList_L, MUIA_List_Active,
       SWGetLong(idata->SWA_NList_L, MUIA_List_InsertPosition));

   SWDB_RET(Obj);
} SDFT
# endif


/*****************************************************************************
 * Method: Remove a particular object from the list                          *
 *****************************************************************************/

# if defined SW_NList_RemoveObj || defined SW_NList_DispHook
METHOD(SWM_NList_RemoveObj, SWP_NList_RemoveObj *)
{
   ULONG x;
   Object *lo;

#  ifdef SWV_NList_TrackedMax
   if (FIsSet(idata->NLFlags, SWF_NList_DelRecurFlag))
      SWDB_RET(0);
#  endif

#  ifdef SW_NList_RemoveObjHook
       SW_NList_RemoveObjHook(o, msg->o);
#  endif

   if (idata->SWA_NList_L)
      SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
         if (lo == msg->o) {
            DoMethod(idata->SWA_NList_L, MUIM_List_Remove, x);
            break;
         }

   SWDB_RET(0);
} SDFT
# endif


/*****************************************************************************
 * Method: Write object to disk                                              *
 *****************************************************************************/

# ifndef SWM_WRITE_F
# define SWM_WRITE_F
METHOD(SWM_Write, SWP_Write *)
   SWDB_OBJMSG(__FUNC__, o)
{
   ULONG PrefsVer, Count;
   
#  ifdef NL_Write_Pre
      NL_Write_Pre;
#  endif
   
   if (SWWriteAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, SW_CLIB))
      SWDB_RET(FAILURE);

   PrefsVer = SWV_NList_PrefsVer;
   Count    = SWGetUL(idata->SWA_NList_L, MUIA_List_Entries);
   
   if (SW_WRITE(msg->fp, PrefsVer) ||
       SW_WRITE(msg->fp, Count)) SWDB_RET(NULL);

   DoMethod(o, SWM_NList_ObjMethod, SWM_Write, msg->fp);

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT
# endif

/*****************************************************************************
 * Method: Read object from disk                                             *
 *****************************************************************************/

# ifndef SWM_READINT_F
# define SWM_READINT_F
METHOD(SWM_ReadInt, SWP_Read *)
   SWDB_OBJMSG(__FUNC__, o)
{
   ULONG x, Count, PrefsVer;
   Object *w;
   UBYTE ClassName[SW_MaxClassNameLen];
   Object *Ret = NULL;

   DoMethod(o, MUIM_List_Clear);

   if (SWReadAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, o)) SWDB_RET(NULL);

   if (SW_READ(msg->fp, PrefsVer) || SW_READ(msg->fp, Count))
      SWDB_RET(NULL);
   
   set(idata->SWA_NList_L, MUIA_List_Quiet, TRUE);

#  ifdef NL_ReadCond
   if (NL_ReadCond)
#  endif
      for (x=0; x<Count; x++) {
         if (w = (Object *)DoMethod(o, SWM_Read, msg->fp)) {
            DoMethod(idata->SWA_NList_L, MUIM_List_InsertSingle, w,
                     MUIV_List_Insert_Bottom);
   
#           ifdef SW_NList_ReadObjPost
               SW_NList_ReadObjPost(w);
#           endif
         }
         else {
            DoMethod(o, MUIM_List_Clear);
            set(idata->SWA_NList_L, MUIA_List_Quiet, FALSE);
            goto Done;
         }
      }

#  ifdef NL_ListPostRead
     NL_ListPostRead
#  endif
   
   DoMethod(o, SWM_NList_SetNotify);
   
   if (READCNAME(msg->fp, ClassName)) goto Done;
   if (!DoSuperMethodA(cl, o, msg))   goto Done;

   Ret = o;

  Done:
   set(idata->SWA_NList_L, MUIA_List_Quiet, FALSE);

   SWDB_RET(Ret);
} SDFT
# endif

/*****************************************************************************
 * Method: Add a tracked list                                                *
 *****************************************************************************/

# ifdef SWV_NList_TrackedMax
METHOD(SWM_NList_AddTrackedList, SWP_NList_AddTrackedList *)
{
   ULONG x;

   if (!idata->SWA_NList_Master) {
      FSet(idata->NLFlags,  SWF_NList_MasterFlag);

      if (msg->list != o)
         for (x=0; x<SWV_NList_TrackedMax; x++)
            if (!idata->SWA_NList_Tracked[x]) {
               idata->SWA_NList_Tracked[x] = msg->list;
               break;
            }
   } else
      if (idata->SWA_NList_Master)
         SWDB_RET(DoMethodA(idata->SWA_NList_Master, msg));

   SWDB_RET(0);   
} SDFT
# endif


/*****************************************************************************
 * Method: Remove a tracked list                                             *
 *****************************************************************************/

# ifdef SWV_NList_TrackedMax
METHOD(SWM_NList_RemTrackedList, SWP_NList_RemTrackedList *)
{
   ULONG x;

   if (!FIsSet(idata->NLFlags,  SWF_NList_MasterFlag))
      if (idata->SWA_NList_Master)
         SWDB_RET(DoMethodA(idata->SWA_NList_Master, msg));
      else
         SWDB_RET(NULL);

   for (x=0; x<SWV_NList_TrackedMax; x++)
      if (idata->SWA_NList_Tracked[x] == msg->list) {
         idata->SWA_NList_Tracked[x] = NULL;
         break;
      }

   if (o == msg->list)
      for (x=0; x<SWV_NList_TrackedMax; x++)
         if (idata->SWA_NList_Tracked[x])
            set(idata->SWA_NList_Tracked[x], SWA_NList_Master, NULL);
   
   SWDB_RET(0);
} SDFT
# endif


/*****************************************************************************
 * Method: Call method on all tracked lists                                  *
 *****************************************************************************/

METHOD(SWM_NList_DoTracked, Msg)
{
   ULONG rc;

#  ifdef SWV_NList_TrackedMax
   {
      ULONG x;
      
      if (FIsSet(idata->NLFlags, SWF_NList_RecurFlag))
         SWDB_RET(0);
      
      FSet(idata->NLFlags, SWF_NList_RecurFlag);
      
      if (!FIsSet(idata->NLFlags,  SWF_NList_MasterFlag)) {
         if (idata->SWA_NList_Master)
            DoMethodA(idata->SWA_NList_Master, msg);
      } else
         for (x=0; x<SWV_NList_TrackedMax; x++)
            if (idata->SWA_NList_Tracked[x])
               DoMethodA(idata->SWA_NList_Tracked[x], msg);

      FReset(idata->NLFlags, SWF_NList_RecurFlag);
   }
#  endif

   rc = DoMethodA(idata->SWA_NList_L, msg);
   DoMethod(o, SWM_NList_SetNotify);

   SWDB_RET(rc);
}

/*****************************************************************************
 * Method: Call method on all tracked lists                                  *
 *****************************************************************************/

METHOD(SWM_NList_ListSet, SWP_NList_ListSet *)
{
   SWDB_RET(DoMethod(o, SWM_NList_ObjMethod, MUIM_Set, msg->Attr, msg->Val));
} SDFT


static void SWNListUserSet(Object *o, SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (!idata->SWA_NList_L)
      return;

   if (Tag == MUIA_List_Format)
      set(idata->SWA_NList_L, Tag, Data);

# ifdef SWV_NList_TrackedMax
   if (Tag == MUIA_List_Quiet) {
      if (FIsSet(idata->NLFlags, SWF_NList_SetRecurFlag))
         return;

      set(idata->SWA_NList_L, Tag, Data);

      FSet(idata->NLFlags, SWF_NList_SetRecurFlag);

      if (FIsSet(idata->NLFlags,  SWF_NList_MasterFlag)) {
         ULONG x;

         for (x=0; x<SWV_NList_TrackedMax; x++)
            if (idata->SWA_NList_Tracked[x])
               set(idata->SWA_NList_Tracked[x], Tag, Data);
      } else
         if (idata->SWA_NList_Master)
            set(idata->SWA_NList_Master, Tag, Data);

      FReset(idata->NLFlags, SWF_NList_SetRecurFlag);
   }
#  endif
}

# ifndef SW_USERSET
#    define SW_USERSET SWNListUserSet(o, idata, Tag, Data)
# endif

static void NList_Setup(Object *o, SW_IDATA *idata, TagItem *ti)
{
   idata->SWA_NList_Obj = o;

#  ifdef SW_NList_SetupPre
      SW_NList_SetupPre;
#  endif
   
#  ifdef SW_NList_ClickCol
      DoMethod(idata->SWA_NList_LV, MUIM_Notify, MUIA_Listview_ClickColumn,
               MUIV_EveryTime, o, 2, SWM_NList_ClickCol, MUIV_TriggerValue);
#  endif      

#  ifdef SW_NList_SelChange
      DoMethod(idata->SWA_NList_LV, MUIM_Notify, 
               MUIA_Listview_SelectChange, MUIV_EveryTime,
               o, 1, SWM_NList_SelChange);
#  endif

#  ifdef SW_NListTrackAttrNum
      DoMethod(idata->SWA_NList_L, MUIM_Notify, MUIA_List_Active,
               MUIV_EveryTime, o, 2, SWM_NList_Change, MUIV_TriggerValue);
#  endif

   NListSetupHooks(idata);

   if (GetTagUL(SWA_NList_NoNotifies, NULL, ti))
      FSet(idata->NLFlags, SWF_NList_NoNotifies);
   
#  ifdef SWV_NList_TrackedMax
   {
      ULONG x;
      Object *e, *d = GetTagOB(SWA_NList_Master, NULL, ti);
      idata->SWA_NList_Master = d;
      DoMethod(o, SWM_NList_AddTrackedList, o);
      if (idata->SWA_NList_Master) {
         SW_MUIListLoop(idata->SWA_NList_Master, x, e, TRUE)
            DoMethod(idata->SWA_NList_L, MUIM_List_InsertSingle, e,
                     MUIV_List_Insert_Bottom);
         DoMethod(o, SWM_NList_SetNotify);
      }
   }
#  endif
}

static void NList_Dispose(Object *o, SW_IDATA *idata)
{
   DoMethod(o, SWM_NList_KillNotify);

#  ifdef SW_NList_ListDispHookPre
   SW_NList_ListDispHookPre(o)
#  endif

#  ifdef SW_NList_ListDispHookPreObj
   {
      LONG x;
      Object *lo;

      SW_MUIListLoop(idata->SWA_NList_L, x, lo, TRUE)
         SW_NList_ListDispHookPreObj(o, lo);
   }
#  endif

#  ifdef SWV_NList_TrackedMax
   if (!FIsSet(idata->NLFlags,  SWF_NList_MasterFlag))
      FSet(idata->NLFlags, SWF_NList_DelRecurFlag);

   DoMethod(o, SWM_NList_RemTrackedList, o);
#  endif
}

#  define SWP_NList_SetupCust   NList_Setup((Object *)retval, idata, ti)
#  define SWP_NList_DisposeCust NList_Dispose(o, idata)


/*****************************************************************************
 * Method: Insert clone of object                                            *
 *****************************************************************************/

# ifdef SW_NList_Clone
METHOD(SWM_NList_Clone, Msg)
{
   Object *Active, *Clone;
   
   DoMethod(o, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &Active);
   if (!Active) SWDB_RET(0);

   if (!(Clone = (Object *)DoMethod(Active, SWM_Clone))) SWDB_RET(0);

   AB_SetMod(idata);

   DoMethod(o, MUIM_List_InsertSingle, Clone, MUIV_List_Insert_Active);
   SWDB_RET(0);
} SDFT
# endif


/*****************************************************************************
 * Method: toggle for clicking in a column                                   *
 *****************************************************************************/

# ifdef SW_NList_ClickCol
METHOD(SWM_NList_ClickCol, SWP_NList_ClickCol *)
{
   ULONG x, Col = msg->Column;
   
   Object *Active;
   
   DoMethod(o, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &Active);
   if (!Active) SWDB_RET(0);

#  ifdef SNLT_ListCol
     Col = DoMethod(SNLT_ListCol, SWM_ListCol_TrueCol, Col);
#  endif

#  ifdef SW_NList_ClickCol_Pre
      SW_NList_ClickCol_Pre;
#  endif
   
   for (x=0; SNLT_Attr[x]; x++)
      if (SNLT_Cols[x] == Col) {
         AB_SetMod(idata);
         set(Active, SNLT_Attr[idata->CCLastCol = x],
             idata->CCLastVal = !SWGetUL(Active, SNLT_Attr[x]));
         break;
      }

#  ifdef SW_NList_ClickCol_Post
      SW_NList_ClickCol_Post;
#  endif

   SWDB_RET(0);
} SDFT
# endif


/*****************************************************************************
 * Method: Selection change                                                  *
 *****************************************************************************/

# ifdef SW_NList_SelChange
METHOD(SWM_NList_SelChange, Msg)
{
   Object *Active;
   
   DoMethod(o, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &Active);
   if (!Active) SWDB_RET(0);

   set(Active, SNLT_Attr[idata->CCLastCol], idata->CCLastVal);

   SWDB_RET(0);
} SDFT
# endif


/*****************************************************************************
 * Method: Remove selected items                                             *
 *****************************************************************************/

METHOD(SWM_NList_RemoveSel, Msg)
{
   AB_SetMod(idata);

   SWDB_RET(DoMethod(o, MUIM_List_Remove, MUIV_List_Remove_Selected));
} SDFT


/*****************************************************************************
 * Method: sort the list                                                     *
 *****************************************************************************/
   
METHOD(MUIM_List_Sort, Msg)
{
   ULONG rc;
   Object *App = SWGetObj(o, MUIA_ApplicationObject);
   
   if (App) set(App, MUIA_Application_Sleep, TRUE);
   
   rc = DoMethodA(idata->SWA_NList_L, msg);

   DoMethod(o, SWM_NList_SetNotify);

   if (App) set(App, MUIA_Application_Sleep, FALSE);
   
   SWDB_RET(rc);
} SDFT


/*****************************************************************************
 * Method: Clear the list                                                    *
 *****************************************************************************/
   
METHOD(MUIM_List_Clear, Msg)
{
   DoMethod(o, SWM_NList_Change, MUIV_List_Active_Off);
   
   SWDB_RET(DoMethodA(idata->SWA_NList_L, msg));
} SDFT
