/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/screen_info.h>
#include <linux/notifier.h>
#include <linux/etherdevice.h>
#include <linux/if_ether.h>
#include <linux/ctype.h>
#include <linux/cpu.h>
#include <linux/time.h>

#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/mips-boards/generic.h>
#include <asm/mips-boards/prom.h>
#include <asm/dma.h>
#include <asm/time.h>
#include <asm/asm.h>
#include <asm/traps.h>
#include <asm/reboot.h>
#include <asm/asm-offsets.h>

#include <rtd128x-board.h>

extern void rtd128x_board_setup(void);

#define VAL(n)		STR(n)

/*
 * Macros for loading addresses and storing registers:
 * LONG_L_	Stringified version of LONG_L for use in asm() statement
 * LONG_S_	Stringified version of LONG_S for use in asm() statement
 * PTR_LA_	Stringified version of PTR_LA for use in asm() statement
 * REG_SIZE	Number of 8-bit bytes in a full width register
 */
#define LONG_L_		VAL(LONG_L) " "
#define LONG_S_		VAL(LONG_S) " "
#define PTR_LA_		VAL(PTR_LA) " "

#ifdef CONFIG_64BIT
#warning TODO: 64-bit code needs to be verified
#define REG_SIZE	"8"	/* In bytes */
#endif

#ifdef CONFIG_32BIT
#define REG_SIZE	"4"	/* In bytes */
#endif

static void register_panic_notifier(void);
static int panic_handler(struct notifier_block *notifier_block,
			 unsigned long event, void *cause_string);

/*
 * Install a panic notifier for platform-specific diagnostics
 */
static void register_panic_notifier()
{
	static struct notifier_block panic_notifier = {
		.notifier_call = panic_handler,
		.next = NULL,
		.priority = INT_MAX
	};
	atomic_notifier_chain_register(&panic_notifier_list, &panic_notifier);
}

static int panic_handler(struct notifier_block *notifier_block,
			 unsigned long event, void *cause_string)
{
	struct pt_regs my_regs;

	/* Save all of the registers */
	{
		unsigned long at, v0, v1;	/* Must be on the stack */

		/* Start by saving $at and v0 on the stack. We use $at
		 * ourselves, but it looks like the compiler may use v0 or v1
		 * to load the address of the pt_regs structure. We'll come
		 * back later to store the registers in the pt_regs
		 * structure. */
		__asm__ __volatile__(".set	noat\n"
				     LONG_S_ "$at, %[at]\n"
				     LONG_S_ "$2, %[v0]\n"
				     LONG_S_ "$3, %[v1]\n":
				     [at] "=m"(at),[v0] "=m"(v0),[v1] "=m"(v1)
				     ::"at");

		__asm__ __volatile__(".set	noat\n"
				     "move		$at, %[pt_regs]\n"
				     /* Argument registers */
				     LONG_S_ "$4, " VAL(PT_R4) "($at)\n"
				     LONG_S_ "$5, " VAL(PT_R5) "($at)\n"
				     LONG_S_ "$6, " VAL(PT_R6) "($at)\n"
				     LONG_S_ "$7, " VAL(PT_R7) "($at)\n"
				     /* Temporary regs */
				     LONG_S_ "$8, " VAL(PT_R8) "($at)\n"
				     LONG_S_ "$9, " VAL(PT_R9) "($at)\n"
				     LONG_S_ "$10, " VAL(PT_R10) "($at)\n"
				     LONG_S_ "$11, " VAL(PT_R11) "($at)\n"
				     LONG_S_ "$12, " VAL(PT_R12) "($at)\n"
				     LONG_S_ "$13, " VAL(PT_R13) "($at)\n"
				     LONG_S_ "$14, " VAL(PT_R14) "($at)\n"
				     LONG_S_ "$15, " VAL(PT_R15) "($at)\n"
				     /* "Saved" registers */
				     LONG_S_ "$16, " VAL(PT_R16) "($at)\n"
				     LONG_S_ "$17, " VAL(PT_R17) "($at)\n"
				     LONG_S_ "$18, " VAL(PT_R18) "($at)\n"
				     LONG_S_ "$19, " VAL(PT_R19) "($at)\n"
				     LONG_S_ "$20, " VAL(PT_R20) "($at)\n"
				     LONG_S_ "$21, " VAL(PT_R21) "($at)\n"
				     LONG_S_ "$22, " VAL(PT_R22) "($at)\n"
				     LONG_S_ "$23, " VAL(PT_R23) "($at)\n"
				     /* Add'l temp regs */
				     LONG_S_ "$24, " VAL(PT_R24) "($at)\n"
				     LONG_S_ "$25, " VAL(PT_R25) "($at)\n"
				     /* Kernel temp regs */
				     LONG_S_ "$26, " VAL(PT_R26) "($at)\n"
				     LONG_S_ "$27, " VAL(PT_R27) "($at)\n"
				     /* Global pointer, stack pointer, frame pointer and
				      * return address */
				     LONG_S_ "$gp, " VAL(PT_R28) "($at)\n"
				     LONG_S_ "$sp, " VAL(PT_R29) "($at)\n"
				     LONG_S_ "$fp, " VAL(PT_R30) "($at)\n"
				     LONG_S_ "$ra, " VAL(PT_R31) "($at)\n"
				     /* Now we can get the $at and v0 registers back and
				      * store them */
				     LONG_L_ "$8, %[at]\n"
				     LONG_S_ "$8, " VAL(PT_R1) "($at)\n"
				     LONG_L_ "$8, %[v0]\n"
				     LONG_S_ "$8, " VAL(PT_R2) "($at)\n"
				     LONG_L_ "$8, %[v1]\n"
				     LONG_S_ "$8, " VAL(PT_R3) "($at)\n"::
				     [at] "m"(at),
				     [v0] "m"(v0),
				     [v1] "m"(v1),[pt_regs] "r"(&my_regs)
				     :"at", "t0");

		/* Set the current EPC value to be the current location in this
		 * function */
		__asm__ __volatile__(".set	noat\n"
				     "1:\n"
				     PTR_LA_ "$at, 1b\n"
				     LONG_S_ "$at, %[cp0_epc]\n":
				     [cp0_epc] "=m"(my_regs.cp0_epc)
				     ::"at");

		my_regs.cp0_cause = read_c0_cause();
		my_regs.cp0_status = read_c0_status();
	}

	pr_crit("I'm feeling a bit sleepy. hmmmmm... perhaps a nap would... "
		"zzzz... \n");

	return NOTIFY_DONE;
}

const char *get_system_type(void)
{
	return rtd128x_board_info.name;
}

void __init plat_mem_setup(void)
{
	panic_on_oops = 1;
	register_panic_notifier();

	_machine_restart = rtd128x_board_info.machine_restart;
	_machine_halt = rtd128x_board_info.machine_halt;
	pm_power_off = rtd128x_board_info.machine_poweroff;

	rtd128x_board_setup();
}
