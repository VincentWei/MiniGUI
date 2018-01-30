/**
 * \file psos.h
 * \author Wei Yongming <vincent@minigui.org>
 * \date 2007/05/28
 * 
 * \brief This header contains the declaration of pSOS+ system calls, the definition
 *        of related structures, and the error codes.
 *
 \verbatim

    This file is part of MiniGUI, a mature cross-platform windowing 
    and Graphics User Interface (GUI) support system for embedded systems
    and smart IoT devices.

    Copyright (C) 2008~2018, Beijing FMSoft Technologies Co., Ltd.

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
 * $Id: psos.h 7183 2007-05-16 08:19:34Z weiym $
 *
 *      MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *      pSOS, ThreadX, NuCleus, OSE, and Win32.
 */

#ifndef _PSOS_H
#define _PSOS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* Timed out; returned only if a timeout was requested. */
#define ERR_TIMEOUT     0x01
/* Illegal system service function number. */
#define ERR_SSFN        0x03
/* Node specifier out of range. */
#define ERR_NODENO      0x04
/* object has been deleted. */
#define ERR_OBJDEL      0x05
/* object_id is incorrect; failed validity check */
#define ERR_OBJID       0x06
/* object type doesn't match object ID; failed validity check.*/
#define ERR_OBJTYPE     0x07
/* Node's object table full. */
#define ERR_OBJTFULL    0x08
/* Named object not found. */
#define ERR_OBJNF       0x09
/* Informative; files may be corrupted on restart. */
#define ERR_RSTFS       0x0D
/* Exceeds node's maximum number of tasks. */
#define ERR_NOTCB       0x0E
/* Insufficient space in Region 0 to create stack. */
#define ERR_NOSTK       0x0F
/* Stack too small. */
#define ERR_TINYSTK     0x10
/* Priority out of range. For mu, the MU_PRIO_PROTECT flag has been specified. 
 * The priority must be more than 0 and less than 256. */
#define ERR_PRIOR       0x11
/* Task already started. */
#define ERR_ACTIVE      0x12
/* Cannot restart; this task never was started. */
#define ERR_NACTIVE     0x13
/* Task already suspended. */
#define ERR_SUSP        0x14
/* The task was not suspended. */
#define ERR_NOTSUSP     0x15
/* Cannot change priority; new priority out of range. */
#define ERR_SETPRI      0x16
/* Register number out of range. */
#define ERR_REGNUM      0x17
/* pHILE+ resources in use. */
#define ERR_DELFS       0x18
/* pREPC+ resources in use. */
#define ERR_DELLC       0x19
/* pNA+ resources in use. */
#define ERR_DELNS       0x1A

/* error cods for region manager are omitted ... */
/* error cods for partition manager are omitted ... */

/* Could not send a notification event. Error occurred while sending notification of message availability. */
#define ERR_NOTIFY      0x3D
/* Exceeds node's maximum number of semaphores. */
#define ERR_NOSCB       0x41
/* No semaphore: this error code returns only if SM_NOWAIT was selected. */
#define ERR_NOSEM       0x42
/* Semaphore deleted while task waiting. */
#define ERR_SKILLD      0x43
/* Informative only; there were sm_delete tasks waiting. */
#define ERR_TATSDEL     0x44
/* Maximum number of tokens are already available for a semaphore that was created with the SM_BOUNDED flag. */
#define ERR_BOUND       0x45
/* ZERO bound count is invalid. */
#define ERR_INVBOUND    0x46

/* error cods for timer are omitted ... */

/* object not created from this node. */
#define ERR_ILLRSC      0x53
/* Cannot wait; the remote node is out of Agents. */
#define ERR_NOAGNT      0x54
/* object does not exist any more. */
#define ERR_STALEID     0x65
/* Remote node is no longer in service. */
#define ERR_NDKLD       0x66
/* Exceeds node's maximum number of mutexes. */
#define ERR_NOMUCB      0x70
/* The MU_GLOBAL flag cannot be specified with the MU_PRIO_INHERIT flag. */
#define ERR_NOGLOBAL    0x71
/* Informative only: there were tasks waiting. */
#define ERR_TATMUDEL    0x72
/* The mutex is not owned by the calling task. */
#define ERR_NOTOWNED    0x73
/* The mutex is already locked by the calling task and it was created with MU_NORECURSIVE flag. */
#define ERR_NORECURSIVE 0x74
/* Some other task has already locked the mutex; this error is returned only if MU_NOWAIT flag has been specified. */
#define ERR_MULOCKED    0x75
/* Mutex deleted while the task was waiting on it.   */
#define ERR_MUKILLED    0x76
/* MU_FIFO flag cannot be specified with MU_PRIO_INHERIT or MU_PRIO_PROTECT. */
#define ERR_NOFIFO      0x77
/* Task was created with T_NORELMU flag set and currently owns some mutexes. */
#define ERR_OWNMUTX     0x79
/* Task¡¯s current priority exceedsthe ceiling priority of the guarding mutex. */
#define ERR_OUTCEIL     0x7A
/* Expected mutex object; failed validity. The mutex is not of priority protected type. */
#define ERR_MUTYPE      0x8A

