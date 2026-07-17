# Lab 2: UART Serial Transmission

## Overview

This program configures the ATmega328PB's USART hardware to transmit a string of characters (a "RedID") over a serial connection, one character at a time, followed by a carriage return and line feed. It repeats forever.

By default, the program transmits:

```
123456789
```

with a 500 ms delay between each character, then sends `\r\n` and starts over.

## Hardware Requirements

- AVR board with an ATmega328PB (or compatible), running at 16 MHz
- USB-to-serial connection or onboard debugger with a virtual COM port (TX on `PD1`)
- A serial terminal on the host computer (e.g., PuTTY, Tera Term, Arduino Serial Monitor, `screen`) set to **9600 baud, 8 data bits, no parity, 1 stop bit (8N1)**

## How It Works

### Baud Rate Setup

```c
#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)
```

This computes the UBRR (UART Baud Rate Register) value using the standard AVR formula:

```
UBRR = F_CPU / (16 * BAUD) - 1
```

With `F_CPU = 16,000,000` and `BAUD = 9600`, this evaluates to **103**.

### Core Functions

| Function | Purpose |
|---|---|
| `uart_init()` | Loads the baud rate into `UBRR0H`/`UBRR0L`, enables the transmitter and receiver (`TXEN0`, `RXEN0`), and sets the frame format to 8 data bits (`UCSZ00`, `UCSZ01`) |
| `uart_transmit(unsigned char c)` | Waits until the transmit data register is empty (`UDRE0` flag), then writes the character to `UDR0` to send it |
| `uart_recieve(void)` | Waits until a byte has been received (`RXC0` flag), then reads and returns it from `UDR0` |
| `main()` | Initializes UART, then loops forever sending each character of the RedID string with a 500 ms delay, followed by `\r\n` |

### Transmission Flow

1. `uart_init()` configures the USART0 peripheral for 9600 baud, 8N1.
2. `main()` loops through each character in `redID[]` until the null terminator.
3. Each character is sent via `uart_transmit()`, with a 500 ms pause between characters (useful for watching the output arrive live in a terminal).
4. After the full string is sent, a carriage return (`\r`) and line feed (`\n`) are transmitted to start a new line.
5. The whole process repeats indefinitely.

## Building & Flashing

Written for the AVR toolchain (`avr-gcc` + `avrdude`) or an IDE such as Microchip Studio / Atmel Studio / PlatformIO targeting an ATmega328PB at 16 MHz.

Example command-line build (adjust MCU/port as needed):

```bash
avr-gcc -mmcu=atmega328pb -DF_CPU=16000000UL -Os -o lab2.elf Lab_2.c
avr-objcopy -O ihex lab2.elf lab2.hex
avrdude -c arduino -p atmega328pb -P /dev/ttyACM0 -U flash:w:lab2.hex:i
```

Then open a serial terminal at **9600 baud** on the corresponding port to view the output.

## Customization

- **Change the transmitted ID:** edit the `redID[]` string in `main()`.
- **Change the baud rate:** update `BAUD` (must match the terminal's baud rate setting).
- **Change transmission speed:** adjust the `_delay_ms(500)` value between characters.

## Notes

- `uart_recieve()` is defined but not currently called in `main()` — the program only transmits.
- There's a minor logic quirk in `uart_recieve()`: `while((!(UCSR0A)) & (1<<RXC0));` negates the entire `UCSR0A` register rather than just the `RXC0` bit, so it doesn't correctly poll the receive-complete flag. The standard, correct pattern is `while (!(UCSR0A & (1<<RXC0)));`.
