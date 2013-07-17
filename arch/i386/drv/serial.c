// @file: arch/i386/drv/serial.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

// This file is derived from ucore.

#include <stddef.h>
#include <asm/io.h>
#include <pcore/picirq.h>
#include <pcore/trap.h>

/***** Serial I/O code *****/
#define COM1            0x3F8

#define COM_RX          0       // In:  Receive buffer (DLAB=0)
#define COM_TX          0       // Out: Transmit buffer (DLAB=0)
#define COM_DLL         0       // Out: Divisor Latch Low (DLAB=1)
#define COM_DLM         1       // Out: Divisor Latch High (DLAB=1)
#define COM_IER         1       // Out: Interrupt Enable Register
#define COM_IER_RDI     0x01    // Enable receiver data interrupt
#define COM_IIR         2       // In:  Interrupt ID Register
#define COM_FCR         2       // Out: FIFO Control Register
#define COM_LCR         3       // Out: Line Control Register
#define COM_LCR_DLAB    0x80    // Divisor latch access bit
#define COM_LCR_WLEN8   0x03    // Wordlength: 8 bits
#define COM_MCR         4       // Out: Modem Control Register
#define COM_MCR_RTS     0x02    // RTS complement
#define COM_MCR_DTR     0x01    // DTR complement
#define COM_MCR_OUT2    0x08    // Out2 complement
#define COM_LSR         5       // In:  Line Status Register
#define COM_LSR_DATA    0x01    // Data available
#define COM_LSR_TXRDY   0x20    // Transmit buffer avail
#define COM_LSR_TSRE    0x40    // Transmitter off

#define LPTPORT         0x378

static bool serial_exists = 0;

/// @brief Initialize the serial ports.
void serial_init(void) {
  // Turn off the FIFO
  outb(COM1 + COM_FCR, 0);

  // Set speed; requires DLAB latch
  outb(COM1 + COM_LCR, COM_LCR_DLAB);
  outb(COM1 + COM_DLL, (uint8_t) (115200 / 9600));
  outb(COM1 + COM_DLM, 0);

  // 8 data bits, 1 stop bit, parity off; turn off DLAB latch
  outb(COM1 + COM_LCR, COM_LCR_WLEN8 & ~COM_LCR_DLAB);

  // No modem controls
  outb(COM1 + COM_MCR, 0);
  // Enable rcv interrupts
  outb(COM1 + COM_IER, COM_IER_RDI);

  // Clear any preexisting overrun indications and interrupts
  // Serial port doesn't exist if COM_LSR returns 0xFF
  serial_exists = (inb(COM1 + COM_LSR) != 0xFF);
  (void) inb(COM1+COM_IIR);
  (void) inb(COM1+COM_RX);

  if (serial_exists) {
    pic_enable(IRQ_COM1);
  }
}

static void lpt_putc_sub(int c) {
  int i;
  for (i = 0; !(inb(LPTPORT + 1) & 0x80) && i < 12800; i ++) {
    io_delay();
  }
  outb(LPTPORT + 0, c);
  outb(LPTPORT + 2, 0x08 | 0x04 | 0x01);
  outb(LPTPORT + 2, 0x08);
}

/* lpt_putc - copy console output to parallel port */
static void lpt_putc(int c) {
  if (c != '\b') {
    lpt_putc_sub(c);
  }
  else {
    lpt_putc_sub('\b');
    lpt_putc_sub(' ');
    lpt_putc_sub('\b');
  }
}

static void serial_putc_sub(int c) {
  int i;
  for (i = 0; !(inb(COM1 + COM_LSR) & COM_LSR_TXRDY) && i < 12800; i ++) {
    io_delay();
  }
  outb(COM1 + COM_TX, c);
}

/* serial_putc - print character to serial port */
static void serial_putc(int c) {
  if (c != '\b') {
    serial_putc_sub(c);
  }
  else {
    serial_putc_sub('\b');
    serial_putc_sub(' ');
    serial_putc_sub('\b');
  }
}

/// @brief Put character into serial port.
void serial_putchar(int ch) {
  lpt_putc(ch);
  if (serial_exists)
    serial_putc(ch);
}
