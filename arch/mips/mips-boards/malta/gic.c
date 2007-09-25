#include <linux/init.h>
#include <asm/io.h>
#include <asm/gicregs.h>
#include <asm/gcmpregs.h>
#include <asm/mips-boards/maltaint.h>
#include <asm/irq.h>
#include <linux/hardirq.h>

//#define GIC_DEBUG			1
#define GIC_BASE_ADDR			0x1bdc0000
#define GCMP_BASE_ADDR			0x1fbf8000
#define MSC01_BIU_REG_BASE		0x1bc80000
#define MSC01_SC_CFG_OFS		0x0110
#define MSC01_SC_CFG_GICPRES_MSK	0x00000004
#define MSC01_SC_CFG_GICPRES_SHF	2
#define MSC01_SC_CFG_GICENA_SHF		3
#define GIC_ADDRSPACE_SZ		(128 * 1024)
#define GCMP_ADDRSPACE_SZ		(256 * 1024)
#define MSC01_BIU_ADDRSPACE_SZ		(256 * 1024)

static unsigned long _gic_base;
static unsigned long _gcmp_base;
static unsigned long _msc01_biu_base;
static unsigned int gcmp_present, gic_present;

/* FIXME : Cleanup needed */
static unsigned int gic_pcpu_imasks[2];

void ipi_call_function(unsigned int cpu)
{
#ifdef GIC_DEBUG
	printk(KERN_CRIT "%s called cpu %d to %d\n", __FUNCTION__, smp_processor_id(), cpu);
#endif
	switch(cpu) {
		case(0):
			GIC_REG(SHARED, GIC_SH_WEDGE) = (0x80000000 | GIC_IPI_EXT_INTR_CALLFNC_VPE0);
			break;
		case(1):
			GIC_REG(SHARED, GIC_SH_WEDGE) = (0x80000000 | GIC_IPI_EXT_INTR_CALLFNC_VPE1);
			break;
	}
}

void ipi_resched(unsigned int cpu)
{
#ifdef GIC_DEBUG
	printk(KERN_CRIT "%s called cpu %d to %d \n", __FUNCTION__, smp_processor_id(), cpu);
#endif

	switch(cpu) {
		case(0):
			GIC_REG(SHARED, GIC_SH_WEDGE) = (0x80000000 | GIC_IPI_EXT_INTR_RESCHED_VPE0);
			break;
		case(1):
			GIC_REG(SHARED, GIC_SH_WEDGE) = (0x80000000 | GIC_IPI_EXT_INTR_RESCHED_VPE1);
			break;
	}
}

static void setup_polarities(void)
{
	/* Set everything to +ve polarity */
	GIC_REG(SHARED, GIC_SH_SMASK_31_0)    = 0xffffffff;
	GIC_REG(SHARED, GIC_SH_SMASK_63_32)   = 0xffffffff;
	GIC_REG(SHARED, GIC_SH_SMASK_95_64)   = 0xffffffff;
	GIC_REG(SHARED, GIC_SH_SMASK_127_96)  = 0xffffffff;
	GIC_REG(SHARED, GIC_SH_SMASK_159_128) = 0xffffffff;
	GIC_REG(SHARED, GIC_SH_SMASK_191_160) = 0xffffffff;
	GIC_REG(SHARED, GIC_SH_SMASK_223_192) = 0xffffffff;
	GIC_REG(SHARED, GIC_SH_SMASK_255_224) = 0xffffffff;
}

static void setup_triggers(void)
{
	/* Set everything to level triggering */
	GIC_REG(SHARED, GIC_SH_TRIG_31_0)    = 0;
	GIC_REG(SHARED, GIC_SH_TRIG_63_32)   = 0;
	GIC_REG(SHARED, GIC_SH_TRIG_95_64)   = 0;
	GIC_REG(SHARED, GIC_SH_TRIG_127_96)  = 0;
	GIC_REG(SHARED, GIC_SH_TRIG_159_128) = 0;
	GIC_REG(SHARED, GIC_SH_TRIG_191_160) = 0;
	GIC_REG(SHARED, GIC_SH_TRIG_223_192) = 0;
	GIC_REG(SHARED, GIC_SH_TRIG_255_224) = 0;

	/* ...except the IPI interrupts */
	GIC_REG(SHARED, GIC_SH_TRIG_31_0) |= ((1 << GIC_IPI_EXT_INTR_RESCHED_VPE0) | \
					      (1 << GIC_IPI_EXT_INTR_CALLFNC_VPE0) | \
					      (1 << GIC_IPI_EXT_INTR_RESCHED_VPE1) | \
					      (1 << GIC_IPI_EXT_INTR_CALLFNC_VPE1));
}

