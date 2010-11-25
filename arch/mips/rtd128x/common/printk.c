/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#include <linux/init.h>
#include <asm/io.h>
#include <rtd128x-io.h>

#define UART_LSR_TEMT		0x40	/* Transmitter empty */
#define UART_LSR_THRE		0x20	/* Transmit-hold-register empty */

static void __init wait_xfered(void)
{
	unsigned int val;

	/* wait for any previous char to be transmitted */
	do {
		val = inl(RTD128X_UART_U0LSR);
		if ((val & (UART_LSR_TEMT | UART_LSR_THRE)) ==
		    (UART_LSR_TEMT | UART_LSR_THRE))
			break;
	} while (1);
}

void __init prom_putchar(char c)
{
	wait_xfered();
	outl(c, RTD128X_UART_U0RBR_THR_DLL);
	wait_xfered();
}
