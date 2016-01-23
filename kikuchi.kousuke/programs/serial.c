#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN        PB0
#define DELAY        500

#if F_CPU == 8000000UL
// 19200bps
#define SERIAL_TIME_PER_BIT1    52
#define SERIAL_TIME_PER_BIT2    51
#elif F_CPU == 4000000UL
// 9600bps
#define SERIAL_TIME_PER_BIT1    104
#define SERIAL_TIME_PER_BIT2    102
#elif F_CPU == 2000000UL
// 9600bps
#define SERIAL_TIME_PER_BIT1    103
#define SERIAL_TIME_PER_BIT2    100
#elif F_CPU == 1000000UL
// 4800bps
#define SERIAL_TIME_PER_BIT1    206
#define SERIAL_TIME_PER_BIT2    201
#endif

#define SERIAL_TX_PIN            PB1

void serialBegin()
{
    DDRB |= _BV(SERIAL_TX_PIN);
    PORTB |= _BV(SERIAL_TX_PIN);
}

void serialWrite(uint8_t data)
{
    PORTB &= ~(_BV(SERIAL_TX_PIN));    // start bit
    _delay_us(SERIAL_TIME_PER_BIT1);
    
    uint8_t i;
    for(i=1;i;i<<=1)
    {
        if(data&i)
        {
            PORTB |= _BV(SERIAL_TX_PIN);
        }
        else
        {
            PORTB &= ~(_BV(SERIAL_TX_PIN));
            asm volatile ("nop");
        }
        _delay_us(SERIAL_TIME_PER_BIT2);
    }
    
    PORTB |= _BV(SERIAL_TX_PIN);    // stop bit
    _delay_us(SERIAL_TIME_PER_BIT1);
}

int main(void)
{
    //CCP = 0xD8;
    //CLKMSR = 0x00;
    //CCP = 0xD8;
    //CLKPSR = 0x00;
    
    DDRB |= _BV(LED_PIN);
    
    serialBegin();
    PORTB ^= _BV(LED_PIN);
    _delay_ms(DELAY);
    PORTB ^= _BV(LED_PIN);
    _delay_ms(DELAY);
    PORTB ^= _BV(LED_PIN);
    
    serialWrite('H');
    serialWrite('e');
    serialWrite('l');
    serialWrite('l');
    serialWrite('o');
    serialWrite(' ');
    serialWrite('W');
    serialWrite('o');
    serialWrite('r');
    serialWrite('l');
    serialWrite('d');
    serialWrite('!');
    
    while(1)
    {
        PORTB ^= _BV(LED_PIN);
        _delay_ms(DELAY);
    }
}