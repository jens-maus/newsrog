# ifndef C_NARTICLE_INCLUDE
#  define C_NARTICLE_INCLUDE

#  include "S-Classes.h"
#  include "S-ClassNames.h"

   // -- messages that this class understands --------------------------------

#  define SW_FIRSTTAG         (int)(TAG_USER + 10600)

   enum {
      SWA_NArticle_Followups  = SW_FIRSTTAG,   // [SGI] followups
      SWA_NArticle_Lines,                      // [SGI] lines in buffer
      SWA_NArticle_Txt,                        // [SGI] Article buffer
      SWA_NArticle_MemPool,                    // [ GI] memory pool
      SWA_NArticle_NewState,                   // [   ] just for notifies
      SWA_NArticle_PctDL,                      // [   ] just for notifies
      SWA_NArticle_IDataSize,                  // [  G] instance data size
   };

   enum {
      SWM_NArticle_Fetch = SW_FIRSTTAG,        // fetch article
      SWM_NArticle_Fetch_P,                    // parse fetched article
      SWM_NArticle_FindHeader,                 // find given header data
      SWM_NArticle_FreeText,                   // free body/header/both
      SWM_NArticle_Search,                     // search for expression
      SWM_NArticle_Cache,                      // store in disk cache
      SWM_NArticle_UnCache,                    // remove from disk cache
      SWM_NArticle_GetCache,                   // read article from cache
      SWM_NArticle_SaveText,                   // save to text file
      SWM_NArticle_LoadText,                   // load from text file
      SWM_NArticle_Clipboard,                  // copy to clipboard
      SWM_NArticle_Abort,                      // abort current receive
      SWM_NArticle_AddLines,                   // add array of lines
      SWM_NArticle_GroupCount,                 // count # of groups art is in
      SWM_NArticle_InGroup,                    // see if article is in group
      SWM_NArticle_Age,                        // find article`s age in minutes
      SWM_NArticle_FlagSet,                    // set   article flags (rtn new)
      SWM_NArticle_FlagReset,                  // reset article flags (rtn new)
      SWM_NArticle_FlagTest,                   // test, no change
      SWM_NArticle_SetPerm,                    // make permanent or not
      SWM_NArticle_ParseName,                  // parse from field
      SWM_NArticle_Cmp,                        // compare 2 articles
      SWM_NArticle_GetMsgID,                   // get MsgID (cached)
      SWM_NArticle_GetSubj,                    // get Subject (cached)
      SWM_NArticle_HashFName,                  // hash filename
      SWM_NArticle_Kill,                       // kill article
      SWM_NArticle_IncFollowups,               // inc # of followups
      SWM_NArticle_OVInfo,                     // overview info
      SWM_NArticle_GetIDHash,                  // msg ID hash
      SWM_NArticle_FastUnCache,                // uncache w/o forced remove
      SWM_NArticle_CanExpire,                  // see if we can expire article
      SWM_NArticle_StatMsg,                    // status msg
      SWM_NArticle_ByteSize,                   // get byte size (computed)
      SWM_NArticle_ParseComment,               // parse file comment
      SWM_NArticle_Refs,                       // get references ID cache
      SWM_NArticle_HasParts,                   // check for article parts
      SWM_NArticle_GetBytes,                   // get byte size from hdr (cached)
      SWM_NArticle_ParseDate,                  // get article age num from string
      SWM_NArticle_AddSeparator,               // add header/body separator line
      SWM_NArticle_FixISOHdrs,                 // fix ISO headers
      SWM_NArticle_FixHdrCase,                 // normalize header case
      SWM_NArticle_ClipHeader,                 // copy header data to clipboard
      SWM_NArticle_GetLines,                   // get lines header as an integer
      SWM_NArticle_ParseMPInfo,                // parse [m/n] multipart info
      SWM_NArticle_GetTxt,                     // get article as AllocVec'ed buff
   };



   enum {
      SWF_NArticle_CachedHead = (1<<0),        // flags: header on disk cache
      SWF_NArticle_CachedBody = (1<<1),        //    body on disk cache
      SWF_NArticle_Highlight  = (1<<2),        //    highlight article in artlist
      SWF_NArticle_Read       = (1<<4),        //    has been read
      SWF_NArticle_Permanent  = (1<<5),        //    Doesn`t expire from cache
      SWF_NArticle_OView      = (1<<6),        //    overview only
      SWF_NArticle_Killed     = (1<<7),        //    killed flag
      SWF_NArticle_QueuedDL   = (1<<8),        //    queued download
      SWF_NArticle_NotFound   = (1<<9),        //    not found on server
      SWF_NArticle_Expired    = (1<<10),       //    expired (don`t save)
      SWF_NArticle_Email      = (1<<11),       //    email, not news msg
      SWF_NArticle_Removed    = (1<<12),       //    temporarily removed from view
      SWF_NArticle_UserMk     = (1<<13),       //    user mark
      SWF_NArticle_NewArt     = (1<<14),       //    new article
      SWF_NArticle_AllFlags   = 0x7fff,        // mask for all of above flags

      SWF_NArticle_PermNo     = 0,             // flags for make-perm
      SWF_NArticle_PermYes    = 1,
      SWF_NArticle_PermToggle = 2,

      SWF_NArticle_Head       = (1<<2),        //    header
      SWF_NArticle_Body       = (1<<3),        //    body
        
      SWF_NArticle_HdBd       = (SWF_NArticle_Head       | SWF_NArticle_Body),
      SWF_NArticle_CachedHdBd = (SWF_NArticle_CachedHead | SWF_NArticle_CachedBody),
   };

   enum {
      SWT_NArticle_Parts = SW_FIRSTTAG,        // tags for fetching    (opt)
      SWT_NArticle_MsgID,                      // fetch by MsgID...    (opt)
      SWT_NArticle_ArtNum,                     // or by article number (opt)
      SWT_NArticle_Server,                     // a NServer/list object
      SWT_NArticle_FetchTitle,                 // TCP connection title
      SWT_NArticle_FetchObj,                   // obj to notify

      SWT_NArticle_Pattern,                    // Search stuff: pattern to find
      SWT_NArticle_ParsedPattern,              // parsed pattern
      SWT_NArticle_CaseSens,                   // case sensitive option
      SWT_NArticle_LinePtr,                    // line pointer
      SWT_NArticle_StartLine,                  // line to start searching
      SWT_NArticle_Header,                     // match only in this header

      SWT_NArticle_CacheForce,                 // force cache load
      SWT_NArticle_FSpec,                      // filename

      SWT_NArticle_CacheDir,                   // cache directory
      SWT_NArticle_Perm,                       // make permanent or not

      SWT_NArticle_AddLines,                   // lines to add (array)
      SWT_NArticle_AddLineCount,               // count of lines in array
      SWT_NArticle_AddWhere,                   // where (body/head)
      SWT_NArticle_AddAlloc,                   // T to strdup them

      SWT_NArticle_MatchPat,                   // pattern to match
      SWT_NArticle_MatchPPat,                  // parsed pattern buff
      SWT_NArticle_MatchStr,                   // string to match
      SWT_NArticle_MatchCase,                  // case sensitive flag
      SWT_NArticle_MatchAnywhere,              // else, just beginning
      SWT_NArticle_MatchHeader,                // header to match

      SWT_NArticle_HashStr,                    // string to hash
      SWT_NArticle_HashBuff,                   // filename buffer

      SWT_NArticle_Append,                     // append mode for saving
      SWT_NArticle_Prefix,                     // line prefix for saving
      SWT_NArticle_QMode,                      // quote mode

      SWT_NArticle_Wild,                       // for Search: is pattern a wildcard?
      SWT_NArticle_SubLine,                    // for Search: substr match

      SWT_NArticle_AddFixISOHdrs,              // fix ISO headers
      SWT_NArticle_IncLines,                   // incremental loading line count
      SWT_NArticle_AddFixHdrCase,              // normalize header case
      SWT_NArticle_NoNetwork,                  // don`t do any network I/O

      SWT_NArticle_Grp,                        // group object
      SWT_NArticle_ArtView,                    // artview object
      SWT_NArticle_SvAttr,                     // save attribute (for filtering)
      SWT_NArticle_ResLen,                     // ptr to LONG for strlen of result
   };

   enum {
      SWV_NArticle_Off = 0,                    // no sort
      SWV_NArticle_SortAge,                    // sort by age
      SWV_NArticle_SortAuthor,                 // sort by author
      SWV_NArticle_SortGroups,                 // sort by # of groups
      SWV_NArticle_SortRef,                    // sort by reference
      SWV_NArticle_SortLines,                  // sort by # of lines
      SWV_NArticle_SortSubj,                   // sort by subject
      SWV_NArticle_SortFollowups,              // sort by # followups
      SWV_NArticle_SortFrom,                   // sort by from field
      SWV_NArticle_SortDistrib,                // sort by distribution
      SWV_NArticle_SortRead,                   // sort read ahead of not
      SWV_NArticle_SortPerm,                   // sort perm ahead of not
      SWV_NArticle_SortSize,                   // sort on size
   };

   enum {
      SWV_NArticle_ForwardSort = 0,            // forward sort
      SWV_NArticle_ReverseSort,                // reverse sort
   };

   enum {
      SWV_NArticle_SecsPerAgeTick = 4,
      SWV_NArticle_AgeTicksPerMin = (60 / SWV_NArticle_SecsPerAgeTick),
      SWV_NArticle_AgeTicksPerHr  = SWV_NArticle_AgeTicksPerMin * 60,
      SWV_NArticle_AgeTicksPerDay = SWV_NArticle_AgeTicksPerHr  * 24,
   };

