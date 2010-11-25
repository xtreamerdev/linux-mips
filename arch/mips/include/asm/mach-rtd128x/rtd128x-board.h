/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#ifndef _RTD128X_BOARD_H_
#define _RTD128X_BOARD_H_

#include <linux/types.h>

enum rtd128x_board_type {
	RTD128X_BOARD_UNKNOWN = -1,
	RTD128X_BOARD_EM7080 = 0,
};

struct rtd128x_board {
	char name[16];
	u32 ext_freq;
	unsigned int has_eth0:1;
	unsigned int has_pci:1;
	unsigned int has_pccard:1;
	unsigned int has_ohci0:1;
	unsigned int has_ehci0:1;
	unsigned int has_sata0:1;
	unsigned int has_sata1:1;
	unsigned int has_uart0:1;
	unsigned int has_uart1:1;
	unsigned int has_vfd:1;

	void (*machine_restart) (char *);
	void (*machine_halt) (void);
	void (*machine_poweroff) (void);

	/*
	 * Autodetected / PROM values
	 */

	phys_t memory_size;
	char bootrev[16];
	u16 company_id;
	u32 board_id;
	u8 cpu_id;
	u8 chip_rev;
	u16 chip_id;
};

extern struct rtd128x_board rtd128x_board_info;

#endif
