/*
 *   This file is part of MiniGUI, a mature cross-platform windowing 
 *   and Graphics User Interface (GUI) support system for embedded systems
 *   and smart IoT devices.
 * 
 *   Copyright (C) 2002~2018, Beijing FMSoft Technologies Co., Ltd.
 *   Copyright (C) 1998~2002, WEI Yongming
 * 
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 * 
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 * 
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *   Or,
 * 
 *   As this program is a library, any link to this program must follow
 *   GNU General Public License version 3 (GPLv3). If you cannot accept
 *   GPLv3, you need to be licensed from FMSoft.
 * 
 *   If you have got a commercial license of this program, please use it
 *   under the terms and conditions of the commercial license.
 * 
 *   For more information about the commercial license, please refer to
 *   <http://www.minigui.com/en/about/licensing-policy/>.
 */
/*
** ucos2_pthread.c: implementation of pthread function under uC/OS-II.
**
** Current maintainer: Wei Yongming
**
** Create date: 2004-02-02
*/

#include "mgconfig.h"

#if defined(__UCOSII__) && defined(_MGUSE_OWN_PTHREAD)

#include <errno.h>
#include "os_cpu.h"
#include "os_cfg.h"
#include "ucos_ii.h"

#define _HAVE_TYPE_BYTE 1
#define _HAVE_TYPE_WORD 1
#define _HAVE_TYPE_LONG 1

#include "common.h"
#include "ucos2_pprivate.h"
#include "own_stdio.h"

//-----------------------------------------------------------------------------
// Internal definitions

// Handle entry to a pthread package function. 
#define PTHREAD_ENTRY()

// Handle entry to a pthread package function with no args. 
#define PTHREAD_ENTRY_VOID()

#define _REPORT_RETVAL(err)
#define _REPORT_RETURN()

#define _MACRO_START   do {
#define _MACRO_END     } while (0);

#define PTHREAD_FAIL(info)

// Do a pthread package defined return. This requires the error code to be
// returned as the result of the function. This also gives us a place to
// put any generic tidyup handling needed for things like signal delivery
// and cancellation.
#define PTHREAD_RETURN(err)                     \
_MACRO_START                                    \
    _REPORT_RETVAL( err );                      \
    return err;                                 \
_MACRO_END

// A void variant of the above.
#define PTHREAD_RETURN_VOID                     \
_MACRO_START                                    \
    _REPORT_RETURN();                           \
    return;                                     \
_MACRO_END

// Check that a pointer passed in as an argument is valid and return
// EINVAL if it is not. This should be used to check pointers that are
// required to be valid. Pointers that may optionally be NULL should
// be checked within the function.
#define PTHREAD_CHECK(ptr) if( (ptr) == NULL ) PTHREAD_RETURN(EINVAL);

// Mutex for controlling access to shared data structures
static pthread_mutex_t pthread_mutex;

// Array of pthread control structures. A pthread_t object is
// "just" an index into this array.
static pthread_info thread_table[NR_POSIX_PTHREAD_THREADS_MAX];

//-----------------------------------------------------------------------------
// Thread cancelled return value.
// This is a value returned as the retval in pthread_join() of a
// thread that has been cancelled. By making it the address of a
// location we define we can ensure that it differs from NULL and any
// other valid pointer (as required by the standard).

int pthread_canceled_dummy_var;

// Per-thread key allocation. This key map has a 1 bit set for each
// key that is free, zero if it is allocated.
#define KEY_MAP_TYPE Uint32
#define KEY_MAP_TYPE_SIZE (sizeof(KEY_MAP_TYPE)*8) // in BITS!
static KEY_MAP_TYPE thread_key[PTHREAD_KEYS_MAX/KEY_MAP_TYPE_SIZE];
static void (*key_destructor[PTHREAD_KEYS_MAX]) (void *);
    
//=============================================================================
// Internal functions

//-----------------------------------------------------------------------------
// Private version of pthread_self() that returns a pointer to our internal
// control structure.

static pthread_info *pthread_self_info(void)
{
    int cur_prio, index;
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR cpu_sr = 0;
#endif

    OS_ENTER_CRITICAL ();
    cur_prio = OSPrioCur;
    OS_EXIT_CRITICAL ();

    index = cur_prio - HIGHEST_UCOSII_PTHREAD_PRIORITY;

    if (index >= NR_POSIX_PTHREAD_THREADS_MAX || index < 0)
        return NULL;

    return thread_table + index;
}

static pthread_info *pthread_info_id ( pthread_t id )
{
    pthread_info *info;
    int index = id - HIGHEST_UCOSII_PTHREAD_PRIORITY;

    if (index >= NR_POSIX_PTHREAD_THREADS_MAX || index < 0)
        return NULL;

    info = thread_table + index;

    // Check for a valid entry
    if( info == NULL )
        return NULL;
    
    // Return the pointer
    return info;
}

