/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#include <linux/interrupt.h>
#include <asm/io.h>

#include <rtd128x-io.h>
#include <rtd128x-irq.h>

static irqreturn_t rtd128x_sb2_irq_handler(int irq, void *dev_id)
{
	unsigned int addr;

	if (!(inl(RTD128X_SB2_INV_INTSTAT) & 0x2))
		return IRQ_NONE;

	/* 
	 * The seems to be a problem on Mars with prefetching
	 * specific memory regions. This patch should circumvent
	 * this bug.
	 */
	addr = inl(RTD128X_SB2_INV_ADDR);
	if (addr > 0x8001000 && ((addr & 0xfffff000) != 0x1800c000)) {
		printk("Access to invalid hw address 0x%x\n", addr);
	}
	outl(0xE, RTD128X_SB2_INV_INTSTAT);

	return IRQ_HANDLED;
}

static struct irqaction rtd128x_sb2_irq_action = {
	.handler = rtd128x_sb2_irq_handler,
	.flags = IRQF_SHARED,
	.name = "rtd128x-sb2",
};

void __init rtd128x_sb2_setup(void)
{
	outl(0x3, RTD128X_SB2_INV_INTEN);
	outl(0xe, RTD128X_SB2_INV_INTSTAT);
	setup_irq(RTD128X_IRQ_SB2, &rtd128x_sb2_irq_action);
}
