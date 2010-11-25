/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#ifndef _RTD128X_SOC_H_
#define _RTD128X_SOC_H_

#define RTD128X_DEFAULT_LEXRA_MEMBASE 0x01b00000
#define RTD128X_DEFAULT_HIGHMEM_START 0x02000000

#define RTD128X_IO_PORT_BASE          0x18010000

#define RTD128X_CHIPID_VENUS     0x1281
#define RTD128X_CHIPID_NEPTUNE   0x1282
#define RTD128X_CHIPID_MARS      0x1283
#define RTD128X_CHIPID_JUPITER   0x1284

static inline int rtd128x_is_venus_soc(void)
{
	return (rtd128x_board_info.chip_id == RTD128X_CHIPID_VENUS);
}

static inline int rtd128x_is_neptune_soc(void)
{
	return (rtd128x_board_info.chip_id == RTD128X_CHIPID_NEPTUNE);
}

static inline int rtd128x_is_mars_soc(void)
{
	return (rtd128x_board_info.chip_id == RTD128X_CHIPID_MARS);
}

static inline int rtd128x_is_jupiter_soc(void)
{
	return (rtd128x_board_info.chip_id == RTD128X_CHIPID_JUPITER);
}

#endif
