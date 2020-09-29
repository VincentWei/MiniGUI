/*
	Copyright (C) 2009 William Davy - william.davy@wittenstein.co.uk
	Contributed to FreeRTOS.org V5.3.0.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License (version 2) as published
	by the Free Software Foundation and modified by the FreeRTOS exception.

	FreeRTOS.org is distributed in the hope that it will be useful,	but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA  02111-1307  USA.

	A special exception to the GPL is included to allow you to distribute a
	combined work that includes FreeRTOS.org without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details.


	***************************************************************************
	*                                                                         *
	* Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
	*                                                                         *
	* This is a concise, step by step, 'hands on' guide that describes both   *
	* general multitasking concepts and FreeRTOS specifics. It presents and   *
	* explains numerous examples that are written using the FreeRTOS API.     *
	* Full source code for all the examples is provided in an accompanying    *
	* .zip file.                                                              *
	*                                                                         *
	***************************************************************************

	1 tab == 4 spaces!

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting,
	licensing and training services.
*/

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the Posix port.
 *----------------------------------------------------------*/

#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
/*-----------------------------------------------------------*/
#ifndef MAX_NUMBER_OF_TASKS
#define MAX_NUMBER_OF_TASKS 		( _POSIX_THREAD_THREADS_MAX )
#endif
/*-----------------------------------------------------------*/

/* Parameters to pass to the newly created pthread. */
typedef struct XPARAMS
{
	pdTASK_CODE pxCode;
	void *pvParams;
} xParams;

/* Each task maintains its own interrupt status in the critical nesting variable. */
typedef struct THREAD_SUSPENSIONS
{
	pthread_t hThread;
	xTaskHandle hTask;
	unsigned portBASE_TYPE uxCriticalNesting;
} xThreadState;
/*-----------------------------------------------------------*/

static xThreadState *pxThreads;
static pthread_once_t hSigSetupThread = PTHREAD_ONCE_INIT;
static pthread_attr_t xThreadAttributes;
static pthread_mutex_t xSuspendResumeThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t xSingleThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t hMainThread = ( pthread_t )NULL;
/*-----------------------------------------------------------*/

static volatile portBASE_TYPE xSentinel = 0;
static volatile portBASE_TYPE xSchedulerEnd = pdFALSE;
static volatile portBASE_TYPE xInterruptsEnabled = pdTRUE;
static volatile portBASE_TYPE xServicingTick = pdFALSE;
static volatile portBASE_TYPE xPendYield = pdFALSE;
static volatile portLONG lIndexOfLastAddedTask = 0;
static volatile unsigned portBASE_TYPE uxCriticalNesting;
/*-----------------------------------------------------------*/

/*
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt( void );
static void *prvWaitForStart( void * pvParams );
static void prvSuspendSignalHandler(int sig);
static void prvResumeSignalHandler(int sig);
static void prvSetupSignalsAndSchedulerPolicy( void );
static void prvSuspendThread( pthread_t xThreadId );
static void prvResumeThread( pthread_t xThreadId );
static pthread_t prvGetThreadHandle( xTaskHandle hTask );
static portLONG prvGetFreeThreadState( void );
static void prvSetTaskCriticalNesting( pthread_t xThreadId, unsigned portBASE_TYPE uxNesting );
static unsigned portBASE_TYPE prvGetTaskCriticalNesting( pthread_t xThreadId );
static void prvDeleteThread( void *xThreadId );
/*-----------------------------------------------------------*/

/*
 * Exception handlers.
 */
