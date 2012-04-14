//STK THREADS  - Perry Cook and Gary Scavone
/***************************************************/
/***************************************************/

#if !defined(__THREAD_H)
#define __THREAD_H

#if ( defined(__PLATFORM_LINUX__) || defined(__PLATFORM_MACOSX__) )
#define POSIX_THREADS 
#elif defined(__PLATFORM_WIN32__)
#define WIN_THREADS
#endif  

#if defined(POSIX_THREADS)

  #include <pthread.h>
  #define THREAD_TYPE
  typedef pthread_t THREAD_HANDLE;
  typedef void * THREAD_RETURN;
  typedef void * (*THREAD_FUNCTION)(void *);
  typedef pthread_mutex_t MUTEX;

#elif defined(WIN_THREADS)

  #include <windows.h>
  #include <process.h>
  #define THREAD_TYPE __stdcall
  typedef unsigned long THREAD_HANDLE;
  typedef unsigned THREAD_RETURN;
  typedef unsigned (__stdcall *THREAD_FUNCTION)(void *);
  typedef CRITICAL_SECTION MUTEX;

#endif

class Thread
{
 public:
  Thread();

  ~Thread();

  bool start( THREAD_FUNCTION routine, void * ptr = NULL );

  bool wait( long milliseconds = -1 );

  static void test(void);

 protected:

  THREAD_HANDLE thread;

};

class Mutex
{

 public:
  Mutex();

  ~Mutex();

  void lock(void);
  void unlock(void);

 protected:

  MUTEX mutex;

};

#endif // defined(__THREAD_H)
