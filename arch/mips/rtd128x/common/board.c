/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#include <linux/string.h>
#include <linux/delay.h>
#include <asm/io.h>

#include <rtd128x-board.h>
#include <rtd128x-soc.h>
#include <rtd128x-io.h>

struct rtd128x_board rtd128x_board_info;

static struct rtd128x_board rtd128x_em7080_info = {
	.name = "em7080",
	.ext_freq = 27000000,
	.has_eth0 = 1,
	.has_pci = 0,
	.has_pccard = 0,
	.has_ohci0 = 1,
	.has_ehci0 = 1,
	.has_sata0 = 1,
	.has_sata1 = 1,
	.has_uart0 = 1,
	.has_uart1 = 1,
	.has_vfd = 0,
	.machine_restart = NULL,
	.machine_halt = NULL,
	.machine_poweroff = NULL,
};

/*
 * rtd128x reset and halt handlers
 */

static void rtd128x_common_machine_restart(char *cmd)
{
#ifdef CONFIG_RTD128X_WATCHDOG
	/*
	 * Use Watchdog to reset SoC
	 */
	kill_watchdog();
#else
	/*
	 * TODO: Find a way to reset the SoC
	 */
	outl(0x0, RTD128X_TIMR_TCWCR);
#endif
	msleep(5000);
}

static void rtd128x_common_machine_halt(void)
{
	msleep(5000);
}

/*
 * rtd128x soc
 */

static const char *rtd128x_get_soc_name(void)
{
	switch (rtd128x_board_info.chip_id) {
	case RTD128X_CHIPID_VENUS:
		return "Venus";
	case RTD128X_CHIPID_NEPTUNE:
		return "Neptune";
	case RTD128X_CHIPID_MARS:
		return "Mars";
	case RTD128X_CHIPID_JUPITER:
		return "Jupiter";
	}
	return "unknown";
}

void rtd128x_detect_soc(void)
{
	u32 id, rev;

	id = inl(RTD128X_SB2_CHIP_ID);
	rev = inl(RTD128X_SB2_CHIP_INFO);

	rtd128x_board_info.chip_id = id & 0xffff;
	rtd128x_board_info.chip_rev = (id >> 16) & 0xffff;

	printk("Detected rtd%04x SoC, type %s rev %x\n",
	       rtd128x_board_info.chip_id,
	       rtd128x_get_soc_name(), rtd128x_board_info.chip_rev);
}

/*
 * rtd128x boards
 */

static enum rtd128x_board_type rtd128x_detect_board(void)
{
	return RTD128X_BOARD_EM7080;

	/*
	 * TODO: Detect different board types
	 */

	return RTD128X_BOARD_UNKNOWN;
}

void rtd128x_board_setup(void)
{
	switch (rtd128x_detect_board()) {
	case RTD128X_BOARD_EM7080:
		memcpy(&rtd128x_board_info, &rtd128x_em7080_info,
		       sizeof(struct rtd128x_board));
		break;
	default:
		printk("Unknown rtd128x board.");
		return;
	}

	if (rtd128x_board_info.machine_restart == NULL) {
		rtd128x_board_info.machine_restart =
		    rtd128x_common_machine_restart;
	}

	if (rtd128x_board_info.machine_halt == NULL) {
		rtd128x_board_info.machine_halt = rtd128x_common_machine_halt;
	}

	if (rtd128x_board_info.machine_poweroff == NULL) {
		rtd128x_board_info.machine_poweroff =
		    rtd128x_common_machine_halt;
	}

	printk("Detected %s board\n", rtd128x_board_info.name);
}
