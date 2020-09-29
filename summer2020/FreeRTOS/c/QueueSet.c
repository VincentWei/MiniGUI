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

/*
 * Tests the use of queue sets.
 *
 * A receive task creates a number of queues and adds them to a queue set before
 * blocking on the queue set receive.  A transmit task and (optionally) an
 * interrupt repeatedly unblocks the receive task by sending messages to the
 * queues in a pseudo random order.  The receive task removes the messages from
 * the queues and flags an error if the received message does not match that
 * expected.  The task sends values in the range 0 to
 * queuesetINITIAL_ISR_TX_VALUE, and the ISR sends value in the range
 * queuesetINITIAL_ISR_TX_VALUE to ULONG_MAX.
 */


/* Standard includes. */
#include <stdlib.h>
#include <limits.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo includes. */
#include "QueueSet.h"

/* The number of queues that are created and added to the queue set. */
#define queuesetNUM_QUEUES_IN_SET 3

/* The length of each created queue. */
#define queuesetQUEUE_LENGTH	3

/* Block times used in this demo.  A block time or 0 means "don't block". */
#define queuesetSHORT_DELAY	200
#define queuesetDONT_BLOCK 0

/* Messages are sent in incrementing order from both a task and an interrupt.
The task sends values in the range 0 to 0xfffe, and the interrupt sends values
in the range of 0xffff to ULONG_MAX. */
#define queuesetINITIAL_ISR_TX_VALUE 0xffffUL

/* The priorities used in this demo. */
#define queuesetLOW_PRIORITY	( tskIDLE_PRIORITY )
#define queuesetMEDIUM_PRIORITY ( queuesetLOW_PRIORITY + 1 )

/* For test purposes the priority of the sending task is changed after every
queuesetPRIORITY_CHANGE_LOOPS number of values are sent to a queue. */
#define queuesetPRIORITY_CHANGE_LOOPS	( ( queuesetNUM_QUEUES_IN_SET * queuesetQUEUE_LENGTH ) * 2 )

/* The ISR sends to the queue every queuesetISR_TX_PERIOD ticks. */
#define queuesetISR_TX_PERIOD	( 100UL )

/* A delay inserted when the Tx task changes its priority to be above the idle
task priority to ensure the idle priority tasks get some CPU time before the
next iteration of the queue set Tx task. */
#define queuesetTX_LOOP_DELAY	pdMS_TO_TICKS( ( TickType_t ) 200 )

/* The allowable maximum deviation between a received value and the expected
received value.  A deviation will occur when data is received from a queue
inside an ISR in between a task receiving from a queue and the task checking
the received value. */
#define queuesetALLOWABLE_RX_DEVIATION 3

/* Ignore values that are at the boundaries of allowable values to make the
testing of limits easier (don't have to deal with wrapping values). */
#define queuesetIGNORED_BOUNDARY	( queuesetALLOWABLE_RX_DEVIATION * 2 )

typedef enum
{
	eEqualPriority = 0,	/* Tx and Rx tasks have the same priority. */
	eTxHigherPriority,	/* The priority of the Tx task is above that of the Rx task. */
	eTxLowerPriority	/* The priority of the Tx task is below that of the Rx task. */
} eRelativePriorities;

/*
 * The task that periodically sends to the queue set.
 */
static void prvQueueSetSendingTask( void *pvParameters );

/*
 * The task that reads from the queue set.
 */
static void prvQueueSetReceivingTask( void *pvParameters );

/*
 * Check the value received from a queue is the expected value.  Some values
 * originate from the send task, some values originate from the ISR, with the
 * range of the value being used to distinguish between the two message
 * sources.
 */
static void prvCheckReceivedValue( uint32_t ulReceived );

/*
 * For purposes of test coverage, functions that read from and write to a
 * queue set from an ISR respectively.
 */
static void prvReceiveFromQueueInSetFromISR( void );
static void prvSendToQueueInSetFromISR( void );

/*
 * Create the queues and add them to a queue set before resuming the Tx
 * task.
 */
