/**************************************************************************
 *
 *  arch/mips/fpu_emulator/kernel_linkage.c
 *
 *  Kevin D. Kissell, kevink@acm.org
 *  Copyright (C) 1999 MIPS Technologies, Inc.  All rights reserved.
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
 *************************************************************************/
/*
 * Routines corresponding to Linux kernel FP context
 * manipulation primitives for the Algorithmics MIPS
 * FPU Emulator
 */

#include <linux/sched.h>
#include <asm/processor.h>
#include <asm/signal.h>
#include <asm/uaccess.h>

#include <asm/fpu_emulator.h>

extern struct mips_fpu_emulator_private fpuemuprivate;

#define SIGNALLING_NAN 0x7ff800007ff80000LL

void
fpu_emulator_init_fpu(void)
{
	int i;

	printk("Algorithmics/MIPS FPU Emulator v1.3\n");
	current->tss.fpu.soft.sr = 0;
	for (i = 0; i < 32; i++) {
		current->tss.fpu.soft.regs[i] = SIGNALLING_NAN;
	}
}


/*
 * Emulator context save/restore to/from a signal context
 * presumed to be on the user stack, and therefore accessed
 * with appropriate macros from uaccess.h
 */

void
fpu_emulator_save_context(struct sigcontext *sc)
{
	int i;

	for (i = 0; i < 32; i++) {
		__put_user(current->tss.fpu.soft.regs[i], &sc->sc_fpregs[i]);
	}
	__put_user(current->tss.fpu.soft.sr, &sc->sc_fpc_csr);
	__put_user(fpuemuprivate.eir, &sc->sc_fpc_eir);
}

void
fpu_emulator_restore_context(struct sigcontext *sc)
{
	int i;

	for (i = 0; i < 32; i++) {
		__get_user(current->tss.fpu.soft.regs[i], &sc->sc_fpregs[i]);
	}
	__get_user(current->tss.fpu.soft.sr, &sc->sc_fpc_csr);
	__get_user(fpuemuprivate.eir, &sc->sc_fpc_eir);
}