static void setup_pin_maps(void)
{
	/* GIC Global Src 3 -> CPU Int 0 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(3)) = (GIC_MAP_TO_PIN_MSK | 0);
	/* GIC Global Src 4 -> CPU Int 1 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(4)) = (GIC_MAP_TO_PIN_MSK | 1);
	/* GIC Global Src 5 -> CPU Int 2 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(5)) = (GIC_MAP_TO_PIN_MSK | 2);
	/* GIC Global Src 6 -> CPU Int 3 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(6)) = (GIC_MAP_TO_PIN_MSK | 3);
	/* GIC Global Src 7 -> CPU Int 4 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(7)) = (GIC_MAP_TO_PIN_MSK | 4);
	/* GIC Global Src 8 -> CPU Int 3 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(8)) = (GIC_MAP_TO_PIN_MSK | 3);
	/* GIC Global Src 9 -> CPU Int 3 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(9)) = (GIC_MAP_TO_PIN_MSK | 3);
	/* GIC Global Src 12 -> CPU Int 3 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(12)) = (GIC_MAP_TO_PIN_MSK | 3);
	/* GIC Global Src 13 -> NMI */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(13)) = (GIC_MAP_TO_NMI_MSK);
	/* GIC Global Src 14 -> NMI */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(14)) = (GIC_MAP_TO_NMI_MSK);

	/* IPI : Note that we need to subtract 2 from the Core Interrupt Num */

	/* GIC Global Src 1 -> Resched IPI for VPE0 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(GIC_IPI_EXT_INTR_RESCHED_VPE0)) = \
				(GIC_MAP_TO_PIN_MSK | (MIPSCPU_INT_IPI0 - 2));

	/* GIC Global Src 2 -> SMP Call Function IPI for VPE0 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(GIC_IPI_EXT_INTR_CALLFNC_VPE0)) = \
				(GIC_MAP_TO_PIN_MSK | (MIPSCPU_INT_IPI1 - 2));

	/* GIC Global Src 10 -> Resched IPI for VPE1 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(GIC_IPI_EXT_INTR_RESCHED_VPE1)) = \
				(GIC_MAP_TO_PIN_MSK | (MIPSCPU_INT_IPI0 - 2));

	/* GIC Global Src 11 -> SMP Call Function IPI for VPE1 */
	GIC_REG_ADDR(SHARED, GIC_SH_MAP_TO_PIN(GIC_IPI_EXT_INTR_CALLFNC_VPE1)) = \
				(GIC_MAP_TO_PIN_MSK | (MIPSCPU_INT_IPI1 - 2));

}

static void setup_vpe_maps(unsigned int numintrs)
{
	unsigned int intr;

	/* Map All interrupts to VPE 0 */
	for (intr = 0; intr < numintrs; intr++) {
		GIC_SH_MAP_TO_VPE_SMASK(intr, 0);
	}

	/* ...but map IPIs to specific VPEs.. */
	GIC_SH_MAP_TO_VPE_SMASK(GIC_IPI_EXT_INTR_RESCHED_VPE0, 0);
	GIC_SH_MAP_TO_VPE_SMASK(GIC_IPI_EXT_INTR_CALLFNC_VPE0, 0);
	GIC_SH_MAP_TO_VPE_SMASK(GIC_IPI_EXT_INTR_RESCHED_VPE1, 1);
	GIC_SH_MAP_TO_VPE_SMASK(GIC_IPI_EXT_INTR_CALLFNC_VPE1, 1);
}

static void vpe_local_setup(unsigned int numvpes) 
{
	int i;
    	unsigned long timer_interrupt = 5, perf_interrupt = 5;

	/*
	 * Setup the default performance counter timer interrupts 
	 * for all VPEs
	 */
	for (i = 0; i < numvpes; i++) {
		GIC_REG(VPE_LOCAL, GIC_VPE_OTHER_ADDR) = i;

		/* Are Interrupts locally routable? */
		if (GIC_REG(VPE_OTHER, GIC_VPE_CTL) & GIC_VPE_CTL_TIMER_RTBL_MSK)
			GIC_REG(VPE_OTHER, GIC_VPE_TIMER_MAP) =
				GIC_MAP_TO_PIN_MSK | timer_interrupt;

		if (GIC_REG(VPE_OTHER, GIC_VPE_CTL) & GIC_VPE_CTL_PERFCNT_RTBL_MSK)
			GIC_REG( VPE_OTHER, GIC_VPE_PERFCTR_MAP) =
				GIC_MAP_TO_PIN_MSK | perf_interrupt;
    }

    return;
}

