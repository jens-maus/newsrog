
# define SWM_USERPOSTASSIGN SetupRO(o);

/*****************************************************************************
 * Function: Alloc/Free functions for our strings                            *
 *****************************************************************************/

static void *VLAlloc(SW_IDATA *idata, ULONG s)
{
   ULONG *mem = AllocVec(s+sizeof(ULONG)*2, MEMF_ANY);

   mem[0] = SWV_VarList_Key1;
   mem[1] = SWV_VarList_Key2;

   return (void *)(mem+2);
}

static void VLFree(void *m)
{
   ULONG *mem = m;

   if (mem && mem[-1] == SWV_VarList_Key1 && mem[-2] == SWV_VarList_Key1) {
      mem[-1] = 0;
      mem[-2] = 0;
      FreeVec(mem-2);
   }
}

/*****************************************************************************
 * Function: replace a variable value in a string                            *
 *****************************************************************************/

static LONG ReplaceVal(SW_IDATA *idata, SWS_VarList_RS *RS,
                       LONG begin, LONG end, STRPTR Value)
{
   STRPTR str = RS->Buff;
   STRPTR NewVal = SW_STRDEF(Value, "");
   LONG x, endlen = strlen(str+end+1), vallen = strlen(NewVal), newsize;

   // -- if temp buffer not big enough, allocate a bigger one ----------------
   
   if (!idata->ReplaceBuff || (endlen + 1) > idata->ReplaceBuffSize) {
      FreeVec(idata->ReplaceBuff);
      idata->ReplaceBuffSize  = endlen + SWV_VarList_IncSize;
      idata->ReplaceBuff      = AllocVec(idata->ReplaceBuffSize, MEMF_ANY);

      if (!idata->ReplaceBuff)  return -1;
   }

   // -- copy rest of string into temp holding buffer ------------------------
   
   for (x=0; str[x+end+1]; x++)
      idata->ReplaceBuff[x] = str[x+end+1];
   
   idata->ReplaceBuff[x] = '\0';

   // -- if string isn`t big enough, allocate new one ------------------------

   if ((newsize = (begin + endlen + vallen + 1)) > RS->Size) {
      STRPTR news = VLAlloc(idata, newsize + SWV_VarList_IncSize);

      if (!news) return -1;      // error
      strncpy(news, str, begin); // copy old string to new
      
      if (RS->Alloc) VLFree(RS->Buff);
      RS->Buff  = str = news;
      RS->Alloc = TRUE;
      RS->Size  = newsize + SWV_VarList_IncSize;
   }

   // -- copy replacement for variable ---------------------------------------

   strcpy(str+begin, NewVal);
   strcpy(str+begin+vallen, idata->ReplaceBuff);

   return begin+vallen;  // return next start position
}


/*****************************************************************************
 * Method: Write object to disk                                              *
 *****************************************************************************/

# define SWM_WRITE_F
METHOD(SWM_Write, SWP_Write *)
   SWDB_OBJMSG(__FUNC__, o)
{
   ULONG PrefsVer, Count = 0, x;
   Object *obj;
   
   if (SWWriteAttrs(SW_ATTRMAP, (UBYTE *)idata, msg->fp, SW_CLIB))
      SWDB_RET(FAILURE);

   PrefsVer = 1;

   SW_MUIListLoop(o, x, obj, TRUE)
      if (!SWGetUL(obj, SWA_VarList_RO))
         Count++;
   
   if (SW_WRITE(msg->fp, PrefsVer) ||
       SW_WRITE(msg->fp, Count)) SWDB_RET(NULL);

   SW_MUIListLoop(o, x, obj, TRUE)
      if (!SWGetUL(obj, SWA_VarList_RO))
         DoMethodA(obj, msg);
   SWDB_RET(DoSuperMethodA(cl, o, msg));
} SDFT


/*****************************************************************************
 * Function: Get some func buff size (at least)                              *
 *****************************************************************************/

static ULONG GetFnBuffSize(SW_IDATA *idata, LONG Size)
{
   STRPTR Old = idata->FnBuff;
   
   if (Size <= idata->FnBuffSize) return SUCCESS;

   idata->FnBuffSize = Size + 4;

   if (!(idata->FnBuff = AllocVec(idata->FnBuffSize + 2, MEMF_ANY)))
      return FAILURE;

   if (Old) {
      strcpy(idata->FnBuff, Old);
      FreeVec(Old);
   }

   return SUCCESS;
}


/*****************************************************************************
 * Function: Compare two variables for sorting                               *
 *****************************************************************************/

static LONG CmpVars(Object *o1, Object *o2)
{
   LONG nv1 = SWGetLong(o1, SWA_VarList_RO),
        nv2 = SWGetLong(o2, SWA_VarList_RO);

   return nv1 != nv2 ? (nv1 - nv2) :
      strcmp(SWGetStr(o1, SWA_VarList_Var), SWGetStr(o2, SWA_VarList_Var));
}


/*****************************************************************************
 * Function: date format hook                                                *
 *****************************************************************************/

static __saveds __asm void DateFmtFn(a0 struct Hook *hook, a1 ULONG c)
{
   SW_IDATA *idata = (SW_IDATA *)hook->h_Data;
   putreg(REG_A4, idata->A4);

   idata->FnBuff[idata->BuffPos++] = (UBYTE)c;
}


/*****************************************************************************
 * Function: check for a date variable                                       *
 *****************************************************************************/

