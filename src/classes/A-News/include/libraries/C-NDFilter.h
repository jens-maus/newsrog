# ifndef C_NDFILTER_INCLUDE
#  define C_NDFILTER_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-Root.h"
#  include "Libraries/C-NArticle.h"
#  include "Libraries/C-NSFilter.h"
#  include "Libraries/C-CText.h"
#  include "S-SysCmdVars.h"
#  include <diskfont/diskfont.h>

#  define SW_FIRSTTAG         (int)(TAG_USER + 21000)

   enum {
      SWA_NDFilter_Enabled    = SWA_NSFilter_Enabled,
      SWA_NDFilter_Desc       = SWA_NSFilter_Desc,
      SWA_NDFilter_Groups     = SWA_NSFilter_Groups,
      SWA_NDFilter_AuthNote   = SWA_NSFilter_AuthNote,
      SWA_NDFilter_UseOnOffBt = SWA_NSFilter_UseOnOffBt,
      SWA_NDFilter_OnOffBtImg = SWA_NSFilter_OnOffBtImg,
      SWA_NDFilter_Pri        = SW_FIRSTTAG,
      SWA_NDFilter_MemPool,
      SWA_NDFilter_HasTxtMod,                      // [ G ] can modify text-only?
      SWA_NDFilter_T_Print,                        // [SGI] use for printing
      SWA_NDFilter_T_Save,                         // [SGI] use for saving text
      SWA_NDFilter_T_NetReply,                     // [SGI] use for net replies
      SWA_NDFilter_T_MailReply,                    // [SGI] use for mail replies
      SWA_NDFilter_T_RAW,                          // [ G ] always FALSE
   }; 
    
   enum { 
      SWM_NDFilter_UI         = SWM_NSFilter_UI,
      SWM_NDFilter_ShowUI     = SWM_NSFilter_ShowUI,
      SWM_NDFilter_HideUI     = SWM_NSFilter_HideUI,
      SWM_NDFilter_AddFFIcons = SWM_NSFilter_AddFFIcons,
      SWM_NDFilter_RemFFIcon  = SWM_NSFilter_RemFFIcon,
      SWM_NDFilter_FF_OnOff   = SWM_NSFilter_FF_OnOff,
      SWM_NDFilter_TBUI       = SWM_NSFilter_TBUI,
      SWM_NDFilter_ModTxt     = SW_FIRSTTAG,       // modify txt buffer
      SWM_NDFilter_TBAppendTxt,                    // TxtBuff: append text
      SWM_NDFilter_TBAppend,                       // TxtBuff: append TxtBuff
      SWM_NDFilter_TBSet,                          // TxtBuff: set value
      SWM_NDFilter_TBClear,                        // TxtBuff: clear contents
      SWM_NDFilter_TBDispose,                      // TxtBuff: free contents
      SWM_NDFilter_TBInit,                         // TxtBuff: initialize
      SWM_NDFilter_TBPrependTxt,                   // TxtBuff: prepend text
      SWM_NDFilter_TBAssign,                       // TxtBuff: asgn to another
      SWM_NDFilter_StartMatch,                     // start looking for match
      SWM_NDFilter_EndMatch,                       // end looking for match
      SWM_NDFilter_StyleOn,                        // turn style on
      SWM_NDFilter_StyleOff,                       // turn style off
      SWM_NDFilter_StyleTxt,                       // add style text
      SWM_NDFilter_TBGetSpace,                     // TxtBuff: get space
      SWM_NDFilter_TBAppendChar,                   // TxtBuff: append character
      SWM_NDFilter_TBIsEncaps,                     // TxtBuff: is encapsulated?
      SWM_NDFilter_TBAppendSub,                    // TxtBuff: append substring
      SWM_NDFilter_TBTruncate,                     // TxtBuff: truncate to length
      SWM_TextHide_Toggle,                         // toggle displayed object
      SWM_NDFilter_TBNoCtrl,                       // TxtBuff: region has no ctrl
      SWM_NDFilter_TBSkipHdrs,                     // TxtBuff: ptr to 1st non-hdr
      SWM_NDFilter_RemAddObj,                      // remove added object
      SWM_NDFilter_GenAddObj,                      // generate added object
      SWM_NDFilter_LastParseLine,                  // last line to parse
      SWM_NDFilter_TBMultiInit,                    // TxtBuff: multi init
      SWM_NDFilter_TBMultiDispose,                 // TxtBuff: multi dispose
      SWM_NDFilter_TBAssignDel,                    // TxtBuff: asgn, delete orig
      SWM_NDFilter_ModTxtNoObjs,                   // text only filtering
      SWM_NDFilter_TBDelSub,                       // TxtBuff: delete substring
      SWM_NDFilter_TBCountChar,                    // TxtBuff: count characters
      SWM_NDFilter_TBAddPrefix,                    // TxtBuff: add line prefix
   };

