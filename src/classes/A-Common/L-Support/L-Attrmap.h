
/*****************************************************************************
 *   Attribute support functions: find attribute pos & pointer               *
 *****************************************************************************/

# define sa __saveds __asm

static __inline LONG SWFindPos(const SWAttrMapT Map[],
                               const ULONG Tag, const ULONG SGI)
{
   LONG x;
   
   // -- look for attribute in table -----------------------------------------

   for (x=0; Map[x].Attr; x++)
      if (Map[x].Attr == Tag)
         break;

   // -- if we found an attr, return it, otherwise return -1 -----------------

   return (Map[x].Attr && (Map[x].SGI & SGI)) ? x : -1;
}


static __inline ULONG *SWGetPtr(const SWAttrMapT Map[],
                                const UBYTE *idata,
                                ULONG x)
{
   switch (Map[x].Type) {
      case SWAM_Obj:    // fall through
      case SWAM_Int:    // fall through
      case SWAM_Bool:   // fall through
      case SWAM_StrS:   // fall through
      case SWAM_StrA:   // fall through
      case SWAM_Pen:    // fall through
      case SWAM_BackG:  // fall through
      case SWAM_Struct: return (ULONG *)(idata+Map[x].Offset);
      case SWAM_FixS:   // fall through to FixI
      case SWAM_FixI:   return (ULONG *)(&Map[x].Default);
      case SWAM_MUIA:   return (ULONG *)4;

      default:          SWDB_ERROR("internal error");
                        return NULL;
   }
}


static ULONG SWGetInt(const SWAttrMapT Map[],
                      const UBYTE *idata,
                      const LONG pos,
                      ULONG *Res)
{
   ULONG Dum;
   ULONG *p;

   if (!Res) Res = &Dum;
   
   if (pos < 0) return FALSE;
   
   if (p = SWGetPtr(Map, idata, pos)) {
      switch (Map[pos].Type) {
         case SWAM_Obj:    // fall through
         case SWAM_Int:    // fall through
         case SWAM_Bool:   *Res = *p;       break;
         case SWAM_StrS:   // fall through
         case SWAM_Pen:    // fall through
         case SWAM_Struct: *Res = (ULONG)p; break;
         case SWAM_BackG:  // fall through
         case SWAM_StrA:   *Res = *p;       break; 
         case SWAM_FixS:   // fall through
         case SWAM_FixI:   *Res = *p;       break;
         case SWAM_MUIA:   *Res = Map[pos].Default; break;
   
         default:          SWDB_ERROR("internal error");
                           return FALSE;
      }
   }

   return TRUE;
}

static void NewVal(UBYTE **x, UBYTE *val)
{
   ULONG len;

   if (SW_STRISNULL(val)) { FV(*x); *x = SWPoolNullStr(); return; }

   if (*x == val) return;
   
   len = strlen(val);

   FV(*x);
   if (*x = AV(len+1)) strcpy(*x, val);
}

static void FreeImgSpec(UBYTE **x)
{
   if (!*x) return;
   FreePooled(SWPool, *x, sizeof(MUI_ImageSpec));
   *x = NULL;
}

static void NewImgSpec(UBYTE **x, UBYTE *Data)
{
   FreeImgSpec(x);
   if (SW_STRISNULL(Data)) return;
   *x = AllocPooled(SWPool, sizeof(MUI_ImageSpec));
   if (!*x) return;
   memcpy(*x, Data, sizeof(MUI_ImageSpec));
}


