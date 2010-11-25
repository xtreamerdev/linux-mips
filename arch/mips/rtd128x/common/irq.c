/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <asm/irq_cpu.h>
#include <asm/processor.h>
#include <asm/time.h>

#include <rtd128x-board.h>
#include <rtd128x-io.h>
#include <rtd128x-irq.h>

extern void rtd128x_sb2_setup(void);

static int rtd128x_internal_irq_dispatch(void)
{
	u32 pending;
	static int i;

	pending = inl(RTD128X_MISC_ISR);

	if (!pending)
		return 0;

	while (1) {
		int to_call = i;

		i = (i + 1) & 0x1f;
		if (pending & (1 << to_call)) {
			do_IRQ(to_call + RTD128X_INTERNAL_IRQ_BASE);
			break;
		}
	}
	return 1;
}

void plat_irq_dispatch(void)
{
	u32 cause;

	cause = read_c0_cause() & read_c0_status() & CAUSEF_IP;

	clear_c0_status(cause);

	if (cause & CAUSEF_IP7)
		do_IRQ(7);
	if (cause & CAUSEF_IP2)
		do_IRQ(2);
	if ((cause & CAUSEF_IP3) && (!rtd128x_internal_irq_dispatch()))
		do_IRQ(3);
	if (cause & CAUSEF_IP4)
		do_IRQ(4);
	if (cause & CAUSEF_IP5)
		do_IRQ(5);
	if (cause & CAUSEF_IP6)
		do_IRQ(6);
}

static inline void rtd128x_internal_irq_mask(unsigned int irq)
{
#if 0
	u32 mask;

	irq -= RTD128X_INTERNAL_IRQ_BASE;
	mask = inl(RTD128X_MISC_UMSK_ISR);
	mask &= ~(1 << irq);
	outl(mask, RTD128X_MISC_UMSK_ISR);
#endif
}

static void rtd128x_internal_irq_unmask(unsigned int irq)
{
#if 0
	u32 mask;

	irq -= RTD128X_INTERNAL_IRQ_BASE;
	mask = inl(RTD128X_MISC_UMSK_ISR);
	mask |= (1 << irq);
	outl(mask, RTD128X__MISC_UMSK_ISR);
#endif
}

static void rtd128x_internal_irq_ack(unsigned int irq)
{
	irq -= RTD128X_INTERNAL_IRQ_BASE;
	outl((1 << irq), RTD128X_MISC_ISR);
}

static void rtd128x_internal_irq_mask_ack(unsigned int irq)
{
	rtd128x_internal_irq_mask(irq);
	rtd128x_internal_irq_ack(irq);
}

static unsigned int rtd128x_internal_irq_startup(unsigned int irq)
{
	rtd128x_internal_irq_unmask(irq);
	return 0;
}

static struct irq_chip rtd128x_internal_irq_chip = {
	.name = "rtd128x-irq",
	.startup = rtd128x_internal_irq_startup,
	.shutdown = rtd128x_internal_irq_mask,
	.ack = rtd128x_internal_irq_ack,
	.mask = rtd128x_internal_irq_mask,
	.mask_ack = rtd128x_internal_irq_mask_ack,
	.unmask = rtd128x_internal_irq_unmask,
};

void __init arch_init_irq(void)
{
	int i;

	/* disable RTC interrupts */
	outl(0x0000, RTD128X_RTC_CR);

	/* clear device interrupts */
	outl(0x3ffc, RTD128X_MISC_ISR);

	mips_cpu_irq_init();

	for (i = 0; i < 32; ++i)
		set_irq_chip_and_handler(RTD128X_INTERNAL_IRQ_BASE + i,
					 &rtd128x_internal_irq_chip,
					 handle_level_irq);

	set_c0_status(1 << (RTD128X_IRQ_MISC + 8));

	rtd128x_sb2_setup();
}
