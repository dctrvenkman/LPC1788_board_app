/*
 * misc_cli_cmds.c
 *
 *  Created on: Mar 19, 2014
 *      Author: RMamone
 */

#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "mem_tests.h"
#include "ff.h"

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "task.h"


#define SDRAM_SIZE (16 * 1024 * 1024) /* 16MB SDRAM */
#define PRINT_ROW_CNT  16

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

static portBASE_TYPE mdCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	long int paramLen = 0;
	unsigned int addr, count, i;
	unsigned char numBlanks;
	unsigned char* data;

	const char* params = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	sscanf(params, "0x%x %d", &addr, &count);
	numBlanks = addr % PRINT_ROW_CNT;
	data = addr;

	if(numBlanks)
	{
		printf("\r\n[0x%08X]  ", addr - numBlanks);
		for(; numBlanks > 0; numBlanks--)
			printf(".. ");
	}

	for(i = 0; count > 0; count--, addr++)
	{
		if(!(addr % PRINT_ROW_CNT))
			printf("\r\n[0x%08X]  ", addr);
		printf("%02X ", data[i++]);
	}
	printf("\r\n");

	writeBuffer[0] = '\0';
	return ret;
}

static portBASE_TYPE msetCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	long int paramLen = 0;
	unsigned int addr, count;
	unsigned int data;

	const char* params = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	sscanf(params, "0x%x 0x%x %d", &addr, &data, &count);
	memset(addr, data, count);

	writeBuffer[0] = '\0';
	return ret;
}




static portBASE_TYPE sdCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	FRESULT rc;
	FATFS fatFS;
	FIL fileObj;
	char buffer[256];
	UINT bytesRead;
	portBASE_TYPE ret = 0;

	f_mount(0, &fatFS);
	//rc = f_mkfs(0, 0, 0);

	rc = f_open(&fileObj, "MESSAGE.TXT", FA_READ);
	if(!rc)
	{
		rc = f_read(&fileObj, buffer, sizeof(buffer), &bytesRead);
		rc = f_close(&fileObj);
	}

	writeBuffer[0] = '\0';
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

static const CLI_Command_Definition_t mdCmd =
{
	(const int8_t* const) "md",
	(const int8_t* const) "md:\r\n  Display [param2] number of bytes of memory starting at address [param1]\r\n",
	mdCbk,
	2
};

static const CLI_Command_Definition_t msetCmd =
{
	(const int8_t* const) "mset",
	(const int8_t* const) "mset:\r\n  Set [param3] number of bytes of memory starting at address [param1] to value [param2]\r\n",
	msetCbk,
	3
};



static const CLI_Command_Definition_t sdCmd =
{
	(const char* const) "sd",
	(const char* const) "sd:\r\n  Test\r\n",
	sdCbk,
	0
};



/* Called at startup to register commands */
void registerMiscCmds(void)
{
	FreeRTOS_CLIRegisterCommand(&printRunTimeStatsCmd);
	FreeRTOS_CLIRegisterCommand(&memTestCmd);
	FreeRTOS_CLIRegisterCommand(&sdCmd);

	/* Memory commands */
	FreeRTOS_CLIRegisterCommand(&mdCmd);
	FreeRTOS_CLIRegisterCommand(&msetCmd);
}
