# ifndef C_CTEXT_INCLUDE_I
#  define C_CTEXT_INCLUDE_I

#  include "Libraries/C-CText.h"
#  include "Libraries/C-ListSearch.h"
#  include <graphics/text.h>
#  include <diskfont/diskfont.h>
#  include <proto/diskfont.h>


   // -- this classes names, etc --------------------------------------------

#  define SW_CLIB             SWC_CText
#  define SW_CNAME            "CText"
#  define SW_SUPERPUB         MUIC_Area

#  include "S-Params.h"
#  include "Proto/CText_protos.h"

   // -- our instance specific data ------------------------------------------

   enum {
      CTP_DefPen = 0,
   };

   typedef struct _lineinfo {
      UWORD AboveBase,
            BelowBase,
            LineLens,
            LineFont;
   } LineInfo;

   typedef struct _fontinfo {
      struct TextFont  *Fonts;
      LONG              Pens;
      UBYTE             FontAlloc;
      UBYTE             FontStyle;
      UBYTE             PenAlloc;
   } FontInfo;

   enum {
      SWV_RelLine_BeforeFirst = -1,
      SWV_RelLine_AfterLast   = -2,
   };

   enum {
      FCTP_LineCol,
      FCTP_XYPos,
   };

   enum {
      SWF_CText_Selecting = (1<<0),
   };

   typedef struct _SELREG
   {
      LONG              Lp, L0, L1, Xp, X0, X1, C0, C1;
   } SelReg;

   typedef struct SW_IDATA
   {
      STRPTR           *SWA_CText_Lines;
      LONG              SWA_CText_LineCnt;
      APTR              SWA_CText_MemPool;
      ULONG             SWA_CText_LineSpacing;
      STRPTR            SWA_CText_DefFont;
      MUI_PenSpec       SWA_CText_PopPens[1];
      ULONG             SWA_CText_Select;
      ULONG             SWA_CText_TabSize;
      ULONG             SWA_CText_AutoClip;

      LineInfo         *LineInfo;
      UWORD            *SegLens;
      FontInfo         *FontInfo;

      LONG              FontMax;
      LONG              PenMax;
      
      LONG              FirstLine;
      ULONG             Flags;

      ULONG             DragMode, Secs, Micros;  // for doubleclick detection
      LONG              Drag_MouseXInit, Drag_MouseYInit, DragDir, DragSt;
      Object           *DragGrp;

      STRPTR            LineBuff;
      LONG              LineBuffSize;
      LONG              LineBuffLine;

      LONG              SelRegMax;
      LONG              SelRegCnt;
      SelReg           *SelReg, *SR_Draw;

      LONG              AMM_Height, AMM_Width;

      Object           *PrevCText;
   } SW_IDATA;

#  define SWV_CText_WordBrkChars " ;:'\",<.>/?!@#$%^&*()_+|-=\\`~[]{}"

#  define FONTTABMAX 80
#  define FStyles (FSF_UNDERLINED|FSF_BOLD|FSF_ITALIC)

#  define AV(s)   (SWPoolAV(idata->SWA_CText_MemPool, (s)))
#  define FV(m)    SWPoolFV(idata->SWA_CText_MemPool, (STRPTR)(m))
#  define RV(s,m) (SWPoolRV(idata->SWA_CText_MemPool, (s), (UBYTE **)(m)))
#  define SDup(s) (SWPoolStrDup(idata->SWA_CText_MemPool, (s)))

#  define avtod(x) (LONG)(((x)<='9')?(x)-'0':((x)>='a'?((x)-'a'):((x)-'A'))+10)

#  define CTxt(x) (idata->SWA_CText_Lines[x])

#  define CT_LineHt(idata, x) (idata->LineInfo[x].BelowBase + idata->LineInfo[x].AboveBase)
#  define CT_LineSp(idata, x) (CT_LineHt(idata,x) + idata->SWA_CText_LineSpacing)

#  define CT_HasSelRegion(idata) (idata->SelRegCnt >= 0)
#  define CT_SelRegActive(sr)    ((sr)->L0 >= 0 && (sr)->L1 >= 0)

#  define Shifted(msg) ((msg)->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT))
#  define Alted(msg)   ((msg)->Qualifier & (IEQUALIFIER_LALT | IEQUALIFIER_RALT))

# endif // C_CTEXT_INCLUDE_I
