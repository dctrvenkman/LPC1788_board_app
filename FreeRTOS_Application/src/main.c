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

/* Task definition includes */
#include "usb_task.h"


#include "misc_cli_cmds.h"
#include "cli_task.h"
#include "sd_disk.h"

/*-----------------------------------------------------------*/

/*
 * Configure the hardware for the demo.
 */
static void prvSetupHardware(void);

/*-----------------------------------------------------------*/

SemaphoreHandle_t usb_uart_connected_sem;


static void testTask1(void *pvParameters)
{
	while(1)
	{
		//sd_test();
		vTaskDelay(2 * portTICK_PERIOD_MS);
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

int main(void)
{
	usb_uart_connected_sem = xSemaphoreCreateBinary();

	/* Configure the hardware. */
	prvSetupHardware();

    /* Create the USB task. */
	//USBInit(usb_uart_connected_sem);
    //CLITaskInit(usb_uart_connected_sem);

    xTaskCreate(testTask1, "test1", configMINIMAL_STACK_SIZE * 10, (void *) NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(testTask2, "test2", configMINIMAL_STACK_SIZE, (void *) NULL, tskIDLE_PRIORITY, NULL);

    //LCDdriver_initialisation();
    //LCD_PrintString(5, 10, "FreeRTOS.org", 14, COLOR_GREEN);

    registerMiscCmds();

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



#define SDRAM_SIZE 0x04000000
unsigned int SDRAM_BASE_ADDR = 0xa0000000;

#define SDRAM_PERIOD          8.3	// 120MHz
//#define SDRAM_PERIOD          16.7  // 60MHz

#define P2C(Period)           (((Period<SDRAM_PERIOD)?0:(unsigned int)((float)Period/SDRAM_PERIOD))+1)

#define SDRAM_REFRESH         7813
#define SDRAM_TRP             20
#define SDRAM_TRAS            45
#define SDRAM_TAPR            1
#define SDRAM_TDAL            3
#define SDRAM_TWR             3
#define SDRAM_TRC             65
#define SDRAM_TRFC            66
#define SDRAM_TXSR            67
#define SDRAM_TRRD            15
#define SDRAM_TMRD            3

/*************************************************************************
 * Function Name: SDRAM_Init
 * Parameters: none
 *
 * Return: none
 *
 * Description: SDRAM controller and memory init
 *
 *************************************************************************/
void SDRAM_Init (void)
{
  volatile unsigned int i;

  /* Enable EMC clock*/
  LPC_SYSCTL->PCON |= (1<<11);// PCONP_bit.PCEMC = 1;
  /*The EMC uses the same clock as the CPU*/
  LPC_SYSCTL->EMCCLKSEL = 0;// EMCCLKSEL = 0;

  /*Init SDRAM controller*/
  // EMCDLYCTL_bit.CMDDLY = 0x8;
  /*Set data read delay*/
  // EMCDLYCTL_bit.FBCLKDLY = 0x8;
  /**/
  //EMCDLYCTL_bit.CLKOUT0DLY = 0x8;
  LPC_SYSCTL->EMCDLYCTL = 0x00080808;


  LPC_EMC->CONTROL = 1; //EMCControl      = 1;         // enable EMC
  LPC_EMC->DYNAMICREADCONFIG = 1; //EMCDynamicReadConfig = 1;
  //EMCDynamicRasCas0_bit.CAS = 3;
  //EMCDynamicRasCas0_bit.RAS = 3;
  LPC_EMC->DYNAMICRASCAS0 = 0x303;
  LPC_EMC->DYNAMICRP = P2C(SDRAM_TRP); //EMCDynamictRP = P2C(SDRAM_TRP);
  LPC_EMC->DYNAMICRAS = P2C(SDRAM_TRAS); //EMCDynamictRAS = P2C(SDRAM_TRAS);
  LPC_EMC->DYNAMICSREX = P2C(SDRAM_TXSR); //EMCDynamictSREX = P2C(SDRAM_TXSR);
  LPC_EMC->DYNAMICAPR = SDRAM_TAPR; //EMCDynamictAPR = SDRAM_TAPR;
  LPC_EMC->DYNAMICDAL = SDRAM_TDAL+P2C(SDRAM_TRP); //EMCDynamictDAL = SDRAM_TDAL+P2C(SDRAM_TRP);
  LPC_EMC->DYNAMICWR = SDRAM_TWR; //EMCDynamictWR = SDRAM_TWR;
  LPC_EMC->DYNAMICRC = P2C(SDRAM_TRC); //EMCDynamictRC = P2C(SDRAM_TRC);
  LPC_EMC->DYNAMICRFC = P2C(SDRAM_TRFC); //EMCDynamictRFC = P2C(SDRAM_TRFC);
  LPC_EMC->DYNAMICXSR = P2C(SDRAM_TXSR); //EMCDynamictXSR = P2C(SDRAM_TXSR);
  LPC_EMC->DYNAMICRRD = P2C(SDRAM_TRRD); //EMCDynamictRRD = P2C(SDRAM_TRRD);
  LPC_EMC->DYNAMICMRD = SDRAM_TMRD; //EMCDynamictMRD = SDRAM_TMRD;

  LPC_EMC->DYNAMICCONFIG0 = 0x0000680; //EMCDynamicConfig0 = 0x0000680;        // 13 row, 9 - col, SDRAM, 16 bit data bus

  // JEDEC General SDRAM Initialization Sequence
  // DELAY to allow power and clocks to stabilize ~100 us
  // NOP
  LPC_EMC->DYNAMICCONTROL = 0x0183; //EMCDynamicControl = 0x0183;
  for(i = 200*30; i;i--);
  // PALL
  LPC_EMC->DYNAMICCONTROL = 0x0103; //EMCDynamicControl_bit.I = 2;
  LPC_EMC->DYNAMICREFRESH = 1; //EMCDynamicRefresh = 1;
  for(i= 256; i; --i); // > 128 clk
  LPC_EMC->DYNAMICREFRESH = P2C(SDRAM_REFRESH) >> 4; //EMCDynamicRefresh = P2C(SDRAM_REFRESH) >> 4;
  // COMM
  LPC_EMC->DYNAMICCONTROL = 0x0083; //EMCDynamicControl_bit.I = 1;
  // Burst 4, Sequential, CAS-3
  //volatile unsigned int Dummy = *(volatile unsigned int *)((unsigned int)&SDRAM_BASE_ADDR + (0x33UL << (12)));
  i = *(volatile unsigned int *)(SDRAM_BASE_ADDR + (0x33UL << 12));
  // NORM
  LPC_EMC->DYNAMICCONTROL = 0; //EMCDynamicControl = 0x0000;
  LPC_EMC->DYNAMICCONFIG0 |= 1 << 19;; //EMCDynamicConfig0_bit.B = 1;
  for(i = 100000; i;i--);
}




































#include "mem_tests.h"
void prvSetupHardware(void)
{
	Board_Init();

#if 1
	SDRAM_Init();
	{
		uint32_t* fail_addr;
		uint32_t is_val, ex_val;
		MEM_TEST_SETUP_T test = {EMC_ADDRESS_DYCS0, 16 * 1024 * 1024, fail_addr, is_val, ex_val};
		bool passed = false;
		memset(EMC_ADDRESS_DYCS0, 0xffffffff, 1024 * 1024);
		memset(EMC_ADDRESS_DYCS0, 0x0, 1024 * 1024);
		memset(EMC_ADDRESS_DYCS0, 0xaa55aa55, 1024 * 1024);
		memset(EMC_ADDRESS_DYCS0, 0x55aa55aa, 1024 * 1024);
		memset(EMC_ADDRESS_DYCS0, 0xa5, 1024 * 1024);
		memset(EMC_ADDRESS_DYCS0, 0x5a, 1024 * 1024);
		passed = mem_test_walking0(&test);
		passed = mem_test_walking1(&test);
		passed = mem_test_pattern(&test);
		test.fail_addr;
		while(1)
			;
	}
#endif
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
