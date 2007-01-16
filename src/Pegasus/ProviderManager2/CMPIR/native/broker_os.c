//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
//%/////////////////////////////////////////////////////////////////////////////

#include <pthread.h>
#include "tool.h"
#include "native.h"
#include "debug.h"
#include "mm.h"
#include <stdlib.h>
#include <Pegasus/Provider/CMPI/cmpipl.h>
#ifdef CMPI_PLATFORM_ZOS_ZSERIES_IBM
#include <errno.h>
#endif


static char *resolveFileName (const char *filename)
{
   char dlName[1024];
#if defined(CMPI_PLATFORM_WIN32_IX86_MSVC)
    strcpy(dlName,filename);
    strcat(dlName,".dll");
#elif defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
    strcpy(dlName,"lib");
    strcat(dlName,filename);
    strcat(dlName,".so");
#elif defined(CMPI_OS_HPUX)
    #ifdef CMPI_PLATFORM_HPUX_PARISC_ACC
        strcpy(dlName,"lib");
        strcat(dlName,filename);
        strcat(dlName,".sl");
    #else
        strcpy(dlName,"lib");
        strcat(dlName,filename);
        strcat(dlName,".so");
    #endif
#elif defined(CMPI_OS_VMS)
    strcpy(dlName,"/wbem_lib");
    strcat(dlName,filename);
    strcat(dlName,".exe");
#elif defined(CMPI_OS_OS400)
    strcpy(dlName,filename);
#elif defined(CMPI_OS_DARWIN)
    strcpy(dlName,"lib");
    strcat(dlName,filename);
    strcat(dlName,".dylib");
#else
    strcpy(dlName,"lib");
    strcat(dlName,filename);
    strcat(dlName,".so");
#endif

   return strdup(dlName);
}

/*
   We need to have wrapper for newthread. When we invoke newThread from current
   thread, new thread is not managed by memeory management and may not have
   CMPIBroker and CMPIContext assosiated with that. Add them in wrapper. -V 5245
*/
struct startWrapperArg
{
    void *(CMPI_THREAD_CDECL * start) (void *);
    void *arg;
    CMPIBroker *broker;
    CMPIContext *ctx;
};

typedef struct startWrapperArg startWrapperArg;

void *_start_wrapper(void *arg_)
{
    startWrapperArg *arg = (startWrapperArg *) arg_;
    void* return_value;

    return_value = (*arg->start) (arg->arg);
    free (arg);

    return return_value;
}

static CMPI_THREAD_TYPE newThread
        (CMPI_THREAD_RETURN (CMPI_THREAD_CDECL *start )(void *), void *parm, int detached)
{
    startWrapperArg *wparm = (startWrapperArg*)
                             malloc ( sizeof ( struct startWrapperArg ) );
    wparm->start = start;
    wparm->arg = parm;
    wparm->broker = NULL;
    wparm->ctx = NULL;

#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   pthread_t t;
    pthread_attr_t tattr;
    if (detached) {
       pthread_attr_init(&tattr);
       pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
       pthread_create(&t, &tattr, (void *(*)(void *)) _start_wrapper, wparm);
    }
    else pthread_create(&t, NULL, (void *(*)(void *)) _start_wrapper, wparm);
    return (CMPI_THREAD_TYPE)t;

#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
	#define PTHREAD_CREATE_DETACHED 0
	pthread_t t;
	pthread_attr_t tattr;
	if (detached) {
		pthread_attr_init(&tattr);
		pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
		pthread_create(&t, &tattr, (void *(*)(void *)) _start_wrapper, wparm);
	}
	else pthread_create(&t, NULL, (void *(*)(void *)) _start_wrapper, wparm);

	return (CMPI_THREAD_TYPE) &t;
#else
   #error Platform not yet supported
   #error Platform for Remote CMPI daemon not yet supported

#endif
}

#ifdef CMPI_PLATFORM_ZOS_ZSERIES_IBM
static int threadOnce (pthread_once_t *once, void (*init)(void))
{
	return pthread_once ( once, init );
}
#else

static int threadOnce (int *once, void (*init)(void))
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
  return pthread_once ( once, init );
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}
#endif /* endif of #ifdef CMPI_PLATFORM_ZOS_ZSERIES_IBM */




static int createThreadKey(CMPI_THREAD_KEY_TYPE *key, void (*cleanup)(void*))
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   return pthread_key_create ((pthread_key_t*)key , cleanup );
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
   return pthread_key_create (key , cleanup );
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static int destroyThreadKey(CMPI_THREAD_KEY_TYPE key)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   return pthread_key_delete (key);
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
   // thread keys get deleted at thread-end by system
   return 0;
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static void *getThreadSpecific(CMPI_THREAD_KEY_TYPE key)
{
 #if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
  return pthread_getspecific(key);
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
  return (void*) pthread_getspecific_d8_np(key);
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static int setThreadSpecific(CMPI_THREAD_KEY_TYPE key, void * value)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   return pthread_setspecific(key,value);
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
   return pthread_setspecific(key,value);
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}