static void prvSetupTest( void );

/*
 * Checks a value received from a queue falls within the range of expected
 * values.
 */
static BaseType_t prvCheckReceivedValueWithinExpectedRange( uint32_t ulReceived, uint32_t ulExpectedReceived );

/*
 * Increase test coverage by occasionally change the priorities of the two tasks
 * relative to each other. */
static void prvChangeRelativePriorities( void );

/*
 * Local pseudo random number seed and return functions.  Used to avoid calls
 * to the standard library.
 */
static size_t prvRand( void );
static void prvSRand( size_t uxSeed );

/*-----------------------------------------------------------*/

/* The queues that are added to the set. */
static QueueHandle_t xQueues[ queuesetNUM_QUEUES_IN_SET ] = { 0 };

/* Counts how many times each queue in the set is used to ensure all the
queues are used. */
static uint32_t ulQueueUsedCounter[ queuesetNUM_QUEUES_IN_SET ] = { 0 };

/* The handle of the queue set to which the queues are added. */
static QueueSetHandle_t xQueueSet;

/* If the prvQueueSetReceivingTask() task has not detected any errors then
it increments ulCycleCounter on each iteration.
xAreQueueSetTasksStillRunning() returns pdPASS if the value of
ulCycleCounter has changed between consecutive calls, and pdFALSE if
ulCycleCounter has stopped incrementing (indicating an error condition). */
static volatile uint32_t ulCycleCounter = 0UL;

/* Set to pdFAIL if an error is detected by any queue set task.
ulCycleCounter will only be incremented if xQueueSetTasksSatus equals pdPASS. */
static volatile BaseType_t xQueueSetTasksStatus = pdPASS;

/* Just a flag to let the function that writes to a queue from an ISR know that
the queues are setup and can be used. */
static volatile BaseType_t xSetupComplete = pdFALSE;

/* The value sent to the queue from the ISR is file scope so the
xAreQueeuSetTasksStillRunning() function can check it is incrementing as
expected. */
static volatile uint32_t ulISRTxValue = queuesetINITIAL_ISR_TX_VALUE;

/* Used by the pseudo random number generator. */
static size_t uxNextRand = 0;

/* The task handles are stored so their priorities can be changed. */
TaskHandle_t xQueueSetSendingTask, xQueueSetReceivingTask;

/*-----------------------------------------------------------*/

void vStartQueueSetTasks( void )
{
	/* Create the tasks. */
	xTaskCreate( prvQueueSetSendingTask, "SetTx", configMINIMAL_STACK_SIZE, NULL, queuesetMEDIUM_PRIORITY, &xQueueSetSendingTask );

	if( xQueueSetSendingTask != NULL )
	{
		xTaskCreate( prvQueueSetReceivingTask, "SetRx", configMINIMAL_STACK_SIZE, ( void * ) xQueueSetSendingTask, queuesetMEDIUM_PRIORITY, &xQueueSetReceivingTask );

		/* It is important that the sending task does not attempt to write to a
		queue before the queue has been created.  It is therefore placed into
		the suspended state before the scheduler has started.  It is resumed by
		the receiving task after the receiving task has created the queues and
		added the queues to the queue set. */
		vTaskSuspend( xQueueSetSendingTask );
	}
}
/*-----------------------------------------------------------*/

