/*
 * usb_task.h
 *
 *  Created on: Mar 18, 2014
 *      Author: RMamone
 */

#ifndef USB_TASK_H_
#define USB_TASK_H_

#include "FreeRTOS.h"
#include "semphr.h"

void USBInit(SemaphoreHandle_t usb_uart_connected_sem);

#endif /* USB_TASK_H_ */