static inline void* pthread_malloc( size_t size )
{
    return (void*)malloc( size );
}

static inline void pthread_free( void* m )
{
    free( (void *)m );
}

//-----------------------------------------------------------------------------
// pthread entry function.
// does some housekeeping and then calls the user's start routine.

static void pthread_entry (void* data)
{
    void* retval;
    pthread_info *self = (pthread_info *)data;

    retval = self->start_routine (self->start_arg);

    pthread_exit (retval);
}

//-----------------------------------------------------------------------------
// Check whether there is a cancel pending and if so, whether
// cancellations are enabled. We do it in this order to reduce the
// number of tests in the common case - when no cancellations are
// pending.
// We make this inline so it can be called directly below for speed

static inline int checkforcancel (void)
{
     pthread_info *self = pthread_self_info();

    if (self != NULL &&
        self->cancelpending &&
        self->cancelstate == PTHREAD_CANCEL_ENABLE)
        return 1;
    else
        return 0;
}

//-----------------------------------------------------------------------------
// POSIX ASR
// This is installed as the ASR for all POSIX threads.

//-----------------------------------------------------------------------------
// The (Grim) Reaper.
// This function is called to tidy up and dispose of any threads that have
// exited. This work must be done from a thread other than the one exiting.
// Note: this function _must_ be called with pthread_mutex locked.

static void pthread_reap (void)
{
    int i;

    // Loop over the thread table looking for exited threads. The
    // counter springs us out of this once we have
    // found them all (and keeps us out if there are none to do).
   
    for (i = 0; i < NR_POSIX_PTHREAD_THREADS_MAX; i++) {
        INT8U err;
        pthread_info *thread = thread_table + i;

        if (thread->state == PTHREAD_STATE_EXITED) {
#if 0
            // The thread has exited, so it is a candidate for being
            // reaped. We have to make sure that the OS thread has
            // also reached EXITED state before we can tidy it up.

            while( thread->thread->get_state() != Cyg_Thread::EXITED )
            {
                // The OS thread has not yet exited. This is
                // probably because its priority is too low to allow
                // it to complete.  We fix this here by raising its
                // priority to equal ours and then yielding. This
                // should eventually get it into exited state.

                Cyg_Thread *self = Cyg_Thread::self();

                // Set thread's priority to our current dispatching priority.
                thread->thread->set_priority( self->get_current_priority() );

                // Yield, yield
                self->yield();
    
                // and keep looping until he exits.
            }

            // At this point we have a thread that we can reap.
#endif

            // destroy the joiner condvar
            OSSemDel (thread->joiner, OS_DEL_ALWAYS, &err);

            // Free the stack if we allocated it
            if( thread->freestack )
                pthread_free( thread->stackmem );

            // Finally, set the thread table entry to be freed so that it
            // may be reused.
            thread->state = PTHREAD_STATE_FREE;
        }
    }
}

//=============================================================================
// Functions exported to rest of MiniGUI.

/*----------------------------------------------------------------------------*/
/* Main thread. */

/* Thread ID of main thread. */
static pthread_t main_thread;

/* -------------------------------------------------------------------------- */
/* Main entry function.
 * This is set as the start_routine of the main thread. 
 * It invokes the entry function passed by thread argument.
 */

struct _main_pth_entry_info 
{
    int (* pth_entry) (int argc, const char* argv []);
    int argc;
    const char** argv;
};

static void *main_pthread_entry (void *data)
{
    struct _main_pth_entry_info* entry_info 
        = (struct _main_pth_entry_info*) data;

    entry_info->pth_entry (entry_info->argc, entry_info->argv);

    return NULL; /* placate compiler */
}

/* -------------------------------------------------------------------------- */
/* Start Pthreads system and create the main() thread. */

int start_minigui_pthread (int (* pth_entry) (int argc, const char* argv []),
                int argc, const char* argv[], 
                char* stack_base, unsigned int stack_size)
{
    int i;

    /* Initialize the global mutex object */
    if (pthread_mutex_init (&pthread_mutex, NULL)) {
        PTHREAD_FAIL ("PThread: Can not create global mutex object.\n");
        return 1;
    }

    /* TODO: Initialize other global object */

    /* Initialize the per-thread data key map. */
    for (i = 0; i < (PTHREAD_KEYS_MAX/KEY_MAP_TYPE_SIZE); i++) {
        thread_key [i] = ~0;
    }
    
    /* Create the main thread */
    if (pth_entry) {
        pthread_attr_t attr;
        struct sched_param schedparam;
        struct _main_pth_entry_info entry_info;

        entry_info.pth_entry = pth_entry; 
        entry_info.argc = argc; 
        entry_info.argv = argv;

        if (stack_size < MAIN_PTH_MIN_STACK_SIZE) {
            PTHREAD_FAIL ("PThread: Too small stack size for main pthread.\n");
            return 2;
        }

        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
        pthread_attr_setstackaddr (&attr, stack_base + stack_size);
        pthread_attr_setstacksize (&attr, stack_size);

        pthread_create (&main_thread, &attr, main_pthread_entry, &entry_info);
    }

    return 0;
}