static ULONG SWSetInt(const SWAttrMapT Map[],
                      const UBYTE *idata,
                      const LONG pos,
                      ULONG Data,
                      Object *o)
{
   ULONG *p;

   if (pos < 0) return FALSE;
   
   if (p = SWGetPtr(Map, idata, pos)) {
      switch (Map[pos].Type) {
         case SWAM_Obj:    {
                              Object **op = (Object **)p;

                              if (*op == (Object *)Data) break;

                              if (*op && Data) {
                                 UBYTE *c1, *c2;
                                 if (SW_SAMECLASS_SAFE(*op,
                                                       (Object *)Data, c1, c2)) {
                                    DoMethod(*op, SWM_Assign, Data);
                                    break;
                                 }
                              }
                              
                              MUI_DisposeObject(*op);
                              if (Data)
                                 *op = (Object *)DoMethod((Object *)Data,
                                                          SWM_Clone);
                              else
                                 *op = NULL;
                           }
                           break;
         
         case SWAM_Int:    if (Map[pos].SGI & SWAM_IntBnd) {  // enf bounds
                              Data = max((LONG)Data, Map[pos].Min);
                              Data = min((LONG)Data, Map[pos].Max);
                           }

                           // fall through

         case SWAM_Bool:   *p = Data; break;

         case SWAM_BackG:  NewImgSpec((UBYTE **)p, (UBYTE *)Data);
                           break;

         case SWAM_StrS:   ((UBYTE *)p)[0] = '\0';
                           if (Data) strcpy((UBYTE *)p, (UBYTE *)Data);
                           break;

         case SWAM_StrA:   NewVal((UBYTE **)p, (UBYTE *)Data);
                           break;

         case SWAM_Pen:    if (Data >= 0 && Data < MPEN_COUNT) {
                              Object *pen = PendisplayObject, End;

                              if (!pen) return FALSE;

                              DoMethod(pen, MUIM_Pendisplay_SetMUIPen, Data);

                              if (p)
                                memcpy((UBYTE *)p,
                                       SWGetStr(pen, MUIA_Pendisplay_Spec),
                                       Map[pos].Size);

                              MUI_DisposeObject(pen);
                              break;
                           }
                           // else fall through

         case SWAM_Struct: if (Data && p)
                              memcpy((UBYTE *)p, (UBYTE *)Data, Map[pos].Size);
                           break;

         case SWAM_MUIA:   set(o, Map[pos].Attr, Data);
                           break;

         default:          SWDB_ERROR("internal error");
                           return FALSE;
      }
   }

   return TRUE;
}


/*****************************************************************************
 *   Get an attribute value                                                  *
 *****************************************************************************/

sa ULONG SWGetAttr(a0 const SWAttrMapT Map[], a1 const UBYTE *idata,
                   d0 const ULONG Tag, a2 ULONG *Res)
   SDST(ULONG)
   SWP_XIX(Map, Tag, Mem)
   SWDB_ASSERTNULL(Map)
   SWDB_ASSERTNULL(idata)
   SWDB_ASSERTNULL(Res)
{
   SWDB_RET(SWGetInt(Map, idata, SWFindPos(Map, Tag, SWAM_Get), Res));
} SDFT


/*****************************************************************************
 *   Set an attribute value                                                  *
 *****************************************************************************/
   
sa ULONG SWSetAttr(a0 const SWAttrMapT Map[], a1 const UBYTE *idata,
                   d0 const ULONG Tag, d1 ULONG Data)
   SDST(ULONG)
   SWP_XIX(Map, Tag, Data)
   SWDB_ASSERTNULL(Map)
   SWDB_ASSERTNULL(idata)
{
   SWDB_RET(SWSetInt(Map, idata, SWFindPos(Map, Tag, SWAM_Set), Data, NULL));
} SDFT


/*****************************************************************************
 *   Init attribute values in instance data to defaults                      *
 *****************************************************************************/
   
sa ULONG SWInitAttrs(a0 const SWAttrMapT Map[], a1 const UBYTE *idata,
                     a2 Object *o)
   SDST(ULONG)
   SWP_XX(Map, idata)
   SWDB_ASSERTNULL(Map)
   SWDB_ASSERTNULL(idata)
{
   LONG x;
   
   for (x=0; Map[x].Attr; x++)
      if (Map[x].SGI & SWAM_Init)
         SWSetInt(Map, idata, x, Map[x].Default, o);

   SWDB_RET(TRUE);
} SDFT


