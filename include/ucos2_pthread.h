/**
 * \file ucos2_pthread.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2004/02/03
 * 
 * \brief This header contains the pthread definitions needed to 
 *        support MiniGUI under uC/OS-II. 
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2004~2018, Beijing FMSoft Technologies Co., Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Or,

    As this program is a library, any link to this program must follow
    GNU General Public License version 3 (GPLv3). If you cannot accept
    GPLv3, you need to be licensed from FMSoft.

    If you have got a commercial license of this program, please use it
    under the terms and conditions of the commercial license.

    For more information about the commercial license, please refer to
    <http://www.minigui.com/en/about/licensing-policy/>.

 \endverbatim
 */

/*
 * $Id: ucos2_pthread.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef UCOSII_PTHREAD_H
#define UCOSII_PTHREAD_H

#ifdef __UCOSII__

#include <stddef.h>

/*=============================================================================
** Tunable macros
*/

#define NR_POSIX_PTHREAD_THREADS_MAX    16  /* This can not be less than 5. */
#define HIGHEST_UCOSII_PTHREAD_PRIORITY 16
#define LOWEST_UCOSII_PTHREAD_PRIORITY  (HIGHEST_UCOSII_PTHREAD_PRIORITY + NR_POSIX_PTHREAD_THREADS_MAX - 1)

#undef PTHREAD_KEYS_MAX
#define PTHREAD_KEYS_MAX                64

#undef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN               (PTHREAD_KEYS_MAX * sizeof(void *) + 1024*8)

#undef PTHREAD_DESTRUCTOR_ITERATIONS
#define PTHREAD_DESTRUCTOR_ITERATIONS  4

#define UCOSII_THREAD_LOWEST_PRIORITY   64

/*-----------------------------------------------------------------------------
** Internal types
*/

#define pthread_t ucos2_pthread_t
#define pthread_key_t ucos2_pthread_key_t
#define pthread_once_t ucos2_pthread_once_t
#define pthread_attr_t ucos2_pthread_attr_t
#define pthread_mutex_t ucos2_pthread_mutex_t
#define pthread_mutexattr_t ucos2_pthread_mutexattr_t

/*-----------------------------------------------------------------------------
** Basic types.
*/

typedef unsigned int pthread_t;
typedef int pthread_key_t;
typedef int pthread_once_t;

/*-----------------------------------------------------------------------------
** Scheduling parameters. At present only the priority is defined.
*/

struct sched_param
{
    int                 prio;
};

/*-----------------------------------------------------------------------------
** Thread attribute structure.
*/

typedef struct pthread_attr_t
{
    unsigned int        detachstate:2,
#if 0
    /* not support in uC/OS-II */
                        scope:2,
                        inheritsched:2,
                        schedpolicy:2,
    /* not support in uC/OS-II */
#endif
                        stackaddr_valid:1,
                        stacksize_valid:1;
    struct sched_param  schedparam;
    void                *stackaddr;
    size_t              stacksize;
} pthread_attr_t;

/* Values for detachstate */
#define PTHREAD_CREATE_JOINABLE	        1
#define PTHREAD_CREATE_DETACHED	        2

#if 0
/* not support in uC/OS-II */

/* Values for scope */
#define PTHREAD_SCOPE_SYSTEM            1
#define PTHREAD_SCOPE_PROCESS           2

/* Values for inheritsched */
#define PTHREAD_INHERIT_SCHED           1
#define PTHREAD_EXPLICIT_SCHED          2

#endif /* not support in uC/OS-II */

/*=============================================================================
** General thread operations
*/

/*-----------------------------------------------------------------------------
** Thread creation and management.
*/

/* Create a thread. */
int pthread_create ( pthread_t *thread,
                             const pthread_attr_t *attr,
                             void *(*start_routine) (void *),
                             void *arg);

/* Get current thread id. */
pthread_t pthread_self ( void );

/* Compare two thread identifiers. */
int pthread_equal (pthread_t thread1, pthread_t thread2);

/* Terminate current thread. */
void pthread_exit (void *retval);

/* Wait for the thread to terminate. If thread_return is not NULL then */
/* the retval from the thread's call to pthread_exit() is stored at */
/* *thread_return. */
int pthread_join (pthread_t thread, void **thread_return);

/* Set the detachstate of the thread to "detached". The thread then does not */
/* need to be joined and its resources will be freed when it exits. */
int pthread_detach (pthread_t thread);

#if 0
/* not support in uC/OS-II */
/*----------------------------------------------------------------------------- 
** Thread scheduling controls
*/

/* Set scheduling policy and parameters for the thread */
int pthread_setschedparam (pthread_t thread,
                                   int policy,
                                   const struct sched_param *param);

/* Get scheduling policy and parameters for the thread */
int pthread_getschedparam (pthread_t thread,
                                   int *policy,
                                   struct sched_param *param);
/* not support in uC/OS-II */
#endif 

