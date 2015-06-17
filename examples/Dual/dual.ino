//
// Dual AD9850 oscillator controller
// M J Oldfield
// 17.vi.2015
//
// Connect two AD9850 modules to the Arduino, on the pins
// shown below.
//
// Connect the modules to a common master clock.
//
// Hook up the outputs to a scope in XY mode
//
// Connect to the sketch at 115200 baud, using an unbuffered
// terminal, so that characters are sent individually.
//
// Enjoy!

#include <AD9850.h>

// Oscillator objects: pins for W_CLK, FQ_UD, D7, RESET
// NB: Don't reuse these e.g. one pin or both resets.
AD9850 osc_1(8, 9, 10, 11);
AD9850 osc_2(4, 5,  6,  7);

//
// State variables. Don't set these directly: use the set_ functions
// below instead: that way the oscillator state will be updated too.
//
// You can read these though: I didn't get round to thinking of a better
// abstraction.
//
// The key idea is to have a common base frequency, plus a per-oscillator
// multiplier and tweak. This makes it easier to get exact ratios without
// worrying about rounding issues.
//
// There's also a single phase, which affects oscillator 2 only. In practice
// the two oscillators don't start in phase, so this is needed for manual
// tuning.
//
static uint8_t  mul_1 = 1;
static int8_t   d_1   = 0;
static uint8_t  mul_2 = 1;
static int8_t   d_2   = 0;
static uint8_t  phase = 0;
static double   base_f;
static uint32_t base_dphi;

static void print_state(void)
{
  Serial.println("Current state:");

  Serial.print("  base: ");
  Serial.print(base_f / 1.0e3, 3);
  Serial.print("kHz => "); 
  Serial.println(base_dphi, DEC);

  Serial.print("  osc1: ");
  print_osc_state(mul_1, d_1);

  Serial.print("  osc2: ");
  print_osc_state(mul_2, d_2);

  Serial.print("  phase: ");
  Serial.println(phase, DEC);

  Serial.println("");
}

static void print_osc_state(uint8_t m, int8_t d)
{
  Serial.print("base * ");
  Serial.print(m, DEC);
 
  const char *p = " + ";
  if (d < 0)
  {
    p = " - ";
    d = -d;
  }
  Serial.print(p);
  Serial.println(d, DEC);
}
 
static void set_osc_1(uint8_t m, int8_t d)
{
  mul_1  = m;
  d_1    = d;
  
  uint32_t dphi = base_dphi * mul_1 + d_1;
  osc_1.set_phase_delta(dphi);
  osc_1.set_phase(0);
}

static void set_osc_2(uint8_t m, int8_t d, uint8_t p)
{
  mul_2  = m;
  d_2    = d;
  phase  = p & 0x1f;
  
  uint32_t dphi = base_dphi * mul_2 + d_2;
  osc_2.set_phase_delta(dphi);
  osc_2.set_phase(phase);
}

static void set_base_f(double f)
{
  base_f    = f;
  base_dphi = osc_1.calc_phase_delta(f);
  
  set_osc_1(mul_1, d_1);
  set_osc_2(mul_2, d_2, 0);
}

//
// Some random text
//
static void print_hello(void)
{
  Serial.println("Dual AD9850 controller");
  Serial.println("M J Oldfield, 17.iv.2015");
  Serial.println("");
}

static void print_usage(void)
{
  print_hello();
  
  Serial.println("Controls:");
  Serial.println("  tweak phase:                 <,>"); 
  Serial.println("  change frequency multiplier: 1,..,9");
  Serial.println("  select second oscillator:    :");
  Serial.println("     so e.g. 2:3 does what you expect");
  Serial.println("  tweak frequency of osc 1:    k,l");
  Serial.println("  zero frequency tweaks:       z");
  Serial.println(""); 
  
  print_state();
}

//
// Main code starts here
//
void setup()
{
  set_base_f(1.0e4);
  
  Serial.begin(115200);
  print_hello();
}

void loop() 
{
  if (Serial.available() <= 0)
  {
    return;
  }
    
  char c = Serial.read();
    
  static int osc_sel = 1;
  switch(c)
  {
     // Change the phase
    case(','):
    case('<'):
      set_osc_2(mul_2, d_2, phase - 1);
      break;
      
    case('.'):
    case('>'):
      set_osc_2(mul_2, d_2, phase + 1);
      break;
      
    // Change the frequency multiplier
    case('1'):
    case('2'):
    case('3'):
    case('4'):
    case('5'):
    case('6'):
    case('7'):
    case('8'):
    case('9'):
      if (osc_sel == 1)
      {
        set_osc_1(c - '0', d_1);
      }
      else
      {
        set_osc_2(c - '0', d_2, phase);
        osc_sel = 1;
      }
      break;
    
    case(':'):
    case(';'):
      osc_sel = 2;
      break;
      
    // Tweak the frequency  
    case('k'):
      set_osc_1(mul_1, d_1 - 1);
      break;
      
    case('l'):
      set_osc_1(mul_1, d_1 + 1);
      break;
      
    case('z'):
      set_osc_1(mul_1, 0);
      set_osc_2(mul_2, 0, phase);
      break;
     
    // Miscellanea
    case('\r'): 
    case('\n'):
      print_state();
      break;
      
    default:
      print_usage();
      break;
  } 
}
