/*
 * BRIEF MODULE DESCRIPTION
 * Galileo Evaluation Boards PCI support.
 *
 * The general-purpose functions to read/write and configure the GT64120A's
 * PCI registers (function names start with pci0 or pci1) are either direct
 * copies of functions written by Galileo Technology, or are modifications
 * of their functions to work with Linux 2.4 vs Linux 2.2.  These functions
 * are Copyright - Galileo Technology.
 *
 * Other functions are derived from other MIPS PCI implementations, or were
 * written by RidgeRun, Inc,  Copyright (C) 2000 RidgeRun, Inc.
 *   glonnon@ridgerun.com, skranz@ridgerun.com, stevej@ridgerun.com
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
#include <linux/init.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/kernel.h>
#include <asm/gt64120/gt64120.h>

#define SELF 0

/*
 * These functions and structures provide the BIOS scan and mapping of the PCI
 * devices.
 */

#define MAX_PCI_DEVS 10

struct pci_device {
	u32 slot;
	u32 BARtype[6];
	u32 BARsize[6];
};

static void __init scan_and_initialize_pci(void);
static u32 __init scan_pci_bus(struct pci_device *pci_devices);
static void __init allocate_pci_space(struct pci_device *pci_devices);

/*
 * pci0GetIOspaceBase - Return PCI0 IO Base Address.
 * Inputs: N/A
 * Returns: PCI0 IO Base Address.
 */

static unsigned int pci0GetIOspaceBase(void)
{
	unsigned int base;
	GT_READ(GT_PCI0IOLD_OFS, &base);
	base = base << 21;
	return base;
}

/*
 * pci0GetMemory0Base - Return PCI0 Memory 0 Base Address.
 * Inputs: N/A
 * Returns: PCI0 Memory 0 Base Address.
 */

static unsigned int pci0GetMemory0Base(void)
{
	unsigned int base;
	GT_READ(GT_PCI0M0LD_OFS, &base);
	base = base << 21;
	return base;
}

/*
 * pci_range_ck -
 *
 * Check if the pci device that are trying to access does really exists
 * on the evaluation board.
 *
 * Inputs :
 * bus - bus number (0 for PCI 0 ; 1 for PCI 1)
 * dev - number of device on the specific pci bus
 *
 * Outpus :
 * 0 - if OK , 1 - if failure
 */
static __inline__ int pci_range_ck(unsigned char bus, unsigned char dev)
{
	/*
	 * We don't even pretend to handle other busses than bus 0 correctly.
	 * Accessing device 31 crashes the CP7000 for some reason.
	 */
	if ((bus == 0) && (dev != 31))
		return 0;
	return -1;
}

/*
 * pciXReadConfigReg  - Read from a PCI configuration register
 *                    - Make sure the GT is configured as a master before
 *                      reading from another device on the PCI.
 *                   - The function takes care of Big/Little endian conversion.
 * INPUTS:   regOffset: The register offset as it apears in the GT spec (or PCI
 *                        spec)
 *           pciDevNum: The device number needs to be addressed.
 * RETURNS: data , if the data == 0xffffffff check the master abort bit in the
 *                 cause register to make sure the data is valid
 *
 *  Configuration Address 0xCF8:
 *
 *       31 30    24 23  16 15  11 10     8 7      2  0     <=bit Number
 *  |congif|Reserved|  Bus |Device|Function|Register|00|
 *  |Enable|        |Number|Number| Number | Number |  |    <=field Name
 *
 */
static unsigned int pci0ReadConfigReg(int offset, struct pci_dev *device)
{
	unsigned int DataForRegCf8;
	unsigned int data;

	DataForRegCf8 = ((PCI_SLOT(device->devfn) << 11) |
			 (PCI_FUNC(device->devfn) << 8) |
			 (offset & ~0x3)) | 0x80000000;
	GT_WRITE(GT_PCI0_CFGADDR_OFS, DataForRegCf8);

	/*
	 * The casual observer might wonder why the READ is duplicated here,
	 * rather than immediately following the WRITE, and just have the swap
	 * in the "if".  That's because there is a latency problem with trying
	 * to read immediately after setting up the address register.  The "if"
	 * check gives enough time for the address to stabilize, so the READ
	 * can work.
	 */
	if (PCI_SLOT(device->devfn) == SELF) {	/* This board */
		GT_READ(GT_PCI0_CFGDATA_OFS, &data);
		return data;
	} else {		/* The PCI is working in LE Mode so swap the Data. */
		GT_READ(GT_PCI0_CFGDATA_OFS, &data);
		return cpu_to_le32(data);
	}
}