/* other error cods are omitted ... */

/* system calls for ASR management */
#define T_PREEMPT       0x00000001 /* ASR is preemptible. */
#define T_NOPREEMPT     0x00000002 /* ASR is non-preemptible. */
#define T_TSLICE        0x00000010 /* ASR can be time-sliced. */
#define T_NOTSLICE      0x00000020 /* ASR cannot be time-sliced. */
#define T_ASR           0x00000100 /* ASR nesting enabled. */
#define T_NOASR         0x00000200 /* ASR nesting disabled. */
#define T_USER          0x00001000 /* ASR runs in user mode. */
#define T_SUPV          0x00002000 /* ASR runs in supervisor mode. */
#define T_ISR           0x00010000 /* Interrupts are enabled while ASR runs. */
#define T_NOISR         0x00020000 /* Interrupts are disabled while ASR runs. */

/*
 * This system call allows a task to specify an asynchronous signal routine 
 * (ASR) to handle asynchronous signals. as_catch() supplies the starting 
 * address of the task's ASR, and its initial execution mode. If the input 
 * ASR address is zero, then the caller is deemed to have an invalid ASR, 
 * and any signals sent to it will be rejected.
 *
 * A task's ASR gains control much like an ISR. If a task has pending signals 
 * (sent via as_send()), then the next time the task is dispatched to run, 
 * it will be forced to first execute the task's specified ASR. A task 
 * executes its ASR according to the mode supplied by the as_catch() call 
 * (for example, Non-preemptible, Time-slicing enabled, etc.) Upon entry to 
 * the ASR, all pending signals - including all those received since the 
 * last ASR invocation - are passed as an argument to the ASR. In addition,
 * a stack frame is built to facilitate the return from the ASR.
 *
 * as_catch() replaces any previous ASR for the calling task. Therefore, a 
 * task can have only one ASR at any time. An ASR must exit using the 
 * as_return() system call.
 */
unsigned long as_catch (
   void (* start_addr) (unsigned long),  /* ASR address */
   unsigned long mode           /* ASR attributes */
);

/*
 * This system call registers a set of bit-encoded events for the calling 
 * task that are used to notify the posting of asynchronous signals to 
 * the task. The event notification mechanism provides a way for a task to 
 * synchronously wait for asynchronous signals via an ev_receive() call.
 */
unsigned long as_notify (
   unsigned long events  /* bit-encoded events */
);

/*
 * This system call must be used by a task's ASR to exit and return to 
 * the original flow of execution of the task. The purpose of this call is 
 * to enable the pSOS+ kernel to restore the task to its state before 
 * the ASR. as_return() cannot be called except from an ASR.
 */
unsigned long as_return (void);

/*
 * This system call sends asynchronous signals to a task. Additionally, 
 * if signal notification via event has been requested, the notification 
 * events are also posted to the task. The purpose of these signals is to 
 * force a task to break from its normal flow of execution and execute 
 * its Asynchronous Signal Routine (ASR).
 *
 * Asynchronous signals are like software interrupts, with ASRs taking on 
 * the role of ISRs. Unlike an interrupt, which is serviced almost 
 * immediately, an asynchronous signal does not immediately affect the 
 * state of the task. An as_send() call is serviced only when the task is 
 * next dispatched to run (and that depends on the state of the task and 
 * its priority).
 *
 * Each task has 32 signals. These signals are encoded bit-wise in a single 
 * long word. 
 *
 * Like events, signals are neither queued nor counted.  For example, if 
 * three identical signals are sent to a task before its ASR has a chance 
 * to execute, the three signals have the same effect as one.
 */
unsigned long as_send (
   unsigned long tid,    /* target task ID */
   unsigned long signals /* bit-encoded signal list */
);

/* system calls for task management */

/* 
 * This system call adds a task variable to a task. t_addvar() allocates to the specified
 * task a storage area which is used to hold a private copy of the specified variable.
 */
unsigned long t_addvar (
	unsigned long tid,  /* task identifier */
	void **tv_addr,     /* address of variable */
	void *tv_value      /* initial value for task variable */
);

#define T_GLOBAL	0x0001
#define T_LOCAL		0x0002
#define T_FPU		0x0010
#define T_NOFPU		0x0020


