
/*****************************************************************************
 * Function: Layout hook                                                     *
 *****************************************************************************/

static __saveds ULONG __asm LayoutFunc(a0 struct Hook *h,
                                       a2 Object *obj,
                                       a1 struct MUI_LayoutMsg *lm)
{
   putreg(REG_A4, (LONG)h->h_Data);

   // Comment this out to simplify things for now.  Eventually, we need to
   // get the right min/max size by calling:
   // DoMethod(obj, MUIM_AskMinMax, &lm->lm_MinMax);

   switch (lm->lm_Type) {
     case MUILM_MINMAX: if (lm) {
                           lm->lm_MinMax.MinWidth  = 10;
                           lm->lm_MinMax.MinHeight = 10;
                           lm->lm_MinMax.DefWidth  = 10;
                           lm->lm_MinMax.DefHeight = 10;
                           lm->lm_MinMax.MaxWidth  = 1000;
                           lm->lm_MinMax.MaxHeight = 1000;
                        }

                        return 0;

     case MUILM_LAYOUT: return TRUE;  // no children to layout, by definition
   }

   return MUILM_UNKNOWN;
}


struct Hook LayoutHook = { {0,0}, LayoutFunc, NULL, NULL };


/*****************************************************************************
 * Method: MUI Setup                                                         *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   Object *obj; APTR state; List *l;
   
   // -- set up A4 so we can call DoMethod in the layout hook without crashing.

   LayoutHook.h_Data = (void *)getreg(REG_A4);

   // -- get a count of children ---------------------------------------------

   idata->ChildCount = 0;
   get(o, MUIA_Group_ChildList, &l);
   SW_LISTLOOP(l, state, obj) idata->ChildCount++;

   // -- set custom layout hook only if ChildCount == 0 ----------------------
   
// nfset(o, MUIA_Group_LayoutHook, idata->ChildCount ? NULL : &LayoutHook);

   SWDB_RET((ULONG)DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Addmember                                                         *
 *****************************************************************************/

METHOD(OM_ADDMEMBER, opMember *)
{
   idata->ChildCount++;

   // -- if we`re adding something, we can't be empty, so clear layout hook

// nfset(o, MUIA_Group_LayoutHook, NULL); 

   SWDB_RET((ULONG)DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Method: Remmember                                                         *
 *****************************************************************************/

METHOD(OM_REMMEMBER, Msg)
{
   ULONG rc = (ULONG)DoSuperMethodA(cl, o, msg);

   --idata->ChildCount;

   // -- set custom layout hook only if ChildCount == 0 ----------------------

// nfset(o, MUIA_Group_LayoutHook, idata->ChildCount ? NULL : &LayoutHook);

   SWDB_RET(rc);
} SDFT