static STRPTR GetDateVar(SW_IDATA *idata, STRPTR Name)
{
   ULONG x;
   struct DateStamp ds;

   for (x=0; DateVar[x].VarName; x++)
      if (!strcmp(DateVar[x].VarName, Name))
         break;

   if (!DateVar[x].VarName) return NULL;

   idata->BuffPos = 0;

   DateStamp(&ds);
   FormatDate(idata->loc, DateVar[x].Template, &ds, &idata->DateHook);

   return idata->FnBuff;
}


/*****************************************************************************
 * Function: Get memory variable                                             *
 *****************************************************************************/

static STRPTR GetMemVar(SW_IDATA *idata, STRPTR Name)
{
   ULONG x;

   for (x=0; MemVar[x].VarName; x++)
      if (!strcmp(MemVar[x].VarName, Name))
         break;

   if (!MemVar[x].VarName) return NULL;

   sprintf(idata->FnBuff, "%ld", AvailMem(MemVar[x].Tag) / MemVar[x].Divisor);

   return idata->FnBuff;
}


/*****************************************************************************
 * Function: skip spaces                                                     *
 *****************************************************************************/

static STRPTR SkipSp(STRPTR s)
{
   while (*s == ' ') s++;

   return s;
}


/*****************************************************************************
 * Function: Copy param into buffer (or skip, if Buff == NULL)               *
 *****************************************************************************/

static STRPTR CpParam(STRPTR s, STRPTR Buff, ULONG BuffLen)
{
   ULONG BPos = 0;
   UBYTE TermChr = ' ';
   
   if (!s) return NULL;

   // -- work out where to stop ----------------------------------------------
   
   if (*s == '"')  TermChr = '"',  s++;
   if (*s == '\'') TermChr = '\'', s++;

   // -- copy quoted string --------------------------------------------------
   
   for (; *s && *s != TermChr; s++) {
      if (*s == '\\' && s[1])
         s++;
      if (Buff && BPos < (BuffLen-1)) Buff[BPos++] = *s;
   }

   // -- terminate buffer ----------------------------------------------------
   
   if (Buff) Buff[BPos] = '\0';

   if (TermChr != ' ' && !*s)
      if (Buff)
         Buff[0] = '\0';

   if (*s == '"' || *s == '\'') s++;
   
   return s;
}


/*****************************************************************************
 * find length of parameter at this position                                 *
 *****************************************************************************/

static LONG ParamLen(STRPTR s)
{
   STRPTR end = CpParam(s, NULL, 0);

   return end - s;
}


/*****************************************************************************
 * Function: Find a named parameter in a var name                            *
 *****************************************************************************/

static STRPTR FindParam(STRPTR s, STRPTR Param, STRPTR Buff, ULONG BuffLen,
                        STRPTR Def)
{
   ULONG  PLen = strlen(Param);
   
   if (!Buff) return NULL;
   if (!s)    return NULL;

   Buff[0] = '\0';
   
   // -- parse line ----------------------------------------------------------
   
   while (s && *s) {
      s = SkipSp(s);

      // -- found parameter?  copy it into the buffer. -----------------------
      
      if (Strnicmp(s, Param, PLen) == 0 && s[PLen] == '=') {
         CpParam(s + PLen + 1, Buff, BuffLen);
         return Buff;
      }

      // -- otherwise, skip next param ---------------------------------------

      for (; *s && *s != '=' && *s != ' '; s++);

      if (*s == '=') s = CpParam(s+1, NULL, 0);
      else           s++;
   }

   if (Def) strcpy(Buff, Def);

   return NULL;
}


/*****************************************************************************
 * Function: Find parameter as a number                                      *
 *****************************************************************************/

static LONG FindParamNum(STRPTR s, STRPTR Param, LONG Min, LONG Def, LONG Max)
{
   TEXT Num[8];
   STRPTR p = FindParam(s, Param, Num, sizeof(Num), NULL);
   LONG Val = p ? atol(p) : Def;

   return min(max(Val, Min), Max);
}


/*****************************************************************************
 * Function: Find parameter as a character                                   *
 *****************************************************************************/

static UBYTE FindParamChr(STRPTR s, STRPTR Param, UBYTE Def)
{
   TEXT c[2];

   FindParam(s, Param, c, sizeof(c), NULL);

   if (!c[0]) c[0] = Def;
   
   return c[0];
}


/*****************************************************************************
 * Function: Find part after last named param: ${foo bar=5 test=3 the rest}  *
 *****************************************************************************/

static STRPTR FindRest(STRPTR s, ULONG *Quoted)
{
   STRPTR t, eq;
   ULONG  Param;

   for (; *s && *s != ' '; s++); // skip command name

   while (s && *s) {
      s = SkipSp(s);
      
      for (t=s, Param=FALSE; *t && *t!=' '; t++)
         if (*t == '=') Param = TRUE, eq = t+1;

      if (!Param) {
         if (Quoted) *Quoted = (*s=='\'' || *s=='"');

         return s;
      }
      
      s = CpParam(eq, NULL, 0);
   }

   if (Quoted) *Quoted = (*s=='\'' || *s=='"');
   return s;
}


/*****************************************************************************
 * Function: nuke quote chars                                                *
 *****************************************************************************/

static STRPTR RemQuoteChr(STRPTR s, ULONG Quoted)
{
   if (Quoted) s[strlen(s)-1] = '\0';

   return s;
}

static STRPTR SkipQuoteChr(STRPTR s, ULONG Quoted)
{
   return Quoted ? s+1 : s;
}


/*****************************************************************************
 * Function: Basic boolean logic                                             *
 *****************************************************************************/