/*
 * This service call enables a task to create a new task. t_create() allocates to the
 * new task a Task Control Block (TCB) and a memory segment for its stack(s). The
 * task stack sizes and scheduling priority are established with this call. t_create()
 * leaves the new task in a dormant state; the t_start() call must be used to place
 * the task into the ready state for execution.
 */
unsigned long t_create (
	char name[4], /* task name */
	unsigned long prio, /* task priority */
	unsigned long sstack, /* task supervisor stack size */
	unsigned long ustack, /* task user stack size */
	unsigned long flags, /* task attributes */
	unsigned long *tid /* task identifier */
);

/*
 * This service call enables a task to delete itself or another task. The pSOS+ kernel
 * first dispatches any task deletion callouts, registered via the co_register() pSOS+
 * service, in the reverse order of registration. When all of the deletion callouts have
 * finished, the pSOS+ kernel halts the task and reclaims its TCB, stack segment and
 * any allocated timers.
 * The calling task does not have to be the creator (parent) of the task to be deleted.
 * However, a task must be deleted from the node on which it was created.
 */
unsigned long t_delete (
	unsigned long tid /* task identifier */
);

/*
 * This system call removes a task variable from a task. t_delvar() deallocates the
 * private storage allocated for the task variable.
 */
unsigned long t_delvar (
	unsigned long tid, /* task identifier */
	void **tv_addr /* address of variable*/
);

/*
 * This system call enables the caller to obtain the contents of a task's notepad register.
 * Each task has 16 such software registers, held in the task's TCB. The purpose
 * of these registers is to furnish every task with a set of named, permanent variables.
 * Eight of these registers are reserved for system use. Eight are free to be used for application
 * specific purposes.
 */
unsigned long t_getreg (
	unsigned long tid, /* task identifier */
	unsigned long regnum, /* register number */
	unsigned long *reg_value /* register contents */
);

/*
 * This system call enables the calling task to obtain the task ID of a task it knows only
 * by name. This task ID can then be used in all other operations relating to the task.
 * Most system calls, except t_create() and t_ident(), reference a task by its task
 * ID. t_create() returns the task ID to a task's creator. For other tasks, one way to
 * obtain the task ID is to use t_ident().
 */
unsigned long t_ident (
	char name[4], /* task name */
	unsigned long node, /* node number */
	unsigned long *tid /* task ID */
);

#define TS_DEBUG	0x8000 /* Blocked by debugger */
#define TS_SUSP		0x4000 /* Suspended */
#define TS_TIMING	0x1000 /* Being timed */
#define TS_PAUSE	0x0800 /* Task paused */
#define TS_ABSTIME	0x0400 /* Timed for absolute time */
#define TS_VWAIT	0x0200 /* Waiting for events */
#define TS_SWAIT	0x0080 /* Waiting for semaphore */
#define TS_MWAIT	0x0040 /* Waiting for memory */
#define TS_QWAIT	0x0020 /* Waiting for message */
#define TS_RWAIT	0x0010 /* Waiting for reply packet */
#define TS_MUWAIT	0x0008 /* Waiting for mutex */
#define TS_CVWAIT	0x0004 /* Waiting for condition variable */
#define TS_CWAIT	0x0001 /* Waiting for component resource */
#define TS_READY	0x0000 /* Task is in Ready State */

struct tinfo {
	char name[4]; 		/* Task name */
	unsigned long flags; 	/* Task attributes */
	void (*iip)(void); 	/* Task's initial starting addr. */
	unsigned long next; 	/* ID of the next waiting task */
	unsigned short status; 	/* Task status.*/
	unsigned char cpriority; 	/* Task's current priority.*/
	unsigned char bpriority; 	/* Task's base priority */
	unsigned char ipriority; 	/* Task's initial priority */
	unsigned char evwantcond; 	/* Task's event wait condition */
	unsigned short mode; 	/* Task's current mode.*/
	unsigned short imode; 	/* Task's initial mode */
	unsigned short amode; 	/* Task's ASR mode.*/
	unsigned long tslice_quantum;	/* Per task's time slice in ticks*/
	long tslice_remain; 	/* Remainder time slice in ticks.*/
	unsigned long wtobid; 	/* Object where task is blocked */
	unsigned long evwait;	/* Events - task waiting for.*/
	unsigned long evcaught;	/* Events caught */
	unsigned long evrcvd;	/* Events received */
	unsigned long ss_size;	/* Supervisor stack size */
	unsigned long us_size;	/* User stack size */
	unsigned long *ssp;	/* Current supervisor stack */
	/* pointer */
	unsigned long *issp;	/* Initial supervisor stack pointer */
	unsigned long *usp;	/* Current user stack pointer */
	unsigned long *iusp;	/* Initial user stack pointer */
	unsigned long imask;	/* Interrupt priority level */
	void (*asr_addr)(void);	/* Task's ASR address */
	unsigned long signal;	/* Asynchronous signals pending */
	unsigned long xdate;	/* Timer expiry date */
	unsigned long xtime;	/* Timer expiry time */
	unsigned long xticks;	/* Timer expiry ticks */
	unsigned long nrnunits;	/* No. of region units wanted */
	void **tsdp;	/* Task_specific_Data pointer */
	unsigned long co_toproc;	/* Callouts to process */
	unsigned long co_inprog;	/* Callouts in progress */
	unsigned long evasr_ntfy;	/* ev used for ASR notification */
};

