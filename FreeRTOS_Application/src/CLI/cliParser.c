/*
 * File:   cliParser.c
 * Author: Ryan Mamone
 *
 * Created on November 11, 2010, 4:31 PM
 */

#include "cliParser.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "cliCommands.h"

#define CLI_CMD_DELIMITERS      " (),\r\n"

typedef struct
{
	/* Command name */
    const char* name;
    /* Callback function for the command */
    cliCmdCbk cbk;
    /* Does the callback return a value */
    bool cbkReturn;
    /* Number of parameters in params array */
    unsigned int numParams;
    /* Array of parameters */
    cliParam_t* params;
}cmdEntry_t;

/* CLI cmd data structs */
static cmdEntry_t cmds[CLI_MAX_CMD_COUNT];
static unsigned char cmdCount = 0;

/* Parser data structs */
static char buffer[CLI_PARSER_BUFFER_SIZE];
static unsigned int bufferIdx = 0;
static unsigned int bufferCnt = 0;


cliReturn_t cliRegisterCmd(const char* name, cliCmdCbk callback, unsigned int numParams, cliParam_t* params)
{
    cliReturn_t retVal = CLI_RETURN_FAIL;

    if(cmdCount >= CLI_MAX_CMD_COUNT)
        retVal = CLI_RETURN_TOO_MANY_CMDS;
    else
    {
    	if(numParams > CLI_MAX_PARAM_COUNT)
       		retVal = CLI_RETURN_TOO_MANY_PARAMS;
		else
		{
			/* Add command to cmds array */
			cmds[cmdCount].name = name;
			cmds[cmdCount].cbk = callback;
			cmds[cmdCount].numParams = numParams;
			cmds[cmdCount].params = params;
			cmdCount++;
			retVal = CLI_RETURN_SUCCESS;
		}
    }
    return retVal;
}

void convertParam(char* paramStr, cliParamType_t type, void* paramVal)
{
	switch(type)
	{
		case CLI_PARAM_TYPE_CHAR:
			sscanf(paramStr, "%c", paramVal);
			break;
		case CLI_PARAM_TYPE_UINT:
			sscanf(paramStr, "%ud", paramVal);
			break;
		case CLI_PARAM_TYPE_SINT:
			sscanf(paramStr, "%d", paramVal);
			break;
		case CLI_PARAM_TYPE_ULONG:
			sscanf(paramStr, "%ul", paramVal);
			break;
		case CLI_PARAM_TYPE_SLONG:
			sscanf(paramStr, "%dl", paramVal);
			break;
		case CLI_PARAM_TYPE_FLOAT:
			sscanf(paramStr, "%f", paramVal);
			break;
		case CLI_PARAM_TYPE_POINTER:
			sscanf(paramStr, "%x", paramVal);
			break;
		case CLI_PARAM_TYPE_STRING:
			*((char**)paramVal) = paramStr;
			break;
		default:
			break;
	}
}