#  define SWV_NArticle_NoRefs ((ULONG *)-1)

   enum {
      SWV_NArticle_NewFlags = 1,               // for SWA_NArticle_NewState
   };

   enum {
      SWV_NArticle_Stat_Abort = 0,             // for SWM_NArticle_StatMsg
      SWV_NArticle_Stat_Begin,                 // send before NewLines
      SWV_NArticle_Stat_End,                   // sent after NewLines
      SWV_NArticle_Stat_NotFound,              // article was not found
      SWV_NArticle_Stat_NewLines,              // new article lines
      SWV_NArticle_Stat_NewLinesLast,          // last set of new article lines

      SWV_NArticle_Stat_LocDiskCache,          // locations
      SWV_NArticle_Stat_LocNet,
   };

   enum {
      SWV_NArticle_HashFNBuffLen = 12,         // for hash names
   };

#  define SWV_NA_IsNewLines(msg) \
   ((msg)->Status == SWV_NArticle_Stat_NewLines || \
    (msg)->Status == SWV_NArticle_Stat_NewLinesLast)

#  define SWV_NA_IsLast(msg) \
   ((msg)->Status == SWV_NArticle_Stat_NewLinesLast)

#  define SWV_NA_IsEnd(msg)  ((msg)->Status == SWV_NArticle_Stat_Abort || \
                              (msg)->Status == SWV_NArticle_Stat_End   || \
                              (msg)->Status == SWV_NArticle_Stat_NotFound)

