/*
 * cliCommands.c
 *
 *  Created on: Jun 6, 2014
 *      Author: root
 */

#include "cliCommands.h"
#include "cliParser.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/*
 *
 * cls - Clear screen
 *
 */

void clsCmdCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	UNUSED(params);
    printf("\33[2J"); // Clear screen
    printf("\x1b[H"); // Home cursor
}


/*
 * md - Memory dump
 */
cliParam_t mdParams[] = {
	{ "addr", CLI_PARAM_TYPE_POINTER },
	{ "count", CLI_PARAM_TYPE_UINT }};

void mdCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	unsigned int addr = PARAM_UINT(params[0]);
	unsigned int count = PARAM_UINT(params[1]);
	unsigned char numBlanks = addr % MD_PRINT_BYTES_PER_ROW;

	if(numBlanks)
	{
		printf("\r\n[0x%.4x]  ", addr - numBlanks);
		for(; numBlanks > 0; numBlanks--)
			printf(".. ");
	}

	for(; count > 0; count--)
	{
		if(!(addr % MD_PRINT_BYTES_PER_ROW))
			printf("\r\n[0x%0.8x]  ", addr);
		printf("%.2x ", *((char*)(addr++)));
	}
	printf("\r\n");
}


/*
 * mset - Memory set
 */
cliParam_t msetParams[] = {
	{ "addr", CLI_PARAM_TYPE_POINTER },
	{ "value", CLI_PARAM_TYPE_UINT },
	{ "count", CLI_PARAM_TYPE_UINT }};

void msetCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	void* addr = (void*)PARAM_UINT(params[0]);
	unsigned char value = PARAM_SCHAR(params[1]);
	unsigned int count = PARAM_UINT(params[2]);
	memset(addr, value, count);
}


/*
 * mcpy - Memory copy
 */
cliParam_t mcpyParams[] = {
	{ "dest", CLI_PARAM_TYPE_POINTER },
	{ "source", CLI_PARAM_TYPE_POINTER },
	{ "count", CLI_PARAM_TYPE_UINT }};

void mcpyCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	void* dst = (void*)PARAM_UINT(params[0]);
	void* src = (void*)PARAM_UINT(params[1]);
	unsigned int count = PARAM_UINT(params[2]);
	memcpy(dst, src, count);
}

/* TODO: Needs work */
/*
 * mm - Memory modify
 */
cliParam_t mmParams[] = { { "addr", CLI_PARAM_TYPE_POINTER } };

void mmCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	unsigned int addr = PARAM_UINT(params[0]);
	unsigned int value;
	bool exit = false;
	do
	{
		printf("\r\n[0x%0.8x] = 0x%0.2x: ", addr, *((char*)addr));
		if(scanf("%x", &value))
		{
			value &= 0xff;
			*((char*)addr) = value;
			addr++;
		}
		else
			exit = true;
	}while(!exit);
	printf("\r\n");
}

#include "board.h"
void colorBarsCbk(unsigned int numParams, void** params)
{
	// 480x272 RGB
	UNUSED(numParams);
	UNUSED(params);
	int i, j;
	int* addr;

	if(LPC_LCD->UPBASE = 0xa0000000)
		addr = (int*)0xa007f800;
	else
		addr = (int*)0xa0000000;

	for(i = 0; i < 272; i++)
	{
		for(j = 0; j < 480; j++)
		{
			if(j < 60)
				*addr = 0x0ffffff0;
			else if(j < 120)
				*addr = 0x000ffff0;
			else if(j < 180)
				*addr = 0x0ffff000;
			else if(j < 240)
				*addr = 0x000ff000;
			else if(j < 300)
				*addr = 0x0ff00ff0;
			else if(j < 360)
				*addr = 0x00000ff0;
			else if(j < 420)
				*addr = 0x0ff00000;
			else
				*addr = 0x00000000;
			addr++;
		}
	}

	if(LPC_LCD->UPBASE = 0xa0000000)
		LPC_LCD->UPBASE = 0xa007f800;
	else
		LPC_LCD->UPBASE = 0xa0000000;

	//printf("Filled 0xa0000000-0x%x\r\n", addr);
}

cliParam_t testParams[] = {
	{ "x", CLI_PARAM_TYPE_UINT },
	{ "y", CLI_PARAM_TYPE_UINT },
	{ "width", CLI_PARAM_TYPE_UINT },
	{ "height", CLI_PARAM_TYPE_UINT },
	{ "color", CLI_PARAM_TYPE_POINTER } };
void testCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	unsigned int x = PARAM_UINT(params[0]);
	unsigned int y = PARAM_UINT(params[1]);
	unsigned int width = PARAM_UINT(params[2]);
	unsigned int height = PARAM_UINT(params[3]);
	unsigned int color = PARAM_UINT(params[4]);
	unsigned int* addr;
	unsigned int i, j;

	if(LPC_LCD->UPBASE = 0xa0000000)
		addr = (int*)0xa007f800;
	else
		addr = (int*)0xa0000000;

	for(i = 0; i < height; i++)
	{
		for(j = 0; j < width; j++)
		{
			*(unsigned int*)(addr + ((y + i) * 480) + x + j) = color;
		}
	}

	LPC_LCD->UPBASE = addr;
}

cliParam_t fillParams[] = { { "value", CLI_PARAM_TYPE_POINTER } };
void fillCbk(unsigned int numParams, void** params)
{
	UNUSED(numParams);
	unsigned int value = PARAM_UINT(params[0]);
	unsigned int i;
	unsigned int* addr;

	if(LPC_LCD->UPBASE = 0xa0000000)
		addr = (int*)0xa007f800;
	else
		addr = (int*)0xa0000000;

	for(i = 0; i < (480 * 272); i++)
		*(addr++) = value;

	if(LPC_LCD->UPBASE = 0xa0000000)
		LPC_LCD->UPBASE = 0xa007f800;
	else
		LPC_LCD->UPBASE = 0xa0000000;
}

void cliRegisterExtraCommands(void)
{
	cliRegisterCmd("cls", clsCmdCbk, 0, 0);
	cliRegisterCmd("md", mdCbk, sizeof(mdParams)/sizeof(cliParam_t), mdParams);
	cliRegisterCmd("mset", msetCbk, sizeof(msetParams)/sizeof(cliParam_t), msetParams);
	cliRegisterCmd("mcpy", mcpyCbk, sizeof(mcpyParams)/sizeof(cliParam_t), mcpyParams);
	cliRegisterCmd("mm", mmCbk, sizeof(mmParams)/sizeof(cliParam_t), mmParams);
	cliRegisterCmd("colorbars", colorBarsCbk, 0, 0);
	cliRegisterCmd("test", testCbk, sizeof(testParams)/sizeof(cliParam_t), testParams);
	cliRegisterCmd("fill", fillCbk, sizeof(fillParams)/sizeof(cliParam_t), fillParams);
}
