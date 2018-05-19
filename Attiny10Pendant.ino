 /* POV Pendant

   David Johnson-Davies - www.technoblogy.com - 19th May 2017
   ATtiny10 @ 1MHz (internal oscillator; BOD disabled)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

#include <avr/io.h>
#include <stdint.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

const int Green = 0;
const int Red = 1;
const int Blue = 2;
const int Colours = 6;

volatile int Output = 0;
unsigned int Seed = 1;

unsigned int Random (void) {
  int Temp = Seed & 1;
  Seed = Seed >> 1;
  if (Temp == 1) Seed = Seed ^ 0xB400;
  return Seed;
}

void delay (unsigned long millis) {
  for (volatile unsigned long i = 23*millis; i!=0; i--);
}

// Turn next light on
ISR(TIM0_OVF_vect) {
  Output = (Output+1) % Colours;
  PORTB = Output+1;
}

// Turn all lights off
ISR(TIM0_COMPA_vect) {
  PORTB = 0xFF;
}

int main (void) {
 sei();
 DDRB = 0x07;
 TCCR0A = 2<<WGM00;             // No outputs, fast PWM, Top=ICR0
 TCCR0B = 3<<WGM02 | 1<<CS00;   // Divide clock by 1
 ICR0 = 4095;
 OCR0A = 1;
 TIMSK0 = 1<<OCIE0A | 1<<TOIE0; // OVF and COMPA interrupts
 set_sleep_mode(SLEEP_MODE_PWR_DOWN);
 for(;;) {
   for (int b=0; b<=4050; b++) {
    OCR0A = b;
    delay(2);
   }
   for (int b=4050; b>=0; b--) {
    OCR0A = b;
    delay(2);
   }
   PORTB = 0xFF;  // Turn off LEDs
   sleep_enable();
   sleep_cpu();
 }
}
