# Hardware Notes

## Target board

This repo is configured for:

- SparkFun Pro Micro 5V/16MHz (ATmega32U4)

## Power

- WS2812 LEDs can draw significant current at high brightness.
- If powering from VCC/USB, keep brightness conservative.

## Signal integrity

WS2812 data lines are timing-sensitive.

Recommended:

- Common ground between MCU and LEDs
- 220–470Ω series resistor on the data line
- 470–1000µF capacitor across LED power rails

## Pin mapping

The defaults are defined in `src/main.cpp` in `namespace Config`.

If you change pins, keep in mind:

- Some analog pins on ATmega32U4 boards have different Arduino pin numbers depending on the core/variant.
- Verify with your specific Pro Micro pinout.
