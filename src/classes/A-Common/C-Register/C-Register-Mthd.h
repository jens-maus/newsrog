
/*****************************************************************************
 * Function: See if this is a pirated keyfile                                *
 *****************************************************************************/

static __inline ULONG IsPirated(SW_IDATA *idata)
{
   if ((idata->SWA_RegID + SWV_KCOffset) == (910123 + SWV_KCOffset) ||
       (idata->SWA_RegID + SWV_KCOffset) == (910208 + SWV_KCOffset))
      return (ULONG)SWV_IsPirate;

   return FALSE;
}


/*****************************************************************************
 *   Generate registration status line                                       *
 *****************************************************************************/

static __inline ULONG GenRegStatus(SW_IDATA *idata)
{
   switch (idata->SWA_Registered) {
     case REG_NotRegistered:
      strcpy(idata->SWA_RegLine, "Unregistered");
      break;
      
     case REG_Registered:
      if (idata->SWA_KFSerOnly)
         sprintf(idata->SWA_RegLine, "(#%08ld)", idata->SWA_RegID);
      else
         sprintf(idata->SWA_RegLine, "%s %s (#%08ld)",
                 idata->SWA_RegFirstName,
                 idata->SWA_RegLastName,
                 idata->SWA_RegID);
      break;
      
     case REG_InvalidKey:
      strcpy(idata->SWA_RegLine, "Invalid Key");
      break;

     default:
      strcpy(idata->SWA_RegLine, "Unknown");
   }

   return (ULONG)idata->SWA_RegLine;
}


/*****************************************************************************
 *   Get next non-space character from a file                                *
 *****************************************************************************/

static LONG Val = SWF_CryptInitVal;

static __inline void CryptResetVal(void) { Val = SWF_CryptInitVal; }


static LONG FSkipSpaceGetC(BPTR fp)
{
   LONG c;

   while (c = FGetC(fp))
      if (c != ' ' && c != '\n' && c != '\t')
         return c;

   return -1;
}


/*****************************************************************************
 *   Get nybble value from a crypt file                                      *
 *****************************************************************************/

static LONG GetCryptNibble(LONG Char, LONG APos)
{
   static UBYTE *Array[SWF_KeyArrayNum] =
      { SWF_KeyArray1, SWF_KeyArray2, SWF_KeyArray3 };

   LONG x;

   if (APos < 0 || APos > 2) return -1;
   
   for (x=0; Array[APos][x]; x++)
      if (Array[APos][x] == (UBYTE)Char)
            return x;

   return -1;
}


/*****************************************************************************
 *   Read next unencrypted char from a file                                  *
 *****************************************************************************/

static LONG ReadCryptChar(BPTR fp, ULONG p1, ULONG p2, ULONG p3)
{
   LONG  Nyb1, Nyb2;
   ULONG InChar1, InChar2;
   ULONG v1 = PsRand(Val, p1+61,  p2*4,     p3+199);
   ULONG v2 = PsRand(Val, p1+5,   p2+871,   p3+12341);
   ULONG v3 = PsRand(Val, p1+75,  p2*3,     p3*99);

   if (((InChar1 = FSkipSpaceGetC(fp)) < 0) ||
       ((InChar2 = FSkipSpaceGetC(fp)) < 0)) return -1;

   if ((Nyb1 = GetCryptNibble(InChar1, v1%3)) < 0 ||
       (Nyb2 = GetCryptNibble(InChar2, v3%3)) < 0) return -1;

   return (LONG)((Nyb1 | (Nyb2<<4)) ^ (v2&0xff));
}


/*****************************************************************************
 *   Check valid bounds for registration number                              *
 *****************************************************************************/


static LONG CheckRegID(LONG regnum)
{
   return (regnum >= SWF_MinRegID && regnum <= SWF_MaxRegID)
      ? SUCCESS : FAILURE;
}


/*****************************************************************************
 *   Read next parameter (unencrypted) from a crypt file                     *
 *****************************************************************************/

