
/*****************************************************************************
 * Function: Compare two variables for sorting                               *
 *****************************************************************************/

static LONG CmpVars(Object *o1, Object *o2)
{
   return 
      strcmp(SWGetStr(o1, SWA_FileTypes_FT), SWGetStr(o2, SWA_FileTypes_FT));
}


/*****************************************************************************
 * Function: File/Dir Type                                                   *
 *****************************************************************************/

ULONG FTFindType(STRPTR Name)
{
   if (SWDirExists(Name))  return SWV_Filetypes_Dir;
   if (SWFileExists(Name)) return SWV_Filetypes_File;

   return SWV_Filetypes_Unknown;
}

   
/*****************************************************************************
 * Function: Match file                                                      *
 *****************************************************************************/

static ULONG MatchNameCase(STRPTR Name, STRPTR Pat, STRPTR Buff, ULONG Type)
{
   if (FTFindType(Name) == Type)
      if (ParsePattern(Pat, Buff, FT_PatLen) >= 0)
         return MatchPattern(Buff, FilePart(Name));

   return FALSE;
}


/*****************************************************************************
 * Function: Match file                                                      *
 *****************************************************************************/

static ULONG MatchNameNoCase(STRPTR Name, STRPTR Pat, STRPTR Buff, ULONG Type)
{
   if (FTFindType(Name) == Type)
      if (ParsePatternNoCase(Pat, Buff, FT_PatLen) >= 0)
         return MatchPatternNoCase(Buff, FilePart(Name));

   return FALSE;
}


/*****************************************************************************
 * Function: Hex digit value                                                 *
 *****************************************************************************/

static LONG HexDigit(UBYTE x)
{
   return (x>='0' && x<='9') ? (x-'0')    :
          (x>='a' && x<='f') ? (x-'a'+10) :
          (x>='A' && x<='F') ? (x-'F'+10) : 0;
}


/*****************************************************************************
 * Function: Match file                                                      *
 *****************************************************************************/

static ULONG MatchFCBin(STRPTR Name, STRPTR Pat, STRPTR Buff, ULONG BuffLen)
{
   ULONG Pos = 0;
   UBYTE Val;

   if (!Pat[0]) return FALSE;
   
   while (Pat[0]) {
      while (Pat[0] == ' ') Pat++;

      if (!Pat[0]) break;

      // -- update position --------------------------------------------------

      if (!strncmp(Pat, KW_Pos, KWS_Pos)) {
         Pos = 0;
         Pat = Pat + KWS_Pos;
         while (Pat[0] >= '0' && Pat[0] <= '9')
            Pos = Pos * 10 + *Pat++ - '0';

         if (Pos >= BuffLen) Pos = 0;
         continue;
      }

      // -- parse 0x hex digits ----------------------------------------------
      
      if (Pat[0] == '0' && (Pat[1] == 'x' || Pat[1] == 'X')) {
         Pat += 2;

         while (Pat[0] && Pat[1] && Pat[0] != ' ' && Pat[1] != ' ') {
            Val = HexDigit(Pat[0]) * 16 + HexDigit(Pat[1]);

            if (Val != Buff[Pos++]) return FALSE;

            Pat += 2;
         }

         if (Pat[0]==',') Pat++;

         continue;
      }

      // -- parse normal digits ----------------------------------------------
      
      while(Pat[0] && Pat[0] != ' ') {
         if (Pat[0] != Buff[Pos++]) return FALSE;
         Pat++;
      }
   }
   
   return TRUE;
}


/*****************************************************************************
 * Function: Match file                                                      *
 *****************************************************************************/

static ULONG MatchFCPattern(STRPTR Name, STRPTR Pat, STRPTR Buff)
{
   ULONG found  = FALSE;
   ULONG patlen = strlen(Pat)*2+2;
   BPTR  in = NULL;
   STRPTR Line, PatBuf = NULL;

   if (!(Line   = AllocVec(SWV_Filetypes_LineMax, MEMF_ANY))) goto Done;
   if (!(PatBuf = AllocVec(patlen,                MEMF_ANY))) goto Done;

   if (ParsePattern(Pat, PatBuf, patlen) < 0)                 goto Done;
   
   if (!(in = Open(Name, MODE_OLDFILE)))                      goto Done;

   while (FGets(in, Line, SWV_Filetypes_LineMax))
      if (MatchPattern(PatBuf, Line)) {
         found = TRUE;
         break;
      }

  Done:
   if (in) Close(in);
   FreeVec(Line);
   FreeVec(PatBuf);
   return found;
}


/*****************************************************************************
 * Function: Match file                                                      *
 *****************************************************************************/

static STRPTR AskExternal(STRPTR Name, STRPTR Pat, STRPTR Buff, Object *o)
{
   UBYTE out[16];
   BPTR  fp;

   sprintf(out, "PIPE:SW_%06ld", GetUniqueID());

   if (DoMethod(o, SWM_Root_SysCmd,
                SWT_Root_OutFile,  out,
                SWT_Root_Cmd,      Pat,
                SWT_Root_Arg,      Name,
                SWT_Root_Stack,    32000,
                TAG_DONE))
      return NULL;

   if (!(fp = Open(out, MODE_OLDFILE)))
      return NULL;

   Buff[0] = '\0'; 
   if (FGets(fp, Buff, 64))
      while (FGets(fp, out, 15));  // read rest of file

   Close(fp);

   SW_REMOVENL(Buff);
   
   return Buff[0] ? Buff : NULL;
}