/*
 * This system call returns information of a specified task. The information includes
 * the static information that was specified at object creation time and certain internal
 * state information which is not static.
 */
unsigned long t_info (
	unsigned long tid, /* Task ID */
	struct tinfo *buf  /* Object Information buffer */
);

#if 0
#define T_PREEMPT	0x0001 /* Task is preemptible. */
#define T_NOPREEMPT	0x0002 /* Task is non-preemptible. */
#define T_TSLICE 	0x0004 /* Task can be time-sliced. */
#define T_NOTSLICE	0x0008 /* Task cannot be time-sliced. */
#define T_ASR		0x0010 /* Task's ASR is enabled. */
#define T_NOASR		0x0020 /* Task's ASR is disabled. */
#define T_USER		0X0040 /* Task runs in user mode. */
#define T_SUPV		0x0080 /* Task runs in supervisor mode. */
#define T_ISR		0x0100 /* Hardware interrupts are enabled while the task runs. */
#define T_NOISR		0x0200 /* Hardware interrupts are disabled while the task runs. */
#endif

/*
 * This system call enables a task to modify certain execution mode fields. These are
 * preemption on/off, roundrobin on/off, asynchronous signal handling on/off, and
 * interrupt control.
 * Preemption has precedence over timeslicing. Therefore, if preemption is off, timeslicing
 * does not occur whether or not it is set.
 * The calling task can be preempted as a result of this call, if its preemptibility is
 * turned from off to on and a higher priority task is ready to run.
 * To obtain a task's current execution mode without changing it, use a mask of 0.
 */
unsigned long t_mode (
	unsigned long mask, /* attributes to be changed */
	unsigned long new_mode, /* new attributes */
	unsigned long *old_mode /* prior mode */
);

/*
 * This system call forces a task to resume execution at its original start address regardless
 * of its current state or place of execution. If the task was blocked, the
 * pSOS+ kernel forcibly unblocks it. The task's priority and stacks are set to the original
 * values that t_create() specified. Its start address and execution mode are reset
 * to the original values established by t_start(). Any pending events, signals, or
 * armed timers are cleared. Thereafter, pSOS+ posts any "task restart callouts" that
 * are dispatched in the order they were registered when the task is scheduled to run.
 * All of the "task restart callouts" so dispatched run to completion before the task regains
 * control at its starting address.
 * The t_restart() call accepts a new set of up to four arguments, which, among
 * other things, can be used by the task to distinguish between the initial startup and
 * subsequent restarts.
 * The calling task does not have to be the creator (or parent) of the task it restarts.
 * However, a task must be restarted from the node on which it was created.
 */
unsigned long t_restart (
	unsigned long tid, /* task identifier */
	unsigned long targs[4] /* startup arguments */
);

/*
 * This system call removes the suspension of a task. If the task was suspended while
 * in the ready state, t_resume() releases it to be scheduled for execution. If the task
 * was both suspended and blocked (for example, waiting for a message), t_resume()
 * removes only the suspension. This leaves the task in the blocked state.
 */
unsigned long t_resume (
	unsigned long tid /* task identifier */
);

/*
 * This system call enables the calling task to optionally obtain and modify either its
 * own or another task's base scheduling (software) priority. If oldprio is a non-NULL
 * address, the previous base scheduling priority is returned in the variable pointed to
 * by oldprio, regardless of the task's current priority. If the calling task issues
 * t_setpri() for a target task (self or another) with a valid non-0 value of newprio,
 * then the target task (whether it is running or not running) shall resume execution
 * after all other runnable tasks of equal or greater priority have been scheduled to
 * run.
 */
unsigned long t_setpri (
	unsigned long tid, /* task identifier */
	unsigned long newprio, /* new priority */
	unsigned long *oldprio /* previous priority */
);

/*
 * This system call enables the caller to modify the contents of a task's notepad register.
 * Each task has 16 such software registers, held in the task's TCB. The purpose
 * of these registers is to furnish every task with a set of named, permanent variables.
 * Eight of these registers are reserved for system use, and eight are free to be used for
 * application-specific purposes.
 */
