#include <AD9850.h>
#define pulse(pin) { digitalWrite(pin, HIGH); digitalWrite(pin, LOW); }

AD9850::AD9850(char fq_ud, char w_clk, char d7)
    : FQ_UD(fq_ud), W_CLK(w_clk), D7(d7) {
    frequency = 0;
    phase = 0;
    pinMode(FQ_UD, OUTPUT);
    pinMode(W_CLK, OUTPUT);
    pinMode(D7, OUTPUT);
    pulse(W_CLK);
    pulse(FQ_UD);
}

void AD9850::update() {
    uint32_t f = frequency * 4294967296.0 / EX_CLK;
    for (int i = 0; i < 32; i++, f >>= 1) {
        digitalWrite(D7, f & (uint32_t)0x00000001);
        pulse(W_CLK);
    }
    uint8_t p = phase << 3;
    for (int i = 0; i < 8; i++, p >>= 1) {
        digitalWrite(D7, p & (uint8_t)0x01);
        pulse(W_CLK);
    }
    pulse(FQ_UD);
}

void AD9850::setfreq(double f) {
    frequency = f;
    update();
}
void AD9850::setphase(uint8_t p) {
    phase = p;
    update();
}

void AD9850::down() {
    pulse(FQ_UD);
    uint8_t p = 0x04;
    for (int i = 0; i < 8; i++, p >>= 1) {
        digitalWrite(D7, p & (uint8_t)0x01);
        pulse(W_CLK);
    }
    pulse(FQ_UD);
}

void AD9850::up() { update(); }
