/*
 * AMD Alchemy DB1x00 Reference Boards
 *
 * Copyright 2001 MontaVista Software Inc.
 * Author: MontaVista Software, Inc.
 *         	ppopov@mvista.com or source@mvista.com
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * 
 */
#ifndef __ASM_DB1X00_H
#define __ASM_DB1X00_H


/*
 * Overlay data structure of the Db1x00 board registers.
 * Registers located at physical 1E0000xx, KSEG1 0xAE0000xx
 */
typedef volatile struct
{
	/*00*/	unsigned long whoami;
	/*04*/	unsigned long status;
	/*08*/	unsigned long switches;
	/*0C*/	unsigned long resets;
	/*10*/	unsigned long pcmcia;
	/*14*/	unsigned long specific;
	/*18*/	unsigned long leds;
	/*1C*/	unsigned long swreset;

} BCSR;

/*
 * Register bit definitions for the BCSRs
 */
#define BCSR_WHOAMI_DCID		0x000F
#define BCSR_WHOAMI_CPLD		0x00F0
#define BCSR_WHOAMI_BOARD		0x0F00

#define BCSR_STATUS_PC0VS		0x0003
#define BCSR_STATUS_PC1VS		0x000C
#define BCSR_STATUS_PC0FI		0x0010
#define BCSR_STATUS_PC1FI		0x0020
#define BCSR_STATUS_FLASHBUSY	0x0100
#define BCSR_STATUS_ROMBUSY		0x0400
#define BCSR_STATUS_SWAPBOOT	0x2000
#define BCSR_STATUS_FLASHDEN	0xC000

#define BCSR_SWITCHES_DIP		0x00FF
#define BCSR_SWITCHES_DIP_1		0x0080
#define BCSR_SWITCHES_DIP_2		0x0040
#define BCSR_SWITCHES_DIP_3		0x0020
#define BCSR_SWITCHES_DIP_4		0x0010
#define BCSR_SWITCHES_DIP_5		0x0008
#define BCSR_SWITCHES_DIP_6		0x0004
#define BCSR_SWITCHES_DIP_7		0x0002
#define BCSR_SWITCHES_DIP_8		0x0001
#define BCSR_SWITCHES_ROTARY	0x0F00

#define BCSR_RESETS_PHY0		0x0001
#define BCSR_RESETS_PHY1		0x0002
#define BCSR_RESETS_DC			0x0004

#define BCSR_PCMCIA_PC0VPP		0x0003
#define BCSR_PCMCIA_PC0VCC		0x000C
#define BCSR_PCMCIA_PC0DRVEN	0x0010
#define BCSR_PCMCIA_PC0RST		0x0080
#define BCSR_PCMCIA_PC1VPP		0x0300
#define BCSR_PCMCIA_PC1VCC		0x0C00
#define BCSR_PCMCIA_PC1DRVEN	0x1000
#define BCSR_PCMCIA_PC1RST		0x8000

#define BCSR_BOARD_PCIM66EN		0x0001
#define BCSR_BOARD_PCIM33		0x0100
#define BCSR_BOARD_GPIO200RST	0x0400
#define BCSR_BOARD_PCICFG		0x1000

#define BCSR_LEDS_DECIMALS		0x0003
#define BCSR_LEDS_LED0			0x0100
#define BCSR_LEDS_LED1			0x0200
#define BCSR_LEDS_LED2			0x0400
#define BCSR_LEDS_LED3			0x0800

#define BCSR_SWRESET_RESET		0x0080



/* PCMCIA PB1500 specific defines */
#define PCMCIA_MAX_SOCK 0

/* VPP/VCC */
#define SET_VCC_VPP(VCC, VPP) (((VCC)<<2) | ((VPP)<<0))


#if 0

/* PCMCIA DB1000 specific defines */
#define PCMCIA_MAX_SOCK 1 /* the second socket, 1, is not supported at this time */
#define PCMCIA_NUM_SOCKS (PCMCIA_MAX_SOCK+1)


#define DB1000_PCR     0xAE000010
  #define PCR_SLOT_0_VPP0  (1<<0)
  #define PCR_SLOT_0_VPP1  (1<<1)
  #define PCR_SLOT_0_VCC0  (1<<2)
  #define PCR_SLOT_0_VCC1  (1<<3)
  #define PCR_SLOT_0_RST   (1<<4)

  #define PCR_SLOT_1_VPP0  (1<<8)
  #define PCR_SLOT_1_VPP1  (1<<9)
  #define PCR_SLOT_1_VCC0  (1<<10)
  #define PCR_SLOT_1_VCC1  (1<<11)
  #define PCR_SLOT_1_RST   (1<<12)

/* BOARD_STATUS Register */
#define DB1000_BSR    0xAE000004
  #define ACR1_SLOT_0_CD1    (1<<0)  /* card detect 1     */
  #define ACR1_SLOT_0_CD2    (1<<1)  /* card detect 2     */
  #define ACR1_SLOT_0_READY  (1<<2)  /* ready             */
  #define ACR1_SLOT_0_STATUS (1<<3)  /* status change     */
  #define ACR1_SLOT_0_VS1    (1<<4)  /* voltage sense 1   */
  #define ACR1_SLOT_0_VS2    (1<<5)  /* voltage sense 2   */
  #define ACR1_SLOT_0_INPACK (1<<6)  /* inpack pin status */
  #define ACR1_SLOT_1_CD1    (1<<8)  /* card detect 1     */
  #define ACR1_SLOT_1_CD2    (1<<9)  /* card detect 2     */
  #define ACR1_SLOT_1_READY  (1<<10) /* ready             */
  #define ACR1_SLOT_1_STATUS (1<<11) /* status change     */
  #define ACR1_SLOT_1_VS1    (1<<12) /* voltage sense 1   */
  #define ACR1_SLOT_1_VS2    (1<<13) /* voltage sense 2   */
  #define ACR1_SLOT_1_INPACK (1<<14) /* inpack pin status */

/* Voltage levels */

/* VPPEN1 - VPPEN0 */
#define VPP_GND ((0<<1) | (0<<0))
#define VPP_5V  ((1<<1) | (0<<0))
#define VPP_3V  ((0<<1) | (1<<0))
#define VPP_12V ((0<<1) | (1<<0))
#define VPP_HIZ ((1<<1) | (1<<0))

/* VCCEN1 - VCCEN0 */
#define VCC_3V  ((0<<1) | (1<<0))
#define VCC_5V  ((1<<1) | (0<<0))
#define VCC_HIZ ((0<<1) | (0<<0))

/* VPP/VCC */
#define SET_VCC_VPP(VCC, VPP, SLOT)\
	((((VCC)<<2) | ((VPP)<<0)) << ((SLOT)*8))

/* HEX LED Register */
#define HEX_LEDS_REG	0xAF000000
#endif

#endif /* __ASM_DB1X00_H */