/*
 * pciXWriteConfigReg - Write to a PCI configuration register
 *                    - Make sure the GT is configured as a master before
 *                      writingto another device on the PCI.
 *                    - The function takes care of Big/Little endian conversion.
 * Inputs:   unsigned int regOffset: The register offset as it apears in the
 *           GT spec
 *                   (or any other PCI device spec)
 *           pciDevNum: The device number needs to be addressed.
 *
 *  Configuration Address 0xCF8:
 *
 *       31 30    24 23  16 15  11 10     8 7      2  0     <=bit Number
 *  |congif|Reserved|  Bus |Device|Function|Register|00|
 *  |Enable|        |Number|Number| Number | Number |  |    <=field Name
 *
 */
static void pci0WriteConfigReg(unsigned int offset,
			       struct pci_dev *device, unsigned int data)
{
	unsigned int DataForRegCf8;

	DataForRegCf8 = ((PCI_SLOT(device->devfn) << 11) |
			 (PCI_FUNC(device->devfn) << 8) |
			 (offset & ~0x3)) | 0x80000000;
	GT_WRITE(GT_PCI0_CFGADDR_OFS, DataForRegCf8);
	if (PCI_SLOT(device->devfn) == SELF) {	/* This board */
		GT_WRITE(GT_PCI0_CFGDATA_OFS, data);
	} else {		/* configuration Transaction over the pci. */
		/* The PCI is working in LE Mode so swap the Data. */
		GT_WRITE(GT_PCI0_CFGDATA_OFS, le32_to_cpu(data));
	}
}

/*
 * galileo_pcibios_(read/write)_config_(dword/word/byte) -
 *
 * reads/write a dword/word/byte register from the configuration space
 * of a device.
 *
 * Inputs :
 * bus - bus number
 * dev - device number
 * offset - register offset in the configuration space
 * val - value to be written / read
 *
 * Outputs :
 * PCIBIOS_SUCCESSFUL when operation was succesfull
 * PCIBIOS_DEVICE_NOT_FOUND when the bus or dev is errorneous
 * PCIBIOS_BAD_REGISTER_NUMBER when accessing non aligned
 */

static int galileo_pcibios_read_config_dword(struct pci_dev *device,
					     int offset, u32 * val)
{
	int dev, bus;
	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);

	if (pci_range_ck(bus, dev)) {
		*val = 0xffffffff;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	if (offset & 0x3)
		return PCIBIOS_BAD_REGISTER_NUMBER;
	if (bus == 0)
		*val = pci0ReadConfigReg(offset, device);

	/*
	 * This is so that the upper PCI layer will get the correct return
	 * value if we're not attached to anything.
	 */
	if ((offset == 0) && (*val == 0xffffffff)) {
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	return PCIBIOS_SUCCESSFUL;
}

static int galileo_pcibios_read_config_word(struct pci_dev *device,
					    int offset, u16 * val)
{
	int dev, bus;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);

	if (pci_range_ck(bus, dev)) {
		*val = 0xffff;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}
	if (offset & 0x1)
		return PCIBIOS_BAD_REGISTER_NUMBER;

	if (bus == 0)
		*val =
		    (unsigned short) (pci0ReadConfigReg(offset, device) >>
				      ((offset & ~0x3) * 8));

	return PCIBIOS_SUCCESSFUL;
}

static int galileo_pcibios_read_config_byte(struct pci_dev *device,
					    int offset, u8 * val)
{
	int dev, bus;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);

	if (pci_range_ck(bus, dev)) {
		*val = 0xff;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	if (bus == 0)
		*val =
		    (unsigned char) (pci0ReadConfigReg(offset, device) >>
				     ((offset & ~0x3) * 8));

	/*
	 *  This is so that the upper PCI layer will get the correct return
	 * value if we're not attached to anything.
	 */
	if ((offset == 0xe) && (*val == 0xff)) {
		u32 MasterAbort;
		GT_READ(GT_INTRCAUSE_OFS, &MasterAbort);
		if (MasterAbort & 0x40000) {
			GT_WRITE(GT_INTRCAUSE_OFS,
				 (MasterAbort & 0xfffbffff));
			return PCIBIOS_DEVICE_NOT_FOUND;
		}
	}

	return PCIBIOS_SUCCESSFUL;
}

static int galileo_pcibios_write_config_dword(struct pci_dev *device,
					      int offset, u32 val)
{
	int dev, bus;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);

	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;
	if (offset & 0x3)
		return PCIBIOS_BAD_REGISTER_NUMBER;
	if (bus == 0)
		pci0WriteConfigReg(offset, device, val);

	return PCIBIOS_SUCCESSFUL;
}


