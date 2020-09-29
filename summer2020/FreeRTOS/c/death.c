/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/**
 * Create a single persistent task which periodically dynamically creates another
 * two tasks.  The original task is called the creator task, the two tasks it
 * creates are called suicidal tasks.
 *
 * One of the created suicidal tasks kill one other suicidal task before killing
 * itself - leaving just the original task remaining.
 *
 * The creator task must be spawned after all of the other demo application tasks
 * as it keeps a check on the number of tasks under the scheduler control.  The
 * number of tasks it expects to see running should never be greater than the
 * number of tasks that were in existence when the creator task was spawned, plus
 * one set of four suicidal tasks.  If this number is exceeded an error is flagged.
 *
 * \page DeathC death.c
 * \ingroup DemoFiles
 * <HR>
 */


#include <stdlib.h>

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo program include files. */
#include "death.h"

#define deathSTACK_SIZE		( configMINIMAL_STACK_SIZE + 60 )

/* The task originally created which is responsible for periodically dynamically
creating another four tasks. */
static portTASK_FUNCTION_PROTO( vCreateTasks, pvParameters );

/* The task function of the dynamically created tasks. */
static portTASK_FUNCTION_PROTO( vSuicidalTask, pvParameters );

/* A variable which is incremented every time the dynamic tasks are created.  This
is used to check that the task is still running. */
static volatile uint16_t usCreationCount = 0;

/* Used to store the number of tasks that were originally running so the creator
task can tell if any of the suicidal tasks have failed to die.
*/
static volatile UBaseType_t uxTasksRunningAtStart = 0;

/* When a task deletes itself, it stack and TCB are cleaned up by the Idle task.
Under heavy load the idle task might not get much processing time, so it would
be legitimate for several tasks to remain undeleted for a short period.  There
may also be a few other unexpected tasks if, for example, the tasks that test
static allocation are also being used. */
static const UBaseType_t uxMaxNumberOfExtraTasksRunning = 3;

/* Used to store a handle to the task that should be killed by a suicidal task,
before it kills itself. */
TaskHandle_t xCreatedTask;

/*-----------------------------------------------------------*/

void vCreateSuicidalTasks( UBaseType_t uxPriority )
{
	xTaskCreate( vCreateTasks, "CREATOR", deathSTACK_SIZE, ( void * ) NULL, uxPriority, NULL );

	/* Record the number of tasks that are running now so we know if any of the
	suicidal tasks have failed to be killed. */
	uxTasksRunningAtStart = ( UBaseType_t ) uxTaskGetNumberOfTasks();

	/* FreeRTOS.org versions before V3.0 started the idle-task as the very
	first task. The idle task was then already included in uxTasksRunningAtStart.
	From FreeRTOS V3.0 on, the idle task is started when the scheduler is
	started. Therefore the idle task is not yet accounted for. We correct
	this by increasing uxTasksRunningAtStart by 1. */
	uxTasksRunningAtStart++;

	/* From FreeRTOS version 7.0.0 can optionally create a timer service task.
	If this is done, then uxTasksRunningAtStart needs incrementing again as that
	too is created when the scheduler is started. */
	#if configUSE_TIMERS == 1
	{
		uxTasksRunningAtStart++;
	}
	#endif
}
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vSuicidalTask, pvParameters )
{
volatile long l1, l2;
TaskHandle_t xTaskToKill;
const TickType_t xDelay = pdMS_TO_TICKS( ( TickType_t ) 200 );

	if( pvParameters != NULL )
	{
		/* This task is periodically created four times.  Two created tasks are
		passed a handle to the other task so it can kill it before killing itself.
		The other task is passed in null. */
		xTaskToKill = *( TaskHandle_t* )pvParameters;
	}
	else
	{
		xTaskToKill = NULL;
	}

	for( ;; )
	{
		/* Do something random just to use some stack and registers. */
		l1 = 2;
		l2 = 89;
		l2 *= l1;
		vTaskDelay( xDelay );

		if( xTaskToKill != NULL )
		{
			/* Make sure the other task has a go before we delete it. */
			vTaskDelay( ( TickType_t ) 0 );

			/* Kill the other task that was created by vCreateTasks(). */
			vTaskDelete( xTaskToKill );

			/* Kill ourselves. */
			vTaskDelete( NULL );
		}
	}
}/*lint !e818 !e550 Function prototype must be as per standard for task functions. */
/*-----------------------------------------------------------*/

static portTASK_FUNCTION( vCreateTasks, pvParameters )
{
const TickType_t xDelay = pdMS_TO_TICKS( ( TickType_t ) 1000 );
UBaseType_t uxPriority;

	/* Remove compiler warning about unused parameter. */
	( void ) pvParameters;

	uxPriority = uxTaskPriorityGet( NULL );

	for( ;; )
	{
		/* Just loop round, delaying then creating the four suicidal tasks. */
		vTaskDelay( xDelay );

		xCreatedTask = NULL;

		xTaskCreate( vSuicidalTask, "SUICID1", configMINIMAL_STACK_SIZE, NULL, uxPriority, &xCreatedTask );
		xTaskCreate( vSuicidalTask, "SUICID2", configMINIMAL_STACK_SIZE, &xCreatedTask, uxPriority, NULL );

		++usCreationCount;
	}
}
/*-----------------------------------------------------------*/

/* This is called to check that the creator task is still running and that there
are not any more than four extra tasks. */
BaseType_t xIsCreateTaskStillRunning( void )
{
static uint16_t usLastCreationCount = 0xfff;
BaseType_t xReturn = pdTRUE;
static UBaseType_t uxTasksRunningNow;

	if( usLastCreationCount == usCreationCount )
	{
		xReturn = pdFALSE;
	}
	else
	{
		usLastCreationCount = usCreationCount;
	}

	uxTasksRunningNow = ( UBaseType_t ) uxTaskGetNumberOfTasks();

	if( uxTasksRunningNow < uxTasksRunningAtStart )
	{
		xReturn = pdFALSE;
	}
	else if( ( uxTasksRunningNow - uxTasksRunningAtStart ) > uxMaxNumberOfExtraTasksRunning )
	{
		xReturn = pdFALSE;
	}
	else
	{
		/* Everything is okay. */
	}

	return xReturn;
}


