/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * r4xx0.c: R4000 processor variant specific MMU/Cache routines.
 *
 * Copyright (C) 1996 David S. Miller (dm@engr.sgi.com)
 * Copyright (C) 1997,  1998,  1999,  2000 Ralf Baechle ralf@gnu.org
 */
#include <linux/config.h>
#include <asm/addrspace.h>
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/cacheops.h>
#include <asm/mipsregs.h>
#include <asm/offset.h>

#ifdef CONFIG_64BIT_PHYS_ADDR
#define PGD_SIZE	0x2000
#else
#define PGD_SIZE	0x1000
#endif

	.text
	.set	noat

/* This one still needs to receive cache optimizations  */
LEAF(pgd_init)
	.set	mips0
	addiu	AT, a0, PGD_SIZE / 2
	la	v0, invalid_pte_table
1:	sw	v0, (a0)
	sw	v0, 4(a0)
	sw	v0, 8(a0)
	sw	v0, 12(a0)
	addiu	a0, 32
	sw	v0, -16(a0)
	sw	v0, -12(a0)
	sw	v0, -8(a0)
	sw	v0, -4(a0)
	bne	AT, a0, 1b
	jr	ra
	END(pgd_init)