static STRPTR FnBool(SW_IDATA *idata, STRPTR s, ULONG op)
{
   STRPTR s1, s2, e1, e2;
   LONG   v1, v2, res = FALSE;

   if (!(*(s1 = SkipSp(s)))) return NULL;            // skip whitespace

   e1 = CpParam(s1, NULL, 0);

   if (op != SWV_VL_Not) {
      if (!*e1) return NULL;
      if (!(*(s2 = SkipSp(e1)))) return NULL;        // skip whitespace
      e2 = CpParam(s2, NULL, 0);                     // end of 2nd word
   } else e2 = e1, s2 = s1;

   if (*SkipSp(e2)) return NULL;                     // too many params?

   v1 = atol(s1), v2 = atol(s2);

   // -- numeric comparison --------------------------------------------------
   
   if (ISDIGSGN(s1[0]) && (ISDIGSGN(s2[0]) || op == SWV_VL_Not)) {
      switch (op) {
         case SWV_VL_GT:  res = v1 >  v2; break;
         case SWV_VL_LT:  res = v1 <  v2; break;
         case SWV_VL_GE:  res = v1 >= v2; break;
         case SWV_VL_LE:  res = v1 <= v2; break;
         case SWV_VL_EQ:  res = v1 == v2; break;
         case SWV_VL_NE:  res = v1 != v2; break;
         case SWV_VL_Not: res = !v1; break;
      }
   } else {
      UBYTE c1, c2;

      c1  = *e1,  c2  = *e2;
      *e1 = '\0', *e2 = '\0';
      res = strcmp(s1, s2);
      *e1 = c1, *e2 = c2;

      switch (op) {
         case SWV_VL_GT:  res = res > 0;   break;
         case SWV_VL_LT:  res = res < 0;   break;
         case SWV_VL_GE:  res = res >= 0;  break;
         case SWV_VL_LE:  res = res <= 0;  break;
         case SWV_VL_EQ:  res = res == 0;  break;
         case SWV_VL_NE:  res = res != 0;  break;
         case SWV_VL_Not: return NULL;
      }
   }

   idata->FnBuff[0] = res ? '1' : '0';
   idata->FnBuff[1] = '\0';

   return idata->FnBuff;
}


/*****************************************************************************
 * Function: Basic maths                                                     *
 *****************************************************************************/

static STRPTR FnMath(SW_IDATA *idata, STRPTR s, ULONG op)
{
   LONG PCount = 0, Res = 0, x;

   // -- set up initial value ------------------------------------------------
   
   switch (op) {
     case SWV_VL_And:
     case SWV_VL_Mul: Res = 1; break;
   }

   // -- do the math ---------------------------------------------------------
   
   while (*s) {
      s = SkipSp(s);

      if (!*s) break;
         
      if (!ISDIGSGN(*s)) return NULL;

      x = atol(s);                // get value

      while (*s && *s != ' ') s++; // skip parameter

      switch (op) {
        case SWV_VL_Add: Res = !PCount ? x : Res + x; break;
        case SWV_VL_Sub: Res = !PCount ? x : Res - x; break;
        case SWV_VL_Mul: Res = !PCount ? x : Res * x; break;
        case SWV_VL_Max: Res = !PCount ? x : max(Res, x); break;
        case SWV_VL_Min: Res = !PCount ? x : min(Res, x); break;
        case SWV_VL_Div: if (!x) return NULL;
                         Res = !PCount ? x : Res / x;
                         break;
        case SWV_VL_And: Res = Res && x; break;
        case SWV_VL_Or:  Res = Res || x; break;
        case SWV_VL_Mod: if (PCount) {
           if (!x) return NULL;
           Res = Res % x;
           break;
        } else Res = x;
      }

      PCount++;
   }

   if (op == SWV_VL_Mod && PCount != 2) return NULL;
   
   if (op == SWV_VL_Sub && PCount == 1) Res = -Res;

   sprintf(idata->FnBuff, "%ld", Res);

   return idata->FnBuff;
}


/*****************************************************************************
 * Function: return particular word given ordinal position                   *
 *****************************************************************************/

static STRPTR GetWord(SW_IDATA *idata, STRPTR s, ULONG Pos, ULONG Err)
{
   ULONG Word = 0;
   STRPTR wpos;

   while (TRUE) {
      wpos = s = SkipSp(s);
      
      if (!*s || Word++ >= Pos) break;

      s = CpParam(s, NULL, 0);

      if (!*s) { wpos = s; break; }
   }

   if (!*wpos && Err) return NULL;
   
   if (GetFnBuffSize(idata, ParamLen(wpos)) != SUCCESS)
      return NULL;

   CpParam(wpos, idata->FnBuff, idata->FnBuffSize);

   return idata->FnBuff;
}

# ifdef PROMPTFN

/*****************************************************************************
 * Function: return first true condition                                     *
 *****************************************************************************/

static STRPTR Prompt(SW_IDATA *idata, STRPTR s, Object *SearchObj)
{
   TEXT Var[64];
   TEXT Type[8];
   TEXT Def[256];
   STRPTR OldVal, Rest;
   
   FindParam(Name, "VAR",  Var,  sizeof(Var), NULL);
   FindParam(Name, "TYPE", Type, sizeof(Type), "String");
   FindParam(Name, "DEF",  Def,  sizeof(Def), NULL);

   Rest = FindRest(s, NULL);

   // -- return existing variable value --------------------------------------
   
   if (Var[0] &&
       (OldVal = DoMethod(SrchObj, SWM_VarList_Search, Name+6, SrchObj))) {
      if (GetFnBuffSize(idata, strlen(OldVal)+1) != SUCCESS)
         return NULL;
         
      return strcpy(idata->FnBuff, OldVal);
   }

   // -- build a prompt window -----------------------------------------------
}

