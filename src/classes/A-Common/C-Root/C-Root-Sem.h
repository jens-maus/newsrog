#  ifdef SWSEMAPHOREFNS

/*****************************************************************************
 * get object semaphore                                                      *
 *****************************************************************************/

METHOD(SWM_Semaphore_Obtain, Msg)
{
   SWObtainSemaphore(o);
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * get shared object semaphore                                               *
 *****************************************************************************/

METHOD(SWM_Semaphore_ObtainShared, Msg)
{
   SWObtainSemaphoreShared(o);
   SWDB_RET(0);
} SDFT


/*****************************************************************************
 * release object semaphore                                                  *
 *****************************************************************************/

METHOD(SWM_Semaphore_Release, Msg)
{
   SWReleaseSemaphore(o);
   SWDB_RET(0);
} SDFT

#  endif
