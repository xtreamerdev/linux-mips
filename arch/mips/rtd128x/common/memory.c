/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#include <linux/init.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/pfn.h>
#include <linux/string.h>

#include <asm/bootinfo.h>
#include <asm/page.h>
#include <asm/sections.h>

#include <asm/mips-boards/prom.h>
#include <rtd128x-board.h>
#include <rtd128x-soc.h>

//#define DEBUG

enum yamon_memtypes {
	yamon_dontuse,
	yamon_prom,
	yamon_free,
};
static struct prom_pmemblock mdesc[PROM_MAX_PMEMBLOCKS];

#ifdef DEBUG
static char *mtypes[3] = {
	"Dont use memory",
	"YAMON PROM memory",
	"Free memmory",
};
#endif

/* determined physical memory size, not overridden by command line args  */
unsigned long physical_memsize = 0L;
unsigned long lexra_membase = 0L;

static struct prom_pmemblock *__init prom_getmdesc(void)
{
	char *memsize_str;
	unsigned int memsize;
	char *ptr;
	static char cmdline[COMMAND_LINE_SIZE] __initdata;

	/* otherwise look in the environment */
	memsize_str = prom_getenv("memsize");
	if (!memsize_str) {
		printk(KERN_WARNING
		       "memsize not set in boot prom, set to default (32Mb)\n");
		physical_memsize = 0x02000000;
	} else {
#ifdef DEBUG
		pr_debug("prom_memsize = %s\n", memsize_str);
#endif
		physical_memsize = simple_strtol(memsize_str, NULL, 0);
	}

#ifdef CONFIG_CPU_BIG_ENDIAN
	/* SOC-it swaps, or perhaps doesn't swap, when DMA'ing the last
	   word of physical memory */
	physical_memsize -= PAGE_SIZE;
#endif

	/* Check the command line for a memsize directive that overrides
	   the physical/default amount */
	strcpy(cmdline, arcs_cmdline);
	ptr = strstr(cmdline, "memsize=");
	if (ptr && (ptr != cmdline) && (*(ptr - 1) != ' '))
		ptr = strstr(ptr, " memsize=");

	if (ptr)
		memsize = memparse(ptr + 8, &ptr);
	else
		memsize = physical_memsize;

	/* Check the command line for lexra_membase directive that overrides
	   the default base address */
	ptr = strstr(cmdline, "lexra_membase=");
	if (ptr && (ptr != cmdline) && (*(ptr - 1) != ' '))
		ptr = strstr(ptr, " lexra_membase=");

	if (ptr)
		lexra_membase = memparse(ptr + 8, &ptr);
	else
		lexra_membase = RTD128X_DEFAULT_LEXRA_MEMBASE;

	lexra_membase = CPHYSADDR(PFN_ALIGN(lexra_membase));

	memset(mdesc, 0, sizeof(mdesc));

	/*
	 * free pre-kernel memory
	 */
	mdesc[0].type = yamon_free;
	mdesc[0].base = 0x00000000;
	mdesc[0].size = CPHYSADDR(PFN_ALIGN(&_text));

	/*
	 * protect kernel memory
	 */
	mdesc[1].type = yamon_dontuse;
	mdesc[1].base = CPHYSADDR(PFN_ALIGN(&_text));
	mdesc[1].size = CPHYSADDR(PFN_ALIGN(&_end)) - mdesc[1].base;

	/*
	 * free post-kernel memory
	 */
	mdesc[2].type = yamon_free;
	mdesc[2].base = CPHYSADDR(PFN_ALIGN(&_end));
	mdesc[2].size = lexra_membase - mdesc[2].base;

	/*
	 * protect lexra memory
	 */
	mdesc[3].type = yamon_dontuse;
	mdesc[3].base = lexra_membase;
	mdesc[3].size = RTD128X_DEFAULT_HIGHMEM_START - mdesc[3].base;

	/*
	 * kernel high memory
	 */
	mdesc[4].type = yamon_free;
	mdesc[4].base = RTD128X_DEFAULT_HIGHMEM_START;
	mdesc[4].size = memsize - mdesc[4].base;

	return &mdesc[0];
}

static int __init prom_memtype_classify(unsigned int type)
{
	switch (type) {
	case yamon_free:
		return BOOT_MEM_RAM;
	case yamon_prom:
		return BOOT_MEM_ROM_DATA;
	default:
		return BOOT_MEM_RESERVED;
	}
}

void __init prom_meminit(void)
{
	struct prom_pmemblock *p;

	p = prom_getmdesc();
	while (p->size) {
		long type;
		unsigned long base, size;

		type = prom_memtype_classify(p->type);
		base = p->base;
		size = p->size;
		add_memory_region(base, size, type);
		p++;
	}
}

void __init prom_free_prom_memory(void)
{
	unsigned long addr;
	unsigned long recl = 0;
	int i;

	for (i = 0; i < boot_mem_map.nr_map; i++) {
		if (boot_mem_map.map[i].type != BOOT_MEM_ROM_DATA)
			continue;

		addr = boot_mem_map.map[i].addr;
		free_init_pages("prom memory",
				addr, addr + boot_mem_map.map[i].size);
		recl +=
		    (1 +
		     (boot_mem_map.map[i].size >> PAGE_SHIFT)) << PAGE_SHIFT;
	}

	printk("Freeing PROM memory: %luk\n", recl / 1024);

#ifdef CONFIG_RTD128X_RECLAIM_BOOT_MEMORY
	{
		unsigned long addr = 0;
		unsigned long dest = 0;

		if (rtd138x_is_mars_cpu()) {
			addr = F_ADDR2;
			dest = (debug_flag) ? T_ADDR1 : T_ADDR3;
		} else {
			addr = F_ADDR1;
			dest = (debug_flag) ? T_ADDR1 : T_ADDR2;
		}

		free_init_pages("bootloader memory", addr, dest);
		recl = dest - addr;
		recl = (1 + (recl >> PAGE_SHIFT)) << PAGE_SHIFT;
		printk("Reclaimed bootloader memory: %luk\n", recl);
	}
#endif
}