BaseType_t xAreQueueSetTasksStillRunning( void )
{
static uint32_t ulLastCycleCounter, ulLastISRTxValue = 0;
static uint32_t ulLastQueueUsedCounter[ queuesetNUM_QUEUES_IN_SET ] = { 0 };
BaseType_t xReturn = pdPASS, x;

	if( ulLastCycleCounter == ulCycleCounter )
	{
		/* The cycle counter is no longer being incremented.  Either one of the
		tasks is stalled or an error has been detected. */
		xReturn = pdFAIL;
	}

	ulLastCycleCounter = ulCycleCounter;

	/* Ensure that all the queues in the set have been used.  This ensures the
	test is working as intended and guards against the rand() in the Tx task
	missing some values. */
	for( x = 0; x < queuesetNUM_QUEUES_IN_SET; x++ )
	{
		if( ulLastQueueUsedCounter[ x ] == ulQueueUsedCounter[ x ] )
		{
			xReturn = pdFAIL;
		}

		ulLastQueueUsedCounter[ x ] = ulQueueUsedCounter[ x ];
	}

	/* Check the global status flag. */
	if( xQueueSetTasksStatus != pdPASS )
	{
		xReturn = pdFAIL;
	}

	/* Check that the ISR is still sending values to the queues too. */
	if( ulISRTxValue == ulLastISRTxValue )
	{
		xReturn = pdFAIL;
	}
	else
	{
		ulLastISRTxValue = ulISRTxValue;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static void prvQueueSetSendingTask( void *pvParameters )
{
uint32_t ulTaskTxValue = 0;
size_t uxQueueToWriteTo;
QueueHandle_t xQueueInUse;

	/* Remove compiler warning about the unused parameter. */
	( void ) pvParameters;

	/* Seed mini pseudo random number generator. */
	prvSRand( ( size_t ) &ulTaskTxValue );

	for( ;; )
	{
		/* Generate the index for the queue to which a value is to be sent. */
		uxQueueToWriteTo = prvRand() % queuesetNUM_QUEUES_IN_SET;
		xQueueInUse = xQueues[ uxQueueToWriteTo ];

		/* Note which index is being written to to ensure all the queues are
		used. */
		( ulQueueUsedCounter[ uxQueueToWriteTo ] )++;

		/* Send to the queue to unblock the task that is waiting for data to
		arrive on a queue within the queue set to which this queue belongs. */
		if( xQueueSendToBack( xQueueInUse, &ulTaskTxValue, portMAX_DELAY ) != pdPASS )
		{
			/* The send should always pass as an infinite block time was
			used. */
			xQueueSetTasksStatus = pdFAIL;
		}

		#if( configUSE_PREEMPTION == 0 )
			taskYIELD();
		#endif

		ulTaskTxValue++;

		/* If the Tx value has reached the range used by the ISR then set it
		back to 0. */
		if( ulTaskTxValue == queuesetINITIAL_ISR_TX_VALUE )
		{
			ulTaskTxValue = 0;
		}

		/* Increase test coverage by occasionally change the priorities of the
		two tasks relative to each other. */
		prvChangeRelativePriorities();
	}
}
/*-----------------------------------------------------------*/

static void prvChangeRelativePriorities( void )
{
static UBaseType_t ulLoops = 0;
static eRelativePriorities ePriorities = eEqualPriority;

	/* Occasionally change the task priority relative to the priority of
	the receiving task. */
	ulLoops++;
	if( ulLoops >= queuesetPRIORITY_CHANGE_LOOPS )
	{
		ulLoops = 0;

		switch( ePriorities )
		{
			case eEqualPriority:
				/* Both tasks are running with medium priority.  Now lower the
				priority of the receiving task so the Tx task has the higher
				relative priority. */
				vTaskPrioritySet( xQueueSetReceivingTask, queuesetLOW_PRIORITY );
				ePriorities = eTxHigherPriority;
				break;

			case eTxHigherPriority:
				/* The Tx task is running with a higher priority than the Rx
				task.  Switch the priorities around so the Rx task has the
				higher relative priority. */
				vTaskPrioritySet( xQueueSetReceivingTask, queuesetMEDIUM_PRIORITY );
				vTaskPrioritySet( xQueueSetSendingTask, queuesetLOW_PRIORITY );
				ePriorities = eTxLowerPriority;
				break;

			case eTxLowerPriority:
				/* The Tx task is running with a lower priority than the Rx
				task.  Make the priorities equal again. */
				vTaskPrioritySet( xQueueSetSendingTask, queuesetMEDIUM_PRIORITY );
				ePriorities = eEqualPriority;

				/* When both tasks are using a non-idle priority the queue set
				tasks will starve idle priority tasks of execution time - so
				relax a bit before the next iteration to minimise the impact. */
				vTaskDelay( queuesetTX_LOOP_DELAY );

				break;
		}
	}
}
/*-----------------------------------------------------------*/

static void prvQueueSetReceivingTask( void *pvParameters )
{
uint32_t ulReceived;
QueueHandle_t xActivatedQueue;
TickType_t xBlockTime;

	/* Remove compiler warnings. */
	( void ) pvParameters;

	/* Create the queues and add them to the queue set before resuming the Tx
	task. */
	prvSetupTest();

	for( ;; )
	{
		/* For test coverage reasons, the block time is dependent on the
		priority of this task - which changes during the test.  When the task
		is at the idle priority it polls the queue set. */
		if( uxTaskPriorityGet( NULL ) == tskIDLE_PRIORITY )
		{
			xBlockTime = 0;
		}
		else
		{
			xBlockTime = portMAX_DELAY;
		}

		/* Wait for a message to arrive on one of the queues in the set. */
		xActivatedQueue = xQueueSelectFromSet( xQueueSet, portMAX_DELAY );

		if( xActivatedQueue == NULL )
		{
			if( xBlockTime != 0 )
			{
				/* This should not happen as an infinite delay was used. */
				xQueueSetTasksStatus = pdFAIL;
			}
		}
		else
		{
			/* Reading from the queue should pass with a zero block time as
			this task will only run when something has been posted to a task
			in the queue set. */
			if( xQueueReceive( xActivatedQueue, &ulReceived, queuesetDONT_BLOCK ) != pdPASS )
			{
				xQueueSetTasksStatus = pdFAIL;
			}

			/* Ensure the value received was the value expected.  This function
			manipulates file scope data and is also called from an ISR, hence
			the critical section. */
			taskENTER_CRITICAL();
			{
				prvCheckReceivedValue( ulReceived );
			}
			taskEXIT_CRITICAL();

			if( xQueueSetTasksStatus == pdPASS )
			{
				ulCycleCounter++;
			}
		}
	}
}
/*-----------------------------------------------------------*/

void vQueueSetAccessQueueSetFromISR( void )
{
static uint32_t ulCallCount = 0;

	/* xSetupComplete is set to pdTRUE when the queues have been created and
	are available for use. */
	if( xSetupComplete == pdTRUE )
	{
		/* It is intended that this function is called from the tick hook
		function, so each call is one tick period apart. */
		ulCallCount++;
		if( ulCallCount > queuesetISR_TX_PERIOD )
		{
			ulCallCount = 0;

			/* First attempt to read from the queue set. */
			prvReceiveFromQueueInSetFromISR();

			/* Then write to the queue set. */
			prvSendToQueueInSetFromISR();
		}
	}
}
/*-----------------------------------------------------------*/

static void prvCheckReceivedValue( uint32_t ulReceived )
{
static uint32_t ulExpectedReceivedFromTask = 0, ulExpectedReceivedFromISR = queuesetINITIAL_ISR_TX_VALUE;

	/* Values are received in tasks and interrupts.  It is likely that the
	receiving task will sometimes get preempted by the receiving interrupt
	between reading a value from the queue and calling this function.  When
	that happens, if the receiving interrupt calls this function the values
	will get passed into this function slightly out of order.  For that
	reason the value passed in is tested against a small range of expected
	values, rather than a single absolute value.  To make the range testing
	easier values in the range limits are ignored. */

	/* If the received value is equal to or greater than
	queuesetINITIAL_ISR_TX_VALUE then it was sent by an ISR. */
	if( ulReceived >= queuesetINITIAL_ISR_TX_VALUE )
	{
		/* The value was sent from the ISR. */
		if( ( ulReceived - queuesetINITIAL_ISR_TX_VALUE ) < queuesetIGNORED_BOUNDARY )
		{
			/* The value received is at the lower limit of the expected range.
			Don't test it and expect to receive one higher next time. */
		}
		else if( ( ULONG_MAX - ulReceived ) <= queuesetIGNORED_BOUNDARY )
		{
			/* The value received is at the higher limit of the expected range.
			Don't test it and expect to wrap soon. */
		}
		else
		{
			/* Check the value against its expected value range. */
			if( prvCheckReceivedValueWithinExpectedRange( ulReceived, ulExpectedReceivedFromISR ) != pdPASS )
			{
				xQueueSetTasksStatus = pdFAIL;
			}
		}

		configASSERT( xQueueSetTasksStatus );

		/* It is expected to receive an incrementing number. */
		ulExpectedReceivedFromISR++;
		if( ulExpectedReceivedFromISR == 0 )
		{
			ulExpectedReceivedFromISR = queuesetINITIAL_ISR_TX_VALUE;
		}
	}
	else
	{
		/* The value was sent from the Tx task. */
		if( ulReceived < queuesetIGNORED_BOUNDARY )
		{
			/* The value received is at the lower limit of the expected range.
			Don't test it, and expect to receive one higher next time. */
		}
		else if( ( ( queuesetINITIAL_ISR_TX_VALUE - 1 ) - ulReceived ) <= queuesetIGNORED_BOUNDARY )
		{
			/* The value received is at the higher limit of the expected range.
			Don't test it and expect to wrap soon. */
		}
		else
		{
			/* Check the value against its expected value range. */
			if( prvCheckReceivedValueWithinExpectedRange( ulReceived, ulExpectedReceivedFromTask ) != pdPASS )
			{
				xQueueSetTasksStatus = pdFAIL;
			}
		}

		configASSERT( xQueueSetTasksStatus );

		/* It is expected to receive an incrementing number. */
		ulExpectedReceivedFromTask++;
		if( ulExpectedReceivedFromTask >= queuesetINITIAL_ISR_TX_VALUE )
		{
			ulExpectedReceivedFromTask = 0;
		}
	}
}
/*-----------------------------------------------------------*/

static BaseType_t prvCheckReceivedValueWithinExpectedRange( uint32_t ulReceived, uint32_t ulExpectedReceived )
{
BaseType_t xReturn = pdPASS;

	if( ulReceived > ulExpectedReceived )
	{
		configASSERT( ( ulReceived - ulExpectedReceived ) <= queuesetALLOWABLE_RX_DEVIATION );
		if( ( ulReceived - ulExpectedReceived ) > queuesetALLOWABLE_RX_DEVIATION )
		{
			xReturn = pdFALSE;
		}
	}
	else
	{
		configASSERT( ( ulExpectedReceived - ulReceived ) <= queuesetALLOWABLE_RX_DEVIATION );
		if( ( ulExpectedReceived - ulReceived ) > queuesetALLOWABLE_RX_DEVIATION )
		{
			xReturn = pdFALSE;
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static void prvReceiveFromQueueInSetFromISR( void )
{
QueueSetMemberHandle_t xActivatedQueue;
uint32_t ulReceived;

	/* See if any of the queues in the set contain data. */
	xActivatedQueue = xQueueSelectFromSetFromISR( xQueueSet );

	if( xActivatedQueue != NULL )
	{
		/* Reading from the queue for test purposes only. */
		if( xQueueReceiveFromISR( xActivatedQueue, &ulReceived, NULL ) != pdPASS )
		{
			/* Data should have been available as the handle was returned from
			xQueueSelectFromSetFromISR(). */
			xQueueSetTasksStatus = pdFAIL;
		}

		/* Ensure the value received was the value expected. */
		prvCheckReceivedValue( ulReceived );
	}
}
/*-----------------------------------------------------------*/

static void prvSendToQueueInSetFromISR( void )
{
static BaseType_t xQueueToWriteTo = 0;

	if( xQueueSendFromISR( xQueues[ xQueueToWriteTo ], ( void * ) &ulISRTxValue, NULL ) == pdPASS )
	{
		ulISRTxValue++;

		/* If the Tx value has wrapped then set it back to its initial value. */
		if( ulISRTxValue == 0UL )
		{
			ulISRTxValue = queuesetINITIAL_ISR_TX_VALUE;
		}

		/* Use a different queue next time. */
		xQueueToWriteTo++;
		if( xQueueToWriteTo >= queuesetNUM_QUEUES_IN_SET )
		{
			xQueueToWriteTo = 0;
		}
	}
}
/*-----------------------------------------------------------*/

static void prvSetupTest( void )
{
BaseType_t x;
uint32_t ulValueToSend = 0;

	/* Ensure the queues are created and the queue set configured before the
	sending task is unsuspended.

	First Create the queue set such that it will be able to hold a message for
	every space in every queue in the set. */
	xQueueSet = xQueueCreateSet( queuesetNUM_QUEUES_IN_SET * queuesetQUEUE_LENGTH );

	for( x = 0; x < queuesetNUM_QUEUES_IN_SET; x++ )
	{
		/* Create the queue and add it to the set.  The queue is just holding
		uint32_t value. */
		xQueues[ x ] = xQueueCreate( queuesetQUEUE_LENGTH, sizeof( uint32_t ) );
		configASSERT( xQueues[ x ] );
		if( xQueueAddToSet( xQueues[ x ], xQueueSet ) != pdPASS )
		{
			xQueueSetTasksStatus = pdFAIL;
		}
		else
		{
			/* The queue has now been added to the queue set and cannot be added to
			another. */
			if( xQueueAddToSet( xQueues[ x ], xQueueSet ) != pdFAIL )
			{
				xQueueSetTasksStatus = pdFAIL;
			}
		}
	}

	/* Attempt to remove a queue from a queue set it does not belong
	to (NULL being passed as the queue set in this case). */
	if( xQueueRemoveFromSet( xQueues[ 0 ], NULL ) != pdFAIL )
	{
		/* It is not possible to successfully remove a queue from a queue
		set it does not belong to. */
		xQueueSetTasksStatus = pdFAIL;
	}

	/* Attempt to remove a queue from the queue set it does belong to. */
	if( xQueueRemoveFromSet( xQueues[ 0 ], xQueueSet ) != pdPASS )
	{
		/* It should be possible to remove the queue from the queue set it
		does belong to. */
		xQueueSetTasksStatus = pdFAIL;
	}

	/* Add an item to the queue before attempting to add it back into the
	set. */
	xQueueSend( xQueues[ 0 ], ( void * ) &ulValueToSend, 0 );
	if( xQueueAddToSet( xQueues[ 0 ], xQueueSet ) != pdFAIL )
	{
		/* Should not be able to add a non-empty queue to a set. */
		xQueueSetTasksStatus = pdFAIL;
	}

	/* Remove the item from the queue before adding the queue back into the
	set so the dynamic tests can begin. */
	xQueueReceive( xQueues[ 0 ], &ulValueToSend, 0 );
	if( xQueueAddToSet( xQueues[ 0 ], xQueueSet ) != pdPASS )
	{
		/* If the queue was successfully removed from the queue set then it
		should be possible to add it back in again. */
		xQueueSetTasksStatus = pdFAIL;
	}

	/* The task that sends to the queues is not running yet, so attempting to
	read from the queue set should fail. */
	if( xQueueSelectFromSet( xQueueSet, queuesetSHORT_DELAY ) != NULL )
	{
		xQueueSetTasksStatus = pdFAIL;
	}

	/* Resume the task that writes to the queues. */
	vTaskResume( xQueueSetSendingTask );

	/* Let the ISR access the queues also. */
	xSetupComplete = pdTRUE;
}
/*-----------------------------------------------------------*/

static size_t prvRand( void )
{
	uxNextRand = ( uxNextRand * ( size_t ) 1103515245 ) + ( size_t ) 12345;
	return ( uxNextRand / ( size_t ) 65536 ) % ( size_t ) 32768;
}
/*-----------------------------------------------------------*/

static void prvSRand( size_t uxSeed )
{
	uxNextRand = uxSeed;
}

