# include "C-DTImg-I.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_DTImg

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_SA(SWA_DTImg_FSpec,      SWAM_NoLab, SWAM_SGI,  NULL, FMSIZE),
   SWAM_SA(SWA_DTImg_Mask,       SWAM_NoLab, SWAM_SGI,  NULL, FMSIZE),
   SWAM_PT(SWA_DTImg_Path,       SWAM_NoLab, SWAM_SGI,  NULL),
   SWAM_PT(SWA_DTImg_Data,       SWAM_NoLab, SWAM_SGI,  NULL),
   SWAM_BO(SWA_DTImg_Del,        SWAM_NoLab, SWAM_SGI, FALSE),
   SWAM_IN(SWA_DTImg_Width,      SWAM_NoLab, SWAM_Get,     0),
   SWAM_IN(SWA_DTImg_Height,     SWAM_NoLab, SWAM_Get,     0),
   SWAM_IN(SWA_DTImg_Depth,      SWAM_NoLab, SWAM_Get,     0),
   SWAM_IN(SWA_DTImg_Precision,  SWAM_NoLab, SWAM_SGI, PRECISION_IMAGE),
   SWAM_IN(SWA_DTImg_MinXRes,    SWAM_NoLab, SWAM_SGI,   800),
   SWAM_BO(SWA_DTImg_Adjust,     SWAM_NoLab, SWAM_SGI,  FALSE),
   SWAM_BO(SWA_DTImg_HasGhost,   SWAM_NoLab, SWAM_SGI,  FALSE),
   SWAM_BO(SWA_DTImg_Ghosted,    SWAM_NoLab, SWAM_SGI,  FALSE),
   SWAM_PT(SWA_DTImg_ImgCache,   SWAM_NoLab, SWAM_SGI,  FALSE),
   SWAM_BO(SWA_DTImg_Sleep,      SWAM_NoLab, SWAM_SGI,  FALSE),
   
   SWP_Root_Attrs,
   SWAM_DONE
};


static ULONG Create(Object *o, SW_IDATA *idata)
{
   set(o, MUIA_FillArea, TRUE);
        
   return SUCCESS;
}

# define SETUPCUST if (Create((Object *)retval, idata) != SUCCESS) \
                       SWDB_RET(NULL);

# define DISPOSECUST                             \
   if (idata->SWA_DTImg_Del) {                   \
      if (!SW_STRISNULL(idata->SWA_DTImg_FSpec)) \
         DeleteFile(idata->SWA_DTImg_FSpec);     \
      if (!SW_STRISNULL(idata->SWA_DTImg_Mask))  \
         DeleteFile(idata->SWA_DTImg_Mask);      \
   }

# define SW_USERSET                                            \
  if (Tag == SWA_DTImg_Ghosted && idata->SWA_DTImg_HasGhost) { \
     idata->SWA_DTImg_Ghosted = Data;                          \
     MUI_Redraw(o, MADF_DRAWOBJECT);                           \
  }

# define SW_Methods                 \
  SW_DO_MF(SWM_DTImg_MakeImgCache); \
  SW_DO_MF(SWM_DTImg_FreeImgCache); \
  SW_DO_MF(MUIM_AskMinMax);         \
  SW_DO_MF(MUIM_Draw);              \
  SW_DO_MF(MUIM_Setup);             \
  SW_DO_MF(MUIM_Cleanup);           \

# include "C-DTImg-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
