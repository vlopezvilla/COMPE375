# Lab 1: Morse Code

## Overview

This program blinks an onboard LED to output a message in Morse code. It runs on an AVR microcontroller (ATmega328P-style board) at 16 MHz, using `PB5` (the Arduino Uno's built-in LED pin) as the output.

By default, the program continuously blinks the message:

```
Victoria 123456789
```

waiting 5 seconds between each repetition.

## Hardware Requirements

- AVR-based board with an ATmega328P (or compatible), 16 MHz clock
- Onboard LED connected to `PB5` (e.g., Arduino Uno's built-in LED)
- Programmer/debugger set up to flash the AVR (e.g., AVR-ISP, avrdude, or Atmel Studio/Microchip Studio debug processor)

## How It Works

### Timing Unit

All Morse timing is derived from a single base unit:

```c
#define UNIT 200  // 1 unit = 200 ms
```

| Element | Duration |
|---|---|
| Dot | 1 unit (200 ms) |
| Dash | 3 units (600 ms) |
| Space between symbols in the same letter | 1 unit |
| Space between letters | 3 units |
| Space between words | 7 units |

### Morse Code Table

The `morse_codes[]` array holds Morse patterns for `A`–`Z` (indices 0–25) followed by `0`–`9` (indices 26–35). Order matters here since letters/digits are looked up by computing an array index directly from the ASCII character.

### Core Functions

| Function | Purpose |
|---|---|
| `on()` / `off()` | Set/clear `PORTB5` to turn the LED on or off |
| `dot()` | LED on for 1 unit |
| `dash()` | LED on for 3 units |
| `space_same_letter()` | 1-unit pause between symbols within a letter |
| `space_different_letter()` | 3-unit pause between letters |
| `space_between_words()` | 7-unit pause between words |
| `translate_morse(char* str)` | Parses a string and blinks each character's Morse code |
| `main()` | Configures `PB5` as output, then loops forever, translating the message every 5 seconds |

### Translation Logic

`translate_morse()` walks through the input string character by character:

1. Lowercase letters are converted to uppercase.
2. Letters (`A`-`Z`) map to indices `0`-`25`; digits (`0`-`9`) map to indices `26`-`35`.
3. A space character triggers a 7-unit word gap and marks the next character as the start of a new word.
4. For valid characters, a letter gap is inserted before the symbol (unless it's the first letter of a word), then each `.`/`-` in its Morse pattern is blinked, with 1-unit gaps between symbols in the same letter.

## Building & Flashing

This is written for the AVR toolchain (`avr-gcc` + `avrdude`) or an IDE such as Microchip Studio / Atmel Studio / PlatformIO configured for an ATmega328P at 16 MHz.

Example command-line build (adjust MCU/port as needed):

```bash
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -o morse.elf morse.c
avr-objcopy -O ihex morse.elf morse.hex
avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U flash:w:morse.hex:i
```

## Customization

- **Change the message:** edit the `name[]` string in `main()`.
- **Change the speed:** adjust `UNIT` (in milliseconds) — smaller values blink faster.
- **Change the output pin:** update the `PORTB5`/`DDB5` references in `on()`, `off()`, and `main()` to use a different pin.

## Notes

- Only letters, digits, and spaces are supported; any other character is silently skipped (its `index` stays `-1` and it fails the bounds check).
- `translate_morse()` blocks (via `_delay_ms`) for the entire duration of the message, so nothing else runs on the MCU while it's blinking.
