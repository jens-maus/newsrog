# include "C-Objectbox-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_Objectbox

// -- attribute map ----------------------------------------------------------

static SWAttrMapT SW_ATTRMAP[] = {
   SWAM_PT(SWA_OBox_EditObj,     SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_PT(SWA_OBox_ContentObj,  SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_IN(SWA_OBox_ContentAttr, SWAM_NoLab, SWAM_SGIN,  NULL),
   SWAM_BO(SWA_OBox_VirtGroup,   SWAM_NoLab, SWAM_SGIN, FALSE),

   SWAM_MU(MUIA_Group_Horiz, TRUE),

   SWP_Root_Attrs,
   SWAM_DONE
};


// -- catalog strings --------------------------------------------------------

STRPTR SW_CATSTR[] = {
   "Edit...",
   "Save...",
   "Load...",

   "Objects can be dropped on this box from elsewhere to\n"
      "copy a clone of that object into the box.  Objects in\n"
      "this box can be dragged to other places (such as other\n"
      "object boxes, an object tree editor, or a string gadget.",

   "Bring up an editor for the object in the box.",
   "Save the contents of the box to a file.",
   "Load an object from a file into the box.",

   NULL,
};

STRPTR   SW_MSGSTR[SWS_LASTSTR+1];
Catalog *SW_CATALOG;


# define  SETUPCUST   SetAttrs((Object *)retval, ImageButtonFrame, TAG_DONE);

# include "C-Objectbox-Mthd.h"
# include "S-DefSetup.h"
# include "S-DefDispose.h"
# include "S-DefGet.h"
# include "S-DefSet.h"


/*****************************************************************************
 *   Handle the methods this class understands.                              *
 *****************************************************************************/

#define SW_METHOD
static __asm __inline ULONG SWMethod(METHODFNPROTO)
   SDST(ULONG)
   SWP_XXX(cl, o, msg)
   SWDB_ASSERTCLFN()
{
   ULONG x;
   SW_IDATA *idata = INST_DATA(cl, o);

   switch (msg->MethodID) {
      SW_DO_MF(SWM_OBox_InsertClass);
      SW_DO_MF(SWM_OBox_InsertObject);
      SW_DO_MF(SWM_OBox_UpdateObject);
      SW_DO_MF(SWM_OBox_Editor);
      SW_DO_MF(SWM_OBox_Edit);
      SW_DO_MF(SWM_OBox_Save);
      SW_DO_MF(SWM_OBox_Load);
      SW_DO_MF(SWM_OBox_MethodForward);
      SW_DO_MF(SWM_Root_GetDragObj);
      SW_DO_MF(MUIM_Cleanup);              // when we`re hidden
      SW_DO_MF(MUIM_AskMinMax);            // ask min max size
      SW_DO_MF(MUIM_Draw);                 // draw object
      SW_DO_MF(MUIM_DragQuery);            // drag query request
      SW_DO_MF(MUIM_DragDrop);             // drag drop report
   }

   // -- handle forward methods ---------------------------------------------

   for (x=0; x<MAXMTHF; x++)  // move this stuff to root class?  Its useful.
      if (idata->Method[x] == msg->MethodID)
         SWDB_RET((ULONG)DoMethodA(idata->Target[x], msg));

  SuperMethod:
   SWDB_RET((ULONG)DoSuperMethodA(cl, o, msg));
} SDFT


# include "S-Dispatcher.h"
