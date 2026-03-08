/* * Lab_3.c * 
* Created: 2/13/2026 
* Author : Victoria Lopez-Villa 
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

// setting the baud rate
#define BAUD 9600
// using the formula to find baudrate = 103
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1) 

// creating 2D array representing all the possible characters on the num pad
unsigned char keypad[4][4] = {{'1', '2', '3', 'A'},
				    {'4', '5', '6', 'B'},
				    {'7', '8', '9', 'C'},
				    {'*', '0', '#', 'D'}};

// configuring USART hardware
void uart_init(void)
{
	UBRR0H = (BAUDRATE>>8);  // shift the register by 8 bits (upper 8 bits) - sets high byte of baud rate register
	UBRR0L = BAUDRATE;			   // lower 8 bits set
	UCSR0B |= (1<<TXEN0) | (1<<RXEN0);  //ENABLES transmission/receiving of data
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);	// UCSZ00 and UCSZ01 = 1, but UCSZ02 = 0 means it is 8 bits
}

// transmit function
void uart_transmit(unsigned char data)
{
	while (!(UCSR0A & (1<<UDRE0)));		// wait until transmit buffer is ready
	UDR0 = data;	   // SENDING DATA - writes character into USART data register prompts transmission
}


int main(void)
{
	uart_init();

	// Rows PD4–PD7 as output pins
	DDRD |= (1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);

	// Columns PB0–PB3 as input pins
	DDRB &= ~((1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3));

	// Pull-ups on columns -- makes default state HIGH
	// without pull-ups they would be "floating" (not connected to power/gnd)
	// (we only use pull-ups on input pins since they read voltage)
	PORTB |= (1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3);

	// Set all rows HIGH
	PORTD |= (1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7);

	while (1)
	{
		// iterating through the rows
		for (int i=0; i<4; i++)
		{
			// setting the current row LOW
			PORTD &= ~(1<<(i+4));

			// iterating through the columns
			for (int j=0; j<4; j++)
			{
				// if the button in that row is pressed it means col has been set to low
				if (!(PINB & (1<<j)))   // column LOW = pressed
				{
					// calling transmit function passing the 2D array of the keypad
					// based on what is pressed
					// the position of the button that is pressed is determined and matched to the 4x4 map
					uart_transmit(keypad[i][j]);
					_delay_ms(200);     
				}
			}

			// Set row back HIGH
			PORTD |= (1<<(i+4));
			
			// when no button is pressed (each row is still put to low one-by-one
			// columns stay high because of pull up resistors
			
		}
	}
}