# endif


/*****************************************************************************
 * Function: return first true condition                                     *
 *****************************************************************************/

static STRPTR Cond(SW_IDATA *idata, STRPTR s)
{
   ULONG Found;
   
   idata->FnBuff[0] = '\0';
   
   while (TRUE) {
      if (!(*(s = SkipSp(s)))) break;

      Found = atol(s);
      if (!(*(s = CpParam(s, NULL, 0)))) break;  // skip condition number
      s = SkipSp(s);                             // skip spaces

      if (Found) {
         if (GetFnBuffSize(idata, ParamLen(s)) != SUCCESS)
            return NULL;

         CpParam(s, idata->FnBuff, idata->FnBuffSize);
         break;
      }
   }
   
   return idata->FnBuff;
}


/*****************************************************************************
 * Function: return word count                                               *
 *****************************************************************************/

static STRPTR WordCount(SW_IDATA *idata, STRPTR s)
{
   ULONG Words = 0;

   while (TRUE) {
      if (!(*(s = SkipSp(s)))) break;

      Words++;
      
      s = CpParam(s, NULL, 0);
   }
   
   sprintf(idata->FnBuff, "%ld", Words);

   return idata->FnBuff;
}


/*****************************************************************************
 * Function: return abbreviation for string                                  *
 *****************************************************************************/

static STRPTR MkAbbrev(SW_IDATA *idata, STRPTR s)
{
   TEXT    Sep[16], Term[16], Skip[80];
   ULONG   OutPos = 0, SepLen, AddSep = FALSE, Quoted;

   FindParam(s, "SEP",  Sep,  sizeof(Sep), NULL);
   FindParam(s, "TERM", Term, sizeof(Term), NULL);
   FindParam(s, "SKIP", Skip, sizeof(Skip), NULL);
   SepLen = strlen(Sep);

   s = SkipQuoteChr(FindRest(s, &Quoted), Quoted);
   
   while (TRUE) {
      if (!(*(s = SkipSp(s)))) break;

      if (*s == '"' || *s == '\'') s++;    // skip quotes
      if (!*s) break;

      if (AddSep) {
         if (GetFnBuffSize(idata, OutPos + SepLen + 82) != SUCCESS)
            return NULL;
         strcpy(idata->FnBuff + OutPos, Sep);
         OutPos += SepLen;
      }

      if (!strchr(Skip, s[0])) {
         idata->FnBuff[OutPos++] = s[0];
         AddSep                  = TRUE;
      } else AddSep = FALSE;

      s = CpParam(s, NULL, 0);
   }

   strcpy(idata->FnBuff + OutPos, Term);
   
   return idata->FnBuff;
}
      


/*****************************************************************************
 * Function: Get Function variable                                           *
 *****************************************************************************/

