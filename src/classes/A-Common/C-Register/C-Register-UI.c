
# include "Libraries/C-Register.h"
# include "C-Register-I.h"

struct RG_ptab {
   UBYTE *Keyword;
   LONG  Required;
} RG_ParseTable[] = {
   "First",       TRUE,
   "Last",        TRUE,
   "Company",     FALSE,
   "Email",       FALSE,
   "Addr1",       FALSE,
   "Addr2",       FALSE,
   "City",        FALSE,
   "State/Prov",  FALSE,
   "Zip",         FALSE,
   "Country",     FALSE,
   "DayPhone",    FALSE,
   "EvePhone",    FALSE,
   NULL,          NULL,
};


/*****************************************************************************
 *   Generate a UI form for mailing in                                       *
 *****************************************************************************/


__saveds __asm ULONG GROM_Generate (a2 Object *Win, a1 SW_IDATA **idata)
{
   UBYTE fspec[FMSIZE];
   BPTR  fp = NULL;
   ULONG x, Cost1 = (ULONG)(idata[2]), Cost2 = (ULONG)(idata[3]);
   UBYTE DoneText[] = "\33c\33bDone:\33n Registration form saved to file %s\n\n"
                      "Email to " SRK_EMAIL " or send by postal mail to:\n\n"
                      "   %s Registration\n"
                      "   405 Pulsar St.\n"
                      "   Fort Collins, CO 80525 USA\n\n"
                      "See the above file for more information.\n\n"
                      "Thank you for registering %s!";
   UBYTE ErrText[]  = "\33c\33bError:\33n Unable to open registration file %s.";
   UBYTE *Prog = (UBYTE *)(idata[1]);

   AddPart(strcpy(fspec, "RAM:"), Prog, FMSIZE);
   strcat(fspec, ".Regform");

   for (x=0; RG_ParseTable[x].Keyword; x++) {
      UBYTE *Val;
      get((*idata)->Gads[x], MUIA_String_Contents, &Val);

      if (RG_ParseTable[x].Required && (!Val || !Val[0])) {
         MUI_Request(Win, (*idata)->RegWin, 0,NULL, "*_Ok",(char *)
                     "\33c\33bError:\33n No value suppled for required "
                     "field '%s'\n",
                     RG_ParseTable[x].Keyword, NULL);
         goto Done;
      }
   }


   if (!(fp = Open(fspec, MODE_NEWFILE))) {
      MUI_Request(Win, (*idata)->RegWin, 0,NULL, "*_Ok",(char *)ErrText,
                  fspec, NULL, NULL);
      goto Done;
   }

   FPrintf(fp, "E-mail this form to %s with the subject line:\n"
               "  CMD REGISTER %s\n\n"
               "Registration cost is US $%ld for E-Mail registration and\n"
               "US $%ld for postal mail registration.  E-Mail registration\n"
               "will receive a keyfile shortly.  Postal registration should\n"
               "allow 4 weeks and include a disk and self addressed stamped\n"
               "envelope.  Checks can be made payable to ShadowWorks Software.\n\n"
               "Registration request for %s\n\n",

           SRK_EMAIL, Prog, Cost1, Cost2, Prog);

   for (x=0; RG_ParseTable[x].Keyword; x++) {
      UBYTE *Val;
      
      get((*idata)->Gads[x], MUIA_String_Contents, &Val);

      if (Val && Val[0])
         FPrintf(fp, "%-10s = %s\n", RG_ParseTable[x].Keyword, Val);
   }

   Close(fp); fp = NULL;

   MUI_Request(Win, (*idata)->RegWin, 0,NULL, "*_Ok",(char *)DoneText,
               fspec, Prog, Prog);

   set(Win, MUIA_Window_Open, FALSE);

  Done:
   if (fp) Close(fp);

   return 0;
}


/*****************************************************************************
 *   Method: Display our UI                                                  *
 *****************************************************************************/

struct Hook RGOM_Hook_Generate = { {0,0}, GROM_Generate,  NULL, NULL };

ULONG RGOM_RegisterUI(SW_IDATA *idata, SWP_RegUI *UI)
{
   Object *g;
   LONG   x;

   if (!idata->RegWin) {

      idata->RegWin = WindowObject,
         MUIA_Window_Title, "Registration",
         MUIA_Window_ID,    MAKE_ID('r','g','s','r'),
         WindowContents, VGroup,
            Child, ScrollgroupObject,
               MUIA_Scrollgroup_FreeHoriz, FALSE,
               MUIA_Scrollgroup_Contents,  VirtgroupObject,
                  VirtualFrame,
                  Child, g = ColGroup(3),
                     GroupFrameT("Registration Information"),
                  End,
               End,
            End,
            Child, HGroup, GroupFrame,
               Child, idata->B_Generate = SimpleButton("_Generate"),
               Child, idata->B_Cancel   = SimpleButton("_Cancel"),
            End,
         End,
      End;

      if (!idata->RegWin) return FAILURE;

      for (x=0; RG_ParseTable[x].Keyword; x++) {
         DoMethod(g, OM_ADDMEMBER, Label2(RG_ParseTable[x].Keyword));
         DoMethod(g, OM_ADDMEMBER,
                  idata->Gads[x] =String("", SWF_CryptLineSize));
         set(idata->Gads[x], MUIA_CycleChain,         1);
         set(idata->Gads[x], MUIA_String_AdvanceOnCR, 1);
         DoMethod(g, OM_ADDMEMBER,
                  Label2(RG_ParseTable[x].Required ? "Req" : "Opt"));
      }

      set(idata->B_Generate, MUIA_CycleChain, 1);
      set(idata->B_Cancel,   MUIA_CycleChain, 1);

      DoMethod(idata->B_Generate, MUIM_Notify, MUIA_Pressed, FALSE,
               UI->App, 6, MUIM_CallHook, &RGOM_Hook_Generate,
               idata, UI->Prog, UI->Cost1, UI->Cost2);

      SW_NOTIFYSINGLE(idata->B_Cancel, MUIA_Pressed, FALSE,
                      idata->RegWin, MUIA_Window_Open, FALSE);

      SW_NOTIFYCLOSEWIN(idata->RegWin);

      DoMethod(UI->App, OM_ADDMEMBER, idata->RegWin);
   }

   if (!idata->RegWin) return FAILURE;

   set(idata->RegWin, MUIA_Window_Open, TRUE);

   return SUCCESS;
}
