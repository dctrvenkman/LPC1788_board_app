/*
 * misc_cli_cmds.c
 *
 *  Created on: Mar 19, 2014
 *      Author: RMamone
 */

#include <stdio.h>
#include "board.h"
#include "mem_tests.h"
#include "ff.h"
#include "cliParser.h"
#include "FreeRTOS.h"
#include "task.h"

STATIC FATFS fatFS;	/* File system object */

/*
 * printRTStats - Prints FreeRTOS task runtime stats
 */
void printRunTimeStatsCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	UNUSED(params);
	char buff[128];
	vTaskGetRunTimeStats(buff);
	puts(buff);
}


/*
 * runMemTests - Runs SDRAM memory tests
 */
void runMemTestsCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	UNUSED(params);
	uint32_t* fail_addr = 0;
	uint32_t is_val = 0, ex_val = 0;
	MEM_TEST_SETUP_T test = {(uint32_t*)EMC_ADDRESS_DYCS0, SDRAM_SIZE, fail_addr, is_val, ex_val};

	printf("Walking 0's test...");
	if(mem_test_walking0(&test))
		printf(" PASSED.\r\n");
	else
		printf("FAILED at address 0x%x expected 0x%x found 0x%x.\r\n", test.fail_addr, test.ex_val, test.is_val);

	printf("Walking 1's test...");
	if(mem_test_walking1(&test))
		printf(" PASSED.\r\n");
	else
		printf("FAILED at address 0x%x expected 0x%x found 0x%x.\r\n", test.fail_addr, test.ex_val, test.is_val);

	printf("Pattern test...");
	if(mem_test_pattern(&test))
		printf("PASSED.\r\n");
	else
		printf("FAILED at address 0x%x expected 0x%x found 0x%x.\r\n", test.fail_addr, test.ex_val, test.is_val);
}


/*
 * mount - Mount SD Card FS
 */
void mountCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	UNUSED(params);
	FRESULT rc;
	rc = f_mount(0, &fatFS);
}


/*
 * umount - Unmount SD Card FS
 */
void umountCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	UNUSED(params);
	FRESULT rc;
	rc = f_mount(0, 0);
	memset(&fatFS, 0, sizeof(fatFS));
}


/*
 * ls - List directory
 */
void lsCbk(unsigned int numParams, void** params)
{
	FRESULT rc;		/* Result code */
	DIR dir;		/* Directory object */
	FILINFO fno;	/* File information object */

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
				printf("<dir>\t %s\r\n", fno.fname);
			else
				printf("%8lu %s\r\n", fno.fsize, fno.fname);
		}
	}
}


/*
 * cd - Change directory
 */
cliParam_t cdParams[] = {
	{ "path", CLI_PARAM_TYPE_STRING }};

void cdCbk(unsigned int numParams, void** params)
{
	f_chdir(PARAM_STRING(params[0]));
}


/*
 * cat - Print out file contents
 */
cliParam_t catParams[] = {
	{ "file", CLI_PARAM_TYPE_STRING }};

void catCbk(unsigned int numParams, void** params)
{
	FIL fileObj;			/* File object */
	FRESULT rc;				/* Result code */
	unsigned char buffer;	/* Working buffer */
	UINT br;

	rc = f_open(&fileObj, PARAM_STRING(params[0]), FA_READ);
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
}





/* Called at startup to register commands */
void registerMiscCmds(void)
{
	cliRegisterCmd("printRTStats", printRunTimeStatsCbk, 0, 0);
	cliRegisterCmd("runMemTests", runMemTestsCbk, 0, 0);
	cliRegisterCmd("mount", mountCbk, 0, 0);
	cliRegisterCmd("umount", umountCbk, 0, 0);
	cliRegisterCmd("ls", lsCbk, 0, 0);
	cliRegisterCmd("cd", cdCbk, sizeof(cdParams)/sizeof(cliParam_t), cdParams);
	cliRegisterCmd("cat", catCbk, sizeof(catParams)/sizeof(cliParam_t), catParams);
}
