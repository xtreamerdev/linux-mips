/*
 *
 * BRIEF MODULE DESCRIPTION
 *	Alchemy Titanium board setup.
 *
 * Copyright 2000-2003 MontaVista Software Inc.
 * Author: MontaVista Software, Inc.
 *         	ppopov@mvista.com or source@mvista.com
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <linux/config.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#include <linux/console.h>
#include <linux/mc146818rtc.h>
#include <linux/delay.h>

#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/keyboard.h>
#include <asm/mipsregs.h>
#include <asm/reboot.h>
#include <asm/pgtable.h>
#include <asm/au1000.h>
#include <asm/pb1500.h>

#ifdef CONFIG_USB_OHCI
// Enable the workaround for the OHCI DoneHead
// register corruption problem.
#define CONFIG_AU1000_OHCI_FIX
#endif

#if defined(CONFIG_AU1X00_SERIAL_CONSOLE)
extern void console_setup(char *, int *);
char serial_console[20];
#endif

#ifdef CONFIG_BLK_DEV_INITRD
extern unsigned long initrd_start, initrd_end;
extern void * __rd_start, * __rd_end;
#endif

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
extern struct ide_ops std_ide_ops;
extern struct ide_ops *ide_ops;
#endif

extern struct rtc_ops no_rtc_ops;
extern char * __init prom_getcmdline(void);
extern void au1000_restart(char *);
extern void au1000_halt(void);
extern void au1000_power_off(void);
extern struct resource ioport_resource;
extern struct resource iomem_resource;

extern phys_t (*fixup_bigphys_addr)(phys_t phys_addr, phys_t size);
static phys_t au1500_fixup_bigphys_addr(phys_t phys_addr, phys_t size);

void __init au1x00_setup(void)
{
	char *argptr;
	u32 pin_func;
	
	argptr = prom_getcmdline();

	/* NOTE: The memory map is established by YAMON 2.08+ */

	/* Various early Au1500 Errata corrected by this */
	set_c0_config(1<<19); /* Config[OD] */

#ifdef CONFIG_AU1X00_SERIAL_CONSOLE
	if ((argptr = strstr(argptr, "console=")) == NULL) {
		argptr = prom_getcmdline();
		strcat(argptr, " console=ttyS0,115200");
	}
#endif	  

#if defined(CONFIG_SOUND_AU1X00)
	strcat(argptr, " au1000_audio=vra");
	argptr = prom_getcmdline();
#endif

	rtc_ops = &no_rtc_ops;
	_machine_restart = au1000_restart;
	_machine_halt = au1000_halt;
	_machine_power_off = au1000_power_off;
	fixup_bigphys_addr = au1500_fixup_bigphys_addr;

	// IO/MEM resources. 
	ioport_resource.start = 0x00000000;
	ioport_resource.end = 0xffffffff;
	iomem_resource.start = 0x10000000;
	iomem_resource.end = 0xffffffff;

#ifdef CONFIG_BLK_DEV_INITRD
	ROOT_DEV = MKDEV(RAMDISK_MAJOR, 0);
	initrd_start = (unsigned long)&__rd_start;
	initrd_end = (unsigned long)&__rd_end;
#endif


#if defined (CONFIG_USB_OHCI) || defined (CONFIG_AU1X00_USB_DEVICE)
#ifdef CONFIG_USB_OHCI
	if ((argptr = strstr(argptr, "usb_ohci=")) == NULL) {
	        char usb_args[80];
		argptr = prom_getcmdline();
		memset(usb_args, 0, sizeof(usb_args));
		sprintf(usb_args, " usb_ohci=base:0x%x,len:0x%x,irq:%d",
			USB_OHCI_BASE, USB_OHCI_LEN, AU1000_USB_HOST_INT);
		strcat(argptr, usb_args);
	}

	// enable host controller and wait for reset done
	au_writel(0x08, USB_HOST_CONFIG);
	udelay(1000);
	au_writel(0x0c, USB_HOST_CONFIG);
	udelay(1000);
	au_readl(USB_HOST_CONFIG);
	while (!(au_readl(USB_HOST_CONFIG) & 0x10))
	    ;
	au_readl(USB_HOST_CONFIG);
#endif

#endif // defined (CONFIG_USB_OHCI) || defined (CONFIG_AU1000_USB_DEVICE)


	// CONFIG_MYCABLE_XXS_1500
	// set multiple use pins (UART3/GPIO) to UART (it's used as UART too)
	pin_func = au_readl(SYS_PINFUNC) & (u32)(~SYS_PF_UR3);
	pin_func |= SYS_PF_UR3;
	au_writel(pin_func, SYS_PINFUNC);

	// enable UART
	au_writel(0x01, UART3_ADDR+UART_MOD_CNTRL); // clock enable (CE)
	mdelay(10);
	au_writel(0x03, UART3_ADDR+UART_MOD_CNTRL); // CE and "enable"
	mdelay(10);

	// enable DTR = USB power up
	au_writel(0x01, UART3_ADDR+UART_MCR); //? UART_MCR_DTR is 0x01???
	// CONFIG_MYCABLE_XXS_1500


#ifdef CONFIG_PCMCIA_TITANIUM
	/* setup pcmcia signals */
	au_writel(0, SYS_PININPUTEN);

	/* gpio 0, 1, and 4 are inputs */
	au_writel(1 | (1<<1) | (1<<4), SYS_TRIOUTCLR);

	/* enable GPIO2 if not already enabled */
	au_writel(1, GPIO2_ENABLE);
	/* gpio2 208/9/10/11 are inputs */
	au_writel((1<<8) | (1<<9) | (1<<10) | (1<<11), GPIO2_DIR);
	
	/* turn off power */
	au_writel((au_readl(GPIO2_PINSTATE) & ~(1<<14))|(1<<30), GPIO2_OUTPUT);
#endif
	
#ifdef CONFIG_FB
	conswitchp = &dummy_con;
#endif


#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
	ide_ops = &std_ide_ops;
#endif

#ifdef CONFIG_PCI
#if defined(__MIPSEB__)
	au_writel(0xf | (2<<6) | (1<<4), Au1500_PCI_CFG);
#else
	au_writel(0xf, Au1500_PCI_CFG);
#endif
#endif

	while (au_readl(SYS_COUNTER_CNTRL) & SYS_CNTRL_E0S);
	au_writel(SYS_CNTRL_E0 | SYS_CNTRL_EN0, SYS_COUNTER_CNTRL);
	au_sync();
	while (au_readl(SYS_COUNTER_CNTRL) & SYS_CNTRL_T0S);
	au_writel(0, SYS_TOYTRIM);
}

static phys_t au1500_fixup_bigphys_addr(phys_t phys_addr, phys_t size)
{
	u32 pci_start = (u32)Au1500_PCI_MEM_START;
	u32 pci_end = (u32)Au1500_PCI_MEM_END;

	/* Don't fixup 36 bit addresses */
	if ((phys_addr >> 32) != 0) return phys_addr;

	/* check for pci memory window */
	if ((phys_addr >= pci_start) && ((phys_addr + size) < pci_end)) {
		return (phys_t)((phys_addr - pci_start) +
				     Au1500_PCI_MEM_START);
	}
	else 
		return phys_addr;
}
