/* 
 * sgieisa.h: Defines for the (E)ISA bus on the SGI Indigo2
 *
 * Copyright (C) 1999 Andrew R. Baker (andrewb@uab.edu) 
 *
 */
#ifndef _MIPS_SGIEISA_H
#define _MIPS_SGIEISA_H

#ifdef CONFIG_SGI_EISA
extern void i8259_disable_irq(unsigned int);
extern void i8259_enable_irq(unsigned int);
extern int i8259_request_irq(unsigned int irq,
			     void (*handler)(int, void *, struct pt_regs *),
			     unsigned long flags,
			     const char *device,
			     void *dev_id);

extern void i8259_free_irq(unsigned int irq, void *dev_id);

extern void i8259_do_irq(int, struct pt_regs *);

extern void i8259_init(void);
#endif /* CONFIG_SGI_EISA */

#endif /* !(_MIPS_SGIEISA_H) */