static STRPTR GetFnVar(Object *o, SW_IDATA *idata, STRPTR Name,
                       Object *SrchObj)
{
   ULONG Quoted;

   // -- handle bytes formatting ---------------------------------------------
   
   if (!strncmp(Name, "bytes ", 4)) {
      STRPTR siq = SkipQuoteChr(FindRest(Name, &Quoted), Quoted);
      return SWFmtSize(atol(siq), idata->FnBuff,
                       FindParamNum(Name, "dig", 0, 2, 3));
   }

   // -- handle substring ----------------------------------------------------

   if (!strncmp(Name, "substr ", 7)) {
      STRPTR siq  = SkipQuoteChr(FindRest(Name, &Quoted), Quoted);
      ULONG Left  = FindParamNum(Name, "LEFT",  0, 0, LONG_MAX),
            Len   = FindParamNum(Name, "LEN", 0, strlen(siq), LONG_MAX);

      if (GetFnBuffSize(idata, Len+1) != SUCCESS)
         return NULL;
      
      strncpy(idata->FnBuff, siq + min(Left, strlen(siq)), Len)[Len] = '\0';

      return idata->FnBuff;
   }
      
   // -- see if variable exists ----------------------------------------------
   
   if (!strncmp(Name, "bound ", 6)) {
      sprintf(idata->FnBuff, "%ld",
              DoMethod(SrchObj, SWM_VarList_Search, Name+6, SrchObj) ? 1 : 0);

      return idata->FnBuff;
   }

   // -- see if variable exists ----------------------------------------------
   
   if (!strncmp(Name, "default ", 6)) {
      TEXT  Var[64];
      STRPTR s, siq = SkipQuoteChr(FindRest(Name, &Quoted), Quoted);

      FindParam(Name, "VAR", Var, sizeof(Var), NULL);

      if (!Var[0]) return NULL;
      
      if (s = (STRPTR)DoMethod(SrchObj, SWM_VarList_Search, Var, SrchObj)) {
         if (GetFnBuffSize(idata, strlen(s)+1) != SUCCESS)
            return NULL;
         
         return strcpy(idata->FnBuff, s);
      }

      if (GetFnBuffSize(idata, strlen(siq)+1) != SUCCESS)
         return NULL;
      
      strcpy(idata->FnBuff, siq);
      goto RetRemQQ;
   }

   // -- handle padding ------------------------------------------------------
   
   if (!strncmp(Name, "pad ", 4)) {
      STRPTR PadStr = FindRest(Name, &Quoted);
      UBYTE  PadChr = FindParamChr(Name, "CHAR", ' ');
      LONG   Len    = FindParamNum(Name, "LEN", 0, 4, 32768),
             x;
      
      if (GetFnBuffSize(idata, Len+1) != SUCCESS)
            return NULL;

      if ((x=(strlen(PadStr) - (Quoted ? 2 : 0))) >= Len) {
         strcpy(idata->FnBuff, SkipQuoteChr(PadStr,Quoted));

         goto RetRemQQ;
      }

      Len -= x;
      
      for (x=0; x<Len; x++)
         idata->FnBuff[x] = PadChr;

      strcpy(idata->FnBuff+x, SkipQuoteChr(PadStr, Quoted));
      
      goto RetRemQQ;
   }

   // -- remchars ------------------------------------------------------------
   
   if (!strncmp(Name, "remchars ", 9)) {
      TEXT  Chr[32];
      ULONG x, Max, rep;
      UBYTE c;

      FindParam(Name, "CHARS", Chr, sizeof(Chr), " ");
         
      Max = FindParamNum(Name, "MAX", 0, 0, 256);

      Name = SkipQuoteChr(FindRest(Name, &Quoted), Quoted);

      if (GetFnBuffSize(idata, strlen(Name)+1) != SUCCESS)
         return NULL;

      for (x=0; *Name && (!Quoted || Name[1]);)
         if (!strchr(Chr, *Name))
            idata->FnBuff[x++] = *Name++;
         else {
            for (c = *Name, rep = 0; rep < Max && *Name == c; rep++)
               idata->FnBuff[x++] = *Name++;

            while (*Name == c) Name++;
         }

      idata->FnBuff[x] = '\0';

      return idata->FnBuff;
   }


   // -- charcount -----------------------------------------------------------
   
   if (!strncmp(Name, "charcount ", 10)) {
      TEXT Chr[32];
      ULONG  Count;

      FindParam(Name, "CHARS", Chr, sizeof(Chr), " ");

      Name = SkipQuoteChr(FindRest(Name, &Quoted), Quoted);

      for (Count=0; *Name && (!Quoted || Name[1]); Name++)
         if (strchr(Chr, *Name))
            Count++;

      sprintf(idata->FnBuff, "%ld", Count);

      return idata->FnBuff;
   }

   // -- word ----------------------------------------------------------------
   
   if (!strncmp(Name, "word ", 5))
      return GetWord(idata, FindRest(Name, &Quoted),
                     FindParamNum(Name, "POS", 0, 0, 256),
                     FALSE);

   // -- cond ----------------------------------------------------------------
   
   if (!strncmp(Name, "cond ", 5))
      return Cond(idata, Name+5);

   // -- word ----------------------------------------------------------------
   
   if (!strncmp(Name, "wordcount ", 10))
      return WordCount(idata, Name+10);

   // -- if ------------------------------------------------------------------
   
   if (!strncmp(Name, "if ", 3))
      return GetWord(idata, Name+3, atol(Name+3) ? 1 : 2, FALSE);

   // -- abbrev --------------------------------------------------------------
   
   if (!strncmp(Name, "abbrev ", 7))
      return MkAbbrev(idata, Name);

   // -- repeat --------------------------------------------------------------
   
   if (!strncmp(Name, "repeat ", 7)) {
      STRPTR Str   = SkipQuoteChr(FindRest(Name, &Quoted), Quoted);
      ULONG  Count = FindParamNum(Name, "COUNT", 0, 2, 32768),
             SLen  = strlen(Str),
             x, BLen;

      if (GetFnBuffSize(idata, Count * (SLen+1)) != SUCCESS)
         return NULL;

      for (x=BLen=0; x<Count; x++) {
         strcpy(idata->FnBuff + BLen, Str);
         BLen += SLen;
      }

      return idata->FnBuff;
   }

   // -- strlen --------------------------------------------------------------
   
   if (!strncmp(Name, "concat ", 7)) {
      STRPTR s = FindRest(Name, &Quoted);
      TEXT Sep[32];
      ULONG Pos = 0;

      FindParam(Name, "SEP", Sep, sizeof(Sep), NULL);

      idata->FnBuff[0] = '\0';

      while (s && *s) {
         if (GetFnBuffSize(idata, Pos + ParamLen(s) + sizeof(Sep)) != SUCCESS)
            return NULL;
         
         s = CpParam(s, idata->FnBuff + Pos, idata->FnBuffSize - Pos);
         s = SkipSp(s);
         
         Pos += strlen(idata->FnBuff + Pos);

         if (*s)
            Pos += strlen(strcpy(idata->FnBuff + Pos, Sep));
      }

      return idata->FnBuff;
   }

   // -- replace -------------------------------------------------------------
   
   if (!strncmp(Name, "replace ", 8)) {
      STRPTR old = AllocVec(256, MEMF_ANY), new = AllocVec(256, MEMF_ANY);
      STRPTR s = FindRest(Name, &Quoted);
      LONG x, oldsize, newsize, RCount;
      ULONG  All = FindParamNum(Name, "ALL", 0, 1, 1);
      STRPTR Res = NULL;

      if (!old || !new) goto ReplaceDone;

      FindParam(Name, "OLD", old, 256, "%s");
      FindParam(Name, "NEW", new, 256, NULL);

      if (!old[0]) goto ReplaceDone;

      oldsize = strlen(old);
      newsize = strlen(new);
      
      for (x=RCount=0; *s; ) {
         if (strncmp(s, old, oldsize)==0 && (!RCount || All)) {
            if (GetFnBuffSize(idata, x + newsize + 2) != SUCCESS)
                return NULL;

            strcpy(idata->FnBuff+x, new);
            x += newsize;
            s += oldsize;
            RCount++;
         } else
            idata->FnBuff[x++] = *s++;
      }

      (Res = idata->FnBuff)[x] = '\0';

     ReplaceDone:
      
      FreeVec(old);
      FreeVec(new);

      return Res;
   }

      
   // -- strlen --------------------------------------------------------------
   
   if (!strncmp(Name, "strlen ", 7)) {
      LONG Len;
      STRPTR siq = SkipQuoteChr(FindRest(Name, &Quoted), Quoted);
      
      Len = strlen(siq) - (Quoted ? 1 : 0);

      sprintf(idata->FnBuff, "%ld", Len);

      return idata->FnBuff;
   }

   // -- toupper -------------------------------------------------------------
   
   if (!strncmp(Name, "toupper ", 8) ||
       !strncmp(Name, "tolower ", 8)) {
      STRPTR siq = SkipQuoteChr(FindRest(Name, &Quoted), Quoted);
      LONG   x, up = !strncmp(Name, "toupper ", 8);

      if (GetFnBuffSize(idata, strlen(siq)) != SUCCESS)
         return NULL;
      
      for (x=0; *siq; x++, siq++)
         idata->FnBuff[x] = (up ? toupper(*siq) : tolower(*siq));

      x -= (Quoted ? 1 : 0);

      idata->FnBuff[x] = '\0';

      return idata->FnBuff;
   }


   // -- getenv --------------------------------------------------------------

   if (!strncmp(Name, "getenv ", 7)) {
      STRPTR siq = SkipQuoteChr(FindRest(Name, &Quoted), Quoted),
             Res = idata->FnBuff;
      ULONG len;

      if (Quoted) {
         if (!(len = strlen(siq))) return NULL;
         if (siq[len-1] != '"')    return NULL;
         siq[len-1] = '\0';
      }

      if (GetVar(siq, Res, idata->FnBuffSize, 0) < 0)
         if (!FindParam(Name, "DEF", Res, idata->FnBuffSize, NULL))
            Res = NULL;

      if (Quoted) siq[len-1] = '"';

      return Res;
   }

   // -- function ------------------------------------------------------------

   if (!strncmp(Name, "func ", 5)) {
      STRPTR OldFP = idata->FuncParams, Res, NameBegin, NameEnd, Params, Defn;

      NameBegin = Name + 5;
      
      if (!(NameEnd = strchr(Name+5, ' '))) return NULL;

      Params = SkipSp(NameEnd);
      *NameEnd = '\0';
      
      Defn = (STRPTR)DoMethod(SrchObj, SWM_VarList_Search, NameBegin, SrchObj);
      *NameEnd = ' ';

      if (!Defn) return NULL;
      
      idata->FuncParams = Params;

      Res = (STRPTR)DoMethod(o, SWM_VarList_ReplaceValues, Defn, SrchObj);
      
      idata->FuncParams = OldFP;

      if (!Res) return NULL;

      if (GetFnBuffSize(idata, strlen(Res)) != SUCCESS) {
         DoMethod(o, SWM_VarList_FreeString, Res, SrchObj);
         return NULL;
      }

      strcpy(idata->FnBuff, Res);

      DoMethod(o, SWM_VarList_FreeString, Res, SrchObj);

      return idata->FnBuff;
   }


   // -- basic maths ---------------------------------------------------------

   if (Name[1] == ' ') {
      if (Name[0] == '+') return FnMath(idata, Name+1, SWV_VL_Add);
      if (Name[0] == '-') return FnMath(idata, Name+1, SWV_VL_Sub);
      if (Name[0] == '*') return FnMath(idata, Name+1, SWV_VL_Mul);
      if (Name[0] == '/') return FnMath(idata, Name+1, SWV_VL_Div);
      if (Name[0] == '%') return FnMath(idata, Name+1, SWV_VL_Mod);
      if (Name[0] == '>') return FnBool(idata, Name+1, SWV_VL_GT);
      if (Name[0] == '<') return FnBool(idata, Name+1, SWV_VL_LT);
      if (Name[0] == '=') return FnBool(idata, Name+1, SWV_VL_EQ);
   }

   if (!strncmp(Name, ">= ", 3))  return FnBool(idata, Name+3, SWV_VL_GE);
   if (!strncmp(Name, "<= ", 3))  return FnBool(idata, Name+3, SWV_VL_LE);
   if (!strncmp(Name, "!= ", 3))  return FnBool(idata, Name+3, SWV_VL_NE);
   if (!strncmp(Name, "min ", 4)) return FnMath(idata, Name+4, SWV_VL_Min);
   if (!strncmp(Name, "max ", 4)) return FnMath(idata, Name+4, SWV_VL_Max);
   if (!strncmp(Name, "and ", 4)) return FnMath(idata, Name+4, SWV_VL_And);
   if (!strncmp(Name, "or ",  3)) return FnMath(idata, Name+3, SWV_VL_Or);
   if (!strncmp(Name, "not ", 4)) return FnBool(idata, Name+4, SWV_VL_Not);
   if (!strncmp(Name, "xor ", 4)) return FnBool(idata, Name+4, SWV_VL_Xor);

   return NULL;

  RetRemQQ:
   return RemQuoteChr(idata->FnBuff, Quoted);
}


