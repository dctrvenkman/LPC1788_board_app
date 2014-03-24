/*
 * misc_cli_cmds.c
 *
 *  Created on: Mar 19, 2014
 *      Author: RMamone
 */

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"
#include <string.h>
#include <stdlib.h>

/* CLI Callback Definitions */
static portBASE_TYPE printRunTimeStatsCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	vTaskGetRunTimeStats(writeBuffer);
	writeBufferLen = strlen((char*)writeBuffer);
	return ret;
}

/* CLI Command Definitions */
static const CLI_Command_Definition_t printRunTimeStatsCmd =
{
	(const int8_t* const) "printRunTimeStats",
	(const int8_t* const) "printRunTimeStats:\r\n  Prints the FreeRTOS runtime stats\r\n",
	printRunTimeStatsCbk,
	0
};

/* Called at startup to register commands */
void registerMiscCmds(void)
{
	FreeRTOS_CLIRegisterCommand(&printRunTimeStatsCmd);
}