/*-----------------------------------------------------------------------------
** Thread attribute handling.
*/

/*
** Initialize attributes object with default attributes:
** detachstate          == PTHREAD_JOINABLE
** scope                == PTHREAD_SCOPE_SYSTEM
** inheritsched         == PTHREAD_EXPLICIT_SCHED
** schedpolicy          == SCHED_OTHER
** schedparam           == unset
** stackaddr            == unset
** stacksize            == 0
*/ 
int pthread_attr_init (pthread_attr_t *attr);

/* Destroy thread attributes object */
int pthread_attr_destroy (pthread_attr_t *attr);


/* Set the detachstate attribute */
int pthread_attr_setdetachstate (pthread_attr_t *attr,
                                         int detachstate);

/* Get the detachstate attribute */
int pthread_attr_getdetachstate (const pthread_attr_t *attr,
                                         int *detachstate);


/* Set scheduling contention scope */
int pthread_attr_setscope (pthread_attr_t *attr, int scope);

/* Get scheduling contention scope */
int pthread_attr_getscope (const pthread_attr_t *attr, int *scope);


/* Set scheduling inheritance attribute */
int pthread_attr_setinheritsched (pthread_attr_t *attr, int inherit);

/* Get scheduling inheritance attribute */
int pthread_attr_getinheritsched (const pthread_attr_t *attr,
                                          int *inherit);


/* Set scheduling policy */
int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy);

/* Get scheduling policy */
int pthread_attr_getschedpolicy (const pthread_attr_t *attr,
                                         int *policy);


/* Set scheduling parameters */
int pthread_attr_setschedparam (pthread_attr_t *attr,
				        const struct sched_param *param);

/* Get scheduling parameters */
int pthread_attr_getschedparam (const pthread_attr_t *attr,
                                        struct sched_param *param);


/* Set starting address of stack. Whether this is at the start or end of */
/* the memory block allocated for the stack depends on whether the stack */
/* grows up or down. */
int pthread_attr_setstackaddr (pthread_attr_t *attr, void *stackaddr);

/* Get any previously set stack address. */
int pthread_attr_getstackaddr (const pthread_attr_t *attr,
                                       void **stackaddr);


/* Set minimum creation stack size. */
int pthread_attr_setstacksize (pthread_attr_t *attr,
                                       size_t stacksize);

/* Get current minimal stack size. */
int pthread_attr_getstacksize (const pthread_attr_t *attr,
                                       size_t *stacksize);


/*=============================================================================
** Dynamic package initialization
*/

/* Initializer for pthread_once_t instances */
#define PTHREAD_ONCE_INIT       0

/* Call init_routine just the once per control variable. */
int pthread_once (pthread_once_t *once_control,
                          void (*init_routine) (void));


/*=============================================================================
**Thread specific data
*/

/* Create a key to identify a location in the thread specific data area. */
/* Each thread has its own distinct thread-specific data area but all are */
/* addressed by the same keys. The destructor function is called whenever a */
/* thread exits and the value associated with the key is non-NULL. */
int pthread_key_create (pthread_key_t *key,
                                void (*destructor) (void *));

/* Delete key. */
int pthread_key_delete (pthread_key_t key);

/* Store the pointer value in the thread-specific data slot addressed */
/* by the key. */
int pthread_setspecific (pthread_key_t key, const void *pointer);

/* Retrieve the pointer value in the thread-specific data slot addressed */
/* by the key. */
void *pthread_getspecific (pthread_key_t key);


/*=============================================================================
** Thread Cancellation
*/

/*-----------------------------------------------------------------------------
** Data structure used to manage cleanup functions
*/

struct pthread_cleanup_buffer
{
    struct pthread_cleanup_buffer *prev;        /* Chain cleanup buffers */
    void (*routine) (void *);     	            /* Function to call */
    void *arg;				                    /* Arg to pass */
};

/*-----------------------------------------------------------------------------
** Thread cancelled return value.
** This is a value returned as the retval in pthread_join() of a
** thread that has been cancelled. By making it the address of a
** location we define we can ensure that it differs from NULL and any
** other valid pointer (as required by the standard).
*/

extern int pthread_canceled_dummy_var;

#define PTHREAD_CANCELED                ((void *)(&pthread_canceled_dummy_var))


/*-----------------------------------------------------------------------------
** Cancelability enable and type
*/

#define PTHREAD_CANCEL_ENABLE           1
#define PTHREAD_CANCEL_DISABLE          2

#define PTHREAD_CANCEL_ASYNCHRONOUS     1
#define PTHREAD_CANCEL_DEFERRED         2

/*-----------------------------------------------------------------------------
** Functions
*/

/* Set cancel state of current thread to ENABLE or DISABLE. */
/* Returns old state in *oldstate. */
int pthread_setcancelstate (int state, int *oldstate);

/* Set cancel type of current thread to ASYNCHRONOUS or DEFERRED. */
/* Returns old type in *oldtype. */
int pthread_setcanceltype (int type, int *oldtype);