/*****************************************************************************
 * Function: Get Exec variable                                               *
 *****************************************************************************/

static STRPTR GetExecVar(Object *o, SW_IDATA *idata, STRPTR Name)
{
   UBYTE out[16];
   BPTR  fp;
   TEXT  Cmd[FMSIZE],
         Arg[FMSIZE];
   LONG  Stack, Pri;

   // -- return if not an exec command ---------------------------------------
   
   if (strncmp(Name, "exec ", 5) != 0)
      return NULL;

   Name += 5;

   // -- find parameters -----------------------------------------------------

   FindParam(Name, "CMD",   Cmd,    sizeof(Cmd), NULL);
   FindParam(Name, "ARGS",  Arg,    sizeof(Arg), NULL);
   
   Stack = FindParamNum(Name, "STACK", 4192, 32000, LONG_MAX);
   Pri   = FindParamNum(Name, "PRI", -127, 0, 127);

   if (SW_STRISNULL(Cmd)) return NULL;
   
   idata->FnBuff[0] = '\0';

   sprintf(out, "PIPE:SW_%06ld", GetUniqueID());

   if (DoMethod(o, SWM_Root_SysCmd,
                SWT_Root_OutFile,  out,
                SWT_Root_Cmd,      Cmd,
                SWT_Root_Arg,      Arg,
                SWT_Root_Stack,    Stack,
                SWT_Root_Pri,      Pri,
                TAG_DONE))
      return NULL;

   if (!(fp = Open(out, MODE_OLDFILE)))
      return NULL;

   if (FGets(fp, idata->FnBuff, idata->FnBuffSize))
      while (FGets(fp, out, 15));  // read rest of file

   Close(fp);

   SW_REMOVENL(idata->FnBuff);
   
   return idata->FnBuff[0] ? idata->FnBuff : NULL;
}

