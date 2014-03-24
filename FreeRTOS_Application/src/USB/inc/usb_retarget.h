/*
 * usb_retarget.h
 *
 *  Created on: Mar 24, 2014
 *      Author: RMamone
 */

#ifndef USB_RETARGET_H_
#define USB_RETARGET_H_

#include "board.h"

#if (__REDLIB_INTERFACE_VERSION__ >= 20000)
	// We are using new Redlib_v2 semihosting interface
	#define WRITEFUNC __sys_write
	#define READFUNC __sys_readc
#else
	// We are using original Redlib semihosting interface
	#define WRITEFUNC __write
	#define READFUNC __readc
#endif

#endif /* USB_RETARGET_H_ */
