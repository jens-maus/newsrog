/*****************************************************************************
 * Set attributes, but not for children, only ourself                        *
 *****************************************************************************/

METHOD(SWM_NoForwardSet, ULONG *)
{
   ULONG x, stack[12];

   stack[0] = MUIA_Group_Forward;
   stack[1] = FALSE;
   
   for (x=1; msg[x]!=TAG_DONE; x+=2)
      stack[x+1] = msg[x+0],
      stack[x+2] = msg[x+1];

   stack[x+1] = TAG_DONE;

   SWDB_RET(SetAttrsA(o, (TagItem *)stack));
} SDFT