void vPortYield( void );
void vPortSystemTickHandler( int sig );

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
void vPortStartFirstTask( void );
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
/* Should actually keep this struct on the stack. */
xParams *pxThisThreadParams = pvPortMalloc( sizeof( xParams ) );

	(void)pthread_once( &hSigSetupThread, prvSetupSignalsAndSchedulerPolicy );

	if ( (pthread_t)NULL == hMainThread )
	{
		hMainThread = pthread_self();
	}

	/* No need to join the threads. */
	pthread_attr_init( &xThreadAttributes );
	pthread_attr_setdetachstate( &xThreadAttributes, PTHREAD_CREATE_DETACHED );

	/* Add the task parameters. */
	pxThisThreadParams->pxCode = pxCode;
	pxThisThreadParams->pvParams = pvParameters;

	vPortEnterCritical();

	lIndexOfLastAddedTask = prvGetFreeThreadState();

	/* Create the new pThread. */
	if ( 0 == pthread_mutex_lock( &xSingleThreadMutex ) )
	{
		xSentinel = 0;
		if ( 0 != pthread_create( &( pxThreads[ lIndexOfLastAddedTask ].hThread ), &xThreadAttributes, prvWaitForStart, (void *)pxThisThreadParams ) )
		{
			/* Thread create failed, signal the failure */
			pxTopOfStack = 0;
		}

		/* Wait until the task suspends. */
		(void)pthread_mutex_unlock( &xSingleThreadMutex );
		while ( xSentinel == 0 );
		vPortExitCritical();
	}

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void vPortStartFirstTask( void )
{
	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	/* Start the first task. */
	vPortEnableInterrupts();

	/* Start the first task. */
	prvResumeThread( prvGetThreadHandle( xTaskGetCurrentTaskHandle() ) );
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
portBASE_TYPE xResult;
int iSignal;
sigset_t xSignals;
sigset_t xSignalToBlock;
sigset_t xSignalsBlocked;
portLONG lIndex;

	/* Establish the signals to block before they are needed. */
	sigfillset( &xSignalToBlock );

	/* Block until the end */
	(void)pthread_sigmask( SIG_SETMASK, &xSignalToBlock, &xSignalsBlocked );

	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		pxThreads[ lIndex ].uxCriticalNesting = 0;
	}

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Start the first task. Will not return unless all threads are killed. */
	vPortStartFirstTask();

	/* This is the end signal we are looking for. */
	sigemptyset( &xSignals );
	sigaddset( &xSignals, SIG_RESUME );

	while ( pdTRUE != xSchedulerEnd )
	{
		if ( 0 != sigwait( &xSignals, &iSignal ) )
		{
			printf( "Main thread spurious signal: %d\n", iSignal );
		}
	}

	printf( "Cleaning Up, Exiting.\n" );
	/* Cleanup the mutexes */
	xResult = pthread_mutex_destroy( &xSuspendResumeThreadMutex );
	xResult = pthread_mutex_destroy( &xSingleThreadMutex );
	vPortFree( (void *)pxThreads );

	/* Should not get here! */
	return xResult;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
portBASE_TYPE xNumberOfThreads;
portBASE_TYPE xResult;
	for ( xNumberOfThreads = 0; xNumberOfThreads < MAX_NUMBER_OF_TASKS; xNumberOfThreads++ )
	{
		if ( ( pthread_t )NULL != pxThreads[ xNumberOfThreads ].hThread )
		{
			/* Kill all of the threads, they are in the detached state. */
			xResult = pthread_cancel( pxThreads[ xNumberOfThreads ].hThread );
            if (xResult)
                printf("pthread_cancel error!\n");
		}
	}

	/* Signal the scheduler to exit its loop. */
	xSchedulerEnd = pdTRUE;
	(void)pthread_kill( hMainThread, SIG_RESUME );
}
/*-----------------------------------------------------------*/

void vPortYieldFromISR( void )
{
	/* Calling Yield from a Interrupt/Signal handler often doesn't work because the
	 * xSingleThreadMutex is already owned by an original call to Yield. Therefore,
	 * simply indicate that a yield is required soon.
	 */
	xPendYield = pdTRUE;
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
	vPortDisableInterrupts();
	uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
	/* Check for unmatched exits. */
	if ( uxCriticalNesting > 0 )
	{
		uxCriticalNesting--;
	}

	/* If we have reached 0 then re-enable the interrupts. */
	if( uxCriticalNesting == 0 )
	{
		/* Have we missed ticks? This is the equivalent of pending an interrupt. */
		if ( pdTRUE == xPendYield )
		{
			xPendYield = pdFALSE;
			vPortYield();
		}
		vPortEnableInterrupts();
	}
}
/*-----------------------------------------------------------*/