#  define TxBuff(t)    ((t)->TB_Buff)
#  define TxSize(t)    ((t)->TB_Size)
#  define TxLen(t)     ((t)->TB_Len)
#  define TxIsEmpty(t) (TxLen(t)==0)

   typedef struct _txtbuff {
      STRPTR      TB_Buff;
      ULONG       TB_Size;
      ULONG       TB_Len;
   } TxtBuff;

   SWM_P(SWP_NDFilter_TBUI,        Object *Grp);

   SWM_P(SWP_NDFilter_FF_OnOff,    Object *Grp; ULONG State; Object *NSFList);

   SWM_P(SWP_NDFilter_AddFFIcons,
         Object *Grp;
         Object *NSFList);

   SWM_P(SWP_NDFilter_RemFFIcon,
         Object *Grp;
         Object *NSFList;
         Object *Icon);

   SWM_P(SWP_NDFilter_ShowUI,       Object *App);

   SWM_P(SWP_NDFilter_RemAddObj,    Object *Data; Object *ArtView; ULONG FltData);
   SWM_P(SWP_NDFilter_GenAddObj,    Object *Data; Object *ArtView;
                                    Object *Grp;  Object *Art; ULONG FltData);

   SWM_P(SWP_NDFilter_Style,
         TxtBuff *Tx;
         STRPTR  Font;
         ULONG   B;
         ULONG   U;
         ULONG   I;
         ULONG   UseColor;
         struct  MUI_PenSpec *Color);

   SWM_P(SWP_NDFilter_StyleTxt,
         TxtBuff *Tx;
         STRPTR  Font;
         ULONG   B;
         ULONG   U;
         ULONG   I;
         ULONG   UseColor;
         struct  MUI_PenSpec *Color;
         STRPTR  Text);

   SWM_P(SWP_NDFilter_TBAppend,     TxtBuff *Tx1; TxtBuff *Tx2);
   SWM_P(SWP_NDFilter_TBAppendSub,  TxtBuff *Tx1; STRPTR s; ULONG p1; ULONG p2);
   SWM_P(SWP_NDFilter_TBAppendTxt,  TxtBuff *Tx;  STRPTR  s);
   SWM_P(SWP_NDFilter_TBPrependTxt, TxtBuff *Tx;  STRPTR  s);
   SWM_P(SWP_NDFilter_TBSet,        TxtBuff *Tx;  STRPTR  s);
   SWM_P(SWP_NDFilter_TBClear,      TxtBuff *Tx);
   SWM_P(SWP_NDFilter_TBDispose,    TxtBuff *Tx);
   SWM_P(SWP_NDFilter_TBInit,       TxtBuff *Tx);
   SWM_P(SWP_NDFilter_TBSkipHdrs,   TxtBuff *Tx;  ULONG HasHdrs);
   SWM_P(SWP_NDFilter_TBAssign,     TxtBuff *Tx1; TxtBuff *Tx2);
   SWM_P(SWP_NDFilter_TBGetSpace,   TxtBuff *Tx;  ULONG Size);
   SWM_P(SWP_NDFilter_TBAppendChar, TxtBuff *Tx;  ULONG Char);
   SWM_P(SWP_NDFilter_TBIsEncaps,   STRPTR s;     ULONG P1; ULONG P2);
   SWM_P(SWP_NDFilter_TBNoCtrl,     TxtBuff *Tx;  ULONG P1; ULONG P2);
   SWM_P(SWP_NDFilter_TBTruncate,   TxtBuff *Tx;  ULONG len);
   SWM_P(SWP_NDFilter_TBDelSub,     TxtBuff *Tx;  ULONG P1; ULONG P2);
   SWM_P(SWP_NDFilter_TBCountChar,  TxtBuff *Tx;  ULONG c);
   SWM_P(SWP_NDFilter_TBAddPrefix,  TxtBuff *Tx;  STRPTR Prefix; ULONG QMode);

   SWM_P(SWP_NDFilter_StartMatch, // start match
         Object     *Art;         // [IN] article
         Object     *Grp;         // [IN] group
         Object     *ArtView;     // [IN] artview object
         ULONG      *FltData);    // ADDR of data for this use of the filter

   SWM_P(SWP_NDFilter_EndMatch,   // end match
         Object     *Art;         // [IN]  article
         Object     *Grp;         // [IN] group
         Object     *ArtView;     // [IN] artview object
         ULONG      *FltData);    // ADDR of data for this use of the filter

   SWM_P(SWP_NDFilter_ModTxt,
         TxtBuff *Tx;               // TxtBuff to modify
         ULONG   *HasHeaders;       // buffer includes headers?
         Object  *Art;              // article object
         Object  *Grp;              // group object
         Object  *ArtView;          // article view object
         ULONG   FltData;           // data for this use of the filter
         ULONG   EndOfData;         // T if this is the end of the data
         TxtBuff *Scratch);         // scratch buffer

   SWM_P(SWP_NDFilter_LastParseLine,
         STRPTR  *Lines;            // input lines
         ULONG   First;             // first candidate
         ULONG   Last;              // last candidate
         ULONG   FltData;           // data for this use of the filter
         ULONG   EndOfData);        // T if this the last batch of lines

   enum {
      SWV_NDFilter_Complete,        // parsing of text was complete
      SWV_NDFilter_Incomplete,      // parsing of text was incomplete
   };