static void set_interrupt_masks(void)
{
	GIC_REG(SHARED, GIC_SH_SMASK_31_0)    = 0x0;
	GIC_REG(SHARED, GIC_SH_SMASK_63_32)   = 0x0;
	GIC_REG(SHARED, GIC_SH_SMASK_95_64)   = 0x0;
	GIC_REG(SHARED, GIC_SH_SMASK_127_96)  = 0x0;
	GIC_REG(SHARED, GIC_SH_SMASK_159_128) = 0x0;
	GIC_REG(SHARED, GIC_SH_SMASK_191_160) = 0x0;
	GIC_REG(SHARED, GIC_SH_SMASK_223_192) = 0x0;
	GIC_REG(SHARED, GIC_SH_SMASK_255_224) = 0x0;
}

static void setup_mappings(unsigned int numintrs, unsigned int numvpes)
{
	setup_pin_maps();
	vpe_local_setup(numvpes);
	setup_vpe_maps(numintrs);
}

static int do_probe(void)
{
	int retval = 1;

	gcmp_present = (GCMPGCB(GCMPB) & GCMP_GCB_GCMPB_GCMPBASE_MSK) == GCMP_BASE_ADDR;
	gic_present = (REG(_msc01_biu_base, MSC01_SC_CFG) & 
			MSC01_SC_CFG_GICPRES_MSK) >> MSC01_SC_CFG_GICPRES_SHF;

	if (gcmp_present)  {
		printk (KERN_CRIT "GCMP present\n");
		GCMPGCB(GICBA) = GIC_BASE_ADDR | GCMP_GCB_GICBA_EN_MSK;
	}

	if (gic_present) {
		printk (KERN_CRIT "GIC present\n");
	}
	else retval = 0;

	return(retval);
}

static void basic_init(unsigned long numintrs, unsigned long numvpes)
{
	setup_polarities();
	setup_triggers();
	set_interrupt_masks();
	setup_mappings(numintrs, numvpes);
}

static inline int clz(unsigned long x)
{
	__asm__ (
	"	.set	push					\n"
	"	.set	mips32					\n"
	"	clz	%0, %1					\n"
	"	.set	pop					\n"
	: "=r" (x)
	: "r" (x));

	return x;
}

static unsigned int get_int(void)
{
	unsigned int pending, mask;

	pending = GIC_REG(SHARED, GIC_SH_PEND_31_0);
	mask = GIC_REG(SHARED, GIC_SH_MASK_31_0);
	pending = (pending & mask & gic_pcpu_imasks[smp_processor_id()]);
	pending = (31 - clz(pending));
	return (pending);
}

asmlinkage void malta_ipi_irqdispatch(void)
{
	int irq;

	irq = get_int();
	GIC_REG(SHARED, GIC_SH_WEDGE) = irq;
	if (irq < 0) {
		return;  /* interrupt has already been cleared */
	}
	do_IRQ(MIPS_GIC_IRQ_BASE + irq);
}

static unsigned int gic_irq_startup(unsigned int irq)
{
	irq -= MIPS_GIC_IRQ_BASE;
	switch(irq) {
		case(GIC_IPI_EXT_INTR_RESCHED_VPE0):
		case(GIC_IPI_EXT_INTR_CALLFNC_VPE0):
			clear_c0_cause(0x800 << (irq - 1));
			set_c0_status(0x800 << (irq - 1));
			irq_enable_hazard();
			break;
		case(GIC_IPI_EXT_INTR_RESCHED_VPE1):
		case(GIC_IPI_EXT_INTR_CALLFNC_VPE1):
			clear_c0_cause(0x800 << (irq - 10));
			set_c0_status(0x800 << (irq - 10));
			irq_enable_hazard();
			break;
	}
	GIC_REG(SHARED, GIC_SH_SMASK_31_0) = (1 << irq);
	return (0);
}

static void gic_irq_ack(unsigned int irq)
{
	irq -= MIPS_GIC_IRQ_BASE;
	switch(irq) {
		case(GIC_IPI_EXT_INTR_RESCHED_VPE0):
		case(GIC_IPI_EXT_INTR_CALLFNC_VPE0):
			clear_c0_cause(0x800 << (irq - 1));
			clear_c0_status(0x800 << (irq - 1));
			irq_disable_hazard();
			break;
		case(GIC_IPI_EXT_INTR_RESCHED_VPE1):
		case(GIC_IPI_EXT_INTR_CALLFNC_VPE1):
			clear_c0_cause(0x800 << (irq - 10));
			clear_c0_status(0x800 << (irq - 10));
			irq_disable_hazard();
			break;
	}
	GIC_REG(SHARED, GIC_SH_RMASK_31_0) = (1 << irq);
	GIC_REG(SHARED, GIC_SH_WEDGE) = irq; 
}

