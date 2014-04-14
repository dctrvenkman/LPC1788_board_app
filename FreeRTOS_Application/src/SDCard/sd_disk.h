/*
 * sd_disk.h
 *
 *  Created on: Apr 7, 2014
 *      Author: root
 */

#ifndef SD_DISK_H_
#define SD_DISK_H_

#include "chip.h"

SDMMC_CARD_T* SDCardInit(void);

void sd_test(void);





/* RTC STUFF */

#include "integer.h"

typedef struct {
	WORD	year;	/* 1..4095 */
	BYTE	month;	/* 1..12 */
	BYTE	mday;	/* 1.. 31 */
	BYTE	wday;	/* 1..7 */
	BYTE	hour;	/* 0..23 */
	BYTE	min;	/* 0..59 */
	BYTE	sec;	/* 0..59 */
} RTC;

int rtc_initialize (void);		/* Initialize RTC */
int rtc_gettime (RTC*);			/* Get time */
int rtc_settime (const RTC*);	/* Set time */


#endif /* SD_DISK_H_ */