static int galileo_pcibios_write_config_word(struct pci_dev *device,
					     int offset, u16 val)
{
	int dev, bus;
	unsigned long tmp;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);

	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;
	if (offset & 0x1)
		return PCIBIOS_BAD_REGISTER_NUMBER;
	if (bus == 0)
		tmp = pci0ReadConfigReg(offset, device);

	if ((offset % 4) == 0)
		tmp = (tmp & 0xffff0000) | (val & 0xffff);
	if ((offset % 4) == 2)
		tmp = (tmp & 0x0000ffff) | ((val & 0xffff) << 16);

	if (bus == 0)
		pci0WriteConfigReg(offset, device, tmp);
	return PCIBIOS_SUCCESSFUL;
}

static int galileo_pcibios_write_config_byte(struct pci_dev *device,
					     int offset, u8 val)
{
	int dev, bus;
	unsigned long tmp;

	bus = device->bus->number;
	dev = PCI_SLOT(device->devfn);

	if (pci_range_ck(bus, dev))
		return PCIBIOS_DEVICE_NOT_FOUND;
	if (bus == 0)
		tmp = pci0ReadConfigReg(offset, device);

	if ((offset % 4) == 0)
		tmp = (tmp & 0xffffff00) | (val & 0xff);
	if ((offset % 4) == 1)
		tmp = (tmp & 0xffff00ff) | ((val & 0xff) << 8);
	if ((offset % 4) == 2)
		tmp = (tmp & 0xff00ffff) | ((val & 0xff) << 16);
	if ((offset % 4) == 3)
		tmp = (tmp & 0x00ffffff) | ((val & 0xff) << 24);

	if (bus == 0)
		pci0WriteConfigReg(offset, device, tmp);

	return PCIBIOS_SUCCESSFUL;
}

/*
 * structure galileo_pci_ops
 *
 * This structure holds the pointers for the PCI configuration space
 * access, and the fixup for the interrupts.
 * This structure is registered to the operating system in boot time
 */
struct pci_ops galileo_pci_ops = {
	galileo_pcibios_read_config_byte,
	galileo_pcibios_read_config_word,
	galileo_pcibios_read_config_dword,
	galileo_pcibios_write_config_byte,
	galileo_pcibios_write_config_word,
	galileo_pcibios_write_config_dword
};

struct pci_fixup pcibios_fixups[] = {
	{0}
};

void __init pcibios_fixup_bus(struct pci_bus *c)
{
	gt64120_board_pcibios_fixup_bus(c);
}

/*
 * This code was derived from Galileo Technology's example
 * and significantly reworked.
 *
 * This is very simple.  It does not scan multiple function devices.  It does
 * not scan behind bridges.  Those would be simple to implement, but we don't
 * currently need this.
 */

static void __init scan_and_initialize_pci(void)
{
	struct pci_device pci_devices[MAX_PCI_DEVS];

	if (scan_pci_bus(pci_devices)) {
		allocate_pci_space(pci_devices);
	}
}

/*
 * This is your basic PCI scan.  It goes through each slot and checks to
 * see if there's something that responds.  If so, then get the size and
 * type of each of the responding BARs.  Save them for later.
 */

static u32 __init scan_pci_bus(struct pci_device *pci_devices)
{
	u32 arrayCounter = 0;
	u32 memType;
	u32 memSize;
	u32 pci_slot, bar;
	u32 id;
	u32 c18RegValue;
	struct pci_dev device;

	/*
	 * According to PCI REV 2.1 MAX agents on the bus are 21.  We don't
	 * bother scanning ourselves (slot 0).
	 */
	for (pci_slot = 1; pci_slot < 22; pci_slot++) {

		device.devfn = PCI_DEVFN(pci_slot, 0);
		id = pci0ReadConfigReg(PCI_VENDOR_ID, &device);

		/*
		 *  Check for a PCI Master Abort (nothing responds in the
		 * slot)
		 */
		GT_READ(GT_INTRCAUSE_OFS, &c18RegValue);
		/*
		 * Clearing bit 18 of in the Cause Register 0xc18 by
		 * writting 0.
		 */
		GT_WRITE(GT_INTRCAUSE_OFS, (c18RegValue & 0xfffbffff));
		if ((id != 0xffffffff) && !(c18RegValue & 0x40000)) {
			pci_devices[arrayCounter].slot = pci_slot;
			for (bar = 0; bar < 6; bar++) {
				memType =
				    pci0ReadConfigReg(PCI_BASE_ADDRESS_0 +
						      (bar * 4), &device);
				pci_devices[arrayCounter].BARtype[bar] =
				    memType & 1;
				pci0WriteConfigReg(PCI_BASE_ADDRESS_0 +
						   (bar * 4), &device,
						   0xffffffff);
				memSize =
				    pci0ReadConfigReg(PCI_BASE_ADDRESS_0 +
						      (bar * 4), &device);
				if (memType & 1) {	/*  IO space  */
					pci_devices[arrayCounter].
					    BARsize[bar] =
					    ~(memSize & 0xfffffffc) + 1;
				} else {	/*  memory space */
					pci_devices[arrayCounter].
					    BARsize[bar] =
					    ~(memSize & 0xfffffff0) + 1;
				}
			}	/*  BAR counter  */

			arrayCounter++;
		}
		/*  found a device  */
	}			/*  slot counter  */

	if (arrayCounter < MAX_PCI_DEVS)
		pci_devices[arrayCounter].slot = -1;

	return arrayCounter;
}

