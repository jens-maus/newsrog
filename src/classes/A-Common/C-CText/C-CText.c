# include "C-CText-I.h"
# include "S-Profile.h"

# undef  SWDB_FUNCTYPE
# undef  SWDB_CLASSTYPE
# define SWDB_FUNCTYPE   SWDB_ObjFunc
# define SWDB_CLASSTYPE  SWDB_C_CText

// -- attribute map ----------------------------------------------------------

SWAttrMapT SW_ATTRMAP[] = {
   SWAM_PT(SWA_CText_Lines,       SWAM_NoLab, SWAM_SGI, NULL),
   SWAM_IN(SWA_CText_LineCnt,     SWAM_NoLab, SWAM_SGI, 0),
   SWAM_PT(SWA_CText_MemPool,     SWAM_NoLab, SWAM_SGI, NULL),
   SWAM_PT(SWA_CText_LineSpacing, SWAM_NoLab, SWAM_SGI, 2),
   SWAM_SA(SWA_CText_DefFont,     SWAM_NoLab, SWAM_SGI, "helvetica/13", 64),
   SWAM_PA(SWA_CText_DefPen,      SWAM_NoLab, SWA_CText_PopPens,
           CTP_DefPen, MPEN_TEXT, SWAM_SGI),
   SWAM_BO(SWA_CText_Select,      SWAM_NoLab, SWAM_SGI, SWV_CText_Select_All),
   SWAM_IN(SWA_CText_TabSize,     SWAM_NoLab, SWAM_SGI, 8),
   SWAM_BO(SWA_CText_AutoClip,    SWAM_NoLab, SWAM_SGI, FALSE),

   SWP_Root_Attrs,
   SWAM_DONE
};


static void RmLines(SW_IDATA *idata)
{
   ULONG x;

   if (!idata->SWA_CText_Lines) return;

   for (x=0; CTxt(x); x++)
      FV(CTxt(x));

   FV(idata->LineBuff);
   FV(idata->SWA_CText_Lines);
   idata->SWA_CText_Lines   = NULL;
   idata->SWA_CText_LineCnt = 0;
   idata->LineBuff          = NULL;
   idata->LineBuffSize      = 0;
   idata->LineBuffLine      = -1;
}

static LONG CountNL(STRPTR s)
{
   LONG x = 1;

   while (*s)
      if (*s++ == '\n')
         x++;

   return x;
}

static STRPTR DupLine(SW_IDATA *idata, STRPTR s, Object *PrevCTxt)
{
   STRPTR d, t, ss;
   LONG  len = 1, ExtraLen = 0, Pos, InCtrl;

   for (t=s; *t; t++)
      len += (*t=='\t' ? idata->SWA_CText_TabSize : 1);

   if (PrevCTxt)
      if (ss = (STRPTR)DoMethod(PrevCTxt, SWM_CText_GetStack))
         ExtraLen = strlen(ss) + 1;

   if (!(t = d = AV(len + ExtraLen))) return NULL;

   // -- prepend previous stack junk, if previous object ---------------------

   if (PrevCTxt && ss) {
      t += strlen(strcpy(t, ss));

      DoMethod(PrevCTxt, SWM_CText_FreeStack, ss);
   }

   // -- copy string to ourselves --------------------------------------------

   for (Pos = 0, InCtrl = FALSE; *s; s++)
      if (s[0]=='\\' && s[1]=='0' && s[2]=='3' && s[3]=='3')
         *t++ = '\033', s+=3;
      else if (s[0] == '\t')
         for (len = (idata->SWA_CText_TabSize - (Pos % idata->SWA_CText_TabSize));
              len > 0; len--)
            *t++ = ' ', Pos++;
      else {
         *t++ = *s;

         if (*s == CT_Ctrl) InCtrl = TRUE;

         if (InCtrl) {
            if (*s == CT_End) InCtrl = FALSE;
         } else Pos++;
      }

   *t = '\0';

   return d;
}


static void DupLines(SW_IDATA *idata, STRPTR *Lines)
{
   ULONG  x, lc, l;
   STRPTR s, nl;

   _PROFINIT(0)
   _ProfBegin(__FUNC__);

   RmLines(idata);

   if (!Lines) return;

   // -- allocate the lines --------------------------------------------------

   for (x=l=0; Lines[x]; x++)
      l += CountNL(Lines[x]);

   _ProfCheck(__FUNC__ ": After CountNL");

   idata->SWA_CText_LineCnt = l;

   if (!(idata->SWA_CText_Lines = (STRPTR *)AV((l+1)*sizeof(STRPTR))))
      return;

   _ProfCheck(__FUNC__ ": After Alloc");

   // -- strdup them, with \033 esc support ----------------------------------

   for (x=lc=0; Lines[x]; x++) {
      s = Lines[x];

      do {
         if (nl = strchr(s, '\n')) *nl = '\0';

         CTxt(lc) = DupLine(idata, s, lc==0 ? idata->PrevCText : NULL);
         lc++;

         if (nl) *nl='\n', s = nl+1;
      } while (nl);
   }

   _ProfCheck(__FUNC__ ": After LineDup");

   CTxt(lc) = NULL;

   _ProfCheck(__FUNC__ ": End");
   _PROFTERM;
}


