/*
	Lab 1: Morse Code 
  * Created: 2/6/2026 
  * Author : Victoria Lopez-Villa 
*/

// 16MHz clock from the debug processor
#define F_CPU 16000000UL 
#include <avr/io.h>
#include <util/delay.h>

// Setting the unit of time (this will not change)
// needs to be a constant to be passed into _delay_ms() function - (1 unit = 200ms)
#define UNIT 200	

// Morse codes that correspond to the alphabet and numbers
// Initializing it here because order matters (error otherwise)
const char* morse_codes[] = {
	//Letters
	/* A */ ".-", /* B */ "-...", /* C */ "-.-.", /* D */ "-..", /* E */ ".", 
  /* F */ "..-.", /* G */ "--.", /* H */ "....", /* I */ "..", /* J */ ".---", 
  /* K */ "-.-", /* L */ ".-..",  /* M */ "--", /* N */ "-.", /* O */ "---", 
  /* P */ ".--.", /* Q */ "--.-", /* R */ ".-.", /* S */ "...", /* T */ "-", 
  /* U */ "..-", /* V */ "...-", /* W */ ".--", /* X */ "-..-", /* Y */ "-.--", /* Z */ "--..",

	// Numbers starting at index 26
	/* 0 */ "-----", /* 1 */ ".----", /* 2 */ "..---", /* 3 */ "...--", 
  /* 4 */ "....-", /* 5 */ ".....", /* 6 */ "-....", /* 7 */ "--...", 
  /* 8 */ "---..", /* 9 */ "----."
};

// Function Definitions
// On Function: turns on the LED
// We use portb5 because the LED is connected to that pin on the board
void on()
{
	PORTB |= (1<<PORTB5);
}

// Off Function: turns off the LED
// clears the 5th bit to turn off the LED
void off(){
	PORTB &= ~(1<<PORTB5);
}

// Morse functions
void dot(){
	// turn the LED on and then off after one unit of time
	on();
	_delay_ms(UNIT);
	off();
}

void dash(){
	on();
	_delay_ms(3*UNIT);
	off();
}

// Space functions
void space_same_letter(){
	_delay_ms(UNIT);
}

void space_different_letter(){
	_delay_ms(3 * UNIT);
}

void space_between_words(){
	_delay_ms(7*UNIT);
}

void translate_morse(char* str) {
	// declaring index variable
	int index;

	// int flags
	int is_new_word = 1;

	// Traverse through string sent through
	for (int i = 0; str[i] != '\0'; i++) {
		index = -1;
		char c = str[i];

		// check to see if the letter is lowercase
		// if it is then we convert it to uppercase according to its ascii value
		if (c >= 'a' && c <= 'z')
			c -= 32;

		// if the character is uppercase then it allows for the letter to be matched to the corresponding morse code
		// in the morse_codes array
		if (c >= 'A' && c <= 'Z')
			index = c - 'A';

		// if the character is a numerical character
		if (c >= '0' && c <= '9')
			index = c - '0' + 26;
		// checking to see if the current element is a space
		// if true insert 7 unit delay
		if(c == ' ')
		{
			space_between_words();
			is_new_word = 1;	// set flag
			continue;			// next character is start of new word
		}

		// when the index is valid between the values of 0 to 36
		// then match the character to its corresponding Morse code
		if (index >= 0 && index < 36)
		{
			// checking if element is first letter of a word, if not then add letter spacing
			if(!(is_new_word))
				space_different_letter();
			
			// define the Morse code for the character
			// storing the morse code patter for the string
			const char* code = morse_codes[index];
			
			// based on the symbols call the corresponding function
			for (int j = 0; code[j] != '\0'; j++) 
			{	
				//call dot function to turn on LED
				if (code[j] == '.')
					dot();
				// call dash function to turn on LED for longer
				else if (code[j] == '-')
					dash();

				// adds spacing between the dots/dashes of each letter
				// but ensures it is not at the last letter
				if(code[j+1] != '\0')
					space_same_letter();
			}
			// once it is a new word set the is_new_word var to false (0)
			is_new_word = 0;
		}
	}
}

int main(void) {
	// setting PB5 as output
	// setting data direction
	DDRB |= (1<<DDB5);

	// message into one string
	char name[] = "Victoria 123456789";

	// infinite loop 
	while(1){
		// translating message into Morse
		translate_morse(name);
		_delay_ms(5000);	// waiting 5 seconds before it runs again
	}
	return 0;
}