//=============================================================================
// General thread operations

//-----------------------------------------------------------------------------
// Thread creation and management.

// Create a thread.
int pthread_create ( pthread_t *thread,
                             const pthread_attr_t *attr,
                             void *(*start_routine) (void *),
                             void *arg)
{
    int ucos2_prio;
    OS_STK* stackbase;
    size_t stacksize;
    BOOL freestack = FALSE;
    void* stackmem = 0;
    pthread_info *nthread;
    INT8U err;
    
    pthread_attr_t use_attr;

    PTHREAD_ENTRY();

    PTHREAD_CHECK(thread);
    PTHREAD_CHECK(start_routine);

    // Set use_attr to the set of attributes we are going to
    // actually use. Either those passed in, or the default set.
   
    if( attr == NULL )
        pthread_attr_init (&use_attr);
    else use_attr = *attr;

    // If the stack size is not valid, we can assume that it is at
    // least PTHREAD_STACK_MIN bytes.
    if (use_attr.stacksize_valid)
        stacksize = use_attr.stacksize;
    else
        stacksize = PTHREAD_STACK_MIN;

    if (use_attr.stackaddr_valid) {
        // Set up stack base and size from supplied arguments.

        // Calculate stack base from address and size.
        // FIXME: Falling stack assumed in pthread_create().
        stackbase = stackmem = (Uint8*)use_attr.stackaddr-stacksize;
    }
    else {
        stackbase = stackmem = pthread_malloc (stacksize);

        if( stackmem == 0 )
            PTHREAD_RETURN( EAGAIN );

        freestack = TRUE;
    }

    // Get sole access to data structures
    
    pthread_mutex_lock (&pthread_mutex);
    
    // Dispose of any dead threads
    pthread_reap();
    
    // Find a free slot in the thread table
    
    nthread = NULL;
    ucos2_prio = PTHREAD_UCOSII_PRIORITY(use_attr.schedparam.prio);
    if (use_attr.schedparam.prio == 0) {
        int i;
        for (i = 0; i < NR_POSIX_PTHREAD_THREADS_MAX; i++) {
            if (thread_table [i].state == PTHREAD_STATE_FREE) {
                nthread = thread_table + i;
                break;
            }
        }
    }
    else if (ucos2_prio >= HIGHEST_UCOSII_PTHREAD_PRIORITY
              && ucos2_prio <= LOWEST_UCOSII_PTHREAD_PRIORITY) {
        if (thread_table [ucos2_prio].state == PTHREAD_STATE_FREE)
            nthread  = thread_table + ucos2_prio - HIGHEST_UCOSII_PTHREAD_PRIORITY;
        else
            PTHREAD_RETURN (EINVAL);
    }

    ucos2_prio = nthread - thread_table + HIGHEST_UCOSII_PTHREAD_PRIORITY;

    if (nthread == NULL) {
        pthread_mutex_unlock (&pthread_mutex);
        if( freestack )
            pthread_free( stackmem );
        PTHREAD_RETURN (ENOMEM);
    }

    // Initialize the table entry
    nthread->state              = use_attr.detachstate == PTHREAD_CREATE_JOINABLE ?
                                  PTHREAD_STATE_RUNNING : PTHREAD_STATE_DETACHED;
    nthread->id                 = ucos2_prio;
    nthread->attr               = use_attr;
    nthread->retval             = 0;
    nthread->start_routine      = start_routine;
    nthread->start_arg          = arg;

    nthread->freestack          = freestack;
    nthread->stackmem           = stackmem;
    
    nthread->cancelstate        = PTHREAD_CANCEL_ENABLE;
    nthread->canceltype         = PTHREAD_CANCEL_DEFERRED;
    nthread->cancelbuffer       = NULL;
    nthread->cancelpending      = FALSE;

    nthread->thread_data        = NULL;
    
    // Initialize the joiner semaphore.
    nthread->joiner             = OSSemCreate (0);
    nthread->nr_joined          = 0;

    // create the underlying uC/OS-II task
    err = OSTaskCreate (pthread_entry, (void*)nthread, 
                stackbase + stacksize/sizeof(OS_STK) - 1, ucos2_prio);

    if (err == OS_NO_ERR) {
        // return thread ID
        *thread = nthread - thread_table;

        pthread_mutex_unlock (&pthread_mutex);

        PTHREAD_RETURN(0);
    }
    else {
        nthread->state = PTHREAD_STATE_FREE;
        pthread_mutex_unlock (&pthread_mutex);
        if (freestack)
            pthread_free (stackmem);
    }

    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Get current thread id.

pthread_t pthread_self ( void )
{
    INT8U cur_prio;
    int index;
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR cpu_sr = 0;
#endif

    PTHREAD_ENTRY();
    
    OS_ENTER_CRITICAL ();
    cur_prio = OSPrioCur;
    OS_EXIT_CRITICAL ();

    index = cur_prio - HIGHEST_UCOSII_PTHREAD_PRIORITY;
    if (index >= NR_POSIX_PTHREAD_THREADS_MAX || index < 0)
        return -1;

    return index;
}

//-----------------------------------------------------------------------------
// Compare two thread identifiers.

int pthread_equal (pthread_t thread1, pthread_t thread2)
{
    PTHREAD_ENTRY();
    
    return thread1 == thread2;
}

//-----------------------------------------------------------------------------
// Terminate current thread.

void pthread_exit (void *retval)
{
    pthread_info *self;

    PTHREAD_ENTRY();

    self = pthread_self_info();
    
    // Call cancellation handlers. We eat up the buffers as we go in
    // case any of the routines calls pthread_exit() itself.
    while (self->cancelbuffer != NULL) {
        struct pthread_cleanup_buffer *buffer = self->cancelbuffer;

        self->cancelbuffer = buffer->prev;

        buffer->routine(buffer->arg);
    }

    if (self->thread_data != NULL) {
        // Call per-thread key destructors.
        // The specification of this is that we must continue to call the
        // destructor functions until all the per-thread data values are NULL or
        // we have done it PTHREAD_DESTRUCTOR_ITERATIONS times.
    
        BOOL destructors_called;
        int destructor_iterations = 0;
        Uint32 key;

        do {
            destructors_called = FALSE;

            for (key = 0; key < PTHREAD_KEYS_MAX; key++) {
                // Skip unallocated keys
                if( thread_key[key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE) )
                    continue;

                // Skip NULL destructors
                if( key_destructor[key] == NULL ) continue;

                // Skip NULL data values
                if( self->thread_data[key] == NULL ) continue;

                // If it passes all that, call the destructor.
                // Note that NULLing the data value here is new
                // behaviour in the 2001 POSIX standard.
                {
                    void* value = self->thread_data[key];
                    self->thread_data[key] = NULL;
                    key_destructor[key](value);
                }

                // Record that we called a destructor
                destructors_called = TRUE;
            }

            // Count the iteration
            destructor_iterations++;
        
        } while( destructors_called &&
                 (destructor_iterations <= PTHREAD_DESTRUCTOR_ITERATIONS));
    }
    
    pthread_mutex_lock (&pthread_mutex);

    // Set the retval for any joiner
    self->retval = retval;

    // If we are already detached, go to EXITED state, otherwise
    // go into JOIN state.

    if (PTHREAD_STATE_DETACHED == self->state) {
        self->state = PTHREAD_STATE_EXITED;
    } else {
        self->state = PTHREAD_STATE_JOIN;
    }

    // Kick any waiting joiners
    while (self->nr_joined--) {
        OSSemPost (self->joiner);
    }

    pthread_mutex_unlock (&pthread_mutex);
    
    // Finally, call the exit function; this will not return.
    OSTaskDel (self->id);

    // This loop keeps some compilers happy. pthread_exit() is marked
    // with the noreturn attribute, and without this they generate a
    // call to abort() here in case Cyg_Thread::exit() returns. 
    
    for(;;) continue;
}

//-----------------------------------------------------------------------------
// Wait for the thread to terminate. If thread_return is not NULL then
// the retval from the thread's call to pthread_exit() is stored at
// *thread_return.

int pthread_join (pthread_t thread, void **thread_return)
{
    int err = 0;
    pthread_info *self, *joinee;

    PTHREAD_ENTRY();
    
    // check for cancellation first.
    pthread_testcancel();

    pthread_mutex_lock(&pthread_mutex);
    
    // Dispose of any dead threads
    pthread_reap();
    
    self = pthread_self_info ();
    joinee = pthread_info_id (thread);

    if (joinee == NULL) {
        err = ESRCH;
    }

    if (!err && joinee == self) {
        err = EDEADLK;
    }

    if (!err) {
        switch (joinee->state) {
        case PTHREAD_STATE_RUNNING:
            // The thread is still running, we must wait for it.
            while (joinee->state == PTHREAD_STATE_RUNNING) {
                INT8U ucos2_err;

                OSSemPend (joinee->joiner, 0, &ucos2_err);
                // check if we were woken because we were being cancelled
                if ( checkforcancel() ) {
                    err = EAGAIN;  // value unimportant, just some error
                    break;
                }
            }

            // check that the thread is still joinable
            if (joinee->state == PTHREAD_STATE_JOIN)
                break;

        // The thread has become unjoinable while we waited, so we
        // fall through to complain.
        case PTHREAD_STATE_FREE:
        case PTHREAD_STATE_DETACHED:
        case PTHREAD_STATE_EXITED:
            // None of these may be joined.
            err = EINVAL;
            break;
            
        case PTHREAD_STATE_JOIN:
            break;
        }
    }

    if (!err) {
    
        // here, we know that joinee is a thread that has exited and is
        // ready to be joined.

        // Get the retval
        if( thread_return != NULL )
            *thread_return = joinee->retval;
        
        // set state to exited.
        joinee->state = PTHREAD_STATE_EXITED;
    
        // Dispose of any dead threads
        pthread_reap ();
    }

    pthread_mutex_unlock (&pthread_mutex);
    
    // check for cancellation before returning
    pthread_testcancel();

    PTHREAD_RETURN(err);
}

//-----------------------------------------------------------------------------
// Set the detachstate of the thread to "detached". The thread then does not
// need to be joined and its resources will be freed when it exits.

int pthread_detach (pthread_t thread)
{
    int ret = 0;
    pthread_info *detachee;
    
    PTHREAD_ENTRY ();
    
    pthread_mutex_lock (&pthread_mutex);

    detachee = pthread_info_id (thread);
    
    if (detachee == NULL)
        ret = ESRCH;                    // No such thread
    else if (detachee->state == PTHREAD_STATE_DETACHED)
        ret = EINVAL;                   // Already detached!
    else {
        // Set state to detached and kick any joinees to
        // make them return.
        detachee->state = PTHREAD_STATE_DETACHED;
        while (detachee->nr_joined--) {
            OSSemPost (detachee->joiner);
        }
    }
    
    // Dispose of any dead threads
    pthread_reap ();
    
    pthread_mutex_unlock (&pthread_mutex);

    PTHREAD_RETURN (ret);
}


#if 0 /* not support in uC/OS-II */

//-----------------------------------------------------------------------------
// Thread scheduling controls

//-----------------------------------------------------------------------------
// Set scheduling policy and parameters for the thread

int pthread_setschedparam (pthread_t thread_id,
                                   int policy,
                                   const struct sched_param *param)
{
    PTHREAD_ENTRY();

    if( policy != SCHED_OTHER &&
        policy != SCHED_FIFO &&
        policy != SCHED_RR )
        PTHREAD_RETURN(EINVAL);

    PTHREAD_CHECK(param);

    // The parameters seem OK, change the thread...
    
    pthread_mutex_lock(&pthread_mutex);

    pthread_info *thread = pthread_info_id( thread_id );

    if( thread == NULL )
    {
        pthread_mutex_unlock(&pthread_mutex);
        PTHREAD_RETURN(ESRCH);
    }
    
    thread->attr.schedpolicy = policy;
    thread->attr.schedparam = *param;

    if ( policy == SCHED_FIFO )
         thread->thread->timeslice_disable();
    else thread->thread->timeslice_enable();

    thread->thread->set_priority( PTHREAD_ECOS_PRIORITY( param->prio ));

    pthread_mutex_unlock(&pthread_mutex);
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get scheduling policy and parameters for the thread

int pthread_getschedparam (pthread_t thread_id,
                                   int *policy,
                                   struct sched_param *param)
{
    PTHREAD_ENTRY();

    pthread_mutex_lock(&pthread_mutex);

    pthread_info *thread = pthread_info_id( thread_id );

    if( thread == NULL )
    {
        pthread_mutex_unlock(&pthread_mutex);
        PTHREAD_RETURN(ESRCH);
    }

    if( policy != NULL )
        *policy = thread->attr.schedpolicy;

    if( param != NULL )
        *param = thread->attr.schedparam;
    
    pthread_mutex_unlock(&pthread_mutex);
    
    PTHREAD_RETURN(0);
}

#endif /* not support in uC/OS-II */

//-----------------------------------------------------------------------------
// Thread attribute handling.

//-----------------------------------------------------------------------------
// Initialize attributes object with default attributes:
// detachstate          == PTHREAD_CREATE_JOINABLE
// scope                == PTHREAD_SCOPE_SYSTEM
// inheritsched         == PTHREAD_INHERIT_SCHED
// schedpolicy          == SCHED_OTHER
// schedparam           == unset
// stackaddr            == unset
// stacksize            == 0
// 

int pthread_attr_init (pthread_attr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);
    
    attr->detachstate                 = PTHREAD_CREATE_JOINABLE;
#if 0  /* not support in uC/OS-II */
    attr->scope                       = PTHREAD_SCOPE_SYSTEM;
    attr->inheritsched                = PTHREAD_INHERIT_SCHED;
    attr->schedpolicy                 = SCHED_OTHER;
#endif /* not support in uC/OS-II */
    attr->schedparam.prio             = 0;
    attr->stackaddr_valid             = 0;    
    attr->stackaddr                   = NULL;
    attr->stacksize_valid             = 0;    
    attr->stacksize                   = 0;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Destroy thread attributes object

int pthread_attr_destroy (pthread_attr_t *attr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // Nothing to do here...
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set the detachstate attribute

int pthread_attr_setdetachstate (pthread_attr_t *attr,
                                         int detachstate)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( detachstate == PTHREAD_CREATE_JOINABLE ||
            detachstate == PTHREAD_CREATE_DETACHED )
    {
        attr->detachstate = detachstate;
        PTHREAD_RETURN(0);
    }
    
    PTHREAD_RETURN(EINVAL);
}

//-----------------------------------------------------------------------------
// Get the detachstate attribute
int pthread_attr_getdetachstate (const pthread_attr_t *attr,
                                         int *detachstate)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( detachstate != NULL )
        *detachstate = attr->detachstate;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set scheduling contention scope

int pthread_attr_setscope (pthread_attr_t *attr, int scope)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

#if 0  /* not support in uC/OS-II */
    if( scope == PTHREAD_SCOPE_SYSTEM ||
        scope == PTHREAD_SCOPE_PROCESS )
    {
        if( scope == PTHREAD_SCOPE_PROCESS )
            PTHREAD_RETURN(ENOTSUP);

        attr->scope = scope;

        PTHREAD_RETURN(0);
    }
#endif /* not support in uC/OS-II */
    
    PTHREAD_RETURN(EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling contention scope

int pthread_attr_getscope (const pthread_attr_t *attr, int *scope)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

#if 0  /* not support in uC/OS-II */
    if( scope != NULL )
        *scope = attr->scope;
    
    PTHREAD_RETURN(0);
#else  /* not support in uC/OS-II */
    PTHREAD_RETURN(EINVAL);
#endif /* not support in uC/OS-II */
}

//-----------------------------------------------------------------------------
// Set scheduling inheritance attribute

int pthread_attr_setinheritsched (pthread_attr_t *attr, int inherit)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

#if 0  /* not support in uC/OS-II */
    if( inherit == PTHREAD_INHERIT_SCHED ||
        inherit == PTHREAD_EXPLICIT_SCHED )
    {
        attr->inheritsched = inherit;

        PTHREAD_RETURN(0);
    }
#endif /* not support in uC/OS-II */

    PTHREAD_RETURN(EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling inheritance attribute

int pthread_attr_getinheritsched (const pthread_attr_t *attr, int *inherit)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

#if 0  /* not support in uC/OS-II */
    if( inherit != NULL )
        *inherit = attr->inheritsched;
    
    PTHREAD_RETURN(0);
#else  /* not support in uC/OS-II */
    PTHREAD_RETURN(EINVAL);
#endif /* not support in uC/OS-II */
}

//-----------------------------------------------------------------------------
// Set scheduling policy

int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

#if 0  /* not support in uC/OS-II */
    if( policy == SCHED_OTHER ||
        policy == SCHED_FIFO ||
        policy == SCHED_RR )
    {
        attr->schedpolicy = policy;

        PTHREAD_RETURN(0);
    }
#endif /* not support in uC/OS-II */
    
    PTHREAD_RETURN(EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling policy

int pthread_attr_getschedpolicy (const pthread_attr_t *attr, int *policy)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

#if 0  /* not support in uC/OS-II */
    if( policy != NULL )
        *policy = attr->schedpolicy;

    PTHREAD_RETURN(0);
#else /* not support in uC/OS-II */
    PTHREAD_RETURN(EINVAL);
#endif /* not support in uC/OS-II */
}

//-----------------------------------------------------------------------------
// Set scheduling parameters
int pthread_attr_setschedparam (pthread_attr_t *attr,
				        const struct sched_param *param)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);
    PTHREAD_CHECK(param);

    attr->schedparam = *param;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get scheduling parameters

int pthread_attr_getschedparam (const pthread_attr_t *attr,
                                        struct sched_param *param)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( param != NULL )
        *param = attr->schedparam;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set starting address of stack. Whether this is at the start or end of
// the memory block allocated for the stack depends on whether the stack
// grows up or down.

int pthread_attr_setstackaddr (pthread_attr_t *attr, void *stackaddr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    attr->stackaddr       = stackaddr;
    attr->stackaddr_valid = 1;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get any previously set stack address.

int pthread_attr_getstackaddr (const pthread_attr_t *attr, void **stackaddr)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    if( stackaddr != NULL )
    {
        if( attr->stackaddr_valid )
        {
            *stackaddr = attr->stackaddr;
            PTHREAD_RETURN(0);
        }
        // Stack address not set, return EINVAL.
        else PTHREAD_RETURN(EINVAL);
    }

    PTHREAD_RETURN(0);
}


//-----------------------------------------------------------------------------
// Set minimum creation stack size.

int pthread_attr_setstacksize (pthread_attr_t *attr, size_t stacksize)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // Reject inadequate stack sizes
    if( stacksize < PTHREAD_STACK_MIN )
        PTHREAD_RETURN(EINVAL);
        
    attr->stacksize_valid = 1;    
    attr->stacksize = stacksize;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Get current minimal stack size.

int pthread_attr_getstacksize (const pthread_attr_t *attr, size_t *stacksize)
{
    PTHREAD_ENTRY();
    
    PTHREAD_CHECK(attr);

    // Reject attempts to get a stack size when one has not been set.
    if( !attr->stacksize_valid )
        PTHREAD_RETURN(EINVAL);
    
    if( stacksize != NULL )
        *stacksize = attr->stacksize;
    
    PTHREAD_RETURN(0);
}


//=============================================================================
// Dynamic package initialization
// Call init_routine just the once per control variable.

int pthread_once (pthread_once_t *once_control,
                          void (*init_routine) (void))
{
    pthread_once_t old;

    PTHREAD_ENTRY();

    PTHREAD_CHECK( once_control );
    PTHREAD_CHECK( init_routine );

    // Do a test and set on the once_control object.
    pthread_mutex_lock(&pthread_mutex);

    old = *once_control;
    *once_control = 1;

    pthread_mutex_unlock(&pthread_mutex);

    // If the once_control was zero, call the init_routine().
    if( !old ) init_routine();
    
    PTHREAD_RETURN(0);
}


//=============================================================================
//Thread specific data

#define _LSBIT_INDEX(index, mask) index = _lsbit_index (mask)

static int _lsbit_index (int mask)
{
    int i;
    for (i = 0;  i < 32;  i++) {
      if (mask & (1<<i)) return (i);
    }
    return (-1);
}

//-----------------------------------------------------------------------------
// Create a key to identify a location in the thread specific data area.
// Each thread has its own distinct thread-specific data area but all are
// addressed by the same keys. The destructor function is called whenever a
// thread exits and the value associated with the key is non-NULL.

int pthread_key_create (pthread_key_t *key,
                                void (*destructor) (void *))
{
    Uint32 i;
    pthread_key_t k = -1;

    PTHREAD_ENTRY();

    pthread_mutex_lock (&pthread_mutex);

    // Find a key to allocate
    for (i = 0; i < (PTHREAD_KEYS_MAX/KEY_MAP_TYPE_SIZE); i++) {
        if (thread_key[i] != 0) {
            // We have a table slot with space available

            // Get index of ls set bit.
            _LSBIT_INDEX (k, thread_key[i]);

            // clear it
            thread_key[i] &= ~(1<<k);

            // Add index of word
            k += i * KEY_MAP_TYPE_SIZE;

            // Install destructor
            key_destructor[k] = destructor;
            
            // break out with key found
            break;
        }
    }

    if (k != -1) {
        // plant a NULL in all the valid thread data slots for this
        // key in case we are reusing a key we used before.
        
        Uint32 i;
        for (i = 0; i < NR_POSIX_PTHREAD_THREADS_MAX ; i++) {
            pthread_info *thread = thread_table + i;

            if (thread->thread_data != NULL )
                thread->thread_data[k] = NULL;
        }
    }
    
    pthread_mutex_unlock (&pthread_mutex);    

    if (k == -1) PTHREAD_RETURN (EAGAIN);

    *key = k;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Delete key.

int pthread_key_delete (pthread_key_t key)
{
    PTHREAD_ENTRY ();

    pthread_mutex_lock (&pthread_mutex);

    // Set the key bit to 1 to indicate it is free.
    thread_key [key/KEY_MAP_TYPE_SIZE] |= 1<<(key%(KEY_MAP_TYPE_SIZE));

    pthread_mutex_unlock (&pthread_mutex);        
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Store the pointer value in the thread-specific data slot addressed
// by the key.

int pthread_setspecific (pthread_key_t key, const void *pointer)
{
    int i;
    pthread_info *self;

    PTHREAD_ENTRY();

    if( thread_key[key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE) )
        PTHREAD_RETURN(EINVAL);

    self = pthread_self_info();

    if (self->thread_data == NULL) {
        // Allocate the per-thread data table
        self->thread_data = (void **)(self->stackmem + sizeof (pthread_info));

        // Clear out all entries
        for(i  = 0; i < PTHREAD_KEYS_MAX; i++ )
            self->thread_data[i] = NULL;
    }
    
    self->thread_data[key] = (void *)pointer;
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Retrieve the pointer value in the thread-specific data slot addressed
// by the key.

void *pthread_getspecific (pthread_key_t key)
{
    void *val;
    pthread_info *self;

    PTHREAD_ENTRY();

    self = pthread_self_info();

    if( thread_key[key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE) )
        PTHREAD_RETURN(NULL);

    if( self->thread_data == NULL )
        val = NULL;
    else val = self->thread_data[key];

    PTHREAD_RETURN(val);
}

//=============================================================================
// Thread Cancellation Functions

//-----------------------------------------------------------------------------
// Set cancel state of current thread to ENABLE or DISABLE.
// Returns old state in *oldstate.

int pthread_setcancelstate (int state, int *oldstate)
{
    pthread_info *self;

    PTHREAD_ENTRY();

    if( state != PTHREAD_CANCEL_ENABLE &&
        state != PTHREAD_CANCEL_DISABLE )
        PTHREAD_RETURN(EINVAL);
    
    pthread_mutex_lock (&pthread_mutex);

    self = pthread_self_info();

    if( oldstate != NULL ) *oldstate = self->cancelstate;
    
    self->cancelstate = state;
    
    pthread_mutex_unlock (&pthread_mutex);
    
    // Note: This function may have made it possible for a pending
    // cancellation to now be delivered. However the standard does not
    // list this function as a cancellation point, so for now we do
    // nothing. In future we might call pthread_testcancel() here.
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Set cancel type of current thread to ASYNCHRONOUS or DEFERRED.
// Returns old type in *oldtype.

int pthread_setcanceltype (int type, int *oldtype)
{
    pthread_info *self;

    PTHREAD_ENTRY();

    if( type != PTHREAD_CANCEL_ASYNCHRONOUS &&
        type != PTHREAD_CANCEL_DEFERRED )
        PTHREAD_RETURN(EINVAL);
    
    pthread_mutex_lock(&pthread_mutex);

    self = pthread_self_info();
        
    if( oldtype != NULL ) *oldtype = self->canceltype;

    self->canceltype = type;
    
    pthread_mutex_unlock(&pthread_mutex);   

    // Note: This function may have made it possible for a pending
    // cancellation to now be delivered. However the standard does not
    // list this function as a cancellation point, so for now we do
    // nothing. In future we might call pthread_testcancel() here.
    
    PTHREAD_RETURN(0);
}

//-----------------------------------------------------------------------------
// Cancel the thread.

int pthread_cancel (pthread_t thread)
{
    pthread_info *th;

    PTHREAD_ENTRY();

    th = pthread_info_id (thread);

    pthread_mutex_lock (&pthread_mutex);

    if (th == NULL) {
        pthread_mutex_unlock (&pthread_mutex);
        PTHREAD_RETURN (ESRCH);
    }

    th->cancelpending = TRUE;

    if (th->cancelstate == PTHREAD_CANCEL_ENABLE) {
#if 0  /* do nothing for uC/OS-II */
        if (th->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS) {
            // If the thread has cancellation enabled, and it is in
            // asynchronous mode, then we can do the
            // cancellation processing. 
        }
        else if (th->canceltype == PTHREAD_CANCEL_DEFERRED) {
            // If the thread has cancellation enabled, and it is in 
            // deferred mode, call OSTaskDelReq to mark the delete
            // request flag.  OSTaskDelReq (th->id);
        }
        else
            PTHREAD_FAIL ("Unknown cancellation type");
#endif /* do nothing for uC/OS-II */
    }

    // Otherwise the thread has cancellation disabled, in which case
    // it is up to the thread to enable cancellation
    
    pthread_mutex_unlock (&pthread_mutex);   
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Test for a pending cancellation for the current thread and terminate
// the thread if there is one.

void pthread_testcancel (void)
{
    PTHREAD_ENTRY_VOID();

    if (checkforcancel ()) {
        // If we have cancellation enabled, and there is a cancellation
        // pending, then go ahead and do the deed. 
        
        // Exit now with special retval. pthread_exit() calls the
        // cancellation handlers implicitly.
        pthread_exit (PTHREAD_CANCELED);
    }
        
    PTHREAD_RETURN_VOID;
}

//-----------------------------------------------------------------------------
// These two functions actually implement the cleanup push and pop functionality.

void pthread_cleanup_push_inner (struct pthread_cleanup_buffer *buffer,
                                         void (*routine) (void *),
                                         void *arg)
{
    pthread_info *self;

    PTHREAD_ENTRY();

    self = pthread_self_info();

    buffer->routine     = routine;
    buffer->arg         = arg;
    
    buffer->prev        = self->cancelbuffer;

    self->cancelbuffer  = buffer;

    return;
}

void pthread_cleanup_pop_inner (struct pthread_cleanup_buffer *buffer,
                                        int execute)
{
    pthread_info *self = pthread_self_info();
    
    PTHREAD_ENTRY();

    if (self->cancelbuffer == buffer) {
        // Remove the buffer from the stack
        self->cancelbuffer = buffer->prev;
    }
    else {
        // If the top of the stack is not the buffer we expect, do not
        // execute it.
        execute = 0;
    }

    if (execute)
        buffer->routine (buffer->arg);
    
    return;
}

#endif /* __UCOSII__ && _MGUSE_OWN_PTHREAD */

