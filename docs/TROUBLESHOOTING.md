# Troubleshooting

## Upload problems (Pro Micro / ATmega32U4)

### Upload keeps failing / can’t find device

- Try a different USB cable (many are charge-only).
- Double-tap reset on the Pro Micro to enter bootloader, then immediately run upload again.
- If PlatformIO picks the wrong port, run:
  - `pio device list`
  - Set `upload_port = COMx` in `platformio.ini`

### Serial monitor shows garbage

- Confirm baud is `9600`.
- Confirm `monitor_speed = 9600` in `platformio.ini`.

## LED ring issues

### No LEDs light

- Confirm **GND is shared** between Pro Micro and LED ring power.
- Confirm the data pin matches `NeoPixelPin` in `src/main.cpp`.
- Confirm the ring is WS2812/NeoPixel (single-wire), not a clocked strip.

### Random flicker / unstable colors

- Add a bulk capacitor across LED ring power (e.g., 470–1000µF).
- Add a small series resistor on the data line (e.g., 220–470Ω).
- Keep the data wire short.

## Remote input issues

- Pins are configured as `INPUT_PULLUP`.
- Pressed should read LOW.
- If your remote outputs HIGH when pressed, either invert in hardware or change the logic in `pollRemotePinsForChanges()`.

## Onboard LEDs

- Pro Micro RX/TX LEDs may blink with USB traffic.
- The Pro Micro **PWR LED** is typically hardwired to VCC and cannot be disabled in firmware.
