/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <asm/time.h>

#include <rtd128x-board.h>
#include <rtd128x-soc.h>
#include <rtd128x-irq.h>
#include <rtd128x-io.h>

/*
 * rtd128x uart
 */

static unsigned int rtd128x_serial_in(struct uart_port *p, int offset)
{
	offset <<= p->regshift;
	return readl(p->membase + offset) & 0xff;
}

static void rtd128x_serial_out(struct uart_port *p, int offset, int value)
{
	offset <<= p->regshift;
	writel(value & 0xff, p->membase + offset);
}

static struct plat_serial8250_port rtd128x_serial_data[] = {
	[0] = {},
	[1] = {},
	{}
};

static struct platform_device rtd128x_serial8250_device = {
	.name = "serial8250",
	.id = PLAT8250_DEV_PLATFORM,
	.dev = {
		.platform_data = rtd128x_serial_data,
		},
};

static void __init rtd128x_register_uart(void)
{
	int n = 0;

	if (rtd128x_board_info.has_uart0) {
		rtd128x_serial_data[n].iobase = RTD128X_UART0_BASE;
		rtd128x_serial_data[n].membase =
		    (unsigned char __iomem *)KSEG1ADDR(RTD128X_IO_PORT_BASE +
						       RTD128X_UART0_BASE);
		rtd128x_serial_data[n].mapbase =
		    RTD128X_IO_PORT_BASE + RTD128X_UART0_BASE;
		rtd128x_serial_data[n].irq = RTD128X_IRQ_UART0;
		rtd128x_serial_data[1].uartclk = rtd128x_board_info.ext_freq;
		rtd128x_serial_data[n].iotype = UPIO_MEM;
		rtd128x_serial_data[n].flags = UPF_BOOT_AUTOCONF;
		rtd128x_serial_data[n].regshift = 2;
		rtd128x_serial_data[n].serial_in = &rtd128x_serial_in;
		rtd128x_serial_data[n].serial_out = &rtd128x_serial_out;
		n++;
	}

	if (rtd128x_board_info.has_uart1) {
		rtd128x_serial_data[n].iobase = RTD128X_UART1_BASE;
		rtd128x_serial_data[n].membase =
		    (unsigned char __iomem *)KSEG1ADDR(RTD128X_IO_PORT_BASE +
						       RTD128X_UART1_BASE);
		rtd128x_serial_data[n].mapbase =
		    RTD128X_IO_PORT_BASE + RTD128X_UART1_BASE;
		rtd128x_serial_data[n].irq = RTD128X_IRQ_UART1;
		rtd128x_serial_data[0].uartclk = rtd128x_board_info.ext_freq;
		rtd128x_serial_data[n].iotype = UPIO_MEM;
		rtd128x_serial_data[n].flags = UPF_BOOT_AUTOCONF;
		rtd128x_serial_data[n].regshift = 2;
		rtd128x_serial_data[n].serial_in = &rtd128x_serial_in;
		rtd128x_serial_data[n].serial_out = &rtd128x_serial_out;
		n++;
	}

	if (n)
		platform_device_register(&rtd128x_serial8250_device);
}

/*
 * platform and device init
 */

void __init platform_init(void)
{
}

static int __init rtd128x_devinit(void)
{
	rtd128x_register_uart();
	return 0;
}

device_initcall(rtd128x_devinit);