/*****************************************************************************
 *   Generate a tag list of object attributes                                *
 *****************************************************************************/

sa ULONG *SWGenTagList(a0 const SWAttrMapT Map[], a1 const UBYTE  *idata,
                       a2 ULONG *tags)
{
   ULONG x, Res;

   // -- fill in array -----------------------------------------------------

   for (x=0; Map[x].Attr; x++) {
      if (SWAM_GetSettable(Map[x].SGI))
         if (SWGetInt(Map, idata, x, &Res))
            *tags++ = Map[x].Attr,
            *tags++ = Res;

      if (Map[x].Type == SWAM_MUIA)    // handle MUI init attrs
         *tags++ = Map[x].Attr,
         *tags++ = Map[x].Default;
   }

   *tags = TAG_DONE;

   return tags;
}


/*****************************************************************************
 *   Compare one object's attributes to another's                            *
 *****************************************************************************/

static LONG SWCmpInt(const ULONG Type, const ULONG Res1, const ULONG Res2,
                     const ULONG Size)
{
   switch (Type) {
     case SWAM_Obj:    return (LONG)DoMethod((Object *)Res1,SWM_Compare,Res2);
     case SWAM_Int:    // fall through
     case SWAM_Bool:   return Res1 != Res2;
     case SWAM_BackG:  // fall through
     case SWAM_StrS:   // fall through
     case SWAM_StrA:   return strcmp((UBYTE *)Res1, (UBYTE *)Res2);
     case SWAM_Pen:    // fall through
     case SWAM_Struct: return memcmp((UBYTE *)Res1, (UBYTE *)Res2, Size);
     case SWAM_FixS:   // fall through
     case SWAM_FixI:   return 0;
     default:          SWDB_ERROR("internal error");
                       return FALSE;
   }
}

sa ULONG SWCmpAttrs(a0 const SWAttrMapT Map[], a1 const UBYTE *idata,
                    a2 const UBYTE *idata2)
   SDST(ULONG)
   SWP_XXX(Map, idata, idata2)
   SWDB_ASSERTNULL(Map)
   SWDB_ASSERTNULL(idata)
   SWDB_ASSERTNULL(idata2)
{
   LONG x;
   ULONG Res1, Res2;
   
   for (x=0; Map[x].Attr; x++)
      if ((Map[x].SGI & SWAM_Get) && !(Map[x].SGI & SWAM_NoStor))
         if (SWGetInt(Map, idata,  x, &Res1) &&
             SWGetInt(Map, idata2, x, &Res2))
            if (SWCmpInt(Map[x].Type, Res1, Res2, Map[x].Size))
               SWDB_RET(1);

   SWDB_RET(0);
} SDFT


/*****************************************************************************
 *   Write object's attributes to disk                                       *
 *****************************************************************************/


static LONG SWWriteInt(const BPTR fp,
                       const ULONG Type, const ULONG Res, const ULONG Size)
{
   ULONG Len;
   
   switch (Type) {
     case SWAM_Obj:    Len = Res ? 1 : 0;
                       return SW_WRITE(fp, Len) ||
                              (LONG)DoMethod((Object *)Res, SWM_Write, fp);
     case SWAM_Int:    // fall through
     case SWAM_Bool:   return SW_WRITE(fp, (Res));

     case SWAM_BackG:  // fall through
     case SWAM_StrS:   // fall through
     case SWAM_StrA:   Len = (Res ? strlen((UBYTE *)Res) : 0) + 1;
                       return SW_WRITE(fp, Len)  ||
                              (FWrite(fp, Res ? (STRPTR)(Res):"", Len, 1) != 1);

     case SWAM_Pen:    // fall through
     case SWAM_Struct: return SW_WRITE(fp, Size) ||
                              (FWrite(fp,(STRPTR)(Res), Size, 1) != 1);

     case SWAM_FixS:   // fall through
     case SWAM_FixI:   return SUCCESS;

     default:          SWDB_ERROR("internal error");
                       return 1;
   }
}

