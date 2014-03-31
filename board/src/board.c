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

#include "board.h"
#include <string.h>

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* TODO: Touchscreen definitions */


/**
 * LCD configuration data
 */
const LCD_CONFIG_T NHD_43_480x272 = {
	2,		/* Horizontal back porch in clocks */
	2,		/* Horizontal front porch in clocks */
	41,		/* HSYNC pulse width in clocks */
	480,	/* Pixels per line */
	1050/*2*/,	/* Vertical back porch in clocks */
	1050/*2*/,	/* Vertical front porch in clocks */
	5250/*10*/,	/* VSYNC pulse width in clocks */
	272,	/* Lines per panel */
	0,		/* Invert output enable, 1 = invert */
	1,		/* Invert panel clock, 1 = invert */
	1,		/* Invert HSYNC, 1 = invert */
	1,		/* Invert VSYNC, 1 = invert */
	1,		/* AC bias frequency in clocks (not used) */
	6,		/* Maximum bits per pixel the display supports */
	LCD_TFT,				/* LCD panel type */
	LCD_COLOR_FORMAT_RGB,	/* BGR or RGB */
	0		/* Dual panel, 1 = dual panel display */
};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/* System oscillator rate and RTC oscillator rate */
const uint32_t OscRateIn = 12000000;
const uint32_t RTCOscRateIn = 0;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Very simple (inaccurate) delay function */
static void delay(uint32_t i)
{
	while (i--) {}
}

/* Delay in miliseconds  (cclk = 120MHz) */
static void delayMs(uint32_t ms)
{
	delay(ms * 40000);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Initialize UART pins */
void Board_UART_Init(LPC_USART_T *pUART)
{
	if(pUART == LPC_UART0)
	{
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 2, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* U0_TXD */
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 3, (IOCON_FUNC1 | IOCON_MODE_INACT)); /* U0_RXD */
	}
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
	Board_UART_Init(DEBUG_UART);

	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaud(DEBUG_UART, 115200);
	Chip_UART_ConfigData(DEBUG_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);

	/* Enable UART Transmit */
	Chip_UART_TXEnable(DEBUG_UART);
#endif
}

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	while ((Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	if (Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_RDR) {
		return (int) Chip_UART_ReadByte(DEBUG_UART);
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#endif
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	/* Initializes GPIO */
	Chip_GPIO_Init(LPC_GPIO);
	Chip_IOCON_Init(LPC_IOCON);
}

#if 0
/* Initialize LCD Interface */
void Board_LCD_Init(void)
{
	uint8_t i;

	for (i = 4; i <= 9; i++) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, i, (IOCON_FUNC7 | IOCON_MODE_INACT));
	}
	for (i = 20; i <= 29; i++) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 1, i, (IOCON_FUNC7 | IOCON_MODE_INACT));
	}
	for (i = 0; i <= 6; i++) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, i, (IOCON_FUNC7 | IOCON_MODE_INACT));
	}
	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 7, (IOCON_FUNC7 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 8, (IOCON_FUNC7 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 9, (IOCON_FUNC7 | IOCON_MODE_INACT));

	for (i = 11; i <= 13; i++) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, i, (IOCON_FUNC7 | IOCON_MODE_INACT));
	}
	Chip_IOCON_PinMuxSet(LPC_IOCON, 4, 28, (IOCON_FUNC7 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 4, 29, (IOCON_FUNC7 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, LCD_BACKLIGHT_PORTNUM, LCD_BACKLIGHT_PINNUM, (IOCON_FUNC0 | IOCON_MODE_INACT));
	Chip_GPIO_WriteDirBit(LPC_GPIO, LCD_BACKLIGHT_PORTNUM, LCD_BACKLIGHT_PINNUM, true);

	Board_InitLCDController();
}

/* Initialize the LCD controller on the external QVGA (320x240) TFT LCD*/
void Board_InitLCDController(void)
{
	/* SSP pin configuration */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 15, (IOCON_FUNC2 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 16, (IOCON_FUNC2 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 17, (IOCON_FUNC2 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC2 | IOCON_MODE_INACT));
	/*  DC pin */
	Chip_IOCON_PinMuxSet(LPC_IOCON, TSC2046_DC_PORTNUM, TSC2046_DC_PINNUM, (IOCON_FUNC0 | IOCON_MODE_INACT));
	Chip_GPIO_WriteDirBit(LPC_GPIO, TSC2046_DC_PORTNUM, TSC2046_DC_PINNUM, true);

	Chip_SSP_Init(LCD_SSP_CTRL);
	Chip_SSP_SetMaster(LCD_SSP_CTRL, true);
	Chip_SSP_SetBitRate(LCD_SSP_CTRL, 1000000);
	Chip_SSP_SetFormat(LCD_SSP_CTRL, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_MODE0);
	
	Chip_SSP_Enable(LCD_SSP_CTRL);

	delayMs(200);
	/* initialize LCD controller */
	initSSD1289();

	/* Power of SSP inteface */
	Chip_SSP_Disable(LCD_SSP_CTRL);
	Chip_SSP_DeInit(LCD_SSP_CTRL);

	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 15, (IOCON_FUNC0 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 16, (IOCON_FUNC0 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 17, (IOCON_FUNC0 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 18, (IOCON_FUNC0 | IOCON_MODE_INACT));
}

/* Initialize touchscreen controller */
void Board_InitTouchController(void)
{
	uint16_t dummy_data;

	configTSC2046Pins();

	Chip_SSP_Init(LCD_SSP_CTRL);

	Chip_SSP_SetMaster(LCD_SSP_CTRL, true);
	Chip_SSP_SetBitRate(LCD_SSP_CTRL, 200000);
	Chip_SSP_SetFormat(LCD_SSP_CTRL, SSP_BITS_8, SSP_FRAMEFORMAT_SPI, SSP_CLOCK_MODE3);
	Chip_SSP_Enable(LCD_SSP_CTRL);

	/* Enable Touch Screen Controller */
	readWriteTSC2046(PWRDOWN, &dummy_data);
}

/* Get Touch coordinates */
bool Board_GetTouchPos(int16_t *pX, int16_t *pY)
{
	uint16_t tmp;
	uint16_t x, y, z1, z2, z = 0;
	Status Sts = SUCCESS;

	readWriteTSC2046(X_MEASURE, &x);
	readWriteTSC2046(Y_MEASURE, &y);
	readWriteTSC2046(Z1_MEASURE, &z1);
	readWriteTSC2046(Z2_MEASURE, &z2);

	if (z1 != 0) {
		z = x * ((z2 / z1) - 1);
	}
	if ((z <= 0) || (z > 35000)) {
		return false;
	}
	/* Get X-Coordinate */
	Sts = evalTSC2046Coord(X_MEASURE, &x);

	if (Sts == ERROR) {
		return false;
	}
	/* Get Y-Coordinate */
	Sts = evalTSC2046Coord(Y_MEASURE, &y);
	if (Sts == ERROR) {
		return false;
	}
	/* Get Z1-Value */
	Sts = evalTSC2046Coord(Z1_MEASURE, &z1);
	if (Sts == ERROR) {
		return false;
	}
	/* Get Z2-Value */
	Sts = evalTSC2046Coord(Z2_MEASURE, &z2);
	if (Sts == ERROR) {
		return false;
	}

	z = x * ((z2 / z1) - 1);
	if ((z <= 0) || (z > 35000)) {
		return false;
	}
	else {
		/* Swap, adjust to truly size of LCD */
		if (TSC_Config.swap_xy) {
			*pY = calibrateTSC2046(x, TSC_Config.ad_top, TSC_Config.ad_bottom, TSC_Config.lcd_height);
			*pX = calibrateTSC2046(y, TSC_Config.ad_left, TSC_Config.ad_right, TSC_Config.lcd_width);
		}
		else {
			*pX = calibrateTSC2046(x, TSC_Config.ad_top, TSC_Config.ad_bottom, TSC_Config.lcd_width);
			*pY = calibrateTSC2046(y, TSC_Config.ad_left, TSC_Config.ad_right, TSC_Config.lcd_height);
		}
	}
	readWriteTSC2046(PWRDOWN, &tmp);

	return true;
}

/* Turn on Board LCD Backlight */
void Board_SetLCDBacklight(uint8_t Intensity)
{
	bool OnOff = (bool) (Intensity != 0);

	Chip_GPIO_WritePortBit(LPC_GPIO, LCD_BACKLIGHT_PORTNUM, LCD_BACKLIGHT_PINNUM, OnOff);
}

/* Sets up board specific I2C interface */
void Board_I2C_Init(I2C_ID_T id)
{
	switch (id) {
	case I2C0:
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 27, IOCON_FUNC1);
		Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 28, IOCON_FUNC1);
		break;

	case I2C1:
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 14, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 15, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
		break;

	case I2C2:
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 30, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 31, (IOCON_FUNC2 | IOCON_MODE_PULLUP | IOCON_OPENDRAIN_EN));
		break;

	default:
		return;
	}
}

/* Pin mux for Event Monitor/Recorder */
void Board_RTC_EV_Init(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 7, (IOCON_FUNC4 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 8, (IOCON_FUNC4 | IOCON_MODE_INACT));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, (IOCON_FUNC4 | IOCON_MODE_INACT));
}
#endif