/* Cancel the thread. */
int pthread_cancel (pthread_t thread);

/* Test for a pending cancellation for the current thread and terminate */
/* the thread if there is one. */
void pthread_testcancel (void);

/* Install a cleanup routine. */
/* Note that pthread_cleanup_push() and pthread_cleanup_pop() are macros that */
/* must be used in matching pairs and at the same brace nesting level. */
#define pthread_cleanup_push(routine,arg)                       \
    {                                                           \
        struct pthread_cleanup_buffer _buffer_;                 \
        pthread_cleanup_push_inner (&_buffer_, (routine), (arg));

/* Remove a cleanup handler installed by the matching pthread_cleanup_push(). */
/* If execute is non-zero, the handler function is called. */
#define pthread_cleanup_pop(execute)                            \
        pthread_cleanup_pop_inner (&_buffer_, (execute));       \
    }


/* These two functions actually implement the cleanup push and pop function. */
void pthread_cleanup_push_inner (struct pthread_cleanup_buffer *buffer,
                                         void (*routine) (void *),
                                         void *arg);

void pthread_cleanup_pop_inner (struct pthread_cleanup_buffer *buffer,
                                        int execute);


/*-----------------------------------------------------------------------------
** Mutex object
*/

typedef struct
{
    void*   os_mutex;
} pthread_mutex_t;

/* We do not implement PTHREAD_MUTEX_INITIALIZER */
#undef PTHREAD_MUTEX_INITIALIZER

/*-----------------------------------------------------------------------------
** Mutex attributes structure
*/

typedef struct
{
    unsigned char prio;
} pthread_mutexattr_t;

/*=============================================================================
** Mutexes
*/

/*-----------------------------------------------------------------------------
** Mutex attributes manipulation functions
*/

/* Initialize attribute object */
int pthread_mutexattr_init ( pthread_mutexattr_t *attr);

/* Destroy attribute object */
int pthread_mutexattr_destroy ( pthread_mutexattr_t *attr);

/* Set priority */
int pthread_mutexattr_setpriority ( pthread_mutexattr_t *attr, int priority);

/* Get priority */
int pthread_mutexattr_getpriority ( pthread_mutexattr_t *attr, int* priority);

/*-----------------------------------------------------------------------------
** Mutex functions
*/

/* Initialize mutex. If mutex_attr is NULL, use default attributes. */
int pthread_mutex_init (pthread_mutex_t *mutex,
                                const pthread_mutexattr_t *mutex_attr);

/* Destroy mutex. */
int pthread_mutex_destroy (pthread_mutex_t *mutex);

/* Lock mutex, waiting for it if necessary. */
int pthread_mutex_lock (pthread_mutex_t *mutex);

/* Try to lock mutex. */
int pthread_mutex_trylock (pthread_mutex_t *mutex);


/* Unlock mutex. */
int pthread_mutex_unlock (pthread_mutex_t *mutex);

/* We do not implement pthread condition variable */
#if 0
/*-----------------------------------------------------------------------------
** Condition Variable structure.
** Like mutexes, this must match the underlying eCos implementation class.
*/

typedef struct
{
    int         dummy;
} pthread_cond_t;

#define PTHREAD_COND_INITIALIZER { 0, 0 }

/*-----------------------------------------------------------------------------
** Condition variable attributes structure
*/

typedef struct
{
    int         dummy;
} pthread_condattr_t;

/*=============================================================================
** Condition Variables
*/

/*-----------------------------------------------------------------------------
** Attribute manipulation functions
** We do not actually support any attributes at present, so these do nothing.
*/

/* Initialize condition variable attributes */
int pthread_condattr_init (pthread_condattr_t *attr);

/* Destroy condition variable attributes */
int pthread_condattr_destroy (pthread_condattr_t *attr);

/*-----------------------------------------------------------------------------
** Condition variable functions
*/

/* Initialize condition variable. */
int pthread_cond_init (pthread_cond_t *cond,
                               const pthread_condattr_t *attr);

/* Destroy condition variable. */
int pthread_cond_destroy (pthread_cond_t *cond);

/* Wake up one thread waiting for condition variable */
int pthread_cond_signal (pthread_cond_t *cond);

/* Wake up all threads waiting for condition variable */
int pthread_cond_broadcast (pthread_cond_t *cond);

/* Block on condition variable until signalled. The mutex is */
/* assumed to be locked before this call, will be unlocked */
/* during the wait, and will be re-locked on wakeup. */
int pthread_cond_wait (pthread_cond_t *cond,
                               pthread_mutex_t *mutex);

/* Block on condition variable until signalled, or the timeout expires. */
int pthread_cond_timedwait (pthread_cond_t *cond,
                                    pthread_mutex_t *mutex,
                                    const struct timespec *abstime);

/* We do not implement pthread condition variable*/
#endif

#endif /* __UCOSII__ */
#endif /* UCOSII_PTHREAD_H */

