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

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


#include "ff.h"
#include "ffconf.h"
#include "diskio.h"



/* buffer size (in byte) for R/W operations */
#define BUFFER_SIZE     4096

STATIC volatile int32_t sdcWaitExit = 0;
STATIC SDMMC_EVENT_T *event;
STATIC volatile Status  eventResult = ERROR;
SDMMC_CARD_T sdCardInfo;

/*****************************************************************************
 * IRQ functions
 ****************************************************************************/

/**
 * @brief	GPDMA interrupt handler sub-routine
 * @return	Nothing
 */
void DMA_IRQHandler(void)
{
	eventResult = Chip_GPDMA_Interrupt(LPC_GPDMA, event->DmaChannel);
	sdcWaitExit = 1;
	NVIC_DisableIRQ(DMA_IRQn);
}

/**
 * @brief	SDC interrupt handler sub-routine
 * @return	Nothing
 */
void SDIO_IRQHandler(void)
{
	int32_t Ret;
	Ret = Chip_SDMMC_IRQHandler(LPC_SDC, NULL,0,NULL,0);
	if(Ret < 0)
	{
		eventResult = ERROR;
		sdcWaitExit = 1;
	}
}


/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Disk Status */
static volatile DSTATUS Stat = STA_NOINIT;

/* 100Hz decrement timer stopped at zero (disk_timerproc()) */
static volatile WORD Timer2;

static SDMMC_CARD_T *hCard;








/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* buffer size (in byte) for R/W operations */
#define BUFFER_SIZE     4096

STATIC FATFS fatFS;	/* File system object */
STATIC FIL fileObj;	/* File object */
STATIC INT buffer[BUFFER_SIZE / 4];		/* Working buffer */


/*****************************************************************************
 * Private functions
 ****************************************************************************/

/**
 * @brief	Wait for the SD card to complete all operations and become ready
 * @param	hCrd	: Pointer to Card Handle
 * @param	tout	: Time to wait, in milliseconds
 * @return	0 when operation failed 1 when successfully completed
 */
STATIC INLINE int FSMCI_CardReadyWait(SDMMC_CARD_T *hCrd, int tout)
{
	while(tout--)
	{
		vTaskDelay(portTICK_PERIOD_MS);
		if (Chip_SDMMC_GetCardStatus(LPC_SDC, hCrd) & R1_READY_FOR_DATA)
			return 1;
	}
	return 0;
}


/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize Disk Drive */
DSTATUS disk_initialize(BYTE drv)
{
	if(drv)
		return STA_NOINIT;				/* Supports only single drive */
	/*	if (Stat & STA_NODISK) return Stat;	*//* No card in the socket */

	if(Stat != STA_NOINIT)
		return Stat;					/* card is already enumerated */

	#if !_FS_READONLY
	rtc_initialize();
	#endif

	/* Initialize the Card Data Structure */
	hCard = SDCardInit();

	/* Reset */
	Stat = STA_NOINIT;

	/* TODO: Implement Card Detect */
	//FSMCI_CardInsertWait(hCard); /* Wait for card to be inserted */

	/* Enumerate the card once detected. Note this function may block for a little while. */
	if(!Chip_SDMMC_Acquire(LPC_SDC, hCard))
	{
		DEBUGOUT("Card Acquire failed...\r\n");
		return Stat;
	}

	Stat &= ~STA_NOINIT;
	return Stat;

}

/* Disk Drive miscellaneous Functions */
DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
	DRESULT res;
	BYTE *ptr = buff;

	if(drv)
		return RES_PARERR;

	if(Stat & STA_NOINIT)
		return RES_NOTRDY;

	res = RES_ERROR;

	switch (ctrl)
	{
	case CTRL_SYNC:	/* Make sure that no pending write process */
		if(FSMCI_CardReadyWait(hCard, 50))
			res = RES_OK;
		break;

	case GET_SECTOR_COUNT:	/* Get number of sectors on the disk (DWORD) */
		*(DWORD *) buff = hCard->blocknr;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE:	/* Get R/W sector size (WORD) */
		*(WORD *) buff = hCard->block_len;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE:/* Get erase block size in unit of sector (DWORD) */
		*(DWORD *) buff = 4UL * 1024; /* NOTE: Fixed at 4KB */
		res = RES_OK;
		break;

	case MMC_GET_TYPE:		/* Get card type flags (1 byte) */
		*ptr = hCard->card_type;
		res = RES_OK;
		break;

	case MMC_GET_CSD:		/* Receive CSD as a data block (16 bytes) */
		*((uint32_t *) buff + 0) = hCard->csd[0];
		*((uint32_t *) buff + 1) = hCard->csd[1];
		*((uint32_t *) buff + 2) = hCard->csd[2];
		*((uint32_t *) buff + 3) = hCard->csd[3];
		res = RES_OK;
		break;

	case MMC_GET_CID:		/* Receive CID as a data block (16 bytes) */
		*((uint32_t *) buff + 0) = hCard->cid[0];
		*((uint32_t *) buff + 1) = hCard->cid[1];
		*((uint32_t *) buff + 2) = hCard->cid[2];
		*((uint32_t *) buff + 3) = hCard->cid[3];
		res = RES_OK;
		break;

	case MMC_GET_SDSTAT:/* Receive SD status as a data block (64 bytes) */
		if(Chip_SDMMC_GetSDStatus(LPC_SDC, hCard, buff) > 0)
			res = RES_OK;
	break;

	default:
		res = RES_PARERR;
		break;
	}

	return res;
}

