
# include "S-SetUIToggle.h"
# include "S-GetEOL.h"

/*****************************************************************************
 * Function: Figure out what type of reference this is (email/etc)           *
 *    Pass in: Addr = beginning of address                                   *
 *             Line = pointer to beginning of line                           *
 *             Type = if this is a known type already                        *
 *    Returns: RefType_Unknown, RefType_EMail, or RefType_MsgID              *
 *****************************************************************************/

static ULONG RefType(STRPTR Addr, STRPTR Line, ULONG Type)
{
   STRPTR at;
   ULONG  SChr = 0, Dig = 0, HexDig = 0, Len, UScore = 0, Bracket = FALSE,
          Pref;

   if (Type != RefType_Unknown) return Type;
   
   if (!Addr || !*Addr) return RefType_Unknown;

   // -- can we just look at the header? -------------------------------------
   
   if (SWP_HasHeader(Line, SWF_NAHeader_Ref)      ||
       SWP_HasHeader(Line, SWF_NAHeader_Followup) ||
       SWP_HasHeader(Line, SWF_NAHeader_MsgID))
      return RefType_MsgID;

   if (SWP_HasHeader(Line, SWF_NAHeader_From) ||
       SWP_HasHeader(Line, SWF_NAHeader_XComplain))
      return RefType_EMail;

   // -- guess not - carry on... ---------------------------------------------
   
   if (*Addr == '<')
      Addr++, Bracket = TRUE;

   // -- count various things, like the # of digits etc ----------------------
   
   for (at=Addr; *at && *at != '@'; at++)
      if (*at == '$' || *at == '#')
         SChr++;
      else if (*at == '_')
         UScore++;
      else if (IsDig(*at))
         Dig++, HexDig++;
      else if (IsHexDig(*at) || *at == '.')
         HexDig++;

   // -- gotta have one of these! --------------------------------------------
   
   if (*at != '@') return RefType_Unknown;

   // -- figure out what we`re looking at ------------------------------------
      
   Len = at - Addr;

   Pref = (Len > 8 &&
           (at[-1]=='f' && at[-2]=='s' && at[-3]=='f' && at[-4]=='.' ||
            at[-1]=='n' && at[-2]=='l' && at[-3]=='.'));

   if (!Bracket && Addr >= (Line + 5) && Strnicmp(Addr-5, "news:", 5) == 0    ||
        Bracket && Addr >= (Line + 9) &&
           (Strnicmp(Addr-9, "article ", 8) == 0 ||
            Strnicmp(Addr-9, "message ", 8) == 0))
      return RefType_MsgID;
   
   if ((Dig == 0 || !Bracket ||
        Len < 20 && UScore && !SChr) && !Pref)
      return RefType_EMail;

   if (Addr > (Line + 5) &&
       Addr[-2] == ' ' && Addr[-3] == '"')
      return RefType_EMail;

   if (Len > 15                   ||  // long things, or...
       SChr                       ||  // things with special characters, or,
       (Len >=8 &&
          (Len == HexDig          ||  // medium things that are all hex, or,
           IsDig(*Addr)           ||  // medium things w/ 1st char a number, or,
           Pref)))                    // things that have .fsf@/etc in them
      return RefType_MsgID;

   return RefType_EMail;
}


/*****************************************************************************
 * Method: Start match                                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_StartMatch, SWP_NDFilter_StartMatch *)
{
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
 * Function: see if there's a pattern in this line                           *
 *****************************************************************************/

static __inline STRPTR EMail_Find(SW_IDATA *idata,
                                  STRPTR s, LONG *SCol, LONG *ECol)
{
   LONG x, b, e, found, founddot;
   
   if (!*ECol) *ECol = strlen(s);

   if (s[0] == '\n') return NULL;

   for (x=*SCol; x<=*ECol; x++)
      if (s[x] == '@') {
         if (!x) continue;

         // -- look for beginning of address ---------------------------------
         
         for (found=FALSE, b=x-1; b>=*SCol; b--)
            if (strchr(UserBrkChrs, s[b])) {
               found = TRUE;
               break;
            }

         if (found || b < 0) b++;

         // -- look for end of address ---------------------------------------
         
         for (e=x+1, found=founddot=FALSE; e<=*ECol; e++) {
            if (s[e] == '.') founddot = TRUE;
            if (strchr(DomainBrkChrs, s[e])) {
               found = TRUE;
               break;
            }
         }

         // -- make sure we got a dot (.) in the addr if required ------------
         
         if (idata->SWA_NDF_EMail_RequireDot && !founddot) continue;

         if (found || s[e] == '\0') e--;

         // -- nuke trailing ., since it`s probably the end of a sentence. ---

         while (e > x && s[e] == '.') e--;

         if (b == x || (e-x) < 3) continue;

         *SCol = b;
         *ECol = e;

         return s + *SCol;
      }

   return NULL;
}


/*****************************************************************************
 * Method: Modify buffer appropriately                                       *
 *****************************************************************************/

