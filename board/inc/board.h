/*
 * @brief Embedded Artists LPC1788 and LPC4088 Development Kit board file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
    out of the code at build time.
 */
#define DEBUG_ENABLE

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
    semihosting with this option.
 */
// #define DEBUG_SEMIHOSTING

/** Board UART used for debug output and input using the DEBUG* macros. This
    is also the port used for Board_UARTPutChar, Board_UARTGetChar, and
    Board_UARTPutSTR functions.
 */
#define DEBUG_UART LPC_UART0

/* Board name */
#define BOARD_LPC1788

#define FRAMEBUFFER_ADDR        EMC_ADDRESS_DYCS0	/*!< Frame buffer address for LCD */

#define I2CDEV_UDA1380_ADDR     0x1A/*!< UDA1380 address */
#define UDA1380_I2C_BUS         I2C0/**< I2C Bus to which UDA1380 is connected */

/**
 * LED defines
 */
#define LEDS_LED1           0x01
#define LEDS_LED2           0x02
#define LEDS_LED3           0x04
#define LEDS_LED4           0x08
#define LEDS_NO_LEDS        0x00

/**
 * Button defines
 */
#define BUTTONS_BUTTON1     0x01
#define NO_BUTTON_PRESSED   0x00

/**
 * Joystick defines
 */
#define JOY_UP              0x01
#define JOY_DOWN            0x02
#define JOY_LEFT            0x04
#define JOY_RIGHT           0x08
#define JOY_PRESS           0x10

/**
 * LCD configuration data
 */
extern const LCD_CONFIG_T NHD_43_480x272;

/**
 * Default LCD configuration data for examples
 */
#define BOARD_LCD NHD_43_480x272

/**
 * @brief	Initialize pin muxing for a UART
 * @param	pUART	: Pointer to UART register block for UART pins to init
 * @return	Nothing
 */
void Board_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	Initialize pin muxing for SSP interface
 * @param	pSSP	: Pointer to SSP interface to initialize
 * @return	Nothing
 */
void Board_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @brief	Sets up board specific I2C interface
 * @param	id	: I2C peripheral ID (I2C0, I2C1 or I2C2)
 * @return	Nothing
 */
void Board_I2C_Init(I2C_ID_T id);

/**
 * @brief	Initialize LCD Interface
 * @return	Nothing
 */
void Board_LCD_Init(void);

/**
 * @brief	Initialize the LCD controller on the QVGA (320x240) TFT LCD
 * @return	Nothing
 */
void Board_InitLCDController(void);

/**
 * @brief	Initialize touchscreen controller
 * @return	Nothing
 */
void Board_InitTouchController(void);

/**
 * @brief	Get Touch coordinates
 * @param	pX	: Pointer to x-Coord to populate
 * @param	pY	: Pointer to y-Coord to populate
 * @return	true if touch is detected or false if otherwise
 */
bool Board_GetTouchPos(int16_t *pX, int16_t *pY);

/**
 * @brief	Set LCD Backlight
 * @param   Intensity   : Intensity value to be set in LCD
 * @return	Nothing
 */
void Board_SetLCDBacklight(uint8_t Intensity);

/**
 * @brief	Initialize buttons on the board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief	Get button status
 * @return	status of button
 */
uint32_t Buttons_GetStatus(void);

/**
 * @brief	Initialize Joystick
 * @return	Nothing
 */
void Board_Joystick_Init(void);

/**
 * @brief	Get Joystick status
 * @return	status of Joystick
 */
uint8_t Joystick_GetStatus(void);

/**
 * @brief	Create Serial Stream
 * @param	Stream	: Pointer to stream
 * @return	Nothing
 */
void Serial_CreateStream(void *Stream);

/**
 * @brief	Sets up board specific SDC peripheral
 * @return	Nothing
 */
void Board_SDC_Init(void);

/**
 * @brief	Initializes USB device mode pins per board design
 * @param	port	: USB port to be enabled 
 * @return	Nothing
 * @note	Only one of the USB port can be enabled at a given time.
 */
void Board_USBD_Init(uint32_t port);

#include "board_api.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
