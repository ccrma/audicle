/***************************************************/
/***************************************************/

#include "thread.h"

Thread :: Thread()
{
  thread = 0;
}

Thread :: ~Thread()
{

#if defined(POSIX_THREADS)

  pthread_cancel(thread);
  pthread_join(thread, NULL);

#elif defined(WIN_THREADS)

  if ( thread )
    TerminateThread((HANDLE)thread, 0);

#endif
}

bool Thread :: start( THREAD_FUNCTION routine, void * ptr )
{
  bool result = false;

#if defined(POSIX_THREADS)

  if ( pthread_create(&thread, NULL, *routine, ptr) == 0 )
    result = true;

#elif defined(WIN_THREADS)

  unsigned thread_id;
  thread = _beginthreadex(NULL, 0, routine, ptr, 0, &thread_id);
  if ( thread ) result = true;

#endif

  return result;
}

bool Thread :: wait( long milliseconds )
{
  bool result = false;
#if defined(POSIX_THREADS)

  pthread_cancel(thread);
  pthread_join(thread, NULL);

#elif defined(WIN_THREADS)

  DWORD timeout, retval;
  if ( milliseconds < 0 ) timeout = INFINITE;
  else timeout = milliseconds;
  retval = WaitForSingleObject( (HANDLE)thread, timeout );
  if ( retval == WAIT_OBJECT_0 ) {
    CloseHandle( (HANDLE)thread );
    thread = 0;
    result = true;
  }

#endif
  return result;
}

void Thread :: test(void)
{
#if defined(POSIX_THREADS)

  pthread_testcancel();

#endif
}


Mutex :: Mutex()
{

#if defined(POSIX_THREADS)

  pthread_mutex_init(&mutex, NULL);

#elif defined(WIN_THREADS)

  InitializeCriticalSection(&mutex);

#endif 
}

Mutex :: ~Mutex()
{
#if defined(POSIX_THREADS)

  pthread_mutex_destroy(&mutex);

#elif defined(WIN_THREADS)

  DeleteCriticalSection(&mutex);

#endif 
}

void Mutex :: lock()
{
#if defined(POSIX_THREADS)

  pthread_mutex_lock(&mutex);

#elif defined(WIN_THREADS)

  EnterCriticalSection(&mutex);

#endif 
}

void Mutex :: unlock()
{
#if defined(POSIX_THREADS)

  pthread_mutex_unlock(&mutex);

#elif defined(WIN_THREADS)

  LeaveCriticalSection(&mutex);

#endif 
}