void vPortYield( void )
{
pthread_t xTaskToSuspend;
pthread_t xTaskToResume;

	if ( 0 == pthread_mutex_lock( &xSingleThreadMutex ) )
	{
		xTaskToSuspend = prvGetThreadHandle( xTaskGetCurrentTaskHandle() );

		vTaskSwitchContext();

		xTaskToResume = prvGetThreadHandle( xTaskGetCurrentTaskHandle() );
		if ( xTaskToSuspend != xTaskToResume )
		{
			/* Remember and switch the critical nesting. */
			prvSetTaskCriticalNesting( xTaskToSuspend, uxCriticalNesting );
			uxCriticalNesting = prvGetTaskCriticalNesting( xTaskToResume );
			/* Switch tasks. */
			prvResumeThread( xTaskToResume );
			prvSuspendThread( xTaskToSuspend );
		}
		else
		{
			/* Yielding to self */
			(void)pthread_mutex_unlock( &xSingleThreadMutex );
		}
	}
}
/*-----------------------------------------------------------*/

void vPortDisableInterrupts( void )
{
	xInterruptsEnabled = pdFALSE;
}
/*-----------------------------------------------------------*/

void vPortEnableInterrupts( void )
{
	xInterruptsEnabled = pdTRUE;
}
/*-----------------------------------------------------------*/

portBASE_TYPE xPortSetInterruptMask( void )
{
portBASE_TYPE xReturn = xInterruptsEnabled;
	xInterruptsEnabled = pdFALSE;
	return xReturn;
}
/*-----------------------------------------------------------*/

