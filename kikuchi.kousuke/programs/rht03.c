#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned int seg[6];
unsigned int mx;

#define cbi(addr, bit)     addr &= ~(1<<bit)
#define sbi(addr, bit)     addr |=  (1<<bit)

// milisecond order delay
void delay_ms(unsigned int t)
{
    while(t--)
        _delay_ms(1);
}

// Returns mask 7-seg. display
unsigned int mask(unsigned int num) {
      switch (num) {
        case 0 : return 0xa0;
        case 1 : return 0xfc;
        case 2 : return 0xc1;
        case 3 : return 0xd0;
        case 4 : return 0x9c;
        case 5 : return 0x92;
        case 6 : return 0x82;
        case 7 : return 0xf8;
        case 8 : return 0x80;
        case 9 : return 0x90;
        case 10 : return 0xdf;  // -
        case 11 : return 0x83;    // E
        case 12 : return 0xcf;    // r
        case 99 : return 0xff;     // space
        default: return 0xfe;
    }
}

// Timer1 Overflow interrupt
ISR(TIMER1_OVF_vect)
{
    TCNT1 = 128;
    PORTA = seg[mx];
    switch(mx) {
        case 0: PORTB = ~_BV(PB6); break;
        case 1: PORTB = ~_BV(PB5); break; 
        case 2: PORTB = ~_BV(PB3); break;
        case 3: PORTB = ~_BV(PB2); break;
        case 4: PORTB = ~_BV(PB1); break; 
        case 5: PORTB = ~_BV(PB0); break;
    }
    mx++;
    if(mx > 5)
        mx = 0;
}

// read 1-wire sensor DHT22(RHT03)
void dht22_read(int *temp, int *humd)
{
    unsigned int i, j;
    unsigned char rs[5];
    unsigned int c;

    cli();

    // Host send start signal
    sbi(DDRA, DDA7);    // PA7 output
    cbi(PORTA, PA7);    // PA7 low
    delay_ms(1);
    // Host pull up and wait for sensor response
    cbi(DDRA, DDA7);    // PA7 input

    _delay_us(20);    // このディレイは最適化できていません。
    loop_until_bit_is_clear(PINA, PINA7);     // wait sensor send out response(20-40us)
    loop_until_bit_is_set(PINA, PINA7);        // wait sensor pull up get ready(80us)
    loop_until_bit_is_clear(PINA, PINA7);    // wait sensor send data(80us)

    // 40bit recive data 
    for(i = 0; i < 5; i++) {
           for(j = 0; j < 8; j++) {
            c = 0;
            loop_until_bit_is_set(PINA, PINA7);
            while(bit_is_set(PINA, PINA7)) {
                c++;
                _delay_us(1);
            }
            rs[i] <<= 1;
            if(c > 40)        // over 40count is "1"
                       rs[i] |= 1;
        }
    }

    sei();

    // check sum and value set
    if((rs[0] + rs[1] + rs[2] + rs[3] & 0xff) == rs[4]) {
        *humd = (rs[0] << 8 | rs[1]);
        *temp = (rs[2] << 8 | rs[3]);        // 室内での利用なので氷点下には対応しない
    } else {
        *humd = 1000;    //　エラー表示のための適当な値
        *temp = 1000;
    }
}

int main(void)
{
    unsigned int i;
    int tem, hum;

     DDRA = 0b01111111;        // PA0-6 7seg cathode, PA7 DHT22 1-wire
    DDRB = 0b11111111;        // 7seg Anode control

    TCNT1 = 128;             // Timer1 counter set
    TCCR1A= 0;                // Timer1 standard timer mode                
    TCCR1B= 8;                // Timer1 prescaler 1/128
    TIMSK = _BV(TOIE1);        // Timer1 Overflow Interrupt Enable

    delay_ms(2000);
    sei();

    while(1) {

        dht22_read(&tem, &hum);

        if(tem > 500 || tem < -99) {        // 表示は氷点下9.9℃まで対応している
            seg[0] = mask(11);
            seg[1] = mask(12);
            seg[2] = mask(12);
        } else {
            if(tem < 0) {
                seg[0] = mask(10);
            } else {
                i = (tem / 100) % 10;
                if(i == 0)
                    i = 99;
                seg[0] = mask(i);
            }
            i = abs((tem / 10) % 10);
            seg[1] = mask(i);
            i = abs(tem % 10);
            seg[2] = mask(i);
        }
        if(hum > 999 || hum < 0) {    // 湿度100%はありうるのでこれはマズイな。
            seg[3] = mask(11);
            seg[4] = mask(12);
            seg[5] = mask(12);
        } else {
            i = (hum / 100) % 10;
            if(i == 0)
                i = 99;
            seg[3] = mask(i);
            i = (hum / 10) % 10;
            seg[4] = mask(i);
            i = hum % 10;
            seg[5] = mask(i);
        }
        delay_ms(2000);    // DHT22での測定は2秒間隔
    }
}