static void gic_mask_irq(unsigned int irq)
{
	irq -= MIPS_GIC_IRQ_BASE;
	switch(irq) {
		case(GIC_IPI_EXT_INTR_RESCHED_VPE0):
		case(GIC_IPI_EXT_INTR_CALLFNC_VPE0):
			clear_c0_status(0x800 << (irq - 1));
			irq_disable_hazard();
			break;
		case(GIC_IPI_EXT_INTR_RESCHED_VPE1):
		case(GIC_IPI_EXT_INTR_CALLFNC_VPE1):
			clear_c0_status(0x800 << (irq - 10));
			irq_disable_hazard();
			break;
	}
	GIC_REG(SHARED, GIC_SH_RMASK_31_0) = (1 << irq); 
}

static void gic_unmask_irq(unsigned int irq)
{
	irq -= MIPS_GIC_IRQ_BASE;
	switch(irq) {
		case(GIC_IPI_EXT_INTR_RESCHED_VPE0):
		case(GIC_IPI_EXT_INTR_CALLFNC_VPE0):
			set_c0_status(0x800 << (irq - 1));
			irq_enable_hazard();
			break;
		case(GIC_IPI_EXT_INTR_RESCHED_VPE1):
		case(GIC_IPI_EXT_INTR_CALLFNC_VPE1):
			set_c0_status(0x800 << (irq - 10));
			irq_enable_hazard();
			break;
	}

	GIC_REG(SHARED, GIC_SH_SMASK_31_0) = (1 << irq); 
}

static void gic_set_affinity(unsigned int irq, cpumask_t dest)
{
#ifdef GIC_DEBUG
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
#endif
}

static struct irq_chip gic_irq_controller = {
	.name		=	"MIPS GIC",
	.startup	=	gic_irq_startup,
	.ack		=	gic_irq_ack,
	.mask		=	gic_mask_irq,
	.mask_ack	=	gic_mask_irq,
	.unmask		=	gic_unmask_irq,
	.eoi		=	gic_unmask_irq,
	.set_affinity	=	gic_set_affinity,
};

static void gic_irq_init(unsigned int numintrs)
{
	int irq_base = MIPS_GIC_IRQ_BASE;
	int i;

	for (i = irq_base; i < (irq_base + numintrs); i++)
		set_irq_chip(i, &gic_irq_controller);

	/* Create Per-CPU Masks */
	gic_pcpu_imasks[0] = ((1 << GIC_IPI_EXT_INTR_RESCHED_VPE0) | (1 << GIC_IPI_EXT_INTR_CALLFNC_VPE0));
	gic_pcpu_imasks[1] = ((1 << GIC_IPI_EXT_INTR_RESCHED_VPE1) | (1 << GIC_IPI_EXT_INTR_CALLFNC_VPE1));

	/* Clear GIC IMASK */
	GIC_REG(SHARED, GIC_SH_MASK_31_0) = 0;
}

void __init gic_init(void)
{
	unsigned int  numintrs, numvpes, data;

	_gic_base = (unsigned long) ioremap_nocache(GIC_BASE_ADDR, GIC_ADDRSPACE_SZ);
	_gcmp_base = (unsigned long) ioremap_nocache(GCMP_BASE_ADDR, GCMP_ADDRSPACE_SZ);
	_msc01_biu_base = (unsigned long) ioremap_nocache(MSC01_BIU_REG_BASE, MSC01_BIU_ADDRSPACE_SZ);

	if (!do_probe())
		return;

	numintrs = (GIC_REG(SHARED, GIC_SH_CONFIG) &
		GIC_SH_CONFIG_NUMINTRS_MSK) >> GIC_SH_CONFIG_NUMINTRS_SHF;
	numintrs = ((numintrs + 1) * 8);

	numvpes = (GIC_REG(SHARED, GIC_SH_CONFIG) &
		GIC_SH_CONFIG_NUMVPES_MSK) >> GIC_SH_CONFIG_NUMVPES_SHF;

#ifdef GIC_DEBUG
	printk (KERN_CRIT "GIC : %d External Interrupts Supported!\n", numintrs);
#endif

	/* Setup mappings, polarities, triggers etc */
	basic_init(numintrs, numvpes);

	gic_irq_init(numintrs);

	if (!gcmp_present) {
		/* Enable the GIC */
	    	data = REG(_msc01_biu_base, MSC01_SC_CFG);
	    	REG(_msc01_biu_base, MSC01_SC_CFG) =
		(data | (0x1 << MSC01_SC_CFG_GICENA_SHF));
	
#ifdef GIC_DEBUG
		printk(KERN_CRIT "GIC Enabled\n");
#endif
	}

#ifdef GIC_DEBUG
	printk(KERN_CRIT "%s called\n", __FUNCTION__);
#endif

	return;
}