void vPortClearInterruptMask( portBASE_TYPE xMask )
{
	xInterruptsEnabled = xMask;
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt( void )
{
struct itimerval itimer, oitimer;
portTickType xMicroSeconds = portTICK_PERIOD_MS * 1000;

	/* Initialise the structure with the current timer information. */
	if ( 0 == getitimer( TIMER_TYPE, &itimer ) )
	{
		/* Set the interval between timer events. */
		itimer.it_interval.tv_sec = 0;
		itimer.it_interval.tv_usec = xMicroSeconds;

		/* Set the current count-down. */
		itimer.it_value.tv_sec = 0;
		itimer.it_value.tv_usec = xMicroSeconds;

		/* Set-up the timer interrupt. */
		if ( 0 != setitimer( TIMER_TYPE, &itimer, &oitimer ) )
		{
			printf( "Set Timer problem.\n" );
		}
	}
	else
	{
		printf( "Get Timer problem.\n" );
	}
}
/*-----------------------------------------------------------*/

void vPortSystemTickHandler( int sig )
{
pthread_t xTaskToSuspend;
pthread_t xTaskToResume;

    (void)(sig);
	if ( ( pdTRUE == xInterruptsEnabled ) && ( pdTRUE != xServicingTick ) )
	{
		if ( 0 == pthread_mutex_trylock( &xSingleThreadMutex ) )
		{
			xServicingTick = pdTRUE;

			xTaskToSuspend = prvGetThreadHandle( xTaskGetCurrentTaskHandle() );
			/* Tick Increment. */
			xTaskIncrementTick();

			/* Select Next Task. */
#if ( configUSE_PREEMPTION == 1 )
			vTaskSwitchContext();
#endif
			xTaskToResume = prvGetThreadHandle( xTaskGetCurrentTaskHandle() );

			/* The only thread that can process this tick is the running thread. */
			if ( xTaskToSuspend != xTaskToResume )
			{
				/* Remember and switch the critical nesting. */
				prvSetTaskCriticalNesting( xTaskToSuspend, uxCriticalNesting );
				uxCriticalNesting = prvGetTaskCriticalNesting( xTaskToResume );
				/* Resume next task. */
				prvResumeThread( xTaskToResume );
				/* Suspend the current task. */
				prvSuspendThread( xTaskToSuspend );
			}
			else
			{
				/* Release the lock as we are Resuming. */
				(void)pthread_mutex_unlock( &xSingleThreadMutex );
			}
			xServicingTick = pdFALSE;
		}
		else
		{
			xPendYield = pdTRUE;
		}
	}
	else
	{
		xPendYield = pdTRUE;
	}
}
/*-----------------------------------------------------------*/

void vPortForciblyEndThread( void *pxTaskToDelete )
{
xTaskHandle hTaskToDelete = ( xTaskHandle )pxTaskToDelete;
pthread_t xTaskToDelete;
pthread_t xTaskToResume;
portBASE_TYPE xResult;

	if ( 0 == pthread_mutex_lock( &xSingleThreadMutex ) )
	{
		xTaskToDelete = prvGetThreadHandle( hTaskToDelete );
		xTaskToResume = prvGetThreadHandle( xTaskGetCurrentTaskHandle() );

		if ( xTaskToResume == xTaskToDelete )
		{
			/* This is a suicidal thread, need to select a different task to run. */
			vTaskSwitchContext();
			xTaskToResume = prvGetThreadHandle( xTaskGetCurrentTaskHandle() );
		}

		if ( pthread_self() != xTaskToDelete )
		{
			/* Cancelling a thread that is not me. */
			if ( xTaskToDelete != ( pthread_t )NULL )
			{
				/* Send a signal to wake the task so that it definitely cancels. */
				pthread_testcancel();
				xResult = pthread_cancel( xTaskToDelete );
                if (xResult)
                    printf("pthread_cancel error!\n");
				/* Pthread Clean-up function will note the cancellation. */
			}
			(void)pthread_mutex_unlock( &xSingleThreadMutex );
		}
		else
		{
			/* Resume the other thread. */
			prvResumeThread( xTaskToResume );
			/* Pthread Clean-up function will note the cancellation. */
			/* Release the execution. */
			uxCriticalNesting = 0;
			vPortEnableInterrupts();
			(void)pthread_mutex_unlock( &xSingleThreadMutex );
			/* Commit suicide */
			pthread_exit( (void *)1 );
		}
	}
}
/*-----------------------------------------------------------*/

void *prvWaitForStart( void * pvParams )
{
xParams * pxParams = ( xParams * )pvParams;
pdTASK_CODE pvCode = pxParams->pxCode;
void * pParams = pxParams->pvParams;
	vPortFree( pvParams );

	pthread_cleanup_push( prvDeleteThread, (void *)pthread_self() );

	if ( 0 == pthread_mutex_lock( &xSingleThreadMutex ) )
	{
		prvSuspendThread( pthread_self() );
	}

	pvCode( pParams );

	pthread_cleanup_pop( 1 );
	return (void *)NULL;
}
/*-----------------------------------------------------------*/

void prvSuspendSignalHandler(int sig)
{
sigset_t xSignals;

	/* Only interested in the resume signal. */
	sigemptyset( &xSignals );
	sigaddset( &xSignals, SIG_RESUME );
	xSentinel = 1;

	/* Unlock the Single thread mutex to allow the resumed task to continue. */
	if ( 0 != pthread_mutex_unlock( &xSingleThreadMutex ) )
	{
		printf( "Releasing someone else's lock.\n" );
	}

	/* Wait on the resume signal. */
	if ( 0 != sigwait( &xSignals, &sig ) )
	{
		printf( "SSH: Sw %d\n", sig );
	}

	/* Will resume here when the SIG_RESUME signal is received. */
	/* Need to set the interrupts based on the task's critical nesting. */
	if ( uxCriticalNesting == 0 )
	{
		vPortEnableInterrupts();
	}
	else
	{
		vPortDisableInterrupts();
	}
}
/*-----------------------------------------------------------*/

void prvSuspendThread( pthread_t xThreadId )
{
portBASE_TYPE xResult = pthread_mutex_lock( &xSuspendResumeThreadMutex );
	if ( 0 == xResult )
	{
		/* Set-up for the Suspend Signal handler? */
		xSentinel = 0;
		xResult = pthread_mutex_unlock( &xSuspendResumeThreadMutex );
		xResult = pthread_kill( xThreadId, SIG_SUSPEND );
        if (xResult)
            printf("pthread_kill error!\n");
		while ( ( xSentinel == 0 ) && ( pdTRUE != xServicingTick ) )
		{
			sched_yield();
		}
	}
}
/*-----------------------------------------------------------*/

void prvResumeSignalHandler(int sig)
{
    (void)(sig);
	/* Yield the Scheduler to ensure that the yielding thread completes. */
	if ( 0 == pthread_mutex_lock( &xSingleThreadMutex ) )
	{
		(void)pthread_mutex_unlock( &xSingleThreadMutex );
	}
}
/*-----------------------------------------------------------*/

void prvResumeThread( pthread_t xThreadId )
{
portBASE_TYPE xResult;
	if ( 0 == pthread_mutex_lock( &xSuspendResumeThreadMutex ) )
	{
		if ( pthread_self() != xThreadId )
		{
			xResult = pthread_kill( xThreadId, SIG_RESUME );
            if (xResult)
                printf("pthread_kill error!\n");
		}
		xResult = pthread_mutex_unlock( &xSuspendResumeThreadMutex );
        if (xResult)
            printf("pthread_mutex_unlock error!\n");
	}
}
/*-----------------------------------------------------------*/

void prvSetupSignalsAndSchedulerPolicy( void )
{
/* The following code would allow for configuring the scheduling of this task as a Real-time task.
 * The process would then need to be run with higher privileges for it to take affect.
int iPolicy;
int iResult;
int iSchedulerPriority;
	iResult = pthread_getschedparam( pthread_self(), &iPolicy, &iSchedulerPriority );
	iResult = pthread_attr_setschedpolicy( &xThreadAttributes, SCHED_FIFO );
	iPolicy = SCHED_FIFO;
	iResult = pthread_setschedparam( pthread_self(), iPolicy, &iSchedulerPriority );		*/

struct sigaction sigsuspendself, sigresume, sigtick;
portLONG lIndex;

	pxThreads = ( xThreadState *)pvPortMalloc( sizeof( xThreadState ) * MAX_NUMBER_OF_TASKS );
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		pxThreads[ lIndex ].hThread = ( pthread_t )NULL;
		pxThreads[ lIndex ].hTask = ( xTaskHandle )NULL;
		pxThreads[ lIndex ].uxCriticalNesting = 0;
	}

	sigsuspendself.sa_flags = 0;
	sigsuspendself.sa_handler = prvSuspendSignalHandler;
	sigfillset( &sigsuspendself.sa_mask );

	sigresume.sa_flags = 0;
	sigresume.sa_handler = prvResumeSignalHandler;
	sigfillset( &sigresume.sa_mask );

	sigtick.sa_flags = 0;
	sigtick.sa_handler = vPortSystemTickHandler;
	sigfillset( &sigtick.sa_mask );

	if ( 0 != sigaction( SIG_SUSPEND, &sigsuspendself, NULL ) )
	{
		printf( "Problem installing SIG_SUSPEND_SELF\n" );
	}
	if ( 0 != sigaction( SIG_RESUME, &sigresume, NULL ) )
	{
		printf( "Problem installing SIG_RESUME\n" );
	}
	if ( 0 != sigaction( SIG_TICK, &sigtick, NULL ) )
	{
		printf( "Problem installing SIG_TICK\n" );
	}
	printf( "Running as PID: %d\n", getpid() );
}
/*-----------------------------------------------------------*/

