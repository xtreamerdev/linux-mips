/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 */

#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>

#include <asm/bootinfo.h>
#include <linux/io.h>
#include <asm/system.h>
#include <asm/cacheflush.h>
#include <asm/traps.h>

#include <rtd128x-board.h>
#include <rtd128x-soc.h>
#include <asm/mips-boards/prom.h>
#include <asm/mips-boards/generic.h>

extern void platform_init(void);
extern void platform_setup(void);
extern void prom_putchar(char);
extern void rtd128x_detect_soc(void);

//#define DEBUG

void prom_puts(char *s)
{
	while (*s != '\0') {
		if (*s == '\n') {
			prom_putchar(*s++);
			prom_putchar('\r');
		} else
			prom_putchar(*s++);
	}
}

void prom_puthex(unsigned long l)
{
	char n;
	int i;
	prom_putchar('0');
	prom_putchar('x');
	for (i = 7; i >= 0; i--) {
		n = (char)((l >> (4 * i)) & 0xf);
		switch (n) {
		case 10:
			prom_putchar('a');
			break;
		case 11:
			prom_putchar('b');
			break;
		case 12:
			prom_putchar('c');
			break;
		case 13:
			prom_putchar('d');
			break;
		case 14:
			prom_putchar('e');
			break;
		case 15:
			prom_putchar('f');
			break;
		default:
			prom_putchar(0x30 + n);
		}
	}
}

int prom_argc;
int *_prom_argv, *_prom_envp;
unsigned long _prom_memsize;

/*
 * YAMON (32-bit PROM) pass arguments and environment as 32-bit pointer.
 * This macro take care of sign extension, if running in 64-bit mode.
 */
#define prom_envp(index) ((char *)(long)_prom_envp[(index)])

char *prom_getenv(char *envname)
{
	char *result = NULL;

	if (_prom_envp != NULL) {
		/*
		 * Return a pointer to the given environment variable.
		 * In 64-bit mode: we're using 64-bit pointers, but all pointers
		 * in the PROM structures are only 32-bit, so we need some
		 * workarounds, if we are running in 64-bit mode.
		 */
		int i, index = 0;

		i = strlen(envname);

		while (prom_envp(index)) {
			if (strncmp(envname, prom_envp(index), i) == 0) {
				result = prom_envp(index + 1);
				break;
			}
			index += 2;
		}
	}

	return result;
}

/*
 * YAMON (32-bit PROM) pass arguments and environment as 32-bit pointer.
 * This macro take care of sign extension.
 */
#define prom_argv(index) ((char *)(long)_prom_argv[(index)])

char *__init prom_getcmdline(void)
{
	return &(arcs_cmdline[0]);
}

void __init prom_init_cmdline(void)
{
	char *cp;
	int actr;

	actr = 1;		/* Always ignore argv[0] */

	cp = &(arcs_cmdline[0]);
	while (actr < prom_argc) {
		strcpy(cp, prom_argv(actr));
		cp += strlen(prom_argv(actr));
		*cp++ = ' ';
		actr++;
	}
	if (cp != &(arcs_cmdline[0])) {
		/* get rid of trailing space */
		--cp;
		*cp = '\0';
	}
}

/* TODO: Verify on linux-mips mailing list that the following two  */
/* functions are correct                                           */
/* TODO: Copy NMI and EJTAG exception vectors to memory from the   */
/* BootROM exception vectors. Flush their cache entries. test it.  */

static void __init mips_nmi_setup(void)
{
	void *base;
#if defined(CONFIG_CPU_MIPS32_R1)
	base = cpu_has_veic ?
	    (void *)(CAC_BASE + 0xa80) : (void *)(CAC_BASE + 0x380);
#elif defined(CONFIG_CPU_MIPS32_R2)
	base = (void *)0xbfc00000;
#else
#error NMI exception handler address not defined
#endif
}

static void __init mips_ejtag_setup(void)
{
	void *base;

#if defined(CONFIG_CPU_MIPS32_R1)
	base = cpu_has_veic ?
	    (void *)(CAC_BASE + 0xa00) : (void *)(CAC_BASE + 0x300);
#elif defined(CONFIG_CPU_MIPS32_R2)
	base = (void *)0xbfc00480;
#else
#error EJTAG exception handler address not defined
#endif
}

void __init rtd128x_env_get_bootrev(void)
{
	char *envp;
	unsigned short v0, v1, v2;

	envp = prom_getenv("bootrev");
	if (envp) {
		strcpy(rtd128x_board_info.bootrev, envp);
		sscanf(envp, "%hx.%hx.%hx", &v0, &v1, &v2);

		rtd128x_board_info.company_id = v0;
		rtd128x_board_info.board_id = (v0 << 16) | v1;

		/* old bootrev format : aa.bb.ccc */
		/* new bootrev format : aaaa.bbbb.cccc */
		if (envp[2] == '.')
			rtd128x_board_info.cpu_id = (v1 & 0xf0) >> 4;
		else
			rtd128x_board_info.cpu_id = (v1 & 0xff00) >> 8;
#ifdef DEBUG
		printk
		    ("bootrev = '%s' => company_id = %04x, cpu_id = %02x, board_id = %08x\n",
		     rtd128x_board_info.bootrev, rtd128x_board_info.company_id,
		     rtd128x_board_info.cpu_id, rtd128x_board_info.board_id);
#endif
	}
}

void __init prom_init(void)
{
	prom_argc = fw_arg0;
	_prom_argv = (int *)fw_arg1;
	_prom_envp = (int *)fw_arg2;
	_prom_memsize = (unsigned long)fw_arg3;

	board_nmi_handler_setup = mips_nmi_setup;
	board_ejtag_handler_setup = mips_ejtag_setup;

	rtd128x_board_info.memory_size = _prom_memsize;

	rtd128x_env_get_bootrev();

	set_io_port_base(KSEG1ADDR(RTD128X_IO_PORT_BASE));
	rtd128x_detect_soc();

	platform_init();
	prom_init_cmdline();
	prom_meminit();
}