unsigned long t_setreg (
	unsigned long tid, /* task identifier */
	unsigned long regnum, /* register number */
	unsigned long reg_value /* register value */
);

/*
 * This system call places a newly created task into the ready state to await scheduling
 * for execution. Thereafter, pSOS+ posts any "task startup callouts" that are dispatched
 * in the order they were registered when the task is scheduled to run. All of
 * the "task startup callouts" so dispatched run to completion before the task regains
 * control at its starting address. The calling task does not have to be the creator (or
 * parent) of the task to be started. However, a task must be started from the node on
 * which it was created.
 */
unsigned long t_start (
	unsigned long tid, /* task identifier */
	unsigned long mode, /* initial task attributes */
	void (*start_addr) (unsigned long), /* task address */
	unsigned long targs[4] /* startup task arguments */
);

/*
 * This system call suspends execution of a task until a t_resume() call is made for
 * the suspended task. The calling task suspends either itself or another task. The
 * t_suspend() call prevents the specified task from contending for CPU time but
 * does not directly prevent contention for any other resource.
 */
unsigned long t_suspend (
	unsigned long tid /* task identifier */
);

/*
 * This system call enables the calling task to obtain and optionally modify its own or
 * another task's timeslice quantum. It allows applications to control how much execution
 * time is distributed among tasks with the same priority. It also returns the previous
 * timeslice quantum.
 */
unsigned long t_tslice (
	unsigned long tid, /* task identifier */
	unsigned long new_tslice, /* new timeslice value*/
	unsigned long *old_tslice /* old timeslice value */
);


/* system calls for mutext */

#define MU_GLOBAL		0x0001 /* Mutex is globally addressable by other nodes of a multiprocessor system. The single-processor version of the pSOS+ kernel ignores MU_GLOBAL. */
#define MU_LOCAL		0x0002 /* Mutex can be addressed only by the local node where it was created. */
#define MU_RECURSIVE		0x0004 /* Mutex can be acquired in a recursive fashion. */
#define MU_NORECURSIVE		0x0008 /* Mutex cannot be acquired in a recursive fashion. */
#define MU_FIFO			0x0010 /* The waiting tasks are queued in the FIFO order.  This flag can be specified with MU_PRIO_NONE flag, but not with MU_PRIO_INHERIT or MU_PRIO_PROTECT. */
#define MU_PRIOR		0x0020 /* The waiting tasks are queued in decreasing order of their current priority. */
#define MU_PRIO_NONE		0x0100 /* Mutex does not protect against unbounded priority inversion. */
#define MU_PRIO_INHERIT		0x0200 /* Mutex uses the priority inheritance protocol to prevent unbounded priority inversion. */
#define MU_PRIO_PROTECT		0x0400 /* Mutex uses the priority protect protocol to prevent unbounded priority inversion. */

/*
 * This system call creates a mutex and initializes it according to the specifications
 * supplied with the call.
 */
unsigned long mu_create (
	char name[4], /* user assigned name */
	unsigned long flags, /* mutex attributes */
	unsigned long ceiling, /* ceiling priority */
	unsigned long *muid /* newly created mutex id */
);

/*
 * This system call deletes a mutex with the specified mutex ID and frees the associated
 * kernel resources. This call can be made only from the node where the specified
 * mutex was created.
 * The mu_delete() system call can be invoked by any task, and not necessarily the
 * task that originally created the mutex. In the event that there are other tasks waiting
 * to acquire that mutex, the tasks are unblocked and an error code is returned to
 * them.
 */
unsigned long mu_delete (
	unsigned long muid /* MUTEX identifier */
);

/*
 * This system call enables the calling task to obtain the mutex ID of a mutex it only
 * knows by name. This mutex ID can be used in all other operations relating to the
 * mutex.
 * The pSOS+ kernel does not check for duplicate names. If multiple mutexes with the
 * same name exist, either on the local node, or on a remote note in a multi-processor
 * system, a mu_ident() call may return the ID of any one mutex with the specified
 * name. It can't be ascertained which ID is returned, though, to a certain extent it depends
 * on the standard search order followed by pSOS+ kernel, as defined in the
 * pSOSystem System Concepts manual.
 */
unsigned long mu_ident (
	char name[4], /* mutex name */
	unsigned long node, /* node number */
	unsigned long *muid /* mutex identifier */
);

struct muinfo {
	char name[4]; /* Name of the Mutex */
	unsigned long flags; /* Mutex attributes */
	unsigned long wqlen; /* No. of waiting tasks */
	unsigned long wtid;/* ID of the first waiting task */
	unsigned long count; /* Mutex reference count */
	unsigned long ownid; /* ID of mutex owner */
	unsigned long node; /* Owner task's node number */
	unsigned long ceilprio; /* ceiling priority for MU_PROTECT mutexes */
	unsigned long phpwt; /* Highest of all waiting task priorities */
};

