#ifndef S_ATTRMAPS_INCLUDE
# define S_ATTRMAPS_INCLUDE

enum {
   SWAM_None=1,        // no type
   SWAM_Int,           // integer attribute
   SWAM_Bool,          // boolean attribute
   SWAM_StrS,          // fixed string
   SWAM_StrA,          // allocated string
   SWAM_Struct,        // structure
   SWAM_FixS,          // fixed value, string
   SWAM_FixI,          // fixed value, integer
   SWAM_Obj,           // object pointer
   SWAM_Pen,           // pen object (use with MUI_Poppen, etc)
   SWAM_BackG,         // background object
   SWAM_MUIA,          // initial MUIA value for object setup
};

enum {
   SWAM_Set     = (1<< 0),  // settable attribute
   SWAM_Get     = (1<< 1),  // gettable attribute
   SWAM_Init    = (1<< 2),  // inittable attribute
   SWAM_Stor    = (1<< 3),  // always store, even if can't set or get
   SWAM_NoStor  = (1<< 4),  // never store or compare, even if we can set/get
   SWAM_HasUI   = (1<< 5),  // should have a UI to control it
   SWAM_NewHG   = (1<< 6),  // new horizontal group
   SWAM_NewVG   = (1<< 7),  // new vertical group
   SWAM_NewCG2  = (1<< 8),  // new colgroup(2)
   SWAM_NewCG4  = (1<< 9),  // new colgroup(4)
   SWAM_NewRG   = (1<<10),  // new register group
   SWAM_EndG    = (1<<11),  // end group
   SWAM_Bal     = (1<<12),  // balance group object (if used with SWAM_None)
   SWAM_Cycle   = (1<<13),  // integer is cycle gad, not slider
   SWAM_HVSpace = (1<<13),  // hvspace object (if used with SWAM_None)
   SWAM_PopFile = (1<<14),  // pop ASL file requestor for string gadget
   SWAM_TimeSld = (1<<14),  // time slider (same val as PopFile - str vs int)
   SWAM_TimeSec = (1<<15),  // timeslider slides seconds, not millis
   SWAM_IntStr  = (1<<15),  // integer uses string gad, not slider
   SWAM_IntBnd  = (1<<16),  // enforce integer bounds
   SWAM_PopFont = (1<<17),  // pop ASL font requestor for string gadget
   SWAM_KbSld   = (1<<17),  // kb slider (same val as PopFont - str vs int)
   SWAM_DropStr = (1<<18),  // can drop objects on string w/ attr
   SWAM_CyNoLab = (1<<18),  // ommit 1st label for cycle gadget
   SWAM_Asgn    = (1<<19),  // assign, even if nostore
   SWAM_Lab     = (1<<20),  // label only
   SWAM_GrpLab  = (1<<20),  // use group label
   SWAM_NoFrame = (1<<21),  // no frame for this group
   SWAM_LeftLab = (1<<22),  // left aligned label
   SWAM_HalfWt  = (1<<23),  // half weight
   SWAM_NewCG6  = (1<<24),  // new colgroup(6)
   SWAM_StrRet  = (1<<25),  // for string, accept on return only, not keys
   SWAM_PopDir  = (1<<26),  // pop ASL dir requestor for string gadget
   SWAM_Notify  = (1<<27),  // set with notifications
   SWAM_ObjRead = (1<<28),  // read object directly instead of replacing

   SWAM_SGI     = (SWAM_Set|SWAM_Get|SWAM_Init),  // set, get, & init
   SWAM_SGIN    = (SWAM_SGI|SWAM_NoStor),         // set, get, & init, no store
   SWAM_RS      = (SWAM_Get|SWAM_Stor),           // get & store
   SWAM_UI      = (SWAM_SGI|SWAM_HasUI),          // SGI with UI
};

# define SWAM_Storable(x) ((((x)&SWAM_Get)&&((x)&SWAM_Set)||((x)&SWAM_Stor))\
                           && !((x)&SWAM_NoStor))
# define SWAM_GetSettable(x) ((((x)&SWAM_Set)&&((x)&SWAM_Get)&& \
                              !((x)&SWAM_NoStor))||((x)&SWAM_Asgn))

# define SWAM_NoLab 0

typedef struct _swattrmap {
   Tag    Attr;          // attribute
   size_t Offset;        // offset into instance data
   size_t Size;          // size of attribute
   ULONG  Type;          // type (int/str/etc)
   ULONG  SGI;           // set/get/init type
   ULONG  Default;       // default value
   LONG   Min;           // min value
   LONG   Max;           // max value
   ULONG  Label;         // MsgStr entry for object label
} SWAttrMapT;

