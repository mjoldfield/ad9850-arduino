/*
 * ------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@FreeBSD.ORG> wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return
 * Poul-Henning Kamp
 *
 * Hacked around somewhat to facilitate playing with multiple boards
 * M J Oldfield
 * 17.vi.2015
 * ------------------------------------------------------------------------
 */
#ifndef AD9850_H
#define AD9850_H

#include <Arduino.h>
#include <stdint.h>

#define EX_CLK 125.0e6
// change if ad9850 is connected to other frequency crystal oscillator

class AD9850
{
 private:
  const char W_CLK; // word load clock pin
  const char FQ_UD; // frequency update pin
  const char D7;    // serial input pin
  const char RESET; // reset line
  uint32_t phase_delta;
  uint8_t phase; // phase offset
  void update();

 public:
  AD9850(char w_clk, char fq_ud, char d7, char reset);
  /* NOTE: For device start-up in serial mode,
     hardwire pin 2 at 0, pin 3 at 1, and pin 4 at 1 */

  // Given a frequency in Hz, return the multiplier
  // This is double -> int which facilitates getting
  // exact ratios of frequencies
  uint32_t calc_phase_delta(double f);

  // Set the oscillator frequency
  void set_phase_delta(uint32_t dphi);
  void set_freq(double f);

  // Set phase
  void set_phase(uint8_t p);
  // for flexibility, p is an int value and 0 <= p <= 32,
  // as input, 360 degree devide into 32 parts,
  // you will get phase increments for 360/32*p degree

  // Power up/down
  void down();
  void up();
};
#endif