/* Create Serial Stream */
void Serial_CreateStream(void *Stream)
{
	/* To be implemented */
}

/* Setup board for SD Card interface */
void Board_SDC_Init(void)
{
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 19, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SD_CLK */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 20, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SD_CMD */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SD_DAT[0] */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 7, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SD_DAT[1] */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 11, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SD_DAT[2] */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 12, (IOCON_FUNC2 | IOCON_MODE_INACT));	/* SD_DAT[3] */
	Chip_IOCON_PinMuxSet(LPC_IOCON, 2, 15, (IOCON_FUNC0 | IOCON_MODE_INACT));	/* SD_DET */
}

void Board_USBD_Init(uint32_t port)
{
	if(port == 2)
	{
		Chip_IOCON_PinMux(LPC_IOCON, 1, 30, IOCON_MODE_INACT, IOCON_FUNC2); /* USB VBUS */
		Chip_IOCON_PinMux(LPC_IOCON, 0, 31, IOCON_MODE_INACT, IOCON_FUNC1);	/* USB_D+2 */
		/* NOTE: USB_D-2 not muxed */
		Chip_IOCON_PinMux(LPC_IOCON, 0, 14, IOCON_MODE_INACT, IOCON_FUNC3); /* USB_CONNECT2 */
		Chip_IOCON_PinMux(LPC_IOCON, 0, 13, IOCON_MODE_INACT, IOCON_FUNC1); /* USB_UP_LED2 */

		LPC_USB->USBClkCtrl = 0x1A; /* Dev, AHB clock enable */
		while ((LPC_USB->USBClkSt & 0x1A) != 0x1A)
			;
		/* Port Select register when USB device is configured. */
		LPC_USB->StCtrl = 0x3;
	}
}