/*****************************************************************************
 * Method: search for a param variable                                       *
 *****************************************************************************/


static STRPTR GetParam(Object *o, SW_IDATA *idata, STRPTR Name)
{
   STRPTR s;
   
   if (!idata->FuncParams) return NULL;

   if (Name[0] == '*' && Name[1] == '\0')
      return idata->FuncParams;

   if (Name[0] == '#' && Name[1] == '\0')
      return WordCount(idata, idata->FuncParams);

   for (s=Name; *s; s++)
      if (!ISDIG(*s))
         return NULL;
   
   return GetWord(idata, idata->FuncParams, atol(Name), TRUE);
}


/*****************************************************************************
 * Method: search for a variable                                             *
 *****************************************************************************/

METHOD(SWM_VarList_Search, SWP_VarList_Search *)
{
   Object *ValObj;
   UBYTE  *Val;

   if (!msg->Name) return NULL;

   if (Val = GetParam  (o, idata, msg->Name))           SWDB_RET((ULONG)Val);
   if (Val = GetFnVar  (o, idata, msg->Name, msg->obj)) SWDB_RET((ULONG)Val);
   if (Val = GetExecVar(o, idata, msg->Name))           SWDB_RET((ULONG)Val);
   if (Val = GetDateVar(idata, msg->Name))              SWDB_RET((ULONG)Val);
   if (Val = GetMemVar (idata, msg->Name))              SWDB_RET((ULONG)Val);

   ValObj = (Object *)DoMethod(o,SWM_NList_FindObj,SWA_VarList_Var,msg->Name);

   SWDB_RET(SWGetUL(ValObj, SWA_VarList_Val));
} SDFT


/*****************************************************************************
 * Function: search for beginning & end of next variable ${ }                *
 *****************************************************************************/

static LONG FindNextVar(Object *o, STRPTR name, STRPTR str, ULONG start,
                        LONG *end, LONG *nested)
{
   LONG startpos = -1, nesting;

   if (nested) *nested = FALSE;

   // -- search for beginning of variable ------------------------------------
   
   for (; str[start] && str[start+1]; start++) {
      if (ESCMARK  (str, start)) { start++; continue;       } // backslash esc
      if (BEGINMARK(str, start)) { startpos = start; break; } // start of var
   }

   if (startpos < 0) return startpos;  // no variable found

   // -- search for end of variable, keeping track of nesting ----------------

   for(start=startpos+2, nesting=1; str[start]; start++) {
      if (ESCMARK  (str, start)) { start++; continue;            } // backslash

      if (ENDMARK  (str, start)) {    // end variable
         if (!--nesting) break;       // if end of nesting, exit loop
         else            continue;    // else continue
      }

      if (BEGINMARK(str, start)) {    // begin variable
         nesting++; start++;          // increment nesting count
         if (nested) *nested = TRUE;  // set nested flag
         continue;                    // continue
      }
   }

   if (nesting)
      return (LONG)SWERR(o, -1, SWS_Err_UnterminatedVar, name, NULL);

   if (end) *end = start;
      
   return startpos;
}


/*****************************************************************************
 * Method: Replace escape sequences                                          *
 *****************************************************************************/

static void ReplaceEsc(SW_IDATA *idata, SWS_VarList_RS *RS)
{
   LONG x, y;
   STRPTR s = RS->Buff;

   // -- look for escaped string ---------------------------------------------
   
   for (x=0; s[x]; x++)
      if (s[x] == '\\')
         break;

   if (!s[x]) return;

   if (!(s = VLAlloc(idata, strlen(RS->Buff) + SWV_VarList_IncSize)))
      return;

   for (x=y=0; RS->Buff[y]; ) {
      if (RS->Buff[y] != '\\') {
         s[x++] = RS->Buff[y++]; continue;
      }

      if (RS->Buff[y+1]=='n')
         { s[x++] = '\n';   y+=2; continue; }
      
      if (RS->Buff[y+1]=='3' && RS->Buff[y+2]=='3' )
         { s[x++] = '\033'; y+=3; continue; }

      s[x++] = RS->Buff[++y]; y++;
   }

   s[x] = '\0';

   if (RS->Alloc) VLFree(RS->Buff);
   RS->Buff  = s;
   RS->Alloc = TRUE;
   RS->Size  = strlen(RS->Buff) + SWV_VarList_IncSize;
}

   
/*****************************************************************************
 * Method: Replace variable values in string                                 *
 *****************************************************************************/

