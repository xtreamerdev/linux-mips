/*
 * $Id$
 *
 * Exported functions of module b1pcmcia to be called by
 * avm_cs card services module.
 *
 * Copyright 1999 by Carsten Paeth (calle@calle.in-berlin.de)
 *
 * $Log$
 * Revision 1.1.2.1  1999/09/22 19:43:53  ralf
 * Merge with Linux 2.2.11.
 *
 * Revision 1.1  1999/07/01 15:26:56  calle
 * complete new version (I love it):
 * + new hardware independed "capi_driver" interface that will make it easy to:
 *   - support other controllers with CAPI-2.0 (i.e. USB Controller)
 *   - write a CAPI-2.0 for the passive cards
 *   - support serial link CAPI-2.0 boxes.
 * + wrote "capi_driver" for all supported cards.
 * + "capi_driver" (supported cards) now have to be configured with
 *   make menuconfig, in the past all supported cards where included
 *   at once.
 * + new and better informations in /proc/capi/
 * + new ioctl to switch trace of capi messages per controller
 *   using "avmcapictrl trace [contr] on|off|...."
 * + complete testcircle with all supported cards and also the
 *   PCMCIA cards (now patch for pcmcia-cs-3.0.13 needed) done.
 *
 */

#ifndef _B1PCMCIA_H_
#define _B1PCMCIA_H_

int b1pcmcia_addcard_b1(unsigned int port, unsigned irq);
int b1pcmcia_addcard_m1(unsigned int port, unsigned irq);
int b1pcmcia_addcard_m2(unsigned int port, unsigned irq);
int b1pcmcia_delcard(unsigned int port, unsigned irq);

#endif	/* _B1PCMCIA_H_ */
