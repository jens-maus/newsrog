/*****************************************************************************
 * Build temp stack & fix up special values                                  *
 *****************************************************************************/

static ULONG DM_FixElem(Object *o, Object *entry, ULONG pos, ULONG e)
{
   // --- replace special values for one element -------
   switch (e) {
     case SWV_ListDoMethod_App:   return (ULONG)_app(o);
     case SWV_ListDoMethod_Entry: return (ULONG)entry;
     case SWV_ListDoMethod_Win:   return (ULONG)_win(o);
     case SWV_ListDoMethod_Pos:   return (ULONG)pos;
     default:                     return e;
   }
}


static Msg DM_FixStack(Object *o, Object *entry, ULONG pos,
                       ULONG *stack, ULONG *msg)
{
   ULONG x;

   // --- Loop through stack, replacing special values -----
   // msg[4] = stack count, msg[5] = first stack value for method

   for (x=0; x<msg[4]; x++)
      stack[x] = DM_FixElem(o, entry, pos, msg[x+5]);

   return (Msg)stack;
}


/*****************************************************************************
 * Function: Fix up target & stack values, & call the method                 *
 *****************************************************************************/

static ULONG DM_DM(Object *o, Object *entry, ULONG pos,
                   ULONG *msg, ULONG *stack)
{
   return DoMethodA((Object *)DM_FixElem(o, entry, 0, msg[3]),
                    DM_FixStack(o, entry, pos, stack, msg));
}


/*****************************************************************************
 * Function: iterate through selected entries in list                        *
 *****************************************************************************/

static ULONG DM_Selected(Object *o, Object *list, ULONG *msg, ULONG *stack)
{
   ULONG x; Object *entry;

   for (x = MUIV_List_NextSelected_Start;;) {
      DoMethod(list, MUIM_List_NextSelected, &x);
      if (x == MUIV_List_NextSelected_End) break;
      DoMethod(list, MUIM_List_GetEntry, x, &entry);
      if (!entry) break;
      DM_DM(o, entry, x, msg, stack);
   }

   return 0;
}


/*****************************************************************************
 * Function: iterate through all entries in list                             *
 *****************************************************************************/

static ULONG DM_All(Object *o, Object *list, ULONG *msg, ULONG *stack)
{
   ULONG x; Object *entry;

   for (x=0;; x++) {
      DoMethod(list, MUIM_List_GetEntry, x, &entry);
      if (!entry) break;
      DM_DM(o, entry, x, msg, stack);
   }

   return 0;
}


/*****************************************************************************
 * Function: iterate (sic) through active list entry                         *
 *****************************************************************************/

static ULONG DM_Active(Object *o, Object *list, ULONG *msg, ULONG *stack)
{
   Object *entry;
   LONG  Active = SWGetLong(list, MUIA_List_Active);

   if (Active == MUIV_List_Active_Off) return 0;
   
   DoMethod(list, MUIM_List_GetEntry, Active, &entry);

   if (!entry) return 0;
   
   return DM_DM(o, entry, Active, msg, stack);
}


static ULONG DM(Object *o, Object *list, ULONG *msg)
{
   ULONG stack[16];

   switch (msg[2]) {
     case SWV_ListDoMethod_Active:   return DM_Active  (o, list, msg, stack);
     case SWV_ListDoMethod_Selected: return DM_Selected(o, list, msg, stack);
     case SWV_ListDoMethod_All:      return DM_All     (o, list, msg, stack);
     default: return 0;
   }
}


/*****************************************************************************
 * Method: pass method to all in list                                        *
 *****************************************************************************/

METHOD(SWM_Root_ListDoMethod, ULONG *)
{

   // msg[1] = list object (or NULL for this object)
   // msg[2] = type (active/selected/etc),
   // msg[3] = target,
   // msg[4] = stack count
   // msg[5] = 1st elem of stack
   
   SWDB_RET(DM(o, (Object *)msg[1], msg));
} SDFT
