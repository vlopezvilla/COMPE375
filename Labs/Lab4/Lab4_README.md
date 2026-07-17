# Lab 4: Button-Controlled LED Brightness (Software PWM)

## Overview

This program dims and brightens an LED using software-timed PWM on Timer0, controlled by a single pushbutton. Holding the button down increases the LED's brightness (duty cycle); releasing it decreases the brightness, until it eventually turns fully off.

## Hardware Requirements

- AVR board with an ATmega328-family MCU, running at 16 MHz
- LED connected to `PB5`
- Pushbutton connected to `PB7`, wired to pull the pin LOW when pressed (internal pull-up is enabled in software, so no external resistor is needed)

## How It Works

### Timer0 Configuration

Timer0 is set up in **CTC (Clear Timer on Compare Match) mode** with a prescaler of 1024, producing a ~10 ms PWM period:

```
f_timer = F_CPU / prescaler = 16,000,000 / 1024 = 15,625 counts/sec
T_tick  = 1 / f_timer        = 64 µs per count
period  = 10 ms / 64 µs      ≈ 156 counts  →  OCR0A = 155
```

- **`OCR0A`** (fixed at 155) defines the total period of one PWM cycle (~10 ms).
- **`OCR0B`** (starts at 0) defines how long, within that period, the LED stays on — this is the value the button adjusts to change brightness.

### Pin Setup

| Pin | Direction | Purpose |
|---|---|---|
| `PB7` | Input, pull-up enabled | Button (LOW = pressed) |
| `PB5` | Output | LED |

### PWM / Brightness Loop

Each pass through the main loop performs one manual PWM cycle:

1. **Turn the LED on**, then busy-wait until the timer counter reaches `OCR0B` (the `OCF0B` compare flag sets). This is the "on" portion of the duty cycle.
2. Clear the `OCF0B` flag, then **turn the LED off**, and busy-wait until the timer reaches `OCR0A` (the `OCF0A` flag sets), completing the ~10 ms period.
3. Clear the `OCF0A` flag, which allows the cycle to restart.
4. **Check the button and adjust brightness:**
   - If the button is pressed (`PINB` bit 7 reads LOW) and the duty cycle hasn't maxed out (`OCR0B < OCR0A`), increment `OCR0B` — the LED stays on longer each cycle, so it appears brighter.
   - If the button is released (bit 7 reads HIGH) and the duty cycle is above 0, decrement `OCR0B` — the LED stays on for less time each cycle, so it dims.

Because this runs once per ~10 ms cycle, holding the button gradually ramps brightness up to 100%, and releasing it gradually ramps it back down to 0%, rather than snapping instantly.

## Building & Flashing

Written for the AVR toolchain (`avr-gcc` + `avrdude`) or an IDE such as Microchip Studio / Atmel Studio / PlatformIO targeting an ATmega328-series MCU at 16 MHz.

Example command-line build (adjust MCU/port as needed):

```bash
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -o lab4.elf Lab4.c
avr-objcopy -O ihex lab4.elf lab4.hex
avrdude -c arduino -p atmega328p -P /dev/ttyACM0 -U flash:w:lab4.hex:i
```

## Customization

- **Change PWM period/resolution:** adjust `OCR0A` (higher = longer period, finer possible brightness steps, but slower overall PWM frequency).
- **Change ramp speed:** since brightness changes by 1 count per ~10 ms cycle, `OCR0A`'s value indirectly sets how fast brightness ramps from 0% to 100%.
- **Change pins:** update the `DDRB`/`PORTB`/`PINB` bit references if the LED or button are wired to different pins.

## Notes

- This is a fully software-driven (busy-wait) PWM implementation rather than using the timer's built-in hardware PWM output — the CPU spends the entire loop polling `TIFR0` flags instead of doing other work.
- Brightness changes only one step per ~10 ms cycle, so reaching full brightness from off takes roughly `OCR0A` cycles (~1.5 seconds at the current settings).
