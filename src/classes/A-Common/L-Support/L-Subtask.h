
# include <dos/dostags.h>

# define sa __saveds __asm

/*****************************************************************************
 * Send message to subtask's message port                                    *
 *****************************************************************************/

sa LONG SWSendSubTaskMsg(a0 struct SubTask *st,d0 WORD command,d1 APTR params)
{
   if (!st) return 0;

   st->st_Message.stm_Message.mn_ReplyPort = st->st_Reply;
   st->st_Message.stm_Message.mn_Length    = sizeof(struct SubTaskMsg);
   st->st_Message.stm_Command              = command;
   st->st_Message.stm_Parameter            = params;
   st->st_Message.stm_Result               = 0;

   PutMsg(command==STC_Startup ? &((struct Process *)st->st_Task)->pr_MsgPort :
          st->st_Port,(struct Message *)&st->st_Message);
 
   WaitPort(st->st_Reply);
   GetMsg(st->st_Reply);

   return st->st_Message.stm_Result;
}


/*****************************************************************************
 * Create message port, using a per-process shared sigbit                    *
 *****************************************************************************/

# define MAXTASK 10
static UBYTE MP_SigBits[MAXTASK], MP_UseCounts[MAXTASK];
static struct Task *MP_Task[MAXTASK] = {0};

struct MsgPort *SWCreateMsgPort(void)
{
   ULONG x;
   struct MsgPort *mp=AllocVec(sizeof(struct MsgPort),MEMF_PUBLIC|MEMF_CLEAR);

   if (!mp) goto Error;

   NewList(&mp->mp_MsgList);
   mp->mp_SigTask = FindTask(0);

   for (x=0; x<MAXTASK; x++) if (mp->mp_SigTask == MP_Task[x]) break;

   if (x < MAXTASK) {
      // -- here, we found this task already in the task list ----------------
      mp->mp_SigBit = MP_SigBits[x];
      MP_UseCounts[x]++;
   } else {
      // -- otherwise, we need to allocate a new sigbit ----------------------
      for (x=0; x<MAXTASK; x++) if (!MP_Task[x]) break;
      if (x >= MAXTASK) goto Error;

      MP_SigBits[x]   = mp->mp_SigBit = AllocSignal(-1);
      MP_Task[x]      = mp->mp_SigTask;
      MP_UseCounts[x] = 1;
   }

   return mp;

  Error:
    FreeVec(mp);
    return NULL;
}


/*****************************************************************************
 * Delete a message port created with SWCreateMsgPort                        *
 *****************************************************************************/

void SWDeleteMsgPort(struct MsgPort *mp)
{
   ULONG x;
   
   if (!mp) return;

   for (x=0; x<MAXTASK; x++) if (mp->mp_SigTask == MP_Task[x]) break;

   SWDB_ASSERT(x >= MAXTASK, "Task not found in task list");

   if (!--MP_UseCounts[x]) {
      FreeSignal(MP_SigBits[x]);
      MP_Task[x] = NULL;
   }
   
   FreeVec(mp);
}


/*****************************************************************************
 * Spawn a new subtask                                                       *
 *****************************************************************************/

sa struct SubTask *SWSpawnSubTask(a0 UBYTE *name, a1 VOID (*func)(VOID),
                                  d0 APTR data, d1 ULONG pri)
{
   struct SubTask *st;

   if (!(st = AllocVec(sizeof(struct SubTask),MEMF_PUBLIC|MEMF_CLEAR)))
      goto Error;
   
   if (!(st->st_Reply = SWCreateMsgPort())) goto Error;

   st->st_Data = data;
               
   if (!(st->st_Task = (struct Task *)
         CreateNewProcTags(NP_Entry, func, NP_Name, name, NP_CopyVars, FALSE,
                           TAG_DONE)))
      goto Error;

   if (SWSendSubTaskMsg(st,STC_Startup,st)) {
      SetTaskPri(st->st_Task,  pri);
      return st;
   }

 Error:
   if (st && st->st_Reply) SWDeleteMsgPort(st->st_Reply);
   if (st)                 FreeVec(st);
   return NULL;
}


/*****************************************************************************
 * Kill a subtask                                                            *
 *****************************************************************************/

sa VOID SWKillSubTask(a0 struct SubTask *st)
{
   if (!st) return;
   SetTaskPri(st->st_Task, 2);           // bump its priority
   SWSendSubTaskMsg(st,STC_Shutdown,st); // ... and kill it
   SWDeleteMsgPort(st->st_Reply);
   FreeVec(st);
}


/*****************************************************************************
 * Exit from subtask and clean up                                            *
 *****************************************************************************/

sa VOID SWExitSubTask(a0 struct SubTask *st, a1 struct SubTaskMsg *stm,
                      a2 struct Library *lib)
{
   if (lib) CloseLibrary(lib);
   
   if (st->st_Port) DeleteMsgPort(st->st_Port);

   Forbid();
   stm->stm_Result = FALSE;
   ReplyMsg((struct Message *)stm);
}


/*****************************************************************************
 * Init subtask message port etc                                             *
 *****************************************************************************/

sa struct SubTask *SWInitSubTask(void)
{
   struct Task *me = FindTask(NULL);
   struct SubTask *st;
   struct SubTaskMsg *stm;

   // -- wait for startup message --------------------------------------------

   WaitPort(&((struct Process *)me)->pr_MsgPort);
   stm  = (struct SubTaskMsg *)GetMsg(&((struct Process *)me)->pr_MsgPort);
   st   = (struct SubTask *)stm->stm_Parameter;

   if (st->st_Port = CreateMsgPort()) {
      stm->stm_Result = TRUE;
      ReplyMsg((struct Message *)stm);
      return st;
   }
   
   SWExitSubTask(st, stm, NULL);
   return NULL;
}

# undef sa
