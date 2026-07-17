# Lab 3: 4x4 Keypad Scanning with UART Output

## Overview

This program scans a 4x4 matrix keypad connected to an ATmega328PB and transmits the character corresponding to whichever key is pressed over UART at 9600 baud. It continuously scans in a loop, so key presses are reported in near real time.

## Hardware Requirements

- AVR board with an ATmega328PB (or compatible), running at 16 MHz
- 4x4 matrix keypad wired as follows:
  - **Rows** → `PD4`, `PD5`, `PD6`, `PD7` (outputs)
  - **Columns** → `PB0`, `PB1`, `PB2`, `PB3` (inputs, internal pull-ups enabled)
- USB-to-serial connection or onboard debugger with a virtual COM port
- A serial terminal on the host computer (e.g., PuTTY, Tera Term, Arduino Serial Monitor) set to **9600 baud, 8 data bits, no parity, 1 stop bit (8N1)**

## Keypad Layout

The `keypad[4][4]` array maps physical row/column positions to characters:

| | Col 0 | Col 1 | Col 2 | Col 3 |
|---|---|---|---|---|
| **Row 0** | 1 | 2 | 3 | A |
| **Row 1** | 4 | 5 | 6 | B |
| **Row 2** | 7 | 8 | 9 | C |
| **Row 3** | * | 0 | # | D |

## How It Works

### Baud Rate Setup

```c
#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)
```

Using the standard AVR formula `UBRR = F_CPU / (16 * BAUD) - 1`, with `F_CPU = 16,000,000` and `BAUD = 9600`, this evaluates to **103**.

### Core Functions

| Function | Purpose |
|---|---|
| `uart_init()` | Sets the baud rate registers, enables transmit/receive (`TXEN0`, `RXEN0`), and configures 8-bit data frames (`UCSZ00`, `UCSZ01`) |
| `uart_transmit(unsigned char data)` | Waits until the transmit buffer is empty (`UDRE0` flag), then sends a byte via `UDR0` |

### Keypad Scanning (Row/Column Multiplexing)

Matrix keypads use fewer pins than a full grid of switches by scanning one row at a time:

1. **Setup:**
   - Rows (`PD4`–`PD7`) are configured as outputs and driven HIGH by default.
   - Columns (`PB0`–`PB3`) are configured as inputs with internal pull-ups enabled, so they read HIGH when no button connects them to a row.

2. **Scan loop** (runs forever):
   - For each row `i` (0–3): that row is pulled LOW while the other rows stay HIGH.
   - For each column `j` (0–3): the program checks if that column reads LOW (`PINB & (1<<j)` is 0). If a button at `(row i, col j)` is pressed, it briefly connects the LOW row to that column, pulling the column LOW.
   - If a press is detected, `uart_transmit()` sends the matching character from `keypad[i][j]`, and a 200 ms delay provides basic debounce/repeat-rate control.
   - The row is then set back HIGH before moving to the next row.

3. This process repeats continuously, so any key press is picked up on the next scan pass.

## Building & Flashing

Written for the AVR toolchain (`avr-gcc` + `avrdude`) or an IDE such as Microchip Studio / Atmel Studio / PlatformIO targeting an ATmega328PB at 16 MHz.

Example command-line build (adjust MCU/port as needed):

```bash
avr-gcc -mmcu=atmega328pb -DF_CPU=16000000UL -Os -o lab3.elf Lab_3.c
avr-objcopy -O ihex lab3.elf lab3.hex
avrdude -c arduino -p atmega328pb -P /dev/ttyACM0 -U flash:w:lab3.hex:i
```

Then open a serial terminal at **9600 baud** and press keys on the keypad to see the corresponding characters appear.

## Customization

- **Remap keys:** edit the `keypad[4][4]` array to change which character is sent for each position.
- **Change scan/repeat rate:** adjust the `_delay_ms(200)` value — shorter delays make repeated presses register faster but may increase bounce/false triggers.
- **Change baud rate:** update `BAUD` (must match the terminal's baud rate setting).
- **Change pin mapping:** update the `DDRD`/`DDRB`/`PORTD`/`PORTB` bit assignments if wiring differs.

## Notes

- Debouncing here is handled only by the fixed 200 ms delay after a detected press; a held key will repeatedly transmit its character roughly every 200 ms until released.
- No newline/carriage return is sent between keypresses, so output in a terminal will appear as one continuous stream of characters.
