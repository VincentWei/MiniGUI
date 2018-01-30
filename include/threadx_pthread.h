/**
 * \file threadx_pthread.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2005/01/11
 * 
 * \brief This is the POSIX PThreads implementation in order to 
 *        run MiniGUI on ThreadX.
 *
 \verbatim

    This header contains all the definitions needed to support
    pthreads under ThreadX. The reader is referred to the POSIX
    standard or equivalent documentation for details of the
    functionality contained herein.
  
    We do this work in order to run MiniGUI on ThreadX.

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2005~2018, Beijing FMSoft Technologies Co., Ltd.

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
 * $Id: threadx_pthread.h 11349 2009-03-02 05:00:43Z weiym $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef FMTX_PTHREAD_H
#define FMTX_PTHREAD_H

#include <stddef.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*-----------------------------------------------------------------------------
** Basic types.
*/

#ifndef BOOL
    #define BOOL    int
#endif
#ifndef TRUE
    #define TRUE    1
#endif
#ifndef FALSE
    #define FALSE   0
#endif

#define TXPTH_LEN_NAME              12

typedef unsigned int pthread_t;
typedef int pthread_key_t;
typedef int pthread_once_t;

/*-----------------------------------------------------------------------------
** Scheduling parameters. At present only the priority is defined.
*/

struct sched_param
{
    unsigned int priority;
    unsigned int preempt_threshold;
    unsigned long time_slice;
};

/*-----------------------------------------------------------------------------
** Thread attribute structure.
*/

typedef struct pthread_attr_t
{
    unsigned int        detachstate:2,
                        scope:2,
                        inheritsched:2,
                        schedpolicy:2,
                        stackaddr_valid:1,
                        stacksize_valid:1;
    struct sched_param  schedparam;
    void                *stackaddr;
    size_t              stacksize;
} pthread_attr_t;

/* Values for detachstate */
#define PTHREAD_CREATE_JOINABLE	        1
#define PTHREAD_CREATE_DETACHED	        2

/* Values for scope */
#define PTHREAD_SCOPE_SYSTEM            1
#define PTHREAD_SCOPE_PROCESS           2

/* Values for inheritsched */
#define PTHREAD_INHERIT_SCHED           1
#define PTHREAD_EXPLICIT_SCHED          2

#define SCHED_OTHER                     1
#define SCHED_RR                        2
#define SCHED_FIFO                      3

#define PTHREAD_STACK_MIN               (1024*2)
#define PTHREAD_STACK_DEFAULT           (1024*4)

#ifdef PTHREAD_KEYS_MAX
    #undef PTHREAD_KEYS_MAX
#endif
#define PTHREAD_KEYS_MAX                64

#ifdef PTHREAD_DESTRUCTOR_ITERATIONS
    #undef PTHREAD_DESTRUCTOR_ITERATIONS
#endif
#define PTHREAD_DESTRUCTOR_ITERATIONS  4

/*=============================================================================
** General thread operations
*/

/*-----------------------------------------------------------------------------
** Thread creation and management.
*/

/* Create a thread. */
int pthread_create (pthread_t *thread,
                             const pthread_attr_t *attr,
                             void *(*start_routine) (void *),
                             void *arg);

/* Get current thread id. */
pthread_t pthread_self (void);

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


/*=============================================================================
** Mutexes
*/

/*-----------------------------------------------------------------------------
**pthread_mutex_t
*/

typedef struct TXPTH_MUTEX_STRUCT
{
    unsigned long tx_mutex_id;
    char* tx_mutex_name;
    unsigned long tx_mutex_ownership_count;
    void *tx_mutex_owner;
    unsigned int tx_mutex_inherit;
    unsigned int tx_mutex_original_priority;
    unsigned int tx_mutex_original_threshold;
    void *tx_mutex_suspension_list;
    unsigned long tx_mutex_suspended_count;
    void *tx_mutex_created_next;
    void *tx_mutex_created_previous;
} TXPTH_MUTEX;

typedef struct  _pthread_mutex_t 
{
	TXPTH_MUTEX tx_mutex;
	char     name [TXPTH_LEN_NAME];
} pthread_mutex_t;

/*define pthread_mutexattr_t */
typedef struct _pthread_mutexattr_t 
{
	unsigned int protocol;
} pthread_mutexattr_t;

/*-----------------------------------------------------------------------------
** Mutex attributes manipulation functions
*/

/* Initialize attribute object */
int pthread_mutexattr_init ( pthread_mutexattr_t *attr);

/* Destroy attribute object */
int pthread_mutexattr_destroy ( pthread_mutexattr_t *attr);

/* Set/Get the priority inherit protocol */
int pthread_mutexattr_setprotocol (pthread_mutexattr_t *attr, int protocol);
int pthread_mutexattr_getprotocol (pthread_mutexattr_t *attr, int* protocol);

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

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !FMTX_PTHREAD_H */

/* End of threadx_pthread.h */

