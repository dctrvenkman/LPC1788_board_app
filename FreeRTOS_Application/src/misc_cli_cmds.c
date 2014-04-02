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

#include "board.h"
#include "mem_tests.h"

/* CLI Callback Definitions */
static portBASE_TYPE printRunTimeStatsCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	vTaskGetRunTimeStats(writeBuffer);
	writeBufferLen = strlen((char*)writeBuffer);
	return ret;
}

static portBASE_TYPE memTestCbk(int8_t* writeBuffer, size_t writeBufferLen, const int8_t* cmdString)
{
	portBASE_TYPE ret = 0;
	uint32_t* fail_addr;
	uint32_t is_val, ex_val;
	MEM_TEST_SETUP_T test = {EMC_ADDRESS_DYCS0, 8 * 1024 * 1024, fail_addr, is_val, ex_val};
	bool passed = false;

	passed = mem_test_walking0(&test);
	passed = mem_test_walking1(&test);

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

static const CLI_Command_Definition_t memTestCmd =
{
	(const int8_t* const) "memtest",
	(const int8_t* const) "memtest:\r\n  Runs SDRAM memory tests\r\n",
	memTestCbk,
	0
};


/* Called at startup to register commands */
void registerMiscCmds(void)
{
	FreeRTOS_CLIRegisterCommand(&printRunTimeStatsCmd);
	FreeRTOS_CLIRegisterCommand(&memTestCmd);
}
