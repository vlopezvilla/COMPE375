/*
 * Lab4.c
 * Author : Victoria Lopez-Villa
*/

// CPU clock frequency = 16 MHz
#define F_CPU 16000000UL 
// AVR I/O register definitions
#include <avr/io.h>            

int main(void)
{
	// 16MHZ / prescale -> 16MHZ / 1024 =  
    // prescaler options are: 1, 8, 64, 256, 1024 (from datasheet)
	 // We choose prescaler = 1024 because it slows the timer enough
	 // to produce a ~10 ms period using an 8-bit timer.
	
	// Timer frequency calculation:
	// f_timer = f_CPU / prescaler = 16,000,000 / 1024 = 15625 counts per second

	// Time per timer count:
	// T_tick = 1 / f_timer = 1 / 15625 = 0.000064 seconds = 64 µs
	
	// desired PWM period to be 10ms
	// counts = 0.01 / 0.000064 approx ~ 156 counts
	
	// A register is the period
	// B register controls how long the LED stays on
	
	OCR0A = 155;                   // Timer max count = 155 → period = 156 counts → ~10 ms
    OCR0B = 0;                   // Compare value for duty cycle → initially 0% (LED OFF)

    DDRB &= ~(1<<7);             // Set PB7 as input (button)
    PORTB |= (1<<7);             // Enable internal pull-up on PB7 so when it is not pressed = HIGH, pressed = LOW
    DDRB |= (1<<5);              // Set PB5 as output (LED)

    TCCR0A |= (1<<WGM01);				 // Enable CTC mode according to the datasheet
	
	// Setting the prescale value according to the datasheet
    TCCR0B |= (1<<CS02) | (1<<CS00);	// Set prescaler = 1024 -> timer tick = 64 microseconds

    while(1)
    {
		// When button is pressed duty cycle is incremented
        PORTB |= (1<<5);                    // Turn LED ON
        while ( (TIFR0 & (1<<OCF0B)) == 0); // Wait until timer reaches OCR0B -> turns on
        TIFR0 |= (1<<OCF0B);                // Clear OCF0B flag

        PORTB &= ~(1<<5);                   // Turn LED OFF
        while ( (TIFR0 & (1<<OCF0A)) == 0); // Wait until timer reaches OCR0A -> turns off
        TIFR0 |= (1<<OCF0A);                // Clear OCF0A flag
		
		    // this enables so the timer/duty cycle restarts
        if(!(PINB & (1<<7)) && (OCR0B < OCR0A)) // Button pressed & duty cycle < 100%
        {
            OCR0B++;                           // Increase turn on which makes the LED gets brighter
        }

        if((PINB & (1<<7)) && (OCR0B > 0))    // Button released & duty cycle > 0%
        {
            OCR0B--;                         // Decrease the brightness which dims the light
        }
    }
}