METHOD(SWM_NDFilter_ModTxt, SWP_NDFilter_ModTxt *)
{
   STRPTR  Buff = TxBuff(msg->Tx);
   LONG   StCol, EndCol, bol, eol, eol2, LastPos = 0, RType;
   UBYTE   c, c1;
   TEXT    AddObj[AV_AddOSize];
   AddData *AD;

   if (!msg->Tx) SWDB_RET(SWV_NDFilter_Incomplete);

   DoMethod(o, SWM_NDFilter_TBGetSpace, msg->Scratch, TxSize(msg->Tx));

   // -- loop through buffer looking for URL pattern -------------------------

   for (bol=0; Buff[bol]; bol = eol + 1) {
      eol2 = eol = GetEOL(Buff, bol);

      if (SWP_HasHeader(Buff+bol, SWF_NAHeader_MsgID)    ||
          SWP_HasHeader(Buff+bol, SWF_NAHeader_Path))
         continue;
      
      c    = Buff[eol2], Buff[eol2] = '\0';

      StCol  = 0;
      EndCol = eol2 - bol - 1;

      // -- see if we have an email type thing in this line ------------------
      
      if (EMail_Find(idata, Buff+bol, &StCol, &EndCol) &&
          DoMethod(o, SWM_NDFilter_TBNoCtrl, msg->Tx,bol+StCol,bol+EndCol)) {

         // -- find out what the heck this is --------------------------------
         
         RType = RefType(Buff + bol + StCol -
                         ((Buff[bol + StCol - 1] == '<') ? 1 : 0),
                         Buff + bol, RefType_Unknown);

         // -- if it`s a message-ID, get proper bounds -----------------------
         
         if (RType == RefType_MsgID && Buff[bol+StCol-1] == '<') {
            while (StCol > 0 && Buff[bol+StCol] != '<')         StCol--;
            while (Buff[bol+EndCol] && Buff[bol+EndCol] != '>') EndCol++;
         }

         // -- append the stuff that comes before this -----------------------
         
         if ((bol+StCol) > 0)
            DoMethod(o,SWM_NDFilter_TBAppendSub, msg->Scratch,
                     Buff, LastPos, bol+StCol-1);

         if (!(AD = (AddData *)NDF_AV(sizeof(AddData))))
            SWDB_RET(SWV_NDFilter_Incomplete);

         AD->Grp     = msg->Grp;
         AD->Art     = msg->Art;
         AD->RefType = RType;
         DoMethod(o, SWM_NDFilter_TBInit, &AD->EMail);

         c1 = Buff[bol+EndCol+1], Buff[bol+EndCol+1] = '\0';

         DoMethod(o, SWM_NDFilter_TBAppendTxt, &AD->EMail, Buff + bol + StCol);
         DoMethod(o, SWM_NDFilter_TBTruncate,  &AD->EMail, MaxURLLen);

         Buff[bol+EndCol+1] = c1;

         DoMethod(o, SWM_NDFilter_TBAppendTxt, msg->Scratch,
                  DoMethod(msg->ArtView, SWM_NArticleView_AddObj, AddObj, AD, o));

         LastPos = (eol = bol + EndCol) + 1;
      }

      Buff[eol2] = c;
   }

   if (LastPos) {
      DoMethod(o, SWM_NDFilter_TBAppendSub, msg->Scratch, Buff, LastPos,
               TxLen(msg->Tx));
   
      DoMethod(o, SWM_NDFilter_TBAssign, msg->Tx, msg->Scratch);
   }

   SWDB_RET(SWV_NDFilter_Complete);
} SDFT


/*****************************************************************************
 * Method: Generate Text Object                                              *
 *****************************************************************************/

static Object *TxtObj(Object *o, SW_IDATA *idata, STRPTR EMail,
                      TxtBuff *Tx, ULONG UseColor, ULONG Pen,
                      STRPTR DefFont, void *DefColor, ULONG Type)
{
   if (Type == RefType_MsgID)
      DoMethod(o, SWM_NDFilter_StyleTxt, Tx,
               idata->SWA_NDF_EMail_MsgIDFont,
               idata->SWA_NDF_EMail_MsgIDB,
               idata->SWA_NDF_EMail_MsgIDU,
               idata->SWA_NDF_EMail_MsgIDI,
               UseColor, &idata->SWA_NDF_EMail_Pens[Pen], EMail);
   else
      DoMethod(o, SWM_NDFilter_StyleTxt, Tx,
               idata->SWA_NDF_EMailFont,
               idata->SWA_NDF_EMailB,
               idata->SWA_NDF_EMailU,
               idata->SWA_NDF_EMailI,
               UseColor, &idata->SWA_NDF_EMail_Pens[Pen], EMail);

   return CTextObject,
      MUIA_HorizWeight,      1,
      SWA_CText_MemPool,     idata->SWA_NDFilter_MemPool,
      SWA_CText_Contents,    TxBuff(Tx),
      SWA_CText_AutoClip,    NN_AutoClip(idata),
      SWA_CText_DefFont,     DefFont,
      SWA_CText_DefPen,      DefColor,
      SWA_CText_Select,      SWV_CText_Select_NoMulti,
   End;
}


/*****************************************************************************
 * Method: Generate add object                                               *
 *****************************************************************************/

