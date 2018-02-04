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
** vxworks_pthread.c: Implementation of the POSIX pthread functions
**             for VxWorks.
**
** Create Date: 2005-09-21
*/

#include "mgconfig.h"

#if defined (__VXWORKS__) && defined (_MGUSE_OWN_PTHREAD)

#include <string.h>

#include <taskLib.h>
#include <semLib.h>

#include "common.h"
#include "vxworks_pprivate.h"

//-----------------------------------------------------------------------------
// First check that the configuration contains the elements we need

//=============================================================================
// Internal data structures

// Mutex for controlling access to shared data structures
static SEM_ID __vxpthread_mutex;

// Array of pthread control structures. A pthread_t object is
// "just" an index into this array.
static pthread_info *thread_table [VXPTHNUM_POSIX_PTHREAD_THREADS_MAX];

// Count of number of threads in table.
static int pthread_count = 0;

// Count of number of threads that have exited and not been reaped.
static int pthreads_exited;

// Count of number of threads that are waiting to be joined
static int pthreads_tobejoined;

// Per-thread key allocation. This key map has a 1 bit set for each
// key that is free, zero if it is allocated.
#define KEY_MAP_TYPE unsigned int
#define KEY_MAP_TYPE_SIZE (sizeof(KEY_MAP_TYPE)*8) // in BITS!
static KEY_MAP_TYPE thread_key [PTHREAD_KEYS_MAX/KEY_MAP_TYPE_SIZE];
static void (*key_destructor [PTHREAD_KEYS_MAX]) (void *);
    
// Index of next pthread_info to allocate from thread_table array.
static int thread_info_next = 0;

// This is used to make pthread_t values unique even when reusing
// a table slot. This allows VXPTHNUM_POSIX_PTHREAD_THREADS_MAX to range
// up to 1024.
#define THREAD_ID_COOKIE_INC 0x00000400
#define THREAD_ID_COOKIE_MASK (THREAD_ID_COOKIE_INC-1)
static pthread_t thread_id_cookie = THREAD_ID_COOKIE_INC;

//=============================================================================
// Exported variables

int pthread_canceled_dummy_var;           // pointed to by PTHREAD_CANCELED

//=============================================================================
// Internal functions

//-----------------------------------------------------------------------------
// Private version of pthread_self() that returns a pointer to our internal
// control structure.

void __vxpth_get_name (char *name, char type, int lock)
{
    static char *name_template = "fm-00000000";
    int i;
    static int j = 0, k;

    if (lock) semTake (__vxpthread_mutex, WAIT_FOREVER);

    for (i = 0; name_template[i]; i++)
        name[i] = name_template[i];

    name[2] = type;

    k = j;
    for (i = 10; i > 2; i--) {
        name [i] = "0123456789ABCDEF" [k&0xF];
        k >>= 4;
    }
    j++;

    if (lock) semGive (__vxpthread_mutex);
}

static inline pthread_info *pthread_self_info (void)
{
    return (pthread_info *)(taskIdSelf());
}

pthread_info *pthread_info_id (pthread_t id)
{
    pthread_t index = id & THREAD_ID_COOKIE_MASK;
    pthread_info *info = thread_table [index];

    // Check for a valid entry
    if(info == NULL)
        return NULL;
    
    // Check that this is a valid entry
    if (info->state == PTHREAD_STATE_FREE ||
            info->state == PTHREAD_STATE_EXITED)
        return NULL;

    // Check that the entry matches the id
    if (info->id != id) return NULL;

    // Return the pointer
    return info;
}

//-----------------------------------------------------------------------------
// pthread entry function.
// does some housekeeping and then calls the user's start routine.

static int pthread_entry (void *argv)
{
    pthread_info *self = (pthread_info *)argv;
    void *retval;

    retval = self->start_routine (self->start_arg);

    pthread_exit (retval);
    return 0;
}

//-----------------------------------------------------------------------------
// Check whether there is a cancel pending and if so, whether
// cancellations are enabled. We do it in this order to reduce the
// number of tests in the common case - when no cancellations are
// pending.
// We make this inline so it can be called directly below for speed

static __inline int checkforcancel (void)
{
    pthread_info *self = pthread_self_info ();

    if (self != NULL && 
            self->cancelpending && 
            self->cancelstate == PTHREAD_CANCEL_ENABLE)
        return 1;
    else
        return 0;
}