/*
 * This system call provides information of a specified Mutex object. The information
 * includes the static information that was specified at object creation time and certain
 * internal state information which is not static.
 */
unsigned long mu_info (
	unsigned long muid, /* Mutex ID */
	struct muinfo *buf  /* Object Information buffer */
);

#define MU_WAIT		0x01 /* If the mutex is locked, block until it is available. */
#define MU_NOWAIT	0x02 /* Return with error if the mutex is locked. */

/*
 * This system call allows a task to lock a mutex specified by the muid argument. If the
 * mutex is not locked by another task, the mutex is put in the locked state, and the
 * calling task becomes the owner of the mutex. If the mutex was created with the
 * MU_RECURSIVE attribute set (see mu_create on page 2-87), a task can invoke
 * mu_lock() multiple times to acquire the same mutex in a recursive fashion. The
 * same number of mu_unlock() calls must then be made by the same task to release
 * the mutex. A mutex created with the MU_NORECURSIVE flag can be locked only once
 * by any task. If the task calls mu_lock again while it is holding the mutex, the call
 * will return with an error.
 * If the mutex is locked by another task then, depending on the flags argument, the
 * calling task is either put in the blocked state and is queued in the wait queue associated
 * with the mutex, or an error is returned.
 */
unsigned long mu_lock (
	unsigned long muid, /* mutex identifier */
	unsigned long flags, /* call attributes */
	unsigned long timeout /* timeout interval */
);

/*
 * This system call enables the calling task to optionally obtain and modify a mutex's
 * ceiling priority. The call only operates on mutexes that have been created with the
 * MU_PRIO_PROTECT flag. If oldprio is a non-NULL address, the previous ceiling priority
 * is returned in the variable pointed to by oldprio.
 * The mu_setceil() call either locks the mutex if it is unlocked, or blocks until it
 * can successfully lock the mutex; then it changes the priority ceiling of the mutex
 * and releases the mutex. If the calling task already owns the mutex when it invokes
 * the mu_setceil() call with a valid non-0 value of newprio, then the task's current
 * priority may change according to the priority protect protocol.
 */
unsigned long mu_setceil (
	unsigned long muid, /* mutex identifier */
	unsigned long newprio, /* new ceiling priority */
	unsigned long *oldprio /* previous ceiling priority */
);

/*
 * This system call allows a task to unlock a mutex specified by the muid argument. If
 * the caller currently owns the mutex then mu_unlock() call decreases the recursive
 * level associated with the mutex. If the recursive level reaches zero, the mutex is unlocked
 * and the calling task loses ownership of the mutex. An error is returned if the
 * calling task does not currently own the mutex.
 * For mutexes which have not been created with the MU_PRIO_PROTECT flag, when
 * the calling task loses the ownership of the mutex, and if any tasks are waiting to
 * lock the mutex, the ownership of the mutex is transferred to the waiting task at the
 * head of the mutex's wait queue. It is unblocked, and made ready-to-run.
 * For mutexes which have been created with the MU_PRIO_PROTECT flag, when the
 * calling task loses ownership of the mutex, there could be tasks waiting at the mutex's
 * wait queue, on one of two action types. If the task at the head of the queue is
 * waiting to lock the mutex, the ownership of the mutex is transferred to the waiting
 * task, and its priority is raised if necessary. It is unblocked, and made ready-to-run.
 * If the task at the head of the queue is waiting to change the ceiling priority of the
 * mutex, the ceiling priority is modified, the task is unblocked and made ready-torun;
 * and the next task in the waiting queue is checked. This process of determining
 * the requested action type associated with a waiting task goes on till either the queue
 * is empty, or a task which can successfully lock the mutex is found.
 */
unsigned long mu_unlock (
	unsigned long muid /* mutex identifier */
);

/*
 * This system call is used to asynchronously release a semaphore token. It is identical
 * to sm_v() except the call is made asynchronously. Refer to the description of
 * sm_v() for further information. This call is only supported by the pSOS+m kernel
 * (the multiprocessor version).
 */
unsigned long sm_av (
	unsigned long smid /* semaphore identifier */
);

#define SM_GLOBAL	0x0001 /* Semaphore can be addressed by other nodes. */
#define SM_LOCAL	0x0002 /* Semaphore can be addressed by local node only. */
#define SM_PRIOR	0x0004 /* Tasks are queued by priority. */
#define SM_FIFO		0x0008 /* Tasks are queued by FIFO. */
#define SM_UNBOUNDED	0x0010 /* Semaphore count is not bounded. */
#define SM_BOUNDED	0x0020 /* Semaphore count is bounded. When the SM_BOUNDED flag is specified the count parameter specifies the initial semaphore token count, as well as the upper bound of available tokens. */

