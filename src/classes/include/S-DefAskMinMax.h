
/*****************************************************************************
 * Find object min/max size                                                  *
 *****************************************************************************/

# ifndef AMM_MIN
#   define AMM_MIN 10
# endif

# ifndef AMM_DEF
#   define AMM_DEF 25
# endif

# ifndef AMM_MAX
#   define AMM_MAX 4096
# endif

# ifndef AMM_MINWIDTH
#   define AMM_MINWIDTH AMM_MIN
# endif

# ifndef AMM_MINHEIGHT
#   define AMM_MINHEIGHT AMM_MIN
# endif

# ifndef AMM_MAXWIDTH
#   define AMM_MAXWIDTH AMM_MAX
# endif

# ifndef AMM_MAXHEIGHT
#   define AMM_MAXHEIGHT AMM_MAX
# endif

# ifndef AMM_DEFWIDTH
#   define AMM_DEFWIDTH AMM_DEF
# endif

# ifndef AMM_DEFHEIGHT
#   define AMM_DEFHEIGHT AMM_DEF
# endif

# define MUIM_ASKMINMAX
METHOD(MUIM_AskMinMax, struct MUIP_AskMinMax *)
{
   ULONG rc = DoSuperMethodA(cl, o, (Msg)msg);
 
   SWDB_ASSERTNULL(msg->MinMaxInfo)

#  ifdef AMM_CUST
      AMM_CUST
#  endif

#  ifdef AMM_RETURNIFCHILDREN
   {
      List *l; APTR s;
      get(o, MUIA_Group_ChildList, &l);
      s = l->lh_Head;
      if (NextObject(&s)) SWDB_RET(rc);
   }
#  endif

   msg->MinMaxInfo->MinWidth  += AMM_MINWIDTH;
   msg->MinMaxInfo->DefWidth  += AMM_DEFWIDTH;
   msg->MinMaxInfo->MaxWidth  += AMM_MAXWIDTH;
   
   msg->MinMaxInfo->MinHeight += AMM_MINHEIGHT;
   msg->MinMaxInfo->DefHeight += AMM_DEFHEIGHT;
   msg->MinMaxInfo->MaxHeight += AMM_MAXHEIGHT;
 
   SWDB_RET(rc);

} SDFT
