/*****************************************************************************
 * Setup code to localize menus and strings                                  *
 *****************************************************************************/

static Object *RegObj           = NULL;

int _STI_6000_RegSetup(void)
{
   if (!(RegObj = NewObject(GetRegisterClass(),   NULL, TAG_DONE))) return -1;
   
   return 0;
}

void _STD_6000_RegSetup(void)
{
   DisposeObject(RegObj);
}

# ifndef RegBanner
   static const char RegBanner[] =
      MUIX_C MUIX_B "Unregistered demo version\33n\n\n"
      "Please consider supporting this software so that\n"
      "more high quality Amiga applications can be\n"
      "created in the future.  Thank you!";
# endif

# define SWRegCheck(App, Win)                                           \
         if (DoMethod(RegObj, SWM_RegCheck, "PROGDIR:" APPNAME ".key",  \
                      APPNAME) != REG_Registered)                       \
            set(Win, MUIA_Window_Open, TRUE);                           \
         DoMethod(RegObj, SWM_Reg_Banner, App, Win, RegBanner)
