/**/
/* Library functions*/
/**/
#pragma libcall SWSupportBase SWOpenLibs 1e 8002
#pragma libcall SWSupportBase SWCloseLibs 24 8002
#pragma libcall SWSupportBase SWInit 2a 0
#pragma libcall SWSupportBase SWExit 30 0
#pragma libcall SWSupportBase SWGetVal 36 1A02
/**/
/* Debug functions*/
/**/
#pragma libcall SWSupportBase SWDebugOpen 3c 10803
#pragma libcall SWSupportBase SWDebugSet 42 1002
#pragma libcall SWSupportBase SWDebugClose 48 0
#pragma libcall SWSupportBase SWDebugEnter 4e 81003
#pragma libcall SWSupportBase SWDebugExit 54 81003
#pragma tagcall SWSupportBase SWDebugPrint 5a 981004
/**/
/* Attribute map functions*/
/**/
#pragma libcall SWSupportBase SWGetAttr 60 A09804
#pragma libcall SWSupportBase SWSetAttr 66 109804
#pragma libcall SWSupportBase SWInitAttrs 6c A9803
#pragma libcall SWSupportBase SWFreeAttrs 72 9802
#pragma libcall SWSupportBase SWGenTagList 78 A9803
#pragma libcall SWSupportBase SWCmpAttrs 7e A9803
#pragma libcall SWSupportBase SWWriteAttrs 84 B19804
#pragma libcall SWSupportBase SWReadAttrs 8a A19804
#pragma libcall SWSupportBase SWAttrUI 90 BA9804
#pragma libcall SWSupportBase SWKillAttrUI 96 A9803
/**/
/* Locale functions*/
/**/
#pragma libcall SWSupportBase SWLocalizeNewMenu 9c A9803
#pragma libcall SWSupportBase SWLocalizeStringArray a2 A9803
/**/
/* Subtask functions*/
/**/
#pragma libcall SWSupportBase SWSendSubTaskMsg a8 10803
#pragma libcall SWSupportBase SWSpawnSubTask ae 109804
#pragma libcall SWSupportBase SWKillSubTask b4 801
#pragma libcall SWSupportBase SWExitSubTask ba A9803
#pragma libcall SWSupportBase SWInitSubTask c0 0
/**/
/* Custom Classes*/
/**/
#pragma libcall SWSupportBase SWTimesliderClass c6 0
#pragma libcall SWSupportBase SWObjStringClass cc 0
#pragma libcall SWSupportBase SWObjListClass d2 0
/**/
/* Memory Pool fns*/
/**/
#pragma libcall SWSupportBase SWPoolAV d8 0802
#pragma libcall SWSupportBase SWPoolFV de 9802
#pragma libcall SWSupportBase SWPoolRV e4 90803
#pragma libcall SWSupportBase SWPoolNullStr ea 0
#pragma libcall SWSupportBase SWPoolStrDup f0 9802
#pragma libcall SWSupportBase SWPoolCreate f6 A98004
#pragma libcall SWSupportBase SWPoolDelete fc 801
/**/
/* Formatting functions*/
/**/
#pragma libcall SWSupportBase SWFmtSize 102 19003
/**/
/* Command line utilities*/
/**/
#pragma libcall SWSupportBase SWBoolVal 108 801
#pragma libcall SWSupportBase SWGetCmdLine 10e 801
#pragma libcall SWSupportBase SWFreeCmdLine 114 801
/**/
/* UI Functions*/
/**/
#pragma libcall SWSupportBase SWUI_FrameText 11a A90804
#pragma libcall SWSupportBase SWUI_Button 120 9802
#pragma libcall SWSupportBase SWUI_ImageBt 126 1A90805
/**/
/* Directory fns*/
/**/
#pragma libcall SWSupportBase SWMakePath 12c 801
#pragma libcall SWSupportBase SWDirExists 132 801
#pragma libcall SWSupportBase SWFileExists 138 801
/**/
/* Clipboard Fns*/
/**/
#pragma libcall SWSupportBase SWOpenClipWrite 13e 001
#pragma libcall SWSupportBase SWCloseClipWrite 144 801
#pragma libcall SWSupportBase SWOpenClipRead 14a 001
#pragma libcall SWSupportBase SWCloseClipRead 150 801
#pragma libcall SWSupportBase SWTextToClip 156 9802
#pragma libcall SWSupportBase SWClipToText 15c 9802
/**/
/* Pattern fns*/
/**/
#pragma libcall SWSupportBase SWPB_Start 162 A109805
#pragma libcall SWSupportBase SWPB_Find 168 1BA9805
#pragma libcall SWSupportBase SWPB_End 16e 801
/**/
/* More Custom Classes*/
/**/
#pragma libcall SWSupportBase SWKbsliderClass 174 0
/**/
/* Decoding fns for UUencode, Base64Decode, QuotedPrintable*/
/**/
#pragma libcall SWSupportBase SWUUIsBegin 17a 801
#pragma libcall SWSupportBase SWUUIsEnd 180 801
#pragma libcall SWSupportBase SWUUFileName 186 801
#pragma libcall SWSupportBase SWUUSize 18c 801
#pragma libcall SWSupportBase SWUUDecodeLine 192 A09804
#pragma libcall SWSupportBase SWUUDecode 198 DBA90806
#pragma libcall SWSupportBase SWBase64Decode 19e DBA90806
#pragma libcall SWSupportBase SWQPDecode 1a4 DBA90806
#pragma libcall SWSupportBase SWMimeDecode 1aa DBA90806
#pragma libcall SWSupportBase SWCountLines 1b0 801
#pragma libcall SWSupportBase SWDecodeErrStr 1b6 001
/**/
/* Misc Utility Fns*/
/**/
#pragma libcall SWSupportBase SWReadFileContents 1bc A9803
#pragma libcall SWSupportBase SWWriteFileContents 1c2 09803
#pragma libcall SWSupportBase SWWriteFileString 1c8 09803
#pragma libcall SWSupportBase SWDeleteFile 1ce 801
#pragma libcall SWSupportBase SWOpenAppend 1d4 801
/**/
/* Rexx Cmds*/
/**/
#pragma libcall SWSupportBase SWSendRexxCmd 1da 0BA9805
/**/
/* more en/decoding functions*/
/**/
#pragma libcall SWSupportBase SWQPEncodeStr 1e0 9802
/**/
/* Test fns*/
/**/
#pragma libcall SWSupportBase SWDumpObj 1e6 0802
/**/