/*****************************************************************************
 * Method: Find a file type given its name                                   *
 *****************************************************************************/

static STRPTR Type_Mem(Object *o, SW_IDATA *idata,
                       STRPTR Name, STRPTR Buff, ULONG Size)
{
   ULONG x;
   Object *entry;

   SW_MUIListLoop(idata->SWA_NList_L, x, entry, TRUE) {
      STRPTR Pat, Type;

      Type = SWGetStr(entry, SWA_FileTypes_FT);
      Pat  = SWGetStr(entry, SWA_FileTypes_Patt);

      if (SW_STRISNULL(Type) || SW_STRISNULL(Pat))
         continue;

      switch (SWGetUL(entry, SWA_FileTypes_Method)) {
        case FT_MatchFNameNoCase: if (Name &&
                                      MatchNameNoCase  (Name, Pat, idata->Buff,
                                                        SWV_Filetypes_File))
                                     return Type;
                                  break;
        case FT_MatchFNameCase:   if (Name && MatchNameCase(Name, Pat, idata->Buff,
                                                            SWV_Filetypes_File))
                                     return Type;
                                  break;
        case FT_MatchFCBin:       if (Buff && MatchFCBin   (Name, Pat, Buff,
                                                            Size))
                                     return Type;
                                  break;
        case FT_MatchFCPattern:   if (MatchFCPattern   (Name, Pat, idata->Buff))
                                     return Type;
                                  break;
        case FT_MatchDNameNoCase: if (Name && MatchNameNoCase  (Name, Pat, idata->Buff,
                                                                SWV_Filetypes_Dir))
                                     return Type;
                                  break;
        case FT_MatchDNameCase:   if (Name && MatchNameCase    (Name, Pat, idata->Buff,
                                                                SWV_Filetypes_Dir))
                                     return Type;
                                  break;
        case FT_AskExternal:      if (Name && (Type=AskExternal (Name, Pat, idata->Buff, o)))
                                     return Type;
                                  break;
      }
   }

   SWDB_RET(NULL);
} SDFT


/*****************************************************************************
 * Method: Find a file type                                                  *
 *****************************************************************************/

METHOD(SWM_FileTypes_FindType, Msg)
{
   STRPTR FSpec  = GetTagStr(SWT_FileTypes_FT_FSpec,    NULL, msg+1);
   STRPTR Mem    = GetTagStr(SWT_FileTypes_FT_Mem,      NULL, msg+1);
   ULONG  MemSize= GetTagUL(SWT_FileTypes_FT_MemSize,      0, msg+1);
   BPTR   in;
   STRPTR Contents = NULL, Type;

   if (SW_STRISNULL(FSpec)) SWDB_RET(NULL);

   if (!Mem) {
      if (!(Contents = AllocVec(FT_PatLen+2, MEMF_ANY)))
         SWDB_RET(NULL);

      if (in = Open(FSpec, MODE_OLDFILE)) {
         Mem     = Contents;
         MemSize = FRead(in, Contents, 1, FT_PatLen);
         Close(in);
      } else MemSize = 0;
   }

   Type = Type_Mem(o, idata, FSpec, Mem, MemSize);

   if (Contents)
      FreeVec(Contents);
   
   SWDB_RET(Type);
} SDFT


/*****************************************************************************
 * Method: insert object from disk                                           *
 *****************************************************************************/
   
METHOD(SWM_FileTypes_Ins, Msg)
{
   Object *new;

   if (new = (Object *)DoMethod(o, SWM_Root_M_Open,
                                SWT_Root_SL_Win,         _win(o),
                                SWT_Root_SL_InitDrawer,  SWF_DefObjDir,
                                SWT_Root_SL_InitPattern, SWF_DefObjPattern,
                                TAG_DONE))
      if (SW_ISCLASS(new, SWC_Attrval))
         DoMethod(o, MUIM_List_InsertSingle, new, MUIV_List_Insert_Bottom);
      else
         MUI_DisposeObject(new);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: insert object from disk                                           *
 *****************************************************************************/
   
METHOD(SWM_FileTypes_Save, Msg)
{
   Object *Item;
   
   DoMethod(o, MUIM_List_GetEntry, MUIV_List_GetEntry_Active, &Item);
   
   if (Item)
      DoMethod(Item, SWM_Root_M_SaveAs,
               SWT_Root_SL_Win,         _win(o),
               SWT_Root_SL_InitDrawer,  SWF_DefObjDir,
               SWT_Root_SL_InitPattern, SWF_DefObjPattern,
               SWT_Root_SL_SaveMode,    TRUE,
               TAG_DONE);

   SWDB_RET(0);
} SDFT
