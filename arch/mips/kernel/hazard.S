/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2005, 06 MIPS Technologies, Inc.
 */
#include <asm/regdef.h>
#include <asm/asm.h>

/*
 * MIPS32R2 Instruction Hazard Barrier - must be called
 */

LEAF(mips_ihb)
	.set	mips32r2
	jr.hb	ra
	.set	mips0
	END(mips_ihb)
