#
# Makefile for the Linux/MIPS kernel.
#
# Note! Dependencies are done automagically by 'make dep', which also
# removes any old dependencies. DON'T put your own dependencies here
# unless it's something special (ie not a .c file).
#

O_TARGET 		:= pci-mips.o

export-objs		= pci.o

obj-$(CONFIG_PCI)	+= pci.o
obj-$(CONFIG_PCI_AUTO)	+= pci_auto.o

include $(TOPDIR)/Rules.make
