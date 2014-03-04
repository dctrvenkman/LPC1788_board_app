/*
    FreeRTOS V8.0.0 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>! NOTE: The modification to the GPL is included to allow you to distribute
    >>! a combined work that includes FreeRTOS without being obliged to provide
    >>! the source code for proprietary components outside of the FreeRTOS
    >>! kernel.

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/


/*
 * "USB" task - Enumerates the USB device as a CDC class, then echoes back all
 * received characters with a configurable offset (for example, if the offset
 * is 1 and 'A' is received then 'B' will be sent back).  A dumb terminal such
 * as Hyperterminal can be used to talk to the USB task.
 */

/* Standard includes. */
#include "stdio.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Red Suite includes. */
#include "lcd_driver.h"
#include "lcd.h"

/* Bit definitions. */
#define PCONP_PCGPIO    0x00008000
#define PLLFEED_FEED1   0x000000AA
#define PLLFEED_FEED2   0x00000055
/*-----------------------------------------------------------*/

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware(void);

/*
 * The task that handles the USB stack.
 */
extern void vUSBTask(void *pvParameters);

/*-----------------------------------------------------------*/

int main(void)
{
	/* Configure the hardware for use by this demo. */
	prvSetupHardware();

    /* Create the USB task. */
    xTaskCreate(vUSBTask, "USB", configMINIMAL_STACK_SIZE + 0x100, (void *) NULL, tskIDLE_PRIORITY, NULL);

	LCDdriver_initialisation();
	LCD_PrintString(5, 10, "FreeRTOS.org", 14, COLOR_GREEN);

    /* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task.  The idle task is created within vTaskStartScheduler(). */
	while(1)
		;
}

void vApplicationTickHook( void )
{
	/* Called from every tick interrupt as described in the comments at the top
	of this file. */
}

void prvSetupHardware( void )
{
	/* Disable peripherals power. */
	LPC_SYSCTL->PCONP = 0;

	/* Enable GPIO power. */
	LPC_SYSCTL->PCONP = PCONP_PCGPIO;

	/* Disable TPIU. */
	LPC_IOCON->PINSEL[10] = 0;

	if(LPC_SYSCTL->PLL[0].PLLSTAT & (1 << 25))
	{
		/* Enable PLL, disconnected. */
		LPC_SYSCTL->PLL[0].PLLCON = 1;
		LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED1;
		LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED2;
	}

	/* Disable PLL, disconnected. */
	LPC_SYSCTL->PLL[0].PLLCON = 0;
	LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED1;
	LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED2;

	/* Enable main OSC. */
	LPC_SYSCTL->SCS |= 0x20;
	while(!(LPC_SYSCTL->SCS & 0x40))
		;

	/* select main OSC, 12MHz, as the PLL clock source. */
	LPC_SYSCTL->CLKSRCSEL = 0x1;

	LPC_SYSCTL->PLL[0].PLLCFG = 0x20031;
	LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED1;
	LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED2;

	/* Enable PLL, disconnected. */
	LPC_SYSCTL->PLL[0].PLLCON = 1;
	LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED1;
	LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED2;

	/* Set clock divider. */
	LPC_SYSCTL->CCLKSEL = 0x03;

	/* Configure flash accelerator. */
	LPC_SYSCTL->FLASHCFG = 0x403a;

	/* Check lock bit status. */
	while(((LPC_SYSCTL->PLL[0].PLLSTAT & (1 << 26)) == 0))
		;

	/* Enable and connect. */
	LPC_SYSCTL->PLL[0].PLLCON = 3;
	LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED1;
	LPC_SYSCTL->PLL[0].PLLFEED = PLLFEED_FEED2;
	while(((LPC_SYSCTL->PLL[0].PLLSTAT & (1 << 25)) == 0))
		;

	/* Configure the clock for the USB. */

	if(LPC_SYSCTL->PLL[1].PLLSTAT & (1 << 9))
	{
		/* Enable PLL, disconnected. */
		LPC_SYSCTL->PLL[1].PLLCON = 1;
		LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED1;
		LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED2;
	}

	/* Disable PLL, disconnected. */
	LPC_SYSCTL->PLL[1].PLLCON = 0;
	LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED1;
	LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED2;

	LPC_SYSCTL->PLL[1].PLLCFG = 0x23;
	LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED1;
	LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED2;

	/* Enable PLL, disconnected. */
	LPC_SYSCTL->PLL[1].PLLCON = 1;
	LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED1;
	LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED2;
	while(((LPC_SYSCTL->PLL[1].PLLSTAT & (1 << 10)) == 0))
		;

	/* Enable and connect. */
	LPC_SYSCTL->PLL[1].PLLCON = 3;
	LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED1;
	LPC_SYSCTL->PLL[1].PLLFEED = PLLFEED_FEED2;
	while(((LPC_SYSCTL->PLL[1].PLLSTAT & (1 << 9)) == 0))
		;

	/*  Setup the peripheral bus to be the same as the PLL output (64 MHz). */
	LPC_SYSCTL->PCLKSEL[0] = 0x05555555;
}

void vApplicationStackOverflowHook( xTaskHandle pxTask, char *pcTaskName )
{
	/* This function will get called if a task overflows its stack. */
	(void)pxTask;
	(void)pcTaskName;
	for( ;; );
}

void vConfigureTimerForRunTimeStats( void )
{
	const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

	/* This function configures a timer that is used as the time base when
	collecting run time statistical information - basically the percentage
	of CPU time that each task is utilising.  It is called automatically when
	the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
	to 1). */

	/* Power up and feed the timer. */
	LPC_SYSCTL->PCONP |= 0x02UL;
	LPC_SYSCTL->PCLKSEL[0] = (LPC_SYSCTL->PCLKSEL[0] & (~(0x3<<2))) | (0x01 << 2);

	/* Reset Timer 0 */
	LPC_TIMER0->TCR = TCR_COUNT_RESET;

	/* Just count up. */
	LPC_TIMER0->CTCR = CTCR_CTM_TIMER;

	/* Prescale to a frequency that is good enough to get a decent resolution,
	but not too fast so as to overflow all the time. */
	LPC_TIMER0->PR =  (configCPU_CLOCK_HZ / 10000UL) - 1UL;

	/* Start the counter. */
	LPC_TIMER0->TCR = TCR_COUNT_ENABLE;
}
