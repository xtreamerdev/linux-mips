/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2006, 07 MIPS Technologies, Inc.
 *     written by Ralf Baechle <ralf@linux-mips.org>
 */

#include <linux/init.h>
#include <linux/errno.h>
#include <linux/module.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>

static struct mtd_partition malta_mtd_partitions[] = {
	{
		.name =		"YAMON",
		.offset =	0x0,
		.size =		0x100000,
		.mask_flags =	MTD_WRITEABLE
	}, {
		.name =		"User FS",
		.offset = 	0x100000,
		.size =		0x2e0000
	}, {
		.name =		"Board Config",
		.offset =	0x3e0000,
		.size =		0x020000,
		.mask_flags =	MTD_WRITEABLE
	}
};

#define nr_malta_mtd_partitions ARRAY_SIZE(malta_mtd_partitions)

static struct map_info malta_flash_map = {
	.name = "Malta-Flash",
	.size = 0x400000UL,
	.bankwidth = 4,
	.phys = 0x1e000000UL,
};

static struct mtd_info *malta_mtd;

static int __init malta_mtd_init(void)
{
	struct map_info *map = &malta_flash_map;

	printk(KERN_NOTICE "Malta: flash mapping: 0x%lx at 0x%lx\n",
	       map->size, map->phys);

	map->virt = ioremap(map->phys, map->size);
	if (!map->virt) {
		printk(KERN_ERR "malta_mtd_init: failed to ioremap\n");
		return -EIO;
	}

	simple_map_init(map);
	malta_mtd = do_map_probe("cfi_probe", map);

	if (malta_mtd) {
		malta_mtd->owner = THIS_MODULE;

		return add_mtd_partitions(malta_mtd,
		                          malta_mtd_partitions,
		                          nr_malta_mtd_partitions);
        }

	return -ENXIO;
}

static void __exit malta_mtd_exit(void)
{
	if (malta_mtd) {
		del_mtd_partitions(malta_mtd);
		map_destroy(malta_mtd);
		iounmap((void *)malta_flash_map.virt);
	}
}

module_init(malta_mtd_init)
module_exit(malta_mtd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ralf Baechle <ralf@linux-mips.org>");
MODULE_DESCRIPTION("MTD map driver for the MIPS Malta evaluation board");
