// @file: arch/i386/drv/clock.c
// Copyright (c) 2013 Korepwx. All rights reserved.
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Contributors:
//   Korepwx  <public@korepwx.com>  2013-07-17
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.

#include <asm/io.h>
#include <pcore/trap.h>
#include <pcore/picirq.h>
#include <pcore/clock.h>
#include <stdio.h>

/* *
 * Support for time-related hardware gadgets - the 8253 timer,
 * which generates interruptes on IRQ-0.
 * */

#define IO_TIMER1           0x040               // 8253 Timer #1

/* *
 * Frequency of all three count-down timers; (TIMER_FREQ/freq)
 * is the appropriate count to generate a frequency of freq Hz.
 * */

#define TIMER_FREQ      1193182
#define TIMER_DIV(x)    ((TIMER_FREQ + (x) / 2) / (x))

#define TIMER_MODE      (IO_TIMER1 + 3)       // timer mode port
#define TIMER_SEL0      0x00                  // select counter 0
#define TIMER_RATEGEN   0x04                  // mode 2, rate generator
#define TIMER_16BIT     0x30                  // r/w counter 16 bits, LSB first

// TODO: use 64-bit atomic instructions to manipulate the clock value.
static volatile clock_t __ticks;

/**
 * @brief Initialize the hardware clock.
 * initialize 8253 clock to interrupt CLOCKS_PER_SECOND times per second, 
 * and then enable IRQ_TIMER.
 * */
void clock_init(void) {
  // set 8253 timer-chip
  outb(TIMER_MODE, TIMER_SEL0 | TIMER_RATEGEN | TIMER_16BIT);
  outb(IO_TIMER1, TIMER_DIV(CLOCKS_PER_SECOND) % 256);
  outb(IO_TIMER1, TIMER_DIV(CLOCKS_PER_SECOND) / 256);

  // initialize time counter 'ticks' to zero
  __ticks = 0;

  printf("[irq] Clock interrupts installed.\n");
  pic_enable(IRQ_TIMER);
}

// Increase system clock by one.
void clock_inc(void) {
  ++__ticks;
}

// Reset system clock to zero.
void clock_reset(void) {
  __ticks = 0;
}

// Get the value of sysclock.
clock_t ksysclock() {
  return __ticks;
}

