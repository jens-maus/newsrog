// -- To faciliate CreateImage/DeleteImage -----------------------------------

# define SWP_Img_AttrDefs \
  struct {                \
     UBYTE   Img[16];     \
     Object  *I;          \
     Object  *O;          \
  } SWImages[SWV_Img_MAX]


# define SWImg(i, n) ((i)->SWImages[n].Img)

# define SWMakeImg(i, n, img) if (!((i)->SWImages[n].O=(img))) SWDB_RET(FALSE);

# define SWCreateImagesObj(o,i) {                                              \
     ULONG x;                                                                  \
     for (x=0; x<SWV_Img_MAX; x++) {                                           \
        if (!((i)->SWImages[x].I=(Object *)DoMethod((o),MUIM_List_CreateImage, \
                                                    (i)->SWImages[x].O, 0)))   \
           SWDB_RET(FALSE);                                                    \
        sprintf((i)->SWImages[x].Img, "\33O[%08lx]", (i)->SWImages[x].I);      \
     }                                                                         \
  }

# define SWCreateImages(i) SWCreateImagesObj(o,i)

# define SWDeleteImagesObj(o,i) {                                     \
     ULONG x;                                                         \
     for (x=0; x<SWV_Img_MAX; x++) {                                  \
         if ((i)->SWImages[x].I)                                      \
            DoMethod((o), MUIM_List_DeleteImage, (i)->SWImages[x].I); \
         MUI_DisposeObject((i)->SWImages[x].O);                       \
        (i)->SWImages[x].I = (i)->SWImages[x].O = NULL;               \
     }                                                                \
  }

# define SWDeleteImages(i) SWDeleteImagesObj(o,i)

# define SWListImg_Methods \
  SW_DO_M(MUIM_List_CreateImage, SWM_NList_ListMethod_F); \
  SW_DO_M(MUIM_List_DeleteImage, SWM_NList_ListMethod_F)
