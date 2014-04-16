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


STATIC FATFS fatFS;	/* File system object */



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


/*
 *
 * FS Related commands
 *
 */
static portBASE_TYPE mountCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	FRESULT rc;		/* Result code */

	rc = f_mount(0, &fatFS);

	writeBuffer[0] = '\0';
	return ret;
}

static portBASE_TYPE umountCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	FRESULT rc;		/* Result code */

	rc = f_mount(0, 0);
	memset(&fatFS, 0, sizeof(fatFS));

	writeBuffer[0] = '\0';
	return ret;
}

static portBASE_TYPE lsCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	FRESULT rc;		/* Result code */
	DIR dir;		/* Directory object */
	FILINFO fno;	/* File information object */
	unsigned char debugBuf[1024];

	rc = f_opendir(&dir, "");
	if(!rc)
	{
		for(;;)
		{
			/* Read a directory item */
			rc = f_readdir(&dir, &fno);
			if(rc || !fno.fname[0])
				break;					/* Error or end of dir */
			if(fno.fattrib & AM_DIR)
				sprintf(debugBuf, "<dir>  %s\r\n", fno.fname);
			else
				sprintf(debugBuf, "%8lu  %s\r\n", fno.fsize, fno.fname);
			printf(debugBuf);
		}
	}

	writeBuffer[0] = '\0';
	return ret;
}

static portBASE_TYPE cdCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	long int paramLen = 0;
	const char* params = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);
	f_chdir(params);

	writeBuffer[0] = '\0';
	return ret;
}

static portBASE_TYPE catCbk(char* writeBuffer, size_t writeBufferLen, const char* cmdString)
{
	portBASE_TYPE ret = 0;
	long int paramLen = 0;
	const char* params = FreeRTOS_CLIGetParameter(cmdString, 1, &paramLen);

	FIL fileObj;			/* File object */
	FRESULT rc;				/* Result code */
	unsigned char buffer;	/* Working buffer */
	UINT br;

	rc = f_open(&fileObj, params, FA_READ);
	if(!rc)
	{
		for(;;)
		{
			/* Read a chunk of file */
			rc = f_read(&fileObj, &buffer, 1, &br);
			if(rc || !br)
				break;					/* Error or end of file */
			putchar(buffer);
		}
		rc = f_close(&fileObj);
	}

	printf("\r\n");

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



static const CLI_Command_Definition_t mountCmd =
{
	(const int8_t* const) "mount",
	(const int8_t* const) "mount:\r\n \r\n",
	mountCbk,
	0
};

static const CLI_Command_Definition_t umountCmd =
{
	(const int8_t* const) "umount",
	(const int8_t* const) "umount:\r\n \r\n",
	umountCbk,
	0
};

static const CLI_Command_Definition_t catCmd =
{
	(const int8_t* const) "cat",
	(const int8_t* const) "cat:\r\n \r\n",
	catCbk,
	1
};

static const CLI_Command_Definition_t cdCmd =
{
	(const int8_t* const) "cd",
	(const int8_t* const) "cd:\r\n \r\n",
	cdCbk,
	1
};

static const CLI_Command_Definition_t lsCmd =
{
	(const int8_t* const) "ls",
	(const int8_t* const) "ls:\r\n \r\n",
	lsCbk,
	0
};






/* Called at startup to register commands */
void registerMiscCmds(void)
{
	FreeRTOS_CLIRegisterCommand(&printRunTimeStatsCmd);
	FreeRTOS_CLIRegisterCommand(&memTestCmd);

	/* Memory commands */
	FreeRTOS_CLIRegisterCommand(&mdCmd);
	FreeRTOS_CLIRegisterCommand(&msetCmd);

	/* FS commands */
	FreeRTOS_CLIRegisterCommand(&catCmd);
	FreeRTOS_CLIRegisterCommand(&lsCmd);
	FreeRTOS_CLIRegisterCommand(&cdCmd);
	FreeRTOS_CLIRegisterCommand(&mountCmd);
	FreeRTOS_CLIRegisterCommand(&umountCmd);
}