pthread_t prvGetThreadHandle( xTaskHandle hTask )
{
pthread_t hThread = ( pthread_t )NULL;
portLONG lIndex;
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		if ( pxThreads[ lIndex ].hTask == hTask )
		{
			hThread = pxThreads[ lIndex ].hThread;
			break;
		}
	}
	return hThread;
}
/*-----------------------------------------------------------*/

portLONG prvGetFreeThreadState( void )
{
portLONG lIndex;
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		if ( pxThreads[ lIndex ].hThread == ( pthread_t )NULL )
		{
			break;
		}
	}

	if ( MAX_NUMBER_OF_TASKS == lIndex )
	{
		printf( "No more free threads, please increase the maximum.\n" );
		lIndex = 0;
		vPortEndScheduler();
	}

	return lIndex;
}
/*-----------------------------------------------------------*/

void prvSetTaskCriticalNesting( pthread_t xThreadId, unsigned portBASE_TYPE uxNesting )
{
portLONG lIndex;
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		if ( pxThreads[ lIndex ].hThread == xThreadId )
		{
			pxThreads[ lIndex ].uxCriticalNesting = uxNesting;
			break;
		}
	}
}
/*-----------------------------------------------------------*/

unsigned portBASE_TYPE prvGetTaskCriticalNesting( pthread_t xThreadId )
{
unsigned portBASE_TYPE uxNesting = 0;
portLONG lIndex;
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		if ( pxThreads[ lIndex ].hThread == xThreadId )
		{
			uxNesting = pxThreads[ lIndex ].uxCriticalNesting;
			break;
		}
	}
	return uxNesting;
}
/*-----------------------------------------------------------*/