METHOD(SWM_VarList_ReplaceValues, SWP_VarList_ReplaceValues *)
   SWDB_ASSERTNULL(msg->Str)
   SWDB_ASSERTNULL(msg->Obj)
{
   SWS_VarList_RS rs;
   LONG start, end, iter = 0, replace = TRUE, nested, NameLen;
   STRPTR Value;
   STRPTR VarName = NULL;
   ULONG VarNameMax = 0;

   rs.Alloc = FALSE;
   rs.Buff  = msg->Str;
   rs.Size  = 0;
   
   if (!rs.Buff || !msg->obj) SWDB_RET(NULL);

   DoMethod(msg->obj, SWM_ClearObjError);

   if (GetFnBuffSize(idata, SWV_VarList_FnBuffSize) != SUCCESS)
      SWDB_RET(NULL);
   
   while (replace) {
      replace = FALSE;
      start   = 0;

      if (++iter > SWV_VarList_MaxIter)
         SWDB_RET(SWERR(msg->obj, GetS(SWS_L_Error), SWS_Err_TooManyReplacements,
                        "<unnamed>", idata->BadVar));

      // -- loop through the string, replacing what we can each time ---------
      
      while ((start = FindNextVar(msg->obj, "<unnamed>", rs.Buff,
                                  start, &end, &nested)) >= 0) {
         if (nested) {         // don`t replace if nested.
            start+=2;          // just proceed inside the nesting,
            continue;          // continue to search
         }

         // -- copy variable name --------------------------------------------

         if ((NameLen = (end - start - 2)) + 2 >= VarNameMax) {
            FreeVec(VarName);
            VarNameMax = max(NameLen + 8, 64);
            if (!(VarName = AllocVec(VarNameMax, MEMF_ANY)))
               SWDB_RET(SWERR(msg->obj, GetS(SWS_L_Error), SWS_Err_Malloc,
                              NULL, NULL));
         }
         
         strncpy(VarName, rs.Buff+start+2, NameLen)[NameLen] = '\0';

         strncpy(idata->BadVar, VarName, sizeof(idata->BadVar)-1)
            [sizeof(idata->BadVar)-1] = '\0';

         // -- search for replacement with the given object/method -----------

         if (!(Value = (STRPTR)DoMethod(msg->obj, SWM_VarList_Search,
                                        VarName, msg->obj))) {
            FreeVec(VarName);
            SWDB_RET(SWERR(msg->obj, GetS(SWS_L_Error), SWS_Err_NoExpansion,
                           idata->BadVar, "<unnamed>"));
         }

         if ((start = ReplaceVal(idata, &rs, start, end, Value)) < 0) {
            FreeVec(VarName);
            SWDB_RET((ULONG)GetS(SWS_L_Error));
         }
         
         replace = TRUE;
      }
   }

   ReplaceEsc(idata, &rs);

   FreeVec(VarName);

   SWDB_RET((ULONG)rs.Buff);
} SDFT


/*****************************************************************************
 * Method: Free string if required                                           *
 *****************************************************************************/

METHOD(SWM_VarList_FreeString, SWP_VarList_FreeString *)
{
   DoMethod(msg->obj, SWM_Root_CheckErr, NULL);
   VLFree(msg->Str);
   SWDB_RET(0);
}


/*****************************************************************************
 * Setup (when this object is displayed)                                     *
 *****************************************************************************/

METHOD(MUIM_Setup, Msg)
{
   SetupRO(o);
   SWDB_RET(DoSuperMethodA(cl,o,msg));
} SDFT


/*****************************************************************************
 * Cleanup (when this object is hidden)                                      *
 *****************************************************************************/

METHOD(MUIM_Cleanup, Msg)
{
   RemoveRO(o);
   SWDB_RET(DoSuperMethodA(cl,o,msg));
} SDFT


/*****************************************************************************
 * Method: Copy parameter                                                    *
 *****************************************************************************/

METHOD(SWM_VarList_CpParam, SWP_VarList_CpParam *)
{
   SWDB_RET(CpParam(msg->Name, msg->Buff, msg->BuffLen));
}


/*****************************************************************************
 * Method: Find a parameter's value                                          *
 *****************************************************************************/

METHOD(SWM_VarList_FindParam, SWP_VarList_FindParam *)
{
   SWDB_RET(FindParam(msg->Name, msg->Param, msg->Buff, msg->BuffLen, msg->Def));
}


/*****************************************************************************
 * Method: Get numeric parameter                                             *
 *****************************************************************************/

METHOD(SWM_VarList_FindParamNum, SWP_VarList_FindParamNum *)
{
   SWDB_RET(FindParamNum(msg->Name, msg->Param, msg->Min, msg->Def, msg->Max));
}


/*****************************************************************************
 * Method: Get character parameter                                           *
 *****************************************************************************/

METHOD(SWM_VarList_FindParamChr, SWP_VarList_FindParamChr *)
{
   SWDB_RET(FindParamChr(msg->Name, msg->Param, msg->Def));
}


/*****************************************************************************
 * Method: Get rest of line after parameters                                 *
 *****************************************************************************/

METHOD(SWM_VarList_FindRest, SWP_VarList_FindRest *)
{
   SWDB_RET(FindRest(msg->Name, msg->Quoted));
}