//-----------------------------------------------------------------------------
// The (Grim) Reaper.
// This function is called to tidy up and dispose of any threads that have
// exited. This work must be done from a thread other than the one exiting.
// Note: this function _must_ be called with pthread_mutex locked.

static void pthread_reap ()
{
    int i;
    UINT vx_ret;

    // Loop over the thread table looking for exited threads. The
    // pthreads_exited counter springs us out of this once we have
    // found them all (and keeps us out if there are none to do).
 
    for (i = 0; pthreads_exited > 0 && i < VXPTHNUM_POSIX_PTHREAD_THREADS_MAX ; i++) {
        pthread_info *thread = thread_table[i];

        if (thread && thread->state == PTHREAD_STATE_EXITED) {
/*
            The thread has exited, so it is a candidate for being
            reaped. We have to make sure that the OS thread has
            also reached EXITED state before we can tidy it up.
*/
            //STATUS status;
            char task_status[10];
            int prio;

            if (taskStatusString (thread->task_id, task_status) != OK)
                continue;

            taskPriorityGet (taskIdSelf(), &prio);

            while (strcmp(task_status, "DEAD") != 0) {
                printf ("vxpthread: exited thread/task %d is not dead!\n", thread->task_id);
                //TODO
                taskPrioritySet (thread->task_id, prio);
                //relinquish?
                taskDelay (2);
            }

            // At this point we have a thread that we can reap.

            // destroy the thread
            taskDelete (thread->task_id);

            // destroy the joiner condvar
            semDelete (thread->joiner);

            // Free the stack if we allocated it
            if (thread->freestack)
                free (thread->stackmem);

            // Finally, set the thread table entry to NULL so that it
            // may be reused.
            thread_table[i] = NULL;

            pthread_count--;
            pthreads_exited--;
        }
    }
}

//=============================================================================
// Functions exported to rest of POSIX subsystem.

//-----------------------------------------------------------------------------
// Main thread.

// Thread ID of main thread.
static pthread_t main_thread;

//-----------------------------------------------------------------------------
// Main entry function.
// This is set as the start_routine of the main thread. 
// It invokes the entry function passed by thread argument.

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

    // Initialize the global mutex object
    __vxpthread_mutex = semBCreate (SEM_Q_PRIORITY, SEM_FULL);

    if (__vxpthread_mutex == 0) {
        VXPTH_FAIL ("PThread: Can not create global mutex object.\n");
        return 1;
    }

    // TODO: Initialize other global object

    // Initialize the per-thread data key map.
    for (i = 0; i < (PTHREAD_KEYS_MAX/KEY_MAP_TYPE_SIZE); i++) {
        thread_key [i] = ~0;
    }
 
    // Create the main thread
    if (pth_entry) {
        pthread_attr_t attr;
        struct sched_param schedparam;
        static struct _main_pth_entry_info entry_info;

        entry_info.pth_entry = pth_entry; 
        entry_info.argc = argc; 
        entry_info.argv = argv;

        if (stack_size < MAIN_PTH_MIN_STACK_SIZE) {
            VXPTH_FAIL ("PThread: Tool small stack size of main thread.\n");
            return 2;
        }

        schedparam.priority          = VXPTH_POSIX_MAIN_DEF_PRIORITY;
        schedparam.preempt_threshold = VXPTH_POSIX_MAIN_DEF_PREEMPT_THRESHOLD;
        schedparam.time_slice        = VXPTH_POSIX_MAIN_DEF_TIME_SLICE;

        pthread_attr_init (&attr);
        pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_DETACHED);
        pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setstackaddr (&attr, stack_base + stack_size);
        pthread_attr_setstacksize (&attr, stack_size);
        pthread_attr_setschedpolicy (&attr, SCHED_FIFO);
        pthread_attr_setschedparam (&attr, &schedparam);

        pthread_create (&main_thread, &attr, main_pthread_entry, &entry_info);
    }

    return 0;
}

//=============================================================================
// General thread operations

//-----------------------------------------------------------------------------
// Thread creation and management.