cliReturn_t cliParseInputChar(char c)
{
    cliReturn_t retVal = CLI_RETURN_STILL_PARSING;
    static char state = 0; // state machine for escape characters

    if(0x8 == c) // backspace
    {
        if(0 < bufferIdx) // Make sure there is something in the buffer
        {
            buffer[--bufferIdx] = 0;
            //printf("\x1b\x5b\x44\x1b[K"); // Move left one character and erase end of line
            printf("\x1b\x5b\x44 \x1b\x5b\x44");
        }
    }
    else if(0x1B == c) // esc
    {
        state = c;
    }
    else if(0x1B == state) // '['
    {
        state = c;
    }
    else if(0x5B == state) // final character of term escape seq
    {
        switch(c)
        {
            case 0x41: // up
                break;
            case 0x42: // down
                break;
            case 0x43: // right
                if(bufferIdx < bufferCnt)
                {
                    bufferIdx++;
                    printf("\x1b\x5b\x43");
                }
                state = 0;
                break;
            case 0x44: // left
                if(0 < bufferIdx)
                {
                    bufferIdx--;
                    printf("\x1b\x5b\x44");
                }
                state = 0;
                break;
            default:
                state = 0;
        }
    }
    else //if((0x20 >= c) || (c <= 0x7E) || (0x0d == c) || (0x0a == c)) // printable ascii character or return
    {
        if((CLI_PARSER_BUFFER_SIZE - 1 > bufferIdx) && CLI_TERM_CHARACTER != c) /* Space available in receive buffer? */
        {
        	putchar(c); // echo back character
        	buffer[bufferIdx] = c; // store character in buffer
        	/* If current index is at the end increment buffer count */
        	if(bufferIdx == bufferCnt)
        		bufferCnt++;
        	bufferIdx++;
        }

        if(CLI_TERM_CHARACTER == c) /* Terminating character */
        {
            unsigned char currCmdIdx = 0;

            /* Replace terminating character with null in order
             * to properly terminate c string */
            buffer[bufferCnt] = '\0';
            printf("\r\n"); // Move terminal cursor to the next line

            if(0 == bufferIdx)
            {
                retVal = CLI_RETURN_NULL_CMD;
            }
            else
            {
                /* Find corresponding registered command */
                /* NOTE: Eval must be short circuiting */
                while((currCmdIdx < cmdCount) && (0 == strstr(buffer, cmds[currCmdIdx].name)))
                {
                    currCmdIdx++;
                }

                if(currCmdIdx == cmdCount)
                {
                    /* Reached end of cmd list (i.e. command not found) */
                    retVal = CLI_RETURN_CMD_NOT_FOUND;
                }
                else
                {
                    /* Command found at index currCmdIdx in cmd list */
                    if(cmds[currCmdIdx].params)
                    {
                    	char* paramStr;
                    	/* Reserve enough memory for worst case (long) parameter size
                    	 * NOTE: Data not necessarily float type or size */
                    	long paramData[cmds[currCmdIdx].numParams];
                    	void* paramPtrs[cmds[currCmdIdx].numParams];
                    	unsigned char i;

						/* Tokenize the command name moving the ptr to the first param */
						paramStr = strtok(buffer, " (,");

						/* Parse all the parameters */
						for(i = 0; i < cmds[currCmdIdx].numParams; i++)
						{
							paramStr = strtok(NULL, CLI_CMD_DELIMITERS);

							if(0 == paramStr)
								retVal = CLI_RETURN_TOO_FEW_PARAMS;
							else
							{
								paramPtrs[i] = (void*)&paramData[i];
								convertParam(paramStr, cmds[currCmdIdx].params[i].type, paramPtrs[i]);
							}
						}

						/* Check for extra params
						 * NOTE: check not required */
						if(strtok(NULL, CLI_CMD_DELIMITERS))
							retVal = CLI_RETURN_TOO_MANY_PARAMS;

						if(CLI_RETURN_STILL_PARSING == retVal)
						{
							cmds[currCmdIdx].cbk(cmds[currCmdIdx].numParams, paramPtrs);
							retVal = CLI_RETURN_CMD_EXECUTED;
						}
                    }
                    else /* No parameters supplied */
                    {
                    	if(CLI_RETURN_STILL_PARSING == retVal)
						{
							cmds[currCmdIdx].cbk(0, 0);
							retVal = CLI_RETURN_CMD_EXECUTED;
						}
                    }
                }
                memset(buffer, 0, sizeof(buffer));
                bufferIdx = 0;
                bufferCnt = 0;
                state = 0;
            }
        }
    }
    return retVal;
}

/* Must match cliParamType_t enum */
static const char* paramDescrString[] = {"none",
                                         "unsigned char",
                                         "char",
                                         "unsigned int",
                                         "int",
                                         "unsigned long",
                                         "long",
                                         "float",
                                         "void *"};

void helpCmdCbk(unsigned int numParams, void** params)
{
    int i;
    UNUSED(numParams);
    UNUSED(params);

    printf("Registered Commands\r\n");
    printf("-------------------\r\n");

    for(i = 0; i < cmdCount; i++)
    {
    	unsigned int j;
    	printf("%s(", cmds[i].name);
        if(cmds[i].params)
        {
			for(j = 0; j < cmds[i].numParams; j++)
			{
				printf("%s %s", paramDescrString[cmds[i].params[j].type],
						cmds[i].params[j].name);
				if(j != cmds[i].numParams - 1)
					printf(", ");
			}
        }
        printf(")\r\n");
    }
}

void cliInit(void)
{
	cliRegisterCmd("help", helpCmdCbk, 0, 0);
	cliRegisterExtraCommands();
}

void cliRunLoop(void)
{
	putchar(CLI_DEFAULT_PROMPT);
	while(1)
	{
		cliReturn_t ret = cliParseInputChar(getchar());

		switch(ret)
		{
			case CLI_RETURN_CMD_NOT_FOUND:
				printf("Invalid command\r\n");
				break;
			case CLI_RETURN_TOO_FEW_PARAMS:
				printf("Too few parameters\r\n");
				break;
			case CLI_RETURN_TOO_MANY_PARAMS:
				printf("Too many parameters\r\n");
				break;
			default:
				break;
		}

		if(CLI_RETURN_STILL_PARSING != ret)
			putchar(CLI_DEFAULT_PROMPT);
	}
}