void prvDeleteThread( void *xThreadId )
{
portLONG lIndex;
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		if ( pxThreads[ lIndex ].hThread == ( pthread_t )xThreadId )
		{
			pxThreads[ lIndex ].hThread = (pthread_t)NULL;
			pxThreads[ lIndex ].hTask = (xTaskHandle)NULL;
			if ( pxThreads[ lIndex ].uxCriticalNesting > 0 )
			{
				uxCriticalNesting = 0;
				vPortEnableInterrupts();
			}
			pxThreads[ lIndex ].uxCriticalNesting = 0;
			break;
		}
	}
}
/*-----------------------------------------------------------*/

void vPortAddTaskHandle( void *pxTaskHandle )
{
portLONG lIndex;

	pxThreads[ lIndexOfLastAddedTask ].hTask = ( xTaskHandle )pxTaskHandle;
	for ( lIndex = 0; lIndex < MAX_NUMBER_OF_TASKS; lIndex++ )
	{
		if ( pxThreads[ lIndex ].hThread == pxThreads[ lIndexOfLastAddedTask ].hThread )
		{
			if ( pxThreads[ lIndex ].hTask != pxThreads[ lIndexOfLastAddedTask ].hTask )
			{
				pxThreads[ lIndex ].hThread = ( pthread_t )NULL;
				pxThreads[ lIndex ].hTask = NULL;
				pxThreads[ lIndex ].uxCriticalNesting = 0;
			}
		}
	}
}
/*-----------------------------------------------------------*/

void vPortFindTicksPerSecond( void )
{
	/* Needs to be reasonably high for accuracy. */
	unsigned long ulTicksPerSecond = sysconf(_SC_CLK_TCK);
	printf( "Timer Resolution for Run TimeStats is %ld ticks per second.\n", ulTicksPerSecond );
}
/*-----------------------------------------------------------*/

unsigned long ulPortGetTimerValue( void )
{
struct tms xTimes;
	unsigned long ulTotalTime = times( &xTimes );
	/* Return the application code times.
	 * The timer only increases when the application code is actually running
	 * which means that the total execution times should add up to 100%.
	 */
	return ( unsigned long ) xTimes.tms_utime;

	/* Should check ulTotalTime for being clock_t max minus 1. */
	(void)ulTotalTime;
}
/*-----------------------------------------------------------*/