METHOD(SWM_NDFilter_GenAddObj, SWP_NDFilter_GenAddObj *)
{
   Object *Obj, *Obj0, *Obj1;
   AddData *AD = (AddData *)msg->Data;
   STRPTR EMail, DefFont, Type, Var, Action;
   TxtBuff Tx;
   TEXT   URL[MaxURLLen+80];
   void   *DefColor;
   
   DefFont  = (STRPTR)DoMethod(msg->ArtView, SWM_NArticleView_GetDefFont);
   DefColor = (void *)DoMethod(msg->ArtView, SWM_NArticleView_GetDefColor);

   EMail       = TxBuff(&AD->EMail);
   URL[0]      = '\0';
   TxSize(&Tx) = sizeof(URL);
   TxLen(&Tx)  = 0;
   TxBuff(&Tx) = URL;

   DoMethod(o, SWM_NDFilter_TBClear, &Tx);

   Obj0   = TxtObj(o, idata, EMail, &Tx,
                   AD->RefType == RefType_MsgID ? 
                      idata->SWA_NDF_EMail_MsgIDUseColor :
                      idata->SWA_NDF_EMailUseColor,
                   AD->RefType == RefType_MsgID ? PEN_MsgID : PEN_EMail,
                   DefFont, DefColor, AD->RefType);

   DoMethod(o, SWM_NDFilter_TBClear, &Tx);

   Obj1 = TxtObj(o, idata, EMail, &Tx, TRUE,
                 AD->RefType == RefType_MsgID ? PEN_SelMsgID : PEN_SelEMail,
                 DefFont, DefColor, AD->RefType);

   // -- set variables appropriate for this type -----------------------------

   if (AD->RefType == RefType_MsgID)
      Type   = "application/X-MsgID",
      Var    = "MsgID",
      Action = NULL;
   else
      Type   = "application/X-email",
      Var    = "email",
      Action = idata->SWA_NDF_EMail_Action;

   // -- make the clickable thingamajig --------------------------------------
   
   Obj = AV_TextCmdClass,
            AppBaseDefTop(idata),
            SWA_TextHide_UI,        FALSE,
            SWA_TextHide_Obj0,      Obj0,
            SWA_TextHide_Obj1,      Obj1,
            SWA_TextCmd_Art,        AD->Art,
            SWA_TextCmd_Grp,        AD->Grp,
            SWA_TextCmd_Type,       Type,
            SWA_TextCmd_Action,     Action,
            SWA_TextCmd_Var,        Var,
            SWA_TextCmd_Val,        EMail,
            SWA_TextCmd_NeedDouble, idata->SWA_NDF_EMail_DClick,
            SWA_TextCmd_UsePopMenu, idata->SWA_NDF_EMail_PopMenus,
         End;

   if (!Obj) SWDB_RET(NULL);

   // -- if this is a MsgID, set action to view it ---------------------------
   
   if (AD->RefType == RefType_MsgID)
      DoMethod(Obj, MUIM_Notify, SWA_TextCmd_Activated, TRUE,
               o, 3, SWM_NDF_EMail_ViewMsgID, Obj, AD->Grp);
   
   SWDB_RET(Obj);
} SDFT


/*****************************************************************************
 * Method: Dispose add object                                                *
 *****************************************************************************/

METHOD(SWM_NDFilter_RemAddObj, SWP_NDFilter_RemAddObj *)
{
   AddData *AD = (AddData *)msg->Data;

   if (!AD) SWDB_RET(0);

   DoMethod(o, SWM_NDFilter_TBDispose, &AD->EMail);

   NDF_FV(AD);
   
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * Method: Show UI for this object                                           *
 *****************************************************************************/

# define SWF_FilterUIF
METHOD(SWM_NDFilter_UI, Msg)
{
   idata->UI = SWAttrUI(SW_ATTRMAP, SW_MSGSTR, SW_HELPMAP, o);

   SWP_NDF_FltToggle(idata, SWA_NDF_EMail);
   SWP_NDF_FltToggle(idata, SWA_NDF_EMail_MsgID);

   SWDB_RET(idata->UI);
} SDFT



/*****************************************************************************
 * Method: View a given message ID                                           *
 *****************************************************************************/

METHOD(SWM_NDF_EMail_ViewMsgID, SWP_NDF_EMail_ViewMsgID *)
{
   STRPTR MsgID, s = idata->MsgID;
   
   if (!msg->Bt || !msg->Grp) SWDB_RET(FAILURE);

   MsgID = SWGetStr(msg->Bt, SWA_TextCmd_Val);

   if (SW_STRISNULL(MsgID)) SWDB_RET(FAILURE);

   if (*MsgID != '<')
      *s++ = '<';

   strncpy(s, MsgID, sizeof(idata->MsgID)-3)
      [sizeof(idata->MsgID)-3]='\0';

   if (*MsgID != '<')
      strcat(s, ">");
   
   DoMethod(NN_App(idata), MUIM_Application_PushMethod,
            msg->Grp, 4, SWM_NGroup_ViewMsgID,
            SWT_NGroup_VMID_MsgID, idata->MsgID,
            TAG_DONE);

   SWDB_RET(SUCCESS);

} SDFT