#  define SWV_NArticle_IsRe(s) (*((ULONG *)(s)) == MAKE_ID('R','e',':',' '))
#  define SWV_NArticle_ReLen   4
#  define SWV_NArticle_ReStr   "Re: "
#  define SWV_NArticle_SubjNoRe(s) \
          (SWV_NArticle_IsRe(s) ? (s) + SWV_NArticle_ReLen : (s))

#  define SWV_NArticle_SameSubj(s1,s2)                                       \
   ((s1) && (s2) &&                                                          \
           !strcmp(SWV_NArticle_SubjNoRe(s1), SWV_NArticle_SubjNoRe(s2)))

   SWM_P(SWP_NArticle_ParseMPInfo,             // parse [m/n] or (m/n) info
         LONG *Part;                           // ptr to part number
         LONG *MaxPart;                        // ptr to max part #
         LONG *SubjPos;                        // len of subj string before '['
         Object *Art);                         // if non-NULL, must be part of
                                               // the same MP group as this.

   SWM_P(SWP_NArticle_ParseDate,               // parse article date
         STRPTR Date;                          // date string
         struct DateStamp *DS;                 // struct datestamp, or null
         ULONG  AddTZOffset);                  // TRUE to add timezone offset

   SWM_P(SWP_NArticle_FreeText,                // free body/header/both
         ULONG Parts);                         // which parts (body/hdr flags)

   SWM_P(SWP_NArticle_Clipboard,               // copy parts to clipboard
         ULONG Parts);                         // which parts (body/hdr flags)

   SWM_P(SWP_NArticle_FindHeader,              // find header data
         UBYTE *Header);                       // header to find

   SWM_P(SWP_NArticle_ClipHeader,              // copy header data to clipboard
         UBYTE *Header);                       // header to find

   SWM_P(SWP_NArticle_Flags,                   // flags.
         ULONG Flags);                         //

   SWM_P(SWP_NArticle_Age,                     // get articles age in minutes
         struct DateStamp *DS);                // ptr to buff for DS of article

   SWM_P(SWP_NArticle_InGroup,                 // find if article is posted to group
         STRPTR Group);                        // group name

   SWM_P(SWP_NArticle_ParseName,               // parse from field
         STRPTR FullName;                      // full name field (NULL=from:)
         STRPTR NameBuff;                      // buffer for user name
         ULONG  NameBuffLen;                   // length of buffer
         STRPTR EmailBuff;                     // buffer for email addr
         ULONG  EmailBuffLen);                 // length of buffer

   SWM_P(SWP_NArticle_ParseComment,            // parse file comment
         STRPTR Comment);                      // file comment (in)

   struct  SWS_NArticle_CmpInfo {
      ULONG    Count;                          // count of attrs
      ULONG    Attrs[4];                       // compare attributes
      ULONG    Dirs[4];                        // compare directions
      Object  *ArtMgr;                         // for threading info
   };

   SWM_P(SWP_NArticle_Cmp,                     // compare 2 articles
         Object *Article;                      // article to compare with
         struct SWS_NArticle_CmpInfo *CmpInfo);

   struct  SWS_NArticle_OVInfo {
      LONG    GroupCount,
              Followups,
              Age;
      ULONG   Flags,
              Hash;
      STRPTR  Subject,
              Lines;
      STRPTR  SNameBuff;
      ULONG   SNBuffLen;
      struct  DateStamp ds;
      ULONG   Bytes;
   };

   SWM_P(SWP_NArticle_OVInfo,                  // get article overview info
         struct SWS_NArticle_OVInfo *OVInfo);

   SWM_P(SWP_NArticle_CanExpire,               // get article overview info
         LONG AgeLimit);                       // age limit

   SWM_P(SWP_NArticle_StatMsg,                 // server status msg
         Object *Article;                      // ptr to article
         ULONG Status;                         // status
         STRPTR *Lines;                        // text lines
         LONG FirstNew;                        // first new line
         LONG LastNew;                         // last new line
         LONG LastHdr;                         // last header line
         ULONG Loc);                           // where it came from

