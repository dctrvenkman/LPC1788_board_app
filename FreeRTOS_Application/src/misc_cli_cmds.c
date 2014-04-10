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

#define SDRAM_SIZE (16 * 1024 * 1024) /* 16MB SDRAM */

/* CLI Callback Definitions */
static portBASE_TYPE printRunTimeStatsCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	vTaskGetRunTimeStats(writeBuffer);
	return ret;
}

static portBASE_TYPE memTestCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	uint32_t* fail_addr = 0;
	uint32_t is_val = 0, ex_val = 0;
	MEM_TEST_SETUP_T test = {(uint32_t*)EMC_ADDRESS_DYCS0, SDRAM_SIZE, fail_addr, is_val, ex_val};

	writeBuffer[0] = '\0';

	strcat(writeBuffer, "Walking 0's test: ");
	if(mem_test_walking0(&test))
		strcat(writeBuffer, "PASSED.\r\n");
	else
		sprintf(&writeBuffer[strlen(writeBuffer)], "FAILED at address 0x%x expected 0x%x found 0x%x.\r\n", test.fail_addr, test.ex_val, test.is_val);

	strcat(writeBuffer, "Walking 1's test: ");
	if(mem_test_walking1(&test))
		strcat(writeBuffer, "PASSED.\r\n");
	else
		sprintf(&writeBuffer[strlen(writeBuffer)], "FAILED at address 0x%x expected 0x%x found 0x%x.\r\n", test.fail_addr, test.ex_val, test.is_val);

	strcat(writeBuffer, "Pattern test: ");
	if(mem_test_pattern(&test))
		strcat(writeBuffer, "PASSED.\r\n");
	else
		sprintf(&writeBuffer[strlen(writeBuffer)], "FAILED at address 0x%x expected 0x%x found 0x%x.\r\n", test.fail_addr, test.ex_val, test.is_val);

	return ret;
}


/* CLI Command Definitions */
static const CLI_Command_Definition_t printRunTimeStatsCmd =
{
	(const char* const) "printRunTimeStats",
	(const char* const) "printRunTimeStats:\r\n  Prints the FreeRTOS runtime stats\r\n",
	printRunTimeStatsCbk,
	0
};

static const CLI_Command_Definition_t memTestCmd =
{
	(const char* const) "memtest",
	(const char* const) "memtest:\r\n  Runs SDRAM memory tests\r\n",
	memTestCbk,
	0
};


/* Called at startup to register commands */
void registerMiscCmds(void)
{
	FreeRTOS_CLIRegisterCommand(&printRunTimeStatsCmd);
	FreeRTOS_CLIRegisterCommand(&memTestCmd);
}
