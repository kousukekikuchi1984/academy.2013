//
//
// hello.stepper.44.full.c
//
// full stepping hello-world
//
// Neil Gershenfeld
// 11/14/10
//
// (c) Massachusetts Institute of Technology 2010
// Permission granted for experimental and personal use;
// license for commercial sale available from MIT.
//

#include <avr/io.h>
#include <util/delay.h>

#define output(directions,pin) (directions |= pin) // set port direction for output
#define set(port,pin) (port |= pin) // set port pin
#define clear(port,pin) (port &= (~pin)) // clear port pin
#define pin_test(pins,pin) (pins & pin) // test for port pin
#define bit_test(byte,bit) (byte & (1 << bit)) // test for bit set

#define MOSFET_port PORTA // MOSFET port
#define MOSFET_direction DDRA // MOSFET direction
#define brown (1 << PA0) // MOSFET output pins
#define black (1 << PA1) // "
#define yellow (1 << PA2) // "
#define orange (1 << PA3) // "
#define on_delay() _delay_us(50) // PWM on time
#define off_delay() _delay_us(10) // 10// PWM off time
#define PWM_count 200 // number of PWM cycles

// LED
#define sbi(BYTE,BIT) BYTE|=_BV(BIT)
#define cbi(BYTE,BIT) BYTE&=~_BV(BIT)

static uint8_t count;

//
// yellow, brown PWM pulse
//
void pulse_yellow_brown() {
   for (count = 0; count < PWM_count; ++count) {
      set(MOSFET_port, yellow);
      set(MOSFET_port, brown);
      on_delay();
      clear(MOSFET_port, yellow);
      clear(MOSFET_port, brown);
      off_delay();
      }
   }
//
// yellow, black PWM pulse
//
void pulse_yellow_black() {
   for (count = 0; count < PWM_count; ++count) {
      set(MOSFET_port, yellow);
      set(MOSFET_port, black);
      on_delay();
      clear(MOSFET_port, yellow);
      clear(MOSFET_port, black);
      off_delay();
      }
   }
//
// orange, brown PWM pulse
//
void pulse_orange_brown() {
   for (count = 0; count < PWM_count; ++count) {
      set(MOSFET_port, orange);
      set(MOSFET_port, brown);
      on_delay();
      clear(MOSFET_port, orange);
      clear(MOSFET_port, brown);
      off_delay();
      }
   }
//
// orange, black PWM pulse
//
void pulse_orange_black() {
   for (count = 0; count < PWM_count; ++count) {
      set(MOSFET_port, orange);
      set(MOSFET_port, black);
      on_delay();
      clear(MOSFET_port, orange);
      clear(MOSFET_port, black);
      off_delay();
      }
   }
//
// clockwise step
//
void step_cw() {
   pulse_yellow_brown();
   pulse_yellow_black();
   pulse_orange_black();
   pulse_orange_brown();
   }
//
// counter-clockwise step
//
void step_ccw() {
   pulse_orange_brown();
   pulse_orange_black();
   pulse_yellow_black();
   pulse_yellow_brown();
   }

int main(void) {
   //
   // main
   //
   //static uint8_t i1, i2, i3, i4;
   //
   static uint8_t i1;
   // set clock divider to /1
   //
   CLKPR = (1 << CLKPCE);
   CLKPR = (0 << CLKPS3) | (0 << CLKPS2) | (0 << CLKPS1) | (0 << CLKPS0);
   //
   // initialize MOSFET pins
   //
   clear(MOSFET_port, brown);
   output(MOSFET_direction, brown);
   clear(MOSFET_port, black);
   output(MOSFET_direction, black);
   clear(MOSFET_port, yellow);
   output(MOSFET_direction, yellow);
   clear(MOSFET_port, orange);
   output(MOSFET_direction, orange);
    
    //DDRA = _BV(PORTB1);
    
   //
   // main loop
   //
   while (1) {
       // one rotation is 50
       for(i1=0; i1 < 50; i1++){
           step_ccw();
       }
      
       /*sbi(PORTB, PB1);
       for(i1=0; i1 < 200; i1++){
           step_cw();
       }
       cbi(PORTB, PB1);
       for (i2=0; i2 < 200; i2++){
           step_ccw();
       }*/
       
   }
}
