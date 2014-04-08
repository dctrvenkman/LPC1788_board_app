/*
 * sd_disk.c
 *
 *  Created on: Apr 7, 2014
 *      Author: root
 */

#include "sd_disk.h"

#include <string.h>
#include "board.h"
#include "chip.h"
#include "ff.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* buffer size (in byte) for R/W operations */
#define BUFFER_SIZE     4096

STATIC FATFS fatFS;	/* File system object */
STATIC FIL fileObj;	/* File object */
STATIC INT buffer[BUFFER_SIZE / 4];		/* Working buffer */
STATIC volatile int32_t sdcWaitExit = 0;
STATIC SDMMC_EVENT_T *event;
STATIC volatile Status  eventResult = ERROR;
SDMMC_CARD_T sdCardInfo;


/* Delay callback for timed SDIF/SDMMC functions */
STATIC void waitMs(uint32_t time)
{
	vTaskDelay(time * portTICK_PERIOD_MS);
}

/**
 * @brief	Sets up the event driven wakeup
 * @param	pEvent : Event information
 * @return	Nothing
 */
STATIC void setupEvWakeup(void *pEvent)
{
	/* Wait for IRQ - for an RTOS, you would pend on an event here with a IRQ based wakeup. */
	NVIC_ClearPendingIRQ(DMA_IRQn);
	event = (SDMMC_EVENT_T *)pEvent;
	sdcWaitExit = 0;
	eventResult = ERROR;
	NVIC_EnableIRQ(DMA_IRQn);
}

/**
 * @brief	A better wait callback for SDMMC driven by the IRQ flag
 * @return	0 on success, or failure condition (Nonzero)
 */
STATIC uint32_t waitEvIRQDriven(void)
{
	/* Wait for event, would be nice to have a timeout, but keep it  simple */
	while (sdcWaitExit == 0) {}
	if (eventResult) {
		return 0;
	}

	return 1;
}

void die(FRESULT rc)
{
	//DEBUGOUT("Failed with rc=%u.\r\n", rc);
	//	for (;; ) {}
}

SDMMC_CARD_T sdCardInfo;

void sd_test(void)
{
	FRESULT rc;		/* Result code */
	DIR dir;		/* Directory object */
	FILINFO fno;	/* File information object */
	UINT bw, br, i;
	uint8_t *ptr;
	char debugBuf[64];


	memset(&sdCardInfo, 0, sizeof(sdCardInfo));
	sdCardInfo.evsetup_cb = setupEvWakeup;
	sdCardInfo.waitfunc_cb = waitEvIRQDriven;
	sdCardInfo.msdelay_func = waitMs;

	Chip_SDC_Init(LPC_SDC);

	/* Enable SD interrupt */
	NVIC_EnableIRQ(SDC_IRQn);


	rc = f_mount(0, &fatFS);		/* Register volume work area (never fails) */
	rc = f_open(&fileObj, "MESSAGE.TXT", FA_READ);
	if (rc) {
		die(rc);
	}
	else
	{
		for(;;)
		{
			/* Read a chunk of file */
			rc = f_read(&fileObj, buffer, sizeof buffer, &br);
			if(rc || !br)
				break;					/* Error or end of file */
			ptr = (uint8_t *) buffer;
			for(i = 0; i < br; i++)/* Type the data */
				DEBUGOUT("%c", ptr[i]);
		}
		if(rc)
			die(rc);
		rc = f_close(&fileObj);
		if(rc)
			die(rc);
	}
}
