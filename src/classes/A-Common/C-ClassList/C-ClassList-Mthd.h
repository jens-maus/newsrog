
# define MAXINLINE 200

/*****************************************************************************
 * Function: copy single component                                           *
 *****************************************************************************/

static STRPTR CopyComponent(STRPTR inline, STRPTR target)
{
   while (*inline && *inline != ':') *target++ = *inline++;
   return ++inline;
}

/*****************************************************************************
 * Function: Load an individual reg file                                     *
 *****************************************************************************/

static void LoadRegInfoInt(SW_IDATA *idata, STRPTR Path)
{
   UBYTE   InLine[MAXINLINE], *in;
   BPTR    fp;
   ClassInfo *ci;

   if (!(fp = Open(Path, MODE_OLDFILE))) return;

   while (FGets(fp, InLine, MAXINLINE-1)) {
      if (!InLine[0] || InLine[0] == '\n') continue;

      if (!(ci = AllocVec(sizeof(ClassInfo), MEMF_ANY | MEMF_CLEAR)))
         goto Done;

      in = InLine;
      in = CopyComponent(in, ci->ClassName);
      in = CopyComponent(in, ci->ClassLib);
      in = CopyComponent(in, ci->ClassDesc);
           CopyComponent(in, ci->ClassType);

      AddTail(&idata->ClassList, (struct Node *)&ci->node);
   }

  Done:
   Close(fp);
}


/*****************************************************************************
 * Build class list from registration files                                  *
 *****************************************************************************/

static void BuildList(SW_IDATA *idata, STRPTR Path)
{
   struct FileInfoBlock *fib;
   BPTR                  lock = NULL;
   UBYTE                 FilePath[FMSIZE];

   NewList(&idata->ClassList);

   if (!(fib=(void *)AllocVec(sizeof(*fib), MEMF_ANY|MEMF_CLEAR))) goto Done;

   if (!(lock = Lock(Path, ACCESS_READ))) goto Done;   // lock the path
   if (!Examine(lock, fib))               goto Done;   // examine lock
   if (fib->fib_DirEntryType < 0)         goto Done;   // need a dir not a file

   while (ExNext(lock, fib)) {
      AddPart(strcpy(FilePath, Path), fib->fib_FileName, FMSIZE-1);

      if (fib->fib_DirEntryType < 0)                // load file info
         LoadRegInfoInt(idata, FilePath);
   }

  Done:
   FreeVec(fib);  // NULL is ok
   UnLock(lock);  // NULL is ok
}


/*****************************************************************************
 * Display hook for ClassLists                                               *
 *****************************************************************************/

static __saveds __asm LONG SW_CList_DspFn(a2 char **array, a1 ClassInfo *ci,
                                          a0 struct Hook *hook)
{
   putreg(REG_A4, (ULONG)hook->h_Data);  // use proper A4

   if (ci)
      *array++ = (STRPTR)ci->ClassName,
      *array   = (STRPTR)ci->ClassDesc;
   else
      *array++ = GetS(SWS_Name),
      *array   = GetS(SWS_Desc);
   
   return 0;
}

static struct Hook SW_CList_DspFnHook = {
        {NULL, NULL}, (void *)SW_CList_DspFn, NULL, NULL };


/*****************************************************************************
 * Sort hook for ClassLists                                                  *
 *****************************************************************************/

static __saveds __asm LONG SW_CList_CmpFn(a1 ClassInfo *c1, a2 ClassInfo *c2)
{
   return strcmp((STRPTR)c1->ClassName, (STRPTR)c2->ClassName);
}

static struct Hook SW_CList_CmpFnHook = {
        {NULL, NULL}, (void *)SW_CList_CmpFn, NULL, NULL };


/*****************************************************************************
 * Return an object of the selected class                                    *
 *****************************************************************************/

METHOD(SWM_ClassList_MakeObj, Msg)
{
   SWDB_RET(DoMethod(o, SWM_ClassList_MakeObjNew, SWV_ClassList_MakeObj_Active));
} SDFT


/*****************************************************************************
 * Return an object of the selected class                                    *
 *****************************************************************************/

METHOD(SWM_ClassList_MakeObjNew, ULONG *)
{
   ClassInfo *ci;
   LONG Pos = GetTagL(SWT_ClassList_MakeObj_Pos, SWV_ClassList_MakeObj_Active,
                      msg+1);
   
   DoMethod(o, MUIM_List_GetEntry,
            (Pos == SWV_ClassList_MakeObj_Active)
                ? MUIV_List_GetEntry_Active : Pos,
            &ci);

   if (!ci) SWDB_RET(NULL);

   SWDB_RET(MUI_NewObject(ci->ClassLib, TAG_DONE));
} SDFT


