/*
 * LPC1788 Board
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

	/* USB Device */
	{0, 31, (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* USB_D+2 NOTE: USB_D-2 is not muxed */
	{0, 13, (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* USB_UP_LED2 */
	{0, 14, (IOCON_FUNC3 | IOCON_MODE_INACT)}, /* USB_CONNECT2 */
	//{1, 30, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* USB_VBUS */

	/* SPI Flash */
	{0, 15, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SPI_FLASH_SCK */
	{0, 16, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SPI_FLASH_CS */
	{0, 17, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SPI_FLASH_MISO */
	{0, 18, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SPI_FLASH_MOSI */

	/* SD Card */
	{0, 19, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_CLK */
	{0, 20, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_CMD */
	{0, 22, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_DAT[0] */
	{1, 7,  (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_DAT[1] */
	{1, 11, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_DAT[2] */
	{1, 12, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* SD_DAT[3] */
	{2, 15, (IOCON_FUNC0 | IOCON_MODE_INACT)}, /* SD_DET */

	/* DAC */
	{0, 26, (IOCON_FUNC2 | IOCON_DAC_EN | IOCON_HYS_EN | IOCON_MODE_PULLUP)}, /* DAC_OUT */

	/* External Interrupts */
	/* TODO: check special pad func */
	{0, 29, (IOCON_FUNC2 | IOCON_MODE_INACT)}, /* KEY_INT */

	/* External Key Interface */
	{5, 2,  (IOCON_FUNC5 | IOCON_MODE_INACT)}, /* KEY_I2C_SDA */
	{5, 3,  (IOCON_FUNC5 | IOCON_MODE_INACT)}, /* KEY_I2C_SCL */
	{2, 14, (IOCON_FUNC0 | IOCON_MODE_INACT)}, /* TOUCH_INT */

	/* Expansion Header */
	/* Exp. SPI */
	{1, 0,  (IOCON_FUNC4 | IOCON_MODE_INACT)}, /* SPI_SCK */
	{1, 8,  (IOCON_FUNC4 | IOCON_MODE_INACT)}, /* SPI_CS */
	{1, 1,  (IOCON_FUNC4 | IOCON_MODE_INACT)}, /* SPI_MOSI */
	{1, 4,  (IOCON_FUNC4 | IOCON_MODE_INACT)}, /* SPI_MISO */
	/* Exp. UART */
	{0, 10, (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* EXT_UART_TX */
	{0, 11, (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* EXT_UART_RX */
	/* Exp. I2C */
	/* TODO: look at special pad func Section 22.1 */
	{0, 27,	(IOCON_FUNC1 | IOCON_MODE_INACT)}, /* SDA */
	{0, 28, (IOCON_FUNC1 | IOCON_MODE_INACT)}, /* SCL */
	/* Exp. GPIO */
	{3, 31, (IOCON_FUNC0 | IOCON_MODE_INACT)}, /* GPIO 1 */
	{0, 23, (IOCON_FUNC0 | IOCON_MODE_INACT)}, /* GPIO 2 */
	{5, 1,  (IOCON_FUNC0 | IOCON_MODE_INACT)}, /* GPIO 3 */
	{5, 0,  (IOCON_FUNC0 | IOCON_MODE_INACT)}, /* GPIO 4 */
	/* -- Expansion Header -- */

	/* SDRAM */
	{4, 25, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* WE */
	{2, 16, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* CAS */
	{2, 17, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* RAS */
	{2, 18, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* CLK */
	{2, 20, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* DYCS0 */
	{2, 24, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* CKE */
	{2, 28, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* DQM0 */
	{2, 29, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* DQM1 */
	{3, 0,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D0 */
	{3, 1,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D1 */
	{3, 2,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D2 */
	{3, 3,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D3 */
	{3, 4,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D4 */
	{3, 5,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D5 */
	{3, 6,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D6 */
	{3, 7,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D7 */
	{3, 8,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D8 */
	{3, 9,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D9 */
	{3, 10, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D10 */
	{3, 11, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D11 */
	{3, 12, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D12 */
	{3, 13, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D13 */
	{3, 14, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D14 */
	{3, 15, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* D15 */
	{4, 0,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A0 */
	{4, 1,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A1 */
	{4, 2,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A2 */
	{4, 3,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A3 */
	{4, 4,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A4 */
	{4, 5,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A5 */
	{4, 6,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A6 */
	{4, 7,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A7 */
	{4, 8,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A8 */
	{4, 9,  (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A9 */
	{4, 10, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A10 */
	{4, 11, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A11 */
	{4, 12, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A12 */
	{4, 13, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A13 */
	{4, 14, (IOCON_FUNC1 | IOCON_FASTSLEW_EN)}, /* A14 */

#if 0
	/* LCD */
	{2, 0,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* LCD_PWR */
	{2, 1,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* LCD_LE */
	{2, 2,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* LCD_DCLK */
	{2, 3,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* LCD_FP */
	{2, 4,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* LCD_ENAB_M */
	{2, 5,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* LCD_LP */
	{0, 4,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* R0 */
	{0, 5,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* R1 */
	{2, 8,  ( | IOCON_MODE_INACT)}, /* R2 */
	{2, 9,  ( | IOCON_MODE_INACT)}, /* R3 */
	{2, 6,  ( | IOCON_MODE_INACT)}, /* R4 */
	{2, 7,  ( | IOCON_MODE_INACT)}, /* R5 */
	{1, 20, ( | IOCON_MODE_INACT)}, /* R6 */
	{1, 21, ( | IOCON_MODE_INACT)}, /* R7 */
	{0, 6,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* G0 */
	{0, 7,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* G1 */
	{4, 28, ( | IOCON_MODE_INACT)}, /* G2 */
	{4, 29, ( | IOCON_MODE_INACT)}, /* G3 */
	{1, 22, ( | IOCON_MODE_INACT)}, /* G4 */
	{1, 23, ( | IOCON_MODE_INACT)}, /* G5 */
	{1, 24, ( | IOCON_MODE_INACT)}, /* G6 */
	{1, 25, ( | IOCON_MODE_INACT)}, /* G7 */
	{0, 8,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* B0 */
	{0, 9,  (IOCON_FUNC7 | IOCON_MODE_INACT)}, /* B1 */
	{2, 12, ( | IOCON_MODE_INACT)}, /* B2 */
	{2, 13, ( | IOCON_MODE_INACT)}, /* B3 */
	{1, 26, ( | IOCON_MODE_INACT)}, /* B4 */
	{1, 27, ( | IOCON_MODE_INACT)}, /* B5 */
	{1, 28, ( | IOCON_MODE_INACT)}, /* B6 */
	{1, 29, ( | IOCON_MODE_INACT)}, /* B7 */

	{1, 2,  (IOCON_FUNC0 | IOCON_MODE_INACT)}, /* LED_CTRL */
#endif
};

/* EMC clock delay */
#define CLK0_DELAY 7

/* SDRAM timing and chip Config for AS4C8M16S-7 */
STATIC const IP_EMC_DYN_CONFIG_T AS4C8M16S_config = {
	EMC_NANOSECOND(64000000 / 4096), /* 4096 refresh cycles per 64ms */
	1,					/* Command Delayed */ /* TODO: Check value */
	3,					/* tRP */
	7,					/* tRAS */
	EMC_NANOSECOND(70),	/* tSREX */
	EMC_CLOCK(1),		/* tAPR */ /* TODO: Check value */
	EMC_CLOCK(5),		/* tDAL */
	EMC_NANOSECOND(20),	/* tWR */
	EMC_NANOSECOND(63),	/* tRC */
	EMC_NANOSECOND(63),	/* tRFC */ /* TODO: Check value */
	EMC_NANOSECOND(65),	/* tXSR */
	EMC_NANOSECOND(14),	/* tRRD */
	EMC_CLOCK(2),		/* tMRD */
	{
		{
			EMC_ADDRESS_DYCS0,
			2,	/* RAS */
			EMC_DYN_MODE_WBMODE_PROGRAMMED |
			EMC_DYN_MODE_OPMODE_STANDARD |
			EMC_DYN_MODE_CAS_2 |
			EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
			EMC_DYN_MODE_BURST_LEN_4,

			EMC_DYN_CONFIG_DATA_BUS_16 |
			EMC_DYN_CONFIG_LPSDRAM | /* TODO: Check if Bank, Row, Column */
			EMC_DYN_CONFIG_8Mx16_4BANKS_12ROWS_9COLS |
			EMC_DYN_CONFIG_MD_SDRAM
		},
		{0, 0, 0, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	}
};

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
	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_EMC);
	Chip_SYSCTL_PeriphReset(SYSCTL_RESET_IOCON);

	/* Setup system level pin muxing */
	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
}

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
	Chip_EMC_Dynamic_Init((IP_EMC_DYN_CONFIG_T *) &AS4C8M16S_config);

	/* EMC Shift Control */
	LPC_SYSCTL->SCS |= 1;
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	Board_SetupMuxing();
	Board_SetupClocking();
	//Board_SetupExtMemory();
}