sa ULONG SWWriteAttrs(a0 const SWAttrMapT Map[], a1 const UBYTE *idata,
                      d1 BPTR const fp, a3 UBYTE *ClassName)
   SDST(ULONG)
   SWP_XX(Map, idata)
   SWDB_ASSERTNULL(Map)
   SWDB_ASSERTNULL(idata)
   SWDB_ASSERTNULL(fp)
{
   LONG x;
   ULONG Res, pv = SWV_Support_PrefsVer;

   if (!fp) SWDB_RET(FAILURE);

   // -- first write class tag -----------------------------------------------

   if (FPuts(fp, ClassName))     SWDB_RET(FAILURE);
   FPutC(fp, '\n');

   if (SW_WRITE(fp, pv))     SWDB_RET(FAILURE);   // write prefs version

   // -- now write attributes ------------------------------------------------
   
   for (x=0; Map[x].Attr; x++)
      if (SWAM_Storable(Map[x].SGI))
         if (SWGetInt(Map, idata, x, &Res))
            if (SW_WRITE(fp,   Map[x].Attr) ||
                SWWriteInt(fp, Map[x].Type, Res, Map[x].Size))
               SWDB_RET(FAILURE);

   // -- mark end of attribute list ------------------------------------------

   Res = TAG_DONE;
   if (SW_WRITE(fp, Res)) SWDB_RET(FAILURE);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 *   Read object's attributes from disk                                      *
 *****************************************************************************/

static LONG SWReadInt(const SWAttrMapT Map[],
                      const UBYTE *idata,
                      const BPTR fp,
                      const ULONG Type,
                      const ULONG Size, const ULONG Pos,
                      const Object *o)
{
   ULONG Data, Len;
   UBYTE **sp = (UBYTE **)SWGetPtr(Map, idata, Pos);
   UBYTE ClassName[SW_MaxClassNameLen];

   switch (Type) {
     case SWAM_Obj:    { Object **op;
                         op  = (Object **)sp;

                         if ((Map[Pos].SGI & SWAM_ObjRead) && *op) {
                            if (SW_READ(fp, Data)) return FAILURE;
                            if (!Data)             return SUCCESS;

                            if (READCNAME(fp, ClassName)) return FAILURE;

                            return DoMethod(*op, SWM_ReadInt, fp) ?
                               SUCCESS : FAILURE;
                         } else {
                            MUI_DisposeObject(*op);
                            *op = NULL;
                            if (SW_READ(fp, Data)) return FAILURE;
                            if (!Data)             return SUCCESS;
                            *op = (Object *)DoMethod((Object *)o, SWM_Read, fp);
                            return *op ? SUCCESS : FAILURE;
                          }
                       }

     case SWAM_Int:    // fall through
     case SWAM_Bool:   if (SW_READ(fp, Data)) return FAILURE;
                       return !SWSetInt(Map, idata, Pos, Data, NULL);
      
      
     case SWAM_StrA:   if (SW_READ(fp, Len)) return FAILURE;
                       FV(*sp);
                       *sp = AV(Len+1);
                       if (!*sp) return FAILURE;
                       return FRead(fp, *sp, Len, 1) != 1;
     
     case SWAM_BackG:  { MUI_ImageSpec is;
                         if (SW_READ(fp, Len))            return FAILURE;
                         if (FRead(fp, &is, Len, 1) != 1) return FAILURE;
                         NewImgSpec(sp, (UBYTE *)&is);
                         return SUCCESS;
                      }
                         
     case SWAM_StrS:   // fall through
     case SWAM_Pen:    // fall through
     case SWAM_Struct: if (SW_READ(fp, Len)) return FAILURE;
                       return FRead(fp, (UBYTE *)sp, Len, 1) != 1;
      
     default:          SWDB_ERROR("internal error");
                       return FAILURE;
   }
}


sa ULONG SWReadAttrs(a0 const SWAttrMapT Map[], a1 const UBYTE *idata,
                     d1 BPTR const fp, a2 const Object *o)
   SDST(ULONG)
   SWP_XX(Map, idata)
   SWDB_ASSERTNULL(Map)
   SWDB_ASSERTNULL(idata)
   SWDB_ASSERTNULL(fp)
{
   LONG pos, Attr, pv;

   if (!fp) SWDB_RET(FAILURE);

   if (SW_READ(fp, pv)) SWDB_RET(FAILURE);              // prefs version

   while (TRUE) {
      if (SW_READ(fp, Attr)) SWDB_RET(FAILURE);         // read attribute
      if (Attr == TAG_DONE) break;                      // break loop if done

      if ((pos = SWFindPos(Map, Attr, 0xff)) < 0) {     // find it in map
         SWDB_ERROR("Read attribute not found");        // yell if not found
         SWDB_RET(FAILURE);
      }
      
      if (!SWAM_Storable(Map[pos].SGI)) {               // not storable? yell.
         SWDB_ERROR("Read non-storable attribute");
         SWDB_RET(FAILURE);
      }
         
      if (SWReadInt(Map, idata, fp, Map[pos].Type, Map[pos].Size, pos, o))
         SWDB_RET(FAILURE);
   }

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 *   Free an attribute's storage, if any                                     *
 *****************************************************************************/

static LONG SWFreeInt(const SWAttrMapT Map[], const UBYTE *idata,
                      const ULONG Pos)
{
   UBYTE **sp = (UBYTE **)SWGetPtr(Map, idata, Pos);

   switch (Map[Pos].Type) {
     case SWAM_Obj:    MUI_DisposeObject((Object *)*sp);
                       *sp = NULL;
                       break;

     case SWAM_BackG:  NewImgSpec(sp, NULL);
                       break;

     case SWAM_Int:
     case SWAM_Bool:
     case SWAM_StrS: 
     case SWAM_Pen:
     case SWAM_Struct: break;

     case SWAM_StrA:   FV(*sp); *sp = NULL;
                       break;
      
     default:          SWDB_ERROR("internal error");
                       return FAILURE;
   }

   return SUCCESS;
}


sa ULONG SWFreeAttrs(a0 const SWAttrMapT Map[], a1 const UBYTE *idata)
   SDST(ULONG)
   SWP_XX(Map, idata)
   SWDB_ASSERTNULL(Map)
   SWDB_ASSERTNULL(idata)
{
   LONG x;

   for (x=0; Map[x].Attr; x++)
      SWFreeInt(Map, idata, x);

   SWDB_RET(SUCCESS);
} SDFT


/*****************************************************************************
 *   Local functions - localize string arrays, and so on.                    *
 *****************************************************************************/

sa void SWLocalizeNewMenu(a0 const Catalog *Cat, a1 const UBYTE  *CatStr[],
                          a2 NewMenu *nm)
   SDSV
   SWP_XXX(Cat, CatStr, nm)
   SWDB_ASSERTNULL(CatStr)
   SWDB_ASSERTNULL(nm)
{
   for (;nm->nm_Type!=NM_END;nm++)
      if (nm->nm_Label != NM_BARLABEL)
         nm->nm_Label = GetCatalogStr((Catalog *)Cat, (int)nm->nm_Label,
                                      (STRPTR)CatStr[(int)nm->nm_Label-1]);
} SDFV


sa void SWLocalizeStringArray(a0 const Catalog *Cat, a1 const STRPTR CatStr[],
                              a2 STRPTR MsgStr[])
   SDSV
   SWP_XXX(Cat, CatStr, MsgStr)
   SWDB_ASSERTNULL(CatStr)
   SWDB_ASSERTNULL(MsgStr)
{
   LONG x;
   
   for (x=0; CatStr[x]; x++)
      MsgStr[x+1] = GetCatalogStr((Catalog *)Cat, x, (STRPTR)CatStr[x]);
} SDFV


/*****************************************************************************
 *   Allocate a UI for editing an attribute array                            *
 *****************************************************************************/

static Object *NewGrp(const SWAttrMapT *Map, const STRPTR MsgStr[])
{
   if (Map->Label && (Map->SGI & SWAM_GrpLab)) {
      STRPTR s = MsgStr[Map->Label];

      if (Map->SGI & SWAM_NewHG)   return HGroup,      GroupFrameT(s), End;
      if (Map->SGI & SWAM_NewVG)   return VGroup,      GroupFrameT(s), End;
      if (Map->SGI & SWAM_NewCG2)  return ColGroup(2), GroupFrameT(s), End;
      if (Map->SGI & SWAM_NewCG4)  return ColGroup(4), GroupFrameT(s), End;
      if (Map->SGI & SWAM_NewCG6)  return ColGroup(6), GroupFrameT(s), End;
   } else {
      if (Map->SGI & SWAM_NewHG)   return HGroup,      End;
      if (Map->SGI & SWAM_NewVG)   return VGroup,      End;
      if (Map->SGI & SWAM_NewCG2)  return ColGroup(2), End;
      if (Map->SGI & SWAM_NewCG4)  return ColGroup(4), End;
      if (Map->SGI & SWAM_NewCG6)  return ColGroup(6), End;
   }
   
   if (Map->SGI & SWAM_NewRG) {
      ULONG  x;
      UBYTE **RegLabels = (UBYTE **)Map->Min;

      for (x=0; x<Map->Default; x++)
         RegLabels[x] = MsgStr[x+Map->Label];

      RegLabels[x] = NULL;

      return RegisterGroup(RegLabels),
                 MUIA_Register_Frame, TRUE,
                 MUIA_CycleChain,     1,
                 MUIA_Background,     MUII_RegisterBack,
             End;
   }

   return NULL;
}

/*****************************************************************************
 * Function: Attach bubble help to objects                                   *
 *****************************************************************************/

static void AttachBubbleHelp(Object *ui, ULONG a, const STRPTR *MsgStr,
                             const HelpMap Help[])
{
   ULONG x;

   if (!Help) return;

   for (x=0; Help[x].Attr; x++)
      if (Help[x].Attr == a) {
         set(ui, MUIA_ShortHelp, MsgStr[Help[x].Label]);
         break;
      }
}

# include "C-Support-UIObjs.h"


/*****************************************************************************
 * Function: Create edit object for various types                            *
 *****************************************************************************/

static Object *NewObj(const SWAttrMapT *Map, const STRPTR MsgStr[],
                      Object *o, const HelpMap Help[])
{
   Object *so = NULL, *e;
   Tag  EditA, SetA;

   // -- create an edit gadget -----------------------------------------------

   switch (Map->Type) {
     case SWAM_StrS:  // fall through
     case SWAM_StrA:  e = UI_Str  (Map, MsgStr, &EditA, &SetA, o, &so); break;
     case SWAM_BackG: e = UI_BackG(Map, MsgStr, &EditA, &SetA, o, &so); break;
     case SWAM_Pen:   e = UI_Pen  (Map, MsgStr, &EditA, &SetA, o, &so); break;
     case SWAM_Int:   e = UI_Int  (Map, MsgStr, &EditA, &SetA, o, &so); break;
     case SWAM_Bool:  e = UI_Bool (Map, MsgStr, &EditA, &SetA, o, &so); break;
     case SWAM_Obj:   e = UI_Obj  (Map, MsgStr, &EditA, &SetA, o, &so); break;
     default:         return NULL;
   }

   if (so) {
      SetAttrs(so, 
               EditA,           SWGetUL(o, Map->Attr),
               MUIA_UserData,   Map->Attr,
               MUIA_CycleChain, 1,
               TAG_DONE);

      if ((Map->SGI & SWAM_Notify) && SetA == EditA) {
         SW_NOTIFYUPD(o, Map->Attr, so, SetA);
      } else {
         SWSetupUINotify(o, so, Map->Attr, EditA, SetA);
      }

      AttachBubbleHelp(so, Map->Attr, MsgStr, Help);

      if (Map->SGI & SWAM_HalfWt) set(so, MUIA_Weight, 50);
   }

   return e;
}


/*****************************************************************************
 * Create UI from the list of attributes.                                    *
 *****************************************************************************/

sa Object *SWAttrUI(a0 const SWAttrMapT Map[], a1 const STRPTR *MsgStr,
                    a2 const HelpMap Help[], a3 Object *o)
{
#  define MAXSTACK 10

   LONG x, l, sp = -1;
   Object *e, *Stack[MAXSTACK];

   Stack[0] = NULL;

   for (x=0; Map[x].Attr; x++) {
      // -- push a new group object if asked to ------------------------------

      if (e = NewGrp(&Map[x], MsgStr)) Stack[++sp] = e;
      if (sp == -1)                    Stack[++sp] = e = ColGroup(2), End;

      if (e && sp != 0) DoMethod(Stack[sp-1], OM_ADDMEMBER, Stack[sp]);

      if (Map[x].Type == SWAM_None &&  (Map[x].SGI & SWAM_Bal))
         DoMethod(Stack[sp], OM_ADDMEMBER, BalanceObject,End);

      if (Map[x].Type == SWAM_None &&  (Map[x].SGI & SWAM_HVSpace))
         DoMethod(Stack[sp], OM_ADDMEMBER, RectangleObject,
                  MUIA_HorizWeight, Map[x].Min,
                  MUIA_VertWeight,  Map[x].Max,
                  End);
      
      if (!(Map[x].SGI & SWAM_HasUI)) goto Pop;
      
      // -- create a label ---------------------------------------------------

      if ((l = Map[x].Label) && !((Map[x].SGI & SWAM_CyNoLab) &&
                                  (Map[x].SGI & SWAM_Cycle))) {
         if (!MsgStr[l])               goto Pop;
         if (!(e = (Map[x].SGI & SWAM_LeftLab ?
                    LLabel2(MsgStr[l]) : Label2(MsgStr[l])))) goto Error;

         DoMethod(Stack[sp], OM_ADDMEMBER, e);
      }

      if (Map[x].SGI & SWAM_Lab) goto Pop;

      // -- Create new object & add to group ---------------------------------

      if (!(e = NewObj(&Map[x], MsgStr, o, Help))) goto Error;

      DoMethod(Stack[sp], OM_ADDMEMBER, e);

      // -- pop group object if asked to -------------------------------------

     Pop:
      if (Map[x].SGI & SWAM_EndG) {
         SWDB_ASSERT(sp>0, "group stack frame underflow");
         sp--;
      }
   }

   return Stack[0];

 Error:
   MUI_DisposeObject(Stack[0]);
   return NULL;
}

   
/*****************************************************************************
 * Kill notifies set up by attr UI                                           *
 *****************************************************************************/

sa void SWKillAttrUI(a0 const SWAttrMapT Map[], a1 Object *ui,
                     a2 Object *o)
{
   Object *g;
   LONG x;

   for (x=0; Map[x].Attr; x++)
      if (g = (Object *)DoMethod(ui, MUIM_FindUData, Map[x].Attr))
         DoMethod(o, MUIM_KillNotifyObj, Map[x].Attr, g);
}

# undef sa
# undef MAXSTACK