#  define SWP_NDF_StyleAttrs(x,b,i,u,ip,pens,pen,uc)                          \
   SWAM_PF(x##Font, SWS_L_Font, SWAM_UI, NULL,  MAXFONTNAME),                 \
   SWAM_TX(SWS_L_Color, 0),                                                   \
   SWAM_BO(x##UseColor, SWAM_NoLab, SWAM_UI|SWAM_NewHG|SWAM_NoFrame,  uc),    \
   SWAM_PA(x##Color, SWAM_NoLab, pens, pen, ip, SWAM_UI|SWAM_EndG),           \
   SWAM_TX(SWS_L_Style, 0),                                                   \
   SWAM_BO(x##B,   SWS_L_B,    SWAM_UI|SWAM_NewHG|SWAM_NoFrame, b),           \
   SWAM_BO(x##U,   SWS_L_U,    SWAM_UI,    i),                                \
   SWAM_BO(x##I,   SWS_L_I,    SWAM_UI|SWAM_EndG, u)                          \

#  define SWP_NDF_StyleAttrDefs(x) \
      ULONG  x##B;                 \
      ULONG  x##U;                 \
      ULONG  x##I;                 \
      ULONG  x##UseColor;          \
      STRPTR x##Font               \

#  define SWP_NDF_StyleAttrValsF(x, fv) \
      x##B = fv,                        \
      x##U,                             \
      x##I,                             \
      x##Font,                          \
      x##Color,                         \
      x##UseColor                       \

#  define SWP_NDF_StyleAttrVals(x) \
      x##B,                        \
      x##U,                        \
      x##I,                        \
      x##Font,                     \
      x##Color,                    \
      x##UseColor                  \

   enum {
      SWV_NDF_StyleB = 0,          // offsets from ##B attribute
      SWV_NDF_StyleU,
      SWV_NDF_StyleI,
      SWV_NDF_StyleUseColor,
      SWV_NDF_Color,
      SWV_NDF_Font,
   };

#  define SWP_NDF_StyleHelp(x,h)   \
      x##B,        h,              \
      x##U,        h,              \
      x##I,        h,              \
      x##Font,     h,              \
      x##Color,    h,              \
      x##UseColor, h               \

#  define SWP_NDF_StyleLabs \
      SWS_L_Font,           \
      SWS_L_Style,          \
      SWS_L_Color,          \
      SWS_L_B,              \
      SWS_L_U,              \
      SWS_L_I

#  define SWP_NDF_StyleLabStr \
   "Font: ",                  \
   "Style: ",                 \
   "Color: ",                 \
   "Bold: ",                  \
   "Underline: ",             \
   "Italic: "

#  define SWP_NDF_FltToggle(i,x)                                     \
   SetUIToggle((i)->UI, x##Color, x##UseColor, MUIV_NotTriggerValue, \
               !(i)->x##UseColor)

#  define SWP_NDF_TStyleOn(o,i,x,trg,pens,pen)                    \
      DoMethod(o, SWM_NDFilter_StyleOn, (trg), (i)->x##Font,      \
               (i)->x##B, (i)->x##U, (i)->x##I, (i)->x##UseColor, \
               &i->pens[pen])

#  define SWP_NDF_TStyleOff(o,i,x,trg,pens,pen)                   \
      DoMethod(o, SWM_NDFilter_StyleOff, (trg), (i)->x##Font,     \
               (i)->x##B, (i)->x##U, (i)->x##I, (i)->x##UseColor, \
               &i->pens[pen])

#  define SWP_NDF_TStyle(o,i,x,trg,pens,pen,src)                  \
      DoMethod(o, SWM_NDFilter_StyleTxt, (trg), (i)->x##Font,     \
               (i)->x##B, (i)->x##U, (i)->x##I, (i)->x##UseColor, \
               &i->pens[pen], (src))

#  define SWP_NDF_TSObj(msg,o,i,x,trg,pens,pen,src)      \
       DoMethod((msg)->ArtView, SWM_NArticleView_GenObj, \
                SWP_NDF_TStyle(o,i,x,trg,pens,pen,src), NULL)

#  define NDF_AV(s)   (SWPoolAV(idata->SWA_NDFilter_MemPool, (s)))
#  define NDF_SD(s)   (SWPoolStrDup(idata->SWA_NDFilter_MemPool, (s)))
#  define NDF_FV(m)    SWPoolFV(idata->SWA_NDFilter_MemPool, (UBYTE *)(m))
#  define NDF_RV(s,m) (SWPoolRV(idata->SWA_NDFilter_MemPool, (s), (UBYTE **)(m)))

   enum {
      SWV_NDFilter_Off    = SWV_NSFilter_Off,
      SWV_NDFilter_On     = SWV_NSFilter_On,
      SWV_NDFilter_Toggle = SWV_NSFilter_Toggle,
   };

#  define SWP_NDFilter_AttrDefs \
   APTR   SWA_NDFilter_MemPool

#  define SWP_NDFilter_Attrs(Pri) \
   SWAM_FI(SWA_NDFilter_Pri,        SWAM_NoLab,    SWAM_SGIN,    Pri), \
   SWAM_PT(SWA_NDFilter_MemPool,    SWAM_NoLab,    SWAM_SGIN,    NULL)

#  undef SW_FIRSTTAG

# endif
