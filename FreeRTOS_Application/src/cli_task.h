/*
 * cli_task.h
 *
 *  Created on: Mar 19, 2014
 *      Author: RMamone
 */

#ifndef __CLI_TASK_H__
#define __CLI_TASK_H__

#include "FreeRTOS.h"
#include "semphr.h"

#define PRIORITY_CLI_TASK		1

unsigned long CLITaskInit(SemaphoreHandle_t usb_uart_connected_sem);

#endif // __CLI_TASK_H__