// Create a thread.
int pthread_create (pthread_t *thread,
                    const pthread_attr_t *attr,
                    void *(*start_routine) (void *),
                    void *arg)
{
    UINT vx_ret;
    char* stackbase;
    size_t stacksize;
    BOOL freestack = FALSE;
    void* stackmem = 0;
    pthread_info *nthread;
    int thread_next = thread_info_next;
 
    pthread_attr_t use_attr;

    PTHREAD_ENTRY();

    PTHREAD_CHECK (thread);
    PTHREAD_CHECK (start_routine);

    // Set use_attr to the set of attributes we are going to
    // actually use. Either those passed in, or the default set.
   
    if (attr == NULL)
        pthread_attr_init (&use_attr);
    else
        use_attr= *attr;

    // Adjust the attributes to cope with the setting of inheritsched.

    //FIXME
    /*
    if (use_attr.inheritsched == PTHREAD_INHERIT_SCHED) {
        pthread_info *self = pthread_self_info ();

        use_attr.schedpolicy = self->attr.schedpolicy;
        use_attr.schedparam = self->attr.schedparam;
    }
    */

    // If the stack size is not valid, we can assume that it is at
    // least PTHREAD_STACK_DEFAULT bytes.
    if (use_attr.stacksize_valid) {
        stacksize = use_attr.stacksize;
    }
    else {
        stacksize = PTHREAD_STACK_DEFAULT;
    }

    // Set up stack base and size from supplied arguments.
    // Calculate stack base from address and size.
    // FIXME: Falling stack assumed in pthread_create().
    if (use_attr.stackaddr_valid) {
        // Set up stack base and size from supplied arguments.
        // Calculate stack base from address and size.
        stackmem = stackbase = (char*)use_attr.stackaddr - stacksize;
    }
    else {
        stackmem = stackbase = malloc (stacksize);
        if (stackmem == 0)
            PTHREAD_RETURN (EAGAIN);
        freestack = TRUE;
    }

    semTake (__vxpthread_mutex, WAIT_FOREVER);
    
    /* Dispose of any dead threads */
    pthread_reap ();
    
    /* Find a free slot in the thread table */
    while (thread_table[thread_next] != NULL) {
        thread_next++;
        if (thread_next >= VXPTHNUM_POSIX_PTHREAD_THREADS_MAX)
            thread_next = 0;

        /* check for wrap, and return error if no slots left */
        if (thread_next == thread_info_next) {
            semGive (__vxpthread_mutex);
            if (freestack)
                free (stackmem);
            PTHREAD_RETURN (ENOMEM);
        }
    }

/* 
 * For _STACK_GROWS_DOWN:
 * 
 *.CS
 *     - HIGH MEMORY -
 *  ---------------------
 *  |                  |
 *  |     WIND_TCB     |
 *  |                  |
 *  -------------------->--- pStackBase, pTcb
 *  |//// 16 bytes ////| (16 bytes clobbered during objFree()
 *  |                  |  in taskDestroy are not accounted for)
 *  |                  |
 *  |    TASK STACK    |
 *  |                  |
 *  |                  |
 *  -------------------->--- pTaskMem
 *     - LOW MEMORY -
 *.CE
 *
 *
 * For _STACK_GROWS_UP:
 * 
 *.CS
 *     - HIGH MEMORY -
 *  ---------------------
 *  |                  |
 *  |                  |
 *  |    TASK STACK    |
 *  |                  |
 *  |                  |
 *  -------------------->--- pStackBase
 *  |                  |
 *  |     WIND_TCB     |
 *  |                  |
 *  -------------------->--- pTcb
 *  |//// 16 bytes ////| (16 bytes clobbered during objFree() of taskDestroy)
 *  -------------------->--- pTaskMem
 *     - LOW MEMORY -
 *.CE
 *
 */

#if (_STACK_DIR == _STACK_GROWS_DOWN)
    stacksize -= (STACK_ROUND_DOWN(sizeof(pthread_info)));
    stackbase += stacksize;
    nthread = (pthread_info *)stackbase;
#else
    //16 bytes clobbered during objFree() in taskDestroy are not accounted for.
    nthread = (pthread_info *)(stackbase + 16);
    stackbase += STACK_ROUND_UP(16 + sizeof(pthread_info));
    stacksize -= STACK_ROUND_UP(16 + sizeof(pthread_info));
#endif

    thread_table [thread_next] = nthread;

    // Set new next index
    thread_info_next = thread_next + 1;
    
    // step the cookie
    thread_id_cookie += THREAD_ID_COOKIE_INC;

    // Initialize the table entry
    nthread->state              = use_attr.detachstate == PTHREAD_CREATE_JOINABLE ?
                                  PTHREAD_STATE_RUNNING : PTHREAD_STATE_DETACHED;
    nthread->id                 = thread_next + thread_id_cookie;
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

    memset(nthread->thread_data, 0, PTHREAD_KEYS_MAX);
    
    /* Initialize the joiner event flag */
    nthread->joiner = semBCreate (SEM_Q_PRIORITY, SEM_FULL);

    /* vxworks task id */
    nthread->task_id = (int) &nthread->tcb;

    /* generate a name for this thread */
    __vxpth_get_name (nthread->name, NAME_TYPE_THREAD, 0);

    /* create the underlying VxWorks task */
    vx_ret = taskInit (&nthread->tcb, nthread->name, use_attr.schedparam.priority,
                        VX_NO_STACK_FILL,
                        stackbase, stacksize, pthread_entry,
                        (int)nthread, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    if (vx_ret == ERROR) {
        semGive (__vxpthread_mutex);
        if (freestack)
            free (stackmem);
        PTHREAD_RETURN (EINVAL);
    }
 
    /* return thread ID */
    *thread = nthread->id;

    pthread_count++;

    semGive (__vxpthread_mutex);

    /* finally, set the thread going */
    taskActivate (nthread->task_id);

    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Get current thread id.

pthread_t pthread_self (void)
{
    pthread_info* info;
    
    PTHREAD_ENTRY();
    
    info = pthread_self_info ();

    return info->id;
}

//-----------------------------------------------------------------------------
// Compare two thread identifiers.

int pthread_equal (pthread_t thread1, pthread_t thread2)
{
    PTHREAD_ENTRY();
    
    return thread1 == thread2;
}

//-----------------------------------------------------------------------------
// Terminate a specific thread.

static void vxpth_pthread_exit (pthread_info* thread, void *retval)
{
    UINT vx_ret;
    BOOL call_exit = FALSE;

    PTHREAD_ENTRY ();
    
    // Call cancellation handlers. We eat up the buffers as we go in
    // case any of the routines calls pthread_exit() itthread.
    while (thread->cancelbuffer != NULL) {
        struct pthread_cleanup_buffer *buffer = thread->cancelbuffer;
        thread->cancelbuffer = buffer->prev;
        buffer->routine (buffer->arg);
    }

    if (thread->thread_data != NULL) {
        // Call per-thread key destructors.
        // The specification of this is that we must continue to call the
        // destructor functions until all the per-thread data values are NULL or
        // we have done it PTHREAD_DESTRUCTOR_ITERATIONS times.
    
        BOOL destructors_called;
        int destructor_iterations = 0;

        do {
            unsigned int key;

            destructors_called = FALSE;
        
            for (key = 0; key < PTHREAD_KEYS_MAX; key++) {
                // Skip unallocated keys
                if (thread_key[key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE))
                    continue;

                // Skip NULL destructors
                if (key_destructor[key] == NULL) continue;

                // Skip NULL data values
                if (thread->thread_data[key] == NULL) continue;

                // If it passes all that, call the destructor.
                // Note that NULLing the data value here is new
                // behaviour in the 2001 POSIX standard.
                {
                    void* value = thread->thread_data[key];
                    thread->thread_data[key] = NULL;
                    key_destructor[key](value);
                }

                // Record that we called a destructor
                destructors_called = TRUE;
            }

            // Count the iteration
            destructor_iterations++;
        
        } while (destructors_called &&
                 (destructor_iterations <= PTHREAD_DESTRUCTOR_ITERATIONS));

    }
    
    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    // Set the retval for any joiner
    thread->retval = retval;

    // If we are already detached, go to EXITED state, otherwise
    // go into JOIN state.
 
    if (PTHREAD_STATE_DETACHED == thread->state) {
        thread->state = PTHREAD_STATE_EXITED;
        pthreads_exited++;
    }
    else {
        thread->state = PTHREAD_STATE_JOIN;
        pthreads_tobejoined++;
    }

    // notify the waiting joiner
    // FIXME, only one joiner?
    // Maybe we should use events.
    semGive (thread->joiner);

    // if this is the last thread (other than threads waiting to be joined)
    // then we need to call exit() later
    if (pthreads_exited + pthreads_tobejoined == pthread_count)
        call_exit = TRUE;

    vx_ret = semGive (__vxpthread_mutex);
    
    taskDelete (thread->task_id);

    // This loop keeps some compilers happy. pthread_exit() is marked
    // with the noreturn attribute, and without this they generate a
    // call to abort() here in case Cyg_Thread::exit() returns. 

    //FIXME
    for(;;) continue;
}

void pthread_exit (void *retval)
{
    vxpth_pthread_exit (pthread_self_info(), retval);
}

//-----------------------------------------------------------------------------
// Wait for the thread to terminate. If thread_return is not NULL then
// the retval from the thread's call to pthread_exit() is stored at
// *thread_return.

int pthread_join (pthread_t thread, void **thread_return)
{
    UINT vx_ret;
    int err = 0;
    pthread_info* self;
    pthread_info* joinee;

    PTHREAD_ENTRY ();
    
    // check for cancellation first.
    pthread_testcancel ();

    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);
    
    // Dispose of any dead threads
    pthread_reap ();
    
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
  
            semGive (__vxpthread_mutex);
            vx_ret = semTake (joinee->joiner, WAIT_FOREVER);
            semTake (__vxpthread_mutex, WAIT_FOREVER);

            if (vx_ret != OK) { //TODO
                // check if we were woken because we were being cancelled
                if (checkforcancel ()) {
                    err = EAGAIN;  // value unimportant, just some error
                    break;
                }
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
        if (thread_return != NULL)
            *thread_return = joinee->retval;
        
        // set state to exited.
        joinee->state = PTHREAD_STATE_EXITED;
        pthreads_exited++;
        pthreads_tobejoined--;
    
        // Dispose of any dead threads
        pthread_reap ();
    }

    vx_ret = semGive (__vxpthread_mutex);
    
    // check for cancellation before returning
    pthread_testcancel ();

    PTHREAD_RETURN (err);
}

//-----------------------------------------------------------------------------
// Set the detachstate of the thread to "detached". The thread then does not
// need to be joined and its resources will be freed when it exits.

int pthread_detach (pthread_t thread)
{
    UINT vx_ret;
    int ret = 0;
    pthread_info* detachee;

    PTHREAD_ENTRY();
    
    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    detachee = pthread_info_id (thread);
    
    if (detachee == NULL)
        ret = ESRCH;                    // No such thread
    else if (detachee->state == PTHREAD_STATE_DETACHED)
        ret = EINVAL;                   // Already detached!
    else {
        // Set state to detached and kick any joinees to
        // make them return.
        detachee->state = PTHREAD_STATE_DETACHED;
        semGive (detachee->joiner);
    }
    
    // Dispose of any dead threads
    pthread_reap ();
    
    vx_ret = semGive (__vxpthread_mutex);

    PTHREAD_RETURN (ret);
}


//-----------------------------------------------------------------------------
// Thread attribute handling.

//-----------------------------------------------------------------------------
// Initialize attributes object with default attributes:
// detachstate          == PTHREAD_CREATE_JOINABLE
// scope                == PTHREAD_SCOPE_SYSTEM
// inheritsched         == PTHREAD_INHERIT_SCHED
// schedpolicy          == SCHED_FIFO
// schedparam           == unset
// stackaddr            == unset
// stacksize            == 0
// 

int pthread_attr_init (pthread_attr_t *attr)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    
    attr->detachstate                 = PTHREAD_CREATE_JOINABLE;
    attr->scope                       = PTHREAD_SCOPE_SYSTEM;
    attr->inheritsched                = PTHREAD_INHERIT_SCHED;
    attr->schedpolicy                 = SCHED_FIFO;
    attr->schedparam.priority         = VXPTH_THREAD_DEF_PRIORITY;
    attr->schedparam.preempt_threshold = VXPTH_THREAD_DEF_PREEMPT_THRESHOLD;
    attr->schedparam.time_slice       = VXPTH_THREAD_DEF_TIME_SLICE;
    attr->stackaddr_valid             = 0;
    attr->stackaddr                   = NULL;
    attr->stacksize_valid             = 0;
    attr->stacksize                   = 0;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Destroy thread attributes object

int pthread_attr_destroy (pthread_attr_t *attr)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    // Nothing to do here...
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Set the detachstate attribute

int pthread_attr_setdetachstate (pthread_attr_t *attr,
                                         int detachstate)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (detachstate == PTHREAD_CREATE_JOINABLE
            || detachstate == PTHREAD_CREATE_DETACHED) {
        attr->detachstate = detachstate;
        PTHREAD_RETURN (0);
    }
    
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Get the detachstate attribute
int pthread_attr_getdetachstate (const pthread_attr_t *attr,
                                         int *detachstate)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (detachstate != NULL)
        *detachstate = attr->detachstate;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Set scheduling contention scope

int pthread_attr_setscope (pthread_attr_t *attr, int scope)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (scope == PTHREAD_SCOPE_SYSTEM) {
        attr->scope = scope;

        PTHREAD_RETURN (0);
    }
    else if (scope == PTHREAD_SCOPE_PROCESS) {
        PTHREAD_RETURN (ENOTSUP);
    }
    
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling contention scope

int pthread_attr_getscope (const pthread_attr_t *attr, int *scope)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (scope != NULL)
        *scope = attr->scope;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Set scheduling inheritance attribute

int pthread_attr_setinheritsched (pthread_attr_t *attr, int inherit)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (inherit == PTHREAD_INHERIT_SCHED 
            || inherit == PTHREAD_EXPLICIT_SCHED) {
        attr->inheritsched = inherit;

        PTHREAD_RETURN (0);
    }

    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling inheritance attribute

int pthread_attr_getinheritsched (const pthread_attr_t *attr,
                                          int *inherit)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if( inherit != NULL )
        *inherit = attr->inheritsched;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Set scheduling policy

int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (policy == SCHED_FIFO) {
        attr->schedpolicy = policy;

        PTHREAD_RETURN (0);
    }
    else if (policy == SCHED_OTHER || policy == SCHED_RR) {
        PTHREAD_RETURN (ENOTSUP);
    }
    
    PTHREAD_RETURN (EINVAL);
}

