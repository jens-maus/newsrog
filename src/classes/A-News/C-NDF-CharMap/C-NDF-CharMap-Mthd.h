# include "S-CTSupport.h"

/*****************************************************************************
 * Function: save editor contents                                            *
 *****************************************************************************/

static void SaveEditContents(Object *o, SW_IDATA *idata)
{
   STRPTR Buff;
   
   // -- save contents of editor ---------------------------------------------

   if (Buff = (STRPTR)DoMethod(idata->Ed, MUIM_TextEditor_ExportText)) {
      set(o, SWA_NDF_CharMap_MapInfo, Buff);
      FreeVec(Buff);
   }
}


/*****************************************************************************
 * Function: Skip space                                                      *
 *****************************************************************************/

static STRPTR SkipSp(STRPTR s)
{
   while (IsWhite(*s) || *s == '\n') s++;

   return s;
}


/*****************************************************************************
 * Function: Skip line                                                       *
 *****************************************************************************/

static STRPTR SkipLine(STRPTR s)
{
   while (*s && *s != '\n') s++;

   return *s ? s + 1 : s;
}


/*****************************************************************************
 * Function: read character value                                            *
 *****************************************************************************/

static STRPTR ReadChVal(STRPTR s, UBYTE *c)
{
   ULONG v;

   if (!*(s = SkipSp(s))) return s;

   if (s[0] == '0') {
      if ((s[1] == 'x' || s[1] == 'X') && IsHD(s[2]) && IsHD(s[3])) {
         // -- 0xnn - hex ----------------------------------------------------
         v = (hdtod(s[2]) << 4) | hdtod(s[3]), s += 4;
      } else if (IsOD(s[1]) && IsOD(s[2]) && IsOD(s[3])) {
         // -- 0ddd - octal --------------------------------------------------
         v = ddtod(s[1]) * 64 + ddtod(s[2]) * 8 + ddtod(s[3]), s += 4;
      } else v = *s++;
   } else if (IsDD(s[0])) {
      if (IsDD(s[1]) && IsDD(s[2])) {
         // -- ddd - decimal -------------------------------------------------
         v = ddtod(s[0]) * 100 + ddtod(s[1]) * 10 + ddtod(s[2]), s += 3;
      } else if (IsDD(s[1])) {
         // -- dd - decimal --------------------------------------------------
         v = ddtod(s[0]) * 10 + ddtod(s[1]), s += 2;
      } else v = *s++;
   } else v = *s++;

   if (v > 256) { *c = 0; return s; }

   *c = (UBYTE)v;

   return SkipSp(s);
}


/*****************************************************************************
 * Function: Clear character mapping tables                                  *
 *****************************************************************************/

static void ClearMapping(SW_IDATA *idata)
{
   memset(idata->Mapping,  0, sizeof(idata->Mapping));
   memset(idata->Deletion, 0, sizeof(idata->Deletion));

   idata->HaveMap = FALSE;
}

/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
   if (idata->Ed || !idata->HaveMap) {
      STRPTR s;
      UBYTE  c1, c2, c3, c4;
      
      if (idata->Ed) SaveEditContents(o, idata);

      ClearMapping(idata);

      // -- parse the table --------------------------------------------------
      
      for (s=idata->SWA_NDF_CharMap_MapInfo; *s; s) {
         // -- skip comments -------------------------------------------------

         s = SkipSp(s);
            
         if (s[0] == '#' && s[1] == '#') {
            s = SkipLine(s);
            continue;
         }

         // -- add to deletion table -----------------------------------------
         
         if ((s[0] == 'D' || s[0] == 'd') &&
             (s[1] == 'E' || s[1] == 'e') &&
             (s[2] == 'L' || s[2] == 'l')) {

            s = SkipSp(s+3);
            if (*s++ != '=') goto Err;
            s = ReadChVal(s, &c1);

            idata->Deletion[c1] = 1;
            
         } else {
            if (!*(s = ReadChVal(s, &c1))) goto Err;
            if (*s == '=') {
               // -- char = char ------------------------------------------------
               s++;
               s = ReadChVal(s, &c2);
               if (!c1 || !c2)             goto Err;
               idata->Mapping[c1] = c2;
            } else if (*s == '-') {
               // -- char-char = char-char --------------------------------------
               s++;
               if (!*(s = ReadChVal(s, &c2))) goto Err;
               if (*s++ != '=')               goto Err;
               if (!*(s = ReadChVal(s, &c3))) goto Err;
               if (*s++ != '-')               goto Err;
               s = ReadChVal(s, &c4);
   
               if (!c1 || !c2 || !c3 || !c4 ||
                   (c2 < c1  || c4 < c3) ||
                   (c2 - c1) != (c4 - c3))    goto Err;
   
               for (; c1<=c2; c1++, c3++)
                  idata->Mapping[c1] = c3;
                  
            } else goto Err;
         }
      }
   }

   idata->HaveMap = TRUE;
   
   SWDB_RET(SUCCESS);

  Err:
   ClearMapping(idata);
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: End match                                                         *
 *****************************************************************************/

METHOD(SWM_NDFilter_EndMatch, SWP_NDFilter_EndMatch *)
{
   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  bp, Buff = TxBuff(msg->Tx);
   UBYTE   c;

   // -- skip headers --------------------------------------------------------

   Buff += DoMethod(o, SWM_NDFilter_TBSkipHdrs, msg->Tx, *msg->HasHeaders);

   // -- translate characters ------------------------------------------------
   
   if (idata->HaveMap)
      for (bp=Buff; *Buff; )
         if (CT_IsCtrl(Buff) || CT_IsAddObj(Buff))
            do c = *bp++ = *Buff++; while (c != CT_End);
         else if (c = idata->Mapping[*Buff])
            *bp++ = c, Buff++;
         else if (!idata->Deletion[*Buff])
            *bp++ = *Buff++;

   SWDB_RET(SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/
   
# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   idata->UI = VGroup,
      Child, Label2(GetS(SWS_L_TopTitle)),
      Child, idata->Ed = NN_EditorObject,
         AppBaseDefTop(idata),
         MUIA_ShortHelp, GetS(SWS_H_MapHelp),
         SWA_NNews_Editor_InitStrAddr, &idata->SWA_NDF_CharMap_MapInfo,
      End,
   End;

   if (!idata->UI) SWDB_RET(NULL);

   SWDB_RET(idata->UI);
} SDFT


/*****************************************************************************
 * Method: Hide UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterHIDEUIF
METHOD(SWM_NDFilter_HideUI, Msg)
{
   if (!idata->UI) SWDB_RET(FAILURE);

   SaveEditContents(o, idata);

   idata->Ed      = NULL;
   idata->HaveMap = FALSE;

   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT
