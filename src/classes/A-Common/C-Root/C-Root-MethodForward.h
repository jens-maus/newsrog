
typedef struct _MFNode { struct MinNode mf_Node; Object *o; } MFNode;

SignalSemaphore MF_Sem;
MinList         MF_List;

/*****************************************************************************
 * Method: register a new object on the list                                 *
 *****************************************************************************/

METHOD(SWM_Root_MFObjReg, SWP_Root_MFObjReg *)
{
   MFNode *node;
   static ULONG Init = FALSE;

   if (!Init) { InitSemaphore(&MF_Sem); Init=TRUE; NewList((List *)&MF_List); }

   ObtainSemaphore(&MF_Sem);

   if (node = AllocVec(sizeof(MFNode), MEMF_ANY)) {
      node->o = msg->o;
      AddHead((List *)&MF_List, (Node *)node);
   }

   ReleaseSemaphore(&MF_Sem);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Function: search for node on list (caller must semaphore protect)         *
 *****************************************************************************/

static MFNode *MFFind(Object *o)
{
   MinNode *node;

   for (node=MF_List.mlh_Head; node->mln_Succ; node=node->mln_Succ)
      if (((MFNode *)node)->o == o)
         return (MFNode *)node;

   return NULL;
}


/*****************************************************************************
 * Method: register a new object on the list                                 *
 *****************************************************************************/

METHOD(SWM_Root_MFObjUnReg, SWP_Root_MFObjUnReg *)
{
   MFNode *node;
   ObtainSemaphore(&MF_Sem);

   if (node = MFFind(msg->o)) {
      Remove((Node *)node);
      FreeVec(node);
   }
   
   ReleaseSemaphore(&MF_Sem);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Forward a method, ONLY if the object is on the list               *
 *****************************************************************************/

// -- msg[1] = target, msg[2] = count, msg[3+] = stack

METHOD(SWM_Root_MFForward, ULONG *)
{
   MFNode *node;

   ObtainSemaphore(&MF_Sem);
   node = MFFind((Object *)msg[1]);
   ReleaseSemaphore(&MF_Sem);

   SWDB_RET(node ? DoMethodA((Object *)msg[1], (Msg)(msg+3)) : NULL);
} SDFT
