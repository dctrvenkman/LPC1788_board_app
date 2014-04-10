/*
 * usb_retarget.c
 *
 *  Created on: Mar 24, 2014
 *      Author: RMamone
 */

#include <stdio.h>
#include <string.h>

#include "board.h"
#include "cdc_vcom.h"
#include "usb_retarget.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

// Function __write() / __sys_write
//
// Called by bottom level of printf routine within RedLib C library to write
// a character. This version writes the character to the USB CDC device.
int WRITEFUNC(int iFileHandle, char *pcBuffer, int iLength)
{
	int bytesSent = 0;
	while(bytesSent != iLength)
		if(iLength - bytesSent > 64)
			bytesSent += vcom_write((uint8_t*)&pcBuffer[bytesSent], 64);
		else
			bytesSent += vcom_write((uint8_t*)&pcBuffer[bytesSent], iLength - bytesSent);
	return bytesSent;
}

// Function __readc() / __sys_readc
//
// Called by bottom level of scanf routine within RedLib C library to read
// a character. This version reads the character from the USB CDC device.
int READFUNC(void)
{
	uint8_t c;
	while(!vcom_bread(&c, 1))
	{
		/* TODO: Use semaphore instead of wait */
		vTaskDelay(portTICK_PERIOD_MS);
	}
	return (int)c;
}