# define SWAM(m,oo,so,t,s,d,mn,mx,l)   \
         {m, oo, so, (ULONG)t, (ULONG)s, (ULONG)d, (LONG)mn, (LONG)mx, l}


// -- default size & offset macros -------------------------------------------

# define sizeofmemb(d,m) sizeof(((d *)0)->m)


/*****************************************************************************
 * m  = member                                                               *
 * oo = offset into idata structure                                          *
 * so = sizeof member                                                        *
 * t  = type (SWAM_Int/SWAM_Bool/etc)                                        *
 * s  = Flags (SWAM_Set/SWAM_HasUI/etc)                                      *
 * d  = default value                                                        *
 * mn = min value                                                            *
 * mx = max value                                                            *
 * l  = label in CatMsg array.                                               *
 *****************************************************************************/

# define SWAM_D(m,t,s,d,mn,mx,l) \
         SWAM(m, offsetof(SW_IDATA,m), sizeofmemb(SW_IDATA, m), t,s,d,mn,mx,l)

// -- various parameters types -----------------------------------------------

# define SWAM_IB(m,l,s,d,mn,mx) SWAM_D(m, SWAM_Int, (s)|SWAM_IntBnd, d,mn,mx,l)

# define SWAM_IN(m,l,s,d)       SWAM_D(m, SWAM_Int,  s, d, LONG_MIN,LONG_MAX,l)
# define SWAM_BO(m,l,s,d)       SWAM_D(m, SWAM_Bool, s, d, 0,        1,     l)
# define SWAM_SS(m,l,s,d)       SWAM_D(m, SWAM_StrS, s, d, 0, sizeofmemb(SW_IDATA, m), l)
# define SWAM_SA(m,l,s,d,mx)    SWAM_D(m, SWAM_StrA, s, d, 0,       mx,     l)
# define SWAM_DS(m,l,s,d,mx,a)  SWAM_D(m, SWAM_StrA, (s)|SWAM_DropStr, d, a, mx, l)
# define SWAM_DSS(m,l,s,d,mx)   SWAM_DS(m,l,s,d,mx,m)
# define SWAM_FR(m,l,s,d,mx)    SWAM_DSS(m,l,(s)|SWAM_PopFile,d,mx)
# define SWAM_PF(m,l,s,d,mx)    SWAM_DSS(m,l,(s)|SWAM_PopFont,d,mx)
# define SWAM_FI(m,l,s,d)       SWAM(m, 0, 4, SWAM_FixI, s, d, 0, 0, l)
# define SWAM_FS(m,l,s,d)       SWAM(m, 0, 0, SWAM_FixS, s, d, 0, 0, l)
# define SWAM_CY(m,l,s,d,c,a)   SWAM_D(m,SWAM_Int,(s)|SWAM_Cycle,d,c,a,l)
# define SWAM_BG(m,l,s,d)       SWAM_D(m,SWAM_BackG,s,d,0,0,l)
# define SWAM_MU(m,d)           SWAM(m,0,0,SWAM_MUIA,SWAM_Init,d,0,0,0)
# define SWAM_DONE              SWAM(0,0,0,0,0,0,0,0,0)
# define SWAM_UICTL(s,l,p1,p2)  SWAM(SWAM_None,0,0,0,s,(ULONG)p1,(ULONG)p2,0,l)

# define SWAM_PT(m,l,s,d) \
         SWAM_D(m, SWAM_Int,  s|SWAM_NoStor, d, LONG_MIN, LONG_MAX, l)

# define SWAM_TX(l,s) \
         SWAM(SWAM_None,0,0,0, s|SWAM_HasUI|SWAM_Lab,0,0,0,l)

# define SWAM_AR(m,l,b,o,s) \
         SWAM(m, offsetof(SW_IDATA, b[o]), sizeofmemb(SW_IDATA, b[0]), \
              SWAM_Struct, s, 0, 0, 0, l)

# define SWAM_PA(m,l,b,o,d,s) \
         SWAM(m, offsetof(SW_IDATA, b[o]), sizeofmemb(SW_IDATA, b[0]), \
              SWAM_Pen, s, d, 0, 0, l)

# define SWAM_OB(m,l,s) \
         SWAM_D(m,SWAM_Obj,(s),NULL,LONG_MIN,LONG_MAX,l)

# define SWAM_OBO(m,mp,l,s) \
         SWAM_OB(m,l,s), \
         SWAM(mp, offsetof(SW_IDATA, m), sizeofmemb(SW_IDATA, m), SWAM_Int, \
              s|SWAM_NoStor, NULL, LONG_MIN, LONG_MAX, NULL)

# define SWAM_HVS(h,v) \
         SWAM(SWAM_None,0,0,SWAM_None,SWAM_HVSpace,0,h,v,0)

# endif // S_ATTRMAPS_INCLUDE