static CMPI_MUTEX_TYPE newMutex (int opt)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   static pthread_mutex_t tmpl=PTHREAD_MUTEX_INITIALIZER;
   pthread_mutex_t *m=calloc(1,sizeof(pthread_mutex_t));
   *m=tmpl;
   return m;
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
   // pthread_mutex_t init_m;
   pthread_mutex_t *new_mutex = (pthread_mutex_t*) calloc(1,sizeof(pthread_mutex_t));
   // PTHREAD_MUTEX_INITIALIZER;
   errno = 0;

   if (pthread_mutex_init(new_mutex, NULL) != 0)
   {
	  TRACE_CRITICAL(("pthread_mutex_init failed: %s",strerror(errno)));
	  return NULL;
   }
   return (CMPI_MUTEX_TYPE) new_mutex;
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static void destroyMutex (CMPI_MUTEX_TYPE m)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   free(m);
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
   if (pthread_mutex_destroy((pthread_mutex_t *) m)!=0)
   {
	   TRACE_CRITICAL(("pthread_mutex_destroy failed: %s",strerror(errno)));
   }
   free((pthread_mutex_t*) m);
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static void lockMutex (CMPI_MUTEX_TYPE m)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
  pthread_mutex_lock ((pthread_mutex_t *)m );
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
  pthread_mutex_lock ((pthread_mutex_t *)m );
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static void unlockMutex (CMPI_MUTEX_TYPE m)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   pthread_mutex_unlock ((pthread_mutex_t *)m );
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
   pthread_mutex_unlock ((pthread_mutex_t *)m );
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}



static CMPI_COND_TYPE newCondition (int opt)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
  pthread_cond_t tmpl=PTHREAD_COND_INITIALIZER;
   pthread_cond_t *c=calloc(1,sizeof(pthread_cond_t));
   *c=tmpl;
   return c;
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
	pthread_cond_t *c=(pthread_cond_t*) calloc(1,sizeof(pthread_cond_t));;
    if (pthread_cond_init(c, NULL) != 0)
	{
		TRACE_CRITICAL(("pthread_cond_init failed: %s",strerror(errno)));
	}
	return (CMPI_COND_TYPE) c;
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static void destroyCondition (CMPI_COND_TYPE c)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
  free(c);
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
  if (pthread_cond_destroy((pthread_cond_t*) c) != 0)
  {
	  TRACE_CRITICAL(("pthread_cond_destroy failed: %s",strerror(errno)));
  }
  free((pthread_cond_t*) c);
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static int timedCondWait(CMPI_COND_TYPE c, CMPI_MUTEX_TYPE m, struct timespec *wait)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   return pthread_cond_timedwait((pthread_cond_t*)c, (pthread_mutex_t*)m, wait );
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
   return pthread_cond_timedwait((pthread_cond_t*)c, (pthread_mutex_t*)m, wait );
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static int condWait(CMPI_COND_TYPE c, CMPI_MUTEX_TYPE m)
{
#if defined(CMPI_PLATFORM_LINUX_GENERIC_GNU)
   return pthread_cond_wait((pthread_cond_t*)c, (pthread_mutex_t*)m);
#elif defined(CMPI_PLATFORM_ZOS_ZSERIES_IBM)
   return pthread_cond_wait((pthread_cond_t*)c, (pthread_mutex_t*)m);
#else
   #error Platform for Remote CMPI daemon not yet supported
#endif
}

static void threadSleep(int msec)
{
#if defined(PEGASUS_HAVE_NANOSLEEP)

    struct timespec wait;
    wait.tv_sec = msec / 1000;
    wait.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&wait, NULL);

#elif defined(PEGASUS_PLATFORM_OS400_ISERIES_IBM)

   int loop;
   int microsecs = msec * 1000; /* convert from milliseconds to microseconds */

   if (microsecs < 1000000)
       usleep(microsecs);
   else
   {
       loop = microsecs / 1000000;
       for(int i = 0; i < loop; i++)
           usleep(1000000);
       if ((loop*1000000) < microsecs)
           usleep(microsecs - (loop*1000000));
   }

#elif defined(PEGASUS_PLATFORM_WIN64_IA64_MSVC) || \
      defined(PEGASUS_PLATFORM_WIN64_X86_64_MSVC) || \
      defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)

    if (msec == 0)
    {
        Sleep(0);
        return;
    }

    struct _timeb end, now;
    _ftime( &end );
    end.time += (msec / 1000);
    msec -= (msec / 1000);
    end.millitm += msec;

    do
    {
        Sleep(0);
        _ftime(&now);
    }
    while( end.millitm > now.millitm && end.time >= now.time);

#elif defined(PEGASUS_PLATFORM_ZOS_ZSERIES_IBM)
    int seconds;
    if (msec < 1000)
    {
        usleep(msec*1000);
    }
    else
    {
        // sleep for loop seconds
        sleep(msec / 1000);
        // Usleep the remaining micro seconds
        usleep( (msec*1000) % 1000000 );
    }
#elif defined(PEGASUS_OS_VMS)

    ::sleep(msec / 1000);

#endif
}

static CMPIBrokerExtFT brokerExt_FT={
     CMPICurrentVersion,
     resolveFileName,
     newThread,
     NULL,                      // Join not implemented yet
     NULL,                      // exit not implemented yet
     NULL,                      // cancel not implemented yet
     threadSleep,
     threadOnce,

     createThreadKey,
     destroyThreadKey,
     getThreadSpecific,
     setThreadSpecific,

     newMutex,
     destroyMutex,
     lockMutex,
     unlockMutex,

     newCondition,
     destroyCondition,
     condWait,
     timedCondWait,
     NULL                       // Signal not supported yet
};

CMPIBrokerExtFT *CMPI_BrokerExt_Ftab=&brokerExt_FT;




