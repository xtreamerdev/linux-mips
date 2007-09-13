#ifndef __ASM_BUG_H
#define __ASM_BUG_H

#include <asm/sgidefs.h>

#ifdef CONFIG_HWTRIGGER

/* Hardware assisted debugging */

#define _HWTRIGGER(code)						\
do {									\
	*(volatile unsigned int *)0xbfc00000 = code;			\
	__asm__ __volatile__ ("sync");					\
} while (0)

#define HWTRIGGER(regs, code, why)					\
do {									\
	extern void show_registers(struct pt_regs *);			\
	_HWTRIGGER((((int)code) << 16) | 0xdead);			\
	if (why) {							\
                char *who = (!regs || !user_mode(regs)) ?		\
			"Kernel" : current->comm;			\
		printk("%s:%d: HWTRIGGER(%d) for %s: %s\n",		\
		       __FILE__, __LINE__,				\
		       (int)code, who, why);				\
	}								\
        if (regs)							\
		show_registers (regs);					\
} while (0)

#define CHWTRIGGER(regs, code, why)					\
do {									\
	extern int hwtrigger (struct pt_regs *);			\
	if (hwtrigger(regs))						\
		HWTRIGGER(regs, code, why);				\
} while (0)
#else
#define _HWTRIGGER(code)
#define HWTRIGGER(regs, code, why)
#define CHWTRIGGER(regs, code, why)
#endif

#ifdef CONFIG_BUG

#include <asm/break.h>

#define BUG()								\
do {									\
	_HWTRIGGER(0xffff);						\
	__asm__ __volatile__("break %0" : : "i" (BRK_BUG));		\
} while (0)

#define HAVE_ARCH_BUG

#if (_MIPS_ISA > _MIPS_ISA_MIPS1)

#define BUG_ON(condition)						\
do {									\
	__asm__ __volatile__("tne $0, %0, %1"				\
			     : : "r" (condition), "i" (BRK_BUG));	\
} while (0)

#define HAVE_ARCH_BUG_ON

#endif /* _MIPS_ISA > _MIPS_ISA_MIPS1 */

#endif

#include <asm-generic/bug.h>

#endif /* __ASM_BUG_H */
