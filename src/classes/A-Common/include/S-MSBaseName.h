
static STRPTR MSBaseName(STRPTR FSpec)
{
   STRPTR Base;
   
   return (Base = strrchr(FSpec, '\\')) ? Base + 1 : FSpec;
}
