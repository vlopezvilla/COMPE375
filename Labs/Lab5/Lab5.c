/*
* Lab 5 Music Notes
*
* Created: 3/27/2026
* Author : Victoria Lopez-Villa
*/

// CTC mode, duty cycle does not change, calculation of frequency
// OCRA = period, OCRB = duty cycle
#define F_CPU 16000000UL // setting clock speed
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
// 4x4 keypad map
unsigned char keypad[4][4] = {{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}};

// storing frequencies for 16 keys
double freq[16];

// Initialize frequencies using the provided formula: T_ON = 1/(440*(2^(1/12))^x)
// which becomes 1/T_ON = (440*(2^(1/12))^x)
// x represents the index number of keypad buttons
void init_frequencies()
{
    // iterating through each of the elements in the keypad which is 16 total (4x4)
    for (int x = 0; x < 16; x++){
      freq[x] = 440.0 * pow(2.0, x / 12.0);
    }
}

// using timer 1 instead of timer0 because timer1 is 16bits register
void timer1_init()
{
  TCCR1A = 0;
  TCCR1B = (1 << WGM12); // CTC mode
}

// Play tone on PC5
void play_tone(double f)
{
  // preventing invalid freq
  if (f <= 0) 
    return;
  // Converting the frequency to OCR and we know that:
  // OCR = (F_CPU / (2 * prescaler * f)) - 1
  int prescaler = 256;
  
  //ocr is a double floating num
  double ocr = (F_CPU / (2.0 * prescaler * f)) - 1;
  
  // 16 bit integer that is being set to the casted 16 int value of ocr
  // Computes timer val, converts to int, loading it into timer1 comp register
  OCR1A = (uint16_t)ocr;
  
  //TCCR1B |= (1 << CS11); // prescaler = 8
  // enabling the prescale value to 265 using the datasheet
  // this will the start the timer
  TCCR1B |= (1 << CS12); // prescaler = 256
  
  while (1)
  {
    //toggling to generate the square wave
    // this is synchronizes using Timer1 to control freq
    PORTC ^= (1 << 5); // toggle speaker
    while (!(TIFR1 & (1 << OCF1A))); // wait for COMP flag
    TIFR1 |= (1 << OCF1A); // clear flag
      
    // break if key released
    if ((PINB & 0x0F) == 0x0F) break;
  }
  
// stop timer
TCCR1B &= ~(1 << CS12);
}
int main(void){
  // building an array of musical notes
  init_frequencies();

  // configures Timer1 in CTC mode
  timer1_init();

  // code from Lab 3
  // PC5 as output (speaker)
  DDRC |= (1 << 5);

  // Rows PD4–PD7 output
  DDRD |= (1<<DDD4)|(1<<DDD5)|(1<<DDD6)|(1<<DDD7);

  // Columns PB0–PB3 input
  DDRB &= ~((1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3));

  // Enable pull-ups
  PORTB |= (1<<0)|(1<<1)|(1<<2)|(1<<3);

  // Set all rows HIGH
  PORTD |= (1<<4)|(1<<5)|(1<<6)|(1<<7);

  while (1){
    for (int i = 0; i < 4; i++){
      PORTD &= ~(1 << (i + 4)); // setting row LOW
      for (int j = 0; j < 4; j++){
        if (!(PINB & (1 << j))) { // button pressed 
          int index = i * 4 + j; // 0–15
          // using the frquency table to send to correct tone
          play_tone(freq[index]);
          // debounce delay to prevent triggers from one press
          _delay_ms(150);
        }
      }
    PORTD |= (1 << (i + 4)); // row HIGH
  }
}