/*
 * This system call creates a semaphore by allocating and initializing a Semaphore
 * Control Block (SMCB) according to the specifications supplied with the call.
 * Like all objects, a semaphore has a user-assigned name, and a pSOS+-assigned
 * semaphore ID returned by sm_create(). Several flag bits specify the characteristics
 * of the semaphore, including whether tasks will wait for the semaphore by task
 * priority or strictly FIFO.
 */
unsigned long sm_create (
	char name[4], /* semaphore name */
	unsigned long count, /* number of tokens */
	unsigned long flags, /* semaphore attributes */
	unsigned long *smid /* semaphore identifier */
);

/*
 * This system call deletes the semaphore with the specified semaphore ID, and frees
 * the SMCB to be reused. sm_delete() takes care of cleaning up the semaphore. If
 * there are tasks waiting, they will be unblocked and given an error code.
 * The calling task does not have to be the creator (parent) of the semaphore to be deleted.
 * However, a semaphore must be deleted from the node on which it was created.
 */
unsigned long sm_delete (
	unsigned long smid /* semaphore ID */
);

/*
 * This system call enables the calling task to obtain the semaphore ID of a semaphore
 * it only knows by name. The semaphore ID can then be used in all other operations
 * relating to this semaphore.
 * Most system calls, except sm_create() and sm_ident(), reference a semaphore
 * by the semaphore ID. sm_create() returns the semaphore ID to the semaphore
 * creator. For other tasks, one way to obtain the semaphore ID is to use sm_ident().
 */
unsigned long sm_ident (
	char name[4], /* semaphore name */
	unsigned long node, /* node selector */
	unsigned long *smid /* semaphore ID */
);

struct sminfo {
	char name[4]; 		/* Name of semaphore */
	unsigned long flags; 	/* Semaphore attributes */
	unsigned long wqlen; 	/* No. of waiting tasks */
	unsigned long wtid; 	/* ID of the first waiting task */
	unsigned long count; 	/* Semaphore count */
	unsigned long maxcount; /* Limit for bounded semaphores */
	unsigned long tid_ntfy; /* Task to notify of availability */
	unsigned long ev_ntfy;  /* Ev to post to notify availability */
};

/*
 * This system call returns information of a specified Semaphore object. The information
 * includes the static information that was specified at object creation time and
 * certain internal state information which is not static.
 */
unsigned long sm_info (
	unsigned long smid, /* Semaphore ID */
	struct sminfo *buf  /* Object Information buffer */
);

/*
 * This system call registers a set of bit-encoded events and a task ID for the given
 * semaphore. Once so registered, a notification is sent to the task whenever a semaphore
 * token becomes available, via an implicit call of the form: ev_send(tid,
 * events). The task can choose to receive the notification via ev_receive() call.
 */
unsigned long sm_notify (
	unsigned long smid, /* ID of target semaphore */
	unsigned long tid, /* ID of task to be notified */
	unsigned long events /* bit-encoded events */
);

#define SM_WAIT		0x01 /* Block until semaphore is available. */
#define SM_NOWAIT	0x02 /* Return with error code if semaphore is unavailable. */

/*
 * This system call enables a task or an ISR to acquire a semaphore token. A calling
 * task can specify whether or not it wants to wait for the token. If the semaphore token
 * count is positive, then this call returns the semaphore token immediately. If the
 * semaphore token count is zero and the calling task specified SM_NOWAIT, then
 * sm_p() returns with an error code. If SM_WAIT is elected, the task will be blocked
 * until a semaphore token is released, or if the timeout argument is specified, until
 * timeout occurs, whichever occurs first.
 */
unsigned long sm_p (
	unsigned long smid, /* semaphore identifier */
	unsigned long flags, /* attributes */
	unsigned long timeout /* timeout */
);

/*
 * This system call is used to release a semaphore token. If a task is already waiting at
 * the semaphore, it is unblocked and made ready to run. If there is no task waiting,
 * then the semaphore token count is simply incremented by 1.
 * If, as a result of this call, a semaphore token becomes available, and a set of task
 * and events had been registered via a prior call to sm_vnotify() to notify the availability
 * of the semaphore token, pSOS+ also performs an ev_send() operation to
 * send the registered events to the registered task.
 */
unsigned long sm_v (
	unsigned long smid /* semaphore identifier */
);

/* 
 * This system call asynchronously removes all tasks from a condition-variable¡¯s wait
 * queue. It is identical to cv_broadcast() except that the call is made asynchro-
 * nously. Refer to the description of cv_broadcast() for further information.
 */
unsigned long cv_abroadcast (
   unsigned long cvid  /* condition variable identifier */
);

