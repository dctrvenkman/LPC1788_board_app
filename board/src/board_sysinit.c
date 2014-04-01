/*
 * @brief Embedded Artists LPC1788 Sysinit file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
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

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   includes clock setup and default pin muxing configuration. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Pin muxing configuration */
STATIC const PINMUX_GRP_T pinmuxing[] = {
	/* Console */
	{0, 0,  (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* CONSOLE_TX */
	{0, 1,  (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* CONSOLE_RX */

	/* LCD */
	{0, 4,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* R0 */
	{0, 5,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* R1 */
	{0, 6,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* G0 */
	{0, 7,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* G1 */
	{0, 8,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* B0 */
	{0, 9,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* B1 */

	/* Ext. UART */
	{0, 10, (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* EXT_UART_TX */
	{0, 11, (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* EXT_UART_RX */

	/* USB Device */
	{0, 13, (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* USB_UP_LED2 */
	{0, 14, (IOCON_FUNC3 | IOCON_MODE_INACT)}, /* USB_CONNECT2 */

	/* SPI Flash */
	{0, 15, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SPI_FLASH_SCK */
	{0, 16, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SPI_FLASH_CS */
	{0, 17, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SPI_FLASH_MISO */
	{0, 18, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SPI_FLASH_MOSI */

	/* SD Card */
	{0, 19, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_CLK */
	{0, 20, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_CMD */
	{0, 22, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_DAT[0] */

	/* GPIO */
	{0, 23, (IOCON_FUNC0 | IOCON_MODE_INACT)}, /* GPIO 2 */

	/* DAC */
	{0, 26, (IOCON_FUNC2 | IOCON_DAC_EN | IOCON_HYS_EN | IOCON_MODE_PULLUP)}, /* DAC_OUT */

	/* I2C */
	/* TODO: look at special pad func Section 22.1 */
	{0, 27,  (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* SDA */
	{0, 28,  (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* SCL */


	/* External Interrupts */
	/* TODO: check special pad func */
	{0, 29,  (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* KEY_INT */

	/* USB */
	{0, 31,  (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* USB_D+2 */
	/* NOTE: USB_D-2 is not muxed */


	/* PORT 1 */




};

/* EMC clock delay */
#define CLK0_DELAY 7

#if 0
/* Keil SDRAM timing and chip Config */
STATIC const IP_EMC_DYN_CONFIG_T IS42S32800D_config = {
	EMC_NANOSECOND(64000000 / 4096),
	0x01,				/* Command Delayed */
	3,					/* tRP */
	7,					/* tRAS */
	EMC_NANOSECOND(70),	/* tSREX */
	EMC_CLOCK(0x01),	/* tAPR */
	EMC_CLOCK(0x05),	/* tDAL */
	EMC_NANOSECOND(12),	/* tWR */
	EMC_NANOSECOND(60),	/* tRC */
	EMC_NANOSECOND(60),	/* tRFC */
	EMC_NANOSECOND(70),	/* tXSR */
	EMC_NANOSECOND(12),	/* tRRD */
	EMC_CLOCK(0x02),	/* tMRD */
	{
		{
			EMC_ADDRESS_DYCS0,	/* EA Board uses DYCS0 for SDRAM */
			2,	/* RAS */

			EMC_DYN_MODE_WBMODE_PROGRAMMED |
			EMC_DYN_MODE_OPMODE_STANDARD |
			EMC_DYN_MODE_CAS_2 |
			EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
			EMC_DYN_MODE_BURST_LEN_4,

			EMC_DYN_CONFIG_DATA_BUS_32 |
			EMC_DYN_CONFIG_LPSDRAM |
			EMC_DYN_CONFIG_8Mx16_4BANKS_12ROWS_9COLS |
			EMC_DYN_CONFIG_MD_SDRAM
		},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	}
};
#endif

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Setup system clocking */
void Board_SetupClocking(void)
{
	/* Enable PBOOST for CPU clock over 100MHz */
	Chip_SYSCTL_EnableBoost();

	Chip_SetupXtalClocking();
}

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	int i, j;

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_EMC);
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_IOCON);

#if 0
	/* Setup data, address, and EMC control pins with high slew rate */
	for (i = 3; i <= 4; i++) {
		for (j = 0; j <= 31; j++) {
			Chip_IOCON_PinMuxSet(LPC_IOCON, (uint8_t) i, (uint8_t) j, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
		}
	}
	for (i = 16; i <= 31; i++) {
		Chip_IOCON_PinMuxSet(LPC_IOCON, 2, (uint8_t) i, (IOCON_FUNC1 | IOCON_FASTSLEW_EN));
	}
#endif

	/* Setup system level pin muxing */
	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
}

#if 0
/* Setup external memories */
void Board_SetupExtMemory(void)
{
	/* Setup EMC Delays */
	/* Move all clock delays together */
	LPC_SYSCTL->EMCDLYCTL = (CLK0_DELAY) | (CLK0_DELAY << 8) | (CLK0_DELAY << 16 | (CLK0_DELAY << 24));

	/* Setup EMC Clock Divider for divide by 2 */
	/* Setup EMC clock for a divider of 2 from CPU clock. Enable EMC clock for
	   external memory setup of DRAM. */
	Chip_Clock_SetEMCClockDiv(SYSCTL_EMC_DIV2);
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_EMC);

	/* Init EMC Controller -Enable-LE mode- clock ratio 1:1 */
	Chip_EMC_Init(1, 0, 0);

	/* Init EMC Dynamic Controller */
	Chip_EMC_Dynamic_Init((IP_EMC_DYN_CONFIG_T *) &IS42S32800D_config);

	/* EMC Shift Control */
	LPC_SYSCTL->SCS |= 1;
}
#endif

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	Board_SetupMuxing();
	Board_SetupClocking();
#if 0
	Board_SetupExtMemory();
#endif
}
