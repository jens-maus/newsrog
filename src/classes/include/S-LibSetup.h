
# ifndef SW_LIBSETUP_INCLUDE
#  define SW_LIBSETUP_INCLUDE

   Library *SWSupportBase = NULL;
   Library *MUIMasterBase = NULL;

   int _STI_4000_Libs(void)
   {
     MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN);
     SWSupportBase = OpenLibrary("PROGDIR:MUI/" SWSUPPORTNAME,  SWSUPPORTVER);
     if (!SWSupportBase)
        SWSupportBase = OpenLibrary(SWSUPPORTNAME, SWSUPPORTVER);
   
     if (!MUIMasterBase) PutStr("ERROR: Unable to open " MUIMASTER_NAME "\n");

     if (!SWSupportBase) {
        PutStr("ERROR: Unable to open " SWSUPPORTNAME  "\n");
        if (MUIMasterBase)
           MUI_Request(NULL, NULL, 0, "ERROR", "Ok",
                       "Unable to open %s version %ld.  This\n"
                       "may be because another installed application\n"
                       "is using an older version of the library.",
                       SWSUPPORTNAME, SWSUPPORTVER);
     }
   
     return (SWSupportBase == NULL || MUIMasterBase == NULL) ? -1 : 0;
   }
   
   void _STD_4000_Libs(void)
   {
      SW_CLOSELIB(SWSupportBase);
      SW_CLOSELIB(MUIMasterBase);
   }
   
   int _STI_4100_Setup(void)
   {
      if (SWSupportBase) {
         SWInit();
         SWDB_OPEN(NULL, NULL, NULL);
      }
   
      return 0;
   }
   
   void _STD_4100_Setup(void)
   {
      if (SWSupportBase) {
         SWDB_CLOSE();
         SWExit();
      }
   }
# endif