/*
 * This system call asynchronously removes the task at the head of a condition-
 * variable¡¯s queue. It is identical to cv_signal() except that the call is made asyn-
 * chronously. Refer to the description of cv_signal() for further information.
 */
unsigned long cv_asignal (
   unsigned long cvid   /* condition variable ID */
);

/*
 * This system call removes all the tasks from a condition variable¡¯s wait queue. If the
 * mutex guarding the condition variable is unlocked, the task at the head of the wait
 * queue is given ownership of the guarding mutex and unblocked. The remaining
 * tasks, including the task at the head of the wait queue if the guarding mutex is
 * locked, are placed in the guarding mutex¡¯s wait queue.
 * If there are no tasks waiting, then cv_broadcast() does not do anything.
 */
unsigned long cv_broadcast (
   unsigned long cvid    /* condition variable identifier */
);

/*
 * This system call creates a condition variable and initializes it according to the 
 * specifications supplied with the call.
 * Like all objects, a condition variable has a user-assigned name and a pSOS-
 * assigned condition variable ID returned by cv_create(). Several flag bits specify
 * the characteristics of the condition variable. Tasks can wait at the condition variable
 * either by task priority or strictly FIFO.
 */
unsigned long cv_create (
   char name[4],
   unsigned long flags,
   unsigned long *cvid
);

/* 
 * This system call deletes a condition variable specified by its ID (cvid) and frees the
 * associated kernel resources. If there are tasks waiting at the condition variable, the
 * tasks are unblocked and given an error code.
 * The calling task does not have to be the creator of the condition variable to be de-
 * leted. However, a condition variable must be deleted from the node on which it was
 * created.
 */
unsigned long cv_delete (
   unsigned long cvid    /* condition variable identifier */
);

/* 
 * This system call enables the calling task to obtain the ID of a condition variable it
 * only knows by name. This condition variable ID can be used in all other operations
 * relating to the condition variable.
 * The pSOS+ kernel does not check for duplicate names. If multiple condition vari-
 * ables with the same name exist, either on the local node, or on a remote note in a
 * multi-processor system, a cv_ident() call may return the ID of any one condition
 * variable with the specified name. It can¡¯t be ascertained which ID is returned,
 * though; to a certain extent it depends on the standard search order followed by
 * pSOS+ kernel, as defined in the pSOSystem System Concepts manual.
 */
unsigned long cv_ident (
   char name[4],        /* condition variable name */
   unsigned long node,  /* node number */
   unsigned long *cvid  /* condition variable identifier */
);

#if 0
/* 
 * This system call returns object information for a given condition variable object. The
 * information includes the static information that was specified at object creation
 * time and certain internal state information which is not static.
 */
unsigned long cv_info (
   unsigned long cvid,
   struct cvinfo *buf
);
#endif

/*
 * This system call removes the task at the head of a condition variable¡¯s wait queue
 * from that queue. If the mutex guarding the condition variable is unlocked, the task
 * is given ownership of the guarding mutex and unblocked. If the mutex is already
 * locked, then the task is placed in the guarding mutex¡¯s wait queue according to the
 * queueing policy of the mutex.
 * If there are no tasks waiting at the condition variable, this call does nothing.
 */
unsigned long cv_signal (
   unsigned long cvid    /* condition variable identifier */
);

/*
 * This system call is used by a task to wait on a condition variable. The guarding mu-
 * tex must be owned by the caller. If tasks are waiting at the condition variable, then
 * muid must specify the mutex already guarding the condition variable. Otherwise,
 * any mutex may be specified.
 * cv_wait() unlocks the guarding mutex and blocks the calling task. Unless a time-
 * out occurs, the calling task waits at the condition variable until removed from the
 * wait queue by a cv_signal() or a cv_broadcast() call. When removed, owner-
 * ship of the guarding mutex is given to the task if the guarding mutex is currently
 * unlocked. Otherwise, the task waits for ownership of the guarding mutex according
 * to the queueing policy of the mutex.
 */
unsigned long cv_wait (
   unsigned long cvid,   /* condition variable identifier */
   unsigned long muid,   /* mutex identifier */
   unsigned long timeout /* timeout interval */
);

/*
 * This system call returns the address of the calling task's internal errno variable.
 * The pSOS+ kernel maintains an internal errno variable for every task. Whenever
 * an error is detected by any pSOSystem component, the associated error code is
 * stored into the running task's internal errno variable. The error code can then be
 * retrieved by referencing the errno macro de????ed in the header ????e <psos.h> as
 * follows:
 *      #define errno (*(errno_addr())
 * For example, the following statement expands to include a call to errno_addr():
 *      if (errno == ERR_NOMGB)
 */

unsigned long *errno_addr (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _PSOS_H */