//-----------------------------------------------------------------------------
// Get scheduling policy

int pthread_attr_getschedpolicy (const pthread_attr_t *attr,
                                         int *policy)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (policy != NULL)
        *policy = attr->schedpolicy;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Set scheduling parameters
int pthread_attr_setschedparam (pthread_attr_t *attr,
                        const struct sched_param *param)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);
    PTHREAD_CHECK (param);

    attr->schedparam = *param;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Get scheduling parameters

int pthread_attr_getschedparam (const pthread_attr_t *attr,
                                        struct sched_param *param)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (param != NULL)
        *param = attr->schedparam;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Set starting address of stack. Whether this is at the start or end of
// the memory block allocated for the stack depends on whether the stack
// grows up or down.

int pthread_attr_setstackaddr (pthread_attr_t *attr, void *stackaddr)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    attr->stackaddr       = stackaddr;
    attr->stackaddr_valid = 1;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Get any previously set stack address.

int pthread_attr_getstackaddr (const pthread_attr_t *attr,
                                       void **stackaddr)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    if (stackaddr != NULL) {
        if (attr->stackaddr_valid) {
            *stackaddr = attr->stackaddr;

            PTHREAD_RETURN (0);
        }

        // Stack address not set, return EINVAL.
        else PTHREAD_RETURN (EINVAL);
    }

    PTHREAD_RETURN (0);
}