/*****************************************************************************
 * Return an object of the selected class                                    *
 *****************************************************************************/

METHOD(SWM_ClassList_GetAttr, SWP_ClassList_GetAttr *)
{
   ClassInfo *ci;

   DoMethod(o, MUIM_List_GetEntry, msg->Pos, &ci);

   if (!ci) SWDB_RET(NULL);

   switch (msg->Attr) {
     case SWV_ClassList_Name: SWDB_RET(ci->ClassName);
     case SWV_ClassList_Lib:  SWDB_RET(ci->ClassLib);
     default:                 SWDB_RET(NULL);
   } 

} SDFT


/*****************************************************************************
 * Function: load RegDB                                                      *
 *****************************************************************************/

static void LoadRegDB(Object *o, SW_IDATA *idata)
{
   UBYTE Patt[80];
   if (idata->Setup) return;

   BuildList(idata, idata->Path);

   // -- insert all matching classes into the list ---------------------------

   if (!IsListEmpty(&idata->ClassList)) {
      ClassInfo *ci = (ClassInfo *)idata->ClassList.lh_Head;

      if (ParsePattern(idata->Type, Patt, 63) >= 0)
         while (ci->node.mln_Succ) {
            if (MatchPattern(Patt, ci->ClassType))
                DoMethod(o,MUIM_List_InsertSingle,ci,MUIV_List_Insert_Sorted);
            ci = (ClassInfo *)ci->node.mln_Succ;
         }
   }

   idata->Setup = TRUE;
}


/*****************************************************************************
 * Method: Setup                                                             *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   if (!DoSuperMethodA(cl, o, msg)) SWDB_RET(FALSE);

   LoadRegDB(o, idata);

   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 * Method: Get info about some class                                         *
 *****************************************************************************/

METHOD(SWM_ClassList_InfoLib, SWP_ClassList_InfoLib *)
{
   ClassInfo *ci;
   
   if (!msg->LibName) SWDB_RET(NULL);

   LoadRegDB(o, idata);

   ci = (ClassInfo *)idata->ClassList.lh_Head;
   
   while (ci && ci->node.mln_Succ) {
      if (strcmp(ci->ClassLib, msg->LibName) == 0)
         switch (msg->Info) {
           case SWV_ClassList_Info_Name: SWDB_RET(ci->ClassName);
           case SWV_ClassList_Info_Desc: SWDB_RET(ci->ClassDesc);
           case SWV_ClassList_Info_Lib:  SWDB_RET(ci->ClassLib);
           case SWV_ClassList_Info_Type: SWDB_RET(ci->ClassType);
           default:                      SWDB_RET(NULL);
         }

      ci = (ClassInfo *)ci->node.mln_Succ;
   }
   
   SWDB_RET(NULL);
} SDFT


/*****************************************************************************
 * Method: Get info about some class                                         *
 *****************************************************************************/

METHOD(SWM_ClassList_InfoObj, SWP_ClassList_InfoObj *)
{
   if (!msg->Obj) SWDB_RET(NULL);
   
   SWDB_RET(DoMethod(o, SWM_ClassList_InfoLib,
                     SWGetStr(msg->Obj, SWA_ObjClassLib),
                     msg->Info));
} SDFT


/*****************************************************************************
 * Method: Get info about some class                                         *
 *****************************************************************************/

METHOD(SWM_ClassList_GoodType, SWP_ClassList_GoodType *)
{
   UBYTE Patt[80];

   if (SW_STRISNULL(msg->Type)) SWDB_RET(FALSE);

   if (ParsePattern(idata->Type, Patt, 80) < 0)
      SWDB_RET(FALSE);

   SWDB_RET(MatchPattern(Patt, msg->Type));
} SDFT


/*****************************************************************************
 * Method: Get info about some class                                         *
 *****************************************************************************/

METHOD(SWM_ClassList_GoodTypeObj, SWP_ClassList_GoodTypeObj *)
{
   if (!msg->Obj) SWDB_RET(FALSE);

   SWDB_RET(DoMethod(o, SWM_ClassList_GoodType,
                     DoMethod(o, SWM_ClassList_InfoObj,
                              msg->Obj, SWV_ClassList_Info_Type)));
} SDFT