#  define SWF_NAHeader_Subject    "Subject:"
#  define SWF_NAHeader_Lines      "Lines:"
#  define SWF_NAHeader_Date       "Date:"
#  define SWF_NAHeader_Ref        "References:"
#  define SWF_NAHeader_From       "From:"
#  define SWF_NAHeader_Xref       "Xref:"
#  define SWF_NAHeader_MsgID      "Message-ID:"
#  define SWF_NAHeader_Bytes      "Bytes:"
#  define SWF_NAHeader_Groups     "Newsgroups:"
#  define SWF_NAHeader_Approved   "Approved:"
#  define SWF_NAHeader_Sender     "Sender:"
#  define SWF_NAHeader_Distrib    "Distribution:"
#  define SWF_NAHeader_X_NewsRd   "X-Newsreader:"
#  define SWF_NAHeader_Followup   "Followup-To:"
#  define SWF_NAHeader_Keywords   "Keywords:"
#  define SWF_NAHeader_Organiz    "Organization:"
#  define SWF_NAHeader_Path       "Path:"
#  define SWF_NAHeader_State      "X-snrst:"
#  define SWF_NAHeader_MimeVer    "Mime-Version:"
#  define SWF_NAHeader_MimeType   "Content-Type:"
#  define SWF_NAHeader_MimeDesc   "Content-Description:"
#  define SWF_NAHeader_MimeCTE    "Content-Transfer-Encoding:"
#  define SWF_NAHeader_MimeCD     "Content-Disposition:"
#  define SWF_NAHeader_XComplain  "X-Complaints-To:"

#  define SWP_HasHeader(s, h)       (!strncmp((s),  h, sizeof(h)-1))
#  define SWP_HasHeaderNoCase(s, h) (!Strnicmp((s), h, sizeof(h)-1))

#  define SWV_NArticle_CacheDir   "Articles"
   
   enum {
      SWV_NAHeader_Subject = 0,  // must be in same order as above
      SWV_NAHeader_Lines,
      SWV_NAHeader_Date,
      SWV_NAHeader_Ref,
      SWV_NAHeader_From,
      SWV_NAHeader_MsgID,
      SWV_NAHeader_Bytes,
      SWV_NAHeader_Groups,
      SWV_NAHeader_Approved,
      SWV_NAHeader_Sender,
      SWV_NAHeader_Distrib,
      SWV_NAHeader_X_NewsRd,
      SWV_NAHeader_Followup,
      SWV_NAHeader_Keywords,
      SWV_NAHeader_Organiz,
      SWV_NAHeader_Path,
      SWV_NAHeader_State,
   };

#  undef SW_FIRSTTAG   

# endif