#define ALIGN(val,align)        (((val) + ((align) - 1)) & ~((align) - 1))
#define MAX(val1, val2) ((val1) > (val2) ? (val1) : (val2))

/*
 * This function goes through the list of devices and allocates the BARs in
 * either IO or MEM space.  It does it in order of size, which will limit the
 * amount of fragmentation we have in the IO and MEM spaces.
 */

static void __init allocate_pci_space(struct pci_device *pci_devices)
{
	u32 count, maxcount, bar;
	u32 maxSize, maxDevice, maxBAR;
	u32 alignto;
	u32 base;
	u32 pci0_mem_base = pci0GetMemory0Base();
	u32 pci0_io_base = pci0GetIOspaceBase();
	struct pci_dev device;

	/*  How many PCI devices do we have?  */
	maxcount = MAX_PCI_DEVS;
	for (count = 0; count < MAX_PCI_DEVS; count++) {
		if (pci_devices[count].slot == -1) {
			maxcount = count;
			break;
		}
	}

	do {
		/*  Find the largest size BAR we need to allocate  */
		maxSize = 0;
		for (count = 0; count < maxcount; count++) {
			for (bar = 0; bar < 6; bar++) {
				if (pci_devices[count].BARsize[bar] >
				    maxSize) {
					maxSize =
					    pci_devices[count].
					    BARsize[bar];
					maxDevice = count;
					maxBAR = bar;
				}
			}
		}

		/*
		 * We've found the largest BAR.  Allocate it into IO or mem
		 * space.  We don't idiot check the bases to make sure they
		 * haven't overflowed the current size for that aperture.
		 * Don't bother to enable the device's IO or MEM space here.
		 * That will be done in pci_enable_resources if the device is
		 * activated by a driver.
		 */
		if (maxSize) {
			device.devfn =
			    PCI_DEVFN(pci_devices[maxDevice].slot, 0);
			if (pci_devices[maxDevice].BARtype[maxBAR] == 1) {
				alignto = MAX(0x1000, maxSize);
				base = ALIGN(pci0_io_base, alignto);
				pci0WriteConfigReg(PCI_BASE_ADDRESS_0 +
						   (maxBAR * 4), &device,
						   base | 0x1);
				pci0_io_base = base + alignto;
			} else {
				alignto = MAX(0x1000, maxSize);
				base = ALIGN(pci0_mem_base, alignto);
				pci0WriteConfigReg(PCI_BASE_ADDRESS_0 +
						   (maxBAR * 4), &device,
						   base);
				pci0_mem_base = base + alignto;
			}
			/*
			 * This entry is finished.  Remove it from the list
			 * we'll scan.
			 */
			pci_devices[maxDevice].BARsize[maxBAR] = 0;
		}
	} while (maxSize);
}

void __init pcibios_init(void)
{
	u32 tmp;
	struct pci_dev controller;

	controller.devfn = SELF;

	GT_READ(GT_PCI0_CMD_OFS, &tmp);
	GT_READ(GT_PCI0_BARE_OFS, &tmp);

	/*
	 * You have to enable bus mastering to configure any other
	 * card on the bus.
	 */
	tmp = pci0ReadConfigReg(PCI_COMMAND, &controller);
	tmp |= PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_SERR;
	pci0WriteConfigReg(PCI_COMMAND, &controller, tmp);

	/*  This scans the PCI bus and sets up initial values.  */
	scan_and_initialize_pci();

	/*
	 *  Reset PCI I/O and PCI MEM values to ones supported by EVM.
	 */
	ioport_resource.start	= GT_PCI_IO_BASE;
	ioport_resource.end	= GT_PCI_IO_BASE + GT_PCI_IO_SIZE - 1;
	iomem_resource.start	= GT_PCI_MEM_BASE;
	iomem_resource.end	= GT_PCI_MEM_BASE + GT_PCI_MEM_SIZE - 1;

	pci_scan_bus(0, &galileo_pci_ops, NULL);
}

unsigned __init int pcibios_assign_all_busses(void)
{
	return 1;
}
