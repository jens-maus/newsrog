
#  define SWDiskObjVars struct DiskObject *AIcon = NULL;

#  define SWAppObj(an,anc,ver,desc,bpri,st)                                 \
   (ApplicationObject,                                                      \
       MUIA_Application_Title      , an,                                    \
       MUIA_Application_Version    , "$VER: " an " " ver " " __AMIGADATE__, \
       MUIA_Application_Copyright  , "©1997-1998 Shadowworks Software",     \
       MUIA_Application_Author     , "Steve Koren",                         \
       MUIA_Application_Description, desc,                                  \
       MUIA_Application_Base       , anc,                                   \
       MUIA_Application_BrokerPri  , bpri,                                  \
       MUIA_Application_DiskObject , AIcon = GetDiskObject("PROGDIR:" an),  \
       MUIA_Application_SingleTask , st,                                    \
    End)

#   define SWFreeAppObj(app) \
    MUI_DisposeObject(app); \
    if (AIcon) FreeDiskObject(AIcon);
