
# include "Libraries/C-CText.h"
# include "Libraries/C-ListSearch.h"
# include "S-LibSetup.h"
# include "S-EventLoop.h"

TEXT Buff[1024];

void main (int argc, char *argv[])
{
   ULONG State = 0;
   MUI_PenSpec ps[2];
   Object *pp, *CT1, *CT2, *Win, *App, *vg, *b1, *CT3;

   static STRPTR Lines1[] = {
      "Foo bar test",
      "dummy test " CTxtFont("times/24") "gqah de absoflipping" CTxtFont("") " blah",
      "now is the time\n\n",
      "for " CTxtFont("courier/15/U") "all" CTxtFont("//I") " good men "
         CTxtFont("") "to come" CTxtFont("") " to the aid",
      "(of their country)",
      NULL,
   };

   static STRPTR Lines2 =
      "This\tis lines 2 test\n"
      "It's plain\t\t and boring.\n"
      "But sometimes that's ok for\n"
      "just testing purposes, eh?\n";

   static STRPTR Lines3[] = {
      CTxtFont("topaz/8")CTxtFont("courier/15/U") "BBB"
          CTxtFont("//I") " CCCC ",
      "Nonterm " CTxtFont("times/24") "gqah de absoflipping" CTxtFont("") " blah",
      NULL,
   };

   static STRPTR Lines4[] = {
      "wefwef " CTxtFont("") "boofar fwfw " CTxtFont("") "wefwfww",
      NULL,
   };

   if (!(pp = PendisplayObject, End)) exit(0);

   DoMethod(pp,MUIM_Pendisplay_SetMUIPen,MPEN_SHINE);
   get(pp, MUIA_Pendisplay_Spec, &ps[0]);

   DoMethod(pp,MUIM_Pendisplay_SetMUIPen,MPEN_HALFSHADOW);
   get(pp, MUIA_Pendisplay_Spec, &ps[1]);

   sprintf(Buff,
           "abc\ndef \\033CV{%08lx}this\\033CV{} is a test\nghi\nwfwf\nawf\n",
           &ps[0]);
   
   App = ApplicationObject,
      MUIA_Application_Title, "CText Test",
      SubWindow, Win = WindowObject,
         MUIA_Window_ID,   2,
         WindowContents, VGroup,
            Child, b1 = SimpleButton("Howdy"),
            Child, ScrollgroupObject,
               MUIA_Scrollgroup_Contents, vg = VirtgroupObject,
                  VirtualFrame,
                  MUIA_Group_Horiz,    FALSE,
                  MUIA_Background,     MUII_TextBack,
                  MUIA_Group_Spacing,  0,
                  MUIA_UserData,       SWV_CText_ClickGrpRoot,
                  Child, CT1 = CTextObject,
                     MUIA_VertWeight, 1,
                     SWA_CText_Lines,     Lines1,
                  End,
                  Child, CT2 = CTextObject,
                     MUIA_VertWeight, 1,
                     SWA_CText_PrevCText, CT1,
                     SWA_CText_Contents,  Lines2,
                  End,
                  Child, CTextObject,
                     MUIA_VertWeight, 1,
                     SWA_CText_Contents,  Buff,
                  End,
                  Child, CT3 = CTextObject,
                     MUIA_VertWeight, 1,
                     SWA_CText_Lines,     Lines3,
                  End,
                  Child, CTextObject,
                     MUIA_VertWeight, 1,
                     SWA_CText_PrevCText, CT3,
                     SWA_CText_Lines,     Lines4,
                  End,
                  Child, RectangleObject,
                     MUIA_VertWeight, 1000,
                  End,
               End,
            End,
         End,
      End,
   End;

   if (!App) exit(0);

   DoMethod(Win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, App, 2,
            MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

   DoMethod(vg, SWM_ListSearch_Setup, &State);

   DoMethod(b1, MUIM_Notify, MUIA_Pressed, FALSE,
            vg, 1, SWM_CText_SelAll);

   set(Win, MUIA_Window_Open, TRUE);

   DoMethod(vg, MUIM_List_Jump, 10);

   SWEventLoop(App, Win, NULL, TRUE);

   DoMethod(CT1, SWM_CText_Clip);

   MUI_DisposeObject(App);
   MUI_DisposeObject(pp);
}
