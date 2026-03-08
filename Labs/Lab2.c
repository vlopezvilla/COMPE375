/*
 * Lab_2.c
 *
 * Created: 2/6/2026 12:56:15 PM
 * Author : Victoria Lopez-Villa
 */ 

// 16MHz clock from the debug processor
#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>

// defining macros (lower baud rate slower transmission/ higher baud rate faster transmission)
// serial communication speed (bits/sec)
#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)	
// formula shown in lecture -> [16 MHz / (16 * 9600)] -1 = 103
											
// (pin to transmit data is PD1 on ATmega328pb)
// configures USART hardware
void uart_init(void)
{
	UBRR0H = (BAUDRATE>>8);	// shift register 8 bits - sets high byte to baudrate
	UBRR0L = BAUDRATE;		// (lower 8 bits) - the low byte of baudrate register
	UCSR0B |= (1<<TXEN0) | (1<<RXEN0);    // ENABLES transmission/receiving of data
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);	// (8-bit data format) SETS 8 DATA bits
				                                // UCSZ00 and UCSZ01 = 1, but UCSZ02 = 0 means it is 8 bits
}

//transmit function
void uart_transmit(unsigned char redID)
{
	while (!(UCSR0A & (1<<UDRE0))); 		// wait until transmit buffer is ready
	UDR0 = redID;					              // SENDING DATA - writes char in UART
                                      // data register prompts transmission
}

//receiving data
unsigned char uart_recieve (void)	// received character from UART
{
	while((!(UCSR0A)) & (1<<RXC0)); 		// wait while data is being received
	return UDR0;					              // return 8-bit data
}

int main(void)
{
	// calls uart_init function to configure UART hardware
	uart_init();
	
	// character array containing the 8-digit RedID
	char redID [] = "123456789";

	while (1)
	{
		// iterating through each character of the RedID string until null char
		for (int i = 0; redID[i] != '\0'; i++)
		{
			// sends each character via UART
			uart_transmit(redID[i]);
			_delay_ms(500); 
		}
    
    // Carriage Return w/ line feed to add new line once it completes ID
		 uart_transmit('\r'); // CR
		 uart_transmit('\n'); // LF
	}
}


