# ifndef C_CTEXT_INCLUDE
#  define C_CTEXT_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"
#  include "Libraries/C-Root.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 2800)

   enum {
      SWA_CText_Lines   = SW_FIRSTTAG,       // [ GI] text lines
      SWA_CText_Contents,                    // [  I] single line init
      SWA_CText_LineCnt,                     // [ G ] # of lines
      SWA_CText_MemPool,                     // [ GI] mempool
      SWA_CText_LineSpacing,                 // [ GI] line spacing
      SWA_CText_DefFont,                     // [ GI] default font
      SWA_CText_DefPen,                      // [ GI] default pen
      SWA_CText_Select,                      // [SGI] selectable?
      SWA_CText_PrevCText,                   // [  I] previous ctext (for stack)
                                             // ..... set BEFORE Contents|Lines!
      SWA_CText_TabSize,                     // [  I] tab size (def 8)
      SWA_CText_AutoClip,                    // [  I] T for auto-clip on select
      SWA_CText_NumSelections,               // [ G ] number of selected regions
   };

   enum {
      SWM_CText_Clear    = SW_FIRSTTAG,      // clear all lines
      SWM_CText_AddLines,                    // add lines
      SWM_CText_GetClipStr,                  // get clip string
      SWM_CText_Unsel,                       // unselect select region
      SWM_CText_SelAll,                      // select all
      SWM_CText_DragStart,                   // start drag operation
      SWM_CText_DragStop,                    // stop drag operation
      SWM_CText_GCInt,                       // private
      SWM_CText_GetGrpClip,                  // get clip str for group
      SWM_CText_Clip,                        // copy above to clipboard
      SWM_CText_SelRegion,                   // select region
      SWM_CText_GetStack,                    // get unpopped ctrl stuff
      SWM_CText_FreeStack,                   // free unpopped ctrl stack
      SWM_CText_LineRangeHeight,             // compute sum of line heights
    };

   enum {
      SWV_CText_SelMode_Char = 0,           // drag selection mode
      SWV_CText_SelMode_Word,
      SWV_CText_SelMode_Line,
   };

   enum {
      SWV_CText_Unsel_All   = -1,
      SWV_CText_Unsel_First = -2,
      SWV_CText_Unsel_Last  = -3,
   };

   enum {
      SWV_CText_Sel_All   = -1,
      SWV_CText_Sel_First = -2,
      SWV_CText_Sel_Last  = -3,
   };

   SWM_P(SWP_CText_Unsel, LONG Pos);

   SWM_P(SWP_CText_Clip,            LONG Num);
   SWM_P(SWP_CText_GetClipStr,      LONG Num);
   SWM_P(SWP_CText_GetGrpClip,      LONG Num);
   SWM_P(SWP_CText_AddLines,        STRPTR *Array; LONG Count);
   SWM_P(SWP_CText_DragStart,       LONG x; LONG y; ULONG Qualifier; ULONG Mode);
   SWM_P(SWP_CText_GCInt,           STRPTR **SArray; Object ***OArray;
                                    LONG *Pos; LONG *Size; LONG Num);
   SWM_P(SWP_CText_SelRegion,       LONG L0; LONG C0; LONG L1; LONG C1);
   SWM_P(SWP_CText_FreeStack,       STRPTR s);
   SWM_P(SWP_CText_LineRangeHeight, LONG L0; LONG L1; LONG *Height);

   enum {
      SWV_CText_Select_None = 0,  // can't select text
      SWV_CText_Select_All,       // can select in all ways
      SWV_CText_Select_NoMulti,   // can't multi-click to select
   };

#  define SWV_CText_ClickGrpRoot MAKE_ID('C','t','C','g')

#  define CTextObject MUI_NewObject(SWC_CText

#  define CT_End  '}'
#  define CT_Ctrl '\033'

#  define CTxtFontSt  "\033FN{"
#  define CTxtPenSt   "\033CV{"
#  define CTxtCtrlEnd "}"

#  define CTxtFont(x) CTxtFontSt x CTxtCtrlEnd
#  define CTxtPen(x)  CTxtPenSt  x CTxtCtrlEnd

#  define CT_IsPen(s)   ((s)[0]==CT_Ctrl&&(s)[1]=='C'&&(s)[2]=='V'&&(s)[3]=='{')
#  define CT_IsFont(s) ((s)[0]==CT_Ctrl&&(s)[1]=='F'&&(s)[2]=='N'&&(s)[3]=='{')
#  define CT_IsCtrl(s)  (CT_IsPen(s) || CT_IsFont(s))
#  define CT_IsEmpty(s) ((s)[4]==CT_End)
#  define CT_IsEnd(s)   ((s)[0]==CT_End)

#  define CT_Skip     4

#  define CTxtB()    CTxtFont("//B")
#  define CTxtI()    CTxtFont("//I")
#  define CTxtU()    CTxtFont("//U")
 
#  undef SW_FIRSTTAG   

# endif