static UBYTE *ReadCryptParam(BPTR fp, ULONG regnum, ULONG first)
{
   static ULONG v1, v2, v3;
   static UBYTE KeyLine[SWF_CryptLineSize];
   ULONG  linepos = 0;
   LONG   c, x;
   
   if (first)
      v1 = regnum*991+21334, v2=regnum*123+81, v3=regnum*77+12312351;

   while ((c = ReadCryptChar(fp, v1+=1, v2+=2, v3+=3)) >= 0) {
      if (c != '\n') KeyLine[linepos++] = c;
      else {
         KeyLine[linepos] = '\0';
         
         for (x=0; x < 16-(LONG)strlen(KeyLine); x++)
            if (ReadCryptChar(fp, v1+=1, v2+=2, v3+=3) < 0)
               return NULL;

         return KeyLine;
      }
   }

   return NULL;
}


/*****************************************************************************
 *   Find unencrypted registration ID number                                 *
 *****************************************************************************/

static LONG FindCryptID(BPTR fp)
{
   LONG a, b;

   return ((a=ReadCryptChar(fp, SWF_KeyV1, SWF_KeyV2, SWF_KeyV3))     << 0  |
           (b=ReadCryptChar(fp, SWF_KeyV1+a*7, SWF_KeyV2, SWF_KeyV3)) << 8  |
           ReadCryptChar(fp, SWF_KeyV1, SWF_KeyV2+b*99, SWF_KeyV3)    << 16);
}


/*****************************************************************************
 *   Find start of crypt file                                                *
 *****************************************************************************/

static ULONG FindCryptBegin(BPTR fp)
{
   UBYTE KeyLine[SWF_CryptLineSize];

   CryptResetVal();

   while (FGets(fp, KeyLine, SWF_CryptLineSize-1))
      if (strstr(KeyLine, ":Key:"))
         return TRUE;

   return FALSE;
}


/*****************************************************************************
 *   Method: check for key file                                              *
 *****************************************************************************/

METHOD(SWM_RegCheck, SWP_CheckReg *)
{
   BPTR   fp;
   UBYTE *s;

   idata->SWA_Registered = REG_NotRegistered;

   if (SW_STRISNULL(msg->Name)) goto Done;

   if (fp = Open(msg->Name, MODE_OLDFILE)) {

      idata->SWA_Registered = REG_InvalidKey;

      if (!FindCryptBegin(fp)) goto Done;

      if (CheckRegID(idata->SWA_RegID = FindCryptID(fp)) != SUCCESS)
         goto Done;

      if (IsPirated(idata))
        goto Done;

      if (!(s = ReadCryptParam(fp, idata->SWA_RegID, TRUE)))  goto Done;
      strncpy(idata->SWA_RegFirstName, s, SWF_CryptLineSize);

      if (!(s = ReadCryptParam(fp, idata->SWA_RegID, FALSE))) goto Done;
      strncpy(idata->SWA_RegLastName, s, SWF_CryptLineSize);

      if (!(s = ReadCryptParam(fp, idata->SWA_RegID, FALSE)) ||
          Strnicmp(s, msg->Prog, strlen(msg->Prog))      ||
          strlen(msg->Prog) != strlen(s))
         goto Done;

      idata->SWA_Registered   = REG_Registered;
   }

  Done:

   if (fp) Close(fp);
   return idata->SWA_Registered;
}
   

/*****************************************************************************
 *   Method: Display the UI                                                  *
 *****************************************************************************/

METHOD(SWM_RegisterUI, SWP_RegUI *)
{
   return RGOM_RegisterUI(idata, msg);
}


/*****************************************************************************
 *   Method: Display the unregistered banner if needed                       *
 *****************************************************************************/

METHOD(SWM_Reg_Banner, SWP_RegBanner *)
{
   if (idata->SWA_Registered == REG_Registered)
      return 0;

   MUI_Request(msg->App, msg->Win, 0, NULL, "_Ok", msg->Msg);

   return 0;
}