static ULONG DoSet(SW_IDATA *idata, ULONG Tag, ULONG Data)
{
   if (Tag == SWA_CText_PrevCText) {
      idata->PrevCText = (Object *)Data;
      return Data;
   }

   if (Tag == SWA_CText_Lines) {
      DupLines(idata, (STRPTR *)Data);
      return Data;
   }

   if (Tag == SWA_CText_Contents) {
      STRPTR Lines[2];

      Lines[0] = (STRPTR)Data;
      Lines[1] = NULL;

      DupLines(idata, Lines);

      return Data;
   }

   return NULL;
}

Library *DiskfontBase = NULL;
APTR    CTPool        = NULL;

static APTR GetCTPool(void)
{
   if (CTPool) return CTPool;

   return CTPool = SWPoolCreate(MEMF_ANY, 1024 * 1, 256, NULL);
}


# define SW_USERSETUP \
   if (!(DiskfontBase = OpenLibrary("diskfont.library", 37))) \
      return -1; \
   _ProfInit();

# define SW_USERCLEANUP CloseLibrary(DiskfontBase); \
                        SWPoolDelete(CTPool);       \
                        CTPool = NULL;              \
                        _ProfTerm();

# define DISPOSECUST    RmLines(idata);

# define SW_USERSET     if (DoSet(idata, Tag, Data)) SWDB_RET(TRUE);

# define SW_USERGET \
   if (((opGet *)msg)->opg_AttrID == SWA_CText_NumSelections) { \
      SW_IDATA *idata = INST_DATA(cl, o); \
      *((ULONG *)((opGet *)msg)->opg_Storage) = (idata->SelRegCnt+1); \
      SWDB_RET(1); \
   }
   
static ULONG Create(Object *o, SW_IDATA *idata, struct TagItem *ti)
{
   _PROFINIT(1)
   _ProfBegin(__FUNC__);

   idata->SWA_CText_MemPool = GetTagTyp(SWA_CText_MemPool, NULL, ti, void *);
   idata->LineBuffLine      = -1;
   idata->SelRegCnt         = -1;
   idata->SelRegMax         =  0;
   idata->SelReg            =  NULL;

   if (!idata->SWA_CText_MemPool)
      if (!(idata->SWA_CText_MemPool = GetCTPool()))
         return FAILURE;

   _ProfCheck(__FUNC__ ": End");
   _PROFTERM;

   return SUCCESS;
}


# define SETUPCUST \
   if (Create((Object *)retval, idata, ti) != SUCCESS) \
      SWDB_RET(NULL);

# define SW_Methods                    \
  SW_DO_MF(MUIM_AskMinMax);            \
  SW_DO_MF(MUIM_Draw);                 \
  SW_DO_MF(MUIM_Setup);                \
  SW_DO_MF(MUIM_Cleanup);              \
  SW_DO_MF(MUIM_Show);                 \
  SW_DO_MF(MUIM_Hide);                 \
  SW_DO_MF(MUIM_HandleInput);          \
  SW_DO_MF(MUIM_List_Select);          \
  SW_DO_MF(MUIM_List_GetEntry);        \
  SW_DO_MF(MUIM_List_Jump);            \
  SW_DO_MF(SWM_CText_Unsel);           \
  SW_DO_MF(SWM_CText_SelAll);          \
  SW_DO_MF(SWM_CText_GetClipStr);      \
  SW_DO_MF(SWM_CText_DragStart);       \
  SW_DO_MF(SWM_CText_DragStop);        \
  SW_DO_MF(SWM_CText_GCInt);           \
  SW_DO_MF(SWM_CText_GetGrpClip);      \
  SW_DO_MF(SWM_CText_Clip);            \
  SW_DO_MF(SWM_CText_SelRegion);       \
  SW_DO_MF(SWM_CText_GetStack);        \
  SW_DO_MF(SWM_CText_FreeStack);       \
  SW_DO_MF(SWM_CText_LineRangeHeight); \
  SW_DO_MF(SWM_ListSearch_Setup);      \
  SW_DO_MF(SWM_ListSearch_HLMatch);    \


# include "C-CText-Mthd.h"
# include "S-DefAll.h"
# include "S-Dispatcher.h"
