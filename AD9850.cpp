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

#include <AD9850.h>

#define pulse(pin) {digitalWrite(pin, HIGH); digitalWrite(pin, LOW);}

AD9850::AD9850(char w_clk, char fq_ud, char d7, char reset)
  : W_CLK(w_clk), FQ_UD(fq_ud), D7(d7), RESET(reset)
{
  phase_delta = 0;
  phase = 0;

  pinMode(W_CLK, OUTPUT);
  pinMode(FQ_UD, OUTPUT);
  pinMode(D7,    OUTPUT);
  pinMode(RESET, OUTPUT);

  pulse(RESET);
  pulse(W_CLK);
  pulse(FQ_UD);
}

void AD9850::update()
{
  uint32_t dphi = phase_delta;
  for (int i = 0; i < 32; i++, dphi >>= 1)
    {
      digitalWrite(D7, dphi & (uint32_t)0x00000001);
      pulse(W_CLK);
    }
  
  uint8_t p = phase;
  for (int i = 0; i < 8; i++, p >>= 1)
    {
      digitalWrite(D7, p & (uint8_t)0x01);
      pulse(W_CLK);
    }

  pulse(FQ_UD);
}

uint32_t AD9850::calc_phase_delta(double f)
{
  uint32_t dphi = f * 4294967296.0 / EX_CLK;
  return dphi;
}

void AD9850::set_phase_delta(uint32_t dphi)
{
  phase_delta = dphi;
  update();
}

void AD9850::set_freq(double f)
{
  phase_delta = calc_phase_delta(f);
  update();
}

void AD9850::set_phase(uint8_t p)
{
  phase = p << 3;
  update();
}

void AD9850::down()
{
  pulse(FQ_UD);
  uint8_t p = 0x04;

  for (int i = 0; i < 8; i++, p >>= 1)
    {
      digitalWrite(D7, p & (uint8_t)0x01);
      pulse(W_CLK);
    }
  pulse(FQ_UD);
}

void AD9850::up()
{
  update();
}
