# include "C-ObjectList-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_ObjectList


// -- attribute map ----------------------------------------------------------

static SWAttrMapT SW_ATTRMAP[] = {
   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   MUIX_B MUIX_U "Object Name",
   MUIX_B MUIX_U "Class Name",
   MUIX_B MUIX_U "Class Description",

   "Unable to find class: 0x%lx",
   "Object allocation failed",

   "Object hierarchies can be edited with this tree editor.\n"
      "Double click on nodes to open or close.  The open/close\n"
      "state is retained after editing.  Drag & drop to move\n"
      "objects around in the tree.  Objects can be dragged out of\n"
      "or into the tree.",
   
   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;

# include "S-DefGet.h"
# include "C-ObjectList-Mthd.h"


/*****************************************************************************
 *   Set up the object's instance data                                       *
 *****************************************************************************/


__asm __saveds LONG ocfunc(a1 struct MUIS_Listtree_TreeNode *tn)
{
   // -- save open/closed state each time it changes -------------------------
   nfset(tn->tn_User,SWA_Invocation_ListOpen,(tn->tn_Flags&TNF_OPEN)?TRUE:FALSE);
   return 0;
}


#define SW_SETUP
static __asm __inline ULONG SWSetup(SETUPFNPROTO)
   SDST(ULONG)
   SWP_XX(o, idata)
   SWDB_ASSERTNULL(o)
   SWDB_ASSERTNULL(idata)
{
   static struct Hook ochook = { {NULL, NULL}, (void *)ocfunc, NULL, NULL };

   memset(idata, 0, sizeof(SW_IDATA));           // clear our data space

   SetAttrs((Object *)retval,
            MUIA_Listtree_Format,            "MAW=100 W=50,MAW=100 W=200",
            MUIA_Listtree_DisplayHook,       &SW_OList_DspHook,
            MUIA_Listtree_Title,             TRUE,
            MUIA_Listtree_DuplicateNodeName, FALSE,
            MUIA_Listtree_OpenHook,          &ochook,
            MUIA_Listtree_CloseHook,         &ochook,
//          MUIA_Listtree_ConstructHook,     &cshook,
//          MUIA_Listtree_DestructHook,      &SW_OList_DstHook,
            MUIA_ShortHelp,                  GetS(SWS_H_OList),
            TAG_DONE);

   SW_OList_DspHook.h_Data = (void *)getreg(REG_A4);

   SWDB_RET(retval);
} SDFT


/*****************************************************************************
 *   Dispose of the object's instance data                                   *
 *****************************************************************************/

#define SW_DISPOSE
static __asm __inline void SWDispose(DISPOSEFNPROTO)
   SDSV
   SWP_XX(o, idata)
   SWDB_ASSERTNULL(o)
   SWDB_ASSERTNULL(idata)
{
   if (idata->OGroup)
      DoMethod(idata->OGroup, SWM_Invocation_RemFromListTree);
} SDFV


/*****************************************************************************
 *   Handle the methods this class understands.                              *
 *****************************************************************************/

#define SW_METHOD
static __asm __inline ULONG SWMethod(METHODFNPROTO)
   SDST(ULONG)
   SWP_XXX(cl, o, msg)
   SWDB_ASSERTCLFN()
{
   SW_IDATA *idata = INST_DATA(cl, o);

   switch (msg->MethodID) {
      SW_DO_MF(SWM_OList_Refresh);         // rebuild our tree from objects
      SW_DO_MF(SWM_OList_InsertClass);     // insert new object, given class
      SW_DO_MF(SWM_OList_InsertObject);    // insert new object, given class
      SW_DO_MF(SWM_OList_ActiveObj);       // return active object or NULL
      SW_DO_MF(SWM_OList_ObjClone);        // clone active object
      SW_DO_MF(SWM_OList_ObjEdit);         // edit active object
      SW_DO_MF(SWM_OList_ObjRem);          // remove active object
      SW_DO_MF(SWM_OList_ObjIns);          // insert obj from disk
      SW_DO_MF(SWM_OList_ObjSave);         // save active obj to disk
      SW_DO_MF(SWM_OList_ObjReset);        // reset active obj to dflt
      SW_DO_MF(SWM_Root_GetDragObj);       // get drag object
      SW_DO_MF(MUIM_DragQuery);            // drag query request
      SW_DO_MF(MUIM_DragDrop);             // drag drop report
  }

  SuperMethod:
   SWDB_RET((ULONG)DoSuperMethodA(cl, o, msg));
} SDFT


# include "S-Dispatcher.h"