/* Read Sector(s) */
DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
	if(drv || !count)
		return RES_PARERR;

	if(Stat & STA_NOINIT)
		return RES_NOTRDY;

	if(Chip_SDMMC_ReadBlocks(LPC_SDC, hCard, buff, sector, count))
		return RES_OK;

	return RES_ERROR;
}

/* Get Disk Status */
DSTATUS disk_status(BYTE drv)
{
	if(drv)
		return STA_NOINIT;	/* Supports only single drive */

	return Stat;
}

/* Write Sector(s) */
DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{

	if(drv || !count)
		return RES_PARERR;

	if(Stat & STA_NOINIT)
		return RES_NOTRDY;

	if(Chip_SDMMC_WriteBlocks(LPC_SDC, hCard, buff, sector, count))
		return RES_OK;

	return RES_ERROR;
}



















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
	if(eventResult)
		return 0;

	return 1;
}

void die(FRESULT rc)
{
	//DEBUGOUT("Failed with rc=%u.\r\n", rc);
	//	for (;; ) {}
}


SDMMC_CARD_T* SDCardInit(void)
{
	memset(&sdCardInfo, 0, sizeof(sdCardInfo));
	sdCardInfo.evsetup_cb = setupEvWakeup;
	sdCardInfo.waitfunc_cb = waitEvIRQDriven;
	sdCardInfo.msdelay_func = waitMs;

	Chip_SDC_Init(LPC_SDC);

	/* Enable SD interrupt */
	NVIC_EnableIRQ(SDC_IRQn);
	return &sdCardInfo;
}












/* RTC STUFF */


int rtc_initialize (void)
{
	static int init;
	RTC_TIME_T rtcTime;

	if (init) /* Already initialized */
		return 1;

	/* RTC Block section ------------------------------------------------------ */
	Chip_RTC_Init(LPC_RTC);

	/* Set current time for RTC */
	/* Current time is 8:00:00PM, 2013-01-31 */
	rtcTime.time[RTC_TIMETYPE_SECOND]     = 0;
	rtcTime.time[RTC_TIMETYPE_MINUTE]     = 0;
	rtcTime.time[RTC_TIMETYPE_HOUR]       = 20;
	rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = 31;
	rtcTime.time[RTC_TIMETYPE_MONTH]      = 1;
	rtcTime.time[RTC_TIMETYPE_YEAR]       = 2013;
	Chip_RTC_SetFullAlarmTime(LPC_RTC, &rtcTime);

	/* Enable rtc (starts increase the tick counter and second counter register) */
	Chip_RTC_Enable(LPC_RTC, ENABLE);
	init = 1;

	return 1;
}

int rtc_gettime (RTC *rtc)
{
	RTC_TIME_T rtcTime;

	Chip_RTC_GetFullTime(LPC_RTC, &rtcTime);

	rtc->sec = rtcTime.time[RTC_TIMETYPE_SECOND];
	rtc->min = rtcTime.time[RTC_TIMETYPE_MINUTE];
	rtc->hour = rtcTime.time[RTC_TIMETYPE_HOUR];
	rtc->wday = rtcTime.time[RTC_TIMETYPE_DAYOFWEEK];
	rtc->mday = rtcTime.time[RTC_TIMETYPE_DAYOFMONTH];
	rtc->month = rtcTime.time[RTC_TIMETYPE_MONTH];
	rtc->year = rtcTime.time[RTC_TIMETYPE_YEAR];
  return 1;
}

int rtc_settime (const RTC *rtc)
{
	RTC_TIME_T rtcTime;

	rtcTime.time[RTC_TIMETYPE_SECOND]     = rtc->sec;
	rtcTime.time[RTC_TIMETYPE_MINUTE]     = rtc->min;
	rtcTime.time[RTC_TIMETYPE_HOUR]       = rtc->hour;
	rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = rtc->wday;
	rtcTime.time[RTC_TIMETYPE_DAYOFMONTH] = rtc->mday;
	rtcTime.time[RTC_TIMETYPE_MONTH]      = rtc->month;
	rtcTime.time[RTC_TIMETYPE_YEAR]	      = rtc->year;

	Chip_RTC_GetFullTime(LPC_RTC, &rtcTime);

  return 1;
}

/**
 * @brief	User Provided Timer Function for FatFs module
 * @return	Nothing
 * @note	This is a real time clock service to be called from FatFs module.
 * Any valid time must be returned even if the system does not support a real time clock.
 * This is not required in read-only configuration.
 */
DWORD get_fattime()
{
	RTC rtc;

	/* Get local time */
	rtc_gettime(&rtc);

	/* Pack date and time into a DWORD variable */
	return ((DWORD) (rtc.year - 1980) << 25)
		   | ((DWORD) rtc.month << 21)
		   | ((DWORD) rtc.mday << 16)
		   | ((DWORD) rtc.hour << 11)
		   | ((DWORD) rtc.min << 5)
		   | ((DWORD) rtc.sec >> 1);
}









