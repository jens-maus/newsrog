/*****************************************************************************
 * Function: find location in lent attribute list of given attr (-1 if none) *
 *****************************************************************************/

static LONG FindAttrPos(SW_IDATA *idata, ULONG Attr)
{
   LONG x;

   for (x=0; x<idata->LentAttrCount; x++)
      if ((Attr == LA_Used && idata->LentAttrs[x].Attr) ||
          idata->LentAttrs[x].Attr == Attr)
         return x;

   return -1;
}


/*****************************************************************************
 * Method: Create lent attribute                                             *
 *****************************************************************************/

METHOD(SWM_Root_SetLentAttr, SWP_Root_LendAttr *)
{
   LONG x;
   LentAttr *l;

   // -- see if we can just update an old one --------------------------------
   
   if ((x=FindAttrPos(idata, msg->Attr)) >= 0 ||
       (x=FindAttrPos(idata, LA_Unused)) >= 0)
      goto StuffVal;

   // -- otherwise, gotta allocate more space --------------------------------

   l = AllocVec(sizeof(LentAttr)*(idata->LentAttrCount+LA_Inc),
                MEMF_ANY|MEMF_CLEAR);

   if (!l) SWDB_RET(idata->LentAttrCount = 0);

   if (idata->LentAttrs)
      memcpy(l, idata->LentAttrs, sizeof(LentAttr)*idata->LentAttrCount);

   FreeVec(idata->LentAttrs);
   idata->LentAttrs      = l;
   x                     = idata->LentAttrCount;
   idata->LentAttrCount += LA_Inc;

   StuffVal:
      idata->LentAttrs[x].Attr = msg->Attr;
      idata->LentAttrs[x].Val  = msg->Val;
      SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Free a lent attribute                                             *
 *****************************************************************************/

METHOD(SWM_Root_FreeLentAttr, SWP_Root_FreeAttr *)
{
   LONG x = FindAttrPos(idata, msg->Attr);

   if (x >= 0) {
      idata->LentAttrs[x].Attr =
      idata->LentAttrs[x].Val  = 0;

      if (FindAttrPos(idata, LA_Used) < 0) {  // no more attrs, free list.
         FreeVec(idata->LentAttrs);
         idata->LentAttrs     = NULL;
         idata->LentAttrCount = 0;
      }
   }

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Get a lent attribute value                                        *
 *****************************************************************************/

METHOD(SWM_Root_GetLentAttr, SWP_Root_GetAttr *)
{
   LONG x = FindAttrPos(idata, msg->Attr);

   SWDB_RET(x >= 0 ? idata->LentAttrs[x].Val : 0);
} SDFT

