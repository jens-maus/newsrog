/*****************************************************************************
 * Function: write an icon                                                   *
 *****************************************************************************/

static void SWPutIcon(Object *o, SW_IDATA *idata, UBYTE *Name, STRPTR App,
                      STRPTR Templ)
{
   UBYTE Tool[FMSIZE];

   NameFromLock(GetProgramDir(), Tool, FMSIZE);
   AddPart(Tool, App, FMSIZE);
   
   DoMethod(o, SWM_Root_PutIcon, Templ, Name, Tool);
   DoMethod(o, SWM_Root_CheckErr, _win(o));
}
