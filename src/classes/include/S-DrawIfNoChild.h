
/*****************************************************************************
 * Draw object                                                               *
 *****************************************************************************/

METHOD(MUIM_Draw, struct MUIP_Draw *)
{
   List *l;
   
   DoSuperMethodA(cl, o, msg);       // let parent draw self

   SW_GETLH(o, l);
   if (l && IsListEmpty(l)) {             // if no children, draw blank square
      SetAPen(_rp(o), 0);

      RectFill(_rp(o), _mleft(o), _mtop(o), _mright(o), _mbottom(o));
   }

   SWDB_RET(0);
} SDFT