//-----------------------------------------------------------------------------
// Set minimum creation stack size.

int pthread_attr_setstacksize (pthread_attr_t *attr,
                                       size_t stacksize)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    // Reject inadequate stack sizes
    if( stacksize < PTHREAD_STACK_MIN )
        PTHREAD_RETURN (EINVAL);
        
    attr->stacksize_valid = 1;
    attr->stacksize = stacksize;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Get current minimal stack size.

int pthread_attr_getstacksize (const pthread_attr_t *attr,
                                       size_t *stacksize)
{
    PTHREAD_ENTRY ();
    
    PTHREAD_CHECK (attr);

    // Reject attempts to get a stack size when one has not been set.
    if (!attr->stacksize_valid)
        PTHREAD_RETURN (EINVAL);
    
    if (stacksize != NULL)
        *stacksize = attr->stacksize;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Thread scheduling controls

//-----------------------------------------------------------------------------
// Set scheduling policy and parameters for the thread

int pthread_setschedparam (pthread_t thread_id,
                                   int policy,
                                   const struct sched_param *param)
{
    pthread_info *thread;
    STATUS vx_ret;

    PTHREAD_ENTRY ();

    if (policy != SCHED_OTHER && 
            policy != SCHED_FIFO && 
            policy != SCHED_RR)
        PTHREAD_RETURN (EINVAL);

    PTHREAD_CHECK (param);

    // The parameters seem OK, change the thread...
    
    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    thread = pthread_info_id (thread_id);

    if (thread == NULL) {
        vx_ret = semGive (__vxpthread_mutex);
        PTHREAD_RETURN (ESRCH);
    }
    
    thread->attr.schedpolicy = policy;
    thread->attr.schedparam = *param;

    vx_ret = taskPrioritySet (thread->task_id, param->priority);
    // TODO
    //vx_ret = tx_thread_preemption_change (thread->thread, param->preempt_threshold, &tmp);
    //vx_ret = NU_Change_Time_Slice (thread->thread, param->time_slice);

    vx_ret = semGive (__vxpthread_mutex);
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Get scheduling policy and parameters for the thread

int pthread_getschedparam (pthread_t thread_id,
                                   int *policy,
                                   struct sched_param *param)
{
    UINT vx_ret;
    pthread_info *thread;

    PTHREAD_ENTRY ();

    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    thread = pthread_info_id (thread_id);

    if (thread == NULL) {
        vx_ret = semGive (__vxpthread_mutex);
        PTHREAD_RETURN (ESRCH);
    }

    if (policy != NULL)
        *policy = thread->attr.schedpolicy;

    if (param != NULL)
        *param = thread->attr.schedparam;
    
    vx_ret = semGive (__vxpthread_mutex);
    
    PTHREAD_RETURN (0);
}


//=============================================================================
// Dynamic package initialization
// Call init_routine just the once per control variable.

int pthread_once (pthread_once_t *once_control,
                          void (*init_routine) (void))
{
    STATUS vx_ret;
    pthread_once_t old;

    PTHREAD_ENTRY ();

    PTHREAD_CHECK (once_control);
    PTHREAD_CHECK (init_routine);

    // Do a test and set on the once_control object.
    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    old = *once_control;
    *once_control = 1;

    vx_ret = semGive (__vxpthread_mutex);

    // If the once_control was zero, call the init_routine().
    if (!old) init_routine ();
    
    PTHREAD_RETURN (0);
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
    UINT vx_ret;
    unsigned int i;
    pthread_key_t k = -1;
    
    PTHREAD_ENTRY();

    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

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
        
        for (i = 0; i < VXPTHNUM_POSIX_PTHREAD_THREADS_MAX ; i++) {
            pthread_info *thread = thread_table[i];

            if( thread != NULL && thread->thread_data != NULL )
                thread->thread_data[k] = NULL;
        }
    }
    
    semGive (__vxpthread_mutex);

    if (k == -1)
        PTHREAD_RETURN (EAGAIN);

    *key = k;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Delete key.

int pthread_key_delete (pthread_key_t key)
{
    UINT vx_ret;

    PTHREAD_ENTRY ();

    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    // Set the key bit to 1 to indicate it is free.
    thread_key [key/KEY_MAP_TYPE_SIZE] |= 1<<(key%(KEY_MAP_TYPE_SIZE));

    vx_ret = semGive (__vxpthread_mutex);
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Store the pointer value in the thread-specific data slot addressed
// by the key.

int pthread_setspecific (pthread_key_t key, const void *pointer)
{
    pthread_info *self;

    PTHREAD_ENTRY ();

    if (thread_key [key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE))
        PTHREAD_RETURN (EINVAL);

    self = pthread_self_info ();

    self->thread_data[key] = (void *)pointer;
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Retrieve the pointer value in the thread-specific data slot addressed
// by the key.

void *pthread_getspecific (pthread_key_t key)
{
    void *val;
    pthread_info *self;

    PTHREAD_ENTRY ();

    if (thread_key [key/KEY_MAP_TYPE_SIZE] & 1<<(key%KEY_MAP_TYPE_SIZE))
        PTHREAD_RETURN(NULL);

    self = pthread_self_info ();

    if (self->thread_data == NULL)
        val = NULL;
    else val = self->thread_data [key];

    PTHREAD_RETURN (val);
}

//=============================================================================
// Thread Cancellation Functions

//-----------------------------------------------------------------------------
// Set cancel state of current thread to ENABLE or DISABLE.
// Returns old state in *oldstate.

int pthread_setcancelstate (int state, int *oldstate)
{
    UINT vx_ret;
    pthread_info *self;

    PTHREAD_ENTRY ();

    if (state != PTHREAD_CANCEL_ENABLE &&
            state != PTHREAD_CANCEL_DISABLE)
        PTHREAD_RETURN (EINVAL);
    
    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    self = pthread_self_info ();

    if (oldstate != NULL) *oldstate = self->cancelstate;
    
    self->cancelstate = state;
    
    vx_ret = semGive (__vxpthread_mutex);
    
    // Note: This function may have made it possible for a pending
    // cancellation to now be delivered. However the standard does not
    // list this function as a cancellation point, so for now we do
    // nothing. In future we might call pthread_testcancel() here.
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Set cancel type of current thread to ASYNCHRONOUS or DEFERRED.
// Returns old type in *oldtype.

int pthread_setcanceltype (int type, int *oldtype)
{
    UINT vx_ret;
    pthread_info *self;

    PTHREAD_ENTRY ();

    if (type != PTHREAD_CANCEL_ASYNCHRONOUS &&
            type != PTHREAD_CANCEL_DEFERRED )
        PTHREAD_RETURN (EINVAL);
    
    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    self = pthread_self_info ();
        
    if (oldtype != NULL) *oldtype = self->canceltype;

    self->canceltype = type;
    
    vx_ret = semGive (__vxpthread_mutex);

    // Note: This function may have made it possible for a pending
    // cancellation to now be delivered. However the standard does not
    // list this function as a cancellation point, so for now we do
    // nothing. In future we might call pthread_testcancel() here.
    
    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Cancel the thread.

int pthread_cancel (pthread_t thread)
{
    UINT vx_ret;
    pthread_info *th;

    PTHREAD_ENTRY ();

    vx_ret = semTake (__vxpthread_mutex, WAIT_FOREVER);

    th = pthread_info_id (thread);

    if (th == NULL) {
        vx_ret = semGive (__vxpthread_mutex);
        PTHREAD_RETURN (ESRCH);
    }

    th->cancelpending = TRUE;

    if (th->cancelstate == PTHREAD_CANCEL_ENABLE) {
        if (th->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS) {
            // If the thread has cancellation enabled, and it is in
            // asynchronous mode, set the OS thread's ASR pending to
            // deal with it when the thread wakes up. We also release the
            // thread out of any current wait to make it wake up.

            vxpth_pthread_exit (th, PTHREAD_CANCELED);
        }
        else if (th->canceltype == PTHREAD_CANCEL_DEFERRED) {
            // If the thread has cancellation enabled, and it is in 
            // deferred mode, wake the thread up so that cancellation
            // points can test for cancellation.

            //tx_thread_wait_abort (th->thread);
        }
        else
            VXPTH_FAIL ("Unknown cancellation type");
    }

    // Otherwise the thread has cancellation disabled, in which case
    // it is up to the thread to enable cancellation
    
    vx_ret = semGive (__vxpthread_mutex);

    PTHREAD_RETURN (0);
}

//-----------------------------------------------------------------------------
// Test for a pending cancellation for the current thread and terminate
// the thread if there is one.

void pthread_testcancel (void)
{
    PTHREAD_ENTRY_VOID ();

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

    PTHREAD_ENTRY ();

    self = pthread_self_info ();

    buffer->routine     = routine;
    buffer->arg         = arg;
    
    buffer->prev        = self->cancelbuffer;

    self->cancelbuffer  = buffer;

    return;
}

void pthread_cleanup_pop_inner (struct pthread_cleanup_buffer *buffer,
                                        int execute)
{
    pthread_info *self;

    PTHREAD_ENTRY ();

    self = pthread_self_info ();
    
    VXPTH_ASSERT (self->cancelbuffer == buffer, "Stacking error in cleanup buffers");
    
    if (self->cancelbuffer == buffer) {
        // Remove the buffer from the stack
        self->cancelbuffer = buffer->prev;
    }
    else {
        // If the top of the stack is not the buffer we expect, do not
        // execute it.
        execute = 0;
    }

    if (execute) buffer->routine (buffer->arg);
    return;
}

#endif /* __VXWORKS__ && _MGUSE_OWN_PTHREAD */

