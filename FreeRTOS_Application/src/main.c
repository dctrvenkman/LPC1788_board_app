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

/* Standard includes. */
#include "stdio.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* Red Suite includes. */
#include "lcd_driver.h"
#include "lcd.h"

/* Board definition includes */
#include "board.h"

#include "usb.h"
#include "misc_cli_cmds.h"
#include "cli_task.h"
#include "sd_disk.h"
#include "ff.h"

/*-----------------------------------------------------------*/

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware(void);

/*-----------------------------------------------------------*/


static void testTask1(void *pvParameters)
{
	while(1)
	{
		vTaskDelay(1000 * portTICK_PERIOD_MS);
	}
}

static void testTask2(void *pvParameters)
{
	while(1)
	{
		int i = 0;
		for(;i < 100; i++)
			;
		vTaskDelay(portTICK_PERIOD_MS);
	}
}

void LCD_IRQHandler(void)
{
	int ints = LPC_LCD->INTSTAT;
	LPC_LCD->INTCLR = 0x1E;
}

int main(void)
{
	/* Disable watchdog */
	LPC_WWDT->MOD = 0;

	Chip_LCD_DeInit(LPC_LCD);

	{
		int i, j;
		int* addr = (int*)0xa0000000;
		for(i = 0; i < 272; i++)
		{
			for(j = 0; j < 480; j++)
			{
#if 0
				if(j < 120)
					*addr = 0x000ff000;
				else if(j < 240)
					*addr = 0x0ffffff0;
				else if(j < 360)
					*addr = 0x0ff00000;
				else
					*addr = 0x00000ff0;
#elif 0
				if(j < 60)
					*addr = 0x0ffffff0;
				else if(j < 120)
					*addr = 0x000ffff0;
				else if(j < 180)
					*addr = 0x0ffff000;
				else if(j < 240)
					*addr = 0x000ff000;
				else if(j < 300)
					*addr = 0x0ff00ff0;
				else if(j < 360)
					*addr = 0x00000ff0;
				else if(j < 420)
					*addr = 0x0ff00000;
				else
					*addr = 0x00000000;
#elif 0
				if(j < 60)
					*addr = 0x000000ff;
				else if(j < 120)
					*addr = 0x0000ff00;
				else if(j < 180)
					*addr = 0x0000ffff;
				else if(j < 240)
					*addr = 0x00ff0000;
				else if(j < 300)
					*addr = 0x00ff00ff;
				else if(j < 360)
					*addr = 0x00ffff00;
				else if(j < 420)
					*addr = 0x00ffffff;
				else
					*addr = 0x00000000;
#endif
				addr++;
			}
		}
	}

	/* Set LCD DMA to highest priority */
	LPC_SYSCON->MATRIXARB |= (3 << 10);
	Chip_LCD_Init(LPC_LCD, &NHD_43_480x272);
	//LPC_LCD->CTRL |= 1 << 9; //BEBO - big endian
	Chip_LCD_SetUPFrameBuffer(LPC_LCD, (void*)FRAMEBUFFER_ADDR);
	Chip_LCD_SetLPFrameBuffer(LPC_LCD, (void*)FRAMEBUFFER_ADDR);
	Chip_LCD_EnableInts(LPC_LCD, 0x12);
	NVIC_EnableIRQ(LCD_IRQn);
	Chip_LCD_PowerOn(LPC_LCD);

	/* Configure the hardware. */
	prvSetupHardware();

	CLITaskInit();
	registerMiscCmds();

    xTaskCreate(testTask1, "test1", configMINIMAL_STACK_SIZE * 10, (void *) NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(testTask2, "test2", configMINIMAL_STACK_SIZE, (void *) NULL, tskIDLE_PRIORITY + 1, NULL);

    /* Start the scheduler. */
	vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle
    task.  The idle task is created within vTaskStartScheduler(). */
	while(1)
		;
}

void vApplicationTickHook(void)
{
	/* Called from every tick interrupt as described in the comments at the top
	of this file. */
}

void prvSetupHardware(void)
{
	Board_Init();

	/* Initialize USB device */
	/* NOTE: because of retarget, printf cannot be used before this call */
	USBInit();
}

void vApplicationStackOverflowHook(xTaskHandle pxTask, char *pcTaskName)
{
	/* This function will get called if a task overflows its stack. */
	(void)pxTask;
	(void)pcTaskName;
	while(1)
		;
}

void vConfigureTimerForRunTimeStats(void)
{
	/* This function configures a timer that is used as the time base when
	collecting run time statistical information - basically the percentage
	of CPU time that each task is utilising.  It is called automatically when
	the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
	to 1). */

	/* Power up and feed the timer. */
	Chip_TIMER_Init(LPC_TIMER0);
	Chip_TIMER_Reset(LPC_TIMER0);
	Chip_TIMER_TIMER_SetCountClockSrc(LPC_TIMER0, TIMER_CAPSRC_RISING_PCLK, 0);
	Chip_TIMER_PrescaleSet(LPC_TIMER0, (configCPU_CLOCK_HZ / 100000UL) - 1UL);
	Chip_TIMER_Enable(LPC_TIMER0);
}
